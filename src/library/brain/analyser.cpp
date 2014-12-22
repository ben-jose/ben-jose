

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

analyser.cpp  

analyser class methos and funcs.

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
analyser::init_analyser(brain* brn){
	long tg_lv = INVALID_LEVEL;
	
	de_brain = brn;

	if(brn != NULL_PT){
		init_nk_with_note0(de_nkpr, brn, tg_lv);
		de_ref.init_nkref(&(brn->br_charge_trail));
	}
	de_not_sel_neus.clear_all_neurons();
	de_all_noted.clear(true, true);
	
	de_first_bk_psig.init_prop_signal();
	de_next_bk_psig.init_prop_signal();
}

/*void
analyser::init_nk_with_dots(notekeeper& nkpr, brain* brn, long tg_lv){
	long* pt_tot_dots = NULL_PT;
	if(brn != NULL_PT){
		pt_tot_dots = &(brn->br_tot_qu_dots);
	}

	nkpr.init_notekeeper(brn, tg_lv);
	nkpr.init_funcs(pt_tot_dots, &quanton::has_dot, 
					&quanton::set_dot, &quanton::reset_dot, 
					&set_dots_of, &reset_dots_of);
}*/

void
analyser::init_nk_with_note0(notekeeper& nkpr, brain* brn, long tg_lv){
	long* pt_tot_note0 = NULL_PT;
	if(brn != NULL_PT){
		pt_tot_note0 = &(brn->br_qu_tot_note0);
	}
	nkpr.init_notekeeper(brn, tg_lv);
	nkpr.init_funcs(pt_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0, 
								&append_all_not_note0, &same_quantons_note0
   				);
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
analyser::deduction_analysis(row_quanton_t& causes, deduction& dct){
	//brain& brn = get_de_brain();
	nkref& nkr = de_ref;
	MARK_USED(nkr);
	notekeeper& nkpr = de_nkpr;
	
	deduction_init(causes);
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
analyser::fill_non_forced_from(brain& brn, row<neuron*>& all_neus, 
							   row<neuron*>& sel_neus, neurolayers& not_sel_neus, 
							   row_quanton_t& nmp_upper_quas, long min_ti, long max_ti)
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
		
		row_quanton_t& neu_upper_quas = brn.br_tmp_neu_upper_qu;
		neu_upper_quas.clear();

		long min_pos_ti = INVALID_TIER;
		bool is_in_nmp = neu.in_neuromap(min_ti, max_ti, neu_upper_quas, min_pos_ti);

		if(is_in_nmp){
			if(min_pos_ti == INVALID_TIER){
				sel_neus.push(&neu);

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
analyser::fill_non_forced(neuromap& nxt_nmp){
	brain& brn = get_de_brain();
	neurolayers& not_sel_neus = de_not_sel_neus;
	row<neuron*>& all_fll_in_lower = brn.br_tmp_fill_non_forced;
	row<neuron*>& sel_neus = brn.br_tmp_selected;
	row_quanton_t& nmp_upper_quas = brn.br_tmp_nmp_upper_qu;
	
	nmp_upper_quas.clear();
	
	long min_ti = nxt_nmp.get_min_tier();
	long max_ti = nxt_nmp.get_max_tier();
	
	BRAIN_CK(min_ti != INVALID_TIER);
	BRAIN_CK(max_ti != INVALID_TIER);
	BRAIN_CK(max_ti >= min_ti);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_qu_tot_note3 == 0);

	nxt_nmp.map_set_all_marks_and_spots();
	
	sel_neus.clear();

	fill_non_forced_from(brn, nxt_nmp.na_all_filled_in_propag, sel_neus, not_sel_neus, 
						 nmp_upper_quas, min_ti, max_ti);
	
	all_fll_in_lower.clear();
	not_sel_neus.get_all_ordered_neurons(all_fll_in_lower, min_ti, max_ti + 1);
	fill_non_forced_from(brn, all_fll_in_lower, sel_neus, not_sel_neus, nmp_upper_quas, 
						 min_ti, max_ti);

	// finalize
	
	reset_all_note3(brn, nmp_upper_quas);
	BRAIN_CK(brn.br_qu_tot_note3 == 0);
	
	sel_neus.move_to(nxt_nmp.na_non_forced);
	nmp_upper_quas.move_to(nxt_nmp.na_nf_upper_quas);
	
	BRAIN_CK(ck_motives(brn, nxt_nmp.na_nf_upper_quas));
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
analyser::calc_neuromap(prop_signal const & confl_sg, long min_lv, neuromap* prev_nmp)
{
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
	
	DBG(bool just_updated = false);
	while(nkr.get_curr_qlevel() > min_lv){
		DBG(just_updated = false);
		if(is_end_of_nmp()){
			DBG(just_updated = true);
			out_nmp = update_neuromap(out_nmp);
		}
		find_next_source(true);
	}
	
	if(out_nmp != NULL_PT){
		BRAIN_CK(just_updated);
		out_nmp->map_reset_all_marks_and_spots();
		BRAIN_CK(brn.br_tot_qu_marks == 0);
		BRAIN_CK(brn.br_tot_ne_spots == 0);
	}

	return out_nmp;
}

void
analyser::end_analysis(){
	notekeeper& nkpr = de_nkpr;
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);	
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
		if(is_upper){
			neu_upper_quas.push(&qua);
		}
	}
	
	BRAIN_CK(! is_fll || (dbg_num_after >= 2));
	return is_fll;
}

void
brain::neuromap_write_analysis(long tg_lv, row<neuromap*>& all_nmps){
	//brain& brn = get_de_brain();
	row<leveldat*>& all_lv = br_data_levels;
	
	BRAIN_CK(level() == all_lv.last_idx());
	
	all_nmps.clear();
	
	for(int aa = all_lv.last_idx(); aa >= 0; aa--){
		leveldat& lv = get_data_level(aa);
		
		if(aa == tg_lv){
			break;
		}
		
		binder* fst = lv.ld_nmps_to_write.bn_right;
		binder* lst = &(lv.ld_nmps_to_write);
		binder* rgt = NULL_PT;
		for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
			neuromap& nmp = as_neuromap(rgt);
			BRAIN_CK(nmp.na_active);
			all_nmps.push(&nmp);
		}
	}
}

neuromap*
analyser::neuromap_find_analysis(prop_signal const & confl_sg, 
								 long& nxt_lv, deduction& nxt_dct)
{
	brain& brn = get_de_brain();
	row_quanton_t& nmp_causes = brn.br_tmp_f_analysis;
	notekeeper& nkpr = de_nkpr;
	
	prop_signal aux_ps;
	nxt_dct.init_deduction();
	
	neuromap* out_nmp = calc_neuromap(confl_sg, nxt_lv, NULL_PT);
	BRAIN_CK(out_nmp != NULL_PT);
	while(out_nmp != NULL_PT){
		if(! out_nmp->map_find()){
			break;
		}
		if(nxt_lv <= 0){
			break;
		}
		
		nmp_causes.clear();
		out_nmp->map_get_all_upper_quas(nkpr, nmp_causes);
		
		nxt_dct.init_deduction();
		deduction_analysis(nmp_causes, nxt_dct);
		nxt_lv = nxt_dct.dt_target_level;
		
		out_nmp = calc_neuromap(aux_ps, nxt_lv, out_nmp);
		BRAIN_CK(out_nmp != NULL_PT);
	}
	return out_nmp;
}

long
analyser::find_min_lv_to_setup(long tg_lv){
	brain& brn = get_de_brain();
	row<leveldat*>& all_lv = brn.br_data_levels;
	
	BRAIN_CK(brn.level() == all_lv.last_idx());
	BRAIN_CK(all_lv.is_valid_idx(tg_lv));
	
	long min_lv = INVALID_LEVEL;
	for(int aa = tg_lv; aa > 0; aa--){
		leveldat& lv = brn.get_data_level(aa);

		if(! lv.has_setup_neuromap()){
			break;
		}
		min_lv = aa;
	}
	return min_lv;
}

void
analyser::neuromap_setup_analysis(long nxt_lv, neuromap* in_nmp){
	BRAIN_CK((in_nmp == NULL_PT) || (in_nmp->na_orig_lv > nxt_lv));
	
	long min_lv = find_min_lv_to_setup(nxt_lv);
	if(min_lv < 0){
		neuromap::full_release(in_nmp);
		return;
	}
	BRAIN_CK(min_lv <= nxt_lv);
	
	brain& brn = get_de_brain();
	prop_signal aux_ps;
	
	neuromap* setup_nmp = calc_neuromap(aux_ps, min_lv - 1, in_nmp);
	BRAIN_CK(setup_nmp != NULL_PT);
	BRAIN_CK(setup_nmp->na_orig_lv == min_lv);
	
	setup_nmp->na_is_head = true;
	neuromap* nxt_nmp = setup_nmp;
	
	while((nxt_nmp != NULL_PT) && (nxt_nmp->na_orig_lv <= nxt_lv)){
		BRAIN_CK(nxt_nmp != in_nmp);
		
		leveldat& lv_dat = brn.get_data_level(nxt_nmp->na_orig_lv);
		lv_dat.add_setup_neuromap(*nxt_nmp);
		
		nxt_nmp = nxt_nmp->na_submap;
	}
	
}

bool
brain::set_writing_neuromap(long nxt_lv, neuromap* in_nmp){
	leveldat& lv_w = get_data_level(nxt_lv);
	leveldat& lv_s = get_data_level(nxt_lv + 1);
	
	bool added_in_nmp = false;
	if(lv_s.has_setup_neuromap()){
		neuromap& s_nmp = lv_s.get_setup_neuromap();
		lv_w.add_write_neuromap(s_nmp);
	} else {
		if(in_nmp != NULL_PT){
			BRAIN_CK(! in_nmp->has_mates());
			lv_w.add_write_neuromap(*in_nmp);
			added_in_nmp = true;
		}
	}
	return added_in_nmp;
}

void
write_neuromaps(row<neuromap*>& to_wrt){
	for(int aa = 0; aa < to_wrt.size(); aa++){
		BRAIN_CK(to_wrt[aa] != NULL_PT);
		neuromap& nmp = *(to_wrt[aa]);
		nmp.map_write();
	}
}

void
brain::analyse(prop_signal const & confl, deduction& out_dct){
	br_deducer.deduction_analysis(confl, out_dct);
	
	long tg_lv = out_dct.dt_target_level;
	
	row<neuromap*>& to_wrt = br_tmp_maps_to_write;
	neuromap_write_analysis(tg_lv, to_wrt);
	write_neuromaps(to_wrt);
	
	neuromap* f_nmp = NULL_PT;
	if(to_wrt.is_empty()){
		deduction& fnd_dct = br_tmp_find_dct;
		long tg2 = tg_lv;
		f_nmp = br_neuromaper.neuromap_find_analysis(confl, tg2, fnd_dct);
		BRAIN_CK(f_nmp != NULL_PT);
		if(f_nmp != NULL_PT){
			tg_lv = tg2;
			fnd_dct.move_to_dct(out_dct);
			BRAIN_CK(tg_lv == out_dct.dt_target_level);
		}
	}
	
	set_writing_neuromap(tg_lv, f_nmp);
	
	br_neuromaper.neuromap_setup_analysis(tg_lv, f_nmp);
	br_neuromaper.end_analysis();
}
