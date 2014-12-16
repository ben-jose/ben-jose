

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

neuromap class methos and funcs.

--------------------------------------------------------------*/

#include "brain.h"

long	set_spots_of(brain& brn, row<neuron*>& neus){
	long num_neu = 0;
	for(long ii = 0; ii < neus.size(); ii++){
		neuron* neu = neus[ii];
		if(! neu->ne_spot){
			neu->set_spot(brn);
			num_neu++;
		}
	}
	return num_neu;
}

long	reset_spots_of(brain& brn, row<neuron*>& neus){
	long num_neu = 0;
	for(long ii = 0; ii < neus.size(); ii++){
		neuron* neu = neus[ii];
		if(neu->ne_spot){
			neu->reset_spot(brn);
			num_neu++;
		}
	}
	return num_neu;
}

void
neuromap::map_set_all_marks_and_spots(){
	if(na_submap != NULL_PT){
		na_submap->map_set_all_marks_and_spots();
		return;
	}
	brain& brn = get_brn();
	set_marks_and_spots_of(brn, na_forced);
}

void
neuromap::map_reset_all_marks_and_spots(){
	if(na_submap != NULL_PT){
		na_submap->map_reset_all_marks_and_spots();
		return;
	}
	brain& brn = get_brn();
	reset_marks_and_spots_of(brn, na_forced);
}

void
neuromap::map_get_all_quas(row_quanton_t& all_quas){
	if(na_submap != NULL_PT){
		na_submap->map_get_all_quas(all_quas);
		append_all_trace_quas(na_forced, all_quas);
		return;
	}
	all_quas.clear();
	append_all_trace_quas(na_forced, all_quas);
}

void
neuromap::map_get_all_neus(row<neuron*>& all_neus){
	if(na_submap != NULL_PT){
		na_submap->map_get_all_neus(all_neus);
		na_non_forced.append_to(all_neus);
		return;
	}
	all_neus.clear();
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
neuromap::map_make_dominated(){
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
}

void
brain::deactivate_last_map(){
	BRAIN_CK(! br_maps_active.is_empty());
	neuromap* pt_mpp = br_maps_active.last();
	BRAIN_CK(pt_mpp != NULL_PT);
	neuromap& mpp = *pt_mpp;

	DBG_PRT(112, os << "DEACTIVATING NEUROMAP=" << (void*)(&(mpp)));

	mpp.map_deactivate();
}

void
brain::close_all_maps(){
	while(! br_maps_active.is_empty()){
		deactivate_last_map();
	}
	BRAIN_CK(br_maps_active.is_empty());
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
analyser::init_analyser(brain* brn){
	long tg_lv = INVALID_LEVEL;
	
	de_brain = brn;

	if(brn != NULL_PT){
		init_nk_with_note0(de_nkpr, brn, tg_lv);
		de_ref.init_nkref(&(brn->br_charge_trail));
	}
	de_all_noted.clear(true, true);
	
	de_first_bk_psig.init_prop_signal();
	de_next_bk_psig.init_prop_signal();
}

void
analyser::init_nk_with_dots(notekeeper& nkpr, brain* brn, long tg_lv){
	long* pt_tot_dots = NULL_PT;
	if(brn != NULL_PT){
		pt_tot_dots = &(brn->br_tot_qu_dots);
	}

	nkpr.init_notekeeper(brn, tg_lv);
	nkpr.init_funcs(pt_tot_dots, &quanton::has_dot, 
					&quanton::set_dot, &quanton::reset_dot, 
					&set_dots_of, &reset_dots_of);
}

void
analyser::init_nk_with_note0(notekeeper& nkpr, brain* brn, long tg_lv){
	long* pt_tot_note0 = NULL_PT;
	if(brn != NULL_PT){
		pt_tot_note0 = &(brn->br_qu_tot_note0);
	}
	nkpr.init_notekeeper(brn, tg_lv);
	nkpr.init_funcs(pt_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0);
}

void
analyser::find_next_noted(){
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	quanton* nxt_qua = nkr.get_curr_quanton();
	while(nxt_qua != NULL_PT){
		nxt_qua = nkr.get_curr_quanton();
		if(nxt_qua == NULL_PT){
			break;
		}
		BRAIN_CK(nxt_qua != NULL_PT);
		BRAIN_CK((nxt_qua->qu_source != NULL_PT) || nkpr.nk_has_note(*nxt_qua));
		if(nkpr.nk_has_note(*nxt_qua)){
			break;
		}
		
		DBG_PRT(20, os << "NOT noted " << nkr.get_curr_quanton() 
				<< " in deduc find next noted");
		nkr.dec_curr_quanton();
	}

	BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
	if(nxt_qua == NULL_PT){
		return;
	}
	
	BRAIN_CK(nxt_qua != NULL_PT);
	BRAIN_CK(nkpr.nk_has_note(*nxt_qua));
	BRAIN_CK(nxt_qua->qu_id != 0);
	BRAIN_CK(nxt_qua->is_pos());
	
	long qlv = nxt_qua->qlevel();

	nkpr.update_notes_layer(qlv);
}

void
analyser::find_next_source(bool only_origs)
{
	DBG(
		notekeeper& nkpr = de_nkpr;
		row<prop_signal>& all_noted = de_all_noted;
	)
	
	neuron*& nxt_src = de_next_bk_psig.ps_source;

	bool is_first = (nxt_src == tg_confl());

	BRAIN_CK(! is_first || (all_noted.size() <= 1));
	BRAIN_CK(! is_first || (nxt_src != NULL));

	if((nxt_src != NULL) && (! only_origs || nxt_src->ne_original)){
		set_notes_of(nxt_src->ne_fibres, is_first);
	}

	BRAIN_CK(nkpr.dk_tot_noted > 0);

	find_next_noted();
	update_noted();
}

void
analyser::set_notes_of(row_quanton_t& causes, bool is_first){
	notekeeper& nkpr = de_nkpr;
	BRAIN_CK(! causes.is_empty());
	BRAIN_CK(is_first || (causes[0]->get_charge() == cg_positive) );
	BRAIN_CK(! is_first || (causes[0]->get_charge() == cg_negative) );

	long from = (is_first)?(0):(1);
	long until = causes.size();
	nkpr.set_motive_notes(causes, from, until);
}

void
analyser::update_noted(){
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;
	
	quanton* pt_qua = nkr.get_curr_quanton();
	if(pt_qua == NULL_PT){
		de_next_bk_psig.init_prop_signal();
		return;
	}
	BRAIN_CK(pt_qua != NULL_PT);
	quanton& nxt_qua = *pt_qua;
	DBG_PRT(20, os << "noted found " << nxt_qua 
		<< " num_noted_in_layer " << nkpr.dk_num_noted_in_layer;
		os << " note layer=" << nkpr.dk_note_layer;
	);

	de_next_bk_psig.init_qua_signal(nxt_qua);
	
	prop_signal& nxt_sig = all_noted.inc_sz();
	nxt_sig = de_next_bk_psig;

	nkpr.nk_reset_note(nxt_qua);
	BRAIN_CK(nxt_qua.qlevel() == nkpr.dk_note_layer);
	BRAIN_CK(nxt_qua.is_pos());

	nkpr.dec_notes();

	DBG_PRT(101, os << "qua=" << &nxt_qua << " filled APPEND=";
		nxt_qua.qu_full_charged.print_row_data(os, true, "\n");
	);
}

bool
analyser::ck_deduction_init(long deduc_lv){
#ifdef FULL_DEBUG
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	BRAIN_CK(deduc_lv != INVALID_LEVEL);
	BRAIN_CK(nkpr.nk_get_counter() == 0);
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.dk_note_layer > 0);
	BRAIN_CK(nkpr.dk_note_layer == deduc_lv);
	BRAIN_CK(nkr.has_curr_quanton());
#endif
	return true;
}

void	
analyser::deduction_init(row_quanton_t& causes){
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;

	long deduc_lv = find_max_level(causes);
	
	all_noted.clear(true, true);
	nkpr.init_notes(deduc_lv);
	nkr.reset_curr_quanton();
	de_first_bk_psig.init_prop_signal();
	de_next_bk_psig.init_prop_signal();

	BRAIN_CK(ck_deduction_init(deduc_lv));
	
	set_notes_of(causes, true);
	BRAIN_CK(nkpr.dk_tot_noted > 0);

	find_next_noted();
	update_noted();
}

void
analyser::deduction_init(prop_signal const & confl_sg){
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;
	
	long deduc_lv = confl_sg.get_level();
	
	all_noted.clear(true, true);
	nkpr.init_notes(deduc_lv);
	nkr.reset_curr_quanton();
	
	de_first_bk_psig = confl_sg;
	de_next_bk_psig = confl_sg;
	prop_signal& fst_sig = all_noted.inc_sz();
	fst_sig = confl_sg;

	BRAIN_CK(confl_sg.ps_quanton != NULL);
	BRAIN_CK(confl_sg.ps_quanton->get_charge() == cg_negative);
	BRAIN_CK(confl_sg.ps_quanton->qlevel() == deduc_lv);
	BRAIN_CK(confl_sg.ps_source != NULL);
	BRAIN_CK(! tg_confl()->ne_fibres.is_empty());
	BRAIN_CK(ck_deduction_init(deduc_lv));
	
	find_next_source();
}

void
analyser::deduction_analysis(prop_signal const & confl_sg, deduction& dct){
	//brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	MARK_USED(nkr);
	notekeeper& nkpr = de_nkpr;
	
	deduction_init(confl_sg);
	while(! is_end_of_dct()){
		BRAIN_CK(nkr.has_curr_quanton());
		find_next_source();
	}
	BRAIN_CK(nkr.has_curr_quanton());

	fill_dct(dct);
	
	// reset all
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
analyser::update_all_deduction_noted(){
	brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	row<prop_signal>& all_noted = de_all_noted;
	
	make_all_ps_dominated(brn, all_noted, 0, all_noted.last_idx());
	update_all_ps_deduc_tk(brn, all_noted, 0, all_noted.last_idx());

	quanton* qua = nkr.get_curr_quanton();
	BRAIN_CK(qua != NULL_PT);	
	qua->make_qu_dominated(brn);
}

void
analyser::fill_dct(deduction& dct){
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	DBG(brain& brn = get_de_brain());
	
	quanton* pt_qua = nkr.get_curr_quanton();
	BRAIN_CK(pt_qua != NULL_PT);
	
	quanton& qua = *pt_qua;
	quanton& opp = qua.opposite(); 

	BRAIN_CK(is_end_of_dct());
	BRAIN_CK(opp.qlevel() == nkpr.dk_note_layer);
	BRAIN_CK(opp.get_charge() == cg_negative);

	dct.init_deduction();
	dct.dt_motives.set_cap(nkpr.dk_tot_noted + 1);

	nkpr.dk_quas_lyrs.get_all_ordered_quantons(dct.dt_motives);

	dct.dt_target_level = find_max_level(dct.dt_motives);

	BRAIN_CK(! nkpr.nk_has_note(opp));
	dct.dt_forced = &opp;

	BRAIN_CK(dct.dt_target_level < nkpr.dk_note_layer);
	BRAIN_CK(ck_motives(brn, dct.dt_motives));

	DBG_PRT(20, os << "find_dct of deduction=" << dct);
}

void
analyser::neuromap_write_analysis(deduction& dct, row<neuromap*>& all_nmps){
	brain& brn = get_de_brain();
	row<leveldat*>& all_lv = brn.br_data_levels;
	
	BRAIN_CK(brn.level() == all_lv.last_idx());
	
	long tg_lv = dct.dt_target_level;
	
	all_nmps.clear();
	
	for(int aa = all_lv.last_idx(); aa >= 0; aa--){
		BRAIN_CK(all_lv[aa] != NULL_PT);
		leveldat& lv = *(all_lv[aa]);
		BRAIN_CK(aa == lv.ld_idx);
		BRAIN_CK(lv.ld_chosen != NULL_PT);
		BRAIN_CK(lv.ld_chosen->qlevel() == lv.ld_idx);
		
		if(aa == tg_lv){
			break;
		}
		
		row<neuromap*>& to_wrt = lv.ld_all_nmps_to_write;
		for(int bb = to_wrt.last_idx(); bb >= 0; bb--){
			BRAIN_CK(to_wrt[bb] != NULL_PT);
			neuromap& nmp = *(to_wrt[bb]);
			if(nmp.na_active){
				all_nmps.push(&nmp);
			}
		}
	}
}

bool
analyser::ck_end_of_nmp(){
	if(! is_end_of_nmp()){
		return false;
	}
	if(is_end_of_lv()){
		return true;
	}
	brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	MARK_USED(nkr);
	quanton* qua = de_next_bk_psig.ps_quanton;
	
	BRAIN_CK(qua != NULL_PT);
	BRAIN_CK(qua == nkr.get_curr_quanton());
	
	long qti = qua->qu_tier;
	long num_quas = brn.br_charge_trail.get_qu_layer(qti).size();
	
	return (num_quas == 1);
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
analyser::fill_non_forced_from(row<neuron*>& all_neus, 
							   row<neuron*>& sel_neus, neurolayers& not_sel_neus, 
							   long min_ti, long max_ti)
{
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);

		if(neu.has_spot()){
			continue;
		}
		if(! neu.ne_original){
			continue;
		}

		BRAIN_CK(neu.ne_original);

		long min_pos_ti = INVALID_TIER;
		bool is_in_nmp = neu.in_neuromap(min_ti, max_ti, min_pos_ti);

		if(is_in_nmp){
			if(min_pos_ti == INVALID_TIER){
				sel_neus.push(&neu);
			} else {
				row_neuron_t& not_sel_neus_ly = not_sel_neus.get_ne_layer(min_pos_ti);
				not_sel_neus_ly.push(&neu);
			}
		}
	}
}

void
analyser::fill_non_forced(neuromap& nxt_nmp){
	brain& brn = get_de_brain();
	neurolayers& not_sel_neus = de_not_sel_neus;
	row<neuron*>& all_fll_in_lower = brn.br_tmp_fill_non_forced;
	row<neuron*>& sel_neus = brn.br_tmp_selected;
	
	long min_ti = nxt_nmp.get_min_tier();
	long max_ti = nxt_nmp.get_max_tier();
	
	BRAIN_CK(min_ti != INVALID_TIER);
	BRAIN_CK(max_ti != INVALID_TIER);
	BRAIN_CK(max_ti >= min_ti);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	BRAIN_CK(brn.br_tot_ne_spots == 0);

	nxt_nmp.map_set_all_marks_and_spots();
	
	sel_neus.clear();

	fill_non_forced_from(nxt_nmp.na_all_filled_in_propag, sel_neus, not_sel_neus, 
						 min_ti, max_ti);
	
	all_fll_in_lower.clear();
	not_sel_neus.get_all_ordered_neurons(all_fll_in_lower, min_ti, max_ti + 1);
	fill_non_forced_from(all_fll_in_lower, sel_neus, not_sel_neus, min_ti, max_ti);

	// finalize
	nxt_nmp.map_reset_all_marks_and_spots();
	
	sel_neus.move_to(nxt_nmp.na_non_forced);

	BRAIN_CK(brn.br_tot_qu_marks == 0);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
}

neuromap*
analyser::update_neuromap(neuromap* out_nmp){
	BRAIN_CK(is_end_of_nmp());
	
	brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	row<prop_signal>& all_noted = de_all_noted;

	neuromap& nxt_nmp = brn.locate_neuromap();
	nxt_nmp.na_submap = out_nmp;
	
	quanton* qua = nkr.get_curr_quanton();
	bool eolv = is_end_of_lv();
	if(eolv){
		nxt_nmp.na_orig_lv = qua->qlevel();
		nxt_nmp.na_orig_cho = qua;
	}
	BRAIN_CK(eolv || ck_end_of_nmp());
	all_noted.move_to(nxt_nmp.na_forced);
	
	nxt_nmp.set_all_filled_in_propag();
	
	fill_non_forced(nxt_nmp);

	out_nmp = &nxt_nmp;
	return out_nmp;
}

neuromap*
analyser::calc_neuromap(prop_signal const & confl_sg, long min_lv, neuromap* prev_nmp){
	BRAIN_CK(false); // CODING THIS
	
	brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	notekeeper& nkpr = de_nkpr;
	neurolayers& not_sel_neus = de_not_sel_neus;
	row<prop_signal>& all_noted = de_all_noted;
	
	neuromap* out_nmp = prev_nmp;
	
	BRAIN_CK(brn.br_data_levels.is_valid_idx(min_lv));
	if(out_nmp == NULL_PT){
		BRAIN_CK(brn.br_tot_qu_marks == 0);
		BRAIN_CK(brn.br_tot_ne_spots == 0);
		
		de_first_bk_psig = confl_sg;
		de_next_bk_psig = confl_sg;
		nkpr.init_notes(brn.level());
		not_sel_neus.clear_all_neurons();
		nkr.reset_curr_quanton();
		
		all_noted.clear(true, true);
		prop_signal& fst_sig = all_noted.inc_sz();
		fst_sig = confl_sg;

		long qlv = nkr.get_curr_qlevel();
		MARK_USED(qlv);
		
		BRAIN_CK(confl_sg.ps_quanton != NULL);
		BRAIN_CK(confl_sg.ps_quanton->get_charge() == cg_negative);
		BRAIN_CK(confl_sg.ps_source != NULL);
		BRAIN_CK(nkpr.nk_get_counter() == 0);
		BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
		BRAIN_CK(nkpr.dk_note_layer > 0);
		BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
		BRAIN_CK(! tg_confl()->ne_fibres.is_empty());
		BRAIN_CK(min_lv < qlv);
		
		out_nmp = NULL_PT;

		find_next_source(true);
	}
	
	while(nkr.get_curr_qlevel() > min_lv){
		if(is_end_of_nmp()){
			out_nmp = update_neuromap(out_nmp);
		}
		find_next_source(true);
	}

	return out_nmp;
}

void
analyser::end_calc_neuromap(neuromap& out_nmp){
	notekeeper& nkpr = de_nkpr;
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);	
}

bool
neuron::in_neuromap(long min_tier, long max_tier, long& upper_pos_ti){
	bool is_fll = true;
	upper_pos_ti = INVALID_TIER;
	
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
			// qua is not in memap (! has_mark) so candidate subset is not in memap
			is_fll = false;
			break;
		}

		if(qua.is_pos() && is_upper){
			// neu is sat in upper level so candidate subset is not in memap
			if(upper_pos_ti == INVALID_TIER){
				upper_pos_ti = qti;
			}
			if(qti < upper_pos_ti){
				upper_pos_ti = qti;
			}
		}		
	}
	
	BRAIN_CK(! is_fll || (dbg_num_after >= 2));
	return is_fll;
}

neuromap*
analyser::neuromap_find_analysis(deduction& dct){
	neuromap* out_nmp = NULL_PT;
	return out_nmp;
}

void
analyser::neuromap_setup_analysis(deduction& dct, row<neuromap*>& all_nmps){
}

