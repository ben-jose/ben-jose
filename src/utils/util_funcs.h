

/*************************************************************

This file is part of ben-jose.

ben-jose is free software: you can redistribute it and/or modify
it under the terms of the version 3 of the GNU General Public 
License as published by the Free Software Foundation.

ben-jose is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ben-jose.  If not, see <http://www.gnu.org/licenses/>.

------------------------------------------------------------

Copyright (C) 2011, 2014-2015. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
email: joseluisquirogabeltran@gmail.com

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

util_funcs.h

util funcs.

--------------------------------------------------------------*/

#ifndef UTIL_FUNCS_H
#define UTIL_FUNCS_H

#include "ch_string.h"
#include "tak_mak.h"
#include "tools.h"
#include "sha2.h"

#define NUM_LONG_SHA2 (NUM_BYTES_SHA2/sizeof(long))
#define NUM_BYTES_MINSHA 5
#define NUM_BITS_MINSHA (NUM_BYTES_MINSHA * 8)

ch_string 	long_to_str(long val);
ch_string 	sha_to_minsha(row<uchar_t>& sha_rr);

void 		sha_bytes_of_arr(uchar_t* to_sha, long to_sha_sz, row<uchar_t>& the_sha);
ch_string 	sha_txt_of_arr(uchar_t* to_sha, long to_sha_sz);

#endif		// UTIL_FUNCS_H


