

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

Copyright (C) 2011, 2014-2015. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
email: joseluisquirogabeltran@gmail.com

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

analyser.cpp  

analyser class methos and funcs.

--------------------------------------------------------------*/

#include "brain.h"
#include "solver.h"
#include "dbg_strings_html.h"

//============================================================
// static vars

//neuromap*	analyser::NULL_NEUROMAP = NULL_PT;

//============================================================
// aux funcs

void
analyser::reset_deduc(){
	de_all_noted.clear(true, true);
	
	//de_all_learned_propag.clear();
	
	de_next_bk_psig.init_prop_signal();
}

void
analyser::init_analyser(brain* brn){
	de_brain = brn;

	bool has_br = (brn != NULL_PT);
	if(has_br){
		de_ref.init_qlayers_ref(&(brn->br_charge_trail));
	}
	de_all_confl.clear(true, true);

	reset_deduc();
}

void
analyser::init_nk_with_note0(notekeeper& nkpr, brain& brn){
	long* pt_tot_note0 = NULL_PT;
	pt_tot_note0 = &(brn.br_qu_tot_note0);
	nkpr.init_notekeeper(&brn, INVALID_LEVEL);
	nkpr.init_funcs(pt_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0, 
								&append_all_not_note0, &same_quantons_note0
   				);
	
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
analyser::init_nk_with_note5(notekeeper& nkpr, brain& brn){
	long* pt_tot_note5 = NULL_PT;
	pt_tot_note5 = &(brn.br_qu_tot_note5);
	nkpr.init_notekeeper(&brn, INVALID_LEVEL);
	nkpr.init_funcs(pt_tot_note5, &quanton::has_note5, 
							   &quanton::set_note5, &quanton::reset_its_note5, 
								&set_all_note5, &reset_all_its_note5, 
								&append_all_not_note5, &same_quantons_note5
   				);
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

bool
ck_ord_last_tiers(brain& brn, row<prop_signal>& all_noted){
#ifdef FULL_DEBUG
	if(all_noted.size() < 2){
		return true;
	}
	BRAIN_DBG(
		long cur_idx = all_noted.last_idx();
		long prv_idx = cur_idx - 1;
		prop_signal& cur_ps = all_noted[cur_idx];
		prop_signal& prv_ps = all_noted[prv_idx];
		long c_ti = cur_ps.ps_tier;
		long p_ti = prv_ps.ps_tier;
	)
	BRAIN_CK_PRT((c_ti <= p_ti), os << "____________\n" ;
		ch_string ff = brn.dbg_prt_margin(os); os << ff << bj_eol;
		all_noted.print_row_data(os, true, "\n");
		brn.print_trail(os);
	);
#endif
	return true;
}

void
analyser::set_nxt_noted(quanton* pt_nxt_qua){
	if(pt_nxt_qua == NULL_PT){
		de_next_bk_psig.init_prop_signal();
		return;
	}
	BRAIN_CK(pt_nxt_qua != NULL_PT);
			
	quanton& nxt_qua = *pt_nxt_qua;
	BRAIN_CK(nxt_qua.qu_id != 0);
	BRAIN_CK(nxt_qua.is_pos());
	
	notekeeper& nkpr = de_nkpr;
	if(! nkpr.nk_has_note(nxt_qua)){
		BRAIN_CK(false);
		return;
	}
	
	long qlv = nxt_qua.qlevel();
	nkpr.update_notes_layer(qlv);
	
	row<prop_signal>& all_noted = de_all_noted;
	
	de_next_bk_psig.init_qua_signal(nxt_qua);
	
	prop_signal& nxt_sig = all_noted.inc_sz();
	nxt_sig = de_next_bk_psig;
	if((nxt_sig.ps_source != NULL_PT) && ! nxt_sig.ps_source->ne_original){
		BRAIN_CK(ck_end_of_lrn_nmp());
		nxt_sig.ps_source = NULL_PT;
	}
	BRAIN_CK(ck_ord_last_tiers(get_de_brain(), all_noted));

	nkpr.nk_reset_note(nxt_qua);
	BRAIN_CK(nxt_qua.qlevel() == nkpr.dk_note_layer);
	nkpr.dec_notes();

	BRAIN_CK_PRT((pt_nxt_qua == last_qu_noted()),
			os << "cur_qu=" << de_ref.get_curr_quanton()
			<< " l_qu_not=" << last_qu_noted()
	);
}

bool
analyser::find_next_noted(){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	quanton* nxt_qua = qlr.get_curr_quanton();
	
	bool in_end = false;
	while(nxt_qua != NULL_PT){
		nxt_qua = qlr.get_curr_quanton();
		if(nxt_qua == NULL_PT){
			de_next_bk_psig.init_prop_signal();
			break;
		}
		if(nkpr.nk_has_note(*nxt_qua)){
			set_nxt_noted(nxt_qua);
			break; 
		}
		
		qlr.dec_curr_quanton();
	}

	BRAIN_CK(nkpr.dk_note_layer <= get_de_brain().level());
	return in_end;
}

bool
analyser::find_next_source()
{
	BRAIN_DBG(notekeeper& nkpr = de_nkpr);
	
	neuron*& nxt_src = de_next_bk_psig.ps_source;
	
	if(nxt_src != NULL){
		DBG_PRT(33, os << " nxt_ps=" << de_next_bk_psig << "\n";
			os << " src=" << nxt_src;
		);
		set_notes_of(nxt_src->ne_fibres, false);
	}

	BRAIN_CK_PRT((nkpr.dk_tot_noted > 0), 
				 os << "________"  << bj_eol;
				 os << "tot_no=" << nkpr.dk_tot_noted << bj_eol;
				 os << "cur_qu=" << de_ref.get_curr_quanton() << bj_eol;
				 os << "cur_qlv=" << de_ref.get_curr_qlevel());

	bool in_end = find_next_noted();
	return in_end;
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

bool
analyser::ck_deduc_init(long deduc_lv, bool full_ck){
#ifdef FULL_DEBUG
	BRAIN_DBG(
		qlayers_ref& qlr = de_ref;
		notekeeper& nkpr = de_nkpr;
		brain& brn = get_de_brain();
		bool in_root = (brn.level() == ROOT_LEVEL);
	)
	BRAIN_CK(nkpr.nk_get_counter() == 0);
	
	//long num_propag = (full_ck && qlr.has_curr_quanton()) ? (1) : (0);
	
	BRAIN_CK(nkpr.dk_quas_lyrs.is_ql_empty());
	BRAIN_CK(de_nkpr.nk_get_counter() == 0);
	BRAIN_CK(de_all_noted.size() == 0); 
	
	BRAIN_CK(! full_ck || in_root || (nkpr.dk_note_layer > 0));
	BRAIN_CK(! full_ck || (deduc_lv != INVALID_LEVEL));
	BRAIN_CK(! full_ck || (nkpr.dk_note_layer == deduc_lv));
	BRAIN_CK(! full_ck || (qlr.has_curr_quanton()));
#endif
	return true;
}

void	
analyser::deduction_init(row_quanton_t& causes, long max_lv){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	
	BRAIN_CK(causes.size() > 1);

	reset_deduc();
	BRAIN_CK(ck_deduc_init(INVALID_LEVEL, false));
	
	long deduc_lv = max_lv;
	if(deduc_lv == INVALID_LEVEL){
		deduc_lv = find_max_level(causes);
	}
	BRAIN_CK(deduc_lv != INVALID_LEVEL);
	
	nkpr.init_notes(deduc_lv);
	qlr.reset_curr_quanton();
	
	BRAIN_CK(ck_deduc_init(deduc_lv, true));
	
	set_notes_of(causes, true);
	
	BRAIN_CK_PRT((nkpr.dk_tot_noted > 0), get_de_brain().dbg_prt_margin(os));
	
	find_next_noted();
}

void
analyser::deduction_analysis(row_quanton_t& causes, deduction& dct, long max_lv){
	qlayers_ref& qlr = de_ref;
	MARK_USED(qlr);
	notekeeper& nkpr = de_nkpr;
	
	deduction_init(causes, max_lv);
	while(! is_end_of_dct()){
		BRAIN_CK(qlr.has_curr_quanton());
		find_next_source();
	}
	BRAIN_CK(qlr.has_curr_quanton());

	fill_dct(dct);
	
	// reset all
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_ql_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);
}

void
analyser::fill_dct(deduction& dct){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	brain& brn = get_de_brain();
	
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
	
	bool br_in_root = (brn.level() == ROOT_LEVEL);

	BRAIN_CK(br_in_root || (dct.dt_target_level < nkpr.dk_note_layer));
	BRAIN_CK(ck_motives(brn, dct.dt_motives));

	DBG_PRT(20, os << "find_dct of deduction=" << dct);
	
	if(br_in_root){
		BRAIN_CK(dct.dt_target_level == ROOT_LEVEL);
		BRAIN_CK(dct.dt_motives.is_empty());
		dct.dt_target_level = INVALID_LEVEL;
	}
	
}

bool
analyser::ck_end_of_lrn_nmp(){
#ifdef FULL_DEBUG
	BRAIN_CK(de_next_bk_psig.ps_source != NULL_PT);
	BRAIN_CK(! de_next_bk_psig.ps_source->ne_original);
	
	BRAIN_DBG(
		brain& brn = get_de_brain();
		qlayers_ref& qlr = de_ref;
		quanton* qua = de_next_bk_psig.ps_quanton;
	)
	
	BRAIN_CK(qua != NULL_PT);
	BRAIN_CK(qua == qlr.get_curr_quanton());
	
	BRAIN_DBG(
		long qti = qua->qu_tier;
		long num_quas = brn.br_charge_trail.get_qu_layer(qti).size();

		long q_lv = qua->qlevel();
		bool has_lnd = brn.get_data_level(q_lv).has_learned();
	)
	BRAIN_CK(brn.br_dbg_keeping_learned || (num_quas == 1));
	BRAIN_CK(brn.br_dbg_keeping_learned || has_lnd);
#endif	
	return true;
}

bool
brain::ck_cov_flags(){
#ifdef FULL_DEBUG
	BRAIN_CK(br_qu_tot_note2 == 0);
	BRAIN_CK(br_ne_tot_tag2 == 0);
	BRAIN_CK(br_qu_tot_note3 == 0);
	BRAIN_CK(br_ne_tot_tag3 == 0);
	BRAIN_CK(br_qu_tot_note4 == 0);
	BRAIN_CK(br_ne_tot_tag4 == 0);
#endif
	return true;
}

void
write_all_nmps(row<neuromap*>& to_wrt){
	for(int aa = 0; aa < to_wrt.size(); aa++){
		BRAIN_CK(to_wrt[aa] != NULL_PT);
		neuromap& nmp = *(to_wrt[aa]);
		BRAIN_CK(nmp.na_dbg_cand_sys);

		nmp.map_write();
		
		BRAIN_DBG(
			long& max_ns = nmp.get_brn().br_dbg.dbg_max_wrt_num_subnmp;
			if(nmp.na_num_submap > max_ns){
				max_ns = nmp.na_num_submap;
			}
		);
	}
}

row_quanton_t&
analyser::get_first_causes(){
	brain& brn = get_de_brain();
	
	BRAIN_CK(! de_all_confl.is_empty());
	prop_signal& cfl = de_all_confl.first();
	
	BRAIN_CK(cfl.ps_quanton != NULL);
	BRAIN_CK(cfl.ps_quanton->get_charge() == cg_negative);
	BRAIN_CK(cfl.ps_source != NULL);
	BRAIN_CK(cfl.ps_source->ne_original);
	
	brn.update_tk_write(cfl.ps_source->ne_in_write_tk);
	
	row_quanton_t& causes = cfl.ps_source->ne_fibres;
	BRAIN_CK(! causes.is_empty());
	return causes;
}

bool
brain::analyse_conflicts(row<prop_signal>& all_confl, deduction& out_dct){
	BRAIN_CK(! br_dbg_keeping_learned);
	bool found_top = false;
	BRAIN_CK(ck_cov_flags());
	BRAIN_CK(ck_trail());
	BRAIN_CK(! all_confl.is_empty());
	//BRAIN_CK(all_confl.first().ps_tier >= tier());
	
	analyser& dedser = br_deducer_anlsr;
	dedser.set_conflicts(all_confl);
	
	DBG_PRT(40, os << "bef_ana=" << bj_eol; print_trail(os);
		os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
		<< " br_ti=" << tier();
	);
	
	row<neuromap*>& to_wrt = br_tmp_maps_to_write;
	to_wrt.clear();
	
	dedser.deduction_analysis(dedser.get_first_causes(), out_dct);
	DBG_PRT(40, os << HTMi_h1 << "out_dct=" << HTMe_h1 << out_dct);
	dedser.find_all_to_write(to_wrt);

	BRAIN_DBG(deduction fst_dct; out_dct.copy_to_dct(fst_dct));
	BRAIN_CK(br_ne_tot_tag1 == 0);
	BRAIN_CK(br_qu_tot_note0 == 0);
	
	long tg_lv = out_dct.dt_target_level;
	
	deduction& fnd_dct = br_tmp_find_dct;
	fnd_dct.init_deduction();
	
	candidate_find_analysis(found_top, dedser, out_dct, fnd_dct, to_wrt);
	
	if(fnd_dct.dt_target_level != INVALID_LEVEL){
		BRAIN_CK(! fnd_dct.is_dt_virgin());
		
		tg_lv = fnd_dct.dt_target_level;
		
		out_dct.init_deduction();
		fnd_dct.move_to_dct(out_dct);
		BRAIN_CK(tg_lv == out_dct.dt_target_level);
	}
	
	//BRAIN_CK(! found_top || (out_dct.dt_target_level == INVALID_LEVEL));
	BRAIN_CK(br_ne_tot_tag1 == 0);
	
	DBG_PRT(102, os << "NUM_TO_WRT=" << to_wrt.size());
	write_all_nmps(to_wrt);
	
	BRAIN_CK(ck_cov_flags());
	BRAIN_DBG(bool do_finds = true);
	DBG_COMMAND(4, do_finds = false);
	IF_NEVER_FIND(do_finds = false);
	BRAIN_CK_PRT((do_finds || fst_dct.equal_to_dct(out_dct)),
			os << "________\n";
			os << "fst_dct=" << fst_dct;
			os << "out_dct=" << out_dct;
	);
	
	DBG_PRT(40, os << "AFT_ana=" << bj_eol; print_trail(os);
		os << out_dct << bj_eol
	);
	return (! found_top);
} // end_of_analyse

bool
deduction::equal_to_dct(deduction& dct2){
	bool c1 = dt_motives.equal_to(dct2.dt_motives);
	bool c2 = (dt_forced == dct2.dt_forced);
	bool c3 = (dt_target_level == dct2.dt_target_level);
	bool all_cc = (c1 && c2 && c3);
	return all_cc;
}

neuromap*
quanton::get_candidate_to_write(brain& brn){
	BRAIN_CK(is_pos());
	
	quanton& opp = opposite();
	if(opp.is_mono()){
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this; 
			os << " Mono=" << opp);
		return NULL_PT; 
	}
	
	if(! has_learned_source()){
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this;
			os << " not-has-learned_source. NULL to_wrt");
		return NULL_PT; 
	}
	
	neuromap* to_wrt = qu_candidate_nmp;
	
	if(to_wrt == NULL_PT){ 
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this;
			os << " NULL cand_nmp. NULL to_wrt");
		return NULL_PT; 
	}
	BRAIN_CK(to_wrt->na_candidate_qua == this);
	
	return to_wrt;
}

void
analyser::find_all_to_write(row<neuromap*>& to_wrt){
	brain& brn = get_de_brain();
	row<prop_signal>& all_noted = de_all_noted;
	
	DBG_PRT(102, os << "find-all_to_wrt\n";
		brn.dbg_prt_all_cands(os);
	);

	BRAIN_CK(brn.br_na_tot_na0 == 0);
	for(long ii = 0; ii < all_noted.size(); ii++){
		prop_signal& q_sig = all_noted[ii];
		q_sig.get_ps_cand_to_wrt(brn, to_wrt);
	}
	reset_all_na0(brn, to_wrt);
	BRAIN_CK(brn.br_na_tot_na0 == 0);
	
	BRAIN_DBG(qlayers_ref& qlr = de_ref);
	BRAIN_CK(last_qu_noted() == qlr.get_curr_quanton());
}

void
brain::candidate_find_analysis(bool& found_top, analyser& deducer, 
					deduction& prv_dct, deduction& nxt_dct, 
					row<neuromap*>& to_wrt)
{
	brain& brn = *this;
	long nxt_lv = prv_dct.dt_target_level;
	MARK_USED(nxt_lv);
	
	found_top = false;
	nxt_dct.init_deduction();
	
	row_quanton_t& nmp_causes = br_tmp_f_analysis;
	if(level() == ROOT_LEVEL){
		DBG_PRT(39, os << "find.NULL_NMP root level");
		BRAIN_CK(nxt_dct.dt_target_level == INVALID_LEVEL);
		return;
	}
	
	if(! to_wrt.is_empty()){
		DBG_PRT(39, os << "find.NULL_NMP !to_wrt.empty && h_s_lv+1");
		return;
	}
	
	BRAIN_DBG(bool was_e = to_wrt.is_empty());
	BRAIN_CK(br_qu_tot_note0 == 0);
	
	neuromap* out_nmp = NULL_PT;
	neuromap* last_found = NULL_PT;
	BRAIN_DBG(long num_fnd = 0);
	for(long aa = br_candidate_nmp_lvs.last_idx(); aa >= 0; aa--){
		out_nmp = br_candidate_nmp_lvs[aa];
		BRAIN_CK(out_nmp != NULL_PT);
		BRAIN_CK(out_nmp->nmp_is_cand());
		
		quanton& qua = *(out_nmp->na_candidate_qua);
		BRAIN_CK(qua.is_pos());
		
		DBG_PRT(39, os << " out_nmp=" << out_nmp << "\n";
			os << " qua= " << &qua;
			os << " qu_lv= " << qua.qlevel();
		);
		
		if(qua.has_learned_source()){
			continue;
		}
		
		long q_lv = qua.qlevel();
		if(q_lv > (nxt_lv + 1)){
			continue;
		}
		
		long min_sub = MIN_TRAINABLE_NUM_SUB;
		DBG_COMMAND(104, min_sub = brn.br_dbg_min_trainable_num_sub);
		BRAIN_CK(out_nmp->na_num_submap != INVALID_NUM_SUB);
		if(out_nmp->na_num_submap < min_sub){
			continue;
		}
		
		if(q_lv < nxt_lv){
			break;
		}
		
		BRAIN_DBG(num_fnd++);
		
		neuromap* to_find = out_nmp;
		if(to_find->is_na_mono()){
			to_find = to_find->na_nxt_no_mono;
			BRAIN_CK(to_find != NULL_PT);
		}
		if((to_find != last_found) && ! to_find->map_find()){
			BRAIN_CK(to_find != NULL_PT);
			DBG_PRT(39, os << "CANNOT find nmp=" << (void*)(out_nmp) << "\n";
				os << " nxt_dct=" << nxt_dct << "\n";
				os << " nxt_lv=" << nxt_lv << "\n";
				to_wrt.print_row_data(os, true, "\n");
			);
			DBG_PRT(70, 
				os << "candidate_find_analysis\n";
				os.flush();
				ch_string msg = HTMi_h1 "FindNOT" HTMe_h1;
				to_find->map_dbg_update_html_file(msg);
			);
			break;
		}
		if(! out_nmp->is_na_mono()){
			last_found = out_nmp;
		}
		
		BRAIN_DBG(
			long& max_ns = br_dbg.dbg_max_fnd_num_subnmp;
			if(out_nmp->na_num_submap > max_ns){
				max_ns = out_nmp->na_num_submap;
			}
		);

		BRAIN_CK(br_qu_tot_note0 == 0);
		
		DBG_PRT(39, os << " found_nmp=" << to_find << "\n";
			os << " out_nmp=" << out_nmp << "\n";
		);
		nmp_causes.clear();
		out_nmp->map_get_all_upper_quas(nmp_causes);
		
		BRAIN_DBG(
			row_quanton_t rr_aux;
			to_find->map_get_all_upper_quas(rr_aux);
			BRAIN_CK(same_quantons_note2(brn, rr_aux, nmp_causes));
		);
		
		if(nmp_causes.is_empty()){
			DBG_PRT(39, os << "found_top_1");
			found_top = true;
			break;
		}
		DBG_PRT(39, os << " upper_quas=" << nmp_causes);
	
		long max_lv = find_max_level(nmp_causes);
		if(max_lv == ROOT_LEVEL){
			DBG_PRT(39, os << "found_top_2");
			found_top = true;
			break;
		}
		
		if(nxt_lv <= 0){
			DBG_PRT(39, os << "find_stop_1. (nxt_lv <= 0)."; 
				os << " nxt_dct=" << nxt_dct << "\n";
				os << " nxt_lv=" << nxt_lv << "\n";
				os << " nmp=" << (void*)(out_nmp) << "\n";
			);
			break;
		}
		
		deduction tmp_dct;
		tmp_dct.init_deduction();
		deducer.deduction_analysis(nmp_causes, tmp_dct, max_lv);
 
		long old_lv = nxt_lv;
		if(tmp_dct.dt_target_level >= old_lv){
			BRAIN_CK_PRT(false, 
				DBG_PRT_ABORT(brn);
				print_trail(os);
				os << " nmp_causes=" << nmp_causes << "\n";
				os << " max_lv=" << max_lv << "\n";
				os << " nxt_lv=" << nxt_lv << "\n";
				os << " tg_lv=" << tmp_dct.dt_target_level << "\n";
				os << " nmp=" << out_nmp << "\n";
				os << " neu[8]=" << &(br_neurons[8]) << "\n";
			);
			DBG_PRT(39, os << "find_continue. (tgt_lv >= old_lv). nmp=" << (void*)(out_nmp));
			//continue;
			break;
		}
		
		deducer.find_all_to_write(to_wrt);

		DBG_PRT_COND(103, was_e && ! to_wrt.is_empty(), 
					 os << "\n\nTRAINING_DURING_FIND\n\n");
		
		nxt_dct.init_deduction();
		tmp_dct.move_to_dct(nxt_dct);
		nxt_lv = nxt_dct.dt_target_level;
		
		BRAIN_CK(nxt_lv < out_nmp->na_orig_lv);
		BRAIN_CK(nxt_lv < old_lv);
		BRAIN_REL_CK(nxt_lv < old_lv);

		if(! to_wrt.is_empty()){
			DBG_PRT(39, os << "find_stop_3. has_to_wrt && has_stup. nmp=" << out_nmp;
				os << "\n nxt_dct=" << nxt_dct;
			);
			break;
		}
	}
	
	//return out_nmp;
} // end_of_candidate_fnd_analysis

void
prop_signal::get_ps_cand_to_wrt(brain& brn, row<neuromap*>& to_wrt){
	BRAIN_CK(ps_quanton != NULL_PT);
	if(ps_quanton != NULL_PT){
		if(ps_quanton->is_qu_end_of_neuromap(brn)){
			neuromap* nmp = ps_quanton->get_candidate_to_write(brn);
			if(nmp != NULL_PT){
				BRAIN_CK(nmp->na_dbg_cand_sys);
				DBG_PRT(102, os << "ADDING_CAND_to_wrt ";
					os << "ps=" << this << "\n\n CAND_TO_WRT=\n" << nmp
				);
				long min_sub = MIN_TRAINABLE_NUM_SUB;
				DBG_COMMAND(104, min_sub = brn.br_dbg_min_trainable_num_sub);
				BRAIN_CK(nmp->na_num_submap != INVALID_NUM_SUB);
				if(nmp->na_num_submap >= min_sub){
					if(nmp->is_na_mono()){
						BRAIN_CK(nmp->has_submap());
						nmp = nmp->na_submap;
						BRAIN_CK(! nmp->is_na_mono());
					}
					to_wrt.push(nmp);
				}
			}
		}
	}
	if(ps_source != NULL_PT){
		brn.update_tk_write(ps_source->ne_in_write_tk);
	}
}

