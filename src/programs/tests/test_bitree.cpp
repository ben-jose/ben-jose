
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

bitree.cpp  

file for test and debugging bitree class.

--------------------------------------------------------------*/

#include <set>
#include <sstream>

#include "tools.h"
#include "tak_mak.h"
#include "util_funcs.h"

#define THE_TREE_CMP_FUNC cmp_mem_redblack

typedef std::set<long>	set_long_t;
typedef std::pair<std::set<long>::iterator,bool> ret_set_long_t;

#define MAX_SZ_STR	4000

//define TEST_USE_SETS 1

#ifdef TEST_USE_SETS
#define SETS_COD(prm)	prm
#define BTRS_COD(prm)	/**/
#else
#define SETS_COD(prm)	/**/
#define BTRS_COD(prm)	prm
#endif

#define BITREE_CK(prm)	DBG_CK(prm)

//======================================================================
// bitree_exception

class bitree_exception : public top_exception {
public:
	bitree_exception(char* descr = as_pt_char("undefined bitree exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

class bitree_entry;
class bitree_leaf;
class bitree_node;

//======================================================================
// bitree_entry

class bitree_entry {
public:
	typedef long bit_pos_t;
	bit_pos_t be_pos;
	
	bitree_entry(){
		be_pos = -1;
	}
	
	~bitree_entry(){
	}
	
	static
	bit_pos_t get_last_eq_pos(bit_pos_t from_pos, 
					bit_row& bits_1, bit_row& bits_2){
		if(from_pos < 0){
			from_pos = 0;
		}
		BITREE_CK(from_pos >= 0);
		int sz1 = bits_1.size();
		int sz2 = bits_2.size();
		int min_sz = (sz1 < sz2)?(sz1):(sz2);
		BITREE_CK(from_pos < min_sz);
		
		bit_pos_t ii = -1;
		for(ii = from_pos; ii < min_sz; ii++){
			BITREE_CK(bits_1.is_valid_idx(ii));
			BITREE_CK(bits_2.is_valid_idx(ii));
			bool vv1 = bits_1[ii];
			bool vv2 = bits_2[ii];
			if(vv1 != vv2){
				break;
			}
		}
		bit_pos_t lst_pos = ii - 1;
		if((lst_pos >= from_pos) && (lst_pos < min_sz)){
			BITREE_CK(bits_1[lst_pos] == bits_2[lst_pos]);
			return lst_pos;
		}
		return -1;
	}
	
	virtual
	bool add_bits(bitree_entry*& from, bit_pos_t pos, bit_row& bits_2){
		MARK_USED(pos);
		MARK_USED(bits_2);
		char* bad_bitree_add = as_pt_char("invalid call to bitree_entry::add_bits");
		DBG_THROW_CK(bad_bitree_add != bad_bitree_add);
		throw bitree_exception(bad_bitree_add);
		abort_func(0, bad_bitree_add); 
		return false;
	}
	
	virtual
	bool find_bits(bit_pos_t pos, bit_row& bits_2){
		MARK_USED(pos);
		MARK_USED(bits_2);
		char* bad_bitree_find = as_pt_char("invalid call to bitree_entry::find_bits");
		DBG_THROW_CK(bad_bitree_find != bad_bitree_find);
		throw bitree_exception(bad_bitree_find);
		abort_func(0, bad_bitree_find); 
		return false;
	}
	
	virtual
	bit_row& get_bits(){
		char* bad_bitree_val = as_pt_char("invalid call to bitree_entry::get_bits");
		DBG_THROW_CK(bad_bitree_val != bad_bitree_val);
		throw bitree_exception(bad_bitree_val);
		abort_func(0, bad_bitree_val);
		//return false;
	}

	virtual
	bitree_leaf* get_leaf(){
		char* bad_bitree_lf = as_pt_char("invalid call to bitree_entry::get_leaf");
		DBG_THROW_CK(bad_bitree_lf != bad_bitree_lf);
		throw bitree_exception(bad_bitree_lf);
		abort_func(0, bad_bitree_lf);
		return NULL_PT;
	}

	virtual
	bool check_entry(){
		char* bad_bitree_ck = as_pt_char("invalid call to bitree_entry::check_entry");
		DBG_THROW_CK(bad_bitree_ck != bad_bitree_ck);
		throw bitree_exception(bad_bitree_ck);
		abort_func(0, bad_bitree_ck);
		return false;
	}

	virtual
	bitree_node* walk_tree(row<bitree_leaf*>& all_leafs, bool with_free){
		char* bad_bitree_walk = as_pt_char("invalid call to bitree_entry::walk_tree");
		DBG_THROW_CK(bad_bitree_walk != bad_bitree_walk);
		throw bitree_exception(bad_bitree_walk);
		abort_func(0, bad_bitree_walk);
		return NULL_PT;
	}

	bitree_node* insert_node(bitree_entry*& from, bit_row& bits_2);

};

//======================================================================
// bitree_leaf

class bitree_leaf : public bitree_entry {
public:
	bool lf_reset;
	bit_row lf_bits;
	
	bitree_leaf(bit_pos_t curr_pos, bit_row& bits_2){
		be_pos = curr_pos;
		lf_reset = false;
		bits_2.swap_with(lf_bits);
		BITREE_CK(be_pos < lf_bits.size());
	}
	
	~bitree_leaf(){
	}

	static
	bitree_leaf* create_bitree_leaf(bit_pos_t curr_pos, bit_row& bits_2){
		bitree_leaf* lf = tpl_malloc<bitree_leaf>();
		new (lf) bitree_leaf(curr_pos, bits_2);
		return lf;
	}

	static void
	free_bitree_leaf(bitree_leaf* lf){
		BITREE_CK(lf != NULL_PT);
		lf->~bitree_leaf();
		tpl_free<bitree_leaf>(lf);
	}
	
	virtual
	bool add_bits(bitree_entry*& from, bit_pos_t pos, bit_row& bits_2);
	
	virtual
	bool find_bits(bit_pos_t pos, bit_row& bits_2){
		return false;
	}

	virtual
	bit_row& get_bits(){
		return lf_bits;
	}

	virtual
	bitree_leaf* get_leaf(){
		return this;
	}
	
	int size(){
		return lf_bits.size();
	}

	virtual
	bool check_entry(){
		BITREE_CK(be_pos < lf_bits.size());
		return true;
	}
	
	virtual
	bitree_node* walk_tree(row<bitree_leaf*>& all_leafs, bool with_free){
		if(! lf_reset){
			all_leafs.push(this);
			lf_reset = true;
		}
		return NULL_PT;
	}
	
	static
	long clear_leafs(row<bitree_leaf*>& all_leafs, bool with_free){
		long max_sz = 0;
		for(long aa = 0; aa < all_leafs.size(); aa++){
			bitree_leaf* lf = all_leafs[aa];
			BITREE_CK(lf != NULL_PT);
			lf->lf_reset = false;
			
			int lf_sz = lf->size();
			if(lf_sz > max_sz){
				max_sz = lf_sz;
			}
			
			if(with_free){
				bitree_leaf::free_bitree_leaf(lf);
				all_leafs[aa] = NULL_PT;
			}
		}
		if(with_free){
			all_leafs.clear(true, true);
		}
		return max_sz;
	}
};

//======================================================================
// bitree_node

class bitree_node : public bitree_entry {
public:
	bitree_entry* nd_zero;
	bitree_entry* nd_one;
	bitree_leaf* nd_leaf;
	
	bitree_node(bit_pos_t add_pos){
		init_bitree_node(add_pos);
	}
	
	~bitree_node(){
	}

	void init_bitree_node(bit_pos_t add_pos){
		be_pos = add_pos;
		
		nd_zero = NULL_PT;
		nd_one = NULL_PT;		
		nd_leaf = NULL_PT;
	}
	
	int diff_pos(){
		return (be_pos + 1);
	}
	
	bool has_leaf(){
		return (nd_leaf != NULL_PT);
	}
	
	bool is_final(bitree_leaf* the_lf){
		return ((the_lf != NULL_PT) && (the_lf->size() == diff_pos()));
	}
	
	bool has_final(){
		return is_final(nd_leaf);
	}
	
	bool leaf_is_deeper(){
		if(! has_leaf()){ return true; }
		
		bitree_entry* sd = get_bits_side();
		if(sd == NULL_PT){ return false; }

		int lf_sz = nd_leaf->size();
		if(lf_sz <= sd->be_pos){ return false; }
		
		return true;
	}
	
	void set_leaf(bitree_leaf* the_lf){
		BITREE_CK(! has_leaf() || is_final(the_lf));
		BITREE_CK(has_leaf() || leaf_is_deeper());
		nd_leaf = the_lf;
		BITREE_CK(has_leaf());
	}
	
	bitree_entry*& get_side(bool bit_val){
		if(bit_val){
			return nd_one;
		} else {
			return nd_zero;
		}
	}
	
	bitree_entry* get_bits_side(){
		if(has_final()){
			return NULL_PT;
		}
		bool hx2 = get_bits()[diff_pos()];
		bitree_entry* sd = get_side(hx2);
		return sd;
	}
	
	static
	bitree_node* create_bitree_node(bit_pos_t add_pos){
		bitree_node* the_nd = tpl_malloc<bitree_node>();
		new (the_nd) bitree_node(add_pos);
		return the_nd;
	}

	static void
	free_bitree_node(bitree_node* nd){
		BITREE_CK(nd != NULL_PT);
		nd->~bitree_node();
		tpl_free<bitree_node>(nd);
	}
	
	virtual
	bool add_bits(bitree_entry*& from, bit_pos_t curr_leq_pos, bit_row& bits_2);
	
	virtual
	bool find_bits(bit_pos_t pos, bit_row& bits_2){
		return false;
	}
	
	virtual
	bit_row& get_bits(){
		BITREE_CK(nd_leaf != NULL_PT);
		return nd_leaf->lf_bits;
	}

	virtual
	bitree_leaf* get_leaf(){
		return nd_leaf;
	}

	virtual
	bool check_entry(){
		BITREE_CK(nd_leaf != NULL_PT);
		BITREE_CK(be_pos < get_bits().size());
		if(nd_zero != NULL_PT){
			BITREE_CK(be_pos < nd_zero->be_pos);
		}
		if(nd_one != NULL_PT){
			BITREE_CK(be_pos < nd_one->be_pos);
		}
		return true;
	}
	
	virtual
	bitree_node* walk_tree(row<bitree_leaf*>& all_leafs, bool with_free){
		if(has_final()){
			all_leafs.push(nd_leaf);
		}
		bitree_node* nd1 = nd_zero->walk_tree(all_leafs, with_free);
		bitree_node* nd2 = nd_one->walk_tree(all_leafs, with_free);
		if(with_free){
			if(nd1 != NULL_PT){
				bitree_node::free_bitree_node(nd1);
			}
			if(nd2 != NULL_PT){
				bitree_node::free_bitree_node(nd2);
			}
			init_bitree_node(-2);
		}
		return this;
	}
};

bitree_node*
bitree_entry::insert_node(bitree_entry*& from, bit_row& bits_2){
	BITREE_CK(from == this);
	
	bit_row& bits_1 = get_bits();
	int sz1 = bits_1.size();
	int sz2 = bits_2.size();
	
	if(sz2 == 0){
		return NULL_PT;
	}
	
	bit_pos_t lst_eq = get_last_eq_pos(-1, bits_1, bits_2);
	int fst_diff = lst_eq + 1;
	
	if((sz1 == sz2) && (lst_eq == (sz1 - 1))){
		return NULL_PT;
	}
	
	bitree_leaf* nw_lf = bitree_leaf::create_bitree_leaf(fst_diff, bits_2);
	bitree_node* nw_from = bitree_node::create_bitree_node(lst_eq);
	
	if(nw_from->is_final(nw_lf)){
		BITREE_CK(! nw_from->has_final());
		nw_from->set_leaf(nw_lf);
	} else if(sz1 > sz2){
		nw_from->set_leaf(get_leaf());
	} else {
		nw_from->set_leaf(nw_lf);
	}
	
	BITREE_CK(fst_diff >= 0);
	if(fst_diff < bits_1.size()){
		bool hx1 = bits_1[fst_diff];
		bitree_entry*& sd = nw_from->get_side(hx1);
		BITREE_CK(sd == NULL_PT);
		sd = this;
	}
	if(fst_diff < bits_2.size()){
		bool hx2 = bits_2[fst_diff];
		bitree_entry*& sd = nw_from->get_side(hx2);
		BITREE_CK(sd == NULL_PT);
		sd = nw_lf;
	}
	
	//bj_ostream& oo = (bj_out << "HOLA" << bj_eol);
	
	BITREE_CK(from->be_pos < nw_from->be_pos);
	from = nw_from;
	return nw_from;
}

bool 
bitree_leaf::add_bits(bitree_entry*& from, bit_pos_t curr_leq_pos, bit_row& bits_2){
	BITREE_CK(from == this);
	BITREE_CK(curr_leq_pos < bits_2.size());

	bitree_node* nw_nd = insert_node(from, bits_2);
	if(nw_nd == NULL_PT){
		return false;
	}
	
	BITREE_CK(nw_nd != NULL_PT);
	be_pos = nw_nd->diff_pos();
	BITREE_CK(be_pos <= lf_bits.size());
	BITREE_CK(nw_nd->check_entry());
	return true;
}

bool 
bitree_node::add_bits(bitree_entry*& from, bit_pos_t curr_leq_pos, bit_row& bits_2){
	BITREE_CK(from == this);
	BITREE_CK(curr_leq_pos < bits_2.size());
	
	if(curr_leq_pos <= be_pos){
		bit_row& bits_1 = get_bits();
		BITREE_CK(be_pos < bits_1.size());
		curr_leq_pos = get_last_eq_pos(curr_leq_pos, bits_1, bits_2);
	}
	
	if(curr_leq_pos < be_pos){
		BITREE_CK(check_entry());
		bitree_node* nw_nd = insert_node(from, bits_2);
		bool nd_ok = (nw_nd != NULL_PT);
		BITREE_CK(! nd_ok || (nw_nd->check_entry()));
		return nd_ok;
	}
	
	int dff_pos = diff_pos();
	BITREE_CK(dff_pos <= bits_2.size());
	
	bool add_ok = true;
	if(dff_pos < bits_2.size()){
		bool hx1 = bits_2[dff_pos];
		bitree_entry*& ee = get_side(hx1);
		if(ee == NULL_PT){
			BITREE_CK(curr_leq_pos == be_pos);
			ee = bitree_leaf::create_bitree_leaf(dff_pos, bits_2);
		} else {
			BITREE_CK(be_pos < ee->be_pos);
			add_ok = ee->add_bits(ee, curr_leq_pos, bits_2);
		}
	}
	if(dff_pos == bits_2.size()){
		if(! has_final()){
			bitree_leaf* nw_lf = bitree_leaf::create_bitree_leaf(dff_pos, bits_2);
			set_leaf(nw_lf);
			BITREE_CK(get_bits().equal_to(bits_2));
			BITREE_CK(has_final());
		}
	}
	return add_ok;
}

//======================================================================
// bitree

class bitree {
	bitree_entry* bt_head;
	long bt_sz;
public:
	
	bitree(){
		init_bitree();
	}
	
	~bitree(){
	}
	
	void init_bitree(){
		bt_head = NULL_PT;
		bt_sz = 0;
	}
	
	void add_bits(bit_row& bits_2){
		if(bt_head == NULL_PT){
			bt_head = bitree_leaf::create_bitree_leaf(-1, bits_2);
			bt_sz++;
		} else {
			bool add_ok = bt_head->add_bits(bt_head, -1, bits_2);
			if(add_ok){
				bt_sz++;
			}
		}
	}
	
	long size(){
		return bt_sz;
	}
	
	bool find_bits(bit_row& bits_2){
		return false;
	}
	
	long walk_bitree(row<bitree_leaf*>& all_leafs, bool with_free){
		all_leafs.clear();
		if(bt_head == NULL_PT){
			return 0;
		}
		bitree_node* nd1 = bt_head->walk_tree(all_leafs, with_free);
		long max_sz = bitree_leaf::clear_leafs(all_leafs, with_free);
		if(with_free){
			if(nd1 != NULL_PT){
				bitree_node::free_bitree_node(nd1);
			}
			init_bitree();
		}
		return max_sz;
	}
	
	bj_ostream&	print(bj_ostream& os){
		if(bt_head == NULL_PT){
			os << "NULL_BITREE" << bj_eol;;
			return os;
		}
		row<bitree_leaf*> all_leafs(bt_sz);
		long max_sz = walk_bitree(all_leafs, false);
		for(long aa = 0; aa < all_leafs.size(); aa++){
			bit_row& br = all_leafs[aa]->get_bits();
			long df_sz = max_sz - br.size();
			for(long bb = 0; bb < df_sz; bb++){
				os << " ";
			}
			os << br << bj_eol;;
		}
		return os;
	}

	void reset(){
		if(bt_head == NULL_PT){
			BITREE_CK(bt_sz == 0);
			return;
		}
		BITREE_CK(bt_sz != 0);
		
		row<bitree_leaf*> all_leafs(bt_sz);
		walk_bitree(all_leafs, true);
	}
	
	bool is_empty(){
		bool is_e = (bt_head == NULL);
		BITREE_CK(is_e == (bt_sz == 0));
		return is_e;
	}
	
	bool check_tree(){
		if(bt_head == NULL){
			BITREE_CK(is_empty());
			return true;
		}
		BITREE_CK(bt_head->check_entry());
		return true;
	}
	
};

void set_random_bits(tak_mak& rnd_gen, bit_row& br){
	long max_row_sz = 15;
	
	br.clear();
	long sz_rr = rnd_gen.gen_rand_int32_ie(2, max_row_sz);
	for(long aa = 0; aa < sz_rr; aa++){
		long val_bt = rnd_gen.gen_rand_int32_ie(0, 2);
		if(val_bt == 0){
			br.push(false);
		} else {
			br.push(true);
		}
	}
}

#define NUM_TESTS 3
#define MAX_SZ_BTR 10

void	test_bitree(){
	bj_ostream& os = bj_out;
	BTRS_COD(bitree rr);
	SETS_COD(set_long_t ss);
	ret_set_long_t rsl;

	os << "test_bitree" << bj_eol;
	BTRS_COD(os << "using_btrs" << bj_eol);
	SETS_COD(os << "USING_SETS" << bj_eol);
	os << "MAX_SZ_BTR=" << MAX_SZ_BTR << bj_eol;

	average		avg_sz;

	long rr_tm = 20;
	tak_mak rnd_gen(rr_tm);
	
	long num_tests = NUM_TESTS;

	long min_sz = -1;
	long max_sz = -1;
	
	double start_tm = run_time();
	
	bit_row br;

	for(long jj = 0; jj < num_tests; jj++){
		
		long sz_btr = rnd_gen.gen_rand_int32_ie(0, MAX_SZ_BTR);
			
		if((min_sz == -1) || (sz_btr < min_sz)){
			min_sz = sz_btr;
		}
		if((max_sz == -1) || (sz_btr > max_sz)){
			max_sz = sz_btr;
		}

		//long max_item = rnd_gen.gen_rand_int32_ie(sz_btr, 3 * sz_btr);
		
		BTRS_COD(rr.reset());

		BITREE_CK(rr.is_empty());
		
		os << "sz_btr=" << sz_btr << bj_eol;;
		// fill btr.
		for(long kk = 0; kk < sz_btr; kk++){
			set_random_bits(rnd_gen, br);

			os << "adding=" << br << bj_eol;
			
			BITREE_CK(rr.check_tree());
			BTRS_COD(rr.add_bits(br));
			SETS_COD(
				rsl = ss.insert(nxt_elem);
				ok = rsl.second;
			)
			/*
			if((kk % 1000) == 0){
				os << CARRIAGE_RETURN;
				BTRS_COD(os << rr.size());
				SETS_COD(os << ss.size());
			}
			*/
		}
		
		os << CARRIAGE_RETURN;
		os << as_pt_char("                     ");
		os << CARRIAGE_RETURN;
		BTRS_COD(os << rr.size());
		SETS_COD(os << ss.size());
		os.flush();
		
		BITREE_CK(rr.check_tree());

		BTRS_COD(avg_sz.add_val((double)rr.size()));
		SETS_COD(avg_sz.add_val((double)ss.size()));

		BITREE_CK(rr.check_tree());
		os << CARRIAGE_RETURN;
		os << jj;
		rr.print(os);

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
	//os << "dbg_mem_at_start0=" << GLB().dbg_mem_at_start << bj_eol;

	os << bj_eol;
	
	test_bitree();

	os << "sizeof(bitree_entry)=" << sizeof(bitree_entry) << bj_eol;
	os << "sizeof(bitree_leaf)=" << sizeof(bitree_leaf) << bj_eol;
	os << "sizeof(bitree_node)=" << sizeof(bitree_node) << bj_eol;
	os << "sizeof(void*)=" << sizeof(void*) << bj_eol;
	os << "sizeof(int)=" << sizeof(int) << bj_eol;
	os << "sizeof(long)=" << sizeof(long) << bj_eol;
	os << "sizeof(long long)=" << sizeof(long long) << bj_eol;
	
	os << "End of test-bitree. Type ENTER." << bj_eol;
	return 0;
}


