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

#define N_BYTES 8
#define send_int64_t(__sockfd, __ull_p) send(__sockfd, (char *) __ull_p, 2*sizeof(int), 0) != 2*sizeof(int)
#define recv_int64_t(__sockfd, __ull_p) recv(sockfd, __ull_p, 2*sizeof(long int), 0) != 2*sizeof(long int)

int hton64(unsigned long int **dst, const uint64_t src);
uint64_t ntoh64(unsigned long int *from);
int htonany(unsigned long int **dst, unsigned long int *src, unsigned int size);
int ntohany(unsigned long int **dst, unsigned long int *src, unsigned int size);
