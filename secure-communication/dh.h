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

void alice(uint64_t p);
uint64_t bob(uint64_t p, uint64_t g, uint64_t alice_public);
uint64_t select_secret();
uint64_t get_shared_key(uint64_t partner_public, uint64_t own_secret, uint64_t prime);
uint64_t gen_public_key(uint64_t g, uint64_t secret, uint64_t prime);
