

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

deducer.cpp  

deducer class methos and funcs.

--------------------------------------------------------------*/

#include "brain.h"
#include "solver.h"
#include "html_strings.h"

//============================================================
// static vars

//neuromap*	deducer::NULL_NEUROMAP = NULL_PT;

//============================================================
// aux funcs

void
deducer::reset_deduc(){
	de_all_noted.clear(true, true);
	
	de_next_bk_psig.init_prop_signal();
}

void
deducer::init_deducer(brain* brn){
	de_brain = brn;

	bool has_br = (brn != NULL_PT);
	if(has_br){
		de_ref.init_qlayers_ref(&(brn->br_charge_trail));
	}
	de_all_confl.clear(true, true);

	reset_deduc();
}

void
deducer::init_nk_with_note0(notekeeper& nkpr, brain& brn){
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
deducer::init_nk_with_note5(notekeeper& nkpr, brain& brn){
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
deducer::set_nxt_noted(quanton* pt_nxt_qua){
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
	
	
	de_next_bk_psig.init_qua_signal(nxt_qua);
	
	row<prop_signal>& all_noted = de_all_noted;
	prop_signal& nxt_sig = all_noted.inc_sz();
	nxt_sig = de_next_bk_psig;
	BRAIN_CK(ck_ord_last_tiers(get_de_brain(), all_noted));
	/*if((nxt_sig.ps_source != NULL_PT) && ! nxt_sig.ps_source->ne_original){
		BRAIN_CK(ck_end_of_lrn_nmp());
		nxt_sig.ps_source = NULL_PT;
	}*/

	nkpr.nk_reset_note(nxt_qua);
	BRAIN_CK(nxt_qua.qlevel() == nkpr.dk_note_layer);
	nkpr.dec_notes();
}

bool
deducer::find_next_noted(){
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
deducer::find_next_source()
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
deducer::set_notes_of(row_quanton_t& causes, bool is_first){
	notekeeper& nkpr = de_nkpr;
	BRAIN_CK(! causes.is_empty());
	BRAIN_CK(is_first || (causes[0]->get_charge() == cg_positive) );
	BRAIN_CK(! is_first || (causes[0]->get_charge() == cg_negative) );

	long from = (is_first)?(0):(1);
	long until = causes.size();
	nkpr.set_motive_notes(causes, from, until);
}

bool
deducer::ck_deduc_init(long deduc_lv, bool full_ck){
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
	
	BRAIN_CK_PRT((! full_ck || in_root || (nkpr.dk_note_layer > 0)),
		DBG_PRT_ABORT(brn);
		brn.print_trail(os);
		os << " br_lv=" << brn.level() << "\n";
		os << " deduc_lv=" << deduc_lv << "\n";
		os << " full_ck=" << full_ck << "\n";
		os << " in_root=" << in_root << "\n";
		os << " nt_lyr=" << nkpr.dk_note_layer << "\n";
	);
	BRAIN_CK(! full_ck || (deduc_lv != INVALID_LEVEL));
	BRAIN_CK(! full_ck || (nkpr.dk_note_layer == deduc_lv));
	BRAIN_CK(! full_ck || (qlr.has_curr_quanton()));
#endif
	return true;
}

void	
deducer::deduce_init(row_quanton_t& causes, long max_lv){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	
	BRAIN_CK(causes.size() > 0);

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
deducer::fill_rsn(reason& rsn){
	qlayers_ref& qlr = de_ref;
	notekeeper& nkpr = de_nkpr;
	brain& brn = get_de_brain();
	
	quanton* pt_qua = qlr.get_curr_quanton();
	BRAIN_CK(pt_qua != NULL_PT);
	
	quanton& qua = *pt_qua;
	quanton& opp = qua.opposite(); 

	BRAIN_CK(is_end_of_rsn());
	BRAIN_CK(opp.qlevel() == nkpr.dk_note_layer);
	BRAIN_CK(opp.get_charge() == cg_negative);

	rsn.init_reason();
	
	brn.update_tk_charge(rsn.dt_tk);
	
	rsn.rs_motives.set_cap(nkpr.dk_tot_noted + 1);

	nkpr.dk_quas_lyrs.get_all_ordered_quantons(rsn.rs_motives);

	rsn.rs_target_level = find_max_level(rsn.rs_motives);

	BRAIN_CK(! nkpr.nk_has_note(opp));
	rsn.rs_forced = &opp;
	
	bool br_in_root = (brn.level() == ROOT_LEVEL);

	BRAIN_CK(br_in_root || (rsn.rs_target_level < nkpr.dk_note_layer));
	BRAIN_CK(ck_motives(brn, rsn.rs_motives));

	DBG_PRT(20, os << "find_rsn of reason=" << rsn);
	
	if(br_in_root){
		BRAIN_CK(rsn.rs_target_level == ROOT_LEVEL);
		BRAIN_CK(rsn.rs_motives.is_empty());
		rsn.rs_target_level = INVALID_LEVEL;
	}
	
}

bool
deducer::ck_end_of_lrn_nmp(){
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
		
		BRAIN_CK(! nmp.is_na_virgin());
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

void
deducer::get_first_causes(row_quanton_t& fst_causes){
	brain& brn = get_de_brain();
	
	BRAIN_CK(! de_all_confl.is_empty());
	prop_signal& cfl = de_all_confl.first();
	
	BRAIN_CK(cfl.ps_quanton != NULL);
	BRAIN_CK(cfl.ps_quanton->get_charge() == cg_negative);
	BRAIN_CK(cfl.ps_source != NULL);
	BRAIN_CK(cfl.ps_source->ne_original);
	
	cfl.ps_source->update_ne_to_wrt_tk(brn, brn.br_curr_write_tk);
	
	row_quanton_t& causes = cfl.ps_source->ne_fibres;
	BRAIN_CK(! causes.is_empty());
	
	causes.copy_to(fst_causes);
}

bool
brain::analyse_conflicts(row<prop_signal>& all_confl, deduction& dct){
	dct.reset_deduction();
	
	reason& out_rsn = dct.dt_rsn;
	br_wrt_ref.reset_curr_quanton();
	write_get_tk(br_curr_write_tk);
	
	BRAIN_CK(! br_dbg_keeping_learned);
	bool found_top = false;
	BRAIN_CK(ck_cov_flags());
	BRAIN_CK(ck_trail());
	BRAIN_CK(! all_confl.is_empty());
	//BRAIN_CK(all_confl.first().ps_tier >= tier());
	
	deducer& ddc = br_dedcer;
	ddc.set_conflicts(all_confl);
	
	DBG_PRT(40, os << "bef_ana=" << bj_eol; print_trail(os);
		os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
		<< " br_ti=" << tier();
	);
	
	row_neuromap_t& to_wrt = dct.dt_all_to_wrt;
	to_wrt.clear();
	
	row_quanton_t& fst_causes = dct.dt_first_causes;
	ddc.get_first_causes(fst_causes);
	ddc.deduce(dct);

	DBG_PRT(40, os << HTMi_h1 << "out_rsn=" << HTMe_h1 << out_rsn);
	BRAIN_DBG(reason fst_rsn; out_rsn.copy_to_rsn(fst_rsn));
	BRAIN_CK(br_ne_tot_tag1 == 0);
	BRAIN_CK(br_qu_tot_note0 == 0);
	
	candidate_find_analysis(found_top, ddc, dct);
	
	BRAIN_CK(br_ne_tot_tag1 == 0);
	BRAIN_DBG(br_dbg_found_top = found_top);
	
	DBG_PRT(102, os << "NUM_TO_WRT=" << to_wrt.size());
	write_all_nmps(to_wrt);
	
	BRAIN_CK(ck_cov_flags());
	BRAIN_DBG(bool do_finds = true);
	DBG_COMMAND(4, do_finds = false); // NEVER_FIND
	BRAIN_CK_PRT((do_finds || fst_rsn.equal_to_rsn(out_rsn)),
			os << "________\n";
			os << "fst_rsn=" << fst_rsn;
			os << "out_rsn=" << out_rsn;
	);
	
	DBG_PRT(40, os << "AFT_ana=" << bj_eol; print_trail(os);
		os << out_rsn << bj_eol
	);
	return found_top;
} // end_of_analyse

bool
reason::equal_to_rsn(reason& rsn2){
	bool c1 = rs_motives.equal_to(rsn2.rs_motives);
	bool c2 = (rs_forced == rsn2.rs_forced);
	bool c3 = (rs_target_level == rsn2.rs_target_level);
	bool all_cc = (c1 && c2 && c3);
	return all_cc;
}

neuromap*
quanton::get_candidate_to_write(brain& brn){
	BRAIN_CK(is_pos());
	
	quanton& opp = opposite();
	if(opp.is_mono()){
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this; 
			os << " qua is Mono=" << opp);
		return NULL_PT; 
	}
	
	if(! has_learned_source()){
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this;
			os << " not-has-learned_source.");
		return NULL_PT; 
	}
	
	neuromap* to_wrt = qu_candidate_nmp;
	
	if(to_wrt == NULL_PT){ 
		DBG_PRT(55, os << "NOT_CAND_to_wrt qua=" << this;
			os << " NULL cand_nmp.");
		return NULL_PT; 
	}
	BRAIN_CK(to_wrt->na_candidate_qua == this);
	
	return to_wrt;
}

void
deducer::find_all_to_write(row<neuromap*>& to_wrt, row_long_t& all_wrt_idxs){
	brain& brn = get_de_brain();
	row<prop_signal>& all_noted = de_all_noted;
	
	BRAIN_CK(to_wrt.is_empty());
	all_wrt_idxs.clear();
	
	DBG_PRT(102, os << "find-all_to_wrt\n";
		brn.dbg_prt_all_cands(os);
	);

	BRAIN_DBG(
		ticket curr_wrt_tk;
		brn.write_get_tk(curr_wrt_tk);
	);
	
	BRAIN_CK(brn.br_na_tot_na0 == 0);
	for(long ii = 0; ii < all_noted.size(); ii++){
		prop_signal& q_sig = all_noted[ii];
		BRAIN_CK((q_sig.ps_source == NULL_PT) || (! q_sig.ps_source->ne_original) ||
			is_ticket_eq(q_sig.ps_source->ne_to_wrt_tk, curr_wrt_tk));
		bool added_nmp = q_sig.get_ps_cand_to_wrt(brn, to_wrt);
		if(added_nmp){
			all_wrt_idxs.push(ii);
		}
	}
	reset_all_na0(brn, to_wrt);
	BRAIN_CK(brn.br_na_tot_na0 == 0);
}

void
brain::candidate_find_analysis(bool& found_top, deducer& dedcer, deduction& dct)
{
	reason& nxt_rsn = dct.dt_rsn;
	row_neuromap_t& to_wrt = dct.dt_all_to_wrt;
	brain& brn = *this;
	BRAIN_DBG(
		bj_big_int_t& ncf = br_dbg_num_find_anal;
		ncf++;
		long dbg_num_cicl = 0;
	);

	long nxt_lv = nxt_rsn.rs_target_level;
	MARK_USED(nxt_lv);
	
	found_top = false;
	
	if(level() == ROOT_LEVEL){
		DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << ".find.NULL_NMP root level");
		return;
	}
	
	if(! to_wrt.is_empty()){
		DBG_PRT(39, os << "#f=" << ncf << ".find.NULL_NMP !to_wrt.empty && h_s_lv+1");
		return;
	}
	
	BRAIN_DBG(bool was_e = to_wrt.is_empty());
	BRAIN_CK(br_qu_tot_note0 == 0);
	
	neuromap* out_nmp = NULL_PT;
	neuromap* last_found = NULL_PT;
	BRAIN_DBG(long num_fnd = 0);
	for(long aa = br_candidate_nmp_lvs.last_idx(); aa >= 0; aa--){
		BRAIN_DBG(dbg_num_cicl++);
		out_nmp = br_candidate_nmp_lvs[aa];
		BRAIN_CK(out_nmp != NULL_PT);
		BRAIN_CK(out_nmp->nmp_is_cand());
		
		quanton& qua = *(out_nmp->na_candidate_qua);
		BRAIN_CK(qua.is_pos());
		
		long q_lv = qua.qlevel();
		
		DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl;
			os << ".out_nmp=" << out_nmp << "\n";
			os << " qua= " << &qua;
			os << " q_lv= " << q_lv;
			os << " nxt_lv= " << nxt_lv;
		);
		
		if(qua.has_learned_source()){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << " (qua.has_lrnd_src)");
			continue;
		}
		
		if(q_lv > (nxt_lv + 1)){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl;
				os << " (q_lv > (nxt_lv + 1))"
			);
			continue;
		}
		
		BRAIN_CK(out_nmp->na_num_submap != INVALID_NUM_SUB);
		long min_sub = get_min_trainable_num_sub();
		if(out_nmp->na_num_submap < min_sub){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl;
				os << " (out_nmp->na_num_submap < min_sub)";
				os << " min_sub=" << min_sub;
			);
			continue;
		}
		
		if(q_lv < nxt_lv){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << " (q_lv < nxt_lv)");
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
			DBG_PRT(39, os << "#f=" << ncf;
				os << ".CANNOT find na_idx=" << out_nmp->na_index << "\n";
				os << " nxt_rsn=" << nxt_rsn << "\n";
				os << " nxt_lv=" << nxt_lv << "\n";
				to_wrt.print_row_data(os, true, "\n");
			);
			DBG_PRT(70, 
				os << "#f=" << ncf << "." << dbg_num_cicl << ".find_analysis\n";
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
		
		DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl;
			os << ".found_nmp=" << to_find << "\n";
			os << " out_nmp=" << out_nmp << "\n";
		);
		row_quanton_t& nmp_causes = br_tmp_f_analysis;
		nmp_causes.clear();
		to_find->map_get_all_upper_quas(nmp_causes);
		
		dct.dt_last_found = to_find;
		
		if(nmp_causes.is_empty()){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << ".found_top_1");
			found_top = true;
			break;
		}
		DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl;
			os << ".upper_quas=" << nmp_causes
		);
		
		long max_lv = find_max_level(nmp_causes);
		BRAIN_CK(max_lv <= nxt_lv);
	
		if(max_lv == ROOT_LEVEL){
			reason nil_rsn;
			write_analysis(nmp_causes, nil_rsn);
			
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << ".found_top_2");
			found_top = true;
			brn.add_top_cands(to_wrt);
			break;
		}
		
		BRAIN_CK(nxt_lv > 0);
		BRAIN_REL_CK(nxt_lv > 0);
		
		long old_lv = nxt_lv;
		
		BRAIN_CK(to_wrt.is_empty());
		dct.reset_deduction();
		
		dct.dt_last_found = to_find;
		nmp_causes.copy_to(dct.dt_first_causes);
		dedcer.deduce(dct, max_lv);
		
		BRAIN_CK(nxt_rsn.rs_target_level != INVALID_LEVEL);
		BRAIN_CK(nxt_rsn.rs_target_level < old_lv);
		BRAIN_REL_CK(nxt_rsn.rs_target_level < old_lv);
		
		DBG_PRT_COND(103, was_e && ! to_wrt.is_empty(), 
					 os << "\n\nTRAINING_DURING_FIND\n\n");
		
		nxt_lv = nxt_rsn.rs_target_level;
		
		BRAIN_CK(nxt_lv < out_nmp->na_orig_lv);
		BRAIN_CK(nxt_lv < old_lv);
		BRAIN_REL_CK(nxt_lv < old_lv);

		if(! to_wrt.is_empty()){
			DBG_PRT(39, os << "#f=" << ncf << "." << dbg_num_cicl << ".find_stop_3.";
				os << " has_to_wrt && has_stup. nmp=" << out_nmp;
				os << "\n nxt_rsn=" << nxt_rsn;
			);
			break;
		}
	}
	
	//return out_nmp;
} // end_of_candidate_fnd_analysis

void
brain::add_top_cands(row_neuromap_t& to_wrt){
	row_neuromap_t& all_cand = br_candidate_nmp_lvs;
	for(long aa = 0; aa < all_cand.size(); aa++){
		neuromap* nmp = all_cand[aa];
		BRAIN_CK(nmp != NULL_PT);
		BRAIN_CK(nmp->nmp_is_cand());
		
		quanton& qua = *(nmp->na_candidate_qua);
		BRAIN_CK(qua.is_pos());
		
		long q_lv = qua.qlevel();
		if(q_lv != ROOT_LEVEL){
			break;
		}
		nmp->nmp_add_to_write(to_wrt);
	}
}

bool
prop_signal::get_ps_cand_to_wrt(brain& brn, row<neuromap*>& to_wrt){
	BRAIN_CK(ps_quanton != NULL_PT);
	bool added_nmp = false;
	if(ps_quanton != NULL_PT){
		if(ps_quanton->is_qu_end_of_neuromap(brn)){
			neuromap* nmp = ps_quanton->get_candidate_to_write(brn);
			if(nmp != NULL_PT){
				nmp->nmp_add_to_write(to_wrt);
				added_nmp = true;
			}
		}
	}
	return added_nmp;
}

void
neuromap::nmp_add_to_write(row_neuromap_t& to_wrt){
	brain& brn = get_brn();
	BRAIN_CK(na_dbg_cand_sys);
	DBG_PRT(102, os << "ADDING_CAND_to_wrt ";
		os << "ps=" << this << "\n\n CAND_TO_WRT=\n" << this
	);
	
	BRAIN_CK(na_num_submap != INVALID_NUM_SUB);
	long min_sub = brn.get_min_trainable_num_sub();
	if(na_num_submap >= min_sub){
		BRAIN_CK(! is_na_virgin());
		to_wrt.push(this);
	}
}

bool
deducer::is_de_end_of_neuromap(){
	brain& brn = get_de_brain();
	qlayers_ref& qlr = de_ref;
	quanton* nxt_qua = qlr.get_curr_quanton();
	BRAIN_CK(nxt_qua != NULL_PT);
	bool is_eonmp = nxt_qua->is_qu_end_of_neuromap(brn);
	return is_eonmp;
}

bool
quanton::is_qu_to_upd_wrt_tk(){
	bool is_e = is_ticket_eq(qu_upd_to_wrt_tk, qu_charge_tk);
	return is_e;
}

bool
neuron::is_ne_to_wrt(){
	BRAIN_CK(ck_wrt_qu0(false));
	bool is_e = is_ticket_eq(ne_upd_to_wrt_tk, ne_fibres[0]->qu_charge_tk);
	return is_e;
}

bool
neuron::ck_wrt_qu0(bool just_upd){ 
#ifdef FULL_DEBUG
	quanton* qu0 = ne_fibres[0];
	if(qu0 == NULL_PT){
		return false;
	}
	if(! qu0->has_charge()){
		return false;
	}
	if(! just_upd && ! is_ne_source()){
		return false;
	}
#endif
	return true;
}

void
quanton::update_qu_to_wrt_tk(){
	BRAIN_CK(qu_inverse != NULL_PT);
	qu_upd_to_wrt_tk = qu_charge_tk;
	qu_inverse->qu_upd_to_wrt_tk = qu_charge_tk;
}

void
neuron::update_ne_to_wrt_tk(brain& brn, ticket& wrt_tk){
	ne_to_wrt_tk = wrt_tk;
}

void
brain::write_get_tk(ticket& nw_tk){
	row_neuromap_t& cands = br_candidate_nmp_lvs;
	row_neuromap_t& nxt_cands = br_candidate_nxt_nmp_lvs;
	//BRAIN_CK(nxt_cands.is_empty());
	
	long prv_lv = 0;
	if(! nxt_cands.is_empty()){
		BRAIN_CK(nxt_cands.last() != NULL_PT);
		prv_lv = nxt_cands.last()->get_cand_lv();
	} else {
		if(! cands.is_empty()){
			BRAIN_CK(cands.last() != NULL_PT);
			prv_lv = cands.last()->get_cand_lv();
		}
	}
	
	nw_tk.tk_level = prv_lv;
	nw_tk.tk_recoil = recoil();
}

void
brain::write_update_all_tk(row_quanton_t& causes){
	for(long aa = 0; aa < causes.size(); aa++){
		BRAIN_CK(causes[aa] != NULL_PT);
		quanton& qua = *(causes[aa]);
		qua.update_qu_to_wrt_tk();
	}
}

void
quanton::update_source_wrt_tk(brain& brn){
	quanton* qua = this;
	BRAIN_CK(is_pos());
	if(is_qu_to_upd_wrt_tk()){
		neuron* neu = get_source();
		if((neu != NULL_PT) && ! neu->is_ne_to_wrt()){
			BRAIN_CK(neu->ne_fibres[0] == qua);
			BRAIN_CK(neu->ck_wrt_qu0(false));
			neu->ne_upd_to_wrt_tk = qu_charge_tk;
			BRAIN_CK(neu->is_ne_to_wrt());
			
			neu->update_ne_to_wrt_tk(brn, brn.br_curr_write_tk);
			brn.write_update_all_tk(neu->ne_fibres);
		}
	}
}

void
brain::write_analysis(row_quanton_t& causes, reason& rsn){
	brain& brn = *this;
	qlayers_ref& qlr = br_wrt_ref;
	long tg_lv = rsn.rs_target_level;
	
	write_update_all_tk(causes);
	
	quanton* nxt_qua = qlr.get_curr_quanton();
	while(nxt_qua != NULL_PT){
		BRAIN_CK(nxt_qua->is_pos());
		if(nxt_qua->qlevel() == tg_lv){
			break;
		}
		nxt_qua->update_source_wrt_tk(brn);
		nxt_qua = qlr.dec_curr_quanton();
	}
}

void
deducer::write_all_proof(row_quanton_t& causes, reason& rsn, row_long_t& all_wrt_idxs){
}

void
deducer::deduce(deduction& dct, long max_lv)
{
	row_quanton_t& causes = dct.dt_first_causes;
	reason& rsn = dct.dt_rsn;
	row<neuromap*>& to_wrt = dct.dt_all_to_wrt;
	
	BRAIN_CK(to_wrt.is_empty());
	
	qlayers_ref& qlr = de_ref;
	MARK_USED(qlr);
	notekeeper& nkpr = de_nkpr;
	
	deduce_init(causes, max_lv);
	while(! is_end_of_rsn()){
		BRAIN_CK(qlr.has_curr_quanton());
		find_next_source();
	}
	BRAIN_CK(qlr.has_curr_quanton());

	fill_rsn(rsn);

	// reset all
	nkpr.clear_all_quantons();
	BRAIN_CK(nkpr.dk_quas_lyrs.is_ql_empty());
	BRAIN_CK(nkpr.nk_get_counter() == 0);

	brain& brn = get_de_brain();
	brn.write_analysis(causes, rsn);
	find_all_to_write(to_wrt, dct.dt_all_to_wrt_idxs);
	
	de_all_noted.move_to(dct.dt_all_noted);
}

