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

#include <stdlib.h>
#include <arpa/inet.h>
#include "genprime.h"
#include "helpers.h"

/**
 * hton64(unsigned long int **dst, const uint64_t src):
 *
 * SYNOPSIS
 *  Converts a 64-bit integer into network byte order.
 *
 * DESCRIPTION
 *  hton64() takes a 64-bit integer in host byte order and puts lower and upper 
 *  half in two 32-bit integers. The integers are first cleared -- that's why
 *  calloc() is used, not malloc(). Then both parts are seperately converted
 *  to network byte order.
 *
 * RETURN VALUE
 *  0 if successful, -1 if calloc() call fails.
 */
int hton64(unsigned long int **dst, const uint64_t src) {
	unsigned long int high, low;
	high = (src & 0xFFFFFFFF00000000ULL) >> N_BYTES*4;
	low = src & 0xFFFFFFFFULL;
	if ((*dst = calloc(N_BYTES, 1)) == NULL)
		return -1;
	
	if (low != 0x0)
		**dst = htonl(low);
	dst++;
	
	if (high != 0x0)
		**dst = htonl(high);
	dst--;
	
	return 0;
}

/**
 * ntoh64(unsigned long int *from):
 *
 * SYNOPSIS
 *  Converts a 64-bit integer in network byte order into host byte order.
 *
 * DESCRIPTION
 *  ntoh64() takes a 64-bit integer (given as a pointer to two 32-bit integers)
 *  in network byte order, converts each of them into host byte order using
 *  ntohl() and adds the result to a 64-bit integer which is returned.
 *
 * RETURN VALUE
 *  64-bit integer in host byte order
 */
uint64_t ntoh64(unsigned long int *from) {
	uint64_t ret = 0;
	ret += ntohl(*from);
	ret += ntohl( *(from+1) << N_BYTES*4 );
	return ret;
}

/**
 * htonany(unsigned long int **dst, unsigned long int *src, unsigned int size):
 *
 * SYNOPSIS
 *  Converts an arbitrary-length integer and converts each 32-bit part of it
 *  into network byte order using htonl().
 *
 * DESCRIPTION
 *  htonany() takes a pointer-pointer to a value of 32-bit-multiple length,
 *  allocates size bytes of memory to it and htonl()'s each 32-bit part of the
 *  given integer of arbitrary length to network byte order.
 *
 * RETURN VALUE
 *  0 if successful, -1 if calloc() call fails
 */
int htonany(unsigned long int **dst, unsigned long int *src, unsigned int size) {
	int index;

	if ((*dst = calloc(sizeof(unsigned long int), size)) == NULL)
		return -1;
	
	for (index=0; index < size; index++) {
		**dst = htonl(*(src+index));
		(*dst)++;
	}
	
	(*dst) -= size;
	return 0;
}

/**
 * ntohany(unsigned long int **dst, unsigned long int *src, unsigned int size):
 *
 * SYNOPSIS
 *  Converts an arbitrary-length integer in network byte order and converts each
 *  32-bit part of it into host byte order using ntohl().
 *
 * DESCRIPTION
 *  ntohany() takes a pointer-pointer to a value of 32-bit-multiple length,
 *  allocates size bytes of memory to it and ntohl()'s each 32-bit part of the
 *  given integer of arbitrary length to host byte order.
 *
 * RETURN VALUE
 *  0 if successful, -1 if calloc() call fails
 */
int ntohany(unsigned long int **dst, unsigned long int *src, unsigned int size) {
	int index;

	if ((*dst = calloc(sizeof(unsigned long int), size)) == NULL)
		return -1;
	
	for (index=0; index < size; index++) {
		**dst = ntohl(*(src+index));
		(*dst)++;
	}
	
	(*dst) -= size;
	return 0;
}
