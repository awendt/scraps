/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * Project file for course DVD017 at Blekinge Institute of Technology, fall 2006
 * AUTHOR: André Wendt
 * DATE: 2006-12-12
 */

/**
 *  This is Alice, the client who is setting up a connection to Bob.
 * 
 *  The communication protocol is as follows ("<" sending, ">" receiving):
 *
 *    < Prime p
 *    < Base g (constant of 5)
 *    < Alice's public key a
 *    > Bob's public key b
 *    < Message size
 *    < ciphertext
 *      
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sys/socket.h>

#include "genprime.h"
#include "dh.h"
#include "helpers.h"
#include "blowfish.h"

#define BUFSIZE			4096
#define LEN_BYTES		4
#define DEFAULT_PORT	8080
#define PROMPT			"\n> "
#define msg_cipher_not_sent()	fprintf(stdout, "Encrypted string %s not sent.\n", plaintext)
#define screw_bob(__conn)		shutdown(__conn, SHUT_RDWR); close(__conn);


int main(int argc, char * const argv[]) {
	char plaintext[BUFSIZE];
	int option;						/* current option when parsing using getopt */
	int port = DEFAULT_PORT;		/* port to connect to */
	struct sockaddr_in sin;			/* socket address structure for socket and client */
	int sockfd;						/* File descriptor for socket communication */
	int recvsize;
	char *host;
	int host_given = 0;
	int counter;					/* Mainly used for prettying hex display */
	
	/**
	 * Command-line option parsing
	 */
	while( (option = getopt(argc, argv, "h:p:")) != -1 )
	{
		switch(option)
		{
			case 'h':
				if ((host = strdup(optarg)) == NULL) {
					perror("strdup()");
					return 1;
				}
				host_given = 1;
			case 'p':
				errno = 0;
				port = strtol(optarg, NULL, 0);
				if (errno != 0) {
					perror("strtol()");
					return 1;
				}
				if (port == 0)
				{
					fprintf(stderr, "strtol() Type converting port number\n");
					return 1;
				}
				break;
			default:
				break;
		}
	}
	
	if (!host_given) {
		fprintf(stderr, "No hostname given, use -h option\n");
		return 1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return 2;
	}
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(host);
	sin.sin_port = htons(port);

	/**
	 * Stage 1: Establish connection to Bob
	 */
	if (connect(sockfd, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		perror("connect()");
		return 2;
	}
	
	srand(time(0));
	uint64_t p = gen_prime(LEN_BYTES);
	uint64_t g = 5ULL;
	uint64_t a = select_secret();
	uint64_t public = gen_public_key(g, a, p);
	
	unsigned long int *pp, *pg, *ppub;
	
	/**
	 * Stage 2: Send p, g, and a to Bob after converting them to network order
	 */	
	if ( (hton64(&pp, p) | hton64(&pg, g) | hton64(&ppub, public)) != 0) {
		perror("hton64()");
		return 3;
	}
	
	if (send_int64_t(sockfd, pp) || send_int64_t(sockfd, pg) || send_int64_t(sockfd, ppub)) {
		perror("send()");
		return 3;
	}
	
	/**
	 * Stage 3: Receive Bob's public key and calcute shared key
	 */	
	unsigned long int *b = malloc(8);
	
	if ((recvsize = recv(sockfd, b, 2*sizeof(long int), 0)) < 0) {
		perror("recv()");
		return 4;
	}
	
	uint64_t shared_key = get_shared_key(ntoh64(b), a, p);
	fprintf(stdout, "Got the secret key!\n");
	
	/**
	 * Initialize S-boxes and P-arrays for Blowfish encryption
	 */
	init((char*) &shared_key, sizeof(shared_key));
	
	fprintf(stdout, "\nEnter your plaintext, exit with ^D\n");
	
	unsigned long int *ciphertext, *ciphertext_no;
	unsigned long int bytes_rcvd;
	int *size = malloc(sizeof(unsigned long));
	
	fprintf(stdout, PROMPT);
	while (fgets(plaintext, BUFSIZE, stdin) != NULL) {
		
		if (plaintext[strlen(plaintext) - 1] == "\n")
			plaintext[strlen(plaintext) - 1] = 0; /* replace newline with null */
		
		encrypt_string(&ciphertext, size, plaintext);
		
		if (htonany(&ciphertext_no, ciphertext, *size) != 0) {
			perror("htonany()");
			msg_cipher_not_sent();
			screw_bob(sockfd);
			return 5;
		}
		
		bytes_rcvd = (*size) * sizeof(unsigned long int);
		
		/**
		 * Stages 4, 5: Send message size and ciphertext to Bob
		 */
		if (send(sockfd, (char *) size, sizeof(unsigned long), 0) != sizeof(unsigned long) || 
			send(sockfd, (char *) ciphertext_no, bytes_rcvd, 0) != bytes_rcvd) {
			
			perror("send()");
			msg_cipher_not_sent();
			screw_bob(sockfd);
			return 5;
		}
		
		/**
		 * Stage 6: Put ciphertext in right order and decrypt it for display
		 */
		unsigned long int *ciphertext;
		if (ntohany(&ciphertext, ciphertext_no, bytes_rcvd) != 0) {
			perror("ntohany()");
			screw_bob(sockfd);
			return 5;
		}
		
		fprintf(stdout, "+ Ciphertext sent was:\n "); 
		counter = 0;
		while (*size > 0) {
			counter++;
			fprintf(stdout, " %#.8lx", *ciphertext);
			if (counter % 4 == 0)
				fprintf(stdout, "\n ");
			ciphertext++;
			(*size)--;
		}
		
		fprintf(stdout, PROMPT);
		fflush(stdout);
	}
	
	fprintf(stdout, "\nTidy shutdown requested\n");
	
	*size = 1;
	if (send(sockfd, (char *) size, sizeof(unsigned long), 0) != sizeof(unsigned long)) {
		perror("send()");
		screw_bob(sockfd);
		return 6;
	}

	screw_bob(sockfd);
	
	return 0;

}
