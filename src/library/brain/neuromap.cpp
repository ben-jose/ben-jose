

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

neuromap.cpp  

neuromap class.

--------------------------------------------------------------*/

#include "brain.h"
#include "solver.h"

char* neuromap::CL_NAME = as_pt_char("{neuromap}");

bool
neuromap::map_find(){
	DBG(
		brain& brn = get_brn();
		bool do_finds = (brn.get_solver().slv_dbg.F > 0);
		if(do_finds){
			return map_oper(mo_find);
		}
		return false;
	)
	NOT_DBG(return map_oper(mo_find);)
	return false;
}

bool
neuromap::map_write(){
	DBG(
		brain& brn = get_brn();
		bool do_save = (brn.get_solver().slv_dbg.W > 0);
		if(do_save){
			return map_oper(mo_save);
		}
		return false;
	)
	NOT_DBG(return map_oper(mo_save);)
}

void
neuromap::map_set_all_marks_and_spots(){
	if(na_has_marks_and_spots){
		return;
	}
	if(na_submap != NULL_PT){
		na_submap->map_set_all_marks_and_spots();
	}
	brain& brn = get_brn();
	set_marks_and_spots_of(brn, na_forced);
	na_has_marks_and_spots = true;
}

void
neuromap::map_reset_all_marks_and_spots(){
	if(! na_has_marks_and_spots){
		return;
	}
	if(na_submap != NULL_PT){
		na_submap->map_reset_all_marks_and_spots();
	}
	brain& brn = get_brn();
	reset_marks_and_spots_of(brn, na_forced);
	na_has_marks_and_spots = false;
}

void
neuromap::map_get_all_quas(row_quanton_t& all_quas){
	if(na_submap == NULL_PT){
		all_quas.clear();
	} else {
		na_submap->map_get_all_quas(all_quas);
	}
	append_all_trace_quas(na_forced, all_quas);
}

void
neuromap::map_get_all_forced_neus(row<neuron*>& all_neus){
	if(na_submap == NULL_PT){
		all_neus.clear();
	} else {
		na_submap->map_get_all_forced_neus(all_neus);
	}
	append_all_trace_neus(na_forced, all_neus);
}

void
neuromap::map_get_all_non_forced_neus(row<neuron*>& all_neus, bool with_clear){
	if(na_submap == NULL_PT){
		if(with_clear){
			all_neus.clear();
		}
	} else {
		na_submap->map_get_all_non_forced_neus(all_neus, with_clear);
	}
	na_non_forced.append_to(all_neus);
}

quanton*	
neuromap::map_choose_quanton(){ 
	brain& brn = get_brn();
	row_quanton_t& all_quas = brn.br_tmp_choose;
	all_quas.clear();
	map_get_all_quas(all_quas);
	
	quanton* qua = NULL;
	long max_ii = all_quas.last_idx();
	for(long ii = max_ii; ii >= 0; ii--){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		quanton& qua_ii = *(all_quas[ii]);
		if(qua_ii.get_charge() == cg_neutral){
			if(! qua_ii.is_semi_mono()){
				qua = &qua_ii; 
				break;
			}
			//if(qua == NULL_PT){ qua = qua_ii; }
		}
	}

	BRAIN_CK_0((qua == NULL) || (qua->get_charge() == cg_neutral));
	BRAIN_CK_0((qua == NULL) || 
			(qua->qu_spin == cg_positive) || (qua->qu_spin == cg_negative));
	return qua;
}

bool
neuromap::map_ck_all_qu_dominated(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	row_quanton_t& all_quas = brn.br_tmp_qu_dom;
	all_quas.clear();
	map_get_all_quas(all_quas);
	for(long ii = 0; ii < all_quas.size(); ii++){
		quanton* qua = all_quas[ii];
		MARK_USED(qua);
		BRAIN_CK(qua != NULL_PT);
		BRAIN_CK(qua->in_qu_dominated(brn));
	}
#endif
	return true;
}

bool
neuromap::map_ck_all_ne_dominated(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	row<neuron*>& all_neus = brn.br_tmp_ne_dom;
	all_neus.clear();
	map_get_all_neus(all_neus);
	for(long ii = 0; ii < all_neus.size(); ii++){
		BRAIN_CK(all_neus[ii] != NULL_PT);
		neuron& neu = *(all_neus[ii]);
		MARK_USED(neu);
		BRAIN_CK(neu.in_ne_dominated(brn));
	}
#endif
	return true;
}

void
neuromap::map_activate(){
	brain& brn = get_brn();

	BRAIN_CK(! is_na_virgin());
	BRAIN_CK(! na_active);

	BRAIN_CK(map_ck_all_qu_dominated());
	BRAIN_CK(map_ck_all_ne_dominated());

	brn.br_maps_active.push(this);
	na_active = true;

	map_set_all_qu_curr_dom();
	map_set_all_ne_curr_dom();

	BRAIN_CK(map_ck_all_qu_dominated());
	BRAIN_CK(map_ck_all_ne_dominated());

	BRAIN_CK(! brn.br_maps_active.is_empty());
	BRAIN_CK(brn.br_maps_active.last() == this);

	DBG_PRT(110, os << "ACTIVATING " << this);
}

void
neuromap::map_deactivate(){
	brain& brn = get_brn();
	
	//DBG_PRT(110, os << "DEACTivating " << this << bj_eol << STACK_STR);
	DBG_PRT(110, os << "DEACTivating " << this);

	BRAIN_CK(na_active);
	BRAIN_CK(! brn.br_maps_active.is_empty());

	BRAIN_CK(brn.br_maps_active.last() == this);
	BRAIN_CK(map_ck_all_qu_dominated());
	BRAIN_CK(map_ck_all_ne_dominated());

	brn.br_maps_active.pop();
	na_active = false;

	map_reset_all_qu_curr_dom();
	map_reset_all_ne_curr_dom();

	BRAIN_CK(map_ck_all_qu_dominated());
	BRAIN_CK(map_ck_all_ne_dominated());
	
	na_mates.let_go();
	if(na_is_head){
		full_release(this);
		BRAIN_CK(! na_is_head);
	}
}

void
neuromap::map_set_all_qu_curr_dom(){
	brain& brn = get_brn();
	row_quanton_t& all_quas = brn.br_tmp_qu_dom;
	all_quas.clear();
	map_get_all_quas(all_quas);
	set_all_qu_nemap(all_quas, this);
}

void
neuromap::map_reset_all_qu_curr_dom(){
	brain& brn = get_brn();
	neuromap* upper_map = brn.get_last_upper_map();
	row_quanton_t& all_quas = brn.br_tmp_qu_dom;
	all_quas.clear();
	map_get_all_quas(all_quas);
	set_all_qu_nemap(all_quas, upper_map);
}

void
neuromap::map_set_all_ne_curr_dom(){
	brain& brn = get_brn();
	row<neuron*>& all_neus = brn.br_tmp_ne_dom;
	all_neus.clear();
	map_get_all_neus(all_neus);
	set_all_ne_nemap(all_neus, this);
}

void
neuromap::map_reset_all_ne_curr_dom(){
	brain& brn = get_brn();
	neuromap* upper_map = brn.get_last_upper_map();
	row<neuron*>& all_neus = brn.br_tmp_ne_dom;
	all_neus.clear();
	map_get_all_neus(all_neus);
	set_all_ne_nemap(all_neus, upper_map);
}

bool
quanton::in_qu_dominated(brain& brn){
	neuromap* up_dom = brn.get_last_upper_map();
	bool in_dom = (up_dom == qu_curr_nemap);

	return in_dom;
}

void
quanton::make_qu_dominated(brain& brn){
	while(! in_qu_dominated(brn)){
		brn.deactivate_last_map();
	}
}

bool
neuron::in_ne_dominated(brain& brn){
	if(! ne_original){
		return true;
	}
	neuromap* up_dom = brn.get_last_upper_map();
	bool in_dom = (up_dom == ne_curr_nemap);
	return in_dom;
}

void
neuron::make_ne_dominated(brain& brn){
	while(! in_ne_dominated(brn)){
		brn.deactivate_last_map();
	}
}

void
neuromap::set_all_filled_in_propag(){
	brain& brn = get_brn();
	
	row_quanton_t& nmp_quas = brn.br_tmp_qu_fill_nmp;
	nmp_quas.clear();
	append_all_trace_quas(na_forced, nmp_quas);
	
	for(int aa = 0; aa < nmp_quas.size(); aa++){
		BRAIN_CK(nmp_quas[aa] != NULL_PT);
		quanton& qua = *(nmp_quas[aa]);
		quanton& opp = qua.opposite();
	
		qua.qu_full_charged.append_to(na_all_filled_in_propag);
		opp.qu_full_charged.append_to(na_all_filled_in_propag);
	}
}

void
neuromap::full_release(neuromap* nmp){
	if(nmp == NULL_PT){
		return;
	}
	full_release(nmp->na_submap);
	
	brain& brn = nmp->get_brn();
	brn.release_neuromap(*nmp);
}

void
neuromap::dbg_get_fo_upper_quas(row_quanton_t& fo_upper_quas){
	fo_upper_quas.clear();
	
	long min_ti = get_min_tier();
	BRAIN_CK(min_ti != INVALID_TIER);
	
	brain& brn = get_brn();
	row<neuron*>&	all_neus = brn.br_tmp_fill_non_forced;
	map_get_all_forced_neus(all_neus);
	
	BRAIN_CK(brn.br_qu_tot_note3 == 0);
	
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		
		for(long bb = 0; bb < neu.fib_sz(); bb++){
			BRAIN_CK(neu.ne_fibres[bb] != NULL_PT);
			quanton& qua = *(neu.ne_fibres[bb]);
			long qti = qua.qu_tier;
			BRAIN_CK(qti != INVALID_TIER);
			
			bool is_upper = (qti < min_ti);
			
			if(is_upper && ! qua.has_note3()){
				BRAIN_CK(qua.is_neg());
				qua.set_note3(brn);
				fo_upper_quas.push(&qua);
			}
		}
	}
	
	reset_all_note3(brn, fo_upper_quas);

	BRAIN_CK(brn.br_qu_tot_note3 == 0);
}

void
neuromap::map_get_all_upper_quas(notekeeper& nkpr, row_quanton_t& all_upper_quas){
	brain& brn = get_brn();
	MARK_USED(brn);
	
	all_upper_quas.clear();
	
	nkpr.dk_quas_lyrs.get_all_ordered_quantons(na_fo_upper_quas);
	na_fo_upper_quas.append_to(all_upper_quas);

	nkpr.nk_append_not_noted(na_nf_upper_quas, all_upper_quas);
	
	DBG(
		row_quanton_t& all_fo_upp = brn.br_tmp_neu_upper_qu;
		dbg_get_fo_upper_quas(all_fo_upp);
		BRAIN_CK(same_quantons_note3(brn, all_fo_upp, na_fo_upper_quas));
	)
}

void
coloring::save_colors_from(sort_glb& neus_srg, sort_glb& quas_srg){
	SORTER_CK(! neus_srg.sg_step_has_diff);
	SORTER_CK(! quas_srg.sg_step_has_diff);

	co_all_neu_consec = srt_row_as_colors<neuron>(neus_srg.sg_step_sortees, 
												  co_neus, co_neu_colors);
	co_all_qua_consec = srt_row_as_colors<quanton>(quas_srg.sg_step_sortees, 
												   co_quas, co_qua_colors);

	BRAIN_CK(ck_cols());
}

bool
coloring::has_diff_col_than_prev(row<long>& the_colors, long col_idx){
	BRAIN_CK(the_colors.is_valid_idx(col_idx));
	bool diff_col = false;
	if(col_idx > 0){
		long col0 = the_colors[col_idx - 1];
		long col1 = the_colors[col_idx];
		BRAIN_CK(col0 >= 0);
		BRAIN_CK(col1 >= 0);
		if(col0 != col1){
			BRAIN_CK(col0 < col1);
			diff_col = true;
		}
	}
	return diff_col;
}

void
coloring::load_colors_into(brain& brn, sort_glb& neus_srg, sort_glb& quas_srg, 
						   dima_dims& dims)
{
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row<neuron*>&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;

	//BRAIN_CK(qua_colors.is_sorted(cmp_long));
	//BRAIN_CK(neu_colors.is_sorted(cmp_long));

	BRAIN_CK(brn.br_qu_tot_note4 == 0);
	BRAIN_CK(neus_srg.sg_dbg_num_items == 0);
	BRAIN_CK(quas_srg.sg_dbg_num_items == 0);

	sort_id_t& quas_consec = neus_srg.sg_curr_stab_consec;
	quas_consec++;

	DBG(
		for(long aa = 0; aa < all_quas.size(); aa++){
			BRAIN_CK(all_quas[aa] != NULL_PT);
			quanton& qua = *(all_quas[aa]);
			BRAIN_CK(qua.qu_tee.is_unsorted());
		}
	);

	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);

		BRAIN_CK(! qua.is_note4());
		qua.set_note4(brn);

		bool inc_consec = has_diff_col_than_prev(qua_colors, aa);
		if(inc_consec){ quas_consec++; }

		qua.reset_and_add_tee(quas_srg, quas_consec);
	}

	sort_id_t& neus_consec = quas_srg.sg_curr_stab_consec;
	neus_consec++;

	for(long bb = 0; bb < all_neus.size(); bb++){
		BRAIN_CK(all_neus[bb] != NULL_PT);
		neuron& neu = *(all_neus[bb]);

		bool inc_consec = has_diff_col_than_prev(neu_colors, bb);
		if(inc_consec){ neus_consec++; }

		sortee& neu_tee = neu.ne_tee;
		neu_tee.so_ccl.cc_clear(false);
		neu_tee.so_tee_consec = 0;

		neu.fill_mutual_sortees(brn);

		row<sortee*>& neu_mates = neu.ne_reltee.so_mates;
		if(! neu_mates.is_empty()){
			BRAIN_CK(neu_tee.is_unsorted());
			neu_tee.sort_from(neus_srg, neus_consec);
		}
	}

	reset_all_note4(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note4 == 0);
	BRAIN_CK(ck_cols());
}

void
coloring::add_coloring(brain& brn, coloring& clr){
	BRAIN_CK(ck_cols());
	BRAIN_CK(clr.ck_cols());

	row_quanton_t&	all_quas1 = co_quas;
	row<long>&	qua_colors1 = co_qua_colors;
	row_quanton_t&	all_quas2 = clr.co_quas;
	row<long>&	qua_colors2 = clr.co_qua_colors;

	long qua_col = 0;
	if(! co_qua_colors.is_empty()){
		qua_col = co_qua_colors.last();
	}

	qua_col++;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	DBG(set_all_note1(brn, all_quas1));

	for(long aa = 0; aa < all_quas2.size(); aa++){
		BRAIN_CK(all_quas2[aa] != NULL_PT);
		quanton& qua = *(all_quas2[aa]);

		bool inc_consec = has_diff_col_than_prev(qua_colors2, aa);
		if(inc_consec){ qua_col++; }

		BRAIN_CK(! qua.has_note1());

		all_quas1.push(&qua);
		qua_colors1.push(qua_col);
	}

	DBG(reset_all_note1(brn, all_quas1));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	row<neuron*>&	all_neus1 = co_neus;
	row<long>&	neu_colors1 = co_neu_colors;
	row<neuron*>&	all_neus2 = clr.co_neus;
	row<long>&	neu_colors2 = clr.co_neu_colors;
	BRAIN_CK(all_neus2.size() == neu_colors2.size());

	long neu_col = 0;
	if(! co_neu_colors.is_empty()){
		neu_col = co_neu_colors.last();
	}

	neu_col++;

	for(long bb = 0; bb < all_neus2.size(); bb++){
		BRAIN_CK(all_neus2[bb] != NULL_PT);
		neuron& neu = *(all_neus2[bb]);

		bool inc_consec = has_diff_col_than_prev(neu_colors2, bb);
		if(inc_consec){ neu_col++; }

		all_neus1.push(&neu);
		neu_colors1.push(neu_col);
	}

	BRAIN_CK(ck_cols());
}

void
coloring::move_co_to(coloring& col2){
	BRAIN_CK(col2.is_co_virgin());

	co_quas.move_to(col2.co_quas);
	co_qua_colors.move_to(col2.co_qua_colors);
	col2.co_all_qua_consec = co_all_qua_consec;

	co_neus.move_to(col2.co_neus);
	co_neu_colors.move_to(col2.co_neu_colors);
	col2.co_all_neu_consec = co_all_neu_consec;

	col2.co_szs_idx = co_szs_idx;

	init_coloring();
}

void
coloring::copy_co_to(coloring& col2){
	BRAIN_CK(col2.is_co_virgin());

	co_quas.copy_to(col2.co_quas);
	co_qua_colors.copy_to(col2.co_qua_colors);
	col2.co_all_qua_consec = co_all_qua_consec;

	co_neus.copy_to(col2.co_neus);
	co_neu_colors.copy_to(col2.co_neu_colors);
	col2.co_all_neu_consec = co_all_neu_consec;

	col2.co_szs_idx = co_szs_idx;
}

void
coloring::get_initial_sorting_coloring(brain& brn, coloring& ini_clr, bool fill_neus){
	BRAIN_CK(this != &ini_clr);
	BRAIN_CK(ck_cols());

	ini_clr.init_coloring();

	row_quanton_t&	my_quas = co_quas;

	row_quanton_t&	all_quas = ini_clr.co_quas;
	row<long>&	qua_colors = ini_clr.co_qua_colors;

	row<neuron*>&	all_neus = ini_clr.co_neus;
	row<long>&	neu_colors = ini_clr.co_neu_colors;

	row_quanton_t	all_opp;

	BRAIN_CK(ck_cols());
	BRAIN_CK(all_quas.is_empty());
	BRAIN_CK(all_neus.is_empty());
	
	all_quas.clear();
	all_neus.clear();

	// select quas

	BRAIN_CK(brn.br_qu_tot_note2 == 0);

	long curr_col = 0;

	for(long aa = 0; aa < my_quas.size(); aa++){
		BRAIN_CK(my_quas[aa] != NULL_PT);
		quanton& qua = *(my_quas[aa]);

		if(! qua.has_note2()){
			qua.set_note2(brn);

			quanton& opp = qua.opposite();
			all_quas.push(&qua);
			all_opp.push(&opp);

			curr_col++;
			qua_colors.push(curr_col);
			curr_col++;
			qua_colors.push(curr_col);
		}
	}

	reset_all_note2(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note2 == 0);

	all_opp.append_to(all_quas);

	if(fill_neus){
		row<neuron*>&	my_neus = co_neus;
		my_neus.copy_to(all_neus);
		neu_colors.fill(1, all_neus.size());
	}

	BRAIN_CK(ini_clr.ck_cols());
}

void
quanton::reset_and_add_tee(sort_glb& quas_srg, sort_id_t quas_consec){
	BRAIN_CK(qu_tee.so_related == &qu_reltee);
	qu_tee.reset_sort_info();
	qu_tee.sort_from(quas_srg, quas_consec);
}

void
neuron::fill_mutual_sortees(brain& brn){
	row<sortee*>& neu_mates = ne_reltee.so_mates;

	neu_mates.clear();
	BRAIN_CK(ne_reltee.so_opposite == NULL_PT);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);

		if(qua.has_note4()){
			sortrel& qua_sre = qua.qu_reltee;
			row<sortee*>& qua_mates = qua_sre.so_mates;
			qua_mates.push(&ne_tee);
			neu_mates.push(&(qua.qu_tee));
		}
	}
}

void
split_tees(sort_glb& srg, row<sortee*>& sorted_tees, row<sortee*>& sub_tees, 
		row<canon_clause*>& ccls_in, row<canon_clause*>& ccls_not_in)
{
	ccls_in.clear();
	ccls_not_in.clear();

	long d_sz = sorted_tees.size() - sub_tees.size();
	ccls_not_in.set_cap(d_sz);

	ccls_in.set_cap(sub_tees.size());

	BRAIN_CK(srg.sg_tot_ss_marks == 0);

	// result in all_ccls
	for(long aa = 0; aa < sub_tees.size(); aa++){
		BRAIN_CK(sub_tees[aa] != NULL_PT);
		sortee& rmv1 = *(sub_tees[aa]);
		sorset& ss1 = rmv1.get_vessel();
		ss1.set_ss_mark(srg);	
	}

	long last_tee_id = INVALID_NATURAL;

	for(long bb = 0; bb < sorted_tees.size(); bb++){
		BRAIN_CK(sorted_tees[bb] != NULL_PT);
		sortee& tee1 = *(sorted_tees[bb]);
		
		if(tee1.so_tee_consec != last_tee_id){
			BRAIN_CK(last_tee_id < tee1.so_tee_consec);
			last_tee_id = tee1.so_tee_consec;

			sorset& ss2 = tee1.get_vessel();
			if(ss2.ss_mark){
				ccls_in.push(&(tee1.so_ccl));
			} else {
				ccls_not_in.push(&(tee1.so_ccl));
			}
		}
	}

	for(long cc = 0; cc < sub_tees.size(); cc++){
		BRAIN_CK(sub_tees[cc] != NULL_PT);
		sortee& rmv1 = *(sub_tees[cc]);
		sorset& ss1 = rmv1.get_vessel();
		ss1.reset_ss_mark(srg);	
	}

	BRAIN_CK(srg.sg_tot_ss_marks == 0);
}

// MAP_OPER_CODE

bool
neuromap::map_oper(mem_op_t mm){
	brain& brn = get_brn();
	brn.init_mem_tmps();

	bool prep_ok = map_prepare_mem_oper(mm);

	if(! prep_ok){
		DBG_PRT(122, os << "map_oper skip (prepare == false)");
		return false;
	}
	DBG_PRT(122, os << "map_oper_go");

	skeleton_glb& skg = brn.get_skeleton();

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	tmp_diff_cnf.cf_dbg_shas.push(tmp_tauto_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_diff_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_guide_cnf.cf_sha_str + "\n");

	BRAIN_CK(! tmp_tauto_cnf.has_phase_path());
	BRAIN_CK(tmp_diff_cnf.has_phase_path());
	BRAIN_CK(! tmp_guide_cnf.has_phase_path());

	bj_output_t& o_info = brn.get_out_info();
	
	bool oper_ok = false;
	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();

		long fst_idx = tmp_diff_cnf.first_vnt_i_super_of(skg, &iinfo);

		///////  start of debug of NO DEBUG

		DBG(
			if(brn.br_dbg.dbg_clean_code && (brn.br_dbg.dbg_canon_find_id == 10)){
				bj_ostream& os = bj_dbg;

				os << "TRYING to find cnf=" << bj_eol << tmp_diff_cnf << bj_eol
					<< "SHAS=" << bj_eol << tmp_diff_cnf.cf_dbg_shas << bj_eol
					//<< "fst_vpth='" << fst_vpth << "'" << bj_eol
					<< "fst_idx= " << fst_idx << bj_eol
					<< "find_id= " << brn.br_dbg.dbg_canon_find_id << bj_eol;
			}
		)

		///////  end of debug of NO DEBUG

		oper_ok = (fst_idx != INVALID_NATURAL);
		if(oper_ok){
			ch_string fst_vpth = tmp_diff_cnf.get_variant_path(skg, fst_idx, skg.in_verif());
			DBG_PRT(115, 
				os << "found cnf=" << bj_eol << tmp_diff_cnf << "FOUND CNF" << bj_eol
				<< "SHAS=" << bj_eol << tmp_diff_cnf.cf_dbg_shas << bj_eol
				<< "fst_vpth='" << fst_vpth << "'" << bj_eol
				<< "find_id= " << brn.br_dbg.dbg_canon_find_id);
			DBG_COMMAND(115, getchar());
			
			o_info.bjo_sub_cnf_hits++;

			row<neuron*>& all_tmp_found = brn.br_tmp_found_neus;
			all_tmp_found.clear();

			// BJ_FIX_THIS
			ccl_row_as<neuron>(tmp_diff_cnf.cf_clauses, all_tmp_found, true);
			
			for(long aa = 0; aa < all_tmp_found.size(); aa++){
				BRAIN_CK(all_tmp_found[aa] != NULL_PT);
				neuron& neu = *(all_tmp_found[aa]);			
				neu.ne_recoil_tk.update_ticket(brn);
			}			
		}
	} else {
		ref_strs& phd = tmp_diff_cnf.cf_phdat;
		
		BRAIN_CK(mm == mo_save);
		BRAIN_CK(! skg.kg_save_canon || phd.has_ref());

		ch_string lk_dir = phd.lck_nam();
		int fd_lk = skg.get_write_lock(lk_dir);

		if(fd_lk != -1){
			ch_string sv_pth1 = tmp_tauto_cnf.get_cnf_path() + SKG_CANON_NAME;
			ch_string sv_pth2 = tmp_tauto_cnf.get_cnf_path() + SKG_DIFF_NAME;
			ch_string sv_pth3 = tmp_tauto_cnf.get_cnf_path() + SKG_GUIDE_NAME;

			tmp_tauto_cnf.save_cnf(skg, sv_pth1);
			oper_ok = tmp_diff_cnf.save_cnf(skg, sv_pth2);
			tmp_guide_cnf.save_cnf(skg, sv_pth3);

			o_info.bjo_saved_targets++;
			
			//BRAIN_CK(! oper_ok || srg_forced.base_path_exists(skg));
			
			skg.drop_write_lock(lk_dir, fd_lk);

			ch_string pth1 = phd.pd_ref1_nam;
			ch_string pth2 = phd.pd_ref2_nam;

			BRAIN_CK((pth1 == "") || skg.find_skl_path(skg.as_full_path(pth1)));
			BRAIN_CK((pth2 == "") || skg.find_skl_path(skg.as_full_path(pth2)));

			DBG_CHECK_SAVED(
				if(oper_ok){
					ch_string sv1_name = skg.as_full_path(sv_pth1);
					dbg_run_satex_on(brn, sv1_name);
				}
			);			
		}
	}
	return oper_ok;
}

bool
neuromap::map_prepare_mem_oper(mem_op_t mm){
	brain& brn = get_brn();
	DBG_PRT(110, os << "map_mem_oper=" << ((void*)this));
	BRAIN_CK(map_ck_guides(dbg_call_1));
	DBG_PRT(110, os << "map_mem_oper=" << this);

	map_set_stab_guide();
	
	coloring& guide_col = ma_guide_col;
	MARK_USED(guide_col);
	
	BRAIN_CK(! guide_col.is_co_virgin());
	BRAIN_CK(map_ck_guides(dbg_call_3));
	BRAIN_CK(map_ck_contained_in(guide_col, dbg_call_1));

	// stab uni_guide
	
	skeleton_glb& skg = brn.get_skeleton();
	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;

	canon_cnf& cnf1 = neus_srg.stab_mutual_get_cnf(skg, PHASE_1_COMMENT, false);

	BRAIN_CK(! cnf1.cf_phdat.has_ref());

	ref_strs phtd;
	row_str_t dbg_shas;

	phtd.pd_ref1_nam = cnf1.get_ref_path();	// stab guide 
	phtd.pd_ref2_nam = cnf1.get_lck_path();
	
	dbg_shas.push(cnf1.cf_sha_str + "\n");

	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();
		
		ch_string find_ref = phtd.pd_ref1_nam;
		ch_string pth1 = skg.as_full_path(find_ref);
		bool found1 = skg.find_skl_path(pth1, &iinfo);
		
		bj_output_t& o_info = brn.get_out_info();
		if(! found1){ 
			o_info.bjo_quick_discards++;
			return false; 
		}
		o_info.bjo_num_finds++;
	}

	neus_srg.stab_mutual_unique(quas_srg);
	//canon_cnf& cnf2 = neus_srg.stab_mutual_get_cnf(skg, PHASE_2_COMMENT, false);

	coloring uni_guide_col(&brn);
	uni_guide_col.save_colors_from(neus_srg, quas_srg);
	BRAIN_CK(uni_guide_col.co_all_qua_consec);
	BRAIN_CK(uni_guide_col.co_all_neu_consec);
	BRAIN_CK(map_ck_contained_in(uni_guide_col, dbg_call_2));

	//DBG_PRT_COND(67, (mm == mo_find), map_dbg_prt(os, mm, brn));
	row<sortee*>& guide_tees = brn.br_tmp_wrt_guide_tees;
	neus_srg.sg_step_sortees.move_to(guide_tees);

	// stab uni_colors

	dima_dims dims1;
	coloring ini_tau_col(&brn);

	sort_glb& fnl_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& fnl_qu_srg = brn.br_tauto_quas_srg;

	map_get_initial_tauto_coloring(uni_guide_col, ini_tau_col, (mm == mo_save));

	BRAIN_CK(map_ck_contained_in(ini_tau_col, dbg_call_3));

	brn.all_mutual_init();
	ini_tau_col.load_colors_into(brn, fnl_ne_srg, fnl_qu_srg, dims1);

	fnl_ne_srg.stab_mutual(fnl_qu_srg);
	BRAIN_CK(fnl_qu_srg.sg_step_all_consec);

	canon_cnf& tauto_cnf = fnl_ne_srg.stab_mutual_get_cnf(skg, FINAL_COMMENT, true);

	dbg_shas.push(tauto_cnf.cf_sha_str + "\n");

	row<sortee*>& tauto_tees = brn.br_tmp_wrt_tauto_tees;
	fnl_ne_srg.sg_step_sortees.move_to(tauto_tees);

	// init write ccls 

	brn.init_mem_tmps();

	row<canon_clause*>& 	tmp_tauto_ccls = brn.br_tmp_wrt_tauto_ccls;
	row<canon_clause*>& 	tmp_guide_ccls = brn.br_tmp_wrt_guide_ccls;
	row<canon_clause*>& 	tmp_diff_ccls = brn.br_tmp_wrt_diff_ccls;

	split_tees(fnl_ne_srg, tauto_tees, guide_tees, tmp_guide_ccls, tmp_diff_ccls);
	tauto_cnf.cf_clauses.move_to(tmp_tauto_ccls);

	// init write cnfs

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	tmp_tauto_cnf.init_with(skg, tmp_tauto_ccls);
	tmp_diff_cnf.init_with(skg, tmp_diff_ccls);
	tmp_guide_cnf.init_with(skg, tmp_guide_ccls);

	tmp_diff_cnf.cf_phdat = phtd;
	dbg_shas.move_to(tmp_diff_cnf.cf_dbg_shas);

	BRAIN_CK(tmp_tauto_ccls.is_empty());
	BRAIN_CK(tmp_diff_ccls.is_empty());

	// final checks

	row<canon_clause*>& ccls_tauto_cnf = tmp_tauto_cnf.cf_clauses;
	row<canon_clause*>& ccls_diff_cnf = tmp_diff_cnf.cf_clauses;

	MARK_USED(ccls_tauto_cnf);
	MARK_USED(ccls_diff_cnf);

	DBG_PRT_COND(100, (mm == mo_save), map_dbg_print(os, mm));
	DBG_PRT_COND(100, (mm == mo_save), 
		os << "GUIDE=" << bj_eol;
		tmp_guide_ccls.print_row_data(os, true, "\n");
		os << "TAUTO=" << bj_eol;
		ccls_tauto_cnf.print_row_data(os, true, "\n");
		os << "DIFF=" << bj_eol;
		ccls_diff_cnf.print_row_data(os, true, "\n");
	);

	DBG(
		bool are_eq = false;
		bool is_sub = false;
		cmp_is_sub cmp_resp = k_no_is_sub;

		cmp_resp = ccls_tauto_cnf.sorted_set_is_subset(ccls_diff_cnf, cmp_clauses, are_eq);
		is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));
		BRAIN_CK(is_sub);

		are_eq = false;
		is_sub = false;
		cmp_resp = k_no_is_sub;
		cmp_resp = ccls_tauto_cnf.sorted_set_is_subset(tmp_guide_ccls, cmp_clauses, are_eq);
		is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));
		BRAIN_CK(is_sub);

	);
	BRAIN_CK(! tmp_tauto_cnf.has_phase_path());
	BRAIN_CK(tmp_diff_cnf.has_phase_path());
	BRAIN_CK(! tmp_guide_cnf.has_phase_path());

	return true;
}

void
neuromap::map_get_initial_guide_coloring(coloring& clr){
	BRAIN_CK(&ma_guide_col != &clr);

	brain& brn = get_brn();
	MARK_USED(brn);
	
	clr.init_coloring();

	row_quanton_t&	all_quas = clr.co_quas;
	row<long>&	qua_colors = clr.co_qua_colors;

	row<neuron*>&	all_neus = clr.co_neus;
	row<long>&	neu_colors = clr.co_neu_colors;

	all_quas.clear();
	all_neus.clear();

	row<prop_signal>& dtrace = na_forced;
	long beg_sz = 0;
	long end_sz = dtrace.size();

	long sig_col = 1;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	for(long ii = beg_sz; ii < end_sz; ii++){
		prop_signal& q_sig1 = dtrace[ii];

		bool inc_col = false;
		if(ii > 0){
			prop_signal& q_sig0 = dtrace[ii - 1];
			
			// this two lines are the whole purpose of tiers. 
			// initialize the guide coloring (one color per tier).
			long col0 = q_sig0.ps_tier;
			long col1 = q_sig1.ps_tier;
			
			BRAIN_CK(col0 >= 0);
			BRAIN_CK(col1 >= 0);
			if(col0 != col1){
				BRAIN_CK(col0 > col1);
				inc_col = true;
			}
		}
	
		if(inc_col){ sig_col++; }
		
		BRAIN_CK(q_sig1.ps_quanton != NULL_PT);

		quanton& qua = *(q_sig1.ps_quanton);
		quanton& opp = qua.opposite();

		BRAIN_CK(! qua.has_note1());
		DBG(qua.set_note1(brn));
	
		all_quas.push(&qua);
		qua_colors.push(sig_col);

		all_quas.push(&opp);
		qua_colors.push(sig_col);

		if(q_sig1.ps_source != NULL_PT){
			neuron& neu = *(q_sig1.ps_source);

			all_neus.push(&neu); 
			neu_colors.push(sig_col);
		}
	}

	DBG(reset_all_note1(brn, all_quas));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(clr.ck_cols());
}

void
neuromap::map_get_initial_tauto_coloring(coloring& stab_guide_clr, 
									   coloring& base_tauto_clr, bool ck_tks)
{
	brain& brn = get_brn();
	BRAIN_CK(&stab_guide_clr != &base_tauto_clr);

	base_tauto_clr.init_coloring();

	stab_guide_clr.get_initial_sorting_coloring(brn, base_tauto_clr, false);

	row<neuron*>&	all_neus = base_tauto_clr.co_neus;
	row<long>&	neu_colors = base_tauto_clr.co_neu_colors;

	map_get_layer_neus(all_neus, ck_tks);
	neu_colors.fill(1, all_neus.size());

	BRAIN_CK(base_tauto_clr.ck_cols());
}

void
neuromap::map_get_layer_neus(row<neuron*>& neus, bool ck_tks){
	neus.clear();

	brain& brn = get_brn();
	row<neuron*>& m_neus = brn.br_tmp_fill_non_forced;
	m_neus.clear();
	map_get_all_non_forced_neus(m_neus);
	
	if(! ck_tks){
		m_neus.move_to(neus);
		return;
	}
	
	for(long bb = 0; bb < m_neus.size(); bb++){
		BRAIN_CK(m_neus[bb] != NULL_PT);
		neuron& neu = *(m_neus[bb]);

		bool ck_val = neu.recoiled_in_or_after(na_setup_tk); 
		if(ck_val){ neus.push(&neu); }
	}
}

bool
neuromap::has_stab_guide(){
	brain& brn = get_brn();
	if(na_guide_tk.is_valid()){
		if(brn.recoil() == na_guide_tk.tk_recoil){
			return true;
		}
	}
	return false;
}

void
neuromap::map_set_stab_guide(){
	if(has_stab_guide()){
		return;
	}
	if(na_submap != NULL_PT){
		na_submap->map_set_stab_guide();
	}
	map_init_stab_guide();
	
	brain& brn = get_brn();
	na_guide_tk.update_ticket(brn);
	BRAIN_CK(has_stab_guide());
}

void
neuromap::map_init_stab_guide(){
	brain& brn = get_brn();
	BRAIN_CK(map_ck_guides(dbg_call_1));

	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;
	coloring ini_guide_col(&brn);
	dima_dims dims0;

	map_get_initial_guide_coloring(ini_guide_col);
	
	coloring& guide_col = ma_guide_col;
	guide_col.init_coloring(&brn);
	
	if(na_submap != NULL_PT){
		na_submap->ma_guide_col.copy_co_to(guide_col);
	}
	
	guide_col.add_coloring(brn, ini_guide_col);	

	DBG(long old_quas_sz = guide_col.co_quas.size());
	DBG(long old_neus_sz = guide_col.co_neus.size());

	brn.all_mutual_init();
	neus_srg.sg_one_ccl_per_ss = false;

	DBG_PRT(61, os << " bef_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " bef_load_col quas_srg=" << &quas_srg);
	guide_col.load_colors_into(brn, neus_srg, quas_srg, dims0);
	DBG_PRT(61, os << " aft_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " aft_load_col quas_srg=" << &quas_srg);
	neus_srg.stab_mutual(quas_srg);
	guide_col.save_colors_from(neus_srg, quas_srg);

	BRAIN_CK(old_quas_sz == guide_col.co_quas.size());
	BRAIN_CK(old_neus_sz == guide_col.co_neus.size());

	BRAIN_CK(map_ck_guides(dbg_call_2));
}

void
neuromap::map_dbg_print(bj_ostream& os, mem_op_t mm){
#ifdef FULL_DEBUG
#endif
}

bool
neuromap::map_ck_guides(dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	BRAIN_CK(map_ck_guide_idx(ma_guide_col, dbg_id));
#endif
	return true;
}

bool
neuromap::map_ck_contained_in(coloring& colr, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
#endif
	return true;
}

bool
neuromap::map_ck_guide_idx(coloring& guide_col, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
#endif
	return true;
}

