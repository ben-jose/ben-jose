

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

