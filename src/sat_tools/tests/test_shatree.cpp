
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

shatree.cpp  

file for test and debugging shatree class.

--------------------------------------------------------------*/

#include <set>
#include <sstream>

#include "tools.h"
#include "tak_mak.h"
#include "support.h"


#define THE_TREE_CMP_FUNC cmp_mem_redblack

typedef std::set<long>	set_long_t;
typedef std::pair<std::set<long>::iterator,bool> ret_set_long_t;

#define NUM_TESTS 30
#define MAX_SZ_STR	4000

#define TEST_USE_SETS 1

#ifdef TEST_USE_SETS
#define SETS_COD(prm)	prm
#define SHTS_COD(prm)	/**/
#else
#define SETS_COD(prm)	/**/
#define SHTS_COD(prm)	prm
#endif

#define SHAT_ENTRY_SZ 16

#define SHATREE_CK(prm)	DBG_CK(prm)

#define NUM_BYTES_SHA2	32	// 256 bits
#define NUM_HEX_SHA2	(NUM_BYTES_SHA2 * 2)	// 256 bits

typedef unsigned char uchar_t;

//======================================================================
// shat_exception

class shat_exception : public top_exception {
public:
	shat_exception(char* descr = as_pt_char("undefined shatree exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

//======================================================================
// shat_entry

class shat_entry {
public:
	typedef char hex_pos_t;
	hex_pos_t en_pos;
	
	shat_entry(){
		en_pos = -1;
	}
	
	~shat_entry(){
	}
	
	static
	int get_pos_hx(hex_pos_t pos, row<uchar_t>& sha_rr){
		SHATREE_CK(pos >= 0);
		SHATREE_CK(pos < NUM_HEX_SHA2);
		int sh_pos = (pos / 2);
		uchar_t sh_vv = sha_rr[sh_pos];
		int sh_ch = (pos % 2);
		
		int p_val = -1;
		if(sh_ch == 0){
			p_val = lo_hex_as_int(sh_vv);
		} else {
			p_val = hi_hex_as_int(sh_vv);
		}
		SHATREE_CK(p_val >= 0);
		SHATREE_CK(p_val < SHAT_ENTRY_SZ);
		return p_val;
	}

	static
	hex_pos_t get_first_diff_pos(hex_pos_t from_pos, 
					row<uchar_t>& sha_rr1, row<uchar_t>& sha_rr2){
		SHATREE_CK(from_pos >= 0);
		SHATREE_CK(from_pos < NUM_HEX_SHA2);
		SHATREE_CK(sha_rr1.size() == NUM_BYTES_SHA2);
		SHATREE_CK(sha_rr2.size() == NUM_BYTES_SHA2);
		
		for(hex_pos_t ii = from_pos; ii < NUM_HEX_SHA2; ii++){
			int vv1 = get_pos_hx(ii, sha_rr1);
			int vv2 = get_pos_hx(ii, sha_rr2);
			if(vv1 != vv2){
				return ii;
			}
		}
		return -1;
	}
	
	virtual
	void reset(){
		char* bad_shat_reset = as_pt_char("invalid call to shat_entry::reset");
		DBG_THROW_CK(bad_shat_reset != bad_shat_reset);
		throw shat_exception(bad_shat_reset);
		abort_func(0, bad_shat_reset); 
	}
	
	virtual
	void add_sha(shat_entry*& from, hex_pos_t pos, row<uchar_t>& sha_rr){
		MARK_USED(pos);
		MARK_USED(sha_rr);
		char* bad_shat_add = as_pt_char("invalid call to shat_entry::add");
		DBG_THROW_CK(bad_shat_add != bad_shat_add);
		throw shat_exception(bad_shat_add);
		abort_func(0, bad_shat_add); 
	}
	
	virtual
	bool find_sha(hex_pos_t pos, row<uchar_t>& sha_rr){
		MARK_USED(pos);
		MARK_USED(sha_rr);
		char* bad_shat_find = as_pt_char("invalid call to shat_entry::find_sha");
		DBG_THROW_CK(bad_shat_find != bad_shat_find);
		throw shat_exception(bad_shat_find);
		abort_func(0, bad_shat_find); 
		return false;
	}
	
	virtual
	row<uchar_t>& get_sha_val(){
		char* bad_shat_val = as_pt_char("invalid call to shat_entry::get_sha_val");
		DBG_THROW_CK(bad_shat_val != bad_shat_val);
		throw shat_exception(bad_shat_val);
		abort_func(0, bad_shat_val);
		//return false;
	}

	hex_pos_t pre_add_node(shat_entry*& from, row<uchar_t>& sha_rr);
	
	virtual
	bj_ostream&	print(bj_ostream& os, bool just_cks = false){
		char* bad_shat_prt = as_pt_char("invalid call to shat_entry::print");
		DBG_THROW_CK(bad_shat_prt != bad_shat_prt);
		throw shat_exception(bad_shat_prt);
		abort_func(0, bad_shat_prt);
		return os;
	}
};

//======================================================================
// shat_leaf

class shat_leaf : public shat_entry {
public:
	long lf_num_shares;
	row<uchar_t> lf_sha_val;
	
	shat_leaf(hex_pos_t curr_pos, row<uchar_t>& sha_rr){
		en_pos = curr_pos;
		lf_num_shares = 1;
		sha_rr.copy_to(lf_sha_val);
	}
	
	~shat_leaf(){
	}

	static
	shat_leaf* create_shat_leaf(hex_pos_t curr_pos, row<uchar_t>& sha_rr){
		shat_leaf* lf = tpl_malloc<shat_leaf>();
		new (lf) shat_leaf(curr_pos, sha_rr);
		return lf;
	}

	static void
	free_shat_leaf(shat_leaf* lf){
		SHATREE_CK(lf != NULL_PT);
		lf->~shat_leaf();
		tpl_free<shat_leaf>(lf);
	}
	
	virtual
	void reset(){
	}
	
	virtual
	void add_sha(shat_entry*& from, hex_pos_t pos, row<uchar_t>& sha_rr);
	
	virtual
	bool find_sha(hex_pos_t pos, row<uchar_t>& sha_rr){
		return false;
	}

	virtual
	row<uchar_t>& get_sha_val(){
		return lf_sha_val;
	}

	virtual
	bj_ostream&	print(bj_ostream& os, bool just_cks = false){
		return os;
	}
};

//======================================================================
// shat_node

class shat_node : public shat_entry {
public:
	shat_entry* nd_hx_arr[SHAT_ENTRY_SZ];
	shat_leaf* nd_sha_lf;
	
	shat_node(hex_pos_t add_pos, shat_leaf* lf){
		init_shat_node(add_pos, lf);
	}
	
	~shat_node(){
	}

	void init_shat_node(hex_pos_t add_pos, shat_leaf* the_lf){
		en_pos = add_pos;
		for(int ii = 0; ii < SHAT_ENTRY_SZ; ii++){
			nd_hx_arr[ii] = NULL_PT;
		}
		SHATREE_CK(the_lf != NULL_PT);
		nd_sha_lf = the_lf;
		(nd_sha_lf->lf_num_shares)++;
	}
	
	static
	shat_node* create_shat_node(hex_pos_t add_pos, shat_leaf* the_lf){		
		shat_node* the_nd = tpl_malloc<shat_node>();
		new (the_nd) shat_node(add_pos, the_lf);
		return the_nd;
	}

	static void
	free_shat_node(shat_node* nd){
		SHATREE_CK(nd != NULL_PT);
		nd->~shat_node();
		tpl_free<shat_node>(nd);
	}
	
	virtual
	void reset(){
	}
	
	virtual
	void add_sha(shat_entry*& from, hex_pos_t add_pos, row<uchar_t>& sha_rr);
	
	virtual
	bool find_sha(hex_pos_t pos, row<uchar_t>& sha_rr){
		return false;
	}
	
	virtual
	row<uchar_t>& get_sha_val(){
		SHATREE_CK(nd_sha_lf != NULL_PT);
		return nd_sha_lf->lf_sha_val;
	}

	bool check_node(bool full_diff = false){
		if(!full_diff && (en_pos == 0)){
			return true;
		}
		row<uchar_t>& sha_vv = get_sha_val();

		
		hex_pos_t eq_pos = 0;
		int hx1 = -1;
		if(! full_diff){
			eq_pos = en_pos - 1;
			hx1 = get_pos_hx(eq_pos, sha_vv);
		}
		
		for(int ii = 0; ii < SHAT_ENTRY_SZ; ii++){
			shat_entry* ee = nd_hx_arr[ii];
			if(ee != NULL_PT){
				row<uchar_t>& sha_rr = ee->get_sha_val();
				if(full_diff){
					hex_pos_t fst_diff = get_first_diff_pos(0, sha_vv, sha_rr);
					if(fst_diff != en_pos){
						return false;
					}
				} else {
					SHATREE_CK(hx1 != -1);
					int hx2 = get_pos_hx(eq_pos, sha_rr);
					if(hx2 != hx1){
						return false;
					}
				}
			}
		}
		return true;
	}
	
	virtual
	bj_ostream&	print(bj_ostream& os, bool just_cks = false){
		return os;
	}
};

shat_entry::hex_pos_t
shat_entry::pre_add_node(shat_entry*& from, row<uchar_t>& sha_rr){
	SHATREE_CK(from == this);
	
	row<uchar_t>& sha_vv = get_sha_val();
	hex_pos_t fst_diff = get_first_diff_pos(0, sha_vv, sha_rr);
	
	shat_leaf* nw_lf = shat_leaf::create_shat_leaf(fst_diff, sha_rr);
	shat_node* nw_from = shat_node::create_shat_node(fst_diff, nw_lf);
	
	int hx1 = get_pos_hx(fst_diff, sha_vv);
	int hx2 = get_pos_hx(fst_diff, sha_rr);
	
	(nw_from->nd_hx_arr)[hx1] = this;
	(nw_from->nd_hx_arr)[hx2] = nw_lf;

	SHATREE_CK(nw_from->check_node(true));
	SHATREE_CK(from->en_pos < nw_from->en_pos);
	from = nw_from;
	return fst_diff;
}

void 
shat_leaf::add_sha(shat_entry*& from, hex_pos_t add_pos, row<uchar_t>& sha_rr){
	SHATREE_CK(from == this);

	hex_pos_t fst_diff = pre_add_node(from, sha_rr);
	
	SHATREE_CK(fst_diff >= en_pos);
	en_pos = fst_diff;
}

void 
shat_node::add_sha(shat_entry*& from, hex_pos_t add_pos, row<uchar_t>& sha_rr){
	SHATREE_CK(from == this);
	
	if(add_pos < en_pos){
		SHATREE_CK(check_node(true));
		pre_add_node(from, sha_rr);
		return;
	}
	
	row<uchar_t>& sha_vv = get_sha_val();
	if(add_pos == en_pos){
		add_pos = get_first_diff_pos(en_pos, sha_vv, sha_rr);
	}

	int hx1 = get_pos_hx(en_pos, sha_rr);
	shat_entry*& ee = nd_hx_arr[hx1];
	if(ee == NULL_PT){
		nd_hx_arr[hx1] = shat_leaf::create_shat_leaf(add_pos, sha_rr);
	} else {
		ee->add_sha(ee, add_pos, sha_rr);
	}
}

//======================================================================
// shatree

class shatree {
	shat_entry* st_head;
public:
	shatree(){
		st_head = NULL_PT;
	}
	
	~shatree(){
	}
	
	void reset(){
	}
	
	void add_sha(row<uchar_t>& sha_rr){
		if(st_head == NULL_PT){
			st_head = shat_leaf::create_shat_leaf(0, sha_rr);
		} else {
			st_head->add_sha(st_head, 0, sha_rr);
		}
	}
	
	bool find_sha(row<uchar_t>& sha_rr){
		return false;
	}
};

void
print_all_sha_chars(){
	bj_ostr_stream os;
	
	//unsigned char ii = 0;
	
	std::ios_base::fmtflags old_fls = os.flags();
	os.flags(std::ios::hex);
	for(int ii = -1; ii <= 256; ii++){
		unsigned char ch = (unsigned char)ii;
		os.width(2);
		os.fill('0');
		os << (int)(ch);
	}
	os.flags(old_fls);
	
	bj_out << os.str();
	bj_out << bj_eol;
}

void
check_hex_txt_funcs_with(row<uchar_t>& sha_rr){
	SHATREE_CK(sha_rr.size() == NUM_BYTES_SHA2);
	ch_string str1 = print_hex_as_txt(sha_rr);
	
	row<char> hex_str;
	sha_rr.as_hex_txt(hex_str);
	hex_str.push(0);
	
	ch_string str2(hex_str.get_c_array());
	
	if(str1 != str2){
		ch_string msg = as_pt_char("str1=") + str1 + as_pt_char(" !=\nstr2=") + str2;
		abort_func(0, msg.c_str());
	}
	//bj_out << "str1=" << str1 << bj_eol;
	//bj_out << "str2=" << str2 << bj_eol;
}

void
full_check_hex_txt_funcs(){
	//long mem1 = MEM_STATS.num_bytes_in_use;
	//bj_out << "mem1=" << mem1 << bj_eol;
	//bj_out << "dbg_mem_at_start1=" << GLB().dbg_mem_at_start << bj_eol;
	
	long rr_tm = 20;
	tak_mak rnd_gen(rr_tm);
	
	row<uchar_t> sha_rr;
	
	long num_tests = 10;
	
	for(long aa = 0; aa < num_tests; aa++){
		sha_rr.clear();
		for(long bb = 0; bb < NUM_BYTES_SHA2; bb++){
			long l_vv = rnd_gen.gen_rand_int32_ie(0, 256);
			uchar_t vv = (uchar_t)l_vv;
			sha_rr.push(vv);
		}
		check_hex_txt_funcs_with(sha_rr);
		
		bj_out << CARRIAGE_RETURN;
		bj_out << aa;
	}
	bj_out << bj_eol;
	sha_rr.clear(true, true);

	//long mem2 = MEM_STATS.num_bytes_in_use;
	//bj_out << "mem2=" << mem2 << bj_eol;
	//bj_out << "dbg_mem_at_start2=" << GLB().dbg_mem_at_start << bj_eol;
}

int	
main(int argc, char** argv){
	bj_ostream& os = bj_out;
	os << "dbg_mem_at_start0=" << GLB().dbg_mem_at_start << bj_eol;

	//print_all_sha_chars();
	full_check_hex_txt_funcs();
	
	/*
	os << bj_eol;

	os << "sizeof(shat_entry)=" << sizeof(shat_entry) << bj_eol;
	os << "sizeof(shat_leaf)=" << sizeof(shat_leaf) << bj_eol;
	os << "sizeof(shat_node)=" << sizeof(shat_node) << bj_eol;
	os << "sizeof(void*)=" << sizeof(void*) << bj_eol;
	os << "sizeof(int)=" << sizeof(int) << bj_eol;
	os << "sizeof(long)=" << sizeof(long) << bj_eol;
	os << "sizeof(long long)=" << sizeof(long long) << bj_eol;
	
	*/
	os << "End of test-shatree. Type ENTER." << bj_eol;
	//os << "dbg_mem_at_start3=" << GLB().dbg_mem_at_start << bj_eol;
	return 0;
}


