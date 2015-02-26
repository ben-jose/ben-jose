

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

sortor.cpp

Classes and that implement a sortor.

--------------------------------------------------------------*/

#include "stack_trace.h"

#include "skeleton.h"
#include "sortor.h"

#include "brain.h"	// only needed for print_sortee

char* sortee::CL_NAME = as_pt_char("{sortee}");
char* sorset::CL_NAME = as_pt_char("{sorset}");

sortee&
as_sortee(binder* bdr){
	SORTER_CK_1(bdr != NULL_PT);
	DBG_COND_COMM(! (bdr->get_cls_name() == sortee::CL_NAME) ,
		os << "ABORTING_DATA " << bj_eol;
		os << "bdr->get_cls_name()=" << bdr->get_cls_name() << bj_eol;
		os << "sortee::CL_NAME=" << sortee::CL_NAME << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SORTER_CK_1(bdr->get_cls_name() == sortee::CL_NAME);
	return *((sortee*)(bdr));
}

bj_ostream&
sort_glb::print_sort_glb(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	if(from_pt){
		os << " SRG=" << ((void*)this) 
		<< " sg_step_mutual_op=" << sg_step_mutual_op 
		<< " sg_curr_stab_consec=" << sg_curr_stab_consec
		<< " sg_num_active_ss=" << sg_num_active_ss;

		DBG(
		os << " sg_dbg_num_items=" << sg_dbg_num_items
		<< " sg_dbg_last_id=" << sg_dbg_last_id;
		);

		os << bj_eol;
		os.flush();
		return os;
	}

	sg_prt_srts.clear();
	if(has_head()){
		DBG(sg_prt_srts.set_cap(sg_dbg_num_items));

		sorset& hd = get_head_ss();
		hd.walk_to_row(sg_prt_srts, false);

		os << sg_prt_srts << bj_eol;
	} else {
		os << "EMPTY SORTSETS in sort_glb" << bj_eol;
	}
	os.flush();
	return os;
}

bj_ostream&
sortee::print_sortee(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);

	//os << "(" << so_dbg_extrn_id << ")";
	//os << "(" << (void*)this << ").";
	//os << so_tee_consec;
	/*
	if(so_dbg_me_class == 1){
		os << "." << so_qua_id;
	}
	if(so_dbg_me_class == 2){
		os << "." << so_ccl;
	}
	os << " ";
	*/

	if(! from_pt){
		// NEEDS_brain_h
		os << "tee{";
		
		if(so_dbg_me_class == 1){
			SORTER_CK(so_me != NULL_PT);
			quanton& qua = *((quanton*)so_me);
			os << &qua;
		}
		if(so_dbg_me_class == 2){
			SORTER_CK(so_me != NULL_PT);
			neuron& neu = *((neuron*)so_me);
			os << &neu;
		}
		os << "}";
		/* */
	}

	os.flush();
	return os;

	/*
	bool is_a = is_alone();
	bool is_u = is_unsorted();

	os << "{";
	if(is_a){ os << "A"; }
	if(is_u){ os << "U"; }
	os << "(" << (void*)this << ").";
	os << "wc=";
	os << so_walk_consec;
	os << " sc=";
	os << so_sorset_consec << ".";
	os << "(" << (void*)so_me << ").";
	os << so_vessel << "."; 	// prt_sorset
	//os << so_id_trail << ".";
	os << "}";

	os << bj_eol;

	os.flush();
	return os;
	*/
}

ch_string
get_indentation(long level){
	bj_ostr_stream ind_ss;
	ind_ss << "";
	for(long aa = 0; aa < level; aa++){
		ind_ss << "   ";
	}
	return ind_ss.str();
}

void
sorset::print_data_sorset(bj_ostream& os){
	/*
	os << "(";
	os << "pt=(" << (void*)this << "), ";
	os << ")";
	*/

	// items

	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);
		//os << "it=(" << (void*)(&srt) << "), ";
		os << &srt;
	}
}

void
sorset::print_tree_sorset(bj_ostream& os, long level){
	ch_string ind_str = get_indentation(level);
	os << ind_str << "{" << ss_curr_id << " ";
	print_data_sorset(os);
	os << bj_eol;

	level++;

	binder* fst = ss_subsets.bn_right;
	binder* lst = &(ss_subsets);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sorset& nsr = rcp_as<sorset>(rgt);
		nsr.print_tree_sorset(os, level);
	}

	os << ind_str << "}";
	os << bj_eol;
}

bj_ostream&
sorset::print_sorset(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);

	if(from_pt){
		row_sort_id_t ids;
		row<void*> id_srcs;

		fill_srs_trails(ids, id_srcs);

		os << "ss={";
		print_data_sorset(os);
		//os << " ids=" << ids;
		os << "}";

		os.flush();
		return os;
	}

	print_tree_sorset(os, 0);

	return os;
}

//============================================================
// sorset cmp

comparison
cmp_sorset_trails(row_sort_id_t& ids1, row_sort_id_t& ids2)
{
	long ii = 0;
	DBG(sort_id_t lp1 = -1);
	DBG(sort_id_t lp2 = -1);
	for(ii = 0; (ii < ids1.size()) && (ii < ids2.size()); ii++){
		sort_id_t p1 = ids1[ii];
		sort_id_t p2 = ids2[ii];

		SORTER_CK(p1 > 0);
		SORTER_CK(p2 > 0);
		SORTER_CK(lp1 <= p1);
		SORTER_CK(lp2 <= p2);
		DBG(lp1 = p1);
		DBG(lp2 = p2);

		if(p1 == p2){
			continue;
		}

		if(p1 < p2){ return -1;	}
		if(p1 > p2){ return 1; }

		SORTER_CK(false);
	}

	// ids1 shorter
	if(ii < ids2.size()){
		return 1;
	}

	// ids2 shorter
	if(ii < ids1.size()){
		return -1;
	}

	SORTER_CK(ids1.size() == ids2.size());
	return 0;
}

comparison
cmp_sorsets(sorset* const& srs1, sorset* const& srs2){
	SORTER_CK(srs1 != NULL_PT);
	SORTER_CK(srs2 != NULL_PT);

	row_sort_id_t id_trl1;
	row<void*> src_trl1;

	row_sort_id_t id_trl2;
	row<void*> src_trl2;

	srs1->fill_srs_trails(id_trl1, src_trl1);
	srs2->fill_srs_trails(id_trl2, src_trl2);

	return cmp_sorset_trails(id_trl1, id_trl2);
}

// sortee funcs

void
sortee::sort_me_to(sort_glb& srg, sorset& nsr){
	SORTER_CK(nsr.can_sort_in_me());

	MARK_USED(srg);
	SORTER_CK(so_me != NULL_PT);
	DBG_SORTOR_PRT(14, os << "SORTING " << this << " TO " << &nsr);  // prt_sorset

	SORTER_CK(is_alone());
	SORTER_CK(! has_vessel());
	SORTER_CK(get_cls_name() == sortee::CL_NAME);
	nsr.ss_items.bind_to_my_left(*this);
	so_vessel = &nsr;
	
	DBG(srg.sg_dbg_num_items++);
	DBG(sortee& ll = nsr.last_item());
	SORTER_CK(ll.get_cls_name() == sortee::CL_NAME);
	SORTER_CK(&ll == this);
}

void
sortee::unsort_me(sort_glb& srg){
	MARK_USED(srg);
	SORTER_CK(so_me != NULL_PT);
	SORTER_CK(! is_alone());
	let_go();
	so_vessel = NULL_PT;
	DBG(srg.sg_dbg_num_items--);
	SORTER_CK(is_alone());
}

void
sortee::sort_from(sort_glb& srg, sort_id_t curr_id, void* id_src){
	DBG_SORTOR_PRT(66, os << "sort from srt=" << this << " ID=" << curr_id);

	SORTER_CK(curr_id > 0);
	DBG_COND_COMM(! (curr_id >= srg.sg_dbg_last_id) ,
		os << "ABORTING_DATA " << bj_eol;
		os << "curr_id=" << curr_id << bj_eol;
		os << "srg.sg_dbg_last_id=" << srg.sg_dbg_last_id << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SORTER_CK(curr_id >= srg.sg_dbg_last_id);
	DBG(
		if(curr_id > srg.sg_dbg_last_id){
			srg.sg_dbg_last_id = curr_id;
		}
	);
	SORTER_CK(so_me != NULL_PT);
	sorset* vssl = NULL_PT;

	//long curr_sep = srg.sg_num_separations;

	if(is_alone()){
		SORTER_CK(! has_vessel());
		so_sorset_consec = INVALID_NATURAL;
		so_walk_consec = INVALID_NATURAL;
		
		sorset& hd_nsr = srg.get_head_ss();
		vssl = &hd_nsr;
	} else {
		vssl = so_vessel;
		SORTER_CK(vssl != NULL_PT);
		if(vssl->in_item_id(curr_id)){
			srg.sg_dbg_has_repeated_ids = true;
		}
		unsort_me(srg);
	}
	SORTER_CK(is_alone());
	SORTER_CK(vssl != NULL_PT);

	sorset& nxt_vessl = vssl->get_next_vessel(srg, curr_id, id_src);
	sort_me_to(srg, nxt_vessl);
}

sorset&
sorset::get_next_vessel(sort_glb& srg, sort_id_t curr_id, void* id_src){

	sorset* nxt_vssl = last_sub();
	sorset* add_vssl = this;

	if((nxt_vssl == NULL_PT) || ! nxt_vssl->in_item_id(curr_id)){
		SORTER_CK(add_vssl != NULL_PT);
		nxt_vssl = &(add_vssl->add_sub(srg, curr_id, id_src));
	}

	SORTER_CK(nxt_vssl != NULL_PT);
	SORTER_CK(nxt_vssl->in_item_id(curr_id));

	return *nxt_vssl;
}

sorset*
sorset::last_sub(){
	sorset* last = NULL_PT;

	grip& the_sub_sid = ss_subsets;
	if(! the_sub_sid.is_alone()){
		sorset& l_sub = rcp_as<sorset>(the_sub_sid.bn_left);
		SORTER_CK(l_sub.depth() > depth());
		last = &l_sub;
	}
	return last;
}

sorset&
sorset::add_sub(sort_glb& srg, sort_id_t curr_id, void* id_src){
	SORTER_CK(srg.has_head());
	sorset& l_sub = srg.add_sorset();
	DBG(sorset& hh = srg.get_head_ss());
	SORTER_CK(&hh != &l_sub);

	//long curr_sep = srg.sg_num_separations;

	SORTER_CK(! l_sub.ss_has_val);
	l_sub.set_item_id(curr_id, id_src);

	grip& the_sub_sid = ss_subsets;
	the_sub_sid.bind_to_my_left(l_sub.ss_sub_rcp);
	l_sub.ss_parent_set = this;

	SORTER_CK(last_sub() == &l_sub);
	SORTER_CK(last_sub()->in_item_id(curr_id));
	return l_sub;
}

void
sorset::unsort_all_items(sort_glb& srg){
	while(has_items()){
		sortee& srt = first_item();
		srt.unsort_me(srg);
		srt.so_sorset_consec = INVALID_NATURAL;
		srt.so_walk_consec = INVALID_NATURAL;
	}
	SORTER_CK(ss_items.is_alone());
}

void
sorset::fill_srs_trails(row_sort_id_t& id_trl, row<void*>& src_trl)
{
	if(! has_ss_parent()){
		id_trl.clear();
		src_trl.clear();
	} else {
		ss_parent_set->fill_srs_trails(id_trl, src_trl);
	}
	if(ss_curr_id != 0){
		id_trl.push(ss_curr_id);
		src_trl.push(ss_id_src);
	}
}

bool
sort_glb::ck_all_tees_related(){
	//sort_glb& srg = *this;
	row<sortee*>& tees = sg_step_sortees;
	for(long aa = 0; aa < tees.size(); aa++){
		sortee& tee = *(tees[aa]);
		MARK_USED(tee);
		SORTER_CK(tee.so_tee_consec != INVALID_NATURAL);
		long tee_cons = tee.so_tee_consec;
		MARK_USED(tee_cons);
		SORTER_CK(tee.so_related != NULL_PT);
	}
	return true;
}

// sorting funcs

bool
sorset::is_ss_virgin(bool ck_items){
	bool c1 = (ss_parent_set == NULL_PT);
	bool c2 = (ss_sub_rcp.is_alone());
	bool c3 = (! ck_items || ss_items.is_alone());

	bool c4 = (ss_subsets.is_alone());

	bool c5 = (ss_curr_id == 0);
	bool c6 = (ss_id_src == NULL_PT);
	bool c7 = (! ss_has_val);

	bool is_vir = (c1 && c2 && c3 && c4 && c5 && c6 && c7);

	DBG_COND_COMM(! is_vir,
		os << "ABORTING_DATA " << bj_eol;
		os << "  c1=" << c1 << "  c2=" << c2 << "  c3=" << c3 << "  c4=" << c4;
		os << "  c5=" << c5 << "  c6=" << c6 << "  c7=" << c7;
		os << "END_OF_aborting_data" << bj_eol;
	);
	return is_vir;
}

void
sort_glb::release_sorset(sorset& srs){
	SORTER_CK(! sg_sorsets.is_empty());

	if(sg_head == &srs){ 
		DBG_PRT(128, os << " RELEASING_HEAD !!! sortor=" << this);
		sg_head = NULL_PT; 
	}

	srs.ss_id_src = NULL_PT;

	SORTER_CK(srs.is_ss_virgin());
	sg_free_sorsets.push(&srs);
	srs.init_sorset();

	sg_num_active_ss--;
}

void
sorset::walk_release(sort_glb& srg){
	// subsets

	while(has_subsets()){
		sorset& nsr = first_subset();
		nsr.walk_release(srg);
	}

	// items

	unsort_all_items(srg);

	// finish

	ss_sub_rcp.let_go();
	ss_has_val = false;
	ss_curr_id = 0;
	ss_parent_set = NULL_PT;

	SORTER_CK(is_ss_virgin());

	srg.release_sorset(*this);
}

void
sort_glb::release_all(){
	DBG_PRT(6, os << "releasing sort_glb 0");

	stab_release_all_sorsets();

	SORTER_CK(sg_num_active_ss == 0);
	
	sg_sorsets.clear(true, true);
	sg_free_sorsets.clear();

	SORTER_CK(sg_sorsets.is_empty());

	DBG_PRT(6, os << "RELEASING sort_glb 1");
}

void
sorset::walk_to_row_rec(bool set_consecs, long& consec,
			row<sortee*>& sorted, bool& has_diff)
{
	binder* fst = NULL_PT;
	binder* lst = NULL_PT;
	binder* rgt = NULL_PT;

	// subsets

	fst = ss_subsets.bn_right;
	lst = &ss_subsets;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sorset& nsr = rcp_as<sorset>(rgt);
		nsr.walk_to_row_rec(set_consecs, consec, sorted, has_diff);
	}

	// items

	if(! ss_items.is_alone()){
		consec++;
	}

	fst = ss_items.bn_right;
	lst = &ss_items;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);
		srt.so_sorset_consec = consec;

		if(set_consecs){
			if(! has_diff && (consec != srt.so_walk_consec)){
				DBG_SORTOR_PRT(30, os << "first diff=" << bj_eol
					<< srt << " NEW_consec=" << consec);
				has_diff = true;
			}
			srt.so_walk_consec = consec;
		}

		sorted.push(&srt);
	}
}

void
sort_glb::build_cnf(skeleton_glb& skg, canon_cnf& the_cnf, row<canon_clause*>& the_ccls, 
			ch_string ph_str, bool sorted_cnf)
{
	the_cnf.clear_cnf();
	the_cnf.cf_sorted = sorted_cnf;
	the_cnf.cf_phase_str = ph_str;
	the_cnf.cf_dims = sg_cnf_dims;

	the_ccls.move_to(the_cnf.cf_clauses);

	the_cnf.calc_sha();

	SORTER_CK(! the_cnf.cf_sha_str.empty());

	//DBG_PRT(56, os << "the_cnf.cf_dims.dd_tot_twolits=" << the_cnf.cf_dims.dd_tot_twolits);

	the_cnf.init_skl_paths(skg);
}

// save and recover

void
sort_glb::release_head_sorsets(){
	sort_glb& srg = *this;
	if(has_head()){
		sorset& hd = get_head_ss();
		hd.walk_release(srg);
	}
	SORTER_CK(! has_head());
}

void
sort_glb::stab_release_all_sorsets(){
	sort_glb& srg = *this;
	row<sorset*>& all_ss = sg_step_sorsets;
	while(! all_ss.is_empty()){
		sorset& srs = *(all_ss.last());
		srs.walk_release(srg);
		all_ss.dec_sz();
	}
	SORTER_CK(sg_step_sorsets.is_empty());

	release_head_sorsets();

	sg_dbg_last_id = 0;
	sg_dbg_has_repeated_ids = false;
	sg_tot_stab_steps = 0;
	sg_curr_stab_consec = 0;

	SORTER_CK(! has_head());
	SORTER_CK(sg_num_active_ss == 0);
	SORTER_CK(sg_dbg_num_items == 0);
	SORTER_CK(sg_dbg_last_id == 0);
	SORTER_CK(sg_dbg_has_repeated_ids == false);
	SORTER_CK(sg_curr_stab_consec == 0);

	SORTER_CK(srg.sg_curr_stab_consec >= srg.sg_dbg_last_id);
}

/*
void
sort_glb::stab_recover_it(){
	
	stab_release_all_sorsets();

	sort_glb& srg = *this;
	BRAIN_CK(! has_head());
	sorset& hd_nsr = get_head_ss();
	BRAIN_CK(has_head());

	SORTER_CK(! hd_nsr.has_subsets());
	SORTER_CK(! hd_nsr.has_items());

	row<sortee*>& tees = sg_step_sortees;
	BRAIN_CK(tees.is_empty());
	for(long aa = 0; aa < tees.size(); aa++){
		BRAIN_CK(false);
		sortee& tee = *(tees[aa]);
		SORTER_CK(tee.is_alone());

		long consec = tee.recover_consec(srg);
		if(consec != sg_curr_stab_consec){
			SORTER_CK((sg_curr_stab_consec + 1) == consec);
			sg_curr_stab_consec = consec;
		}
		tee.sort_from(srg, sg_curr_stab_consec);
	}
	
	SORTER_CK(sg_step_sorsets.is_empty());
	sg_step_sorsets.push(&hd_nsr);

	SORTER_CK(sg_dbg_num_saved_consec == 0);
}*/

bool
sort_glb::base_path_exists(skeleton_glb& skg){
	ch_string base_pth = skg.as_full_path(SKG_REF_DIR);

	dima_dims dims;
	dims = sg_cnf_dims;

	bool dim_exis = dims_path_exists(base_pth, dims);
	return dim_exis;
}

void
sortee::update_totals(sort_glb& srg, long tgt_sz){
	if(so_ccl.cc_size() == tgt_sz){
		long n_sz = so_ccl.cc_size();

		if(n_sz == 2){ srg.sg_cnf_dims.dd_tot_twolits++; }
		srg.sg_cnf_dims.dd_tot_lits += n_sz;

		if(n_sz == 1){ srg.sg_dbg_cnf_tot_onelit++; }
	}
}

void
sort_glb::sort_all_from(row<sortee*>& tees, sort_id_t curr_id, 
						bool add_ccl_id, long ccl_id, bool sort_opps, tgt_ccl_t tgt)
{
	sort_glb& srg = *this;

	for(long aa = 0; aa < tees.size(); aa++){
		sortee* pt_tee = tees[aa];
		if(sort_opps){
			SORTER_CK(pt_tee != NULL_PT);
			pt_tee = &(pt_tee->opposite());
		}

		SORTER_CK(pt_tee != NULL_PT);
		sortee& the_tee = *pt_tee;

		SORTER_CK(! the_tee.is_alone());

		the_tee.sort_from(srg, curr_id); 

		if(add_ccl_id){ 
			SORTER_CK(ccl_id != 0);
			the_tee.so_ccl.cc_push(ccl_id);

			long vv = abs_long(ccl_id);
			if(vv > sg_cnf_dims.dd_tot_vars){
				sg_cnf_dims.dd_tot_vars = vv;
			}

			if(tgt != tc_none){
				long tgt_sz = the_tee.get_target_sz(tgt);
				the_tee.update_totals(srg, tgt_sz);
			}
		}
	}	
}

void
sort_glb::init_counters(){
	sg_cnf_dims.init_dima_dims();
}

bool
srs_row_as_clauses(sort_glb& srg, row<sorset*>& rr1, row<canon_clause*>& rr2){
	long last_item_id = INVALID_NATURAL;
	bool all_consec = true;
	rr2.clear();
	rr2.set_cap(rr1.size());

	long old_num_var = srg.sg_cnf_dims.dd_tot_vars;
	srg.init_counters();
	srg.sg_cnf_dims.dd_tot_vars = old_num_var;

	for(long ii = 0; ii < rr1.size(); ii++){
		sorset& srs = *(rr1[ii]);
		SORTER_CK(srs.has_items());

		sortee& srt = srs.first_item();

		all_consec = all_consec && (last_item_id != srt.so_sorset_consec);
		last_item_id = srt.so_sorset_consec;

		canon_clause& the_ccl = srt.so_ccl;
		long ccl_sz = the_ccl.cc_size();

		SORTER_CK(ccl_sz > 1);

		srg.sg_cnf_dims.dd_tot_lits += ccl_sz;
		if(ccl_sz == 2){ srg.sg_cnf_dims.dd_tot_twolits++; }

		rr2.push(&(the_ccl));
	}

	srg.sg_cnf_dims.dd_tot_ccls = rr2.size();

	return all_consec;
}

// mutual stab

void
sorset::step_mutual_stabilize_rec(sort_glb& srg1, sort_glb& srg2)
{
	SORTER_CK(&srg1 != &srg2);
	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	// subsets

	while(has_subsets()){
		sorset& nsr = first_subset();
		nsr.step_mutual_stabilize_rec(srg1, srg2);
	}
	
	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);

	// pre assigs

	step_mutual_op_t oper = srg1.sg_step_mutual_op;
	row<sorset*>& all_ss = srg1.sg_step_sorsets;

	long& tee_consec = srg1.sg_step_consec;
	bool& has_diff = srg1.sg_step_has_diff;
	bool& all_consec = srg1.sg_step_all_consec;
	row<sortee*>& sorted_tees = srg1.sg_step_sortees;

	row<canon_clause*>& all_ccl = srg1.sg_step_mutual_clauses;

	sort_id_t* pt_stab_consec = &(srg2.sg_curr_stab_consec); // matches sort_from srg param
	bool& one_ccl = srg1.sg_one_ccl_per_ss;

	if(oper == sm_with_opps){
		// srg1 is passed to sort from
		pt_stab_consec = &(srg1.sg_curr_stab_consec);
	}

	sort_id_t& curr_stab_consec = *pt_stab_consec;

	// items

	bool set_tee_consecs = (oper != sm_with_opps);

	if(has_items()){
		if(oper != sm_walk){
			curr_stab_consec++;
		}
		if(set_tee_consecs){
			tee_consec++;
		}
		all_ss.push(this);
	}

	//bool has_one_item = one_item();
	bool set_mul_op = ((oper == sm_walk) || (oper == sm_get_ccls)); 
	if(set_mul_op && (srg1.sg_step_first_multiple == NULL_PT) && is_multitem()){
		srg1.sg_step_first_multiple = this;
	}

	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);

		SORTER_CK(&(srt.get_vessel()) == this);
		SORTER_CK(srt.so_related != NULL_PT);

		// consec managment

		if(set_tee_consecs){
			if(! has_diff && (tee_consec != srt.so_tee_consec)){
				DBG_SORTOR_PRT(30, os << "first diff=" << bj_eol
					<< srt << " NEW_consec=" << tee_consec);
				has_diff = true;
			}

			if(oper != sm_walk){
				srt.so_tee_consec = tee_consec;
			}
	
			if(all_consec){			
				bool same_consec = (! sorted_tees.is_empty() && 
					(sorted_tees.last()->so_tee_consec == srt.so_tee_consec));
				if(same_consec){ all_consec = false; }
			}
		}

		// all sortees

		if(set_tee_consecs){
			sorted_tees.push(&srt);
		}

		canon_clause& tee_ccl = srt.so_ccl;
		if(oper == sm_get_ccls){
			// one ccl by sorset
			if(one_ccl){ if(rgt == fst){ all_ccl.push(&tee_ccl); } }
			else { all_ccl.push(&tee_ccl); }
		} else {
			// clear my ccls
			tee_ccl.cc_clear(false);
		}

		// sort oper

		if(oper == sm_with_neus){
			row<sortee*>& all_mates = srt.so_related->so_mates;
			srg2.sort_all_from(all_mates, curr_stab_consec, false, 0, true);
		}

		if(oper == sm_with_opps){
			sortee& opp = srt.opposite();
			SORTER_CK(! opp.is_alone());
			sorset& vssl = opp.get_vessel();
	
			if(&vssl != this){
				opp.sort_from(srg1, curr_stab_consec);
			}

			srt.so_qua_id = 0;
			opp.so_qua_id = 0;
		}

		if(oper == sm_with_quas){
			long qua_id = srt.get_qua_id(srg1);
			
			row<sortee*>& all_mates = srt.so_related->so_mates;
			srg2.sort_all_from(all_mates, curr_stab_consec, true, qua_id, false, tc_mates);
		}
	}

	// finish

	ss_sub_rcp.let_go();
	ss_has_val = false;
	ss_curr_id = 0;
	ss_parent_set = NULL_PT;

	SORTER_CK(is_ss_virgin(false));

	if(! has_items()){
		srg1.release_sorset(*this);
	}
} // step.mutual.stabilize.rec

void
sort_glb::step_mutual_stabilize(sort_glb& srg2, step_mutual_op_t op){
	sort_glb& srg1 = *this;
	
	srg1.sg_tot_stab_steps++;
	srg2.sg_tot_stab_steps++;

	sg_step_op = po_walk;
	sg_step_has_confl = false;
	sg_step_consec = 0;
	sg_step_has_diff = false;
	sg_step_all_consec = true;
	sg_step_sortees.clear();
	sg_step_first_multiple = NULL_PT;
	sg_step_neg_sorsets.clear();
	sg_step_pos_sorsets.clear();

	sg_step_mutual_op = op;
	sg_step_mutual_clauses.clear();

	init_counters();

	sg_dbg_cnf_tot_onelit = 0;

	sg_cnf_clauses.clear();

	row<sorset*> sets;

	SORTER_CK(! sg_step_sorsets.is_empty());
	DBG(long old_ss_sz = sg_step_sorsets.size());
	sg_step_sorsets.move_to(sets);
	sg_step_sorsets.set_cap(sets.size());
	SORTER_CK(sg_step_sorsets.is_empty());

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	for(long aa = 0; aa < sets.size(); aa++){
		SORTER_CK(sets[aa] != NULL_PT);
		sorset& srs = *(sets[aa]);
		sets[aa] = NULL_PT;

		if(! srg1.has_head()){
			DBG_PRT(128, os << "INITING head of " << &srg1);
			srg1.init_head_ss();
		}
		if(! srg2.has_head()){
			DBG_PRT(128, os << "INITING head of " << &srg2);
			srg2.init_head_ss();
		}
		
		srs.step_mutual_stabilize_rec(srg1, srg2);

		SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
		SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	}
	sets.clear();

	SORTER_CK(old_ss_sz <= sg_step_sorsets.size());
	SORTER_CK((op == sm_with_opps) || (sg_step_sorsets.size() <= sg_step_sortees.size()));
	SORTER_CK(sg_cnf_dims.is_dd_virgin());
	SORTER_CK(sg_cnf_clauses.is_empty());
	SORTER_CK((op == sm_get_ccls) || sg_step_mutual_clauses.is_empty());

	if(op == sm_get_ccls){
		sg_step_mutual_clauses.move_to(sg_cnf_clauses);
		sg_cnf_dims.dd_tot_ccls = sg_cnf_clauses.size();
	}
	if(! srg1.has_head()){
		DBG_PRT(128, os << "INITING head of " << &srg1);
		srg1.init_head_ss();
	}
	if(! srg2.has_head()){
		DBG_PRT(128, os << "INITING head of " << &srg2);
		srg2.init_head_ss();
	}
	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	SORTER_CK_PRT(((sm_with_opps == op) || 
		(srg1.sg_step_sortees.size() == srg1.sg_dbg_fst_num_items)),
			  os << " op=" << op << " i_sz=" << srg1.sg_dbg_fst_num_items
			  << " sz=" << srg1.sg_step_sortees.size()
	);
}

void
sort_glb::stab_mutual(sort_glb& srg2){
	SORTER_CK(&srg2 != this);

	sort_glb& srg1 = *this;
	
	DBG(
		srg1.sg_dbg_fst_num_items = srg1.sg_dbg_num_items;
		srg2.sg_dbg_fst_num_items = srg2.sg_dbg_num_items;
	);

	srg1.sg_tot_stab_steps = 0;
	srg2.sg_tot_stab_steps = 0;

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	SORTER_CK(srg1.has_head());
	SORTER_CK(srg2.has_head());

	bool has_diff = true;
	while(has_diff){
		srg1.step_neus(srg2);
		bool diff1 = srg1.sg_step_has_diff;

		srg2.step_opps(srg1);

		srg2.step_quas(srg1);
		bool diff2 = srg2.sg_step_has_diff;

		has_diff = (diff1 || diff2);
	}
	SORTER_CK(! srg1.sg_step_has_diff);
	SORTER_CK(! srg2.sg_step_has_diff);
	
	stab_mutual_end(srg2);
	
	SORTER_CK(srg1.sg_step_sortees.size() == srg1.sg_dbg_fst_num_items);
	SORTER_CK(srg2.sg_step_sortees.size() == srg2.sg_dbg_fst_num_items);
}

void
sort_glb::stab_mutual_end(sort_glb& srg2){
	SORTER_CK(&srg2 != this);
	sort_glb& srg1 = *this;

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	srg1.join_all_tees_in_head();
	srg2.add_neg_and_pos_to(srg2.sg_step_sorsets);
	srg2.step_quas(srg1);
	
	SORTER_CK(srg1.sg_step_sorsets.size() == 1);
	srg1.step_mutual_stabilize(srg2, sm_get_ccls);

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
}

long
sortee::get_qua_id(sort_glb& srg){
	if(so_qua_id == 0){
		SORTER_CK(so_tee_consec > 0);
		SORTER_CK(! is_alone());
		SORTER_CK(has_vessel());

		sortee& opp = opposite();
		SORTER_CK(opp.so_qua_id == 0);
		SORTER_CK(! opp.is_alone());
		SORTER_CK(opp.has_vessel());
		
		row<sorset*>& all_neg = srg.sg_step_neg_sorsets;
		row<sorset*>& all_pos = srg.sg_step_pos_sorsets;
		
		bool in_same = (so_vessel == opp.so_vessel);
		
		bool added_vsl = (all_neg.is_empty() || (so_vessel != all_neg.last()));
		if(added_vsl){
			all_neg.push(so_vessel);
		}
		so_qua_id = -so_tee_consec;
		
		if(in_same){
			opp.so_qua_id = -so_tee_consec;
		} else {
			if(added_vsl){
				all_pos.push(opp.so_vessel);
			}
			opp.so_qua_id = so_tee_consec;
		}
	}
	SORTER_CK(so_qua_id != 0);
	return so_qua_id;
}

void
sort_glb::stab_mutual_choose_one(sort_glb& srg2){
	SORTER_CK(&srg2 != this);
	
	sort_glb& srg1 = *this;

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	step_mutual_stabilize(srg2, sm_walk);
	SORTER_CK(srg1.has_head());

	SORTER_CK(sg_step_first_multiple != NULL_PT);

	sorset& ss_one = *sg_step_first_multiple;

	//SORTER_CK(! ss_one.has_subsets());
	SORTER_CK(ss_one.is_multitem());

	sort_id_t& stab_consec = srg1.sg_curr_stab_consec;

	stab_consec++;

	sortee& to_sort = ss_one.first_item();

	DBG_SORTOR_PRT(57, os << "choose one=" << to_sort);

	to_sort.sort_from(srg1, stab_consec);
	
	SORTER_CK(srg1.has_head());
	SORTER_CK(srg2.has_head());
	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
}

void
sort_glb::stab_mutual_unique(sort_glb& srg2){
	SORTER_CK(&srg2 != this);
	sort_glb& srg1 = *this;

	SORTER_CK(srg1.has_head());
	SORTER_CK(srg2.has_head());
	
	srg2.sg_step_all_consec = false;

	bool all_consec = false;
	while(! all_consec){
		stab_mutual(srg2);

		all_consec = srg2.sg_step_all_consec;

		if(! all_consec){
			srg2.stab_mutual_choose_one(srg1);
		}
	}
	SORTER_CK(srg2.sg_step_all_consec);

	DBG_PRT(63, 
			os << " UNIQUE cnf=" << bj_eol; sg_cnf_clauses.print_row_data(os, true, "\n");
	);
}

canon_cnf&
sort_glb::stab_mutual_get_cnf(skeleton_glb& skg, ch_string comment, bool sorted_cnf){
	DBG_PRT(114, os << " get_cnf=" << this << " " << comment << bj_eol;
		sg_cnf_clauses.print_row_data(os, true, "\n");
	);

	BRAIN_CK(! sg_one_ccl_per_ss || (sg_step_sorsets.size() == sg_cnf_clauses.size()));
	BRAIN_CK(sg_one_ccl_per_ss || (sg_step_sortees.size() == sg_cnf_clauses.size()));

	build_cnf(skg, sg_cnf_step, sg_cnf_clauses, comment, sorted_cnf);
	return sg_cnf_step;
}

void
sort_glb::step_neus(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_neus);
}

void
sort_glb::step_opps(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_opps);
	SORTER_CK(sg_cnf_clauses.is_empty());
}

void
sort_glb::step_quas(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_quas);
	SORTER_CK(sg_cnf_clauses.is_empty());
	
	DBG_PRT(62, os << " STEP_QUAS cnf=" << bj_eol;
		mates_srg.sg_cnf_clauses.print_row_data(os, true, "\n");
	);
}

void
sort_glb::stab_mutual_init(){
	sg_one_ccl_per_ss = false;

	stab_release_all_sorsets();
	sorset& hd_nsr = init_head_ss();
	
	SORTER_CK(sg_step_sorsets.is_empty());
	sg_step_sorsets.push(&hd_nsr);
	
	SORTER_CK(sg_num_active_ss == 1);
	SORTER_CK(sg_dbg_num_items == 0);
	SORTER_CK(sg_step_sorsets.size() == 1);
	SORTER_CK(has_head());
	SORTER_CK(&(get_head_ss()) == sg_step_sorsets.first());
	SORTER_CK(! sg_step_sortees.is_empty() || get_head_ss().is_ss_virgin());
	SORTER_CK(sg_curr_stab_consec == 0);
	SORTER_CK(! sg_one_ccl_per_ss);
	SORTER_CK(sg_dbg_num_saved_consec == 0);

	sg_curr_stab_consec++;
	init_counters();
}


void
sorset::walk_to_srs_row_rec(long& consec, row<sorset*>& sorted)
{
	binder* fst = NULL_PT;
	binder* lst = NULL_PT;
	binder* rgt = NULL_PT;

	// subsets

	fst = ss_subsets.bn_right;
	lst = &ss_subsets;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sorset& nsr = rcp_as<sorset>(rgt);
		nsr.walk_to_srs_row_rec(consec, sorted);
	}

	// items

	if(has_items()){
		consec++;
		sorted.push(this);
	}

}

void
sort_glb::add_neg_and_pos_to(row<sorset*>& dest_ss){
	// careful the behavior of this func must match the behavior of get_qua_id func
	
	dest_ss.clear();

	sg_step_neg_sorsets.append_to(dest_ss, 0 , -1 , true);
	sg_step_pos_sorsets.append_to(dest_ss);
	
	row<sorset*>& all_neg = sg_step_neg_sorsets;
	long qu_id = -(all_neg.size());
	for(long aa = all_neg.last_idx(); aa >= 0; aa--){
		SORTER_CK(all_neg[aa] != NULL_PT);
		sorset& srs = *(all_neg[aa]);
		SORTER_CK(srs.is_final());
		SORTER_CK(qu_id < 0);
		
		srs.set_all_items_qua_id(qu_id);
		
		sortee& srt = srs.first_item();
		SORTER_CK(srt.so_vessel == &srs);
		sortee& opp = srt.opposite();
		SORTER_CK(opp.has_vessel());
		if(opp.so_vessel != &srs){
			opp.so_vessel->set_all_items_qua_id(-qu_id);
		}
		
		qu_id++;
	}
	
	SORTER_CK(ck_sorted_sorsets(dest_ss));
}

void 
sort_glb::join_all_tees_in_head(){
	sort_glb& srg1 = *this;
	DBG(long old_num_tee = sg_step_sortees.size());
	
	stab_mutual_init();
	
	SORTER_CK(has_head());
	SORTER_CK(old_num_tee == sg_step_sortees.size());
	
	row<sortee*>& all_tees = sg_step_sortees;
	for(long aa = 0; aa < all_tees.size(); aa++){
		SORTER_CK(all_tees[aa] != NULL_PT);
		sortee& srt = *(all_tees[aa]);
		srt.so_ccl.cc_clear(false);
		SORTER_CK(srt.is_unsorted());
		srt.sort_from(srg1, 1);
	}
}

void
sorset::set_all_items_qua_id(long qu_id){
	SORTER_CK(qu_id != 0);
	
	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);
		SORTER_CK(srt.so_vessel == this);
		srt.so_qua_id = qu_id;
	}
}

bool
sorset::ck_all_items_same_qua_id(){
#ifdef FULL_DEBUG
	long ss_id = 0;
	
	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);
		long qu_id = srt.so_qua_id;
		SORTER_CK(qu_id != 0);
		if(ss_id == 0){ ss_id = qu_id; }
		SORTER_CK(qu_id == ss_id);
	}
#endif
	return true;
}

bool 
sort_glb::ck_sorted_sorsets(row<sorset*>& dest_ss){
#ifdef FULL_DEBUG
	row_long_t& all_id = sg_tmp_id_trail;
	all_id.clear();
	
	for(long aa = 0; aa < dest_ss.size(); aa++){
		SORTER_CK(dest_ss[aa] != NULL_PT);
		sorset& srs = *(dest_ss[aa]);
		SORTER_CK(srs.is_final());
		SORTER_CK((aa == 0) || (dest_ss[aa - 1] != &srs));
		SORTER_CK(srs.ck_all_items_same_qua_id());
		
		sortee& srt = srs.first_item();
		sortee& opp = srt.opposite();
		long qu_id = srt.so_qua_id;
		
		SORTER_CK(qu_id != 0);
		
		bool sam_vsl = (srt.so_vessel == opp.so_vessel);
		SORTER_CK(sam_vsl || (qu_id == -(opp.so_qua_id)));
		SORTER_CK(! sam_vsl || (qu_id == opp.so_qua_id));
		
		all_id.push(qu_id);
	}
	SORTER_CK(all_id.is_sorted(cmp_canon_ids));
#endif
	return true;
}


