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
#include <stdio.h>
#include <time.h>
#include "genprime.h"
#include "dh.h"

/**
 * select_secret():
 *
 * SYNOPSIS
 *  Returns a random, 64-bit number
 *
 * RETURN VALUE
 *  64-bit integer
 */
uint64_t select_secret() {
	return rand() | 0x80000000;
}

/**
 * gen_public_key(uint64_t g, uint64_t secret, uint64_t prime):
 *
 * SYNOPSIS
 *  Generates the public key
 *
 * RETURN VALUE
 *  64-bit integer
 */
uint64_t gen_public_key(uint64_t g, uint64_t secret, uint64_t prime) {
	return modpow(g, secret, prime);
}

/**
 * get_shared_key(uint64_t partner_public, uint64_t own_secret, uint64_t prime):
 *
 * SYNOPSIS
 *  Gets the shared key for our the given public key of our partner, 
 *  our own secret, and the prime
 *
 * RETURN VALUE
 *  64-bit integer
 */
uint64_t get_shared_key(uint64_t partner_public, uint64_t own_secret, uint64_t prime) {
	return modpow(partner_public, own_secret, prime);
}
