

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

util_funcs.cpp  

util funcs.

--------------------------------------------------------------*/

#include "bj_stream.h"
#include "util_funcs.h"
#include "tools.h"

ch_string
long_to_str(long val){
	bj_ostr_stream ss_val;
	ss_val << val;
	return ss_val.str();
}

ch_string
sha_to_minsha(row<uchar_t>& sha_rr){
	ch_string mnsh = "";
	
	const uchar_t* barr = sha_rr.get_c_array();
	
	long barr_sz = sha_rr.get_c_array_sz();
	MARK_USED(barr_sz);
	TOOLS_CK(barr_sz == NUM_BYTES_SHA2);
	
	TOOLS_CK(sha_rr.size() == NUM_BYTES_SHA2);
	const unsigned long* larr = (const unsigned long*)barr;
	
	tak_mak gg(larr, NUM_LONG_SHA2);

	s_bit_row bbrr;
	sha_rr.init_s_bit_row(bbrr);
	
	row<uchar_t> minsha_rr;
	minsha_rr.fill(0, NUM_BYTES_MINSHA);

	s_bit_row minbbrr;
	minsha_rr.init_s_bit_row(minbbrr);
	
	for(long aa = 0; aa < NUM_BITS_MINSHA; aa++){
		long pos = gg.gen_rand_int32_ie(0, bbrr.size());
		bool vv = bbrr[pos];
		minbbrr[aa] = vv;
	}
	
	mnsh = minsha_rr.as_hex_str();
	TOOLS_CK(mnsh.size() == (NUM_BYTES_MINSHA * 2));
	return mnsh;
}

void
sha_bytes_of_arr(uchar_t* to_sha, long to_sha_sz, row<uchar_t>& the_sha){
	the_sha.clear();
	the_sha.fill(0, NUM_BYTES_SHA2);
	uchar_t* sha_arr = (uchar_t*)(the_sha.get_c_array());

	uchar_t* ck_arr1 = to_sha;
	MARK_USED(ck_arr1);

	sha2(to_sha, to_sha_sz, sha_arr, 0);
	TOOLS_CK(ck_arr1 == to_sha);
	TOOLS_CK((uchar_t*)(the_sha.get_c_array()) == sha_arr);
}

ch_string
sha_txt_of_arr(uchar_t* to_sha, long to_sha_sz){
	row<uchar_t>	the_sha;
	sha_bytes_of_arr(to_sha, to_sha_sz, the_sha);
	ch_string sha_txt = the_sha.as_hex_str();
	return sha_txt;
}

