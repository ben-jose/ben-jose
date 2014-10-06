
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

#include "mem_redblack.h"
//include "row_redblack.h"
#include "tak_mak.h"

#define THE_TREE_CMP_FUNC cmp_mem_redblack

//typedef row_redblack<long>	rbt_long_t;
//rbt_long_t decl01(cmp_long);

typedef mem_redblack<long>	rbt_long_t;
rbt_long_t decl02(cmp_long);

void	prt_rbt_data(char* nam, rbt_long_t& rr){
	bj_ostream& os = bj_out;

	os << nam;
	os << " SIZE=" << rr.size();
	os << " min_ref=" << rr.get_min();
	if(rr.get_min() != rr.get_null()){
		os << " min_val=" << rr.get_obj(rr.get_min());
	}
	os << " max_ref=" << rr.get_max();
	if(rr.get_max() != rr.get_null()){
		os << " max_val=" << rr.get_obj(rr.get_max());
	}
	os << bj_eol;
	//rr.print_tree(os);
	os << rr << bj_eol;
	//os << "---------------";
	//REDBLACK_CK(rr.check_tree());
	os << "--------------------------------------" << bj_eol;
}

/*
void	prt_rbt_cmp(char* title, rbt_long_t& r1, rbt_long_t& r2){
	bj_ostream& os = bj_out;
	os << title << bj_eol;

	//os << "r1=" << r1 << " sz=" << r1.size() << bj_eol;
	//os << "r2=" << r2 << " sz=" << r2.size() << bj_eol;

	os << "--------------------------------------" << bj_eol;
	prt_rbt_data(as_pt_char("r1="), r1);
	prt_rbt_data(as_pt_char("r2="), r2);

	os << "cmp(r1, r2)=";

	bool are_eq = false;
	cmp_is_sub cmp01 = THE_TREE_CMP_FUNC(r1, r2, are_eq);

	os << subset_cmp_str(cmp01) << bj_eol; 
	os << "are_eq=" << are_eq << bj_eol;
	os << "=======================================" << bj_eol;
}
*/

#define NUM_TESTS 30
#define MAX_SZ_RBT	4000

void	test_redblack(){
	bj_ostream& os = bj_out;
	rbt_long_t rr(cmp_long);
	row<long> aux_rr;

	os << "test_redblack" << bj_eol;

	average		avg_sz;

	//long rr_tm = (long)run_time();
	long rr_tm = 20;
	tak_mak rnd_gen(rr_tm);
	//init_random_nums();
	
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;

	for(long jj = 0; jj < num_tests; jj++){
		long sz_rbt = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_RBT);
		//long sz_rbt = gen_random_num(0, MAX_SZ_RBT);
		if((min_sz == -1) || (sz_rbt < min_sz)){
			min_sz = sz_rbt;
		}
		if((max_sz == -1) || (sz_rbt > max_sz)){
			max_sz = sz_rbt;
		}

		REDBLACK_CK(aux_rr.size() == rr.size());
		//os << bj_eol;
		//os << as_pt_char("FASE 0. jj=") << jj << bj_eol;
		//getchar();
		
		long max_item = rnd_gen.gen_rand_int32_ie(sz_rbt, 3 * sz_rbt);
		//long max_item = gen_random_num(sz_rbt, 3 * sz_rbt);

		//os << as_pt_char("sz_rbt=") << sz_rbt << bj_eol;
		// fill the rbt.
		for(long kk = 0; kk < sz_rbt; kk++){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
			//long nxt_elem = gen_random_num(0, max_item);
			//os << bj_eol;
			//rr.print(bj_out);

			REDBLACK_CK(rr.check_tree());
			//rr << nxt_elem;
			bool ok = rr.push(nxt_elem);
			if(ok){
				aux_rr.push(nxt_elem);
			}
			os << CARRIAGE_RETURN;
			os << rr.size();
			REDBLACK_CK(aux_rr.size() == rr.size());
		}
		//os << bj_eol;
		//os << as_pt_char("FASE 1") << bj_eol;
		//getchar();
		REDBLACK_CK(rr.check_tree());

		avg_sz.add_val((double)rr.size());

		// play with rbt.
		long num_plays = rnd_gen.gen_rand_int32_ie(1, 2 * sz_rbt);
		//long num_plays = gen_random_num(1, 2 * sz_rbt);
		//long num_plays = gen_random_num(1, 2 * NUM_TESTS);
		for(long pp = 0; pp < num_plays; pp++){
			long play_kind = rnd_gen.gen_rand_int32_ie(0, 2);
			//long play_kind = gen_random_num(0, 2);
			REDBLACK_CK(rr.check_tree());
			REDBLACK_CK(aux_rr.size() == rr.size());
			if((play_kind == 0) || rr.is_empty()){
				long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
				//long nxt_elem = gen_random_num(0, max_item);
				//rr << nxt_elem;
				bool ok =rr.push(nxt_elem);
				if(ok){
					aux_rr.push(nxt_elem);
				}
			} else {
				REDBLACK_CK(play_kind == 1);
				long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, (aux_rr.size() - 1));
				//long nxt_elem_idx = gen_random_num(0, (rr.size() - 1));
				//long nxt_elem = rr.get_obj(nxt_elem_idx);
				//prt_rbt_data("rr", rr);
				//rr >> nxt_elem;
				
				long nxt_elem = aux_rr[nxt_elem_idx];
				bool ok = rr.pop(nxt_elem);
				REDBLACK_CK(ok);
				
				long old_elem = aux_rr.swap_pop(nxt_elem_idx);
				REDBLACK_CK(old_elem == nxt_elem);
			}
		}
		REDBLACK_CK(rr.check_tree());
		//os << bj_eol;
		//os << as_pt_char("FASE 2") << bj_eol;
		//getchar();

		// remove one by one
		while(! rr.is_empty()){
			long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, (rr.size() - 1));
			//long nxt_elem_idx = gen_random_num(0, (rr.size() - 1));
			//long nxt_elem = rr.get_obj(nxt_elem_idx);
			//prt_rbt_data("rr", rr);
			REDBLACK_CK(rr.check_tree());
			//rr >> nxt_elem;

			long nxt_elem = aux_rr[nxt_elem_idx];
			bool ok = rr.pop(nxt_elem);
			REDBLACK_CK(ok);
			
			long old_elem = aux_rr.swap_pop(nxt_elem_idx);
			REDBLACK_CK(old_elem == nxt_elem);
			//os << CARRIAGE_RETURN;
			//os << rr.size();
		}
		REDBLACK_CK(rr.check_tree());
		os << CARRIAGE_RETURN;
		os << jj;

		//os << bj_eol;
		//os << as_pt_char("FASE 3") << bj_eol;
		//getchar();
		
		REDBLACK_CK(rr.is_empty());
		REDBLACK_CK(aux_rr.is_empty());
	}

	os << bj_eol;
	os << "min_sz=" << min_sz << " max_sz=" << max_sz <<
		" avg_sz=" << avg_sz.avg << bj_eol;
}

/*
void	test_subsets(){
	bj_ostream& os = bj_out;
	rbt_long_t r1(cmp_long);
	rbt_long_t r2(cmp_long);
	rbt_long_t r2plus(cmp_long);

	os << "test_subsets" << bj_eol;

	average		avg_sz;
	cmp_is_sub  cmp_r1_r2;
	bool		are_eq = false;

	long rr_tm = 20;
	tak_mak rnd_gen(rr_tm);
	//init_random_nums();
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;

	long RBT_NUL = r1.get_null();
	REDBLACK_CK(r2.get_null() == RBT_NUL);

	for(long jj = 0; jj < num_tests; jj++){
		long sz_rbt = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_RBT);
		//long sz_rbt = gen_random_num(0, MAX_SZ_RBT);
		r1.clear_redblack();
		r2.clear_redblack();
		r2plus.clear_redblack();

		long max_item = rnd_gen.gen_rand_int32_ie((sz_rbt + 5), (2 * (sz_rbt + 5)));
		//long max_item = gen_random_num((sz_rbt + 5), (2 * (sz_rbt + 5)));

		// fill the rbts.
		for(long kk = 0; kk < sz_rbt; kk++){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
			//long nxt_elem = gen_random_num(0, max_item);
			//r1 << nxt_elem;
			//r2 << nxt_elem;
			r1.push(nxt_elem);
			r2.push(nxt_elem);
		}

		long ini_sz = r1.size();
		if((min_sz == -1) || (ini_sz < min_sz)){
			min_sz = ini_sz;
		}
		if((max_sz == -1) || (ini_sz > max_sz)){
			max_sz = ini_sz;
		}

		REDBLACK_CK(r2.size() == ini_sz);
		avg_sz.add_val((double)r1.size());

		//os << "r1=" << r1 << bj_eol;
		//os << "r2=" << r2 << bj_eol;
		are_eq = false;
		cmp_r1_r2 = THE_TREE_CMP_FUNC(r1, r2, are_eq);
		REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
		REDBLACK_CK(are_eq);

		long mm = 0;

		long diff_val1 = -1;
		while(diff_val1 == -1){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
			//long nxt_elem = gen_random_num(0, max_item);
			if(r1.search(nxt_elem) == RBT_NUL){
				diff_val1 = nxt_elem;
				//r1 << nxt_elem;
				r1.push(nxt_elem);
			}
		}

		long diff_val2 = -1;
		while(diff_val2 == -1){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
			//long nxt_elem = gen_random_num(0, max_item);
			if(r1.search(nxt_elem) == RBT_NUL){
				REDBLACK_CK(r2.search(nxt_elem) == RBT_NUL);
				diff_val2 = nxt_elem;
				//r2 << nxt_elem;
				r2.push(nxt_elem);
			}
		}

		//os << "DIFF_VAL1=" << diff_val1 << bj_eol;
		//os << "DIFF_VAL2=" << diff_val2 << bj_eol;
		//os << "r1=" << r1 << bj_eol;
		//os << "r2=" << r2 << bj_eol;

		REDBLACK_CK(r1.search(diff_val1) != RBT_NUL);
		REDBLACK_CK(r2.search(diff_val1) == RBT_NUL);
		REDBLACK_CK(r1.search(diff_val2) == RBT_NUL);
		REDBLACK_CK(r2.search(diff_val2) != RBT_NUL);

		REDBLACK_CK(r1.size() == (ini_sz + 1));
		REDBLACK_CK(r2.size() == (ini_sz + 1));
		REDBLACK_CK(r1.get_obj(ini_sz) == diff_val1);
		REDBLACK_CK(r2.get_obj(ini_sz) == diff_val2);

		
		//long plus_sz1 = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_RBT);
		//long plus_sz1 = gen_random_num(0, MAX_SZ_RBT);
		//for(mm = 0; mm < plus_sz1; mm++){
		//	long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
		//	long nxt_elem = gen_random_num(0, max_item);
		//	if(nxt_elem != diff_val2){
		//		r1 << nxt_elem;
		//	}
		//}

		long plus_sz2 = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_RBT);
		//long plus_sz2 = gen_random_num(0, MAX_SZ_RBT);
		for(mm = 0; mm < plus_sz2; mm++){
			long nxt_elem = rnd_gen.gen_rand_int32_ie(0, max_item);
			//long nxt_elem = gen_random_num(0, max_item);
			if(nxt_elem != diff_val1){
				if(r2.search(nxt_elem) == RBT_NUL){
					//r2plus << nxt_elem;
					r2plus.push(nxt_elem);
				}
				//r2 << nxt_elem;
				r2.push(nxt_elem);
			}
		}

		//os << CARRIAGE_RETURN << jj;
		while(! r2plus.is_empty()){
			are_eq = false;
			cmp_r1_r2 = THE_TREE_CMP_FUNC(r2, r1, are_eq);
			if(cmp_r1_r2 != k_no_is_sub){
				os << "TEST_FAILED" << bj_eol;
				os << "r1=" << r1 << bj_eol;
				os << "r2=" << r2 << bj_eol;
			}
			REDBLACK_CK(cmp_r1_r2 == k_no_is_sub);
			REDBLACK_CK(! are_eq);

			long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, r2plus.size());
			//long nxt_elem_idx = gen_random_num(0, r2plus.size());
			long nxt_elem = r2plus.get_obj(nxt_elem_idx);
			//r2plus >> nxt_elem;
			//r2 >> nxt_elem;
			r2plus.pop(nxt_elem);
			r2.pop(nxt_elem);
		}

		REDBLACK_CK(r2.size() == (ini_sz + 1));
		//r2 >> diff_val2;
		r2.pop(diff_val2);

		//os << "r1=" << r1 << bj_eol;
		//os << "r2=" << r2 << bj_eol;
		are_eq = false;
		cmp_r1_r2 = THE_TREE_CMP_FUNC(r2, r1, are_eq);
		if(cmp_r1_r2 != k_lft_is_sub){
			os << "TEST_FAILED" << bj_eol;
			os << "r1=" << r1 << bj_eol;
			os << "r2=" << r2 << bj_eol;
		}
		REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
		REDBLACK_CK(! are_eq);

		// remove one by one
		while(! r2.is_empty()){
			long nxt_elem_idx = rnd_gen.gen_rand_int32_ie(0, r2.size());
			//long nxt_elem_idx = gen_random_num(0, r2.size());
			long nxt_elem = r2.get_obj(nxt_elem_idx);
			//r2 >> nxt_elem;
			r2.pop(nxt_elem);

			are_eq = false;
			cmp_r1_r2 = THE_TREE_CMP_FUNC(r2, r1, are_eq);
			if(cmp_r1_r2 != k_lft_is_sub){
				os << "TEST_FAILED" << bj_eol;
				os << "r1=" << r1 << bj_eol;
				os << "r2=" << r2 << bj_eol;
			}
			REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
			REDBLACK_CK(! are_eq);
		}

		//os << "==========================" << bj_eol;
		os << CARRIAGE_RETURN << jj;
	}

	os << bj_eol;
	os << "min_sz=" << min_sz << " max_sz=" << max_sz <<
		" avg_sz=" << avg_sz.avg << bj_eol;
}
*/

int	
main(int argc, char** argv){
	bj_ostream& os = bj_out;

	test_redblack();
	//test_subsets();

	/*
	rbt_long_t r1(cmp_long);
	rbt_long_t r2(cmp_long);

	r1 << 9;
	prt_rbt_data("r1=", r1);

	r1 >> 9;
	prt_rbt_data("r1=", r1);

	r1 << 6 << 7;
	prt_rbt_data("r1=", r1);

	r1 >> 7;
	prt_rbt_data("r1=", r1);

	r1 >> 6;
	prt_rbt_data("r1=", r1);

	r1 << 7 << 6;
	prt_rbt_data("r1=", r1);

	r1 >> 7;
	prt_rbt_data("r1=", r1);

	r1 >> 6;
	prt_rbt_data("r1=", r1);

	r1 << 7 << 25 << 98;
	prt_rbt_data("r1=", r1);

	r1 >> 25;
	prt_rbt_data("r1=", r1);

	r1.clear();
	r2.clear();
	r1 << 9 << 8 << 7 << 6 << 5 << 4 << 3 << 2 << 1;
	r2 << 9 << 7 << 1;
	prt_rbt_cmp("", r1, r2);

	r2 << 4;
	prt_rbt_cmp("<< 4", r1, r2);

	r2 << 8;
	prt_rbt_cmp("<< 8", r1, r2);

	r2.pop(1);
	prt_rbt_cmp("pop(1)", r1, r2);

	r2.pop(9);
	prt_rbt_cmp("pop(9)", r1, r2);

	r1.clear();
	r2.clear();
	prt_rbt_cmp("", r1, r2);
	
	r1 << 9;
	r2.clear();
	prt_rbt_cmp("push(9)", r1, r2);

	r1 << 9;
	r2 << 9;
	prt_rbt_cmp("push(9)", r1, r2);

	r1 << 8;
	r2 << 8;
	prt_rbt_cmp("push(8)", r1, r2);

	r1 << 7;
	r2 << 7;
	prt_rbt_cmp("push(7)", r1, r2);

	r2 << 6;
	prt_rbt_cmp("push(6)", r1, r2);

	r1 << 5;
	prt_rbt_cmp("push(5)", r1, r2);

	r2.pop(6);
	prt_rbt_cmp("pop(6)", r1, r2);
	*/

	/*
	char op = ' ';

	row_redblack<long>	rr(cmp_long);
	while(op != 'q'){
		os << "Choose option.";
		os << " i. insert";
		os << " r. remove";
		os << " c. clear";
		os << " q. quit";
		os << bj_eol;
		std::cin >> op;

		long val;
		if(op == 'i'){
			os << "val=";
			std::cin >> val;
			rr.push(val);
		}
		if(op == 'r'){
			os << "val=";
			std::cin >> val;
			rr.pop(val);
		}
		if(op == 'c'){
			rr.clear();
		}
		if(op != 'q'){
			prt_rbt_data("rr=", rr);
			//os << rr;
		}
	}
	*/

	os << bj_eol;

	os << "End of tests. Type ENTER." << bj_eol;
	return 0;
}


