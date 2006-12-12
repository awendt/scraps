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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "blowinit.h"
#include "blowfish.h"

#define MAXLINE 1000

/**
 * feistel(unsigned int input):
 *
 * SYNOPSIS 
 *  Feistel function of the Blowfish algorithm.
 *
 * DESCRIPTION
 *  Takes a 32-bit integer input, splits it into four 8-bit ints,
 *  and uses the quarters as input for the S-boxes.
 *
 * RETURN VALUE
 *  32-bit integer
 */
unsigned int feistel(unsigned int input) {
	unsigned char quad[4];

	// Split the 32-bit input into four 8-bit ints ...
	quad[0] = input >> 24;
	quad[1] = (input & 0xFF0000L) >> 16;
	quad[2] = (input & 0xFF00L) >> 8;
	quad[3] = input & 0xFFL;

	// ... let the S-boxes take care of them and logically combine the result
	return ((sbox0[quad[0]] + sbox1[quad[1]]) ^ sbox2[quad[2]]) + sbox3[quad[3]];
}

/**
 * encrypt_block(unsigned long *dl, unsigned long xl, unsigned long xr):
 *
 * SYNOPSIS 
 *  Encrypts two 32-bit integers and stores the result in the given array.
 *
 * DESCRIPTION
 *  Encrypts the left and right part of the 64-bit integer given using the 
 *  blowfish algorithm.
 *
 *  The first argument should be a pointer to an int[2].
 */
void encrypt_block(unsigned long *dl, unsigned long xl, unsigned long xr) {
	// temporary variable to swap two values
	unsigned long temp;
	int i;
	
	for (i=0; i<16; i++) {
		xl = xl ^ parray[i];
		xr = feistel(xl) ^ xr;
		
		// Swap values
		temp = xl;
		xl = xr;
		xr = temp;
	}
	
	// Revert last swap
	temp = xl;
	xl = xr;
	xr = temp;
	
	xr = xr ^ parray[16];
	xl = xl ^ parray[17];
	
	// Store the right and left value in the destination
	*dl = xl;
	*(dl+1) = xr;
}

/**
 * decrypt(unsigned long *dl, unsigned long xl, unsigned long xr):
 *
 * SYNOPSIS 
 *  Decrypts two 32-bit integers and stores the result in the given array.
 *
 * DESCRIPTION
 *  Decrypts the left and right part of the 64-bit integer given using the 
 *  blowfish algorithm.
 *
 *  The first argument should be a pointer to an int[].
 */
void decrypt(unsigned long *dl, unsigned long xl, unsigned long xr) {
	// temporary variable to swap two values
	unsigned long temp;
	int i;
	
	for (i=17; i>1; i--) {
		xl = xl ^ parray[i];
		xr = feistel(xl) ^ xr;

		// Swap values
		temp = xl;
		xl = xr;
		xr = temp;
	}

	// Revert last swap
	temp = xl;
	xl = xr;
	xr = temp;
	
	xr = xr ^ parray[1];
	xl = xl ^ parray[0];
	
	// Store the right and left value in the destination
	*dl = xl;
	*(dl+1) = xr;
}

/**
 * init(char *key, const unsigned int n):
 *
 * SYNOPSIS 
 *  Initializes S-boxes and P-arrays depending on the given key.
 *
 * DESCRIPTION
 *  Blowfish's key schedule starts by initializing P-array and S-boxes
 *  with the hexadecimal digits of pi. This has been done already (see init.h).
 *  The secret key is XOR'ed with the P-entries in order (cycling the key
 *  if necessary).
 *
 *  The entire P-array and all S-box entries are replaced by a continuous
 *  encryption of all previous ciphertexts, starting with a 64-bit all-zero
 *  block for P_1 and P_2.
 */
void init(char *key, const unsigned int n) {
	unsigned int k, l;
	unsigned int key_kmodn;
	
	for (k=0; k<18; k++) {
		key_kmodn = 0x0L;
		for (l=0; l<4; l++) {
			// The 32-bit integer is made from four bytes of the key.
			key_kmodn += (unsigned int) key[(4*k+l) % n] << (24-l*8);
		}
		// XOR the P-array with that key.
		parray[k] = parray[k] ^ key_kmodn;
	}
	
	// Encrypt a 64-bit all-zero block
	encrypt_block(parray, 0x0L, 0x0L);
	
	// Replace P-array and S-box values with new values
	for (k=2; k<17; k+=2) {
		encrypt_block(parray+k, parray[k-2], parray[k-1]);
	}
	encrypt_block(sbox0, parray[16], parray[17]);
	
	for (k=2; k<255; k+=2) {
		encrypt_block(sbox0+k, sbox0[k-2], sbox0[k-1]);
	}
	
	encrypt_block(sbox1, sbox0[254], sbox0[255]);
	
	for (k=2; k<255; k+=2) {
		encrypt_block(sbox1+k, sbox1[k-2], sbox1[k-1]);
	}
	
	encrypt_block(sbox2, sbox1[254], sbox1[255]);
	
	for (k=2; k<255; k+=2) {
		encrypt_block(sbox2+k, sbox2[k-2], sbox2[k-1]);
	}
	
	encrypt_block(sbox3, sbox2[254], sbox2[255]);
	
	for (k=2; k<255; k+=2) {
		encrypt_block(sbox3+k, sbox3[k-2], sbox3[k-1]);
	}
	
}

/**
 * encrypt_string(unsigned long **ciphertext, int *size, const char *plaintext):
 *
 * SYNOPSIS 
 *  Encrypts a given string and stores the ciphertext as 32-bit integers.
 *
 * DESCRIPTION
 *  The function first pads the given string so that its length is a multiple
 *  of 8 (Blowfish has a block size of 64 bits). Then the string is encrypted
 *  block by block.
 *
 *  The size of the ciphertext (as number of 32-bit blocks) is stored along 
 *  with the ciphertext itself for the caller.
 */
void encrypt_string(unsigned long **ciphertext, int *size, const char *plaintext) {
	// Plaintext string to be encrypted (after padding the given string)
	char *plain;
	int n, dst_size, k;
	
	n = strlen(plaintext);
	
	// Store the number of characters for padding
	int num_pad[] = { 0, 7, 6, 5, 4, 3, 2, 1 };
	
	// Destination size (with trailing '\0', hence the +1)
	dst_size = n + num_pad[n % 8];
	
	// Allocate memory for the ciphertext
	if ((*ciphertext = malloc((dst_size)*sizeof(unsigned long))) == NULL) {
		perror("malloc()");
		return;
	}
	
	// Allocate memory for the padded plaintext
	if ((plain = malloc(dst_size)) == NULL) {
		perror("malloc()");
		return;
	}
	
	// strncpy() fills the remaining bytes with '\0' -- which is what we want
	strncpy(plain, plaintext, dst_size);
	
	// Store the size of the ciphertext for the caller
	*size = dst_size / 4;
	
	// Encrypt each block of 64 bits
	for (k=0; k<dst_size/8 ; k++) {
		encrypt_block(*ciphertext+2*k, (plain[0] << 24) + (plain[1] << 16) + (plain[2] << 8) + plain[3], (plain[4] << 24) + (plain[5] << 16) + (plain[6] << 8) + plain[7]);
		// encrypt_block(*ciphertext+2*k, plain, plain+4);
		plain += 8;
	}
}

/**
 * decrypt_string(char **ciphertext, int *size, const char *plaintext):
 *
 * SYNOPSIS 
 *  Decrypts a given ciphertext and stores the plaintext as string.
 *
 * DESCRIPTION
 *  The function first pads the given string so that its length is a multiple
 *  of 8 (Blowfish has a block size of 64 bits). Then the string is encrypted
 *  block by block.
 *
 *  The size of the ciphertext (as number of 32-bits blocks) is stored along 
 *  with the ciphertext itself for the caller.
 */
void decrypt_string(char **plaintext, const unsigned long *ciphertext, int size) {
	if ((*plaintext = malloc(size*sizeof(unsigned long)+1)) == NULL) {
		perror("malloc()");
		return;
	}

	int n, k;
	char output[8];
	
	char *append_plain = *plaintext;
	
	// Decrypt each block of 64 bits
	for (k=0; k<size; k += 2 ) {
		decrypt(&output, *(ciphertext+k), *(ciphertext+k+1));
		for (n=3; n>=0; n--) {
			*append_plain = output[n];
			append_plain++;
		}
		for (n=7; n>3; n--) {
			*append_plain = output[n];
			append_plain++;
		}
	}
	*append_plain = '\0';
}
