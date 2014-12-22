

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

char* neuromap::CL_NAME = as_pt_char("{neuromap}");

bool
neuromap::map_find(){
	return false;
}

bool
neuromap::map_write(){
	return false;
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

