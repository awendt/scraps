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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "genprime.h"

#define min(a, b) (a < b ? a : b)

#define PRIME		1
#define NO_PRIME	0

/**
 * These first primes are just to quickly test if a number is NOT prime.
 */
int first_primes[N_PRIMES] = {
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61,
	67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137,
	139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 
	223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283,
	293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,
	383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
	463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563,
	569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643,
	647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739,
	743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829,
	839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937,
	941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021,
	1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097,
	1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193,
	1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283,
	1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
	1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459,
	1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549,
	1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619,
	1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721,
	1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
	1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907,
	1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999
};

/**
 * random_from_dev(const int len_bytes, FILE *dev_random):
 *
 * SYNOPSIS 
 *  Random number generator
 *
 * DESCRIPTION
 *  Get a random number of given length from the given file stream
 *  (which should be /dev/random).
 *
 * RETURN VALUE
 *  random 64-bit integer
 */
uint64_t random_from_dev(const int len_bytes, FILE *dev_random) {
	unsigned char *random = malloc(len_bytes);
	unsigned int bytes_read, byte;
	uint64_t add, result=0x0ULL;
	
	if ((bytes_read = fread(random, 1, len_bytes, dev_random)) != len_bytes)
	{
		perror("fread()");
		return 0;
	}

	random[len_bytes-1] |= 0x80;
	random[0] |= 0x01;
	for (byte=0; byte<len_bytes; byte++) {
		add = ((uint64_t) random[byte]) << (byte*8);
		result += add;
	}
	return result;
}

/**
 * primitive_test(uint64_t prime_candidate):
 *
 * SYNOPSIS 
 *  Primitively tests if a given candidate is prime.
 *
 * DESCRIPTION
 *  primitive_test() looks at the remainder when dividing by the first 
 *  303 prime numbers. Note that this is not sufficient to know if it's
 *  really prime!
 *
 * RETURN VALUE
 *  PRIME if candidate passes the test, otherwise NO_PRIME
 */
int primitive_test(uint64_t prime_candidate) {
	int index = 0;
	while (index < N_PRIMES) {
		if (prime_candidate % first_primes[index] == 0)
			return NO_PRIME;
		index++;
	}
	return PRIME;
}

/**
 * is_prime(uint64_t n, unsigned int k):
 *
 * SYNOPSIS 
 *  Tests if a given candidate is prime with a given number of rounds of
 *  the Miller-Rabin-Test.
 *
 * DESCRIPTION
 *  is_prime() first tests primitely, and if the number passes it,
 *  goes on with the Miller-Rabin-Test. We do this because dividing is faster
 *  than the test.
 *  Note that even here, we don't know for sure if the given number is prime,
 *  it's only a probability test.
 *
 * RETURN VALUE
 *  1 if candidate passes the test, otherwise 0
 */
int is_prime(uint64_t n, unsigned int k) {
	if (n == 2)
		return 1;
	if (n == 1 || (n & 1) == 0)
		return 0;
	if (primitive_test(n) == NO_PRIME)
		return 0;
	
	uint64_t t, a, y, d = n-1;
	while ((d & 1) == 0) {
		d >>= 1;
	}
	int count;
	for (count=0; count < k; count++) {
		a = rand()/100;
		t = d;
		y = modpow(a, t, n);
		while (t != n-1 && y != 1 && y != n-1) {
			y = (y*y) % n;
			t <<= 1;
		}
		if (y != n-1 && (t & 1) == 0)
			return 0;
	}
	return 1;
}

/**
 * modpow(uint64_t b, uint64_t e, uint64_t n):
 *
 * SYNOPSIS
 *  This quickly calculates a**e mod n.
 *
 * Taken from Bruce Schneier, Applied Cryptography, p. 245
 */
uint64_t modpow(uint64_t b, uint64_t e, uint64_t n) {
	uint64_t result = 1;
	while (e) {
		if (e & 1)
			result = (result * b) % n;
		e >>= 1;
		b = (b*b) % n;
	}
	return result;
}

/**
 * gen_prime(int len_bytes):
 *
 * SYNOPSIS
 *  Generates a random prime number of given length.
 *
 * DESCRIPTION
 *  Generates a random number of given length, tests if it passes the above
 *  Miller-Rabin-Test and returns the number if it does.
 *  If not, it generates a new number. The function will block until a prime
 *  is found
 *  Tests if a given candidate is prime with a given number of rounds of
 *  the Miller-Rabin-Test.
 *
 * RETURN VALUE
 *  64-bit prime integer 
 */
uint64_t gen_prime(int len_bytes) {
	uint64_t n;
	FILE *dev_random;
	if ((dev_random = fopen("/dev/random", "r")) == NULL) {
		perror("fopen()");
		return 1;
	}

	fprintf(stdout, "Generating prime");
	while (1) {
		fprintf(stdout, "."); fflush(stdout);
		n = random_from_dev(len_bytes, dev_random);
		if (is_prime(n, 5) == PRIME) {
			fclose(dev_random);
			return n;
		}
	}
}
