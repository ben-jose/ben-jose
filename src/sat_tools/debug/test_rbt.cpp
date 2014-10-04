

/*************************************************************

yosoy_v1

test.cpp  
(C 2007) QUIROGA BELTRAN, Jose Luis. Bogotá - Colombia.

For testing purposes only.

--------------------------------------------------------------*/

#include "row_redblack.h"


typedef row_redblack<long>	rbt_long_t;
rbt_long_t decl01(cmp_long);

/*
void	prt_rbt_data(char* nam, rbt_long_t& rr){
	std::ostream& os = std::cout;

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
	os << std::endl;
	//rr.print_tree(os);
	os << rr << std::endl;
	//os << "---------------";
	//REDBLACK_CK(rr.check_tree());
	os << "--------------------------------------" << std::endl;
}

void	prt_rbt_cmp(char* title, rbt_long_t& r1, rbt_long_t& r2){
	std::ostream& os = std::cout;
	os << title << std::endl;

	//os << "r1=" << r1 << " sz=" << r1.size() << std::endl;
	//os << "r2=" << r2 << " sz=" << r2.size() << std::endl;

	os << "--------------------------------------" << std::endl;
	prt_rbt_data("r1=", r1);
	prt_rbt_data("r2=", r2);

	os << "cmp(r1, r2)=";

	bool are_eq = false;
	cmp_is_sub cmp01 = cmp_row_redblack(r1, r2, are_eq);

	os << subset_cmp_str(cmp01) << std::endl; 
	os << "are_eq=" << are_eq << std::endl;
	os << "=======================================" << std::endl;
}

#define NUM_TESTS 5000
#define MAX_SZ_RBT	10

void	test_redblack(){
	std::ostream& os = std::cout;
	rbt_long_t rr(cmp_long);

	os << "test_redblack" << std::endl;

	average		avg_sz;

	init_random_nums();
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;

	for(long jj = 0; jj < num_tests; jj++){
		long sz_rbt = gen_random_num(0, MAX_SZ_RBT);
		if((min_sz == -1) || (sz_rbt < min_sz)){
			min_sz = sz_rbt;
		}
		if((max_sz == -1) || (sz_rbt > max_sz)){
			max_sz = sz_rbt;
		}

		long max_item = gen_random_num(sz_rbt, 3 * sz_rbt);

		// fill the rbt.
		for(long kk = 0; kk < sz_rbt; kk++){
			long nxt_elem = gen_random_num(0, max_item);
			REDBLACK_CK(rr.check_tree());
			rr << nxt_elem;
			//os << CARRIAGE_RETURN;
			//os << rr.size();
		}
		REDBLACK_CK(rr.check_tree());

		avg_sz.add_val((double)rr.size());

		// play with rbt.
		long num_plays = gen_random_num(1, 2 * sz_rbt);
		//long num_plays = gen_random_num(1, 2 * NUM_TESTS);
		for(long pp = 0; pp < num_plays; pp++){
			long play_kind = gen_random_num(0, 2);
			REDBLACK_CK(rr.check_tree());
			if((play_kind == 0) || rr.is_empty()){
				long nxt_elem = gen_random_num(0, max_item);
				rr << nxt_elem;
			} else {
				REDBLACK_CK(play_kind == 1);
				long nxt_elem_idx = gen_random_num(0, (rr.size() - 1));
				long nxt_elem = rr.get_obj(nxt_elem_idx);
				//prt_rbt_data("rr", rr);
				rr >> nxt_elem;
			}
		}
		REDBLACK_CK(rr.check_tree());

		// remove one by one
		while(! rr.is_empty()){
			long nxt_elem_idx = gen_random_num(0, (rr.size() - 1));
			long nxt_elem = rr.get_obj(nxt_elem_idx);
			//prt_rbt_data("rr", rr);
			REDBLACK_CK(rr.check_tree());
			rr >> nxt_elem;
			//os << CARRIAGE_RETURN;
			//os << rr.size();
		}
		REDBLACK_CK(rr.check_tree());
		os << CARRIAGE_RETURN;
		os << jj;
	}

	os << std::endl;
	os << "min_sz=" << min_sz << " max_sz=" << max_sz <<
		" avg_sz=" << avg_sz.avg << std::endl;
}

void	test_subsets(){
	std::ostream& os = std::cout;
	rbt_long_t r1(cmp_long);
	rbt_long_t r2(cmp_long);
	rbt_long_t r2plus(cmp_long);

	os << "test_subsets" << std::endl;

	average		avg_sz;
	cmp_is_sub  cmp_r1_r2;
	bool		are_eq = false;

	init_random_nums();
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;

	long RBT_NUL = r1.get_null();
	REDBLACK_CK(r2.get_null() == RBT_NUL);

	for(long jj = 0; jj < num_tests; jj++){
		long sz_rbt = gen_random_num(0, MAX_SZ_RBT);
		r1.clear();
		r2.clear();
		r2plus.clear();

		long max_item = gen_random_num((sz_rbt + 5), 
				(2 * (sz_rbt + 5)));

		// fill the rbts.
		for(long kk = 0; kk < sz_rbt; kk++){
			long nxt_elem = gen_random_num(0, max_item);
			r1 << nxt_elem;
			r2 << nxt_elem;
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

		//os << "r1=" << r1 << std::endl;
		//os << "r2=" << r2 << std::endl;
		are_eq = false;
		cmp_r1_r2 = cmp_row_redblack(r1, r2, are_eq);
		REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
		REDBLACK_CK(are_eq);

		long mm = 0;

		long diff_val1 = -1;
		while(diff_val1 == -1){
			long nxt_elem = gen_random_num(0, max_item);
			if(r1.search(nxt_elem) == RBT_NUL){
				diff_val1 = nxt_elem;
				r1 << nxt_elem;
			}
		}

		long diff_val2 = -1;
		while(diff_val2 == -1){
			long nxt_elem = gen_random_num(0, max_item);
			if(r1.search(nxt_elem) == RBT_NUL){
				REDBLACK_CK(r2.search(nxt_elem) == RBT_NUL);
				diff_val2 = nxt_elem;
				r2 << nxt_elem;
			}
		}

		//os << "DIFF_VAL1=" << diff_val1 << std::endl;
		//os << "DIFF_VAL2=" << diff_val2 << std::endl;
		//os << "r1=" << r1 << std::endl;
		//os << "r2=" << r2 << std::endl;

		REDBLACK_CK(r1.search(diff_val1) != RBT_NUL);
		REDBLACK_CK(r2.search(diff_val1) == RBT_NUL);
		REDBLACK_CK(r1.search(diff_val2) == RBT_NUL);
		REDBLACK_CK(r2.search(diff_val2) != RBT_NUL);

		REDBLACK_CK(r1.size() == (ini_sz + 1));
		REDBLACK_CK(r2.size() == (ini_sz + 1));
		REDBLACK_CK(r1.get_obj(ini_sz) == diff_val1);
		REDBLACK_CK(r2.get_obj(ini_sz) == diff_val2);

		
		//long plus_sz1 = gen_random_num(0, MAX_SZ_RBT);
		//for(mm = 0; mm < plus_sz1; mm++){
		//	long nxt_elem = gen_random_num(0, max_item);
		//	if(nxt_elem != diff_val2){
		//		r1 << nxt_elem;
		//	}
		//}

		long plus_sz2 = gen_random_num(0, MAX_SZ_RBT);
		for(mm = 0; mm < plus_sz2; mm++){
			long nxt_elem = gen_random_num(0, max_item);
			if(nxt_elem != diff_val1){
				if(r2.search(nxt_elem) == RBT_NUL){
					r2plus << nxt_elem;
				}
				r2 << nxt_elem;
			}
		}

		//os << CARRIAGE_RETURN << jj;
		while(! r2plus.is_empty()){
			are_eq = false;
			cmp_r1_r2 = cmp_row_redblack(r2, r1, are_eq);
			if(cmp_r1_r2 != k_no_is_sub){
				os << "TEST_FAILED" << std::endl;
				os << "r1=" << r1 << std::endl;
				os << "r2=" << r2 << std::endl;
			}
			REDBLACK_CK(cmp_r1_r2 == k_no_is_sub);
			REDBLACK_CK(! are_eq);

			long nxt_elem_idx = gen_random_num(0, r2plus.size());
			long nxt_elem = r2plus.get_obj(nxt_elem_idx);
			r2plus >> nxt_elem;
			r2 >> nxt_elem;
		}

		REDBLACK_CK(r2.size() == (ini_sz + 1));
		r2 >> diff_val2;

		//os << "r1=" << r1 << std::endl;
		//os << "r2=" << r2 << std::endl;
		are_eq = false;
		cmp_r1_r2 = cmp_row_redblack(r2, r1, are_eq);
		if(cmp_r1_r2 != k_lft_is_sub){
			os << "TEST_FAILED" << std::endl;
			os << "r1=" << r1 << std::endl;
			os << "r2=" << r2 << std::endl;
		}
		REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
		REDBLACK_CK(! are_eq);

		// remove one by one
		while(! r2.is_empty()){
			long nxt_elem_idx = gen_random_num(0, r2.size());
			long nxt_elem = r2.get_obj(nxt_elem_idx);
			r2 >> nxt_elem;

			are_eq = false;
			cmp_r1_r2 = cmp_row_redblack(r2, r1, are_eq);
			if(cmp_r1_r2 != k_lft_is_sub){
				os << "TEST_FAILED" << std::endl;
				os << "r1=" << r1 << std::endl;
				os << "r2=" << r2 << std::endl;
			}
			REDBLACK_CK(cmp_r1_r2 == k_lft_is_sub);
			REDBLACK_CK(! are_eq);
		}

		//os << "==========================" << std::endl;
		os << CARRIAGE_RETURN << jj;
	}

	os << std::endl;
	os << "min_sz=" << min_sz << " max_sz=" << max_sz <<
		" avg_sz=" << avg_sz.avg << std::endl;
}
*/

int	tests_main_(int argc, char** argv){
	std::ostream& os = std::cout;

	//test_redblack();
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
		os << std::endl;
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

	os << std::endl;
	/*
	os << 
	//"bits_of_long=" << bits_of_long << std::endl <<
	"sizeof(bool)=" << sizeof(bool) << std::endl <<
	"sizeof(void*)=" << sizeof(void*) << std::endl <<
	"sizeof(alphan)=" << sizeof(alphan) << std::endl <<
	"sizeof(betan)=" << sizeof(betan) << std::endl <<
	"sizeof(gamman)=" << sizeof(gamman) << std::endl <<
	"sizeof(brain)=" << sizeof(brain) << std::endl <<
	"sizeof(c_alpha)=" << sizeof(c_alpha) << std::endl <<
	"sizeof(a_alpha)=" << sizeof(a_alpha) << std::endl <<
	"sizeof(c_beta)=" << sizeof(c_beta) << std::endl <<
	"sizeof(a_beta)=" << sizeof(a_beta) << std::endl <<
	"sizeof(a_gamma)=" << sizeof(a_gamma) << std::endl <<
	"sizeof(c_brain)=" << sizeof(c_brain) << std::endl <<
	"sizeof(a_brain)=" << sizeof(a_brain) << std::endl;
	*/

	os << "End of tests. Type ENTER." << std::endl;
	return 0;
}


