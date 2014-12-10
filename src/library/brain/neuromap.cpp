

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

quanton*	
neuromap::map_choose_quanton(brain& brn){ 
	row<prop_signal>& trace = na_forced;
	quanton* qua = NULL;

	long max_ii = trace.last_idx();
	for(long ii = max_ii; ii >= 0; ii--){
		quanton* qua_ii = trace[ii].ps_quanton;
		if(qua_ii->get_charge() == cg_neutral){
			if(! qua_ii->is_semi_mono()){
				qua = qua_ii; 
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
neuromap::map_ck_all_qu_dominated(brain& brn){
#ifdef FULL_DEBUG
	row<prop_signal>& sgls = na_forced;
	for(long aa = 0; aa < sgls.size(); aa++){
		quanton* qua = sgls[aa].ps_quanton;
		MARK_USED(qua);
		BRAIN_CK(qua != NULL_PT);
		BRAIN_CK(qua->in_qu_dominated(brn));
	}
#endif
	return true;
}

bool
neuromap::map_ck_all_ne_dominated(brain& brn){
#ifdef FULL_DEBUG
	row<neuron*>& filled = na_non_forced;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			MARK_USED(fll_neu);

			DBG_COND_COMM(! (fll_neu.in_ne_dominated(brn)) ,
				os << "ABORTING_DATA " << bj_eol;
				os << " br_maps_active=" << brn.br_maps_active << bj_eol;
				os << " THIS_NEUROMAP=" << this << bj_eol;
				brn.print_trail(os);
				os << " up_dom=" << (void*)(brn.get_last_upper_map()) << bj_eol;
				os << " this_map=" << (void*)(this) << bj_eol;
				os << " NO_DOM neu==" << &fll_neu << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			BRAIN_CK(fll_neu.in_ne_dominated(brn));
		}
	}
#endif
	return true;
}

void
neuromap::map_make_dominated(brain& brn){
}

void
neuromap::map_activate(brain& brn){

	BRAIN_CK(! is_na_virgin());
	BRAIN_CK(! na_active);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	brn.br_maps_active.push(this);
	na_active = true;

	map_set_all_qu_curr_dom(brn);
	map_set_all_ne_curr_dom(brn);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	BRAIN_CK(! brn.br_maps_active.is_empty());
	BRAIN_CK(brn.br_maps_active.last() == this);

	DBG_PRT(110, os << "ACTIVATING " << this);
}

void
neuromap::map_deactivate(brain& brn){
	//DBG_PRT(110, os << "DEACTivating " << this << bj_eol << STACK_STR);
	DBG_PRT(110, os << "DEACTivating " << this);

	BRAIN_CK(na_active);
	BRAIN_CK(! brn.br_maps_active.is_empty());

	BRAIN_CK(brn.br_maps_active.last() == this);
	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	brn.br_maps_active.pop();
	na_active = false;

	map_reset_all_qu_curr_dom(brn);
	map_reset_all_ne_curr_dom(brn);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));
}

void
brain::deactivate_last_map(){
	BRAIN_CK(! br_maps_active.is_empty());
	brain& brn = *this;
	neuromap* pt_mpp = br_maps_active.last();
	BRAIN_CK(pt_mpp != NULL_PT);
	neuromap& mpp = *pt_mpp;

	DBG_PRT(112, os << "DEACTIVATING NEUROMAP=" << (void*)(&(mpp)));

	mpp.map_deactivate(brn);
}

void
brain::close_all_maps(){
	while(! br_maps_active.is_empty()){
		deactivate_last_map();
	}
	BRAIN_CK(br_maps_active.is_empty());
}

void
neuromap::map_set_all_qu_curr_dom(brain& brn){
	row<prop_signal>& sgls = na_forced;
	for(long aa = 0; aa < sgls.size(); aa++){
		prop_signal& q_sig = sgls[aa];

		quanton* qua = q_sig.ps_quanton;
		BRAIN_CK(qua != NULL_PT);
		//BRAIN_CK(qua->is_pos());

		quanton* inv = qua->qu_inverse;

		BRAIN_CK(qua->qu_curr_nemap != this);
		BRAIN_CK(inv->qu_curr_nemap != this);

		qua->qu_curr_nemap = this;
		inv->qu_curr_nemap = this;
	}
}

void
neuromap::map_reset_all_qu_curr_dom(brain& brn){
	neuromap* upper_map = brn.get_last_upper_map();
	row<prop_signal>& sgls = na_forced;
	for(long aa = 0; aa < sgls.size(); aa++){
		prop_signal& q_sig = sgls[aa];
		quanton* qua = q_sig.ps_quanton;
		BRAIN_CK(qua != NULL_PT);

		quanton* inv = qua->qu_inverse;

		BRAIN_CK(qua->qu_curr_nemap != upper_map);
		BRAIN_CK(inv->qu_curr_nemap != upper_map);

		qua->qu_curr_nemap = upper_map;
		inv->qu_curr_nemap = upper_map;
	}
}

void
neuromap::map_set_all_ne_curr_dom(brain& brn){
	row<neuron*>& filled = na_non_forced;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			BRAIN_CK(fll_neu.ne_curr_nemap != this);
			fll_neu.ne_curr_nemap = this;
		}
	}
}

void
neuromap::map_reset_all_ne_curr_dom(brain& brn){
	neuromap* upper_map = brn.get_last_upper_map();
	row<neuron*>& filled = na_non_forced;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			BRAIN_CK(fll_neu.ne_curr_nemap != upper_map);
			fll_neu.ne_curr_nemap = upper_map;
		}
	}
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
neuromap::map_make_all_qu_dominated(brain& brn){
}

void
neuromap::map_make_all_ne_dominated(brain& brn){
}

void
deducer::init_deducer(brain* brn){
	long tg_lv = INVALID_LEVEL;
	
	de_brain = brn;

	if(brn != NULL_PT){
		init_nk_with_note0(de_dct_nkpr, brn, tg_lv);
		init_nk_with_dots(de_nmp_nkpr, brn, tg_lv);
		
		de_dct_ref.init_nkref(&(brn->br_charge_trail));
		de_nmp_ref.init_nkref(&(brn->br_charge_trail));
	}
	
	de_first_bk_psig.init_prop_signal();
	de_next_bk_psig.init_prop_signal();
	de_all_noted.clear(true, true);

	de_nxt_nmp = NULL_PT;
	de_nxt_dct.init_deduction();
	BRAIN_CK(de_nxt_dct.is_dt_virgin());
}

void
deducer::init_nk_with_dots(notekeeper& nkpr, brain* brn, long tg_lv){
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
deducer::init_nk_with_note0(notekeeper& nkpr, brain* brn, long tg_lv){
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
deducer::find_next_noted(notekeeper& nkpr, nkref& nkr){
	quanton* nxt_qua = nkr.get_curr_quanton();
	while(nxt_qua != NULL_PT){
		nxt_qua = nkr.get_curr_quanton();
		if(nxt_qua == NULL_PT){
			break;
		}
		BRAIN_CK(nxt_qua != NULL_PT);
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
deducer::find_next_source(notekeeper& nkpr, nkref& nkr, row<prop_signal>& all_noted, 
						  bool only_origs)
{
	//brain& brn = get_de_brain();
	
	neuron*& nxt_src = de_next_bk_psig.ps_source;

	bool is_first = (nxt_src == tg_confl());

	BRAIN_CK(! is_first || (de_all_noted.size() <= 1));
	BRAIN_CK(! is_first || (nxt_src != NULL));
	DBG_PRT(20, os << "reasoning_cause_of_conflict " << nxt_src;
		os << " tot_noted=" << nkpr.dk_tot_noted;
		os << " num_noted_in_lv=" << nkpr.dk_num_noted_in_layer;
		os << " note layer=" << nkpr.dk_note_layer;
	);

	if((nxt_src != NULL) && (! only_origs || nxt_src->ne_original)){
		BRAIN_CK(! nxt_src->ne_fibres.is_empty());
		BRAIN_CK(is_first || (nxt_src->ne_fibres[0]->get_charge() == cg_positive) );
		BRAIN_CK(is_first || nxt_src->neu_compute_binary());

		row_quanton_t& causes = nxt_src->ne_fibres;
		long from = (is_first)?(0):(1);
		long until = causes.size();
		nkpr.set_motive_notes(nxt_src->ne_fibres, from, until);
	}

	BRAIN_CK(nkpr.dk_tot_noted > 0);

	find_next_noted(nkpr, nkr);

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
	
	prop_signal& nxt_sig = de_all_noted.inc_sz();
	nxt_sig = de_next_bk_psig;

	nkpr.nk_reset_note(nxt_qua);
	BRAIN_CK(nxt_qua.qlevel() == nkpr.dk_note_layer);
	BRAIN_CK(nxt_qua.is_pos());

	nkpr.dec_notes();

	DBG_PRT(101, os << "qua=" << &nxt_qua << " filled APPEND=";
		nxt_qua.qu_full_charged.print_row_data(os, true, "\n");
	);
}

void
deducer::deduction_analysis(prop_signal const & confl_sg, deduction& dct){
	brain& brn = get_de_brain();
	nkref& nkr = de_dct_ref;
	notekeeper& nkpr = de_dct_nkpr;
	row<prop_signal>& all_noted = de_all_noted;
	
	de_first_bk_psig = confl_sg;
	de_next_bk_psig = confl_sg;
	nkpr.init_notes(brn.level());
	nkr.reset_curr_quanton();
	
	de_all_noted.clear(true, true);	
	prop_signal& fst_sig = de_all_noted.inc_sz();
	fst_sig = confl_sg;

	BRAIN_CK(confl_sg.ps_source != NULL);
	BRAIN_CK(nkpr.nk_get_counter() == 0);
	BRAIN_CK(nkpr.dk_note_layer != INVALID_LEVEL);
	BRAIN_CK(nkpr.dk_note_layer != ROOT_LEVEL);
	BRAIN_CK(nkpr.dk_note_layer > 0);
	BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
	BRAIN_CK(! tg_confl()->ne_fibres.is_empty());
	DBG(row_quanton_t tmp_mots);
	DBG(nkpr.get_all_ordered_motives(tmp_mots));
	BRAIN_CK(tmp_mots.is_empty());
	BRAIN_CK(nkr.has_curr_quanton());
	BRAIN_CK(nkpr.dk_note_layer == brn.level());
	
	//neuromap& fst_nmp = brn.locate_neuromap();
	//de_nxt_nmp = &fst_nmp;

	find_next_source(nkpr, nkr, all_noted);
	while(! is_end_of_dct()){
		BRAIN_CK(nkr.has_curr_quanton());
		find_next_source(nkpr, nkr, all_noted);
	}
	BRAIN_CK(nkr.has_curr_quanton());

	fill_dct(nkpr, nkr, dct);
	
	make_all_ps_dominated(brn, all_noted);
	update_all_ps_deduc_tk(brn, all_noted);

	// reset all

	nkpr.clear_all_motives();

	DBG(nkpr.get_all_ordered_motives(tmp_mots));
	BRAIN_CK(tmp_mots.is_empty());

	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
deducer::fill_dct(notekeeper& nkpr, nkref& nkr, deduction& dct){
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

	nkpr.get_all_ordered_motives(dct.dt_motives);

	find_max_level(dct.dt_motives, dct.dt_target_level);

	BRAIN_CK(! nkpr.nk_has_note(opp));
	dct.dt_forced = &opp;

	BRAIN_CK(dct.dt_target_level < nkpr.dk_note_layer);
	BRAIN_CK(ck_motives(brn, dct.dt_motives));

	DBG_PRT(20, os << "find_dct of deduction=" << dct);
}

void
deducer::fill_nmp(notekeeper& nkpr, nkref& nkr){
	brain& brn = get_de_brain();
	quanton* pt_qua = nkr.get_curr_quanton();
	BRAIN_CK(pt_qua != NULL_PT);
	quanton& qua = *pt_qua;
	
	quanton& opp = qua.opposite();
	neuromap& nmp = nxt_nmp();
	
	qua.qu_full_charged.append_to(nmp.na_fll_in_lv);
	opp.qu_full_charged.append_to(nmp.na_fll_in_lv);

	prop_signal& n_sig = nmp.na_forced.inc_sz();
	n_sig.init_qua_signal(qua);
	n_sig.make_ps_dominated(brn);
}


void
deducer::neuromap_write_analysis(deduction& dct, row<neuromap*>& all_nmps){
	brain& brn = get_de_brain();
	row<leveldat*>& all_lv = brn.br_data_levels;
	
	BRAIN_CK(brn.level() == all_lv.last_idx());
	
	long tg_lv = dct.dt_target_level;
	
	all_nmps.clear();
	
	for(int aa = all_lv.last_idx(); aa >= 0; aa--){
		BRAIN_CK(all_lv[aa] != NULL_PT);
		leveldat& lv = *(all_lv[aa]);
		BRAIN_CK(aa == lv.ld_idx);
		
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

neuromap*
deducer::neuromap_find_analysis(deduction& dct){
	neuromap* out_nmp = NULL_PT;
	return out_nmp;
}

void
deducer::neuromap_setup_analysis(deduction& dct, row<neuromap*>& all_nmps){
}

