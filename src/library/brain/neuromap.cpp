

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
neuromap::map_make_dominated(){
	brain& brn = get_brn();
	bool mk_deduc = false;
	map_make_guide_dominated(mk_deduc);
	make_all_ne_dominated(brn, na_non_forced, mk_deduc);
}

void
neuromap::map_make_guide_dominated(bool mk_deduc){
	if(na_submap != NULL_PT){
		na_submap->map_make_guide_dominated(mk_deduc);
	}
	brain& brn = get_brn();
	make_all_ps_dominated(brn, na_forced, mk_deduc);
}

void
neuromap::map_set_all_marks_and_spots(){
	if(na_has_marks_and_spots){
		return;
	}
	bool has_sub = (na_submap != NULL_PT);
	if(has_sub){
		na_submap->map_set_all_marks_and_spots();
	}
	brain& brn = get_brn();
	long fst_idx = first_forced_body_idx();
	set_marks_and_spots_of(brn, na_forced, fst_idx);
	if(! has_sub){
		prop_signal& fst_sig = na_forced.first();
		BRAIN_CK(fst_sig.has_ps_mark());
		fst_sig.set_ps_spot(brn);
	}
	na_has_marks_and_spots = true;
}

void
neuromap::map_reset_all_marks_and_spots(){
	if(! na_has_marks_and_spots){
		return;
	}
	bool has_sub = (na_submap != NULL_PT);
	if(has_sub){
		na_submap->map_reset_all_marks_and_spots();
	}
	brain& brn = get_brn();
	long fst_idx = first_forced_body_idx();
	reset_marks_and_spots_of(brn, na_forced, fst_idx);
	if(! has_sub){
		prop_signal& fst_sig = na_forced.first();
		BRAIN_CK(! fst_sig.has_ps_mark());
		fst_sig.reset_ps_spot(brn);
	}
	na_has_marks_and_spots = false;
}

void
neuromap::map_get_all_quas(row_quanton_t& all_quas){
	if(na_submap == NULL_PT){
		all_quas.clear();
	} else {
		na_submap->map_get_all_quas(all_quas);
	}
	//long fst_idx = first_forced_body_idx();
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

bool
neuromap::map_can_activate(){
	if(na_orig_lv == INVALID_LEVEL){
		return false;
	}
	BRAIN_CK(na_orig_lv != INVALID_LEVEL);
	brain& brn = get_brn();
	BRAIN_CK(brn.br_data_levels.is_valid_idx(na_orig_lv));
	leveldat& lv_dat = brn.get_data_level(na_orig_lv);
	bool can_act = ! lv_dat.has_setup_neuromap();
	return can_act;
}

void
neuromap::map_activate(dbg_call_id dbg_id){
	brain& brn = get_brn();
	
	if(! map_can_activate()){
		return;
	}
	
	leveldat& lv_dat = brn.get_data_level(na_orig_lv);
	
	BRAIN_CK(! lv_dat.has_setup_neuromap());
	lv_dat.set_setup_neuromap(*this);

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
neuromap::map_add_to_release(){
	BRAIN_CK(na_is_head);
	BRAIN_CK(na_release_idx == INVALID_IDX);
	
	na_mates.let_go();
	
	brain& brn = get_brn();
	brn.br_nmps_to_release.push(this);
	na_release_idx = brn.br_nmps_to_release.last_idx();
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
		map_add_to_release();
	}
}

bool
ck_map_first_quas(row_quanton_t& all_quas){
#ifdef FULL_DEBUG
	BRAIN_CK(all_quas.size() > 2);
	DBG(
		quanton* q0 = all_quas[0];
		quanton* q1 = all_quas[1];
		BRAIN_CK(q0 != NULL_PT);
		BRAIN_CK(q0->qu_inverse == q1);
		BRAIN_CK(q1->qu_inverse == q0);
	);
#endif
	return true;
}

void
neuromap::map_set_all_qu_curr_dom(){
	brain& brn = get_brn();
	row_quanton_t& all_quas = brn.br_tmp_qu_dom;
	all_quas.clear();
	map_get_all_quas(all_quas);
	BRAIN_CK(ck_map_first_quas(all_quas));
	set_all_qu_nemap(all_quas, this, 1);
}

void
neuromap::map_reset_all_qu_curr_dom(){
	brain& brn = get_brn();
	neuromap* upper_map = brn.get_last_upper_map();
	row_quanton_t& all_quas = brn.br_tmp_qu_dom;
	all_quas.clear();
	map_get_all_quas(all_quas);
	BRAIN_CK(ck_map_first_quas(all_quas));
	set_all_qu_nemap(all_quas, upper_map, 1);
}

bool
dbg_ck_all_diff_neus(brain& brn, row<neuron*>& all_neus){
#ifdef FULL_DEBUG
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	neuron* bad_neu = NULL_PT;
	for(long aa = 0; aa < all_neus.size(); aa++){ 
		neuron& neu = *(all_neus[aa]); 
		if(neu.has_tag0()){
			bad_neu = &neu;
			break;
		}
		neu.set_tag0(brn);
	}
	reset_all_tag0(brn, all_neus);
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	BRAIN_CK(bad_neu == NULL_PT);
#endif
	return true;
}

bool
neuromap::dbg_ck_all_neus(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	row<neuron*>& all_neus = brn.br_tmp_ck_neus;
	all_neus.clear();
	map_get_all_neus(all_neus);
	BRAIN_CK(dbg_ck_all_diff_neus(brn, all_neus));
#endif
	return true;
}

void
neuromap::map_set_all_ne_curr_dom(){
	brain& brn = get_brn();
	row<neuron*>& all_neus = brn.br_tmp_ne_dom;
	all_neus.clear();
	map_get_all_neus(all_neus);
	BRAIN_CK(dbg_ck_all_diff_neus(brn, all_neus));
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
	
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	for(int aa = 0; aa < nmp_quas.size(); aa++){
		BRAIN_CK(nmp_quas[aa] != NULL_PT);
		quanton& qua = *(nmp_quas[aa]);
		quanton& opp = qua.opposite();
	
		append_all_not_tag0(brn, qua.qu_full_charged, na_all_filled_in_propag);
		append_all_not_tag0(brn, opp.qu_full_charged, na_all_filled_in_propag);
	}
	reset_all_tag0(brn, na_all_filled_in_propag);
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
}

void
neuromap::full_release(){
	if(na_submap != NULL_PT){
		na_submap->full_release();
	}
	
	BRAIN_CK(! na_active);
	BRAIN_CK(na_mates.is_alone());
	//na_mates.let_go();
	
	brain& brn = get_brn();
	brn.release_neuromap(*this);
}

void
neuromap::dbg_get_fo_upper_quas(row_quanton_t& fo_upper_quas){
	fo_upper_quas.clear();
	
	long min_ti = get_min_tier();
	BRAIN_CK(min_ti != INVALID_TIER);
	
	brain& brn = get_brn();
	row<neuron*>&	all_neus = brn.br_tmp_forced;
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

	DBG_PRT(127, os << "upp_qus");
	
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
	//SORTER_CK(! neus_srg.sg_step_has_diff);
	//SORTER_CK(! quas_srg.sg_step_has_diff);

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
quanton::reset_and_add_tee(sort_glb& quas_srg, sort_id_t quas_consec){
	BRAIN_CK(qu_tee.is_unsorted());
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

		if(qua.has_note1()){
			sortrel& qua_sre = qua.qu_reltee;
			row<sortee*>& qua_mates = qua_sre.so_mates;
			qua_mates.push(&ne_tee);
			neu_mates.push(&(qua.qu_tee));
		}
	}
}

void
coloring::load_colors_into(brain& brn, sort_glb& neus_srg, sort_glb& quas_srg, 
						   dima_dims& dims)
{
	BRAIN_CK(neus_srg.has_head());
	BRAIN_CK(quas_srg.has_head());
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row<neuron*>&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;

	//BRAIN_CK(qua_colors.is_sorted(cmp_long));
	//BRAIN_CK(neu_colors.is_sorted(cmp_long));

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
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

		BRAIN_CK(! qua.has_note1() || ((aa > 0) && (all_quas[aa - 1] == qua.qu_inverse)));
		if(! qua.has_note1()){
			qua.set_note1(brn);
		}

		bool inc_consec = has_diff_col_than_prev(qua_colors, aa);
		if(inc_consec){ quas_consec++; }

		qua.reset_and_add_tee(quas_srg, quas_consec);
	}

	sort_id_t& neus_consec = quas_srg.sg_curr_stab_consec;
	neus_consec++;

	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	
	for(long bb = 0; bb < all_neus.size(); bb++){
		BRAIN_CK(all_neus[bb] != NULL_PT);
		neuron& neu = *(all_neus[bb]);

		BRAIN_CK(! neu.has_tag0());
		DBG(neu.set_tag0(brn));
		
		bool inc_consec = has_diff_col_than_prev(neu_colors, bb);
		if(inc_consec){ neus_consec++; }

		sortee& neu_tee = neu.ne_tee;
		neu_tee.so_ccl.cc_clear(false);
		neu_tee.so_tee_consec = 0;

		neu.fill_mutual_sortees(brn); // uses note1 of quas

		row<sortee*>& neu_mates = neu.ne_reltee.so_mates;
		if(! neu_mates.is_empty()){
			BRAIN_CK(neu_tee.is_unsorted());
			neu_tee.sort_from(neus_srg, neus_consec);
		}
	}

	DBG(reset_all_tag0(brn, all_neus));
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	
	reset_all_note1(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
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

bool
neuron::in_neuromap(long min_tier, long max_tier, row_quanton_t& neu_upper_quas, 
					long& upper_pos_ti)
{
	bool is_fll = true;
	upper_pos_ti = INVALID_TIER;

	neu_upper_quas.clear();
	
	DBG(long dbg_num_after = 0);
	for(long ii = 0; ii < fib_sz(); ii++){
		BRAIN_CK(ne_fibres[ii] != NULL_PT);
		
		// qua was charged in last propagate.
		quanton& qua = *(ne_fibres[ii]);
		long qti = qua.qu_tier;
		BRAIN_CK(qti != INVALID_TIER);
		
		bool is_part = (qti >= min_tier);
		bool is_upper = (qti < min_tier);
		DBG(if(qua.has_mark() && is_part){ dbg_num_after++; });

		if(! qua.has_mark() && is_part){
			// qua is not in neumap (! has_mark) so candidate subset is not in neumap
			is_fll = false;
			break;
		}

		if(qua.is_pos() && is_upper){
			// neu is sat in upper level so candidate subset is not in neumap
			if(upper_pos_ti == INVALID_TIER){
				upper_pos_ti = qti;
			}
			if(qti < upper_pos_ti){
				upper_pos_ti = qti;
			}
		}
		if(is_upper){
			neu_upper_quas.push(&qua);
		}
	}
	
	BRAIN_CK(! is_fll || (upper_pos_ti != INVALID_TIER) || (dbg_num_after >= 2));
	return is_fll;
}

void
neuromap::map_append_non_forced_from(brain& brn, row<neuron*>& all_neus, 
							   row<neuron*>& sel_neus, neurolayers& not_sel_neus, 
							   row_quanton_t& nmp_upper_quas, long min_ti, long max_ti, 
							   dbg_call_id dbg_call)
{
	BRAIN_CK(min_ti <= max_ti);
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);

		if(neu.has_spot()){ // do not add forced ones to non_forced
			continue;
		}
		if(! neu.ne_original){
			continue;
		}

		BRAIN_CK(neu.ne_original);
		
		row_quanton_t& neu_upper_quas = brn.br_tmp_neu_upper_qu;
		neu_upper_quas.clear();

		long min_pos_ti = INVALID_TIER;
		bool is_in_nmp = neu.in_neuromap(min_ti, max_ti, neu_upper_quas, min_pos_ti);

		if(is_in_nmp){
			if(min_pos_ti == INVALID_TIER){
				BRAIN_CK(! neu.has_tag1());
				sel_neus.push(&neu);
				DBG(neu.set_tag1(brn));

				BRAIN_CK(ck_motives(brn, neu_upper_quas));
				append_all_not_note3(brn, neu_upper_quas, nmp_upper_quas);
			} else {
				row_neuron_t& not_sel_neus_ly = not_sel_neus.get_ne_layer(min_pos_ti);
				not_sel_neus_ly.push(&neu);
			}
		}
	}
}

void
neurolayers::get_all_ordered_neurons(row_neuron_t& all_neus, long min_ly, long max_ly)
{
	int the_sz = nl_neus_by_layer.size();
	if(max_ly < 0){ max_ly = the_sz; }
	if(max_ly < min_ly){ max_ly = min_ly; }
	if(max_ly > the_sz){ max_ly = the_sz; }
	if(min_ly > the_sz){ min_ly = the_sz; }

	BRAIN_CK(min_ly <= max_ly);
	BRAIN_CK((min_ly == the_sz) || nl_neus_by_layer.is_valid_idx(min_ly));
	BRAIN_CK((max_ly == the_sz) || nl_neus_by_layer.is_valid_idx(max_ly));
	
	all_neus.clear();
	for(long aa = min_ly; aa < max_ly; aa++){
		row_neuron_t& lv_neus = nl_neus_by_layer[aa];
		lv_neus.append_to(all_neus);
	}
}

void
neuromap::map_fill_non_forced(neurolayers& not_sel_neus){ 
	brain& brn = get_brn();
	row<neuron*>& all_fll_in_lower = brn.br_tmp_fill_non_forced;
	row<neuron*>& sel_neus = brn.br_tmp_selected;
	row_quanton_t& nmp_upper_quas = brn.br_tmp_nmp_upper_qu;
	
	nmp_upper_quas.clear();
	
	long min_ti = get_min_tier();
	long max_ti = get_max_tier();
	
	BRAIN_CK(min_ti != INVALID_TIER);
	BRAIN_CK(max_ti != INVALID_TIER);
	BRAIN_CK(min_ti <= max_ti);
	BRAIN_CK(brn.br_qu_tot_note3 == 0);

	map_set_all_marks_and_spots();
	
	sel_neus.clear();

	map_append_non_forced_from(brn, na_all_filled_in_propag, sel_neus, not_sel_neus, 
						 nmp_upper_quas, min_ti, max_ti, dbg_call_1);
	
	all_fll_in_lower.clear();
	BRAIN_CK(min_ti <= max_ti);
	not_sel_neus.get_all_ordered_neurons(all_fll_in_lower, min_ti, max_ti + 1);
	map_append_non_forced_from(brn, all_fll_in_lower, sel_neus, not_sel_neus, nmp_upper_quas, 
						 min_ti, max_ti, dbg_call_2);

	// finalize
	
	reset_all_note3(brn, nmp_upper_quas);
	BRAIN_CK(brn.br_qu_tot_note3 == 0);
	
	sel_neus.move_to(na_non_forced);
	nmp_upper_quas.move_to(na_nf_upper_quas);
	
	BRAIN_CK(ck_motives(brn, na_nf_upper_quas));
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

		// This func sets cc_spot==true to tmp_diff_cnf clauses in the found vnt.
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

			row<neuron*>& all_neus_in_vnt = brn.br_tmp_found_neus;
			all_neus_in_vnt.clear();
			
			bool only_with_spot = true; // only clauses with cc_spot==true
			ccl_row_as<neuron>(tmp_diff_cnf.cf_clauses, all_neus_in_vnt, only_with_spot);

			// old code updated ne_recoil_tk of all_neus_in_vnt
			// this code updates ne_deduc_tk of all_neus_in_vnt
			bool mk_deduc = true;
			make_all_ne_dominated(brn, all_neus_in_vnt, mk_deduc);
			map_make_guide_dominated(mk_deduc);
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
	coloring& guide_col = na_guide_col;
	MARK_USED(guide_col);
	
	DBG_PRT(110, os << "map_mem_oper=" << ((void*)this));
	DBG_PRT(110, os << "map_mem_oper=" << this);

	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;

	brn.all_mutual_init();
	
	BRAIN_CK(! has_stab_guide());
	
	if(! has_stab_guide()){
		map_set_stab_guide();
	} else {
		DBG_PRT(133, os << "HAD_STAB_GUIDE !!! nmp=(" << (void*)this << ")");
		dima_dims dims0;
		
		neus_srg.sg_one_ccl_per_ss = false;
		guide_col.load_colors_into(brn, neus_srg, quas_srg, dims0);
	}
	
	
	BRAIN_CK(! guide_col.is_co_virgin());
	BRAIN_CK(map_ck_contained_in(guide_col, dbg_call_1));

	// stab uni_guide
	
	skeleton_glb& skg = brn.get_skeleton();

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

	coloring ini_tau_col(&brn);
	map_get_initial_tauto_coloring(uni_guide_col, ini_tau_col, (mm == mo_save));

	BRAIN_CK(map_ck_contained_in(ini_tau_col, dbg_call_3));

	brn.all_mutual_init();
	
	sort_glb& fnl_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& fnl_qu_srg = brn.br_tauto_quas_srg;
	dima_dims dims1;
	
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
	BRAIN_CK(&na_guide_col != &clr);

	brain& brn = get_brn();
	MARK_USED(brn);
	
	clr.init_coloring();

	row_quanton_t&	all_quas = clr.co_quas;
	row<long>&	qua_colors = clr.co_qua_colors;

	row<neuron*>&	all_neus = clr.co_neus;
	row<long>&	neu_colors = clr.co_neu_colors;

	all_quas.clear();
	all_neus.clear();
	qua_colors.clear();
	neu_colors.clear();

	row<prop_signal>& dtrace = na_forced;
	long fst_idx = first_forced_body_idx();
	MARK_USED(fst_idx);
	//long beg_sz = fst_idx;
	long beg_sz = 0;
	long end_sz = dtrace.size();

	long fst_col = 1;
	long sig_col = fst_col;
	bool has_sub = (na_submap != NULL_PT);

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

		if(has_sub || (ii > 0)){
			quanton& qua = *(q_sig1.ps_quanton);
			quanton& opp = qua.opposite();

			BRAIN_CK(! qua.has_note1());
			DBG(qua.set_note1(brn));
			
			long col_qua = sig_col;
			long col_opp = sig_col;
			
			if(! has_sub && (ii == 1)){ 
				BRAIN_CK(&opp == dtrace[0].ps_quanton);
				col_opp = fst_col; 
			}
		
			all_quas.push(&qua);
			qua_colors.push(col_qua);

			all_quas.push(&opp);
			qua_colors.push(col_opp);
		}

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
	
	coloring& tmp_co = brn.br_tmp_tauto_col;

	tmp_co.init_coloring();
	base_tauto_clr.init_coloring();

	map_get_tauto_neus(tmp_co.co_neus, ck_tks);
	tmp_co.co_neu_colors.fill(1, tmp_co.co_neus.size());

	stab_guide_clr.copy_co_to(base_tauto_clr);
	base_tauto_clr.add_coloring(brn, tmp_co);
	
	BRAIN_CK(base_tauto_clr.ck_cols());
}

void
neuromap::map_get_tauto_neus(row<neuron*>& neus, bool ck_tks){
	neus.clear();

	brain& brn = get_brn();
	row<neuron*>& m_neus = brn.br_tmp_tauto_neus;
	m_neus.clear();
	//map_get_all_forced_neus(m_neus);
	map_get_all_non_forced_neus(m_neus, false);
	
	if(! ck_tks){
		m_neus.move_to(neus);
		return;
	}
	
	for(long bb = 0; bb < m_neus.size(); bb++){
		BRAIN_CK(m_neus[bb] != NULL_PT);
		neuron& neu = *(m_neus[bb]);

		bool ck_val = neu.deduced_in_or_after(na_setup_tk); 
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

	coloring ini_guide_col(&brn);

	map_get_initial_guide_coloring(ini_guide_col);
	
	coloring& guide_col = na_guide_col;
	guide_col.init_coloring(&brn);
	
	if(na_submap != NULL_PT){
		na_submap->na_guide_col.copy_co_to(guide_col);
	}
	
	guide_col.add_coloring(brn, ini_guide_col);

	DBG(long old_quas_sz = guide_col.co_quas.size());
	DBG(long old_neus_sz = guide_col.co_neus.size());

	brn.all_mutual_init();
	
	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;
	dima_dims dims0;
	
	neus_srg.sg_one_ccl_per_ss = false;

	DBG_PRT(61, os << " bef_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " bef_load_col quas_srg=" << &quas_srg);
	guide_col.load_colors_into(brn, neus_srg, quas_srg, dims0);
	DBG_PRT(61, os << " aft_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " aft_load_col quas_srg=" << &quas_srg);
	
	// THE STAB FOR THIS NMP BLOCK
	neus_srg.stab_mutual(quas_srg);
	
	guide_col.save_colors_from(neus_srg, quas_srg);

	BRAIN_CK(old_quas_sz == guide_col.co_quas.size());
	BRAIN_CK(old_neus_sz == guide_col.co_neus.size());
}

void
neuromap::map_dbg_print(bj_ostream& os, mem_op_t mm){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	//os << STACK_STR << bj_eol;
	os << "DBG_PRT=" << bj_eol;
	os << this << bj_eol;
	os << "brn_tk=" << brn.br_current_ticket << bj_eol;
	if(mm == mo_save){ os << "SAVE "; }
	if(mm == mo_find){ os << "FIND "; }

	//os << "CERO FILLED___________________________________________ " << bj_eol;
	sort_glb& tauto_srg = brn.br_tauto_neus_srg;
	os << " sg_dbg_cnf_tot_onelit=" << tauto_srg.sg_dbg_cnf_tot_onelit << bj_eol;

	os << " TATUTO_STEP_SORTEES (after step)=" << bj_eol;
	for(long aa = 0; aa < tauto_srg.sg_step_sortees.size(); aa++){
		os << *(tauto_srg.sg_step_sortees[aa]) << bj_eol;
	}

	os << " TAUTO_CNF=" << bj_eol;
	os << tmp_tauto_cnf << bj_eol;
	os << " DIFF_CNF=" << bj_eol;
	os << tmp_diff_cnf << bj_eol;
	os << " GUIDE_CNF=" << bj_eol;
	os << tmp_guide_cnf << bj_eol;

	os << bj_eol;
	BRAIN_DBG(os << " RECOIL_LV=" << brn.br_dbg.dbg_last_recoil_lv);

	os << bj_eol;
	os << brn.get_my_inst().get_f_nam() << bj_eol;
	os << "=========================================================" << bj_eol;
#endif
}

bool
neuromap::map_ck_contained_in(coloring& colr, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	
	// CHECK QUAS
	
	row_quanton_t& m_quas = brn.br_tmp_ck_col;
	map_get_all_quas(m_quas);
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	set_all_note1(brn, m_quas);
	
	row_quanton_t& all_quas = colr.co_quas;
	for(int ii = 0; ii < all_quas.size(); ii++){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		BRAIN_CK(all_quas[ii]->has_note1());
	}
	
	reset_all_note1(brn, m_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	// CHECK NEUS
	
	row<neuron*>& m_neus = brn.br_tmp_all_neus;
	map_get_all_neus(m_neus);
	
	BRAIN_CK(brn.br_tot_ne_spots == 0);
	set_spots_of(brn, m_neus);
	
	//DBG_PRT(133, os << "cll_id=" << dbg_id << " colr=" << bj_eol << colr << bj_eol 
	//		<< "NMP=" << bj_eol << *this);

	row<neuron*>& all_neus = colr.co_neus;
	for(long ii = 0; ii < all_neus.size(); ii++){
		BRAIN_CK(all_neus[ii] != NULL_PT);
		neuron& neu = *(all_neus[ii]);
		BRAIN_CK_PRT(neu.ne_spot, os << "_______\n neu=" << &neu);
	}

	reset_spots_of(brn, m_neus);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
#endif
	return true;
}

void
coloring::set_brain_coloring(){
	brain* pt_br = get_dbg_brn();
	if(pt_br == NULL){
		return;
	}
	brain& brn = *pt_br;
	
	k_row<quanton>& all_br_quas = brn.br_positons;
	k_row<neuron>& all_br_neus = brn.br_neurons;
	
	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row<neuron*>&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;
	
	init_coloring();
	
	for(long aa = 0; aa < all_br_quas.size(); aa++){
		quanton& qua = all_br_quas[aa];
		quanton& opp = qua.opposite();
		
		all_quas.push(&qua);
		qua_colors.push(1);

		all_quas.push(&opp);
		qua_colors.push(1);
	}

	for(long bb = 0; bb < all_br_neus.size(); bb++){
		neuron& neu = all_br_neus[bb];
		if(! neu.is_ne_virgin()){
			all_neus.push(&neu);
			neu_colors.push(1);
		}
	}
	
}


