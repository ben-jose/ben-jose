
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

test.cpp  

file for test and debugging purposes.

--------------------------------------------------------------*/

#include <fstream>
#include <set>
#include <iterator>

//     g++ t1.cpp -lgmpxx -lgmp -o tt.exe

#include "file_tree.h"
#include "ch_string.h"

#include "brain.h"
#include "tak_mak.h"
#include "skeleton.h"
#include "file_funcs.h"

#include "top_exception.h"

#include "dbg_config.h"

skeleton_glb 	GSKE;
tak_mak*	glb_test_tak_mak = NULL_PT;

#define TEST_CK(prm) DBG_BJ_LIB_CK(prm)

#define TEST_ROOT_PATH "/home/jose/devel/tmp/solver/dbg"

void	test_sorted_ops();
void	test_sorted_ops2();
void	test_subsets();
void 	test_nfwt(int argc, char** argv);
void 	test_thrw_obj();

void
test_creat(int argc, char** argv){
	bj_ostream& os = bj_out;
	if(argc < 2){
		os << "Faltan args" << bj_eol;
		return;
	}

	ch_string pth = argv[1];

	os << "pth=" << pth << bj_eol;
	bool pth_ok = path_create(pth);
	MARK_USED(pth_ok);
	BRAIN_CK(pth_ok);
}

void
find_nth_dir(long nn, ch_string& pth, ch_string& sub_pth){
	bj_ostream& os = bj_out;
	typedef ch_string::size_type pos_t;
	//pos_t pp = ch_string::npos;
	pos_t pp = 0;
	pos_t pth_sz = pth.size();
	for(long ii = 0; ii < nn; ii++){
		if(pp > pth_sz){
			pp = pth_sz;
			break;
		}
		pp = pth.find('/', pp + 1);
	}
	sub_pth = pth.substr(0, pp);
	os << "SUB=" << sub_pth << bj_eol;
}

void	test_long_to_pth(int argc, char** argv){
	bj_ostream& os = bj_out;
	if(argc < 2){
		os << "Faltan args" << bj_eol;
		return;
	}
	os << "arg1=" << argv[1] << bj_eol;
	long nn = atol(argv[1]);
	os << "L1=" << nn << bj_eol;
	//ch_string pth = long_to_path(nn, 3);
	ch_string pth = long_to_path(nn, 3);
	os << "pth=" << pth << bj_eol;
}

void	test_dims_to_path(int argc, char** argv){
	bj_ostream& os = bj_out;
	row<bool> dbg_arr;
	dbg_arr.fill(false, DBG_NUM_LEVS);

	dbg_arr[96] = true;

	if(argc < 4){
		os << "Faltan args" << bj_eol;
		return;
	}
	long tl = atol(argv[1]);
	long nc = atol(argv[2]);
	long nv = atol(argv[3]);
	long tt = 0;

	ch_string pth = "";

	dima_dims dims;
	dims.dd_tot_lits = tl;
	dims.dd_tot_ccls = nc;
	dims.dd_tot_vars = nv;
	dims.dd_tot_twolits = tt;

	pth = cnf_dims_to_path(dims);

	os << " tl=" << tl << " nc=" << nc << " nv=" << nv << " tt=" << tt << bj_eol;
	os << " pth='" << pth << "'" << bj_eol;
}

struct ltstr {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) < 0;
	}
};

void
test_sha_pth(){
	bj_ostream& os = bj_out;

	ch_string str_cont = "HOLA_MUNDO";
	ch_string sha_txt;
	row<char> cnn;
	canon_string_append(cnn, str_cont);
	canon_sha(cnn, sha_txt);
	slice_str_to_path(sha_txt);

	os << "'" << sha_txt << "'" << bj_eol;
	os << "sz=" << sha_txt.size() << bj_eol;
}

std::set<ch_string> selected_leafs;

// man 2 open
// man 2 gethostname
// man 2 getpid
// man 2 link

enum ftw_op_t {
	tw_load,
	tw_walk,
	tw_delete,
	tw_sub_leaf,
	tw_sup_leafs
};

void	test_skl();

void
gen_clause(tak_mak& gg, row_long_t& the_ccl,
		long max_ccl_sz, long num_vars_cnf)
{
	//bj_ostream& os = bj_out;

	unsigned long ccl_sz = gg.gen_rand_int32_ie(2, max_ccl_sz);
	long num_neg = gg.gen_rand_int32_ie(1, ccl_sz);
	long num_pos = ccl_sz - num_neg;
	MARK_USED(num_pos);
	BRAIN_CK(num_pos >= 0);

	typedef std::set<long>			set_long_t;
	typedef set_long_t::iterator		set_long_iter_t;

	//os << "ccl_sz=" << ccl_sz << bj_eol;
	//os << "num_vars_cnf=" << num_vars_cnf << bj_eol;

	std::set<long> varset;
	for(unsigned long aa = 0; aa < ccl_sz; aa++){	
		// leave it like this even if ccl_sz is not reached
		// to avoid loop because (ccl_sz > num_vars_cnf) can happen
		long vv = gg.gen_rand_int32_ie(1, num_vars_cnf);
		varset.insert(vv);
	}

	the_ccl.clear(true);

	for(set_long_iter_t aa = varset.begin(); aa != varset.end(); aa++){
		long vv = (*aa);
		if(the_ccl.size() < num_neg){
			vv = -vv;
		}
		BRAIN_CK(vv != 0);
		the_ccl.push(vv);
	}

	//os << "start_sorting=" << the_ccl << bj_eol;

	the_ccl.mix_sort(cmp_canon_ids);
	BRAIN_CK(the_ccl.is_sorted(cmp_canon_ids));
}

void
finish_cnf(canon_cnf& the_cnf, long num_vars, long tot_lits, long tot_twolits){
	row<canon_clause*>& all_ccl = the_cnf.cf_clauses;

	the_cnf.cf_dims.dd_tot_lits = tot_lits;	// purge_clauses change this val so leave this here.
	the_cnf.cf_dims.dd_tot_twolits = tot_twolits;

	the_cnf.purge_clauses(GSKE);

	the_cnf.cf_dims.dd_tot_ccls = all_ccl.size();
	the_cnf.cf_dims.dd_tot_vars = num_vars;

	the_cnf.calc_sha();

	BRAIN_CK(! the_cnf.cf_sha_str.empty());

	the_cnf.init_skl_paths(GSKE);
}

void
gen_ccls_cnf(tak_mak& gg, canon_cnf& the_cnf, long max_ccl_sz,
		long max_num_ccls_cnf, long max_num_vars_cnf)
{
	//bj_ostream& os = bj_out;

	row<canon_clause*>& all_ccl = the_cnf.cf_clauses;
	BRAIN_CK(all_ccl.is_empty());

	long cnf_sz = gg.gen_rand_int32_ie(1, max_num_ccls_cnf);
	long num_vars = gg.gen_rand_int32_ie(2, max_num_vars_cnf);

	//os << "cnf_sz=" << cnf_sz << bj_eol;
	//os << "num_vars=" << num_vars << bj_eol;

	long tot_lits = 0;
	long tot_twolits = 0;
	long ii = 0;
	for(ii = 0; ii < cnf_sz; ii++){
		canon_clause& ccl_1 = the_cnf.add_clause(GSKE);
		row<long>& rr_1 = *((row<long>*)(&ccl_1));

		gen_clause(gg, rr_1, max_ccl_sz, num_vars);
		tot_lits += ccl_1.cc_size();
		if(ccl_1.cc_size() == 2){ tot_twolits++; }
	}

	all_ccl.mix_sort(cmp_clauses);

	finish_cnf(the_cnf, num_vars, tot_lits, tot_twolits);
}

/*
void
recalc_cnf(canon_cnf& the_cnf){
	long num_vars = the_cnf.cf_dims.dd_tot_vars;

	long tot_lits = 0;
	long tot_twolits = 0;

	row<canon_clause*>& all_ccl = the_cnf.cf_clauses;

	row<canon_clause*> non_reps;
	for(long ii = 0; ii < all_ccl.size(); ii++){
		canon_clause* pt_ccl = all_ccl[ii];
		if(pt_ccl != NULL_PT){

			non_reps.push(pt_ccl);
			tot_lits += pt_ccl->cc_size();
			if(pt_ccl->cc_size() == 2){ tot_twolits++; }
		}
	}

	all_ccl.clear();
	the_cnf.init_canon_cnf();

	non_reps.move_to(all_ccl);

	the_cnf.cf_dims.dd_tot_ccls = all_ccl.size();

	the_cnf.cf_dims.dd_tot_vars = num_vars;
	the_cnf.cf_dims.dd_tot_lits = tot_lits;
	the_cnf.cf_dims.dd_tot_twolits = tot_twolits;

	the_cnf.calc_sha();

	BRAIN_CK(! the_cnf.cf_sha_str.empty());

	the_cnf.init_skl_paths(GSKE);
}

void
gen_cnf_skls(tak_mak& gg, canon_cnf& the_cnf, long num_skls, row<ch_string>& pths){
	bj_ostream& os = bj_out;

	pths.push(the_cnf.get_canon_name());

	row<canon_clause*>& all_ccl = the_cnf.cf_clauses;

	for(long hh = 0; hh < num_skls; hh++){
		long num_del = gg.gen_rand_int32_ie(1, all_ccl.size() / 2);
		for(long dd = 0; dd < num_del; dd++){
			long idx_del = gg.gen_rand_int32_ie(0, all_ccl.last_idx());
			canon_clause* ccl2 = all_ccl[idx_del];
			if(ccl2 != NULL_PT){
				all_ccl[idx_del] = NULL_PT;

				BRAIN_CK(ccl2 != NULL_PT);
				bool free_cc_mem = true;
				ccl2->cc_clear(free_cc_mem);
				GSKE.release_clause(*ccl2, free_cc_mem);
			}
		}
		recalc_cnf(the_cnf);

		os << the_cnf.cf_phdat << " has_ph=" << the_cnf.has_phase_path() << bj_eol;

		pths.push(the_cnf.get_canon_name());
		//the_cnf.print_canon_cnf(os);
		the_cnf.save_cnf(GSKE);
	}
}
*/

void
test_nfwt(int argc, char** argv)
{
	bj_ostream& os = bj_out;
	MARK_USED(os);
	
	row<bool> dbg_arr;
	dbg_arr.fill(false, DBG_NUM_LEVS);

	dbg_arr[2] = true;
	dbg_arr[78] = true;
	dbg_arr[81] = true;
	//dbg_arr[82] = true;
	dbg_arr[88] = true;
	dbg_arr[89] = true;
	//dbg_arr[92] = true;
	dbg_arr[93] = true;
	dbg_arr[94] = true;
	dbg_arr[95] = true;

	//ccl_skl_walker& skl_wlkr = GSKE.kg_ccl_wlkr;
	//clause_walker& wlkr = skl_wlkr.fw_walker;

	canon_cnf the_cnf;

	//wlkr.cw_cnf = &the_cnf;

	bool exe_wlk = false;
	bool exe_sub = false;
	bool exe_sup = false;
	bool gen_skl = false;
	bool del_skl = false;
	bool ver_skl = false;

	MARK_USED(exe_wlk);
	MARK_USED(exe_sub);
	MARK_USED(exe_sup);
	MARK_USED(ver_skl);
	
	row<ch_string> load_ops;
	row<ch_string> pth_ld_ops;
	row<ch_string> delete_ops;

	GSKE.kg_root_path = TEST_ROOT_PATH;

	for(long ii = 1; ii < argc; ii++){
		ch_string the_arg = argv[ii];
		if(strcmp(argv[ii], "-sub") == 0){
			exe_sub = true;
		} else if(strcmp(argv[ii], "-sup") == 0){
			exe_sup = true;
		} else if(strcmp(argv[ii], "-g") == 0){
			gen_skl = true;
		} else if(strcmp(argv[ii], "-D") == 0){
			del_skl = true;
		} else if(strcmp(argv[ii], "-w") == 0){
			exe_wlk = true;
		} else if(strcmp(argv[ii], "-v") == 0){
			ver_skl = true;
		} else if((strcmp(argv[ii], "-d") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			ch_string& str_1 = delete_ops.inc_sz();
			str_1 = argv[kk_idx];

		} else if((strcmp(argv[ii], "-l") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			ch_string& str_1 = load_ops.inc_sz();
			str_1 = argv[kk_idx];

		} else if((strcmp(argv[ii], "-L") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			ch_string& str_1 = pth_ld_ops.inc_sz();
			str_1 = argv[kk_idx];

		} else if((strcmp(argv[ii], "-r") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			GSKE.kg_root_path = argv[kk_idx];

		} 
	}

	GSKE.init_paths();
	GSKE.print_paths(os);

	if(del_skl){
		ch_string GSKE_ROOT = GSKE.kg_root_path + SKG_SKELETON_DIR;
		os << "WARNING !!! deleting '" << GSKE_ROOT << "'" << bj_eol;
		delete_directory(GSKE_ROOT);
	}

	if(gen_skl){
		test_skl();
	}

	while(! load_ops.is_empty()){
		ch_string& to_load = load_ops.last();

		ch_string rel_pth = SKG_CNF_DIR + to_load;
		ch_string full_pth = GSKE.as_full_path(rel_pth);
		os << "to_load='" << full_pth << "'" << bj_eol;

		bool all_ok = the_cnf.load_from(GSKE, full_pth);
		if(all_ok){
			the_cnf.print_canon_cnf(os);
		} else {
			os << "LOAD OF " << full_pth << " FAILED !!" << bj_eol;
		}
		os << bj_eol;

		load_ops.dec_sz();
	}
	while(! pth_ld_ops.is_empty()){
		ch_string& to_load = pth_ld_ops.last();

		ch_string full_pth = to_load;
		os << "to_load='" << full_pth << "'" << bj_eol;

		bool all_ok = the_cnf.load_from(GSKE, full_pth);
		if(all_ok){
			the_cnf.print_canon_cnf(os);
		} else {
			os << "LOAD OF " << full_pth << " FAILED !!" << bj_eol;
		}
		os << bj_eol;

		pth_ld_ops.dec_sz();
	}
	while(! delete_ops.is_empty()){
		ch_string& to_del = delete_ops.last();
		os << "to_del='" << to_del << "'" << bj_eol;
		delete_ops.dec_sz();
	}

}

void
test_realpath(int argc, char** argv)
{
	bj_ostream& os = bj_out;

	/* Verify argv[1] is supplied */
	if(argc < 2){
		os << "Faltan agrs !!" << bj_eol;
	}

	ch_string pth = argv[1];
	ch_string r_pth = path_to_absolute_path(pth);

	os << "r_pth='" << r_pth << "'" << bj_eol;
}

// ===============================================

void
gen_sub_cnf(tak_mak& gg, canon_cnf& src_cnf, canon_cnf& dst_cnf){
	//bj_ostream& os = bj_out;

	dst_cnf.release_and_init(GSKE, true);

	row<canon_clause*>& all_ccl = src_cnf.cf_clauses;

	long tot_lits = 0;
	long tot_twolits = 0;

	for(long dd = 0; dd < all_ccl.size(); dd++){
		TEST_CK(all_ccl[dd] != NULL_PT);
		canon_clause& src_ccl = *(all_ccl[dd]);
		long add_it = gg.gen_rand_int32_ie(0, 2);
		if(add_it > 0){
			canon_clause& dst_ccl = dst_cnf.add_clause(GSKE);
			src_ccl.cc_copy_to(dst_ccl);
			tot_lits += dst_ccl.cc_size();
			if(dst_ccl.cc_size() == 2){ tot_twolits++; }
		}		
	}

	finish_cnf(dst_cnf, src_cnf.cf_dims.dd_tot_vars, tot_lits, tot_twolits);
}

bool
test_pair_subsets(tak_mak& rnd_gen, long n_iter){
	bj_ostream& os = bj_out;
	MARK_USED(os);

	canon_cnf the_cnf;
	canon_cnf sub_cnf;

	long max_ccl_sz = 5;
	long max_num_ccls_cnf = 100;
	long max_num_vars_cnf = 10;

	gen_ccls_cnf(rnd_gen, the_cnf, max_ccl_sz, max_num_ccls_cnf, max_num_vars_cnf);
	gen_sub_cnf(rnd_gen, the_cnf, sub_cnf);

	row<canon_clause*>& all_ccls = the_cnf.cf_clauses;
	row<canon_clause*>& all_sub_ccls = sub_cnf.cf_clauses;

	//the_cnf.print_canon_cnf(os);
	//sub_cnf.print_canon_cnf(os);
	//os.flush();

	bool are_eq = false;
	cmp_is_sub cmp_resp =
		all_ccls.sorted_set_is_subset(all_sub_ccls, cmp_clauses, are_eq);


	bool is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));
	MARK_USED(is_sub);
	BRAIN_CK(is_sub);

	the_cnf.release_and_init(GSKE, true);
	sub_cnf.release_and_init(GSKE, true);

	return true;
}

void
test_subsets(){
	bj_ostream& os = bj_out;

	MARK_USED(os);

	row<bool> dbg_arr;
	dbg_arr.fill(false, DBG_NUM_LEVS);
	
	dbg_arr[2] = true;

	GSKE.kg_root_path = TEST_ROOT_PATH;
	GSKE.init_paths();
	GSKE.print_paths(os);

	ch_string GSKE_ROOT = GSKE.kg_root_path + SKG_SKELETON_DIR;
	os << "WARNING !!! deleting '" << GSKE_ROOT << "'" << bj_eol;
	delete_directory(GSKE_ROOT);
	//dbg_arr[80] = true;

	long num_test_to_gen = 1000;
	//unsigned long init_val = (long)(run_time());
	unsigned long init_val = 10;
	tak_mak rnd_gen(init_val);

	for(long aa = 0; aa < num_test_to_gen; aa++){
		test_pair_subsets(rnd_gen, aa);
		os << aa << " ";
		os.flush();
	}
	os << bj_eol;

	GSKE.clear_all();

	os << "2. FINISHING ..."  << bj_eol;
	os.flush();
}

// ========================

long num_vals_free = 0;

class cvals1 {
public:
	long val1;
	long val2;

	cvals1(long v1 = 999, long v2 = 666){
		val1 = v1;
		val2 = v2;
	}

	~cvals1(){
		val1 = -333;
		val2 = -555;
		num_vals_free++;
	}

	//bool operator < (const cvals1& x, const cvals1& y) { return x.val1 < y.val1; }
	//static 

};

bool operator < (const cvals1& x, const cvals1& y) { return x.val1 < y.val1; }

bj_ostream&	operator << (bj_ostream& os, const cvals1& x){
	os << "vals=" << x.val1 << "," << x.val2 << bj_eol;
	return os;
}

void test_sets(){

	bj_ostream& os = bj_out;

	std::set<cvals1> varset;

	os << "num_vals_free=" << num_vals_free << bj_eol;
	for(unsigned long aa = 0; aa < 10; aa++){
		cvals1 vv(aa, aa+5);
		varset.insert(vv);
	}
	os << "num_vals_free=" << num_vals_free << bj_eol;

	copy(varset.begin(), varset.end(),
	std::ostream_iterator<cvals1>(os, "\n"));
	os.flush();

	os << "num_vals_free=" << num_vals_free << bj_eol;
	varset.clear();
	os << "num_vals_free=" << num_vals_free << bj_eol;
}

// ========================

void	test_big_num(){
	bj_big_int_t a, b, c, d;

	a = 0;
	mpz_setbit(a.get_mpz_t(), 6);
	mpz_setbit(a.get_mpz_t(), 12);
	mpz_setbit(a.get_mpz_t(), 17);
	//mpz_setbit(a.get_mpz_t(), 112);
	bj_out << "a is " << a << "\n";

	ch_string s1 = a.get_str(2);
	bj_out << "s1 is " << s1 << bj_eol;

	//long cm1 = mpz_cmp(nu1.get_mpz_t(), nu2.get_mpz_t());

}

void	test_big2(){
	mpz_t v1;
	mpz_t v2;
	bj_big_int_t a, b, c, d;

	//mpz_setbit(d, 6);

	mpz_init_set_ui(v1, 0);
	mpz_init_set_ui(v2, 0);

	mpz_setbit(v1, 6);

	//std::ios_base::showbase = 2;
	bj_out << "v1 is " << v1 << "\n";

	d = bj_big_int_t(v1);
	bj_out << "d is " << d << "\n";

	ch_string s_d;
	d.set_str(s_d, 2);
	bj_out << "s_d is " << d << "\n";

	//ch_string s_v1 = mpz_get_str(NULL, 2, v1);
	//bj_out << "s_v1 is " << s_v1 << "\n";
	ch_string s2_d = mpz_get_str(NULL, 2, d.get_mpz_t());
	bj_out << "s2_d is " << s2_d << "\n";
	//v2 = d.get_mpz_t();

	mpz_clear(v1);
	mpz_clear(v2);

	/*
	a = 1234;
	b = "-5678";
	c = a+b;
	bj_out << "sum is " << c << "\n";
	bj_out << "absolute value is " << abs(c) << "\n";

	mpq_class a2, b2, c2, d2, e2, f2;

	//a2 = 130;
	a2 = 23;
	b2 = 5;
	c2 = a2 / b2;
	d2 = b2 / a2;
	e2 = c2 * d2;
	f2 = c2 + d2;
	//d2 = a2 % b2;
	bj_out << a2 << " div " << b2 << " is " << c2 << "\n";
	bj_out << b2 << " div " << a2 << " is " << d2 << "\n";
	bj_out << c2 << " mul " << d2 << " is " << e2 << "\n";
	bj_out << c2 << " + " << d2 << " = " << f2 << "\n";
	//bj_out << a2 << " mod " << b2 << " is " << d2 << "\n";
	*/
}

void	test_big3(){
	bj_ostream& os = bj_out;

	bj_big_int_t p1, p2;
	p1 = 0;
	p2 = 500;
	for(long aa = 0; aa < 100; aa++){
		os << "p1=" << p1 << " p2=" << p2 << bj_eol;
		mpz_nextprime(p1.get_mpz_t(), p2.get_mpz_t());
		p2 = p1;
	}
	
}

void	test_big4(){
	bj_ostream& os = bj_out;

	bj_big_int_t p1, p2, p3;

	p1 = "123412351346163457";
	p2 = "123412351346163456";
	p3 = "123412351346163457";

	if(p1 != p3){
		os << "numbers diff" << bj_eol;
	}
	if(p1 <= p2){


		os << "p1 <= p2 :" << p1 << " <= " << p2 << bj_eol;
	} else {
		os << "p1 > p2 :" << p1 << " > " << p2 << bj_eol;
	}
}

void	test_str1(){
	bj_ostream& os = bj_out;

	ch_string ss1;
	ch_string ss2;
	ss1 = "jose";
	ss2 = " pedro pablo \n";
	ss1 += ss2;
	os << ss1 << bj_eol;
}

void	test_symlk(int argc, char** argv){
	bj_ostream& os = bj_out;

	if(argc < 3){
		os << "Faltan agrs !!" << bj_eol;
		return;
	}

	ch_string pth1 = argv[1];
	ch_string pth2 = argv[2];

	symlink(pth1.c_str(), pth2.c_str());
}

void	test_tm_elapsed(int argc, char** argv){
	bj_ostream& os = bj_out;
	
	if(argc < 3){
		os << "Faltan agrs !!" << bj_eol;
	}

	ch_string oper = argv[1];
	ch_string pth = argv[2];
	
	long rd_ok = -1;
	bj_big_float_t n_avg;
	average the_avg;

	if(oper == "r"){
		rd_ok = read_elapsed(pth, the_avg);
	} else {
		n_avg = update_elapsed(pth);
	}

	if((n_avg < 0) || (rd_ok < 0)){
		os << "ERROR !!!!" << bj_eol;
		os << "ERROR !!!!" << bj_eol;
		os << "ERROR !!!!" << bj_eol;
		os << "ERROR !!!!" << bj_eol;
	}

	os << "n_avg='" << n_avg << "'" << bj_eol;
}

void	test_open_unlink(int argc, char** argv){
	bj_ostream& os = bj_out;
	
	if(argc < 2){
		os << "Faltan agrs !!" << bj_eol;
	}

	ch_string the_pth = argv[1];
	ch_string oper = argv[2];
	ch_string full_pth = path_to_absolute_path(the_pth);

	if(oper == "d"){
		ch_string top_pth = TEST_ROOT_PATH; 
		ch_string full_pth = path_to_absolute_path(the_pth);

		os << "DELETING " << full_pth << " ..." << bj_eol;
		path_delete(full_pth, top_pth);
	} else {
		std::ifstream istm;
		istm.open(the_pth.c_str(), std::ios::binary);
		if(istm.good() && istm.is_open()){
			os << "File " << the_pth << " OK. Type ENTER ..." << bj_eol;
			getchar();

			istm.seekg (0, std::ios::end);
			long file_sz = istm.tellg();
			istm.seekg (0, std::ios::beg);

			if(file_sz < 0){
				os << "ERROR 1" << bj_eol;
				abort_func(1);
			}

			BRAIN_CK(sizeof(char) == 1);

			char* file_content = tpl_malloc<char>(file_sz + 1);
			istm.read(file_content, file_sz);
			long num_read = istm.gcount();
			if(num_read != file_sz){
				os << "ERROR 2" << bj_eol;
				abort_func(1);
			}
			file_content[file_sz] = 0;

			ch_string fdat = file_content;
			tpl_free<char>(file_content, file_sz + 1);

			os << fdat << bj_eol;

			istm.close();
		} else {
			os << "Could NOT open file " << the_pth << bj_eol;
		}
	}
}

//=================================

int
load_entry(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	bj_ostream& os = bj_out;

	MARK_USED(sb);
	MARK_USED(ftwbuf);

	switch (tflag) {
	case FTW_D:
	case FTW_DNR:
	case FTW_DP:
		break;
	default:{
			ch_string full_pth = fpath;
			ch_string cnn_nm = SKG_CANON_NAME;
			if(path_ends_with(full_pth, cnn_nm)){
				canon_cnf the_cnf;

				os << full_pth;

				the_cnf.release_and_init(GSKE, true);
				bool all_ok = the_cnf.load_from(GSKE, full_pth);
				if(all_ok){
					os << "LOAD OK";
					//os << "LOAD OK";
					//the_cnf.print_canon_cnf(os);
				} else {
					os << "LOAD OF " << full_pth << " FAILED !!";
				}
				//os << bj_eol;
				the_cnf.release_and_init(GSKE, true);
			}
		}
		break;
	}
	return (0);
}

void
load_all_in_dir(ch_string& dir_nm){
	long max_depth = BJ_FILE_MAX_PATH_DEPTH;
	nftw(dir_nm.c_str(), load_entry, max_depth, FTW_DEPTH);
}

int
update_elap_entry(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	//bj_ostream& os = bj_out;

	MARK_USED(sb);
	MARK_USED(ftwbuf);

	BRAIN_CK(glb_test_tak_mak != NULL_PT);

	switch (tflag) {
	case FTW_D:
	case FTW_DNR:
	case FTW_DP:
		break;
	default:{
			ch_string pth_str = fpath;
			ch_string cnn_nm = SKG_CANON_NAME;
			if(path_ends_with(pth_str, cnn_nm)){
				tak_mak& gg = *glb_test_tak_mak;
				long upd_it = gg.gen_rand_int32_ie(0, 2);
				if(upd_it > 0){
					ch_string full_pth = path_get_directory(pth_str, true);

					ch_string elp_nm = full_pth + SKG_ELAPSED_NAME;
					update_elapsed(elp_nm);

				}
			}
		}
		break;
	}
	return (0);
}

void
update_rnd_elap_in_dir(tak_mak& gg, ch_string& dir_nm){
	long num_rr = gg.gen_rand_int32_ie(0, 10);

	long max_depth = BJ_FILE_MAX_PATH_DEPTH;

	for(long aa = 0; aa < num_rr; aa++){
		nftw(dir_nm.c_str(), update_elap_entry, max_depth, FTW_DEPTH);
	}
}

void
gen_phases(tak_mak& gg, ref_strs& the_ph){
	the_ph.init_ref_strs();

	//long num_pha = gg.gen_rand_int32_ie(1, 4);
	long num_pha = gg.gen_rand_int32_ie(1, 3);
	BRAIN_CK(num_pha >= 1);
	//BRAIN_CK(num_pha < 4);
	BRAIN_CK(num_pha < 3);

	ch_string rr_pth = SKG_REF_DIR;

	if(num_pha >= 1){
		the_ph.pd_ref1_nam = rr_pth + "/test_ref1/";
	}
	if(num_pha >= 2){
		the_ph.pd_ref2_nam = rr_pth + "/test_ref2/";
	}
	/*if(num_pha >= 3){
		the_ph.pd_ref3_nam = rr_pth + "/test_ref3/";
	}*/
}

void
test_skl(){
	row<bool> dbg_arr;
	dbg_arr.fill(false, DBG_NUM_LEVS);
	
	dbg_arr[2] = true;
	//dbg_arr[95] = true;

	bj_ostream& os = bj_out;
	MARK_USED(os);

	GSKE.kg_root_path = TEST_ROOT_PATH;

	ch_string GSKE_ROOT = GSKE.kg_root_path + SKG_SKELETON_DIR;
	os << "WARNING !!! deleting '" << GSKE_ROOT << "'" << bj_eol;
	delete_directory(GSKE_ROOT);

	GSKE.init_paths();
	GSKE.print_paths(os);

	GSKE.kg_find_cnn_pth = true;
	GSKE.kg_dbg_verifying = false;
	GSKE.kg_dbg_only_save = false;

	os << "Type RETURN ..." << bj_eol;
	getchar();

	GSKE.kg_dbg_save_canon = true;

	//dbg_arr[107] = true;
	dbg_arr[73] = true;
	//dbg_arr[76] = true;
	//dbg_arr[78] = true;
	//dbg_arr[108] = true;
	//dbg_arr[109] = true;

	//ccl_skl_walker& skl_wlkr = GSKE.kg_ccl_wlkr;
	//clause_walker& wlkr = skl_wlkr.fw_walker;

	canon_cnf the_cnf;
	//wlkr.cw_cnf = &the_cnf;

	//unsigned long init_val = (long)(run_time());
	unsigned long init_val = 10;

	tak_mak rnd_gen(init_val);
	tak_mak rnd_gen2(init_val);

	glb_test_tak_mak = &rnd_gen2;

	long NUM_CNFS = 50;

	long max_ccl_sz = 5;
	long max_num_ccls_cnf = 200;
	long max_num_vars_cnf = 50;

	ch_string rr_pth = SKG_REF_DIR;
	ch_string r1_pth = rr_pth + "/test_ref1/";
	ch_string r2_pth = rr_pth + "/test_ref2/";
	ch_string r3_pth = rr_pth + "/test_ref3/";

	GSKE.ref_create(r1_pth);
	GSKE.ref_create(r2_pth);
	GSKE.ref_create(r3_pth);

	//GSKE.report_err("a_missing_path", GSKE.kg_missing_path);

	ch_string skl_nt_pth = GSKE.as_full_path(SKG_CNF_DIR);

	os << "1. SAVING TEST ..."  << bj_eol;

	for(long aa = 0; aa < NUM_CNFS; aa++){
		the_cnf.release_and_init(GSKE, true);
		gen_phases(rnd_gen, the_cnf.cf_phdat);
		gen_ccls_cnf(rnd_gen, the_cnf, max_ccl_sz, max_num_ccls_cnf, max_num_vars_cnf);

		ch_string cnn_name = the_cnf.get_cnf_path() + SKG_DIFF_NAME;
		if(! GSKE.ref_exists(cnn_name)){
			os << "num_test=" << aa << bj_eol;
			the_cnf.save_cnf(GSKE, cnn_name);

			long f_idx = the_cnf.first_vnt_i_super_of(GSKE);
			MARK_USED(f_idx);
			BRAIN_CK(f_idx != INVALID_NATURAL);

		} else {
			os << bj_eol;
			os << bj_eol;
			os << "!!! SKIPED num_test=" << aa;
		}

		the_cnf.release_and_init(GSKE, true);

		update_rnd_elap_in_dir(rnd_gen2, skl_nt_pth);
	}

	os << "3. LOADING TEST ..."  << bj_eol;

	bool fnd1 = GSKE.find_skl_path(GSKE.as_full_path(rr_pth + "/test_ref1/"));
	bool fnd2 = GSKE.find_skl_path(GSKE.as_full_path(rr_pth + "/test_ref2/"));
	bool fnd3 = GSKE.find_skl_path(GSKE.as_full_path(rr_pth + "/test_ref3/"));

	MARK_USED(fnd1);
	MARK_USED(fnd2);
	MARK_USED(fnd3);

	BRAIN_CK(fnd1);
	BRAIN_CK(fnd2);
	BRAIN_CK(fnd3);

	load_all_in_dir(skl_nt_pth);

	the_cnf.release_and_init(GSKE, true);
	GSKE.clear_all();

	os << "4. FINISHING ..."  << bj_eol;
	os.flush();
}

void
test_lk_name(){
	bj_ostream& os = bj_out;
	MARK_USED(os);

	ch_string str1 = "CADENA DE PRUEBA";
	uchar_t* arr_to_sha = (uchar_t*)(str1.c_str());
	long arr_to_sha_sz = str1.size();
	
	ch_string the_sha = sha_txt_of_arr(arr_to_sha, arr_to_sha_sz);

	dima_dims dim0;
	dim0.dd_tot_lits = 245;
	dim0.dd_tot_ccls = 25;
	dim0.dd_tot_vars = 70;
	dim0.dd_tot_twolits = 18;
	//ch_string lk_nm = canon_lock_name(dim0, the_sha);

	//os << "lk_nm=" << lk_nm << bj_eol;
	//os << "sz_lk_nm=" << lk_nm.size() << bj_eol;
}

//=================================

void
test_row_reduc(){
	bj_ostream& os = bj_out;
	MARK_USED(os);

	row<long> rr1;
	row<long> rr2;

	rr1 << (long)14 << (long)34 << (long)54 << (long)15 << (long)67 << (long)78 << (long)98 << (long)13 
		<< (long)45 << (long)93;
	rr2 << (long)15 << (long)67 << (long)78 << (long)13 
		<< (long)45 << (long)93;

	rr1.mix_sort(cmp_long);
	rr2.mix_sort(cmp_long);

	os << "rr1=" << rr1 << bj_eol;
	os << "rr2=" << rr2 << bj_eol;

	rr1.sorted_set_reduce(rr2, cmp_long);

	os << "AFTER" << bj_eol;
	os << "rr1=" << rr1 << bj_eol;
	os << "rr2=" << rr2 << bj_eol;

}

void
test_num1(){
	bj_ostream& os = bj_out;
	MARK_USED(os);

	bj_big_int_t num_vnts;

	num_vnts = "-1";
	os << "num_vnts=" << num_vnts << bj_eol;

	num_vnts = "987654321";
	os << "num_vnts=" << num_vnts << bj_eol;
}


#define SOR_NUM_ASSIGS		1000
#define SOR_MAX_VARS_ROW	5
#define SOR_MAX_VARS_TEST	5

void	test_sorted_ops2(){
	bj_ostream& os = bj_out;
	os << "test_sorted_ops2" << bj_eol;

	unsigned long init_val = 10;
	tak_mak gg(init_val);

	row_long_t orig_rr;
	row_long_t tmp_rr;
	row_long_t to_red;
	row_long_t diff_rr;
	row_long_t shrd_rr;

	orig_rr.set_cap((SOR_MAX_VARS_TEST * 2) + 4);
	tmp_rr.set_cap((SOR_MAX_VARS_TEST * 2) + 4);
	to_red.set_cap((SOR_MAX_VARS_TEST * 2) + 4);
	diff_rr.set_cap((SOR_MAX_VARS_TEST * 2) + 4);
	shrd_rr.set_cap((SOR_MAX_VARS_TEST * 2) + 4);

	for(long aa = 0; aa < SOR_NUM_ASSIGS; aa++){
		orig_rr.clear();
		tmp_rr.clear();
		to_red.clear();
		diff_rr.clear();
		shrd_rr.clear();

		long num_vars = gg.gen_rand_int32_ie(2, SOR_MAX_VARS_TEST);

		gen_clause(gg, orig_rr, SOR_MAX_VARS_ROW, num_vars);

		orig_rr.copy_to(tmp_rr);
		long cc = 0;
		long num_red = gg.gen_rand_int32_ie(0, tmp_rr.size());
		for(cc = 0; cc < num_red; cc++){
			long idx_red = gg.gen_rand_int32_ie(0, tmp_rr.size());
			long alp = tmp_rr.swap_pop(idx_red);
			to_red.push(alp);
		}
		to_red.mix_sort(cmp_canon_ids);
		tmp_rr.mix_sort(cmp_canon_ids);

		orig_rr.copy_to(diff_rr);

		//os << "BEFORE" << bj_eol;
		//os << "rr1=" << rr1 << bj_eol;
		//os << "rr2=" << rr2 << bj_eol;
		//os << "to_adh=" << to_adh << bj_eol;
		//os << "to_red=" << to_red << bj_eol;

		diff_rr.sorted_set_reduce(to_red, cmp_canon_ids);

		bool eq_sh = diff_rr.equal_to(tmp_rr);
		if(! eq_sh){
			os << "FAILED=" << bj_eol;
			os << "orig_rr=" << orig_rr << bj_eol;
			os << "to_red=" << to_red << bj_eol;
			os << "diff_rr=" << diff_rr << bj_eol;
			os << "tmp_rr=" << tmp_rr << bj_eol;
		}
		TEST_CK(eq_sh);

		DBG(
			bool r_eq = false;
			cmp_is_sub cmp_val;

			cmp_val = cmp_sorted_rows<long>(to_red, orig_rr, cmp_canon_ids, r_eq);
			TEST_CK(cmp_val == k_lft_is_sub);

			cmp_val = cmp_sorted_rows<long>(diff_rr, orig_rr, cmp_canon_ids, r_eq);
			TEST_CK(cmp_val == k_lft_is_sub);
		);

		diff_rr.sorted_set_shared(shrd_rr, to_red, cmp_canon_ids);
		TEST_CK(shrd_rr.is_empty());
		
		//os << "AFTER" << bj_eol;
		//os << "rr1=" << rr1 << bj_eol;
		//os << "rr2=" << rr2 << bj_eol;
		//getchar();
		

		os << CARRIAGE_RETURN << aa;
		os.flush();
		//os << "ALL_OK" << bj_eol;
	}
	os << bj_eol << "ALL_OK" << bj_eol;

}

bj_ostr_stream&
print_hex_as_txt_2(bj_ostr_stream& os, row<uchar_t>& sha_rr){
	std::ios_base::fmtflags old_fls = os.flags();
	os.flags(std::ios::hex);
	for(int ii = 0; ii < sha_rr.size(); ii++){
		os.width(2);
		os.fill('0');
		os << (int)(sha_rr[ii]);
	}
	os.flags(old_fls);
	return os;
}

void pru_hex_as_txt(){
	row<uchar_t> sha_rr;
	tak_mak gg;
	gg.init_with_long((long)run_time());
	int max_num = 100;
	
	for(int aa = 0; aa < max_num; aa++){
		sha_rr.clear();
		for(int bb = 0; bb < 32; bb++){
			uchar_t vv = (uchar_t)gg.gen_rand_int32_ie(0, 256);
			sha_rr << vv;
		}
		BRAIN_CK(sha_rr.size() == 32);
	
		bj_ostr_stream oss;
		print_hex_as_txt_2(oss, sha_rr);
		ch_string val1 = oss.str();
		
		ch_string val2 = print_hex_as_txt(sha_rr);
				
		if(! (val1 == val2)){
			abort_func(1, "NOT EQUAL");
		}
		
		bj_out << val1 << bj_eol;
		bj_out << val2 << bj_eol;
	}
}

int	tests_main_(int argc, char** argv){
	MARK_USED(argc);
	MARK_USED(argv);
	bj_ostream& os = bj_out;
	
	//test_realpath(argc, argv);
	//test_subsets();
	//test_dims_to_path(argc, argv);
	//test_sets();
	//test_big4();
	//test_str1();
	//test_symlk(argc, argv);
	//test_tm_elapsed(argc, argv);
	//test_open_unlink(argc, argv);
	//test_row_reduc();
	//test_sorted_ops2();
	//test_lk_name();
	//test_num1();
	//test_skl();  // last before ben-jose
	
	test_thrw_obj();
	//pru_hex_as_txt();
	
	os.flush();

	os << bj_eol;
	os << "End of tests" << bj_eol;
	//getchar();

	return 0;
}

class test_excep {
private:
	test_excep(test_excep& other){ 
		char* te_ex_bad_creat = 
			as_pt_char("creator test_excep with test_excep not allowed");
		abort_func(0, te_ex_bad_creat);
	}
public:
	ch_string ex_nm;
	long ex_id;
	test_excep(char* descr = as_pt_char("TEST_EXCEP"), long the_id = 0) {
		ex_nm = descr;
		ex_id = the_id;
		bj_out << "creating test_excep" << bj_eol;
		bj_out.flush();
	}
	~test_excep(){
		bj_out << "releasing test_excep" << bj_eol;
		bj_out.flush();
	}
};

class sub_excep : public top_exception {
public:
	sub_excep(char* descr = as_pt_char("THE_SUB_EXCEP")) :
		top_exception(0)
	{
		bj_out << "creating sub_excep" << bj_eol;
		bj_out.flush();
	}
	~sub_excep(){
		bj_out << "releasing sub_excep" << bj_eol;
		bj_out.flush();
	}
};

void th_excep(){
	throw top_exception();
}

void th_sub_excep(){
	sub_excep ex1(as_pt_char("hola SUB exception"));
	throw ex1;
}

void test_thrw_obj(){
	bj_ostream& os = bj_out;
	
	try {
		th_sub_excep();
	} catch (top_exception& ex1){
		DBG(os << "DBG_STACK=" << ex1.ex_stk << bj_eol);
		os << "AFTER_RELEASE" << bj_eol;
		os.flush();
		//delete ex1;
	} catch (test_excep& ex1){
		os << "GOT TEST EXCEP=" << ex1.ex_nm << bj_eol;
		os.flush();
		//delete ex1;
	}
}


