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
 *  This is Bob, the server who is waiting for a connection from Alice.
 * 
 *  The communication protocol is as follows ("<" sending, ">" receiving):
 *
 *    > Prime p
 *    > Base g (constant of 5)
 *    > Alice's public key a
 *    < Bob's public key b
 *    > Message size
 *    > ciphertext
 *      
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sys/socket.h>

#include "helpers.h"
#include "dh.h"
#include "blowfish.h"

#define BUFSIZE 4096
#define DEFAULT_PORT 8080
#define kick_out_alice(__conn)	shutdown(__conn, SHUT_RDWR); close(__conn);

int main(int argc, char * const argv[]) {
	int option;						/* current option when parsing using getopt */
	int port = DEFAULT_PORT;		/* port to listen to for connections */
	struct sockaddr_in sin, pin;	/* socket address structure for socket and client */
	int addrlen;
	int sockfd, conn;				/* File descriptors for socket communication */
	int recvsize;
	int counter;					/* Mainly used for prettying hex display */
	
	/**
	 * Command-line option parsing
	 */
	while( (option = getopt(argc, argv, "p:")) != -1 )
	{
		switch(option)
		{
			case 'p':
				errno = 0;
				port = strtol(optarg, NULL, 0);
				if (errno != 0)
				{
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

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return 2;
	}

	srand(time(0));
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	/**
	 * Setup the socket
	 */
	if (bind(sockfd, (struct sockaddr*) &sin, sizeof(sin)) < 0)
	{
		perror("bind()");
		return 2;
	}

	/**
	 Listen to me!
	 */
	if (listen(sockfd, 10) < 0)
	{
		perror("listen()");
		return 2;
	}
	
	addrlen = sizeof(pin);
	
	unsigned long int *p, *g, *a;	/* prime, base, and Alice's public key */
	uint64_t b;						/* Our secret */
	uint64_t shared_key;			/* Shared key */
	uint64_t bob_public;			/* Our public key for Alice */
	unsigned long int *public_no;	/* Our public key in network byte order */
	unsigned long int *size;		/* Message size pointer */
	unsigned long int *ciphertext_no; /* Ciphertext received in network order */
	int bytes_recv;					/* Bytes to receive */
	unsigned long int *ciphertext;	/* Ciphertext received in host order */
	
	/**
	 * Infinite loop for socket communication
	 */
	while (1)
	{
		/**
		 * Stage 1: Accept a connection
		 */
		if ((conn = accept(sockfd, (struct sockaddr*) &pin, &addrlen)) < 0)
		{
			perror("accept()");
			return 2;
		}
		
		/**
		 * Stage 2: Receive p, g, and a from Alice
		 */
		p = malloc(8), g = malloc(8), a = malloc(8);
		if ((recvsize = recv(conn, p, 2*sizeof(long int), 0)) < 0) {
				perror("recv()");
				kick_out_alice(conn);
				return 2;
			}
		fprintf(stdout, "Received p=%#llx\n", ntoh64(p) );
		
		if ((recvsize = recv(conn, g, 2*sizeof(long int), 0)) < 0) {
				perror("recv()");
				kick_out_alice(conn);
				return 2;
			}
		fprintf(stdout, "Received g=%#llx\n", ntoh64(g) );
		
		if ((recvsize = recv(conn, a, 2*sizeof(long int), 0)) < 0) {
				perror("recv()");
				kick_out_alice(conn);
				return 2;
			}
		fprintf(stdout, "Received a=%#llx\n", ntoh64(a) );
		
		/**
		 * Stage 3: Select own secret, calculate shared key,
		 * and send our public key back to Alice
		 */
		b = select_secret();
		shared_key = get_shared_key(ntoh64(a), b, ntoh64(p));
		fprintf(stdout, "Got the secret key!\n");
		
		/**
		 * Initialize S-boxes and P-arrays for Blowfish encryption
		 */
		init((char*) &shared_key, sizeof(shared_key));
		
		bob_public = gen_public_key(ntoh64(g), b, ntoh64(p));
		
		free(p); free(g); free(a);
			
		if (hton64(&public_no, bob_public) != 0) {
			perror("hton64()");
			kick_out_alice(conn);
			return 3;
		}
		
		if (send_int64_t(conn, public_no)) {
			perror("send()");
			kick_out_alice(conn);
			return 3;
		}
		
		/**
		 * Alice can now send as much data as she wants
		 */
		while (1) {
		
			/**
			 * Stage 4: The key is now securely exchanged, 
			 * so we can receive a message size from Alice
			 */
			size = malloc(sizeof(unsigned long int));
			if ((recvsize = recv(conn, size, sizeof(unsigned long int), 0)) != sizeof(unsigned long int)) {
				perror("recv()");
				kick_out_alice(conn);
				break;
			}
			
			/**
			 * Alice doesn't want to send more data
			 */
			if (*size == 1) {
				fprintf(stdout, "\n-- Alice doesn't want to talk to us anymore! Kicking her out...\n");
				fflush(stdout);
				break;
			}
			
			bytes_recv = (*size) * sizeof(unsigned long int);
			
			ciphertext_no = malloc(bytes_recv);
	
			/**
			 * Stage 5: Message size has been received, memory allocated,
			 * so let's receive the encrypted message from Alice
			 */		
			if ((recvsize = recv(conn, ciphertext_no, bytes_recv, 0)) != bytes_recv) {
				perror("recv()");
				kick_out_alice(conn);
				break;
			}
			
			/**
			 * Stage 6: Put ciphertext in right order and decrypt it for display
			 */
			if (ntohany(&ciphertext, ciphertext_no, *size) != 0) {
				perror("ntohany()");
				kick_out_alice(conn);
				break;
			}
			
			char *plaintext;
			fprintf(stdout, "\n-- Message from Alice, decrypting...\n");
			decrypt_string(&plaintext, ciphertext, *size);
			fprintf(stdout, "Ciphertext was:\n ");
			counter = 0;
			while (*size > 0) {
				counter++;
				fprintf(stdout, " %#.8lx", *ciphertext);
				if (counter % 4 == 0)
					fprintf(stdout, "\n ");
				ciphertext++;
				(*size)--;
			}
			fprintf(stdout, "\nDecrypted:\n  %s", plaintext);
			fflush(stdout);
		}
		kick_out_alice(conn);
	}	

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	
	return 0;

}
