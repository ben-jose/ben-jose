


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

sortor.h  

classes to implement a sortor.

--------------------------------------------------------------*/


#ifndef SORTOR_H
#define SORTOR_H

#include "tools.h"
#include "print_macros.h"
#include "skeleton.h"

#define DBG_SORTOR_PRT(prm1, prm2)	;

#define SORTER_DBG(prm)	DBG(prm)
#define SORTER_CK(prm) 	DBG_CK(prm)
#define SORTER_CK_1(prm) 	DBG_CK(prm)
#define SORTER_CK_2(prm) 	DBG_CK(prm)

#define PHASE_1_COMMENT	"phase_one\n"
#define PHASE_2_COMMENT	"phase_two\n"
#define PHASE_3_COMMENT	"phase_three\n"

#define FINAL_COMMENT	"final\n"
#define MUTUAL_COMMENT	"mutual_stab\n"

class brain;
class skeleton_glb;

class binder;
class sortee;
class sorset;
class sort_glb;

//=================================================================
// type defs

typedef bj_big_int_t	sort_id_t;
typedef row<sort_id_t> 	row_sort_id_t;


//=================================================================
// funtion declarations

sortee&		as_sortee(binder* bdr);

template<class obj_t1>
obj_t1&		as_srt_of(binder* bdr);


//=================================================================
// printing declarations

DECLARE_PRINT_FUNCS(sortee)
DECLARE_PRINT_FUNCS(sorset)

//=================================================================
// binder

class binder {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return binder::CL_NAME;
	}

	binder*		bn_left;
	binder*		bn_right;

	binder(){
		init_binder();
	}

	~binder(){
	}

	void		init_binder(){
		bn_left = this;
		bn_right = this;
	}

	bool	is_alone(){
		return ((bn_left == this) && (bn_right == this));
	}

	virtual
	void	let_go(){
		bn_left->bn_right = bn_right;
		bn_right->bn_left = bn_left;
		bn_left = this;
		bn_right = this;
	}

	bool	ck_binder(){
		SORTER_CK_2(bn_right->bn_left == this);
		SORTER_CK_2(bn_left->bn_right == this);
		return true;
	}

	void	bind_to_my_right(binder& the_rgt){
		SORTER_CK_2(the_rgt.is_alone());
		SORTER_CK_2(ck_binder());

		the_rgt.bn_right = bn_right;
		the_rgt.bn_left = this;
		bn_right->bn_left = &the_rgt;
		bn_right = &the_rgt;

		SORTER_CK_2(the_rgt.ck_binder());
		SORTER_CK_2(ck_binder());
	}

	void	bind_to_my_left(binder& the_lft){
		SORTER_CK_2(the_lft.is_alone());
		SORTER_CK_2(ck_binder());

		the_lft.bn_left = bn_left;
		the_lft.bn_right = this;
		bn_left->bn_right = &the_lft;
		bn_left = &the_lft;

		SORTER_CK_2(the_lft.ck_binder());
		SORTER_CK_2(ck_binder());
	}

	bool	is_grip();
	bool	is_leftmost();
	bool	is_rightmost();

	bool	is_last_in_grip(){
		return (is_leftmost() && is_rightmost());
	}
};

template<class obj_t1>
obj_t1&
rcp_as(binder* bdr);


//=================================================================
// grip

class grip : public binder {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return grip::CL_NAME;
	}

	virtual
	void	let_go(){
		SORTER_CK_2(false);
	}

	bool	is_single(){
		return (! is_alone() && (bn_left == bn_right));
	}

	bool	is_multiple(){
		return (! is_alone() && ! is_single());
	}

	void	move_all_to_my_right(grip& grp){
		if(grp.is_alone()){
			return;
		}

		SORTER_CK_2(ck_binder());

		binder* new_rgt = grp.bn_right;
		binder* new_mid = grp.bn_left;
		binder* old_rgt = bn_right;

		grp.bn_right = &grp;
		grp.bn_left = &grp;

		bn_right = new_rgt;
		bn_right->bn_left = this;

		new_mid->bn_right = old_rgt;
		new_mid->bn_right->bn_left = new_mid;

		SORTER_CK_2(grp.is_alone());
		SORTER_CK_2(new_rgt->ck_binder());
		SORTER_CK_2(new_mid->ck_binder());
		SORTER_CK_2(old_rgt->ck_binder());
		SORTER_CK_2(ck_binder());
	}

	void	move_all_to_my_left(grip& grp){
		if(grp.is_alone()){
			return;
		}

		SORTER_CK_2(ck_binder());

		binder* new_lft = grp.bn_left;
		binder* new_mid = grp.bn_right;
		binder* old_lft = bn_left;

		grp.bn_right = &grp;
		grp.bn_left = &grp;

		bn_left = new_lft;
		bn_left->bn_right = this;

		new_mid->bn_left = old_lft;
		new_mid->bn_left->bn_right = new_mid;

		SORTER_CK_2(grp.is_alone());
		SORTER_CK_2(new_lft->ck_binder());
		SORTER_CK_2(new_mid->ck_binder());
		SORTER_CK_2(old_lft->ck_binder());
		SORTER_CK_2(ck_binder());
	}

	template<class obj_t1>
	void		fill_rcps_as(row<obj_t1*>& rr);
};

inline
bool
binder::is_grip(){
	bool is_g = (get_cls_name() == grip::CL_NAME);
	return is_g;
}

inline
bool
binder::is_leftmost(){
	SORTER_CK_2(! is_grip());
	return bn_left->is_grip();
}

inline
bool
binder::is_rightmost(){
	SORTER_CK_2(! is_grip());
	return bn_right->is_grip();
}

//=================================================================
// receptor

template<class obj_t1>
class receptor : public binder {
public:
	virtual
	char*	get_cls_name(){
		return obj_t1::CL_NAME;
	}

	obj_t1*		re_me;

	receptor(){
		init_receptor();
	}

	~receptor(){
	}

	void	init_receptor(){
		re_me = NULL_PT;
	}

	receptor(obj_t1& tgt){
		re_me = &tgt;
	}

	receptor(obj_t1* tgt){
		re_me = tgt;
	}

	operator obj_t1* () {
		return re_me;
	}

	operator obj_t1& () {
		SORTER_CK(re_me != NULL_PT);
		return *re_me;
	}
};

//=================================================================
// sortrel

enum tgt_ccl_t {
	tc_none = 0,
	tc_mates = 1
};

class sortrel {
public:
	row<sortee*>	so_mates;
	sortee*		so_opposite;

	sortrel(){
		init_sortrel();
	}

	~sortrel(){
	}

	void		init_sortrel(bool free_mem = false){	
		so_mates.clear(free_mem, free_mem);
		so_opposite = NULL_PT;
	}

	bool		ck_related(long tee_cons);

};


//=================================================================
// sortee

class sortee : public binder {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return sortee::CL_NAME;
	}

	void*		so_me;
	sorset*		so_vessel;
	long		so_sorset_consec;
	long		so_walk_consec;

	sortrel*	so_related;

	char		so_dbg_me_class;
	long		so_dbg_extrn_id;
	void*		so_dbg_extrn_src;

	long		so_saved_tee_consec;
	long		so_tee_consec;
	long		so_last_ccl_id;
	canon_clause	so_ccl;

	long		so_qua_id;

	charge_t	so_charge;

	sortee(){
		init_sortee();
	}

	~sortee(){
		DBG(so_ccl.cc_in_free = true);
	}

	void		init_sortee(bool free_mem = false){
		so_me = NULL_PT;
		so_vessel = NULL_PT;
		so_sorset_consec = INVALID_NATURAL;
		so_walk_consec = INVALID_NATURAL;

		so_related = NULL_PT;

		so_dbg_me_class = 0;
		so_dbg_extrn_id = 0;
		so_dbg_extrn_src = NULL_PT;

		so_saved_tee_consec = INVALID_NATURAL;
		so_tee_consec = INVALID_NATURAL;
		so_last_ccl_id = 0;
		so_ccl.cc_clear(free_mem);

		so_qua_id = 0;

		so_charge = cg_neutral;
	}

	template<class obj_t1>
	obj_t1&
	me_as(){
		SORTER_CK_1(so_me != NULL_PT);
		obj_t1& obj = *((obj_t1*)(so_me));
		SORTER_CK_1(obj.get_cls_name() == obj_t1::CL_NAME);
		return obj;
	}

	bool		is_unsorted(){
		return (is_alone() && ! has_vessel());
	}

	sorset&		get_vessel();

	bool		has_vessel(){
		return (so_vessel != NULL_PT);
	}

	void		unsort_me(sort_glb& srg);
	void		sort_me_to(sort_glb& srg, sorset& nsr);
	void		sort_from(sort_glb& srg, sort_id_t curr_nid,	void* id_src = NULL_PT);

	void		save_consec(sort_glb& srg);
	long		recover_consec(sort_glb& srg);

	long 		get_ccl_id(sort_glb& srg);
	void		close_ccl(sort_glb& srg);
	void		update_totals(sort_glb& srg, long tgt_sz);
	bool		is_confl(sort_glb& srg);
	bool		is_dual(sort_glb& srg);

	long		get_qua_id(sort_glb& srg);

	sortee&		opposite(){
		SORTER_CK(so_me != NULL_PT);
		SORTER_CK(so_related != NULL_PT);
		SORTER_CK(so_related->so_opposite);
		return *(so_related->so_opposite);
	}

	long		get_target_sz(tgt_ccl_t tt){
		SORTER_CK(so_me != NULL_PT);
		SORTER_CK(so_related != NULL_PT);

		SORTER_CK(tt != tc_none);
		long the_sz = 0;
		if(tt == tc_mates){
			the_sz = so_related->so_mates.size();
		}
		return the_sz;
	}

	void		reset_sort_info(){
		SORTER_CK(is_unsorted());
		
		so_tee_consec = 0;
		if(so_related != NULL_PT){
			sortrel& quar = *so_related;
			SORTER_CK(quar.so_opposite != NULL_PT);
			quar.so_mates.clear();
		}

		so_dbg_extrn_src = NULL_PT;
		so_ccl.cc_clear(false);
	}
	
	bj_ostream&	print_sortee(bj_ostream& os, bool from_pt = false);
};

template<class obj_t1>
bool
srt_row_as(row<sortee*>& rr1, row<obj_t1*>& rr2){
	long last_item_id = INVALID_NATURAL;
	bool all_consec = true;
	rr2.clear();
	rr2.set_cap(rr1.size());
	for(long ii = 0; ii < rr1.size(); ii++){
		sortee& srt = *(rr1[ii]);

		//all_consec = all_consec && (last_item_id != srt.so_walk_consec);
		//last_item_id = srt.so_walk_consec;

		all_consec = all_consec && (last_item_id != srt.so_sorset_consec);
		last_item_id = srt.so_sorset_consec;

		obj_t1& obj_1 = srt.me_as<obj_t1>();
		rr2.push(&obj_1);
	}
	return all_consec;
}

template<class obj_t1>
bool
srt_row_as_colors(row<sortee*>& rr1, row<obj_t1*>& rr2, row<long>& cols){
	long last_item_id = INVALID_NATURAL;
	bool all_consec = true;
	rr2.clear();
	rr2.set_cap(rr1.size());
	cols.clear();
	cols.set_cap(rr1.size());
	for(long ii = 0; ii < rr1.size(); ii++){
		sortee& srt = *(rr1[ii]);

		all_consec = all_consec && (last_item_id != srt.so_tee_consec);
		last_item_id = srt.so_tee_consec;

		cols.push(last_item_id);

		obj_t1& obj_1 = srt.me_as<obj_t1>();
		rr2.push(&obj_1);
	}
	return all_consec;
}

inline
bool
srt_row_as_clauses(row<sortee*>& rr1, row<canon_clause*>& rr2){
	long last_item_id = INVALID_NATURAL;
	bool all_consec = true;
	rr2.clear();
	rr2.set_cap(rr1.size());
	for(long ii = 0; ii < rr1.size(); ii++){
		sortee& srt = *(rr1[ii]);

		all_consec = all_consec && (last_item_id != srt.so_sorset_consec);
		last_item_id = srt.so_sorset_consec;

		rr2.push(&(srt.so_ccl));
	}
	return all_consec;
}

template<class obj_t1>
void
void_pts_row_as(row<void*>& rr1, row<obj_t1*>& rr2){
	rr2.clear();
	rr2.set_cap(rr1.size());
	for(long ii = 0; ii < rr1.size(); ii++){
		void* pt_gen_obj = rr1[ii];
		obj_t1* pt_obj = (obj_t1*)pt_gen_obj;

		SORTER_CK((pt_obj == NULL_PT) || (pt_obj->get_cls_name() == obj_t1::CL_NAME));
		rr2.push(pt_obj);
	}
}

//=================================================================
// sorset

typedef bool (sorset::*cond_mth)();

typedef receptor<sorset> ss_srs_t;

class sorset {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return sorset::CL_NAME;
	}

	sorset*		ss_parent_set;
	ss_srs_t	ss_sub_rcp;

	grip		ss_items;
	grip		ss_subsets;

	sort_id_t	ss_curr_id;
	void*		ss_id_src;
	bool		ss_has_val;

	bool		ss_mark;

	sorset(){
		init_sorset();
	}

	~sorset(){
	}

	void		init_sorset(){
		ss_parent_set = NULL_PT;
		ss_sub_rcp.re_me = this;
		SORTER_CK(ss_sub_rcp.is_alone());

		SORTER_CK(ss_items.is_alone());
		SORTER_CK(ss_subsets.is_alone());

		ss_curr_id = 0;
		ss_id_src = NULL_PT;
		ss_has_val = false;

		ss_mark = false;
	}

	void		set_ss_mark(sort_glb& srg);
	void		reset_ss_mark(sort_glb& srg);

	bool		is_ss_virgin(bool ck_items = true);

	sorset&		get_next_vessel(sort_glb& srg, sort_id_t curr_id, void* id_src);
	sorset*		last_sub();

	void		fill_srs_trails(row_sort_id_t& id_trl, row<void*>& src_trl);

	bool		has_ss_parent(){
		return (ss_parent_set != NULL_PT);
	}

	bool		has_items(){
		return ! ss_items.is_alone();
	}

	bool		has_subsets(){
		return ! ss_subsets.is_alone();
	}

	bool		has_brothers(){
		return ! ss_sub_rcp.is_alone();
	}

	bool		one_item(){
		return ss_items.is_single();
	}

	bool		is_multitem(){
		return ss_items.is_multiple();
	}

	bool		one_subset(){
		return ss_subsets.is_single();
	}

	bool		can_sort_in_me(){
		bool ok = (! has_subsets() || 
				(one_subset() && (first_subset().ss_curr_id == ss_curr_id)));
		return ok;
	}

	sorset&		first_subset(){
		SORTER_CK(has_subsets());
		sorset& fst = rcp_as<sorset>(ss_subsets.bn_right);
		return fst;
	}

	sortee&		first_item(){
		SORTER_CK(has_items());
		sortee& fst = as_sortee(ss_items.bn_right);
		return fst;
	}

	/*
	bool		is_leaf(){
		bool is_lf = ! has_subsets();
		return is_lf;
	}*/

	bool		is_single_leaf(){
		return (! has_subsets() && ss_items.is_single());
	}

	void		set_item_id(sort_id_t the_val, void* val_src){
		SORTER_CK(the_val > 0);
		SORTER_CK(! ss_has_val);
		ss_has_val = true;
		ss_curr_id = the_val;
		ss_id_src = val_src;
	}

	bool		in_item_id(sort_id_t target_id){
		if(! ss_has_val){
			return false;
		}
		SORTER_CK(target_id > 0);
		SORTER_CK(ss_curr_id != 0);
		SORTER_CK(target_id >= ss_curr_id);
		bool in_the_id = (ss_curr_id == target_id);
		return in_the_id;
	}

	long		depth(){
		long dd = 0;
		sorset* srs = this;
		while(srs != NULL_PT){
			dd++;
			srs = srs->ss_parent_set;
		}
		return dd;
	}

	bool		contained_by(sorset& vsl_srs){
		sorset* srs = this;
		while(srs != NULL_PT){
			if(srs == &vsl_srs){
				return true;
			}
			srs = srs->ss_parent_set;
		}
		return false;
	}

	sorset&		add_sub(sort_glb& srg, sort_id_t curr_id, void* id_src);

	void		step_stabilize_rec(sort_glb& srg);
	void		step_mutual_stabilize_rec(sort_glb& srg1, sort_glb& srg2);

	void		unsort_all_items(sort_glb& srg);
	void		walk_release(sort_glb& srg);
	void		walk_reset_values(sort_glb& srg, grip& flat_ss);

	bool		walk_to_row(row<sortee*>& sorted, bool set_consecs){
		long consec = 0;
		bool has_diff = false;
		sorted.clear();
		walk_to_row_rec(set_consecs, consec, sorted, has_diff);

		return has_diff;
	}

	void		walk_to_row_rec(bool set_consecs, long& consec,
				row<sortee*>& sorted, bool& has_diff);

	void		walk_to_srs_row(row<sorset*>& sorted){
		long consec = 0;
		sorted.clear();
		walk_to_srs_row_rec(consec, sorted);
	}

	void		walk_to_srs_row_rec(long& consec, row<sorset*>& sorted);

	void		print_data_sorset(bj_ostream& os);
	void		print_tree_sorset(bj_ostream& os, long level);
	bj_ostream&	print_sorset(bj_ostream& os, bool from_pt = false);

};

comparison
cmp_sorset_trails(row_sort_id_t& ids1, row_sort_id_t& ids2);

comparison
cmp_sorsets(sorset* const& srs1, sorset* const& srs2);

bool
srs_row_as_clauses(sort_glb& srg, row<sorset*>& rr1, row<canon_clause*>& rr2);

inline
void
srs_row_get_first_tees(row<sorset*>& rr1, row<sortee*>& rr2){
	rr2.clear();
	rr2.set_cap(rr1.size());

	for(long ii = 0; ii < rr1.size(); ii++){
		sorset& srs = *(rr1[ii]);
		SORTER_CK(srs.has_items());

		sortee& srt = srs.first_item();
		rr2.push(&srt);
	}
}

//=================================================================
// sort_glb

enum step_op_t {
	po_full = 0,
	po_walk = 1,
	po_sort = 2
};

enum step_mutual_op_t {
	sm_walk = 0,
	sm_with_neus = 1,
	sm_with_opps = 2,
	sm_with_quas = 3
};

class sort_glb {
public:

	DBG(
		brain*	sg_pt_brn;
	)

	ch_string	sg_name;

	bool		sg_quick_find;
	bool		sg_dbg_always_find_filled;

	k_row<sorset>	sg_sorsets;
	row<sorset*>	sg_free_sorsets;

	sorset*		sg_head;

	row<sortee*>	sg_prt_srts;

	long		sg_num_active_ss;

	//bool		sg_has_dual;
	//sortee*		sg_first_srt;

	sort_id_t	sg_tot_stab_steps;
	sort_id_t	sg_curr_stab_consec;
	bool		sg_one_ccl_per_ss;

	step_op_t	sg_step_op;
	bool		sg_step_has_confl;
	long 		sg_step_consec;
	bool 		sg_step_has_diff;
	bool 		sg_step_all_consec;
	row<sortee*>	sg_step_sortees;
	row<sorset*>	sg_step_sorsets;
	sorset*		sg_step_first_multiple;

	//row<canon_clause*>	sg_step_neg_clauses;
	//row<canon_clause*>	sg_step_pos_clauses;

	step_mutual_op_t	sg_step_mutual_op;
	row<canon_clause*>	sg_step_mutual_clauses;

	long		sg_tot_ss_marks;

	// dbg attrs

	DBG(long	sg_dbg_num_items);
	long		sg_dbg_num_saved_consec;
	bool		sg_dbg_has_repeated_ids;

	sort_id_t	sg_dbg_last_id;

	// tmp attrs

	row<sortee*>	sg_tmp_srts;
	row<sorset*>	sg_tmp_srss;

	//row_long_t	sg_tmp_seps_trail;
	//row_long_t	sg_tmp_id_trail;
	//row<void*> 	sg_tmp_srcs_trail;

	// cnf data

	/*
	long			sg_cnf_tot_vars;
	long			sg_cnf_tot_ccls;
	long			sg_cnf_tot_lits;
	long			sg_cnf_tot_twolits;
	*/

	dima_dims		sg_cnf_dims;
	row<canon_clause*>	sg_cnf_clauses;
	canon_cnf		sg_cnf_step;

	long			sg_dbg_cnf_tot_onelit;

	sort_glb(){
		init_sort_glb();
	}

	~sort_glb(){
		release_all();
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		DBG(the_brn = sg_pt_brn);
		return the_brn;
	}

	void	set_dbg_brn(brain* the_brn){
		DBG(
			sg_pt_brn = the_brn;
			sg_cnf_step.set_dbg_brn(the_brn);
		);
	}
	
	void	init_sort_glb(){
		DBG(sg_pt_brn = NULL);
		
		sg_name = "INVALID_SG_NAM";
		
		sg_quick_find = true;
		sg_dbg_always_find_filled = false;

		sg_prt_srts.clear();

		sg_head = NULL_PT;

		sg_num_active_ss = 0;

		//sg_has_dual = false;
		//sg_first_srt = NULL_PT;

		sg_tot_stab_steps = 0;	
		sg_curr_stab_consec = 0;
		sg_one_ccl_per_ss = true;

		sg_step_op = po_full;
		sg_step_has_confl = false;
		sg_step_consec = 0;
		sg_step_has_diff = false;
		sg_step_all_consec = false;
		sg_step_sortees.clear();
		sg_step_sorsets.clear();
		sg_step_first_multiple = NULL_PT;
		//sg_step_neg_clauses.clear();
		//sg_step_pos_clauses.clear();

		sg_step_mutual_op = sm_walk;
		sg_step_mutual_clauses.clear();

		sg_tot_ss_marks = 0;

		DBG(sg_dbg_num_items = 0);
		sg_dbg_num_saved_consec = 0;
		sg_dbg_last_id = 0;
		sg_dbg_has_repeated_ids = false;

		sg_tmp_srts.clear();
		sg_tmp_srss.clear();

		//sg_tmp_seps_trail.clear();
		//sg_tmp_id_trail.clear();
		//sg_tmp_srcs_trail.clear();

		sg_cnf_dims.init_dima_dims();
		/*
		sg_cnf_tot_vars = 0;
		sg_cnf_tot_ccls = 0;
		sg_cnf_tot_lits = 0;
		sg_cnf_tot_twolits = 0;
		*/

		sg_dbg_cnf_tot_onelit = 0;
	}

	//void	start_sort_glb(long aprox_srs);

	void	sort_all_from(row<sortee*>& tees, sort_id_t curr_id, 
			bool add_ccl_id = false, long ccl_id = 0, bool sort_opps = false, 
			tgt_ccl_t tgt = tc_none);

	bool	has_head(){
		return (sg_head != NULL_PT);
	}

	sorset&	get_head_ss(){
		if(! has_head()){
			sorset& n_hd = add_sorset();
			MARK_USED(n_hd);
			SORTER_CK(&n_hd == sg_head);
		}
		sorset& hd = *sg_head;
		SORTER_CK(! hd.has_ss_parent());
		SORTER_CK(! hd.has_items());
		SORTER_CK(! hd.ss_has_val);
		SORTER_CK(! hd.has_brothers());
		return hd;
	}

	sorset&	add_sorset(){
		sorset* pt_srs = NULL_PT;
		if(! sg_free_sorsets.is_empty()){
			pt_srs = sg_free_sorsets.pop();
		} else {
			pt_srs = &(sg_sorsets.inc_sz());
		}
		sorset& srs = *pt_srs;
		SORTER_CK(srs.is_ss_virgin());

		if(sg_head == NULL_PT){ sg_head = &srs; }

		sg_num_active_ss++;
		return srs;
	}

	void	release_sorset(sorset& srs);

	void		release_head_sorsets();

	void		init_counters();
	void		step_stabilize(step_op_t op, bool ck_consecs = false);
	void		step_mutual_stabilize(sort_glb& mates_srg, step_mutual_op_t op);

	void		step_neus(sort_glb& mates_srg);
	void		step_opps(sort_glb& mates_srg);
	void		step_quas(sort_glb& mates_srg);

	sortee&		get_sortee_of_ccl_id(long ccl_id);

	void		build_cnf(skeleton_glb& skg, canon_cnf& the_cnf, 
					row<canon_clause*>& the_ccls, ch_string ph_str, bool sorted_cnf);
	
	void		step_build_cnf(skeleton_glb& skg, ch_string ph_str = "", 
							   bool sorted_cnf = true);

	void		stab_choose_one();
	void		stab_release_all_sorsets();
	void		stab_save_it();
	void		stab_recover_it();

	bool		all_flags_ccl(canon_clause& ccl, bool val);
	bool		ck_step_sortees();

	void		stabilize();

	void		stab_mutual_init();
	void		stab_mutual(sort_glb& mates_srg);
	void		stab_mutual_unique(sort_glb& mates_srg);
	canon_cnf&	stab_mutual_get_cnf(skeleton_glb& skg, ch_string comment, bool sorted_cnf);
	void		stab_mutual_choose_one(sort_glb& srg2);

	bool		base_path_exists(skeleton_glb& skg);

	bool		ck_all_tees_related();

	void		release_all();
	void		reset_head_values();

	void		clear_consecs();

	bool		sort_to_tmp_srts(bool set_consecs = false){
		bool has_diff = false;
		sg_tmp_srts.clear();
		if(has_head()){
			DBG(sg_tmp_srts.set_cap(sg_dbg_num_items));

			sorset& hd = get_head_ss();
			has_diff = hd.walk_to_row(sg_tmp_srts, set_consecs);
		}
		return has_diff;
	}

	void		sort_to_tmp_srss(){
		sg_tmp_srss.clear();
		if(has_head()){
			DBG(sg_tmp_srss.set_cap(sg_dbg_num_items));

			sorset& hd = get_head_ss();
			hd.walk_to_srs_row(sg_tmp_srss);
		}
	}

	bool		ck_srss_sorted(){
		sort_to_tmp_srss();
		bool ck1 = sg_tmp_srss.is_sorted(cmp_sorsets);
		return ck1;
	}

	bj_ostream&	print_sort_glb(bj_ostream& os, bool from_pt = false);

	template<class obj_t1>
	bool		sort_to_row_and_all_consec(row<obj_t1*>& sorted, bool& all_consec){
		bool set_consecs = true;
		bool has_diff = sort_to_tmp_srts(set_consecs);
		all_consec = srt_row_as<obj_t1>(sg_tmp_srts, sorted);
		DBG_SORTOR_PRT(56, os << "sort_to_row " << obj_t1::CL_NAME);
		return has_diff;
	}
};

//=================================================================
// TEMPLATE DEFS

template<class obj_t1>
obj_t1&
rcp_as(binder* bdr){
	SORTER_CK_1(bdr != NULL_PT);
	SORTER_CK_1(bdr->get_cls_name() == obj_t1::CL_NAME);

	receptor<obj_t1>& rcp = *((receptor<obj_t1>*)(bdr));
	SORTER_CK_1(rcp.re_me != NULL_PT);

	obj_t1& obj_1 = rcp;
	return obj_1;
}

template<class obj_t1>
void
grip::fill_rcps_as(row<obj_t1*>& rr){
	rr.clear();

	binder* fst_bdr = bn_right;
	binder* lst_bdr = this;
	for(binder* bdr_1 = fst_bdr; bdr_1 != lst_bdr; bdr_1 = bdr_1->bn_right){
		obj_t1& ob_1 = rcp_as<obj_t1>(bdr_1);
		rr.push(&ob_1);
	}
}

//=================================================================
// INLINE DEFS

sortee&	as_sortee(binder* bdr);

template<class obj_t1>
obj_t1&
as_srt_of(binder* bdr){
	sortee& srt = as_sortee(bdr);
	return srt.me_as<obj_t1>();
}

inline
sorset&
sortee::get_vessel(){
	SORTER_CK(has_vessel());
	return *so_vessel;
}

inline
void
sortee::save_consec(sort_glb& srg){
	SORTER_CK(so_saved_tee_consec == INVALID_NATURAL);
	so_saved_tee_consec = so_tee_consec;
	SORTER_CK(so_saved_tee_consec != INVALID_NATURAL);
	srg.sg_dbg_num_saved_consec++;
}

inline
long
sortee::recover_consec(sort_glb& srg){
	srg.sg_dbg_num_saved_consec--;
	SORTER_CK(so_saved_tee_consec != INVALID_NATURAL);
	long consec = so_saved_tee_consec;
	so_saved_tee_consec = INVALID_NATURAL;
	return consec;
}

inline
void
sorset::set_ss_mark(sort_glb& srg){
	SORTER_CK(! ss_mark);
	ss_mark = true;
	srg.sg_tot_ss_marks++;
}

inline
void
sorset::reset_ss_mark(sort_glb& srg){
	SORTER_CK(ss_mark);
	ss_mark = false;
	srg.sg_tot_ss_marks--;
}


//=================================================================
// printing operators

DEFINE_PRINT_FUNCS(sorset);
DEFINE_PRINT_FUNCS(sortee);
DEFINE_PRINT_FUNCS(sort_glb);


#endif		// SORTOR_H



