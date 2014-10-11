
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

Copyright (C) 2011, 2014. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
email: joseluisquirogabeltran@gmail.com

------------------------------------------------------------

test_rbt.cpp  

file for test and debugging rb_tree class.

--------------------------------------------------------------*/

#include <set>

#include "mem_redblack.h"
#include "tak_mak.h"


#define THE_TREE_CMP_FUNC cmp_mem_redblack

typedef mem_redblack<long>	rbt_long_t;
typedef mem_redblack<long>::rbt_nod_ref_t rbt_long_nref_t;
rbt_long_t decl02(cmp_long);

typedef std::set<long>	set_long_t;
typedef std::pair<std::set<long>::iterator,bool> ret_set_long_t;

#define NUM_TESTS 30
#define MAX_SZ_RBT	40000000

#define TEST_USE_SETS 1

#ifdef TEST_USE_SETS
#define SETS_COD(prm)	prm
#define RBTS_COD(prm)	/**/
#else
#define SETS_COD(prm)	/**/
#define RBTS_COD(prm)	prm
#endif


void	test_redblack(){
	bj_ostream& os = bj_out;
	RBTS_COD(rbt_long_t rr(cmp_long));
	SETS_COD(set_long_t ss);
	row<long> aux_rr;
	ret_set_long_t rsl;

	os << "test_redblack" << bj_eol;
	RBTS_COD(os << "using_rbts" << bj_eol);
	SETS_COD(os << "USING_SETS" << bj_eol);
	os << "MAX_SZ_RBT=" << MAX_SZ_RBT << bj_eol;

	average		avg_sz;

	long rr_tm = 20;
	tak_mak rnd_gen(rr_tm);
	
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;
	
	double start_tm = run_time();

	for(long jj = 0; jj < num_tests; jj++){
		
		long sz_rbt = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_RBT);
		if((min_sz == -1) || (sz_rbt < min_sz)){
			min_sz = sz_rbt;
		}
		if((max_sz == -1) || (sz_rbt > max_sz)){
			max_sz = sz_rbt;
		}

		REDBLACK_CK(aux_rr.size() == rr.size());
		
		long max_item = rnd_gen.gen_rand_int32_ie(sz_rbt, 3 * sz_rbt);

		// fill rbt.
		for(long kk = 0; kk < sz_rbt; kk++){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);

			REDBLACK_CK(rr.check_tree());
			bool ok = false;
			RBTS_COD(ok = rr.push(nxt_elem));
			SETS_COD(
				rsl = ss.insert(nxt_elem);
				ok = rsl.second;
			)
			if(ok){
				aux_rr.push(nxt_elem);
			}
			if((kk % 1000) == 0){
				os << CARRIAGE_RETURN;
				RBTS_COD(os << rr.size());
				SETS_COD(os << ss.size());
			}
			REDBLACK_CK(aux_rr.size() == rr.size());
		}
		
		os << CARRIAGE_RETURN;
		os << as_pt_char("                     ");
		os << CARRIAGE_RETURN;
		RBTS_COD(os << rr.size());
		SETS_COD(os << ss.size());
		os.flush();
		
		REDBLACK_CK(rr.check_tree());

		RBTS_COD(avg_sz.add_val((double)rr.size()));
		SETS_COD(avg_sz.add_val((double)ss.size()));

		// play with rbt.
		long num_plays = rnd_gen.gen_rand_int32_ie(1, 2 * sz_rbt);
		for(long pp = 0; pp < num_plays; pp++){
			long play_kind = rnd_gen.gen_rand_int32_ie(0, 2);
			REDBLACK_CK(rr.check_tree());
			REDBLACK_CK(aux_rr.size() == rr.size());
			
			bool is_ee = false;
			RBTS_COD(is_ee = rr.is_empty());
			SETS_COD(is_ee = ss.empty());
			if((play_kind == 0) || is_ee){
				long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
				bool ok = false;
				RBTS_COD(ok = rr.push(nxt_elem));
				SETS_COD(
					rsl = ss.insert(nxt_elem);
					ok = rsl.second;
				)
				if(ok){
					aux_rr.push(nxt_elem);
				}
			} else {
				REDBLACK_CK(play_kind == 1);
				long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, (aux_rr.size() - 1));
				long nxt_elem = aux_rr[nxt_elem_idx];
				bool ok = false;
				MARK_USED(ok);
				RBTS_COD(ok = rr.pop(nxt_elem));
				SETS_COD(
					int nme = ss.erase(nxt_elem);
					ok = (nme == 1);
				);
				REDBLACK_CK(ok);
				
				long old_elem = aux_rr.swap_pop(nxt_elem_idx);
				MARK_USED(old_elem);
				REDBLACK_CK(old_elem == nxt_elem);
			}
		}
		REDBLACK_CK(rr.check_tree());
		REDBLACK_CK(rr.check_order());

		// remove one by one
		while(! aux_rr.is_empty()){
			long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, (aux_rr.size() - 1));
			long nxt_elem = aux_rr[nxt_elem_idx];
			
			REDBLACK_CK(rr.check_tree());
			
			bool ok = true;
			MARK_USED(ok);
			RBTS_COD(ok = rr.pop(nxt_elem));
			SETS_COD(
				int nme = ss.erase(nxt_elem);
				ok = (nme == 1);
			);
			REDBLACK_CK(ok);
			
			long old_elem = aux_rr.swap_pop(nxt_elem_idx);
			MARK_USED(old_elem);
			REDBLACK_CK(old_elem == nxt_elem);
		}
		REDBLACK_CK(rr.check_tree());
		os << CARRIAGE_RETURN;
		os << jj;

		REDBLACK_CK(rr.is_empty());
		REDBLACK_CK(aux_rr.is_empty());
		
	}

	double fill_tm = run_time() - start_tm;
	
	os << bj_eol;
	os << "min_sz=" << min_sz << " max_sz=" << max_sz <<
		" avg_sz=" << avg_sz.avg << bj_eol;

	os << "TIME=" << fill_tm << bj_eol;
	
}


int	
main(int argc, char** argv){
	bj_ostream& os = bj_out;

	test_redblack();

	os << bj_eol;

	os << "End of tests. Type ENTER." << bj_eol;
	return 0;
}


