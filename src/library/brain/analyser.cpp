

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
analyser::init_analyser(brain* brn){
	long tg_lv = INVALID_LEVEL;
	
	de_brain = brn;

	if(brn != NULL_PT){
		init_nk_with_note0(de_nkpr, brn, tg_lv);
		de_ref.init_qlayers_ref(&(brn->br_charge_trail));
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
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	quanton* nxt_qua = qlr.get_curr_quanton();
	while(nxt_qua != NULL_PT){
		nxt_qua = qlr.get_curr_quanton();
		if(nxt_qua == NULL_PT){
			break;
		}
		BRAIN_CK(nxt_qua != NULL_PT);
		if(nkpr.nk_has_note(*nxt_qua)){
			break;
		}
		
		qlr.dec_curr_quanton();
	}

	BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
	if(nxt_qua == NULL_PT){
		return;
	}
	//DBG_PRT(119, os << "nxt_no=" << nxt_qua);
	
	BRAIN_CK(nxt_qua != NULL_PT);
	BRAIN_CK(nkpr.nk_has_note(*nxt_qua));
	BRAIN_CK(nxt_qua->qu_id != 0);
	BRAIN_CK(nxt_qua->is_pos());
	
	long qlv = nxt_qua->qlevel();

	nkpr.update_notes_layer(qlv);
}

bool
analyser::is_first_source(){
	bool is_first = (de_next_bk_psig.ps_source == tg_confl());
	return is_first;
}

void
analyser::find_next_source(bool only_origs)
{
	DBG(
		notekeeper& nkpr = de_nkpr;
		row<prop_signal>& all_noted = de_all_noted;
	)
	
	neuron*& nxt_src = de_next_bk_psig.ps_source;

	bool is_first = is_first_source();

	BRAIN_CK(! is_first || (all_noted.size() <= 1));
	BRAIN_CK(! is_first || (nxt_src != NULL));

	if((nxt_src != NULL) && (! only_origs || nxt_src->ne_original)){
		set_notes_of(nxt_src->ne_fibres, is_first);
	}

	BRAIN_CK_PRT((only_origs || (nkpr.dk_tot_noted > 0)), 
				 os << "________"  << bj_eol;
				 os << "tot_no=" << nkpr.dk_tot_noted << bj_eol;
				 os << "cur_qu=" << de_ref.get_curr_quanton() << bj_eol;
				 os << "cur_qlv=" << de_ref.get_curr_qlevel());

	find_next_noted();
	inc_all_noted();
	BRAIN_CK(de_ref.get_curr_quanton() == last_qu_noted());
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
analyser::inc_all_noted(){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;
	
	quanton* pt_qua = qlr.get_curr_quanton();
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
}

bool
analyser::ck_deduction_init(long deduc_lv){
#ifdef FULL_DEBUG
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	BRAIN_CK(deduc_lv != INVALID_LEVEL);
	BRAIN_CK(nkpr.nk_get_counter() == 0);
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.dk_note_layer > 0);
	BRAIN_CK(nkpr.dk_note_layer == deduc_lv);
	BRAIN_CK(qlr.has_curr_quanton());
#endif
	return true;
}

void	
analyser::deduction_init(row_quanton_t& causes){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;

	long deduc_lv = find_max_level(causes);
	
	all_noted.clear(true, true);
	nkpr.init_notes(deduc_lv);
	qlr.reset_curr_quanton();
	de_first_bk_psig.init_prop_signal();
	de_next_bk_psig.init_prop_signal();

	BRAIN_CK(ck_deduction_init(deduc_lv));
	
	set_notes_of(causes, true);
	BRAIN_CK(nkpr.dk_tot_noted > 0);

	find_next_noted();
	inc_all_noted();
}

void
analyser::deduction_init(prop_signal const & confl_sg){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	row<prop_signal>& all_noted = de_all_noted;
	
	long deduc_lv = confl_sg.get_level();
	
	all_noted.clear(true, true);
	nkpr.init_notes(deduc_lv);
	qlr.reset_curr_quanton();
	
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
	BRAIN_CK(is_first_source());
	
	find_next_source();
}

void
analyser::deduction_analysis(row_quanton_t& causes, deduction& dct){
	BRAIN_CK(false); // CHECK THIS CODE
	//brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	MARK_USED(qlr);
	notekeeper& nkpr = de_nkpr;
	
	deduction_init(causes);
	while(! is_end_of_dct()){
		BRAIN_CK(qlr.has_curr_quanton());
		BRAIN_CK(! is_first_source());
		find_next_source();
	}
	BRAIN_CK(qlr.has_curr_quanton());

	fill_dct(dct);
	
	// reset all
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
analyser::deduction_analysis(prop_signal const & confl_sg, deduction& dct){
	//brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	MARK_USED(qlr);
	notekeeper& nkpr = de_nkpr;
	
	deduction_init(confl_sg);
	while(! is_end_of_dct()){
		BRAIN_CK(qlr.has_curr_quanton());
		BRAIN_CK(! is_first_source());
		find_next_source();
	}
	BRAIN_CK(qlr.has_curr_quanton());

	fill_dct(dct);
	
	// reset all
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
analyser::make_noted_dominated_and_deduced(){
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	row<prop_signal>& all_noted = de_all_noted;
	
	bool mk_deduc = true;
	make_all_ps_dominated(brn, all_noted, mk_deduc, 0, all_noted.last_idx());

	quanton* qua = qlr.get_curr_quanton();
	BRAIN_CK(qua != NULL_PT);	
	qua->make_qu_dominated(brn);
}

void
analyser::fill_dct(deduction& dct){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	DBG(brain& brn = get_de_brain());
	
	quanton* pt_qua = qlr.get_curr_quanton();
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
analyser::ck_end_of_lrn_nmp(){
	BRAIN_CK(de_next_bk_psig.ps_source != NULL_PT);
	BRAIN_CK(! de_next_bk_psig.ps_source->ne_original);
	
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	MARK_USED(qlr);
	quanton* qua = de_next_bk_psig.ps_quanton;
	
	BRAIN_CK(qua != NULL_PT);
	BRAIN_CK(qua == qlr.get_curr_quanton());
	
	long qti = qua->qu_tier;
	long num_quas = brn.br_charge_trail.get_qu_layer(qti).size();
	BRAIN_CK(num_quas == 1);

	long q_lv = qua->qlevel();
	bool has_lnd = brn.get_data_level(q_lv).has_learned();
	BRAIN_CK(has_lnd);
	
	return true;
}

neuromap*
analyser::update_neuromap(neuromap* sub_nmp){
	BRAIN_CK(is_end_of_nmp());
	
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	row<prop_signal>& all_noted = de_all_noted;

	neuromap& nxt_nmp = brn.locate_neuromap();
	nxt_nmp.na_submap = sub_nmp;
	nxt_nmp.na_setup_tk.update_ticket(brn);
	
	quanton* qua = qlr.get_curr_quanton();
	//bool eolv = is_end_of_lv();
	//if(eolv){
		nxt_nmp.na_orig_lv = qua->qlevel();
		nxt_nmp.na_orig_cho = qua;
	//}
	all_noted.move_to(nxt_nmp.na_forced);
	
	DBG_PRT(124, os << "tot_no_sel_neus_1=" << de_not_sel_neus.get_tot_neurons());
	nxt_nmp.set_all_filled_in_propag();
	DBG_PRT(124, os << "tot_no_sel_neus_2=" << de_not_sel_neus.get_tot_neurons());
	nxt_nmp.map_fill_non_forced(de_not_sel_neus);

	return &nxt_nmp;
}

void
analyser::init_calc_nmp(prop_signal const & confl_sg, long min_lv)
{
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	neurolayers& not_sel_neus = de_not_sel_neus;
	row<prop_signal>& all_noted = de_all_noted;

	de_first_bk_psig = confl_sg;
	de_next_bk_psig = confl_sg;
	nkpr.init_notes(brn.level());
	not_sel_neus.clear_all_neurons();
	qlr.reset_curr_quanton();
	
	all_noted.clear(true, true);
	prop_signal& fst_sig = all_noted.inc_sz();
	fst_sig = confl_sg;

	long qlv = qlr.get_curr_qlevel();
	MARK_USED(qlv);
	
	DBG_PRT(124, os << "tot_no_sel_neus_0=" << de_not_sel_neus.get_tot_neurons());
	
	BRAIN_CK(brn.br_ne_tot_tag1 == 0);
	BRAIN_CK(confl_sg.ps_quanton != NULL);
	BRAIN_CK(confl_sg.ps_quanton->get_charge() == cg_negative);
	BRAIN_CK(confl_sg.ps_source != NULL);
	BRAIN_CK(nkpr.nk_get_counter() == 0);
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.dk_note_layer > 0);
	BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
	BRAIN_CK(! tg_confl()->ne_fibres.is_empty());
	BRAIN_CK(min_lv < qlv);
	BRAIN_CK(is_first_source());
	
	//out_nmp = NULL_PT;

	find_next_source(true);
}

neuromap*
analyser::calc_neuromap(prop_signal const & confl_sg, long min_lv, neuromap* prev_nmp)
{
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	
	neuromap* out_nmp = prev_nmp;

	DBG_PRT(119, os << "clc_nmp{ " << "cur_qu=" << de_ref.get_curr_quanton()
			<< " tot_no=" << de_nkpr.dk_tot_noted
			<< " min_lv=" << min_lv 
			<< " cur_qlv=" << qlr.get_curr_qlevel() << bj_eol
			<< " prev_nmp=" << prev_nmp 
			<< "}");
	
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
	
	BRAIN_CK(brn.br_data_levels.is_valid_idx(min_lv));
	if(out_nmp == NULL_PT){
		init_calc_nmp(confl_sg, min_lv);
	}
	
	DBG(bool just_updated = false);
	while(qlr.get_curr_qlevel() > min_lv){
		DBG(just_updated = false);
		if(is_end_of_nmp()){
			DBG(just_updated = true);
			out_nmp = update_neuromap(out_nmp);
		}
		BRAIN_CK(! is_first_source());
		find_next_source(true);
	}
	
	if(out_nmp != NULL_PT){
		BRAIN_CK(just_updated);
		out_nmp->map_reset_all_marks_and_spots();
		BRAIN_CK(out_nmp->dbg_ck_all_neus());
	}
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	BRAIN_CK(brn.br_tot_ne_spots == 0);

	DBG_PRT(119, os << "CLC_NMP{" 
			<< " just_updated=" << just_updated
			<< " min_lv=" << min_lv
			<< " cur_qu=" << de_ref.get_curr_quanton()
			<< " cur_qlv=" << qlr.get_curr_qlevel() << bj_eol
			<< " nxt_ps=" << de_next_bk_psig
			<< " out_nmp=" << out_nmp << bj_eol
			<< " eolv=" << is_end_of_lv()
			<< " eonmp=" << is_end_of_nmp()
			<< "}");
	
	return out_nmp;
}

void
analyser::end_analysis(){
	notekeeper& nkpr = de_nkpr;
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);	
}

long
brain::append_all_to_write(long fst_lv, long tg_lv, row<neuromap*>& all_nmps){
	BRAIN_CK(fst_lv >= tg_lv);
	
	row<leveldat*>& all_lv = br_data_levels;	
	BRAIN_CK(level() == all_lv.last_idx());
	BRAIN_CK(all_lv.is_valid_idx(fst_lv));

	long min_has_setup_lv = INVALID_LEVEL;
	for(int aa = fst_lv; aa >= 0; aa--){
		leveldat& lv = get_data_level(aa);
		
		if(aa == tg_lv){
			break;
		}
		if(lv.has_setup_neuromap()){
			min_has_setup_lv = aa;
		}
		if(! lv.has_to_write_neuromaps()){
			continue;
		}
		
		lv.ld_nmps_to_write.append_all_as<neuromap>(all_nmps);
	}
	return min_has_setup_lv;
}

neuromap*
analyser::neuromap_find_analysis(analyser& deducer, prop_signal const & confl_sg, 
					long& nxt_lv, deduction& nxt_dct, row<neuromap*>& to_wrt)
{
	brain& brn = get_de_brain();
	row_quanton_t& nmp_causes = brn.br_tmp_f_analysis;
	notekeeper& nkpr = de_nkpr;
	
	if(! to_wrt.is_empty()){
		return NULL_PT;
	}
	if(brn.lv_has_setup_nmp(nxt_lv + 1)){
		return NULL_PT;
	}
	
	nxt_dct.init_deduction();
	
	neuromap* out_nmp = calc_neuromap(confl_sg, nxt_lv, NULL_PT);
	BRAIN_CK(out_nmp != NULL_PT);
	while(out_nmp != NULL_PT){
		if(! out_nmp->map_find()){
			DBG_PRT(118, os << "CANNOT find nmp=" << (void*)(out_nmp));
			break;
		}
		DBG_PRT(118, os << "found nmp=" << (void*)(out_nmp));
		if(nxt_lv <= 0){
			break;
		}
		
		nmp_causes.clear();
		out_nmp->map_get_all_upper_quas(nkpr, nmp_causes);
		
		nxt_dct.init_deduction();
		deducer.deduction_analysis(nmp_causes, nxt_dct);
		make_noted_dominated_and_deduced();
		
		long tg_lv = nxt_dct.dt_target_level;
		long min_s_lv = brn.append_all_to_write(nxt_lv, tg_lv, to_wrt);
		MARK_USED(min_s_lv);
		
		nxt_lv = tg_lv;

		if(! to_wrt.is_empty()){
			break;
		}
		if(brn.lv_has_setup_nmp(nxt_lv + 1)){
			BRAIN_CK(min_s_lv == (nxt_lv + 1));
			break;
		}
		BRAIN_CK(min_s_lv == INVALID_LEVEL);
		
		out_nmp = calc_neuromap(confl_sg, nxt_lv, out_nmp);
		BRAIN_CK(out_nmp != NULL_PT);
	}
	return out_nmp;
}

long
analyser::find_min_lv_to_setup(long tg_lv){
	brain& brn = get_de_brain();
	row<leveldat*>& all_lv = brn.br_data_levels;
	MARK_USED(all_lv);
	
	BRAIN_CK(brn.level() == all_lv.last_idx());
	BRAIN_CK(all_lv.is_valid_idx(tg_lv));
	
	long min_lv = INVALID_LEVEL;
	for(int aa = tg_lv; aa > 0; aa--){
		leveldat& lv = brn.get_data_level(aa);

		if(lv.has_setup_neuromap()){
			break;
		}
		/*BRAIN_CK_PRT(! lv.has_learned(), os << "______" << bj_eol;
			brn.print_trail(os); 
			os << "LEV=" << &lv;
		);*/
		BRAIN_CK(! lv.has_active_neuromaps());
		min_lv = aa;
	}
	return min_lv;
}

neuromap*
analyser::neuromap_setup_analysis(prop_signal const & confl_sg, long nxt_lv, 
								  neuromap* in_nmp)
{
	brain& brn = get_de_brain();
	
	BRAIN_CK(nxt_lv != INVALID_LEVEL);
	BRAIN_CK((in_nmp == NULL_PT) || (in_nmp->na_orig_lv > nxt_lv));
	BRAIN_CK((in_nmp == NULL_PT) || ! in_nmp->na_is_head);
	
	bool in_nmp_is_last_nmp = ((in_nmp != NULL_PT) && ! brn.lv_has_setup_nmp(nxt_lv + 1));
	
	long min_lv = find_min_lv_to_setup(nxt_lv);
	if(min_lv < 0){
		if(in_nmp_is_last_nmp){
			in_nmp->na_is_head = true;
			DBG(in_nmp->na_dbg_orig_lv = in_nmp->na_orig_lv);
			in_nmp->na_orig_lv = nxt_lv + 1;
			in_nmp->map_activate(dbg_call_1);
		}
		return in_nmp;
	}
	BRAIN_CK(min_lv <= nxt_lv);

	DBG_PRT(123, 
			os << "set_ana{"
			<< " in_nmp=" << in_nmp
			<< " nxt_lv=" << nxt_lv
			<< " has_lv+1=" << brn.lv_has_setup_nmp(nxt_lv + 1)
			<< " cur_qu=" << de_ref.get_curr_quanton()
			<< " tot_no=" << de_nkpr.dk_tot_noted
			<< " min_lv=" << min_lv << " nxt_lv=" << nxt_lv
			<< " all_no=" << de_nkpr.dk_quas_lyrs 
			<< "}" );
	
	neuromap* setup_nmp = calc_neuromap(confl_sg, min_lv - 1, in_nmp);
	BRAIN_CK(setup_nmp != NULL_PT);
	BRAIN_CK_PRT(setup_nmp->na_orig_lv >= min_lv, 
		os << "__________" << bj_eol 
			<< " in_nmp=" << in_nmp
			<< " setup_nmp=" << setup_nmp
			<< " min_lv=" << min_lv
			<< " nxt_lv=" << nxt_lv
	);
	
	if(setup_nmp != NULL_PT){
		setup_nmp->map_make_dominated();
		setup_nmp->na_is_head = true;
	}

	neuromap* nxt_nmp = setup_nmp;
	
	while((nxt_nmp != NULL_PT) && (nxt_nmp->na_orig_lv <= nxt_lv)){
		BRAIN_CK(nxt_nmp != in_nmp);
		nxt_nmp->map_activate(dbg_call_2);
		nxt_nmp = nxt_nmp->na_submap;
	}

	if(in_nmp_is_last_nmp){
		DBG_PRT(123, os << "SET_ANAL{ nmp_act=" << in_nmp);
		DBG(in_nmp->na_dbg_orig_lv = in_nmp->na_orig_lv);
		in_nmp->na_orig_lv = nxt_lv + 1;
		in_nmp->map_activate(dbg_call_3);
	}
	
	DBG_PRT(123, os << "SET_ANAL{ is_lst=" << in_nmp_is_last_nmp);
	return setup_nmp;
}

void
brain::add_lv_neuromap_to_write(long nxt_lv){
	leveldat& lv_s = get_data_level(nxt_lv + 1);
	
	BRAIN_CK_PRT(lv_s.has_setup_neuromap(), dbg_prt_margin(true));
	if(lv_s.has_setup_neuromap()){
		neuromap& nmp = lv_s.get_setup_neuromap();
		
		BRAIN_CK(nmp.na_active);
		nmp.na_mates.let_go();
		
		leveldat& lv_w = get_data_level(nxt_lv);
		lv_w.ld_nmps_to_write.bind_to_my_right(nmp.na_mates);
	} 
}

void
write_all_neuromaps(row<neuromap*>& to_wrt){
	for(int aa = 0; aa < to_wrt.size(); aa++){
		BRAIN_CK(to_wrt[aa] != NULL_PT);
		neuromap& nmp = *(to_wrt[aa]);
		nmp.map_write();
		nmp.map_deactivate();
	}
}

void
brain::analyse(prop_signal const & confl, deduction& out_dct){
	analyser& dedser = br_deducer_anlsr;
	
	dedser.deduction_analysis(confl, out_dct);
	dedser.make_noted_dominated_and_deduced();

	BRAIN_CK(br_ne_tot_tag1 == 0);
	DBG_PRT(125, os << "bef_ana=" << bj_eol; print_trail(os));
	
	row<neuromap*>& to_wrt = br_tmp_maps_to_write;
	to_wrt.clear();
	
	long fst_lv = level();
	long tg_lv = out_dct.dt_target_level;
	
	long min_s_lv = append_all_to_write(fst_lv, tg_lv, to_wrt);
	MARK_USED(min_s_lv);
	BRAIN_CK_PRT((lv_has_setup_nmp(tg_lv + 1) == (min_s_lv != INVALID_LEVEL)), 
		os << "______\n"; dbg_prt_margin();
		os << " fst_lv=" << fst_lv << " tg_lv=" << tg_lv 
		<< " min_s_lv=" << min_s_lv << " to_w_sz=" << to_wrt.size() << bj_eol;
		print_trail(os);
	);
	
	analyser& mper = br_neuromaper_anlsr;
	
	deduction& fnd_dct = br_tmp_find_dct;
	long tg2 = tg_lv;
	neuromap* f_nmp = mper.neuromap_find_analysis(dedser, confl, tg2, fnd_dct, to_wrt);
	if(tg_lv != tg2){
		BRAIN_CK(f_nmp != NULL_PT);
		BRAIN_CK(! fnd_dct.is_dt_virgin());
		if(f_nmp->na_orig_lv < min_s_lv){
			min_s_lv = f_nmp->na_orig_lv;
		}
		
		tg_lv = tg2;
		out_dct.init_deduction();
		fnd_dct.move_to_dct(out_dct);
		BRAIN_CK(tg_lv == out_dct.dt_target_level);
	}

	f_nmp = mper.neuromap_setup_analysis(confl, tg_lv, f_nmp); // activates new maps
	
	add_lv_neuromap_to_write(tg_lv);
	
	mper.end_analysis();
	
	BRAIN_CK((f_nmp == NULL_PT) || ! f_nmp->na_active || f_nmp->na_is_head);
	if((f_nmp != NULL_PT) && ! f_nmp->na_active){
		BRAIN_CK(! f_nmp->na_is_head);
		f_nmp->na_is_head = true;
		f_nmp->map_add_to_release();
	}
	DBG(
		if(f_nmp != NULL_PT){
			row<neuron*>& all_neus = br_tmp_ck_neus;
			all_neus.clear();
			f_nmp->map_get_all_neus(all_neus);
			reset_all_tag1(*this, all_neus);
		}
	)
	BRAIN_CK(br_ne_tot_tag1 == 0);
	
	write_all_neuromaps(to_wrt); // deactivates final wrt maps
	
	release_all_neuromaps();
	
	//DBG_PRT(125, os << "AFT_ANA=" << "out_dct=" << out_dct << bj_eol; print_trail(os));
}

void
brain::release_all_neuromaps(){
	row<neuromap*>& all_nmps = br_nmps_to_release;
	DBG_PRT(132, os << "ALL_NMPS="; all_nmps.print_row_data(os, true, "\n"));
	
	while(! all_nmps.is_empty()){
		BRAIN_CK(all_nmps.last() != NULL_PT);
		neuromap& nmp = *(all_nmps.pop());
		BRAIN_CK_PRT(nmp.na_is_head, os << "____" << bj_eol << "NMP=" << &nmp);
		nmp.full_release();
	}
}
