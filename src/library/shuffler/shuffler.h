

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

shuffler.h

Declaration of functions to shuffler dimacs representations.

--------------------------------------------------------------*/

#ifndef SHUFFLER_H
#define SHUFFLER_H

#include "tools.h"
#include "tak_mak.h"

#define SHUFFLER_DBG(prm)	DBG(prm)
#define SHUFFLER_CK(prm) 	DBG_CK(prm)
#define SHUFFLER_H_CK(prm) 	DBG_CK(prm)

#define FIRST_LIT_IDX	1

//=================================================================
// map funcs

void		shuffle_lit_mapping(tak_mak& rnd_gen, row<integer>& to_shuff);
void		init_lit_mapping(tak_mak& rnd_gen, row<integer>& the_map, long num_var);
integer		map_literal(row<integer>& the_map, integer lit);
integer		shift_literal(long in_lit, long the_shift);
void		map_cnf_lits(row<integer>& the_map, row<long>& in_ccls, row<long>& out_ccls);
void		shuffle_cnf_lits(tak_mak& rnd_gen, long num_var, row<integer>& the_map,
				row<long>& in_ccls, row<long>& out_ccls);

void		shuffle_ccl_mapping(tak_mak& rnd_gen, row<integer>& to_shuff);
void		init_ccl_mapping(tak_mak& rnd_gen, row<integer>& the_map, long num_neu);
void		rl_to_rrl(row<long>& in_lits, row_row_long_t& out_ccls);
void		rrl_to_rl(row_row_long_t& in_ccls, row<long>& out_lits);
void		shuffle_cnf_ccls(tak_mak& rnd_gen, row<integer>& the_map,
				row<long>& in_lits, row<long>& out_lits);

void		shuffle_full_cnf(tak_mak& rnd_gen, long num_var, row<integer>& the_map,
				row<long>& in_ccls, row<long>& out_ccls);

void		shift_cnf_lits(long the_shift, row<long>& in_out_lits);
void		join_cnf_lits(row<long>& in_out_lits1, long num_var1, row<long>& in_out_lits2);


#endif		// SHUFFLER_H


