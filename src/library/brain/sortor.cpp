

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

Copyright (C) 2007-2012, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

sortor.cpp

Classes and that implement a sortor.

--------------------------------------------------------------*/

#include "stack_trace.h"

#include "skeleton.h"
#include "sortor.h"

#include "brain.h"	// only needed for print_sortee & debug of stab_mutual_unique
//include "dbg_sortor.h"	// only needed for elem_sor last decl

char* sortee::CL_NAME = as_pt_char("{sortee}");
char* sorset::CL_NAME = as_pt_char("{sorset}");

DEFINE_GET_DBG_SLV(sort_glb)

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

	//if(! from_pt){
		// NEEDS_brain_h
		os << "tee{";
		os << "(";
		if(so_sorset_consec != INVALID_NATURAL){
			os << so_sorset_consec;
		}
		if((so_walk_consec != 0) && (so_walk_consec != INVALID_NATURAL)){
			os << ".l." << so_walk_consec;
		}
		os << ".c." << so_tee_consec;
		if((so_wlk_consec != 0) && (so_wlk_consec != INVALID_NATURAL)){
			os << ".w." << so_wlk_consec;
		}
		os << ").";
		
		if(so_dbg_me_class == 1){
			os << "q.";
			SORTER_CK(so_me != NULL_PT);
			quanton& qua = *((quanton*)so_me);
			
			neuron* src = qua.get_source();
			qua.print_quanton_base(os, true, qua.qu_tier, src, true);
			
			//os << qua.qu_id; 
			//os << &qua;
			//os << "  ==  " << so_qua_id;
		}
		if(so_dbg_me_class == 2){
			os << "n.";
			SORTER_CK(so_me != NULL_PT);
			//neuron& neu = *((neuron*)so_me);
			//os << &neu;
			os << so_ccl;
		}
		os << "}\n";
		/* */
	//}

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
	DBG_SORTOR_PRT(59, os << "SORTING " << this << " TO " << &nsr);  // prt_sorset

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
	DBG_SORTOR_PRT(58, os << "sort from srt=" << this << " ID=" << curr_id);

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
		DBG_PRT(61, os << " RELEASING_HEAD !!! sortor=" << this);
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
	DBG_PRT(37, os << "releasing sort_glb 0");

	stab_release_all_sorsets();

	SORTER_CK(sg_num_active_ss == 0);
	
	sg_sorsets.clear(true, true);
	sg_free_sorsets.clear();

	SORTER_CK(sg_sorsets.is_empty());

	DBG_PRT(37, os << "RELEASING sort_glb 1");
}

/*
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
				has_diff = true;
			}
			srt.so_walk_consec = consec;
		}

		sorted.push(&srt);
	}
}
*/

void
sort_glb::build_cnf(skeleton_glb& skg, canon_cnf& the_cnf, row<canon_clause*>& the_ccls, 
			bool sorted_cnf)
{
	the_cnf.clear_cnf();
	the_cnf.cf_sorted = sorted_cnf;
	the_cnf.cf_dims = sg_cnf_dims;

	the_ccls.move_to(the_cnf.cf_clauses);
	
	SORTER_CK(! the_cnf.cf_clauses.is_empty());
	SORTER_CK_PRT((the_cnf.cf_dims.dd_tot_ccls == the_cnf.cf_clauses.size()), 
		sort_glb& srg = *this;
		DBG_PRT_ABORT(srg);
		os << " tot=" << the_cnf.cf_dims.dd_tot_ccls << "\n";
		os << " sz=" << the_cnf.cf_clauses.size() << "\n";
	);

	the_cnf.calc_sha();

	SORTER_CK(! the_cnf.cf_sha_str.empty());

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

void
sortee::update_totals(sort_glb& srg, long tgt_sz){
	if(so_ccl.cc_size() == tgt_sz){
		long n_sz = so_ccl.cc_size();

		srg.sg_cnf_dims.update_with_sz(n_sz);

		//if(n_sz == 2){ srg.sg_cnf_dims.dd_tot_twolits++; }
		//srg.sg_cnf_dims.dd_tot_lits += n_sz;

		if(n_sz == 1){ srg.sg_dbg_cnf_tot_onelit++; }
	}
}

void
sort_glb::sort_all_from(row<sortee*>& tees, sort_id_t curr_id, 
						bool add_ccl_id, long ccl_id, bool sort_opps, 
						tgt_ccl_t tgt, sort_glb* dbg_srg, sortee* dbg_srt)
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

		SORTER_CK_PRT(! the_tee.is_alone(),
			brain& brn = get_dbg_brain();
			DBG_PRT_ABORT(brn);
			os << " srg=" << srg << "\n__end_of_srg_\n";
			if(dbg_srg != NULL_PT){
				os << " dbg_srg=" << *dbg_srg << "\n__end_of_dbg_srg_\n";
			}
			if(dbg_srt != NULL_PT){
				os << " dbg_srt=" << *dbg_srt << "\n__end_of_dbg_srt_\n";
			}
			os << "TEES=\n";
			tees.print_row_data(os, true, "\n");
			os << " \n";
			os << " add_ccl_id=" << add_ccl_id << "\n";
			os << " ccl_id=" << ccl_id << "\n";
			os << " curr_id=" << curr_id << "\n";
			os << " (tgt == tc_none) =" << (tgt == tc_none) << "\n";
			os << " the_tee=" << &the_tee << "\n";
		);

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

		//srg.sg_cnf_dims.dd_tot_lits += ccl_sz;
		//if(ccl_sz == 2){ srg.sg_cnf_dims.dd_tot_twolits++; }

		rr2.push(&(the_ccl));
		srg.sg_cnf_dims.update_with_sz(ccl_sz);
	}

	SORTER_CK(srg.sg_cnf_dims.dd_tot_ccls == rr2.size());
	//srg.sg_cnf_dims.dd_tot_ccls = rr2.size();

	return all_consec;
}

comparison
cmp_sortees(sortee* const & srt1, sortee* const & srt2){
	bool s1_ok = (srt1 != NULL_PT);
	bool s2_ok = (srt2 != NULL_PT);
	MARK_USED(s2_ok);
	SORTER_CK(s1_ok == s2_ok);
	if(! s1_ok){ return 0; }
	SORTER_CK(s1_ok);
	SORTER_CK(s2_ok);
	
	long* v1 = srt1->so_cmp_val;
	long* v2 = srt2->so_cmp_val;
	bool v1_ok = (v1 != NULL_PT);
	bool v2_ok = (v2 != NULL_PT);
	MARK_USED(v2_ok);
	SORTER_CK(v1_ok == v2_ok);
	if(! v1_ok){ return 0; }
	SORTER_CK(v1_ok);
	SORTER_CK(v2_ok);
	
	comparison c2 = cmp_long(*v1, *v2);
	return c2;
}

// mutual stab

brain&
sort_glb::get_dbg_brain(){
	brain* pt_brn = get_dbg_brn();
	SORTER_CK(pt_brn != NULL_PT);
	return *pt_brn;
}

ch_string
sort_glb::dbg_prt_margin(bj_ostream& os, bool is_ck){
	ch_string ret = "INVALID_STR_sort_glb::dbg_prt_margin";
	brain* pt_brn = get_dbg_brn();
	if(pt_brn != NULL_PT){
		pt_brn->dbg_prt_margin(os, is_ck);
	}
	return ret;
}

void
sorset::step_mutual_stabilize_rec(sort_glb& srg1, sort_glb& srg2)
{
	// pre checks
	
	step_mutual_op_t oper = srg1.sg_step_mutual_op;
	bool is_wlk_op = ((oper == sm_walk) || (oper == sm_get_ccls)); 
	
	//SORTER_CK((oper == sm_walk) || (&srg1 != &srg2));
	SORTER_CK(is_wlk_op || (&srg1 != &srg2));
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

	row<sorset*>& all_ss = srg1.sg_step_sorsets;

	long& tee_consec = srg1.sg_step_consec;
	bool& has_diff = srg1.sg_step_has_diff;
	bool& all_consec = srg1.sg_step_all_consec;
	row<sortee*>& sorted_tees = srg1.sg_step_sortees;

	row<canon_clause*>& all_ccl = srg1.sg_step_mutual_clauses;

	sort_id_t* pt_stab_consec = &(srg2.sg_curr_stab_consec); // matches sort_from srg param
	bool one_ccl = srg1.sg_one_ccl_per_ss;

	if(oper == sm_with_opps){
		// srg1 is passed to sort from
		pt_stab_consec = &(srg1.sg_curr_stab_consec);
	}

	sort_id_t& curr_stab_consec = *pt_stab_consec;
	SORTER_DBG(sort_id_t old_stb_consec = curr_stab_consec);

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

	bool set_mul_op = is_wlk_op; 
	if(set_mul_op && (srg1.sg_step_first_multiple == NULL_PT) && is_multitem()){
		srg1.sg_step_first_multiple = this;
	}

	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);

		SORTER_CK(&(srt.get_vessel()) == this);
		SORTER_CK((oper == sm_walk) || (srt.so_related != NULL_PT));

		// consec managment

		if(set_tee_consecs){
			if(! has_diff && (tee_consec != srt.so_tee_consec)){
				DBG_SORTOR_PRT(60, os << "first diff=" << bj_eol
					<< srt << " NEW_consec=" << tee_consec);
				has_diff = true;
			}

			SORTER_CK(tee_consec != 0);
			if(oper == sm_walk){
				srt.so_tee_consec = INVALID_NATURAL;
				srt.so_wlk_consec = tee_consec;
			} else {
				srt.so_tee_consec = tee_consec;
				srt.so_wlk_consec = INVALID_NATURAL;
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
			if(srt.is_next_choice(srg1)){
				srg1.sg_step_next_choice = &srt;
			}
		}

		canon_clause& tee_ccl = srt.so_ccl;
		if(oper == sm_get_ccls){
			bool add_it = (! one_ccl || (rgt == fst));
			if(add_it){
				all_ccl.push(&tee_ccl);
				long ccl_sz = tee_ccl.cc_size();
				srg1.sg_cnf_dims.update_with_sz(ccl_sz);
			}
		} 
		if(! is_wlk_op){
			SORTER_CK(oper != sm_get_ccls);
			// clear my ccls
			tee_ccl.cc_clear(false);
		}

		// sort oper

		if(oper == sm_with_neus){
			row<sortee*>& all_mates = srt.so_related->so_mates;
			srg2.sort_all_from(all_mates, curr_stab_consec, false, 0, true, tc_none,
								&srg1, &srt);
		}

		if(oper == sm_with_opps){
			sortee& opp = srt.opposite();
			SORTER_CK_PRT((! opp.is_alone()), 
				brain& brn = srg1.get_dbg_brain();
				DBG_PRT_ABORT(brn);
				os << " SRG1=\n" << srg1 << "\n++++++++++++++++++++++++++++++++\n";
				os << " SRG2=\n" << srg2 << "\n++++++++++++++++++++++++++++++++\n";
				os << " srt=" << srt << "\n";
				os << " srt_alone=" << srt.is_alone() << "\n";
				os << " opp=" << opp << "\n";
				os << " opp_alone=" << opp.is_alone() << "\n";
			);
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
			srg2.sort_all_from(all_mates, curr_stab_consec, true, qua_id, false, tc_mates,
								&srg1, &srt);
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
	SORTER_CK((oper != sm_walk) || (old_stb_consec == curr_stab_consec));
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
	sg_step_next_choice = NULL_PT;
	sg_step_neg_sorsets.clear();
	sg_step_pos_sorsets.clear();

	sg_step_mutual_op = op;
	sg_step_mutual_clauses.clear();

	if(op != sm_walk){
		init_counters();
		sg_dbg_cnf_tot_onelit = 0;

		sg_cnf_clauses.clear();
		sg_got_ccls = false;
	}

	//row<sorset*> sets;
	row<sorset*>& sets = sg_step_prv_sorsets;
	sets.clear();

	SORTER_CK(! sg_step_sorsets.is_empty());
	SORTER_DBG(
		long old_ss_sz = sg_step_sorsets.size();
		bool ss_0_hs = false;
		bool ss_0_hi = false;
		bool ss_0_ee = false;
		if(old_ss_sz == 1){
			sorset* ss_0 = sg_step_sorsets.first();
			SORTER_CK(ss_0 != NULL_PT);
			ss_0_hs = ss_0->has_subsets();
			ss_0_hi = ss_0->has_items();
			ss_0_ee = (! ss_0_hs && ! ss_0_hi);
		}
	);
	sg_step_sorsets.move_to(sets);
	sg_step_sorsets.set_cap(sets.size());
	SORTER_CK(sg_step_sorsets.is_empty());

	SORTER_CK_PRT(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id,
		os << "\n______________\n ABORT_DATA\n";
		os << " sg_curr_stab_consec=" << srg1.sg_curr_stab_consec << "\n";
		os << " sg_dbg_last_id=" << srg1.sg_dbg_last_id << "\n";
	);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	for(long aa = 0; aa < sets.size(); aa++){
		SORTER_CK(sets[aa] != NULL_PT);
		sorset& srs = *(sets[aa]);
		sets[aa] = NULL_PT;

		if(! srg1.has_head()){
			DBG_PRT(61, os << "INITING head of " << &srg1);
			srg1.init_head_ss();
		}
		if(! srg2.has_head()){
			DBG_PRT(61, os << "INITING head of " << &srg2);
			srg2.init_head_ss();
		}
		
		srs.step_mutual_stabilize_rec(srg1, srg2);

		SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
		SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	}
	sets.clear();

	SORTER_CK_PRT(ss_0_ee || (old_ss_sz <= sg_step_sorsets.size()),
		os << "\n_______________\nABORT_DATA\n";
		os << " old_ss_sz=" << old_ss_sz << "\n";
		os << " ssets_sz=" << sg_step_sorsets.size() << "\n";
	);
	SORTER_CK((op == sm_with_opps) || (sg_step_sorsets.size() <= sg_step_sortees.size()));
	//SORTER_CK(sg_cnf_dims.is_dd_virgin());
	SORTER_CK((op == sm_walk) || sg_cnf_clauses.is_empty());
	SORTER_CK((op == sm_get_ccls) || sg_step_mutual_clauses.is_empty());

	if(op == sm_get_ccls){
		sg_step_mutual_clauses.move_to(sg_cnf_clauses);
		SORTER_CK_PRT((sg_cnf_dims.dd_tot_ccls == sg_cnf_clauses.size()), 
			brain& brn = get_dbg_brain();
			DBG_PRT_ABORT(brn);
			os << " tot=" << sg_cnf_dims.dd_tot_ccls << "\n";
			os << " sz=" << sg_cnf_clauses.size() << "\n";
		);
		//sg_cnf_dims.dd_tot_ccls = sg_cnf_clauses.size();
		sg_got_ccls = true;
	}
	if(! srg1.has_head()){
		DBG_PRT(61, os << "INITING head of " << &srg1);
		srg1.init_head_ss();
	}
	if(! srg2.has_head()){
		DBG_PRT(61, os << "INITING head of " << &srg2);
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
sort_glb::stab_mutual_walk(){
	// sets step_sortees
	sort_glb& srg2 = *this;

	DBG(sg_dbg_fst_num_items = sg_dbg_num_items);
	sg_tot_stab_steps = 0;
	
	step_mutual_stabilize(srg2, sm_walk);
	SORTER_CK(has_head());
	SORTER_CK(sg_curr_stab_consec >= sg_dbg_last_id);
}

void
sort_glb::stab_mutual(sort_glb& srg2, bool one_ccl_per_ss){
	stab_mutual_core(srg2);
	stab_mutual_end(srg2, one_ccl_per_ss);
}

void
sort_glb::stab_mutual_core(sort_glb& srg2){
	SORTER_CK(&srg2 != this);
	sort_glb& srg1 = *this;
	
	DBG_PRT(107, os << "STAB_mutual srg1=" << this);
	DBG_PRT(107, os << "INI_SRG1(NEUS)=" << srg1);
	DBG_PRT(107, os << "INI_SRG2(QUAS)=" << srg2);
	
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
		DBG_PRT(107, os << "SRG2(QUAS)=" << srg2);

		srg2.step_opps(srg1);

		srg2.step_quas(srg1);
		bool diff2 = srg2.sg_step_has_diff;
		DBG_PRT(107, os << "SRG1(NEUS)=" << srg1);

		has_diff = (diff1 || diff2);
	}
	SORTER_CK(! srg1.sg_step_has_diff);
	SORTER_CK(! srg2.sg_step_has_diff);
	
	SORTER_CK(srg1.sg_step_sortees.size() == srg1.sg_dbg_fst_num_items);
	SORTER_CK(srg2.sg_step_sortees.size() == srg2.sg_dbg_fst_num_items);

	//stab_mutual_end(srg2, true);
}

void
sort_glb::stab_mutual_end(sort_glb& srg2, bool unique_ccls){
	SORTER_CK(&srg2 != this);
	sort_glb& srg1 = *this;

	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
	
	srg1.join_all_tees_in_head();
	srg2.add_neg_and_pos_to(srg2.sg_step_sorsets);
	srg2.step_quas(srg1);
	
	SORTER_CK(srg1.sg_step_sorsets.size() == 1);
	
	srg1.sg_one_ccl_per_ss = unique_ccls;
	//srg1.step_mutual_stabilize(srg2, sm_get_ccls);
	srg1.step_mutual_stabilize(srg1, sm_get_ccls);

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
	SORTER_CK(sg_step_next_choice != NULL_PT);
	SORTER_CK(sg_step_next_choice->so_vessel == sg_step_first_multiple);

	sorset& ss_one = *sg_step_first_multiple;
	MARK_USED(ss_one);
	SORTER_CK(ss_one.is_multitem());

	sort_id_t& stab_consec = srg1.sg_curr_stab_consec;

	stab_consec++;

	//sortee& to_sort = ss_one.first_item();
	sortee& to_sort = *sg_step_next_choice;
	SORTER_CK(to_sort.so_cmp_val != NULL_PT);

	DBG_SORTOR_PRT(57, os << "choose one=" << to_sort);

	to_sort.sort_from(srg1, stab_consec);
	
	SORTER_CK(srg1.has_head());
	SORTER_CK(srg2.has_head());
	SORTER_CK(srg1.sg_curr_stab_consec >= srg1.sg_dbg_last_id);
	SORTER_CK(srg2.sg_curr_stab_consec >= srg2.sg_dbg_last_id);
}

void
sort_glb::stab_mutual_unique(sort_glb& srg2, neuromap* dbg_nmp){
	SORTER_CK(&srg2 != this);
	sort_glb& srg1 = *this;

	SORTER_DBG(sg_dbg_nmp = dbg_nmp);
	SORTER_DBG(srg2.sg_dbg_nmp = dbg_nmp);
	
	SORTER_CK(srg1.has_head());
	SORTER_CK(srg2.has_head());
	
	srg2.sg_step_all_consec = false;

	bool all_consec = false;
	while(! all_consec){
		stab_mutual_core(srg2);

		all_consec = srg2.sg_step_all_consec;

		if(! all_consec){
			srg2.stab_mutual_choose_one(srg1);
		}
	}
	SORTER_CK(srg2.sg_step_all_consec);
	
	stab_mutual_end(srg2, true);

	DBG_PRT(63, 
			os << " UNIQUE cnf=" << bj_eol; sg_cnf_clauses.print_row_data(os, true, "\n");
	);
}

canon_cnf&
sort_glb::get_final_cnf(skeleton_glb& skg, bool sorted_cnf, long precalc_tot_vars)
{
	// after any stab_mutual
	SORTER_CK(! sg_cnf_clauses.is_empty());
	/*if(sg_cnf_clauses.is_empty()){
		sort_glb& srg1 = *this;
		
		sg_one_ccl_per_ss = one_ccl_per_ss;
		step_mutual_stabilize(srg1, sm_get_ccls);

		//SORTER_CK(precalc_tot_vars != 0);
		sg_cnf_dims.dd_tot_vars = precalc_tot_vars;
	}*/
	SORTER_CK(! sg_cnf_clauses.is_empty());
	DBG_PRT(64, os << " get_cnf=" << this << "\n";
		sg_cnf_clauses.print_row_data(os, true, "\n");
	);

	SORTER_CK(! sg_one_ccl_per_ss || (sg_step_sorsets.size() == sg_cnf_clauses.size()));
	SORTER_CK_PRT((sg_one_ccl_per_ss || (sg_step_sortees.size() == sg_cnf_clauses.size())),
		brain& brn = get_dbg_brain();
		DBG_PRT_ABORT(brn);
		os << " tee_sz=" << sg_step_sortees.size() << "\n";
		os << " cnf_sz=" << sg_cnf_clauses.size() << "\n";
		os << " sg_got_ccls=" << sg_got_ccls << "\n";
		sg_cnf_clauses.print_row_data(os, true, "\n");
	);

	build_cnf(skg, sg_cnf_step, sg_cnf_clauses, sorted_cnf);
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

bool
sort_glb::ck_stab_inited(){
#ifdef FULL_DEBUG
	SORTER_CK(sg_num_active_ss == 1);
	SORTER_CK(sg_dbg_num_items == 0);
	SORTER_CK(sg_step_sorsets.size() == 1);
	SORTER_CK(has_head());
	SORTER_CK(&(get_head_ss()) == sg_step_sorsets.first());
	SORTER_CK(! sg_step_sortees.is_empty() || get_head_ss().is_ss_virgin());
	SORTER_CK(sg_curr_stab_consec == 1);
	//SORTER_CK(! sg_one_ccl_per_ss);
	SORTER_CK(sg_dbg_num_saved_consec == 0);
#endif
	return true;
}

void
sort_glb::stab_mutual_init(){
	//sg_one_ccl_per_ss = false;

	stab_release_all_sorsets();
	sorset& hd_nsr = init_head_ss();
	
	SORTER_CK(sg_step_sorsets.is_empty());
	sg_step_sorsets.push(&hd_nsr);
	sg_curr_stab_consec++;
	
	SORTER_CK(ck_stab_inited());
	
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

	//stab_mutual_walk();
	
	os << "SORTEES=\n";
	os << sg_step_sortees << "\n";
	//os << "SORSETS=\n";
	//os << sg_step_sorsets << "\n"
	
	os << "PRV STEP SORSETS=\n";
	sg_step_prv_sorsets.print_row_data(os, true, "\n.......................\n");

	os.flush();
	return os;
}

bool
sortee::is_next_choice(sort_glb& srg1){
	if(so_vessel != srg1.sg_step_first_multiple){
		return false;
	}
	if(srg1.sg_step_next_choice == NULL_PT){
		return true;
	}
	//comparison c1 = cmp_sortees(srg1.sg_step_next_choice, this);
	//if(c1 > 0){ return true; }
	return false;
}

/*
template
bool srt_row_as_colors<neuron>(sort_glb& srg, row<sortee*>& rr1, row<neuron*>& rr2, 
				row<long>& cols, tee_id_t consec_kk, bool unique_ccls);

template
bool srt_row_as_colors<quanton>(sort_glb& srg, row<sortee*>& rr1, row<quanton*>& rr2, 
				row<long>& cols, tee_id_t consec_kk, bool unique_ccls);

template
bool srt_row_as_colors<elem_sor>(sort_glb& srg, row<sortee*>& rr1, row<elem_sor*>& rr2, 
				row<long>& cols, tee_id_t consec_kk, bool unique_ccls);
*/

