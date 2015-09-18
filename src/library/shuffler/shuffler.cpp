

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

shuffler.cpp  

Functions to shuffle dimacs representations.

--------------------------------------------------------------*/

#include "shuffler.h"

void
shuffle_lit_mapping(tak_mak& rnd_gen, row<integer>& to_shuff){
	row<integer> tmp_bag;

	long sz_0 = to_shuff.size();
	MARK_USED(sz_0);

	row_index from = FIRST_LIT_IDX;
	to_shuff.copy_to(tmp_bag, from);
	to_shuff.clear(true, false, from);

	while(tmp_bag.size() > 0){
		long idx_pop = rnd_gen.gen_rand_int32_ie(0, tmp_bag.size());

		SHUFFLER_CK(idx_pop >= 0);
		SHUFFLER_CK(idx_pop < tmp_bag.size());
		integer var1 = tmp_bag.swap_pop(idx_pop);
		SHUFFLER_CK(var1 > 0);

		long pol = rnd_gen.gen_rand_int32_ie(0, 2);
		if(pol == 0){ var1 = -var1; }

		to_shuff.push(var1);
	}

	SHUFFLER_CK(to_shuff.size() == sz_0);
}

void
init_lit_mapping(tak_mak& rnd_gen, row<integer>& the_map, long num_var){
	//long rnd_base = gen_random_num(0, 99999);
	//tak_mak rnd_gen(rnd_base);

	the_map.set_cap(num_var + 1);
	the_map.clear();
	for(long kk = 0; kk < num_var + 1; kk++){
		the_map.push(kk);
	}

	shuffle_lit_mapping(rnd_gen, the_map);
}

integer
map_literal(row<integer>& the_map, integer lit){
	integer var = get_var(lit);
	SHUFFLER_H_CK(var > 0);
	SHUFFLER_H_CK(var < the_map.size());
	integer map_var = the_map[var];
	if(lit < 0){
		return -map_var;
	}
	return map_var;
}

void
map_cnf_lits(row<integer>& the_map, row<long>& in_lits, row<long>& out_lits)
{
	out_lits.clear();
	for(long ii = 0; ii < in_lits.size(); ii++){
		long in_lit = in_lits[ii];
		long out_lit = 0;
		if(in_lit != 0){
			out_lit = map_literal(the_map, in_lit);
		}
		out_lits.push(out_lit);
	}
}

void
shuffle_cnf_lits(tak_mak& rnd_gen, long num_var, row<integer>& the_map,
		row<long>& in_lits, row<long>& out_lits)
{
	init_lit_mapping(rnd_gen, the_map, num_var);

	map_cnf_lits(the_map, in_lits, out_lits);
}

// join ccls

long
shift_literal(long in_lit, long the_shift){
	integer var = get_var(in_lit);
	SHUFFLER_CK(var > 0);
	SHUFFLER_CK(the_shift > 0);
	var += the_shift;
	if(in_lit < 0){
		return -var;
	}
	return var;
}

void
shift_cnf_lits(long the_shift, row<long>& in_out_lits)
{
	for(long ii = 0; ii < in_out_lits.size(); ii++){
		long in_lit = in_out_lits[ii];
		long out_lit = 0;
		if(in_lit != 0){
			out_lit = shift_literal(in_lit, the_shift);
		}
		in_out_lits[ii] = out_lit;
	}

}

void
join_cnf_lits(row<long>& in_out_lits1, long num_var1, row<long>& in_out_lits2)
{
	shift_cnf_lits(num_var1, in_out_lits2);
	in_out_lits2.append_to(in_out_lits1);
}

// shuffle ccls

void
shuffle_ccl_mapping(tak_mak& rnd_gen, row<integer>& to_shuff){
	row<integer> tmp_bag;

	long sz_0 = to_shuff.size();
	MARK_USED(sz_0);

	to_shuff.copy_to(tmp_bag);
	to_shuff.clear();

	while(tmp_bag.size() > 0){
		long idx_pop = rnd_gen.gen_rand_int32_ie(0, tmp_bag.size());

		SHUFFLER_CK(idx_pop >= 0);
		SHUFFLER_CK(idx_pop < tmp_bag.size());
		integer idx1 = tmp_bag.swap_pop(idx_pop);
		SHUFFLER_CK(idx1 >= 0);

		to_shuff.push(idx1);
	}

	SHUFFLER_CK(to_shuff.size() == sz_0);
}

void
init_ccl_mapping(tak_mak& rnd_gen, row<integer>& the_map, long num_ccl){
	the_map.set_cap(num_ccl);
	the_map.clear();
	for(long kk = 0; kk < num_ccl; kk++){
		the_map.push(kk);
	}

	shuffle_ccl_mapping(rnd_gen, the_map);
}

void
rl_to_rrl(row<long>& in_lits, row_row_long_t& out_ccls)
{
	row_long_t tmp_rr;
	out_ccls.clear();
	tmp_rr.clear();
	for(long ii = 0; ii < in_lits.size(); ii++){
		long in_lit = in_lits[ii];
		if(in_lit != 0){
			tmp_rr.push(in_lit);
		} else {
			row_long_t& rr = out_ccls.inc_sz();
			tmp_rr.move_to(rr);
			SHUFFLER_CK(tmp_rr.is_empty());
		}
	}
}

void
rrl_to_rl(row_row_long_t& in_ccls, row<long>& out_lits)
{
	out_lits.clear();
	for(long ii = 0; ii < in_ccls.size(); ii++){
		row_long_t& rr = in_ccls[ii];
		for(long kk = 0; kk < rr.size(); kk++){
			long in_lit = rr[kk];
			out_lits.push(in_lit);
		}
		out_lits.push(0);
	}
}

void
shuffle_cnf_ccls(tak_mak& rnd_gen, row<integer>& the_map,
		row<long>& in_lits, row<long>& out_lits)
{
	row_row_long_t in_ccls;
	rl_to_rrl(in_lits, in_ccls);

	long num_ccls = in_ccls.size();
	init_ccl_mapping(rnd_gen, the_map, num_ccls);

	SHUFFLER_CK(the_map.size() == num_ccls);

	row_row_long_t out_ccls;
	out_ccls.fill_new(num_ccls);

	for(long ii = 0; ii < in_ccls.size(); ii++){
		row_long_t& rr1 = in_ccls[ii];
		long n_idx = the_map[ii];

		SHUFFLER_CK(out_ccls.is_valid_idx(n_idx));
		row_long_t& rr2 = out_ccls[n_idx];

		SHUFFLER_CK(! rr1.is_empty());
		SHUFFLER_CK(rr2.is_empty());

		rr1.move_to(rr2);
	}

	rrl_to_rl(out_ccls, out_lits);
}

void		shuffle_full_cnf(tak_mak& rnd_gen, long num_var, row<integer>& the_map,
				row<long>& in_ccls, row<long>& out_ccls)
{
	row<long> tmp_ccls;
	shuffle_cnf_lits(rnd_gen, num_var, the_map, in_ccls, tmp_ccls);
	shuffle_cnf_ccls(rnd_gen, the_map, tmp_ccls, out_ccls);
}



