

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

brain.cpp  

Classes and that implement the neural network.

--------------------------------------------------------------*/

#include "stack_trace.h"
#include "util_funcs.h"
#include "file_funcs.h"
#include "parse_funcs.h"
#include "sortor.h"
#include "brain.h"
#include "solver.h"
#include "dimacs.h"
#include "html_strings.h"
#include "proof.h"

#include "dbg_run_satex.h"
#include "dbg_prt.h"
#include "dbg_config.h"

DEFINE_GET_DBG_SLV(quanton)
DEFINE_GET_DBG_SLV(neuron)
DEFINE_GET_DBG_SLV(prop_signal)
DEFINE_GET_DBG_SLV(reason)
DEFINE_GET_DBG_SLV(deduction)
DEFINE_GET_DBG_SLV(coloring)
DEFINE_GET_DBG_SLV(neuromap)
DEFINE_GET_DBG_SLV(qulayers)
DEFINE_GET_DBG_SLV(notekeeper)
DEFINE_GET_DBG_SLV(deducer)
DEFINE_GET_DBG_SLV(leveldat)
DEFINE_GET_DBG_SLV(brain)


DEFINE_NI_FLAG_FUNCS(qu_flags, note0, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note1, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note2, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note3, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note4, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note5, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note6, false); // bina

DEFINE_NI_FLAG_ALL_FUNCS(note0);
DEFINE_NI_FLAG_ALL_FUNCS(note1);
DEFINE_NI_FLAG_ALL_FUNCS(note2);
DEFINE_NI_FLAG_ALL_FUNCS(note3);
DEFINE_NI_FLAG_ALL_FUNCS(note4);
DEFINE_NI_FLAG_ALL_FUNCS(note5);
DEFINE_NI_FLAG_ALL_FUNCS(note6);


DEFINE_NE_FLAG_FUNCS(ne_flags, tag0);
DEFINE_NE_FLAG_FUNCS(ne_flags, tag1);
DEFINE_NE_FLAG_FUNCS(ne_flags, tag2);
DEFINE_NE_FLAG_FUNCS(ne_flags, tag3);
DEFINE_NE_FLAG_FUNCS(ne_flags, tag4);
DEFINE_NE_FLAG_FUNCS(ne_flags, tag5);

DEFINE_NE_FLAG_ALL_FUNCS(tag0);
DEFINE_NE_FLAG_ALL_FUNCS(tag1);
DEFINE_NE_FLAG_ALL_FUNCS(tag2);
DEFINE_NE_FLAG_ALL_FUNCS(tag3);
DEFINE_NE_FLAG_ALL_FUNCS(tag4);
DEFINE_NE_FLAG_ALL_FUNCS(tag5);



DEFINE_PS_FLAG_ALL_FUNCS(0);
DEFINE_PS_FLAG_ALL_FUNCS(1);
DEFINE_PS_FLAG_ALL_FUNCS(2);
DEFINE_PS_FLAG_ALL_FUNCS(3);
DEFINE_PS_FLAG_ALL_FUNCS(4);
DEFINE_PS_FLAG_ALL_FUNCS(5);

DEFINE_NA_FLAG_FUNCS(na_flags, na0);
DEFINE_NA_FLAG_FUNCS(na_flags, na1);
DEFINE_NA_FLAG_FUNCS(na_flags, na2);
DEFINE_NA_FLAG_FUNCS(na_flags, na3);
DEFINE_NA_FLAG_FUNCS(na_flags, na4);
DEFINE_NA_FLAG_FUNCS(na_flags, na5);

DEFINE_NA_FLAG_ALL_FUNCS(na0);
DEFINE_NA_FLAG_ALL_FUNCS(na1);

#define PRINT_PERIOD			4.0
#define SOLVING_TIMEOUT			0.0		// 0.0 if no timeout

//============================================================
// static vars

char*	alert_rel::CL_NAME = as_pt_char("{alert_rel}");
char*	quanton::CL_NAME = as_pt_char("{quanton}");
char*	neuron::CL_NAME = as_pt_char("{neuron}");

brain* NULL_BRN_PT = NULL_PT;

//============================================================
// random generator

long gen_random_num(long min, long max);

// search dbg_ic to config it

//============================================================
// aux funcs

bool
has_neu(row_neuron_t& rr_neus, neuron* neu){
	for(long aa = 0; aa < rr_neus.size(); aa++){
		if(rr_neus[aa] == neu){
			return true;
		}
	}
	return false;
}

//============================================================
// neuron methods

quanton*
neuron::update_fibres(row_quanton_t& synps, bool orig){
	long num_neutral = 0;
	long num_neg_chgs = 0;
	
	neuron* neu = this;

	if(! ne_fibres.is_empty()){
		ck_tunnels();
		ne_fibres[0]->tunnel_swapop(ne_fibre_0_idx);
		ne_fibre_0_idx = INVALID_IDX;
		ne_fibres[1]->tunnel_swapop(ne_fibre_1_idx);
		ne_fibre_1_idx = INVALID_IDX;

		ne_fibres.clear();
	}

	if(synps.size() > 0){
		BRAIN_CK_0(synps.size() > 1);

		synps.copy_to(ne_fibres);
		ne_fibres.mix_sort(cmp_qlevel);
		//ne_fibres.mix_sort(cmp_qtier);
		//ne_fibres.mix_sort(cmp_chg_idx);

		BRAIN_CK(ne_fibres.is_sorted(cmp_qlevel));
		DBG_PRT(16, os << "update_syns " << ne_fibres);

		BRAIN_DBG(
			long id0 = INVALID_IDX;
			long id1 = INVALID_IDX;
		);
		BRAIN_CK(fib_sz() >= 2);

		for(long ii = 0; ii < fib_sz(); ii++){
			quanton* qua = ne_fibres[ii];
			BRAIN_DBG(
				id0 = id1;
				id1 = qua->abs_id();
				BRAIN_CK_0(id0 != id1);
			);

			charge_t q_chg = qua->get_charge();

			if(q_chg == cg_negative){
				num_neg_chgs++;
			} 
			if(q_chg == cg_neutral){
				num_neutral++;
			}

			if(orig){
				qua->qu_all_neus.push(neu);
			}
			BRAIN_DBG(
				brain* pt_brn = get_dbg_brn();
				BRAIN_CK(pt_brn != NULL_PT);
				if(! orig && pt_brn->br_dbg_keeping_learned){
					qua->qu_all_neus.push(neu);
				}
			);
		}
		BRAIN_CK_0(num_neg_chgs < fib_sz());

		ne_fibres[0]->qu_tunnels.push(this);
		ne_fibre_0_idx = ne_fibres[0]->qu_tunnels.size() - 1;
		ne_fibres[1]->qu_tunnels.push(this);
		ne_fibre_1_idx = ne_fibres[1]->qu_tunnels.size() - 1;
		ck_tunnels();
	}
	BRAIN_CK_0((fib_sz() > 0) || (num_neutral != 1));
	quanton* forced_qua = NULL;
	if(num_neg_chgs == (fib_sz() - 1)){
		// ABORT CASE
		if(ne_fibres[0]->is_neg()){
			bj_ostream& os = bj_dbg;
			brain* pt_brn = ne_fibres[0]->get_dbg_brn();
			if(pt_brn != NULL){
				pt_brn->print_trail(os);
			}
			os << this << bj_eol;;
			os << ne_fibres[0] << bj_eol;
			abort_func(0, "ABORT !. ne_fibres[0]->is_neg !!!");
		}
		BRAIN_CK_0(! ne_fibres[0]->is_neg());
		forced_qua = forced_quanton();
	}

	ne_edge_tk.init_ticket();

	return forced_qua;
}

void
brain::update_tk_trail(ticket& nw_tk){
	nw_tk.tk_level = tier();
	nw_tk.tk_recoil = recoil();
}

void
neuron::swap_fibres_1(long idx, long& max_lv_idx){
	BRAIN_CK(ck_tunnels());
	quanton* fb_1 = ne_fibres[1];
	long	idx_1 = ne_fibre_1_idx;

	ne_fibres[1] = ne_fibres[idx];
	ne_fibres[idx] = fb_1;

	ne_fibres[1]->qu_tunnels.push(this);
	ne_fibre_1_idx = ne_fibres[1]->qu_tunnels.size() - 1;

	fb_1->tunnel_swapop(idx_1);
	BRAIN_CK(ck_tunnels());

	if(max_lv_idx == idx){
		max_lv_idx = 1;
	} else
	if(max_lv_idx == 1){
		max_lv_idx = idx;
	}
}

void
neuron::swap_fibres_0(long idx, long& max_lv_idx){
	BRAIN_CK(ck_tunnels());
	quanton* fb_0 = ne_fibres[0];
	long	idx_0 = ne_fibre_0_idx;

	ne_fibres[0] = ne_fibres[idx];
	ne_fibres[idx] = fb_0;

	ne_fibres[0]->qu_tunnels.push(this);
	ne_fibre_0_idx = ne_fibres[0]->qu_tunnels.size() - 1;

	fb_0->tunnel_swapop(idx_0);
	BRAIN_CK(ck_tunnels());

	if(max_lv_idx == idx){
		max_lv_idx = 0;
	} else
	if(max_lv_idx == 0){
		max_lv_idx = idx;
	}
}

void
neuron::neu_swap_edge(brain& brn, long ii, long& max_lv_idx){
	charge_t qua_chg = ne_fibres[1]->get_charge();

	swap_fibres_1(ii, max_lv_idx);

	if(qua_chg == cg_negative){
		ne_edge = ii;
		brn.update_tk_trail(ne_edge_tk);
		BRAIN_CK(ne_fibres[ii]->is_neg());
	}
}

long
neuron::get_min_ti_idx(long fb_idx1, long fb_idx2){
	BRAIN_CK(ne_fibres.is_valid_idx(fb_idx2));
	if(fb_idx1 == INVALID_IDX){
		return fb_idx2;
	}
	BRAIN_CK(ne_fibres.is_valid_idx(fb_idx1));
	long ti1 = ne_fibres[fb_idx1]->qu_tier;
	long ti2 = ne_fibres[fb_idx2]->qu_tier;
	if(ti2 < ti1){
		return fb_idx2;
	}
	return fb_idx1;
}

long
neuron::get_max_lv_idx(long fb_idx1, long fb_idx2){
	BRAIN_CK(ne_fibres.is_valid_idx(fb_idx2));
	if(fb_idx1 == INVALID_IDX){
		return fb_idx2;
	}
	//BRAIN_CK(ne_fibres[fb_idx1]->has_charge());
	//BRAIN_CK(ne_fibres[fb_idx2]->has_charge());
	BRAIN_CK(ne_fibres.is_valid_idx(fb_idx1));
	
	long lv1 = ne_fibres[fb_idx1]->qlevel();
	long lv2 = ne_fibres[fb_idx2]->qlevel();
	if(lv2 > lv1){
		return fb_idx2;
	}
	return fb_idx1;
}

void
neuron::update_create_cand(brain& brn, quanton& r_qua, 
						   neuromap& creat_cand, bool dbg_been_forced)
{
	bool in_prop = (ne_fibres[0]->qu_source == this);
	neuromap* curr_nmp = r_qua.get_candidate_to_fill(brn);
	
	BRAIN_CK(curr_nmp != NULL_PT);
	BRAIN_CK(! in_prop || (curr_nmp == &creat_cand));
	
	BRAIN_DBG(long op1 = 0; MARK_USED(op1););
	
	if(curr_nmp == &creat_cand){
		if(! in_prop){
			BRAIN_DBG(op1 = 1);
			creat_cand.na_all_cov.push(this);
		} 
		/*BRAIN_DBG(
			if(in_prop) {
				prop_signal& nxt_ps = creat_cand.na_all_propag.inc_sz();
				nxt_ps.init_qua_signal(fib0());
			}
		);*/
	} else {
		BRAIN_DBG(op1 = 2);
		cov_entry& cty = curr_nmp->na_all_centry.inc_sz();
		cty.init_cov_entry(&creat_cand, this);
	}
}

quanton*
neuron::dbg_find_first_pos(){
	quanton* f_pos = NULL_PT;
#ifdef FULL_DEBUG
	for(long ii = 0; ii < fib_sz(); ii++){
		quanton* qua = ne_fibres[ii];
		BRAIN_CK(qua != NULL_PT);
		BRAIN_CK(qua->qu_tier != INVALID_TIER);
		if(qua->is_pos()){
			bool is_fst = ((f_pos == NULL_PT) || (qua->qu_tier < f_pos->qu_tier));
			if(is_fst){ 
				f_pos = qua; 
			}
		}
	}
#endif
	return f_pos;
}


//============================================================
// brain methods

brain::brain(solver& ss){
	init_brain(ss);
}

brain::~brain(){
	DBG_PRT(37, os << "releasing brain 0");

	release_brain();

	DBG_PRT(37, os << "RELEASING brain 1");
}

bool
brain::dbg_as_release(){
	return get_solver().slv_prms.sp_as_release;
}

solver&
brain::get_solver(){
	BRAIN_CK(br_pt_slvr != NULL);
	return *br_pt_slvr;
}

skeleton_glb&
brain::get_skeleton(){
	BRAIN_CK(br_pt_slvr != NULL);
	return br_pt_slvr->slv_skl;
}

instance_info&
brain::get_my_inst(){
	BRAIN_CK(br_pt_slvr != NULL_PT);
	return br_pt_slvr->slv_inst;
}

bj_output_t&
brain::get_out_info(){
	bj_output_t& o_inf = get_my_inst().get_out_info();
	return o_inf;
}

void
brain::init_brain(solver& ss){
	brain& brn = *this;
	brain* pt_brn = this;
	
	BRAIN_DBG(
		br_pt_brn = NULL_PT;
		br_dbg_num_phi_grps = INVALID_NATURAL;
		br_dbg_phi_wrt_ids.clear();
		br_dbg_keeping_learned = false;
		br_dbg_min_trainable_num_sub = 0;
		br_dbg_num_find_anal = 0;
		br_dbg_abort_nmp = NULL_PT;
		br_dbg_watched_nmp_idx = INVALID_IDX;
		br_dbg_watched_nmp_tk.init_ticket();
		br_dbg_is_watched_file = false;
		br_dbg_skl_bug = false;
		br_dbg_found_top = false;
		br_dbg_in_analysis = false;
	);
	br_pt_slvr = &ss;
	if(ss.slv_prms.sp_write_proofs){
		ss.slv_skl.reset_proof_path();
	}
	
	DBG_COMMAND(145, get_skeleton().kg_dbg_all_wrt_paths.clear());
	
	init_all_tots();
	
	br_prt_timer.init_timer(PRINT_PERIOD, SOLVING_TIMEOUT);

	br_start_load_tm = 0.0;

	// debug attributes

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = INVALID_LEVEL);

	// temp attributes

	// config attributes
	br_file_name = "<file_name>";
	br_file_name_in_ic = "<file_name_in_ic>";

	// state attributes
	br_prv_round_last_rc = INVALID_RECOIL;
	br_curr_choice_tk.init_ticket();
	br_round = INVALID_ROUND;
	br_curr_write_tk.init_ticket();
	
	br_choice_spin = cg_neutral;
	br_choice_order = k_invalid_order;
	
	br_last_retract = false;
	
	br_last_monocho_idx = 0;
	
	//br_choices_lim = INVALID_IDX;
	br_charge_trail.init_qulayers(pt_brn);

	br_first_psignal = 0;
	br_last_psignal = 0;

	br_dbg_retract_nke0.init_notekeeper(pt_brn);
	br_dbg_retract_nke0.init_funcs(&br_qu_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0,
								&append_all_not_note0, &same_quantons_note0
  							);
	
	br_dbg_full_col.init_coloring();

	deducer& ddc = br_dedcer;
	ddc.init_deducer(pt_brn);
	ddc.init_nk_with_note0(ddc.de_nkpr, brn);
	
	br_pulse_deduc.init_deduction(pt_brn);
	
	br_wrt_ref.init_qlayers_ref(&br_charge_trail);
	
	reset_conflict();

	//br_num_memo = 0;

	br_top_block.init_quanton(pt_brn, cg_neutral, INVALID_IDX, NULL);
	
	DBG_CK(br_top_block.qu_tee.is_alone());

	br_tot_cy_sigs = 0;
	br_tot_cy_nmps = 0;

	br_num_active_neurons = 0;
	br_num_active_alert_rels = 0;
	br_num_active_neuromaps = 0;
	
	br_candidate_next = NULL_PT;
	br_candidate_nxt_rc = 0;
	br_candidate_rsn_lv = INVALID_LEVEL;

	BRAIN_DBG(
		init_all_dbg_brn();  // sets br_pt_brn indicating it is readi for DBG_PRT
		//dbg_init_dbg_conf(get_solver().slv_dbg_conf_info);
		get_solver().slv_dbg_brn = pt_brn;
	);
	BRAIN_CK(br_dbg.dbg_tot_nmps == 0);
}

void
brain::release_all_sortors(){
	br_forced_srg.release_all();
	br_filled_srg.release_all();

	br_guide_neus_srg.release_all();
	br_guide_quas_srg.release_all();

	//br_compl_neus_srg.release_all();
	//br_compl_quas_srg.release_all();
	
	br_tauto_neus_srg.release_all();
	br_tauto_quas_srg.release_all();
}

void
brain::release_brain(){
	br_last_retract = true;
	
	BRAIN_DBG(br_pt_brn = NULL);
	BRAIN_DBG(get_solver().slv_dbg_brn = NULL_PT);
	get_skeleton().set_dbg_brn(NULL);

	bool has_chgs = (br_charge_trail.get_tot_quantons() > 0);
	if(has_chgs){
		retract_to(INVALID_LEVEL, false);
	}
	BRAIN_CK(br_charge_trail.get_tot_quantons() == 0);
	if(level() != ROOT_LEVEL){
		retract_to(ROOT_LEVEL, false);
	}
	while(! br_data_levels.is_empty()){
		dec_level();
	}
	BRAIN_CK(br_data_levels.is_empty());

	// reset neurons
	br_neurons.clear(true);

	// temporal attributes
	br_tmp_fixing_quantons.clear(false, true);
	br_tmp_load_quantons.clear(false, true);

	br_tmp_motives.clear(false, true);

	// state attributes
	br_positons.clear(true, true);	

	br_negatons.clear(true, true);	
	br_choices.clear(false, true);
	br_chosen.clear(false, true);

	br_neurons.clear(false, true);	// already reseted
	br_psignals.clear(true, true);
	br_delayed_psignals.clear(true, true);

	br_neuromaps.clear(true, true);
	
	release_all_sortors();

	get_skeleton().clear_all();

	br_top_block.init_quanton(this, cg_neutral, INVALID_IDX, NULL);

	BRAIN_CK(br_dbg.dbg_tot_nmps == 0);
	BRAIN_CK(br_num_active_neuromaps == 0);
}

void
quanton::qua_tunnel_signals(brain& brn){
	BRAIN_CK(get_charge() != cg_neutral);
	BRAIN_CK(qlevel() != INVALID_LEVEL);
	
	for(long ii = qu_tunnels.size() - 1; (ii >= 0); ii--){
		BRAIN_CK(qu_tunnels[ii] != NULL);
		neuron& neu = *(qu_tunnels[ii]);
		neu.neu_tunnel_signals(brn, *this);
	} // end for (ii)
	BRAIN_CK_2(ck_all_tunnels());
}

void
brain::update_tk_charge(ticket& nw_tk){
	nw_tk.tk_level = br_curr_choice_tk.tk_level;
	nw_tk.tk_recoil = br_curr_choice_tk.tk_recoil;
}

void		
quanton::set_charge(brain& brn, neuron* neu, charge_t cha, long n_tier){
	BRAIN_CK(ck_charge(brn));
	BRAIN_CK(this != &(brn.br_top_block));
	
	BRAIN_CK((cha == cg_positive) || (cha == cg_neutral));
	BRAIN_CK(! is_neg());
	BRAIN_CK(! is_pos() || (cha == cg_neutral));
	BRAIN_CK(has_charge() || (cha == cg_positive));
	
	if(cha == cg_neutral){
		brn.reset_chg_cands_update(*this);
	}

	BRAIN_DBG(bool is_ending = brn.br_last_retract);
	bool with_src_before = has_source();

	qu_charge = cha;
	qu_inverse->qu_charge = negate_trinary(cha);

	reset_cicle_src();
	
	set_source(neu);

	bool with_src_after = has_source();

	if(! has_charge()){
		DBG_PRT(15, os << "unbind " << *this);

		BRAIN_CK(neu == NULL);
		quanton* qua_trl = brn.br_charge_trail.pop_motive();

		MARK_USED(qua_trl);
		BRAIN_CK(qua_trl == this);
		BRAIN_CK(is_ending || (qlevel() != ROOT_LEVEL));

		if(! with_src_before && (qlevel() != ROOT_LEVEL)){
			quanton* qua_cho = brn.br_chosen.pop();
			MARK_USED(qua_cho);
			BRAIN_CK(qua_cho == this);
		}
		
		qu_charge_tk.init_ticket();
		qu_inverse->qu_charge_tk.init_ticket();

		BRAIN_CK(qu_tier != INVALID_TIER);
		BRAIN_CK(qu_inverse->qu_tier != INVALID_TIER);
		qu_tier = INVALID_TIER;
		qu_inverse->qu_tier = INVALID_TIER;

		BRAIN_DBG(
			avg_stat& nfll = brn.get_solver().slv_dbg2.dbg_avg_num_filled;
			nfll.add_val(qu_dbg_num_fill_by_qua);
			nfll.add_val(qu_inverse->qu_dbg_num_fill_by_qua);
			qu_dbg_num_fill_by_qua = 0;
			qu_inverse->qu_dbg_num_fill_by_qua = 0;
		);

	} else {
		BRAIN_CK(qu_dbg_num_fill_by_qua == 0);
		BRAIN_CK(qu_inverse->qu_dbg_num_fill_by_qua == 0);

		quanton& mot = *this;
		brn.br_charge_trail.add_motive(mot, n_tier);

		brn.update_tk_charge(qu_charge_tk);
		brn.update_tk_charge(qu_inverse->qu_charge_tk);

		BRAIN_CK(n_tier != INVALID_TIER);
		BRAIN_CK(qu_tier == INVALID_TIER);
		BRAIN_CK(qu_inverse->qu_tier == INVALID_TIER);
		qu_tier = n_tier;
		qu_inverse->qu_tier = n_tier;

		if(! with_src_after && (qlevel() != ROOT_LEVEL)){
			BRAIN_CK(neu == NULL_PT);
			brn.br_chosen.push(this);
		}

		DBG_PRT(15, os << "BIND " << this; 
			if(neu != NULL){ os << " src: "; os << neu; }
		);

	}
	BRAIN_CK(ck_charge(brn));
	
	if(cha != cg_neutral){
		brn.set_chg_cands_update(*this);
	}
}

long
brain::brn_tunnel_signals(bool only_orig, row_quanton_t& all_impl_cho){
	brain& brn = *this;
	MARK_USED(brn);
	BRAIN_CK(br_ne_tot_tag0 == 0);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK(! found_conflict());
	BRAIN_CK(ck_trail());
	BRAIN_DBG(
		ticket tk1;
		brn.update_tk_charge(tk1);
		BRAIN_CK(! found_conflict());
	);
	
	set_all_note0(brn, all_impl_cho);

	long num_psigs = 0;

	quanton* cho = curr_cho();

	//BRAIN_DBG(quanton* l_trl = br_charge_trail.last_quanton());
	
	while(has_psignals()){
		quanton* qua = receive_psignal(only_orig);
		if(qua == NULL_PT){
			BRAIN_CK((curr_cho() == NULL_PT) || curr_cho()->has_charge());
			continue;
		}

		quanton* inv = qua->qu_inverse;
		BRAIN_CK_0(inv != NULL_PT);

		BRAIN_CK(qua->is_pos());
		BRAIN_CK(inv->is_neg());

		qua->qua_tunnel_signals(brn);
		inv->qua_tunnel_signals(brn);

		DBG_PRT(19, os << "finished tunnelling " << inv);
	
		num_psigs++;
		
		if((cho != NULL_PT) && cho->is_opp_mono()){
			//BRAIN_CK(cho == qua);
			send_next_mono();
		}
	} // end while

	reset_all_note0(brn, all_impl_cho);
	
	BRAIN_CK(! has_psignals());
	reset_psignals();
	
	BRAIN_CK(br_qu_tot_note0 == 0);
	
	// reset tag0
	
	BRAIN_CK(br_ne_tot_tag0 == br_all_conflicts_found.size());
	row_neuron_t& all_cfl = br_tmp_all_confl;
	all_cfl.clear();
	append_all_trace_neus(br_all_conflicts_found, all_cfl);

	BRAIN_CK(br_ne_tot_tag0 == all_cfl.size());
	reset_all_tag0(brn, all_cfl);
	BRAIN_CK(br_ne_tot_tag0 == 0);

	BRAIN_DBG(
		ticket tk2;
		brn.update_tk_charge(tk2);
		BRAIN_CK_0(is_tk_equal(tk1, tk2));
	);
	
	update_monos();
	BRAIN_CK(ck_mono_propag());

	return num_psigs;
}

quanton*	
brain::choose_quanton(){ 
	BRAIN_CK(ck_trail());

	quanton* qua = choose_mono();
	if(qua != NULL_PT){
		return qua;
	}

	long max_idx = br_candidate_nmp_lvs.last_idx();
	for(long aa = max_idx; aa >= 0; aa--){
		neuromap* nmp = br_candidate_nmp_lvs[aa];
		BRAIN_CK(nmp != NULL_PT);
		if(! nmp->na_is_head){
			continue;
		}
		qua = nmp->map_choose_quanton();
		if(qua != NULL_PT){
			BRAIN_DBG(br_dbg.dbg_num_loc_cho++);
			return qua;
		}
	}

	for(long ii = 0; ii < br_choices.size(); ii++){
		quanton* qua_ii = br_choices[ii];
		if(qua_ii->get_charge() == cg_neutral){
			//BRAIN_CK(! qua_ii->has_mono());
			qua = qua_ii;
			break;
		}
	}

	BRAIN_CK_0((qua == NULL) || (qua->get_charge() == cg_neutral));
	BRAIN_CK_0(	(qua == NULL) || 
			(qua->qu_spin == cg_positive) || 
			(qua->qu_spin == cg_negative));

	if((qua != NULL) && (br_choice_spin == cg_negative)){
		qua = qua->qu_inverse;
	}

	BRAIN_DBG(br_dbg.dbg_num_glb_cho++);
	DBG_PRT(105, os << "GLB_CHO qua=" << qua << "\n";
		print_trail(os);
	);
	return qua;
}

void
brain::init_loading(long num_qua, long num_neu){
	brain& brn = *this;
	skeleton_glb& skg = get_skeleton();
	skg.start_local();

	br_neurons.set_cap(2 * num_neu);
	br_neurons.clear(true); 

	br_unit_neurons.clear();
	br_learned_unit_neurons.clear();

	br_choices.clear();

	br_choices.set_cap(num_qua);
	br_choices.fill(NULL, num_qua);

	// reset quantons

	br_top_block.qu_inverse = &(br_top_block);
	BRAIN_CK(! br_top_block.has_source());

	br_positons.set_cap(num_qua);
	br_positons.clear(true);

	br_negatons.set_cap(num_qua);
	br_negatons.clear(true);
	for(long ii = 0; ii < num_qua; ii++){
		br_positons.inc_sz();
		br_negatons.inc_sz();

		quanton* qua_pos = &(br_positons.last());
		quanton* qua_neg = &(br_negatons.last());

		qua_pos->init_quanton(this, cg_positive, ii, qua_neg);
		qua_neg->init_quanton(this, cg_negative, ii, qua_pos);

		long idx = ii;
		if(br_choice_order == k_left_order){
			idx = num_qua - ii - 1;
		}

		BRAIN_CK_0(idx >= 0);
		BRAIN_CK_0(idx < num_qua);
		br_choices[idx] = qua_pos; 
		
		BRAIN_DBG(
			qua_pos->qu_dbg_choice_idx = idx;
			qua_neg->qu_dbg_choice_idx = idx;
		);
		
		BRAIN_DBG(br_all_cy_quas.push(qua_pos));
	}
	BRAIN_CK_0(br_positons.size() == num_qua);
	BRAIN_CK_0(br_negatons.size() == num_qua);
	BRAIN_CK_0(br_choices.size() == num_qua);

	br_tmp_fixing_quantons.set_cap(num_qua);
	br_tmp_motives.set_cap(num_qua);

	//br_choices_lim = 0;

	br_prv_round_last_rc = INVALID_RECOIL;
	br_curr_choice_tk.tk_recoil = 1;
	br_curr_choice_tk.tk_level = 0;
	br_round = 0;

	BRAIN_CK(br_data_levels.is_empty());
	inc_data_levels();

	brn.update_tk_charge(br_top_block.qu_charge_tk);

	BRAIN_CK(br_tot_cy_sigs == 0);
	BRAIN_CK(br_tot_cy_nmps == 0);

	init_all_tots();

	BRAIN_DBG(
		br_dbg.dbg_all_chosen.clear();
	)
}

neuron*
brain::learn_mots(reason& rsn){
	BRAIN_CK(rsn.rs_forced != NULL_PT);
	
	row_quanton_t& the_mots = rsn.rs_motives;
	quanton& forced_qua = *rsn.rs_forced;
	
	DBG_PRT(23, os << "**LEARNING** mots=" << the_mots << " forced=" << &forced_qua;
		if(the_mots.is_empty()){ os << " IS_SINGLETON"; } 
		os << "\n";
		print_trail(os);
	);
	
	neuron* the_neu = NULL_PT;

	BRAIN_CK(! the_mots.is_empty() || (level() == ROOT_LEVEL));

	if(! the_mots.is_empty()){
		//BRAIN_CK(ck_motives(*this, the_mots));
		//BRAIN_CK(the_mots.last() != &forced_qua);
		the_mots.push(&forced_qua);

		quanton* f_qua = NULL_PT;
		neuron& added_neu = add_neuron(the_mots, f_qua, false);
		the_neu = &added_neu;
		
		if(! rsn.rs_tk.is_tk_virgin()){
			added_neu.ne_proof_tk = rsn.rs_tk;
		}
		
		get_leveldat().ld_learned.push(the_neu);
	}

	DBG_PRT(23, os << "added_forced quanton: " << &forced_qua;
		if(the_neu == NULL_PT){ os << " IS_SINGLETON"; } 
	);

	if(the_mots.is_empty()){
		forced_qua.qu_proof_tk = rsn.rs_tk;
		
		BRAIN_CK(! forced_qua.has_charge());
		
		//long nxt_tir = 0;
		long nxt_tir = tier() + 1;
		BRAIN_CK(level() == ROOT_LEVEL);
		BRAIN_CK(the_neu == NULL_PT);
		send_psignal(forced_qua, the_neu, nxt_tir);
	
		neuron& uni_neu = locate_neuron();
		//the_neu = &uni_neu;
		
		uni_neu.ne_original = false;
		if(! rsn.rs_tk.is_tk_virgin()){
			uni_neu.ne_proof_tk = rsn.rs_tk;
		}
		
		uni_neu.ne_fibres.push(&forced_qua);
		br_learned_unit_neurons.push(&uni_neu);
	}

	//	if(br_dbg.dbg_ic_active && br_dbg.dbg_ic_after){
	return the_neu;
}

neuron&
brain::add_neuron(row_quanton_t& quans, quanton*& forced_qua, bool orig){
	brain& brn = *this;
	MARK_USED(brn);
	
	neuron& neu = locate_neuron();
	neu.ne_original = orig;

	forced_qua = neu.update_fibres(quans, orig);

	BRAIN_CK(neu.fib_sz() > 0);
	BRAIN_DBG(
		if(orig){
			avg_stat& nsz = get_solver().slv_dbg2.dbg_avg_neu_sz;
			nsz.add_val(neu.fib_sz());
		}
	);

	DBG_PRT(26, os << "adding " << neu);
	return neu;
}

void
brain::set_file_name_in_ic(ch_string f_nam){
	if(f_nam.empty()){
		return;
	}
	ch_string nam = br_file_name;

	long nm_sz = nam.size();
	for(long ii = 0; ii < nm_sz; ii++){
		if(nam[ii] == '\\'){
			nam[ii] = '|';
		}
	}
	br_file_name_in_ic = nam;
}

void
brain::config_brain(ch_string f_nam){
	if(! f_nam.empty()){
		br_file_name = f_nam;
	}
	set_file_name_in_ic(f_nam);

	//dbg_reset_ic_files(*this);
}

void
brain::read_cnf(dimacs_loader& ldr){
	instance_info& inst_info = get_my_inst();
	ch_string& f_nam = inst_info.get_f_nam();
	
	if(inst_info.is_parsed()){
		ldr.ld_file_name = f_nam;
		return;
	}
	
	if(inst_info.is_read()){
		ldr.ld_file_name = f_nam;
		
		row<char>& dat = inst_info.ist_data;
		dat.swap_with(ldr.ld_content);
		return;
	}
	
	ldr.load_file(f_nam);
}

void
brain::parse_cnf(dimacs_loader& ldr, row<long>& all_ccls){
	instance_info& inst_info = get_my_inst();

	if(inst_info.is_parsed()){
		inst_info.ist_ccls.swap_with(all_ccls);
		return;
	}
	
	ldr.parse_content(all_ccls);
	
	inst_info.ist_num_ccls = ldr.ld_num_ccls;
	inst_info.ist_num_vars = ldr.ld_num_vars;
	
	bj_output_t& o_info = get_out_info();
	o_info.bjo_num_ccls = ldr.ld_num_ccls;
	o_info.bjo_num_vars = ldr.ld_num_vars;
	o_info.bjo_num_lits = ldr.ld_tot_lits;
}

bool
brain::load_instance(){
	br_start_load_tm = run_time();
	
	dimacs_loader	the_loader(this);
	row<long> all_ccls;
	
	read_cnf(the_loader);
	parse_cnf(the_loader, all_ccls);

	// brain loading

	instance_info& inst_info = get_my_inst();
	long num_neu = inst_info.ist_num_ccls;
	long num_var = inst_info.ist_num_vars;
	
	bool all_ok = load_brain(num_neu, num_var, all_ccls);
	return all_ok;
}

void
brain::set_result(bj_satisf_val_t re){
	bj_satisf_val_t& the_result = get_out_info().bjo_result;

	BRAIN_CK(re != bjr_unknown_satisf);
	BRAIN_CK((the_result != bjr_yes_satisf) || (re != bjr_no_satisf));
	BRAIN_CK((the_result != bjr_no_satisf) || (re != bjr_yes_satisf));

	the_result = re;

	DBG_PRT(27, os << "RESULT " << as_ist_satisf_str(the_result));
	DBG_PRT(28, os << "HIT ENTER TO CONTINUE...");
	DBG_COMMAND(28, getchar());
}

bj_satisf_val_t
brain::get_result(){
	bj_satisf_val_t the_result = get_out_info().bjo_result;

	return the_result;
}

neuron&
brain::load_neuron(row_quanton_t& neu_quas){
	DBG_PRT(22, os << "ADDING NEU=" << neu_quas);

	neuron* the_neu = NULL_PT;
	BRAIN_CK_0(neu_quas.size() > 0);
	if(neu_quas.size() > 1){
		bool is_orig = true;
		quanton* forced_qua = NULL;
		neuron& a_neu = add_neuron(neu_quas, forced_qua, is_orig);
		the_neu = &a_neu;
		BRAIN_CK_0(forced_qua == NULL);
	} else {
		BRAIN_CK_0(neu_quas.size() == 1);
		BRAIN_CK_0(level() == ROOT_LEVEL);

		neuron& neu = locate_neuron();
		the_neu = &neu;
		
		neu.ne_original = false;
		neu.ne_fibres.push(neu_quas.first());
		br_unit_neurons.push(&neu);

		send_psignal(*(neu_quas.first()), NULL, 0);
	}
	neu_quas.clear();
	BRAIN_CK(the_neu != NULL_PT);
	return *the_neu;
}

quanton*
brain::get_quanton(long q_id){
	quanton* qua = NULL;
	BRAIN_CK(q_id != 0);
	if(q_id < 0){ 
		long idx_ne = -q_id - 1;
		if(! br_negatons.is_valid_idx(idx_ne)){
			return NULL;
		}
		qua = &(br_negatons[idx_ne]);
	} else {
		BRAIN_CK(q_id > 0);
		long idx_po = q_id - 1;
		if(! br_positons.is_valid_idx(idx_po)){
			return NULL;
		}
		qua =  &(br_positons[idx_po]);
	}
	BRAIN_CK(qua->qu_id == q_id);
	return qua;
}

bool
brain::load_brain(long num_neu, long num_var, row_long_t& load_ccls){
	instance_info& inst_info = get_my_inst();

	init_loading(num_var, num_neu);

	BRAIN_CK(load_ccls.last() == 0);
	BRAIN_CK(num_var > 0);

	br_neurons.set_cap(num_neu);

	long num_neu_added = 0;
	long num_den_added = 0;

	long max_neu_sz = 0;

	//long ii = 0;
	//long first = 0;
	
	row_quanton_t& neu_quas = br_tmp_load_quantons;
	neu_quas.clear();
	
	for(long ii = 0; ii < load_ccls.size(); ii++){
		long nio_id = load_ccls[ii];
		
		if(nio_id != 0){
			quanton* qua = get_quanton(nio_id);
			if(qua == NULL){ return false; }
			neu_quas.push(qua);
		} else {
			num_neu_added++;
			long num_dens = neu_quas.size();

			BRAIN_CK(num_dens > 0);
			num_den_added += num_dens;

			if(num_dens > max_neu_sz){
				max_neu_sz = num_dens;
			}

			neuron& b_neu = load_neuron(neu_quas);
			MARK_USED(b_neu);
			if(get_out_info().bjo_result != bjr_unknown_satisf){ 
				break; 
			}
			BRAIN_DBG(br_all_cy_neus.push(&b_neu));

			//first = ii + 1;
			
			neu_quas.clear();
		}
	}

	//BRAIN_CK(net.nt_neurons.size() == num_neu_added);

	init_alert_neus();
	
	double end_load_tm = run_time();
	double ld_tm = (end_load_tm - br_start_load_tm);	
	get_out_info().bjo_load_time = ld_tm;

	ch_string f_nam = inst_info.get_f_nam();
	return true;
}

void
brain::fill_with_origs(row_neuron_t& neus){
	k_row<neuron>& all_neus = br_neurons;

	neus.clear();
	
	for(long aa = 0; aa < all_neus.size(); aa++){
		neuron& neu = all_neus[aa];
		if(neu.ne_original){
			BRAIN_CK(neu.fib_sz() >= 2);
			neus.push(&neu);
		}
	}
}

void 
brain::think(){
	DBG_PRT(147, dbg_print_all_qua_rels(os));
	
	instance_info& inst_info = get_my_inst();
	bj_output_t& o_info = get_out_info();
	
	get_skeleton().kg_instance_file_nam = inst_info.get_f_nam() + "\n";
	
	if(o_info.bjo_result != bjr_unknown_satisf){ 
		return;
	}

	BRAIN_DBG(
		bool just_read = false; // ONLY_READ
		DBG_COMMAND(6, just_read = true);
		DBG_COMMAND(13, just_read = true);
		if(just_read){
			set_result(bjr_error);
			DBG_PRT(13, os << "____\nFULL_BRAIN_STAB=\n"; dbg_prt_full_stab());
			return;
		} 
	)

	br_start_solve_tm = run_time();

	ch_string f_nam = inst_info.get_f_nam();

	config_brain(f_nam.c_str());
	br_choice_spin = cg_negative;
	br_choice_order = k_right_order;

	while(o_info.bjo_result == bjr_unknown_satisf){
		pulsate();
	}
	
	br_tmp_assig_quantons.clear();

	if(inst_info.ist_with_assig){
		row_quanton_t& the_assig = br_tmp_assig_quantons;
		br_charge_trail.get_all_ordered_quantons(the_assig);
		get_ids_of(the_assig, inst_info.ist_assig);
		inst_info.ist_assig.push(0);	// means the last lit
	}
	
	BRAIN_DBG(
		bj_satisf_val_t resp_solv = get_result();
	
		DBG_PRT(34, dbg_prt_all_cho(*this));
		DBG_PRT(29, os << "BRAIN=" << bj_eol;
			print_brain(os); 
		);

		br_final_msg << f_nam << " ";

		resp_solv = o_info.bjo_result;
		if(resp_solv == bjr_yes_satisf){
			dbg_check_sat_assig();
			br_final_msg << "IS_SAT_INSTANCE";
		} else if(resp_solv == bjr_no_satisf){
			br_final_msg << "IS_UNS_INSTANCE";
		}

		bj_out << br_final_msg.str() << bj_eol; 
		bj_out.flush();

		//if(br_dbg.dbg_ic_active){
	);

	pop_all_canditates();
	//close_all_maps();

	if(level() != ROOT_LEVEL){
		retract_to(ROOT_LEVEL, false);
	}

	br_psignals.clear(true, true);
	br_delayed_psignals.clear(true, true);

	release_all_sortors();

	all_mutual_init();

	DBG_PRT(142, os << "dbg_max_lv=" << br_dbg.dbg_max_lv ;
		os << " dbg_max_wrt_num_subnmp=" << br_dbg.dbg_max_wrt_num_subnmp;
		os << " dbg_max_fnd_num_subnmp=" << br_dbg.dbg_max_fnd_num_subnmp;
	);
	
	BRAIN_DBG(get_solver().update_dbg2(br_dbg));
}

#ifdef FULL_DEBUG
void
solver::update_dbg2(dbg_inst_info& ist_info){
	if(ist_info.dbg_max_lv > slv_dbg2.dbg_max_lv){
		slv_dbg2.dbg_max_lv = ist_info.dbg_max_lv;
	}
	if(ist_info.dbg_max_wrt_num_subnmp > slv_dbg2.dbg_max_wrt_num_subnmp){
		slv_dbg2.dbg_max_wrt_num_subnmp = ist_info.dbg_max_wrt_num_subnmp;
	}
	if(ist_info.dbg_max_fnd_num_subnmp > slv_dbg2.dbg_max_fnd_num_subnmp){
		slv_dbg2.dbg_max_fnd_num_subnmp = ist_info.dbg_max_fnd_num_subnmp;
	}
}
#endif

void
notekeeper::set_motive_notes(row_quanton_t& rr_qua, long from, long until){
	BRAIN_CK(ck_funcs());
	BRAIN_CK(dk_tot_noted >= 0);
	BRAIN_CK(dk_num_noted_in_layer >= 0);

	brain& brn = get_dk_brain();

	DBG_PRT(33, os << "making notes " << rr_qua);
	//DBG_PRT(115, os << "making notes " << rr_qua);

	if(from < 0){ from = 0; }
	if(until > rr_qua.size()){ until = rr_qua.size(); }
	
	bool br_in_root = brn.in_root_lv();

	long ii = from;
	for(; ii < until; ii++){
		BRAIN_CK(rr_qua[ii] != NULL_PT);
		quanton& qua = *(rr_qua[ii]);
		BRAIN_CK(qua.get_charge() == cg_negative);

		bool qu_in_root = (qua.qlevel() == ROOT_LEVEL);
		
		bool to_note = (! qu_in_root || br_in_root);
		to_note = true;
		
		bool has_note = nk_has_note(qua);
		if(! has_note && to_note){
			dk_tot_noted++;
			//(qua.*dk_set_note_fn)(brn);
			nk_set_note(qua);

			BRAIN_CK(dk_note_layer != INVALID_LEVEL);
			BRAIN_DBG(ch_string dbg_msg = "");
			long q_lv = qua.qlevel();

			dk_quas_lyrs.add_motive(qua, q_lv);

			BRAIN_CK(q_lv <= dk_note_layer);
			if(q_lv == dk_note_layer){
				BRAIN_DBG(dbg_msg = "making note ");
				dk_num_noted_in_layer++;
			} 
			BRAIN_DBG(
				if(q_lv != dk_note_layer){
					BRAIN_CK(q_lv < dk_note_layer);
					dbg_msg = "motive ";
				}
			);
			DBG_PRT(20, os << dbg_msg << " qua=" << &qua
					<< " #not_lv " << dk_num_noted_in_layer 
					<< " lv=" << dk_note_layer
					<< " motives_by_lv=\n";
					dk_quas_lyrs.ql_quas_by_layer.print_row_data(os, true, "\n");
			);
			
		}
	}
}

// levels

void
leveldat::release_learned(brain& brn){
	BRAIN_DBG(if(brn.br_dbg_keeping_learned){ return; });
	for(long aa = 0; aa < ld_learned.size(); aa++){
		BRAIN_CK(ld_learned[aa] != NULL_PT);
		neuron& neu = *(ld_learned[aa]);
		BRAIN_CK(! neu.ne_original);
		
		brn.release_neuron(neu);
	}
}

long
notekeeper::clear_all_quantons(long lim_lv, bool reset_notes){
	BRAIN_CK(ck_funcs());

	brain& brn = get_dk_brain();
	row_row_quanton_t& all_qu_lyrs = dk_quas_lyrs.ql_quas_by_layer;
	
	if(lim_lv < 0){
		lim_lv = all_qu_lyrs.size();
	}
	BRAIN_CK(lim_lv <= all_qu_lyrs.size());
	long tot_reset = 0;
	for(long aa = 0; aa < lim_lv; aa++){
		BRAIN_CK(all_qu_lyrs.is_valid_idx(aa));
		row_quanton_t& mots = all_qu_lyrs[aa];

		if(reset_notes){
			long num_re = (*dk_reset_all_fn)(brn, mots);
			dk_tot_noted -= num_re;
			tot_reset += num_re;
		}
		mots.clear();
	}
	if(reset_notes){
		dk_num_noted_in_layer = 0;
	}
	BRAIN_DBG(
		for(long bb = lim_lv; bb < all_qu_lyrs.size(); bb++){
			BRAIN_CK(all_qu_lyrs.is_valid_idx(bb));
			row_quanton_t& mots = all_qu_lyrs[bb];
			BRAIN_CK(mots.is_empty());
		}
	);

	BRAIN_CK(dk_tot_noted == 0);
	BRAIN_CK(dk_num_noted_in_layer == 0);
	return tot_reset;
}

void
brain::dec_level(){
	leveldat& lv = get_leveldat();
	BRAIN_CK(lv.ld_idx == level());
	
	brain& brn = *this;
	lv.reset_monos(brn);
	lv.release_learned(brn);
	//lv.let_maps_go(brn);

	br_curr_choice_tk.tk_level--;
	leveldat* pt_lv = br_data_levels.pop();
	leveldat::release_leveldat(pt_lv);
}

void
brain::retract_to(long tg_lv, bool full_reco)
{
	brain& brn = *this;
	
	write_get_tk(br_curr_write_tk);
	
	DBG_PRT(25, os << "RETRACT_\n"; print_trail(os););
	
	while(level() >= 0){
		bool end_of_lev = (trail_level() != level());
		if(end_of_lev){
			BRAIN_CK((trail_level() + 1) == level());
			BRAIN_CK(level() != ROOT_LEVEL);
			if(full_reco && ((trail_level() + 1) == tg_lv)){
				//BRAIN_CK(false);
				break;
			}
		
			dec_level();
		}

		if(! full_reco && (level() == tg_lv)){
			break;
		}
		if(! br_charge_trail.has_motives()){
			break;
		}
		
		BRAIN_CK_PRT(br_charge_trail.has_motives(),
			os << recoil() << ".lv=" << level() << " tg_lv=" << tg_lv;
			os << " trail_lv=" << trail_level() << bj_eol;
			print_trail(os)
		);
		//BRAIN_CK(level() != ROOT_LEVEL);

		quanton& qua = trail_last();
		BRAIN_CK(qua.is_pos());
		
		qua.update_source_wrt_tk(brn);
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}
	inc_recoil();
	
	BRAIN_DBG(
		dbg_update_config_entries(get_solver().slv_dbg_conf_info, recoil());
	);
	
	BRAIN_CK((tg_lv == INVALID_LEVEL) || (level() == tg_lv));
	BRAIN_CK(full_reco || (tg_lv == INVALID_LEVEL) || (trail_level() == tg_lv));
	BRAIN_CK(! full_reco || (tg_lv == INVALID_LEVEL) || ((trail_level() + 1) == tg_lv));
}

void
brain::put_psignal(quanton& qua, neuron* src, long max_tier){
	BRAIN_CK(br_first_psignal <= br_last_psignal);
	if(br_psignals.is_empty()){
		br_psignals.inc_sz();
	}
	br_last_psignal++;
	if(br_last_psignal == br_psignals.size()){
		br_psignals.inc_sz();
	}
	BRAIN_CK(br_psignals.is_valid_idx(br_last_psignal));
	
	prop_signal& sgnl = br_psignals[br_last_psignal];
	sgnl.init_prop_signal(&qua, src, max_tier);
}
	
quanton*
brain::receive_psignal(bool only_orig){
	BRAIN_CK(has_psignals());
	brain& brn = *this;
	prop_signal& sgnl = pick_psignal();

	BRAIN_CK(sgnl.ps_quanton != NULL_PT);
	quanton* pt_qua = sgnl.ps_quanton;

	quanton& qua = *(pt_qua);
	quanton& opp = qua.opposite();
	neuron* neu = sgnl.ps_source;
	long sg_tier = sgnl.ps_tier;
	BRAIN_CK(sg_tier != INVALID_TIER);
	
	DBG_PRT(21, os << sgnl << " oorig=" << only_orig; os << " HIT ENTER TO CONTINUE...");
	DBG_COMMAND(21, getchar());
	
	if(! qua.has_charge() && opp.is_note0()){
		return NULL_PT;
	}
	
	qua.update_cicle_srcs(brn, neu);

	if(qua.has_charge()){
		BRAIN_REL_CK(neu != NULL_PT);
		BRAIN_CK(neu != NULL_PT);
		if(qua.is_smaller_source(*neu, sg_tier)){
			qua.update_source(brn, *neu);
		}
		
		if(qua.is_neg()){
			BRAIN_CK(brn.br_dbg_keeping_learned || neu->ne_original);
			
			long cnfl_ti = tier() + 1;
			BRAIN_CK(sg_tier <= cnfl_ti);
			
			if(neu->ne_original && ! neu->has_tag0()){
				neu->set_tag0(brn);
				prop_signal& nxt_cfl = br_all_conflicts_found.inc_sz();
				nxt_cfl.init_prop_signal(pt_qua, neu, cnfl_ti);
				DBG_PRT(18, os << "**confict** " << nxt_cfl;
					os << "\n sg_tier=" << sg_tier << " cnfl_ti=" << cnfl_ti);
			}
			
			BRAIN_CK(brn.br_dbg_keeping_learned || found_conflict());
			DBG_PRT_COND(35, (br_all_conflicts_found.size() > 1), 
					os << "num_confl=" << br_all_conflicts_found.size()
			);
		} 
		pt_qua = NULL_PT;
	} else {
		BRAIN_CK_PRT((! only_orig || (sg_tier == tier()) || (sg_tier == (tier() + 1))),
				os << "____________________\n";
				brn.dbg_prt_margin(os);
				os << " only_orig=" << only_orig << "\n";
				os << " ti=" << tier() << "\n";
				os << " sg_tier=" << sg_tier << "\n";
				os << " neu=" << neu << "\n";
				os << " blv=" << brn.level() << "\n";
				os << " sgnl=" << sgnl << "\n";
				os << " qua=" << &qua << "\n";
				brn.print_trail(os);
		);

		qua.set_charge(brn, neu, cg_positive, sg_tier);

		DBG_PRT_COND(30, (neu != NULL_PT), os << "qua=" << &qua << " SRC=" << neu);

		BRAIN_CK((qua.qu_source == neu) || 
			((level() == ROOT_LEVEL) && (qua.qu_source == NULL_PT)));
		BRAIN_CK((qua.qu_tier == sg_tier) || 
			((level() == ROOT_LEVEL) && (qua.qu_tier == 0)));
	}
	
	DBG_PRT(21, os << "init sgnl" << sgnl);
	sgnl.init_prop_signal();

	return pt_qua;
}

bool
brain::ck_mono_propag(){
#ifdef FULL_DEBUG
	if(level() == ROOT_LEVEL){
		return true;
	}
	
	if(! get_leveldat().is_ld_mono()){
		return true;
	}
	
	quanton* l_qua = br_charge_trail.last_quanton();
	
	BRAIN_CK(l_qua != NULL_PT);
	BRAIN_CK(l_qua->is_opp_mono());
	//BRAIN_CK(get_leveldat().ld_chosen == l_qua);
#endif
	return true;
}

long
brain::propagate_signals(){
	if(found_conflict() || ! has_psignals()){
		return 0;
	}

	long num_psigs1 = 0;
	BRAIN_CK(br_delayed_psignals.is_empty());
	quanton* old_cho = NULL_PT;
	quanton* nw_cho = NULL_PT;
	BRAIN_DBG(
		brain& brn = *this;
		quanton* dbg_cho1 = NULL_PT;
		quanton* dbg_cho2 = NULL_PT;
		quanton* dbg_cho3 = NULL_PT;
		long dbg_num_cicl = 0;
	);
	
	row_quanton_t& all_impl_cho = br_tmp_all_impl_cho;
	all_impl_cho.clear();

	while(true){
		BRAIN_DBG(dbg_num_cicl++);
		BRAIN_CK((dbg_cho1 == NULL_PT) == (dbg_cho2 == NULL_PT));
		BRAIN_CK((dbg_cho2 == NULL_PT) == (dbg_cho3 == NULL_PT));
		
		num_psigs1 = brn_tunnel_signals(true, all_impl_cho);
		
		BRAIN_CK_PRT(((dbg_cho1 == NULL_PT) || dbg_cho1->is_pos()),
				DBG_PRT_ABORT(brn);
				print_trail(os);
				os << " CHOSEN=" << brn.br_chosen << "\n";
				os << " dbg_cho1=" << dbg_cho1 << "\n";
				os << " dbg_cho2=" << dbg_cho2 << "\n";
				os << " dbg_cho3=" << dbg_cho3 << "\n";
				os << " dbg_num_cicl=" << dbg_num_cicl << "\n";
				os << " ne_210=" << &(br_neurons[210]) << "\n";
				os << " ne_66=" << &(br_neurons[66]) << "\n";
		);
		BRAIN_CK((dbg_cho2 == NULL_PT) || dbg_cho2->is_pos());
		BRAIN_CK((dbg_cho3 == NULL_PT) || dbg_cho3->is_pos());
		BRAIN_DBG(
			dbg_cho1 = NULL_PT;
			dbg_cho2 = NULL_PT;
			dbg_cho3 = NULL_PT;
		);
		
		BRAIN_CK(br_qu_tot_note1 == 0);
		nw_cho = get_cicles_common_cho(old_cho, all_impl_cho);
		BRAIN_CK(br_qu_tot_note1 == 0);
		//nw_cho = NULL_PT;	// debug purposes
		if(nw_cho == NULL_PT){
			break;
		}
		if(nw_cho != NULL_PT){
			get_last_lv_charges(br_tmp_cicles_lv_quas);
			br_tmp_cicles_lv_quas.append_to(all_impl_cho);
			
			BRAIN_CK(! nw_cho->is_opp_mono());
			BRAIN_CK(! curr_choice().is_opp_mono());
			BRAIN_DBG(
				dbg_cho1 = get_leveldat().ld_chosen;
				dbg_cho2 = old_cho;
				dbg_cho3 = nw_cho;
			);
			BRAIN_CK(dbg_cho1 != NULL_PT);
			BRAIN_CK(dbg_cho2 != NULL_PT);
			BRAIN_CK(dbg_cho3 != NULL_PT);
			
			BRAIN_CK(dbg_cho1->is_pos());
			BRAIN_CK(dbg_cho2->is_pos());
			BRAIN_CK(dbg_cho3->is_pos());
			
			BRAIN_CK(old_cho != NULL_PT);
			DBG_PRT(69, 
				os << "\n FOUND_REPLACE_CHO\n";
				print_trail(os);
				os << " CHOSEN=\n" << brn.br_chosen << "\n";
				os << " all_impl_cho=\n" << all_impl_cho << "\n";
				os << " dbg_cho1=" << dbg_cho1 << "\n";
				os << " dbg_cho2=" << dbg_cho2 << "\n";
				os << " dbg_cho3=" << dbg_cho3 << "\n";
				os << " nw_cho=" << nw_cho << "\n";
				os << " old_cho=" << old_cho << "\n"
			);
			BRAIN_CK(old_cho->qlevel() < level());
			BRAIN_REL_CK(old_cho->qlevel() < level());
			
			replace_choice(*old_cho, *nw_cho, dbg_call_4);
			/*if(found_conflict()){
				reset_conflict();
			}*/
			//all_impl_cho.clear();
			//break;
		} 
	}
	
	all_impl_cho.clear();

	BRAIN_CK(br_delayed_psignals.is_empty());
	long num_psigs2 = 0;
	if(! found_conflict() && ! br_delayed_psignals.is_empty()){
		BRAIN_CK(false);
		BRAIN_CK(! has_psignals());
		BRAIN_CK(has_reset_psignals());
		send_row_psignals(br_delayed_psignals);
		br_delayed_psignals.clear(true, true);

		num_psigs2 = brn_tunnel_signals(false, all_impl_cho);

		BRAIN_CK(br_delayed_psignals.is_empty());
	}

	br_delayed_psignals.clear(true, true);

	BRAIN_CK(! has_psignals());
	BRAIN_CK(num_psignals() == 0);

	//update_monos();
	//BRAIN_CK(ck_mono_propag());
	
	long tot_psigs = num_psigs1 + num_psigs2;
	return tot_psigs;
}

void
brain::get_last_lv_charges(row_quanton_t& all_lv_pos){
	long lv = level();
	
	all_lv_pos.clear();
	
	qlayers_ref qtrl;
	qtrl.init_qlayers_ref(&br_charge_trail);
	qtrl.reset_curr_quanton();

	while(qtrl.get_curr_qlevel() == lv){
		quanton* qua = qtrl.get_curr_quanton();
		BRAIN_CK(qua != NULL_PT);
		
		all_lv_pos.push(qua);
		
		qtrl.dec_curr_quanton();
	}
}

void
brain::pulsate(){
	propagate_signals();

	BRAIN_CK(ck_trail());

	if(found_conflict()){
		DBG(
			ch_string htm_str = "";
			ch_string rc_str = recoil().get_str();
			htm_str += "BRN_recoil=" + rc_str;
		);
		DBG_PRT(72, os << "bef_htm=" << bj_eol; print_trail(os);
			os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
			<< " br_ti=" << tier();
		);
		DBG_COMMAND(72, dbg_update_html_cy_graph(CY_IC_KIND, NULL_PT, htm_str));
	
		br_last_retract = (level() == ROOT_LEVEL);
		
		//dbg_old_reverse_trail(); // change br_dbg.dbg_old_deduc to use.
		
		BRAIN_DBG(bool should_stop = in_root_lv());
		bool go_on = deduce_and_reverse_trail();
		BRAIN_CK(! should_stop || ! go_on);
		if(! go_on){
			set_result(bjr_no_satisf);
			return;
		}
		BRAIN_CK(has_psignals());

	} else {
		BRAIN_CK(! found_conflict());

		brain& brn = *this;
		MARK_USED(brn);

		quanton* pt_qua = NULL;
		pt_qua = choose_quanton();
		if(pt_qua == NULL){
			set_result(bjr_yes_satisf);
			return;
		}

		quanton& qua = *pt_qua;
		
		start_propagation(qua);

		quanton& cho = curr_choice();
		BRAIN_CK_PRT(
			((&cho == &qua) || (&(qua.opposite()) == &cho) || ! cho.has_charge() || 
				(cho.is_choice() && cho.is_pos() && qua.has_charge() && 
					(qua.qu_tier > cho.qu_tier)
				)
			), 
			os << "_______________\n ABORT_DATA \n";
			os << " TRAIL=\n";
			print_trail(os);
			os << " qua=" << &qua << "\n";
			os << " cho=" << &cho << "\n";
		);
		
		if(! cho.qu_has_been_cho){ 
			cho.qu_has_been_cho = true; 
			BRAIN_DBG(
				br_dbg.dbg_all_chosen.push(&cho);
			)
		}
		DBG_PRT(25, os << "**CHOICE** " << &cho << "\n";
			print_trail(os);
		);
	}
}

bool 
neuron::has_qua(quanton& tg_qua){
	for (long ii = 0; ii < fib_sz(); ii++){
		quanton* qua = ne_fibres[ii];
		if(qua == &tg_qua){
			return true;
		}
	}
	return false;
}

void
brain::replace_choice(quanton& cho, quanton& nw_cho, dbg_call_id dbg_id){
	BRAIN_CK(! has_psignals());
	brain& brn = *this;

	BRAIN_CK_0(level() != ROOT_LEVEL);

	DBG_PRT(14, print_trail(os); os << "chosen " << br_chosen);

	long tgt_lvl = cho.qlevel();
	
	// retract loop

	DBG_PRT(101, os << " REPL_CHO. POP_ALL_NXT_CANDS. "; 
		os << " tg_tv=" << tgt_lvl << "\n";
		print_trail(os);
		dbg_prt_all_nxt_cands(os, true);
	);
	pop_all_nxt_cand_lvs(tgt_lvl);
	retract_to(tgt_lvl, true);

	BRAIN_CK((level() == ROOT_LEVEL) || ((trail_level() + 1) == tgt_lvl));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == tgt_lvl));

	// resend opp chosen

	get_leveldat().reset_monos(brn);

	DBG_PRT(25, os << "**NEW_CHO=" << &nw_cho << "\n";
		print_trail(os);
		os << "dbg_call=" << dbg_id << "\n";
	);

	get_leveldat().ld_chosen = &nw_cho;

	BRAIN_CK(! has_psignals());
	send_psignal(nw_cho, NULL, tier() + 1);
	BRAIN_CK(has_psignals());

	DBG_PRT(101, os << " AFTER_REPL_CHO."; dbg_prt_all_nxt_cands(os, true);
		print_trail(os);
	);
	reset_conflict();
}

void 
print_ex(top_exception& ex1){
	BRAIN_DBG(
		bj_err << "got top_exception.\n" << ex1.ex_stk << bj_eol;
		abort_func(0);
	)
}

void
set_dimacs_err(dimacs_exception& dim_ex, bj_output_t& o_inf){
	o_inf.bjo_err_char = dim_ex.val;
	o_inf.bjo_err_line = dim_ex.line;
	o_inf.bjo_err_pos = dim_ex.pt_pos;
	
	o_inf.bjo_err_num_decl_cls = dim_ex.num_decl_cls;
	o_inf.bjo_err_num_decl_vars = dim_ex.num_decl_vars;
	o_inf.bjo_err_num_read_cls = dim_ex.num_read_cls;
	o_inf.bjo_err_bad_lit = dim_ex.bad_lit;
	
	switch(dim_ex.ex_id){
	case dix_no_cnf_decl_1:
	case dix_no_cnf_decl_2:
	case dix_no_cnf_decl_3:
		o_inf.bjo_error = bje_dimacs_no_cnf_declaration;
		break;
	case dix_bad_num_cls:
		o_inf.bjo_error = bje_dimacs_bad_cls_num;
		break;
	case dix_bad_format:
		o_inf.bjo_error = bje_dimacs_format_err;
		break;
	case dix_zero_vars:
		o_inf.bjo_error = bje_dimacs_zero_vars;
		break;
	case dix_zero_cls:
		o_inf.bjo_error = bje_dimacs_zero_clauses;
		break;
	case dix_bad_lit:
		o_inf.bjo_error = bje_dimacs_bad_literal;
		break;
	case dix_cls_too_long:
		o_inf.bjo_error = bje_dimacs_clause_too_long;
		break;
	};
}

void
set_file_err(file_exception& fl_ex, bj_output_t& o_inf){
	switch(fl_ex.ex_id){
	case flx_cannot_open:
		o_inf.bjo_error = bje_file_cannot_open;
		break;
	case flx_cannot_calc_size:
		o_inf.bjo_error = bje_file_corrupted;
		break;
	case flx_cannot_fit_in_mem:
		o_inf.bjo_error = bje_file_too_big;
		break;
	case flx_path_too_long:
		o_inf.bjo_error = bje_path_too_long;
		break;
	};
}

void
brain::dbg_init_html(){
#ifdef FULL_DEBUG
	brain& brn = *this;
	bj_ostream& os = bj_dbg; 
	ch_string htm_pth = get_solver().slv_dbg2.dbg_html_out_path;
	ch_string sub_dir = htm_pth + "/" + br_dbg.dbg_cy_prefix;
	path_delete(sub_dir, htm_pth);
	if(! file_exists(sub_dir)){
		path_create(sub_dir);
		os << "Created dir: '" << sub_dir << "'\n";
	}
	
	ch_string rn_pth = path_get_running_path();
	ch_string rn_dir = path_get_directory(rn_pth, true);
	//ch_string js_dir = rn_dir + "../" + CY_LIB_DIR;
	ch_string js_dir = rn_dir + "/" + CY_LIB_DIR;
	ch_string lk_nm = sub_dir + "/" + CY_LIB_DIR;
	
	os << "creating link \n js_dir=" << js_dir << "\n lk_nm=" << lk_nm;
	bool ok = path_create_link(js_dir, lk_nm);
	BRAIN_CK(ok);

	ch_string cnfs_dir = get_cy_dir(brn) + "/" + br_dbg.dbg_cy_prefix;
	bool dir_ok = file_exists(cnfs_dir);
	if(! dir_ok){
		path_create(cnfs_dir);
		os << "Created dir: '" << cnfs_dir << "'\n";
	}
	BRAIN_CK(file_exists(cnfs_dir));
	
	dbg_start_html();
#endif
}

bj_satisf_val_t
brain::solve_instance(bool load_it){
	brain& brn = *this;
	MARK_USED(brn);
	
	//br_dbg.dbg_old_deduc = true; // comment in normal use. only for old_deduc.

	DBG_COMMAND(1, // KEEP_LEARNED KEEP_ALL_LEARNED
		if(! dbg_as_release()){
			br_dbg_keeping_learned = true;
		}
	); 
	
	BRAIN_DBG(bool init_htm = false);
	DBG_COMMAND(45, init_htm = true);
	DBG_COMMAND(70, init_htm = true);
	DBG_COMMAND(72, init_htm = true);
	BRAIN_DBG(if(init_htm){ dbg_init_html(); });

	DBG_PRT(71, os << "FINDING_PHI_REPE");
	
	instance_info& inst_info = get_my_inst();
	bj_output_t& o_info = get_out_info();
	try{
		if(load_it){
			bool all_ok = load_instance();
			if(! all_ok){
				throw instance_exception(inx_bad_lit);
			}
		}
		think();
	} catch (skeleton_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_invalid_root_directory;
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (file_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		set_file_err(ex1, o_info);
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (parse_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_parse_bad_number;
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (dimacs_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		set_dimacs_err(ex1, o_info);
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (instance_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_instance_cannot_load;
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (mem_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_memout;
		inst_info.ist_err_stack_str = ex1.ex_stk;
	} catch (top_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_internal_ex;
		inst_info.ist_err_stack_str = ex1.ex_stk;
		
		inst_info.ist_err_assrt_str = ex1.ex_assrt;
		
		bj_err << "ASSERT=" << ex1.ex_assrt << bj_eol;
		bj_err << ex1.ex_stk << bj_eol;
	}
	catch (...) {
		BRAIN_DBG(
			bj_out << "INTERNAL ERROR !!! (call_and_handle_exceptions)" << bj_eol;
			bj_out << STACK_STR << bj_eol;
			bj_out.flush();
			abort_func(0);
		)
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_internal;
	}
	
	recoil_counter_t num_laps = recoil();
	o_info.bjo_num_laps = num_laps.get_d();

	double end_solve_tm = run_time();
	double slv_tm = (end_solve_tm - br_start_solve_tm);	
	o_info.bjo_solve_time = slv_tm;
	
	instance_info& iinfo = get_my_inst();
	o_info.bjo_max_variants = iinfo.ist_num_variants_stat.vs_max_val.get_d();
	o_info.bjo_avg_variants = iinfo.ist_num_variants_stat.avg.get_d();

	DBG_PRT(105, os << " LOC_CHOS=" << br_dbg.dbg_num_loc_cho << "/";
		os << (br_dbg.dbg_num_loc_cho + br_dbg.dbg_num_glb_cho);
	);
	
	BRAIN_DBG(if(init_htm){ dbg_finish_html(); });
	
	return o_info.bjo_result;
}

bool
brain::deduce_and_reverse_trail(){
	BRAIN_CK(! br_dbg.dbg_old_deduc);
	BRAIN_CK(! has_psignals());
	
	//BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK(found_conflict());

	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());
	
	// analyse
	
	BRAIN_DBG(if(level() > br_dbg.dbg_max_lv){ br_dbg.dbg_max_lv = level(); });
	
	deduction& dct = br_pulse_deduc;
	reason& rsn = dct.dt_rsn;
	
	dct.reset_deduction();
	//rsn.init_reason();

	candidates_before_analyse();
	
	bool in_full_anls = true;
	DBG_COMMAND(2, // ONLY_DEDUC
		if(! dbg_as_release()){
			in_full_anls = false;
		}
	); 
	if(in_full_anls){
		analyse_conflicts(br_all_conflicts_found, dct);
	} else {
		br_wrt_ref.reset_curr_quanton();
		write_get_tk(br_curr_write_tk);
		
		deducer& ddc = br_dedcer;
		ddc.set_conflicts(br_all_conflicts_found);
		
		DBG_PRT(40, os << "bef_ana=" << bj_eol; print_trail(os);
			os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
			<< " br_ti=" << tier() << "\n";
			os << " cnfl=" << ddc.get_first_conflict().ps_source;
		);
		
		row<neuromap*>& to_wrt = dct.dt_all_to_wrt;
		to_wrt.clear();
		ddc.get_first_causes(dct);
		ddc.deduce(dct);
	}
	
	solver& slv = get_solver();
	if(slv.slv_prms.sp_write_proofs){
		proof_write_all_json_files_for(dct);
	}
	
	DBG_PRT_COND(101, dct.dt_found_top, os << " FOUND_TOP. rsn=" << rsn;
		os << " br_candidate_rsn_lv=" << br_candidate_rsn_lv;
	);
	//bool go_on = ! dct.dt_found_top && ! in_root_lv();
	bool go_on = dct.can_go_on() && ! in_root_lv();
	
	if(go_on){
		candidates_before_reverse(rsn);
		reverse_with(rsn);
		candidates_after_reverse();
	}
	
	return go_on;
}

void
brain::write_all_canditates(){
	BRAIN_CK(br_candidate_nxt_nmp_lvs.is_empty());
	write_all_nmps(br_candidate_nmp_lvs);
}

void
brain::reverse_with(reason& rsn){
	BRAIN_DBG(brain& brn = *this);
	BRAIN_DBG(leveldat& tg_lv = get_leveldat(rsn.rs_target_level));
	BRAIN_CK_PRT((	in_root_lv() || (rsn.rs_target_level == ROOT_LEVEL) || 
					((tg_lv.ld_chosen != NULL_PT) && ! tg_lv.is_ld_mono())), 
		DBG_PRT_ABORT(brn);
		os << " r_tg_lv=" << rsn.rs_target_level << "\n";
		if(tg_lv.ld_chosen != NULL_PT){
			os << " is_mono=" << tg_lv.is_ld_mono() << "\n";
		}
		os << " tg_lv=" << tg_lv << "\n";
		print_trail(os);
	);
	br_prv_round_last_rc = recoil();
	
	// retract
	retract_to(rsn.rs_target_level, false);
	BRAIN_CK(ck_write_quas(rsn));
	
	// some checks

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = rsn.rs_target_level);
	BRAIN_DBG(long rr_lv = trail_level());
	
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == rsn.rs_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == rsn.rs_target_level));
	BRAIN_CK(rsn.rs_forced != NULL_PT);

	// learn motives
	
	neuron* lnd_neu = learn_mots(rsn);
	
	// send forced learned

	quanton* nxt_qua = rsn.rs_forced;
	if(! rsn.rs_motives.is_empty()){
		BRAIN_CK(nxt_qua != NULL_PT);
		send_psignal(*nxt_qua, lnd_neu, tier() + 1);
	}

	// finish reverse
	br_round++;

	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());
	BRAIN_CK((level() == ROOT_LEVEL) || ! get_leveldat().is_ld_mono());

	reset_conflict();
	BRAIN_CK(! found_conflict());
}

void
brain::dbg_prt_full_stab(){
#ifdef FULL_DEBUG
	binder pru_bb;
	grip pru_gg;
	ss_srs_t pru_rr;
	sortee pru_tt;
	sorset pru_ss;

	brain& brn = *this;
	skeleton_glb& skg = get_skeleton();
	
	coloring full_col;
	full_col.init_coloring(this);
	full_col.dbg_set_brain_coloring();

	//bj_out << "COLS_BEF=" << bj_eol << full_col;
	
	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;

	all_mutual_init();
	
	//dima_dims dims0;
	full_col.load_colors_into(neus_srg, quas_srg, dbg_call_5);

	neus_srg.sg_cnf_clauses.clear();
	
	neus_srg.stab_mutual(quas_srg, true);
	
	//bj_out << "ALL_CCL=\n";
	
	canon_cnf& the_cnf = neus_srg.get_final_cnf(skg, true);

	bj_out << "THE_CNF=" << bj_eol;
	bj_out << the_cnf;
	bj_out << "END_of_cnf=" << bj_eol;
	
	coloring finl_col;
	finl_col.save_colors_from(neus_srg, quas_srg, tid_tee_consec, true);
	
	BRAIN_CK(finl_col.co_quas.size() == full_col.co_quas.size());
	BRAIN_CK(finl_col.co_neus.size() == full_col.co_neus.size());
	
	release_all_sortors();
#endif
}

bool 
brain::ck_confl_ti(){
#ifdef FULL_DEBUG
	if(br_all_conflicts_found.size() < 2){
		return true;
	}
	long lst_idx = br_all_conflicts_found.last_idx();
	long prv_idx = lst_idx - 1;
	prop_signal& cfl_0 = br_all_conflicts_found[lst_idx];
	prop_signal& cfl_1 = br_all_conflicts_found[prv_idx];
	BRAIN_CK(cfl_0.ps_tier == cfl_1.ps_tier);
#endif
	return true;
}

long
quanton::find_alert_idx(bool is_init, row_quanton_t& all_pos){
	row_neuron_t& all_neus = qu_all_neus;
	long fst_idx = qu_alert_neu_idx;
	
	all_pos.clear();
	
	if(all_neus.is_empty()){
		return INVALID_IDX;
	}
	
	if(is_init){
		fst_idx = 0;
	} 
	
	BRAIN_CK(all_neus.is_valid_idx(fst_idx));
	BRAIN_CK(all_neus[fst_idx] != NULL_PT);
	
	if(! is_init){
		BRAIN_CK(all_neus[fst_idx]->is_ne_inert());
		fst_idx++;
	}
	
	long found_idx = INVALID_IDX;
	for(long aa = fst_idx; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		quanton* pqu = neu.find_is_pos();
		if(pqu == NULL_PT){
			all_pos.clear();
			found_idx = aa;
			break;
		}
		if(is_init){
			BRAIN_CK(pqu != NULL_PT);
			quanton& opp = pqu->opposite();
			all_pos.push(&opp);
		}
	}
	if(is_init || (found_idx != INVALID_IDX)){
		return found_idx;
	}
	
	long lst_idx = qu_alert_neu_idx;
	BRAIN_CK(all_neus.is_valid_idx(lst_idx));
	BRAIN_CK(all_neus[lst_idx] != NULL_PT);
	BRAIN_CK(all_neus[lst_idx]->is_ne_inert());
	
	for(long aa = 0; aa < lst_idx; aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		if(neu.is_ne_alert()){
			found_idx = aa;
			break;
		}
	}
	return found_idx;
}

void
quanton::update_alert_neu(brain& brn, bool is_init){
	quanton& qua = *this;
	
	BRAIN_CK(! has_charge());
	BRAIN_CK(qu_lv_mono == INVALID_LEVEL);
	BRAIN_CK(! is_init || (qu_alert_neu_idx == INVALID_IDX));
	BRAIN_CK(! is_init || (brn.level() == ROOT_LEVEL));
	BRAIN_CK(is_init || qu_all_neus.is_valid_idx(qu_alert_neu_idx));
	BRAIN_CK(is_init || qu_all_neus[qu_alert_neu_idx] != NULL_PT);
	BRAIN_CK(is_init || qu_all_neus[qu_alert_neu_idx]->is_ne_inert());

	long found_idx = find_alert_idx(is_init, brn.br_tmp_mono_all_neg);
	if(found_idx != INVALID_IDX){
		set_alert_neu(brn, found_idx);
		
		BRAIN_DBG(neuron* al_neu = get_alert_neu());
		BRAIN_CK(al_neu != NULL_PT);
		BRAIN_CK(al_neu->is_ne_alert());
		//BRAIN_CK(qu_all_neus.is_valid_idx(qu_alert_neu_idx));
		//BRAIN_CK(qu_all_neus[qu_alert_neu_idx] != NULL_PT);
		//BRAIN_CK(qu_all_neus[qu_alert_neu_idx]->is_ne_alert());
		return;
	}
	qu_lv_mono = brn.level();
	
	BRAIN_CK(qua.is_mono());
	brn.br_monos.push(&qua);
}

void
brain::init_alert_neus(){
	brain& brn = *this;
	for(long ii = 0; ii < br_positons.size(); ii++){
		quanton& qua_pos = br_positons[ii];
		qua_pos.update_alert_neu(brn, true);
	}

	for(long ii = 0; ii < br_negatons.size(); ii++){
		quanton& qua_neg = br_negatons[ii];
		qua_neg.update_alert_neu(brn, true);
	}
}

void
brain::update_monos(){
	brain& brn = *this;
	long lv = level();
	
	DBG_PRT(143, os << "start monos updte.......... lv=" << lv);
	
	qlayers_ref qtrl;
	qtrl.init_qlayers_ref(&br_charge_trail);
	qtrl.reset_curr_quanton();

	row_quanton_t& all_quas = br_tmp_uncharged_in_alert_neus;
	all_quas.clear();
	
	BRAIN_CK(br_qu_tot_note6 == 0);
	
	while(qtrl.get_curr_qlevel() == lv){
		quanton* qua = qtrl.get_curr_quanton();
		BRAIN_CK(qua != NULL_PT);
		
		qua->append_all_to_alert(brn, all_quas);
		
		qtrl.dec_curr_quanton();
	}
	
	for(long bb = 0; bb < all_quas.size(); bb++){
		BRAIN_CK(all_quas[bb] != NULL_PT);
		quanton& qua = *(all_quas[bb]);
		BRAIN_CK(qua.is_note6());
		qua.reset_note6(brn);
		
		DBG_PRT(143, os << "monos updte qua=" << &qua);
		
		qua.update_alert_neu(brn, false);
	}

	BRAIN_CK(br_qu_tot_note6 == 0);
	all_quas.clear();
}

neuron*
quanton::get_alert_neu(){
	row_neuron_t& all_neus = qu_all_neus;
	long al_idx = qu_alert_neu_idx;
	
	if(! all_neus.is_valid_idx(al_idx)){
		BRAIN_CK(al_idx == INVALID_IDX);
		return NULL_PT;
	}
	
	BRAIN_CK(all_neus[al_idx] != NULL_PT);
	neuron* neu = all_neus[al_idx];
	return neu;
}

bool
quanton::is_mono(){
	bool is_mo = (qu_lv_mono != INVALID_LEVEL);
	BRAIN_CK((qu_alert_neu_idx != INVALID_IDX) || (is_mo && (qu_lv_mono == ROOT_LEVEL)));
	return is_mo;
}

void
alert_rel::init_alert_rel(quanton* alert, quanton* ref){
	ar_alert.re_me = this;
	ar_ref.re_me = this;
	
	ar_qu_alert = alert;
	ar_qu_ref = ref;
	
	ar_alert.let_go();
	ar_ref.let_go();
	
	if(ar_qu_alert != NULL_PT){
		ar_qu_alert->qu_mono_refs.bind_to_my_right(ar_ref);
	}
	
	if(ar_qu_ref != NULL_PT){
		ar_qu_ref->qu_mono_alerts.bind_to_my_right(ar_alert);
	}
}

void
quanton::set_alert_neu(brain& brn, long the_idx){
	BRAIN_CK(qu_all_neus.is_valid_idx(the_idx));
	quanton& the_alert = *this;
	
	row<alert_rel*> all_rels;
	qu_mono_refs.append_all_as<alert_rel>(all_rels, true);
	
	for(long aa = 0; aa < all_rels.size(); aa++){
		BRAIN_CK(all_rels[aa] != NULL_PT);
		alert_rel& rr = *(all_rels[aa]);
		brn.release_alert_rel(rr);
	}
	
	BRAIN_CK(qu_mono_refs.is_alone());

	qu_alert_neu_idx = the_idx;
	
	neuron* pt_neu = get_alert_neu();
	BRAIN_CK(pt_neu != NULL_PT);
	neuron& neu = *pt_neu;

	BRAIN_DBG(bool in_fibs = false);
	for(long aa = 0; aa < neu.fib_sz(); aa++){
		BRAIN_CK(neu.ne_fibres[aa] != NULL_PT);
		quanton& qua = *(neu.ne_fibres[aa]);

		BRAIN_DBG(if(&qua == &the_alert){ in_fibs = true; });
		
		alert_rel& rr1 = brn.locate_alert_rel();
		rr1.init_alert_rel(&the_alert, &qua);
	}
	
	DBG_PRT(143, os << "monos qua=" << &the_alert << " watch_neu=" << pt_neu);
		
	BRAIN_CK(in_fibs);
}

void
quanton::append_all_to_alert(brain& brn, row_quanton_t& all_quas){
	row<alert_rel*> all_rels;
	qu_mono_alerts.append_all_as<alert_rel>(all_rels, true);

	for(long aa = 0; aa < all_rels.size(); aa++){
		BRAIN_CK(all_rels[aa] != NULL_PT);
		alert_rel& rr = *(all_rels[aa]);
		
		BRAIN_CK(rr.ar_qu_alert != NULL_PT);
		quanton& qua = *(rr.ar_qu_alert);
		
		DBG_PRT(143, os << "monos ck qua=" << this << " watch_qua=" << &qua);
	
		if(! qua.is_note6() && ! qua.has_charge() && ! qua.is_mono()){
			qua.set_binote6(brn);
			all_quas.push(&qua);
		}
	}
}

bool
brain::ck_prev_monos(){
#ifdef FULL_DEBUG
	if(br_monos.is_empty()){
		return true;
	}
	for(long aa = 0; aa < br_last_monocho_idx; aa++){
		BRAIN_CK_PRT(br_monos.is_valid_idx(aa), os << "________________\n";
			os << dbg_prt_margin(os);
			os << " aa=" << aa;
			os << " br_last_monocho_idx=" << br_last_monocho_idx;
			os << " lv=" << br_data_levels.last_idx();
			os << "\n br_monos=" << br_monos;
		);
		BRAIN_CK(br_monos[aa] != NULL_PT);
		quanton& qua = *(br_monos[aa]);
		BRAIN_CK(qua.is_mono());
		BRAIN_CK_PRT(qua.has_charge(), os << "________________\n";
			os << dbg_prt_margin(os);
			os << " monos=" << br_monos << "\n";
			os << " mo_idx=" << br_last_monocho_idx;
			os << " aa=" << aa << " qua=" << &qua << " lv=" << br_data_levels.last_idx();
		);
	}
#endif
	return true;
}

void
leveldat::reset_monos(brain& brn){
	DBG_PRT(143, os << "RESET monos ..........");
	BRAIN_CK(ld_idx == brn.level());
	
	row_quanton_t& all_monos = brn.br_monos;
	while(! all_monos.is_empty()){
		quanton& lqua = *(all_monos.last());
		if(lqua.qu_lv_mono < ld_idx){
			break;
		}
		quanton& qua = *(all_monos.pop());
		BRAIN_CK(qua.is_mono());
		BRAIN_CK(qua.qu_lv_mono == ld_idx);
		
		qua.qu_lv_mono = INVALID_LEVEL;
		
		BRAIN_DBG(
			if(ld_idx == ROOT_LEVEL){ continue; }
			neuron* neu = qua.get_alert_neu();
			BRAIN_CK(neu != NULL_PT);
			BRAIN_CK_PRT(neu->is_ne_alert(), 
				DBG_PRT_ABORT(brn);
				brn.print_trail(os);
				os << " lvs_sz=" << brn.br_data_levels.size();
				os << " ld_idx=" << ld_idx << " qua=" << &qua << "\n neu=\n" << neu;
			);
		);
	}

	if(brn.get_leveldat().ld_bak_mono_idx != INVALID_IDX){
		brn.br_last_monocho_idx = brn.get_leveldat().ld_bak_mono_idx;
	} else {
		brn.br_last_monocho_idx = 0;
	}
	BRAIN_CK(brn.ck_prev_monos());
}

/*
void
brain::fill_mono_rsn(reason& rsn, row_quanton_t& mots, quanton& mono){
	BRAIN_DBG(brain& brn = *this);
	
	rsn.init_reason();
	mots.move_to(rsn.rs_motives);
	rsn.rs_target_level = find_max_level(rsn.rs_motives);
	rsn.rs_forced = &mono;
	
	bool br_in_root = (level() == ROOT_LEVEL);

	BRAIN_CK(ck_motives(brn, rsn.rs_motives));

	DBG_PRT(20, os << "find_rsn of reason=" << rsn);
	
	if(br_in_root){
		rsn.rs_motives.clear();
		BRAIN_CK(rsn.rs_target_level == ROOT_LEVEL);
		BRAIN_CK(rsn.rs_motives.is_empty());
		rsn.rs_target_level = INVALID_LEVEL;
	}
}*/

bj_ostream&
brain::dbg_print_all_qua_rels(bj_ostream& os){
#ifdef FULL_DEBUG
	row<alert_rel*> all_alert;
	row<alert_rel*> all_refs;
	
	os << "ALL_RELS\n";
		
	for(long ii = 0; ii < br_positons.size(); ii++){
		quanton& qua_pos = br_positons[ii];
		os << "QUA=" << &qua_pos << "\n";
		qua_pos.qu_mono_alerts.append_all_as<alert_rel>(all_alert, true);
		os << "alerts=" << all_alert << "\n";
		qua_pos.qu_mono_refs.append_all_as<alert_rel>(all_refs, true);
		os << "refs=" << all_refs << "\n";
	}

	for(long ii = 0; ii < br_negatons.size(); ii++){
		quanton& qua_neg = br_negatons[ii];
		os << "QUA=" << &qua_neg << "\n";
		qua_neg.qu_mono_alerts.append_all_as<alert_rel>(all_alert, true);
		os << "alerts=" << all_alert << "\n";
		qua_neg.qu_mono_refs.append_all_as<alert_rel>(all_refs, true);
		os << "refs=" << all_refs << "\n";
	}
#endif
	return os;
}

quanton*
brain::get_curr_mono(){
	long mo_idx = br_last_monocho_idx;
	if(! br_monos.is_valid_idx(mo_idx)){
		return NULL_PT;
	}
	quanton* qua = br_monos[mo_idx];
	BRAIN_CK(qua != NULL_PT);
	return qua;
}

quanton*
brain::choose_mono(){
	//br_mono_rsn.init_reason();
	
	if(get_leveldat().ld_bak_mono_idx == INVALID_IDX){
		get_leveldat().ld_bak_mono_idx = br_last_monocho_idx;
	}
	
	quanton* qua = get_curr_mono();
	while((qua != NULL_PT) && qua->has_charge()){
		br_last_monocho_idx++;
		qua = get_curr_mono();
	}
	//BRAIN_CK(ck_prev_monos());
	if(qua == NULL_PT){
		return NULL_PT;
	}
	BRAIN_CK(ck_prev_monos());
	
	long alrt_idx = qua->find_alert_idx(true, br_tmp_mono_all_neg);
	MARK_USED(alrt_idx);
	BRAIN_CK(alrt_idx == INVALID_IDX);
	BRAIN_CK(br_tmp_mono_all_neg.size() == qua->qu_all_neus.size());
	
	BRAIN_CK(qua->is_mono());
	quanton& opp = qua->opposite();
	
	//fill_mono_rsn(br_mono_rsn, br_tmp_mono_all_neg, opp);
	
	BRAIN_CK(! opp.has_charge());
	return &opp;
}

void
brain::dbg_lv_on(long lv_idx){
	BRAIN_DBG(get_solver().slv_dbg_conf_info.dbg_lv_on(lv_idx));
}

void
brain::dbg_lv_off(long lv_idx){
	BRAIN_DBG(get_solver().slv_dbg_conf_info.dbg_lv_off(lv_idx));
}

void
solver::init_solver(){
	slv_dbg_brn = NULL_PT;
	dbg_read_dbg_conf(slv_dbg_conf_info);
	slv_skl.kg_pt_slv = this;
}

void			
quanton::set_source(neuron* neu){
	BRAIN_CK((qu_source == NULL_PT) || (qu_inverse->qu_source == NULL_PT));
	BRAIN_CK((qu_inverse->qu_source == NULL_PT) || (neu == NULL_PT));

	BRAIN_DBG(
		if(! has_charge()){
			BRAIN_CK(neu == NULL_PT);
			BRAIN_CK(qu_inverse->qu_source == NULL_PT);
		}
	)
	
	qu_source = neu;
	if(neu == NULL_PT){ 
		qu_inverse->qu_source = NULL_PT;
	}

	BRAIN_CK((qu_source == NULL_PT) || qu_source->ck_all_neg(1));
}

void
brain::get_bineu_sources(quanton& cho, quanton& qua, row_quanton_t& all_biqus)
{
	brain& brn = *this;
	
	BRAIN_CK(cho.is_pos());
	BRAIN_CK(qua.is_pos());

	if(! is_tk_equal(qua.qu_cicle_tk, br_curr_choice_tk)){
		DBG_PRT(68, 
			os << " RESET CICLES qua=" << &qua << "\n";
			os << " clc_tk=" << qua.qu_cicle_tk << "\n";
			os << " br_tk=" << br_curr_choice_tk << "\n";
			os << " clc_srcs=" << qua.qu_all_cicle_sources << "\n";
		);
		qua.reset_cicle_src();
	}
	
	row_neuron_t& all_src = qua.qu_all_cicle_sources;

	DBG_PRT(68, 
		os << "----------------------------------------------\n";
		os << "CHO=" << &cho << "\n";
		os << ".qua=" << &qua << "\n";
		os << ".clc_srcs=" << "\n";
		qua.qu_all_cicle_sources.print_row_data(os, true, "\n");
	);
	
	for(long aa = 0; aa < all_src.size(); aa++){
		BRAIN_CK(all_src[aa] != NULL_PT);
		neuron& neu = *(all_src[aa]);
		neu.append_ne_biqu(brn, cho, qua, all_biqus);
	}
}

void
brain::get_all_bineu_sources(quanton& cho, row_quanton_t& all_src)
{
	brain& brn = *this;
	
	BRAIN_CK(cho.is_cicle_choice());
	BRAIN_CK(br_qu_tot_note2 == 0);
	
	row_quanton_t& prv_biqus = br_tmp_prv_biqus;
	row_quanton_t& nxt_biqus = br_tmp_nxt_biqus;
	
	prv_biqus.clear();
	nxt_biqus.clear();
	all_src.clear();

	get_bineu_sources(cho, cho, nxt_biqus);
	while(! nxt_biqus.is_empty()){
		DBG_PRT(68, 
			os << ".prv_biqus=" << prv_biqus << "\n";
			os << ".nxt_biqus=" << nxt_biqus << "\n";
		);
		
		prv_biqus.append_to(all_src);
		prv_biqus.clear();
		nxt_biqus.move_to(prv_biqus);
		BRAIN_CK(nxt_biqus.is_empty());

		for(long aa = 0; aa < prv_biqus.size(); aa++){
			BRAIN_CK(prv_biqus[aa] != NULL_PT);
			quanton& qua = *(prv_biqus[aa]);
			quanton& opp = qua.opposite();
			BRAIN_CK(opp.is_pos());
			neuron* neu = opp.get_source();
			if(neu != NULL_PT){
				neu->append_ne_biqu(brn, cho, opp, nxt_biqus);
			}
		}
	}
	prv_biqus.append_to(all_src);
	prv_biqus.clear();
	
	reset_all_note2(brn, all_src);
	BRAIN_CK(br_qu_tot_note2 == 0);
}

void
neuron::append_ne_biqu(brain& brn, quanton& cho, quanton& pos_qu, row_quanton_t& all_biqus){
	BRAIN_CK(pos_qu.is_pos());
	long b_lv = brn.level();
	long c_lv = cho.qlevel();
	
	quanton* biqu = NULL_PT;
	for(long aa = 0; aa < fib_sz(); aa++){
		quanton* qua = ne_fibres[aa];
		BRAIN_CK(qua != NULL_PT);
		bool is_n = qua->is_neg();
		BRAIN_CK_PRT((! qua->is_pos() || (qua == &pos_qu)),
				os << "________________\n ABORT_DATA\n";
				brn.print_trail(os);
				os << " neu=" << this;
		);
		bool in_confl = qua->has_note1();
		//bool in_confl = false;
		if(is_n){
			long q_lv = qua->qlevel();
			bool is_biqu = ((b_lv == q_lv) && ! in_confl);
			if(is_biqu){
				if(biqu != NULL_PT){
					biqu = NULL_PT;
					break;
				}
				biqu = qua;
			} else {
				if(q_lv >= c_lv){
					biqu = NULL_PT;
					break;
				}
			}
		}
		
	}
	if(biqu != NULL_PT){
		BRAIN_CK(biqu->is_neg());
		if(! biqu->has_note2()){
			biqu->set_note2(brn);
			BRAIN_CK(biqu->ck_biqu(brn));
			all_biqus.push(biqu);
			DBG_PRT(68, 
				os << " ADDED biqu=" << biqu;
				os << " bineu=" << this;
				os << " pos_q=" << &pos_qu;
				os << " b_lv=" << b_lv;
				os << " c_lv=" << c_lv;
			);
			// << " cho=" << &cho << " c_lv=" << c_lv);
		}
	}
}

bool
quanton::ck_biqu(brain& brn){
#ifdef FULL_DEBUG
	BRAIN_CK(is_neg());
	BRAIN_CK(qlevel() == brn.level());
	BRAIN_CK(! has_note1());
#endif
	return true;
}

void
brain::get_all_cicle_cho(row_quanton_t& all_cicl){
	//brain& brn = *this;
	
	all_cicl.clear();
	
	BRAIN_CK(! is_curr_cho_mono());
	
	row_quanton_t& all_cho = br_chosen;
	for(long aa = all_cho.last_idx(); aa >= 0; aa--){
		BRAIN_CK(all_cho[aa] != NULL_PT);
		quanton& qua = *(all_cho[aa]);
		
		if(qua.has_mono()){
			continue;
		}
		if(qua.has_note1()){
			break;
		}
		if(qua.qu_all_cicle_sources.is_empty()){
			break;
		}
		long qlv = qua.qlevel();
		if(qlv == ROOT_LEVEL){
			break;
		}
		leveldat& lv_dat = get_leveldat(qlv);
		BRAIN_CK(lv_dat.ld_chosen == &qua);
		if(lv_dat.has_learned()){
			break;
		}
		BRAIN_CK(qua.is_cicle_choice());

		all_cicl.push(&qua);
	}
}

void
quanton::update_cicle_srcs(brain& brn, neuron* neu){
	if(is_cicle_choice() && (neu != NULL_PT) && is_pos()){
		if(! is_tk_equal(qu_cicle_tk, brn.br_curr_choice_tk)){
			reset_cicle_src();
		}
		if(qu_cicle_tk.is_tk_virgin()){
			brn.update_tk_charge(qu_cicle_tk);
		}
		//DBG_PRT(DBG_ALL_LVS, os << " neu=" << neu);
		qu_all_cicle_sources.push(neu);
	}
}

quanton*
brain::get_cicles_common_cho(quanton*& old_cho, row_quanton_t& all_impl_cho)
{
	brain& brn = *this;
	
	all_impl_cho.clear();
	old_cho = NULL_PT;
	
	if(is_curr_cho_mono()){
		return NULL_PT;
	}
	
	BRAIN_CK(br_qu_tot_note1 == 0);
	set_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
	
	row_quanton_t& all_cicl = br_tmp_all_cicles;
	get_all_cicle_cho(all_cicl);
	
	if(all_cicl.size() < 2){
		reset_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
		BRAIN_CK(br_qu_tot_note1 == 0);
		return NULL_PT;
	}
	
	quanton* nw_cho = NULL_PT;
	
	DBG_PRT(68, os << "Entering get_cicles_common.cho\n";
		print_trail(os);
		os << " CHOSEN=" << br_chosen << "\n";
		os << " ALL_CICL=" << all_cicl << "\n";
		os << " all_confl=\n";
		br_all_conflicts_found.print_row_data(os, true, "\n");
	);
	
	row_quanton_t& bqs_1 = br_tmp_biqus_lv1;
	row_quanton_t& bqs_2 = br_tmp_biqus_lv2;
	row_quanton_t& bqs_3 = br_tmp_biqus_lv3;
	bqs_1.clear();
	bqs_2.clear();
	bqs_3.clear();
	
	quanton* pt_cho_1 = all_cicl.first();
	BRAIN_CK(pt_cho_1 != NULL_PT);
	quanton& cho_1 = *(pt_cho_1);
	BRAIN_CK(cho_1.is_cicle_choice());
	BRAIN_CK(cho_1.is_pos());
	get_all_bineu_sources(cho_1, bqs_1);
	
	if(bqs_1.is_empty()){
		reset_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
		BRAIN_CK(br_qu_tot_note1 == 0);
		return NULL_PT;
	}
	
	BRAIN_CK(! bqs_1.is_empty());
	BRAIN_DBG(long cho_lv = cho_1.qlevel());

	long aa = 0;
	for(aa = 1; aa < all_cicl.size(); aa++){
		BRAIN_CK(all_cicl[aa] != NULL_PT);
		quanton& cho_2 = *(all_cicl[aa]);
		
		BRAIN_CK(cho_2.is_cicle_choice());
		BRAIN_CK(cho_2.is_pos());
		BRAIN_CK(cho_2.qlevel() < cho_lv);
		BRAIN_DBG(cho_lv = cho_2.qlevel());
		
		BRAIN_CK(bqs_2.is_empty());
		get_all_bineu_sources(cho_2, bqs_2);
		if(bqs_2.is_empty()){
			break;
		}
		
		BRAIN_CK(bqs_3.is_empty());
			
		BRAIN_CK(br_qu_tot_note2 == 0);
		set_all_note2(brn, bqs_1);
		append_all_has_note2(brn, bqs_2, bqs_3);
		reset_all_note2(brn, bqs_1);
		BRAIN_CK(br_qu_tot_note2 == 0);

		if(bqs_3.is_empty()){
			break;
		}
		
		if(all_impl_cho.is_empty()){
			all_impl_cho.push(&cho_1);
		}
		all_impl_cho.push(&cho_2);
		
		old_cho = &cho_2;
		bqs_3.move_to(bqs_1);
		bqs_2.clear();
	}
	
	if(old_cho != NULL_PT){
		BRAIN_CK(! bqs_1.is_empty());
		
		quanton* fst_cm = bqs_1.first();
		BRAIN_CK(fst_cm != NULL_PT);
		
		nw_cho = fst_cm->qu_inverse;
		BRAIN_CK(nw_cho != NULL_PT);
		BRAIN_CK(nw_cho != old_cho);
		
		BRAIN_CK(old_cho->is_pos());
		BRAIN_CK(nw_cho->is_pos());
	}
	
	bqs_1.clear();
	bqs_2.clear();
	bqs_3.clear();
	
	DBG_PRT(68, os << "FINISHING get_cicles_common.cho\n";
		os << " all_impl_cho=\n" << all_impl_cho << "\n";
		os << " old_cho=" << old_cho << "\n";
		os << " nw_cho=" << nw_cho << "\n";
		os << " aa=" << aa << "\n";
	);
	
	reset_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
	BRAIN_CK(br_qu_tot_note1 == 0);
	return nw_cho;
}

void
quanton::reset_cicle_src(){
	qu_cicle_tk.init_ticket();
	opposite().qu_cicle_tk.init_ticket();
	
	qu_all_cicle_sources.clear();
	opposite().qu_all_cicle_sources.clear();
}

leveldat*
quanton::qlv_dat(brain& brn){
	if(! has_charge()){
		return NULL_PT;
	}
	long lv = qlevel();
	leveldat& lv_dat = brn.get_leveldat(lv);
	return &lv_dat;
}

bool
quanton::is_lv_choice(brain& brn){
	leveldat* lv_dt = qlv_dat(brn);
	if(lv_dt == NULL_PT){
		return false;
	}
	if(lv_dt->ld_chosen != this){
		return false;
	}
	BRAIN_CK(is_choice());
	return true;
}

void
brain::send_next_mono(){
	//BRAIN_CK(! has_psignals());
	update_monos();
	quanton* qua = choose_mono();
	if(qua == NULL_PT){
		return;
	}
	
	BRAIN_CK(qua->is_opp_mono());
	send_psignal(*qua, NULL, tier());
	BRAIN_CK(has_psignals());
}

bool
quanton::is_qu_end_of_neuromap(brain& brn){
	if(qu_tier == INVALID_TIER){
		return false;
	}
	if(in_root_qlv()){
		if(has_candidate(brn)){
			return true;
		}
		return false;
	}
	row_quanton_t& qlyr = brn.br_charge_trail.get_qu_layer(qu_tier);
	if(qu_source == NULL_PT){
		bool is_fst = (! qlyr.is_empty() && (qlyr.first() == this));
		//BRAIN_CK((qlyr.size() == 1) || is_opp_mono());
		return is_fst;
	}
	if(! qu_source->ne_original){
		BRAIN_CK(qlyr.size() == 1);
		BRAIN_CK(qlyr.first() == this);
		return true;
	}
	return false;
}

neuromap*
brain::pop_cand_lv_in(row_neuromap_t& lvs, bool free_mem, bool force_rel){
	brain& brn = *this;
	MARK_USED(brn);
	
	BRAIN_CK(! lvs.is_empty());
	neuromap* lst_nmp = lvs.pop();
	BRAIN_CK(lst_nmp != NULL_PT);
	
	if(force_rel){
		if(! lst_nmp->na_is_head){
			BRAIN_CK(! lvs.is_empty());
			neuromap* nw_lst = lvs.last();
			BRAIN_CK(nw_lst != NULL_PT);
			BRAIN_CK(nw_lst->na_submap == lst_nmp);
			nw_lst->na_submap = NULL_PT;
			
			lst_nmp->na_is_head = true;
		}
	}
	
	BRAIN_DBG(quanton* q_cand = lst_nmp->na_candidate_qua);
	BRAIN_CK(q_cand != NULL_PT);
	BRAIN_CK(q_cand->qu_candidate_nmp == lst_nmp);
	
	if(free_mem){
		lst_nmp->release_candidate();
		
		BRAIN_CK(! lst_nmp->nmp_is_cand());
		BRAIN_CK(! q_cand->has_candidate(brn));
		
		lst_nmp = NULL_PT;
	}
	
	return lst_nmp;
}

neuromap*
brain::pop_cand_lv(bool free_mem){
	return pop_cand_lv_in(br_candidate_nmp_lvs, free_mem, false);
}

void
brain::pop_all_cand_lvs(){
	while(! br_candidate_nmp_lvs.is_empty()){
		pop_cand_lv(true);
	}
}

void
brain::pop_all_canditates(){
	pop_all_cand_lvs();
	pop_all_nxt_cand_lvs();
	reset_cand_next();
}

bool
neuromap::nmp_is_cand(bool ck_chg, dbg_call_id dbg_id){
	bool is_c = (na_candidate_qua != NULL_PT);
	
	BRAIN_CK_PRT((! is_c || (na_candidate_qua->qu_candidate_nmp == this)), 
		DBG_PRT_ABORT(get_brn());
		os << " call=" << dbg_id << "\n";
		os << " nmp=" << this << "\n";
	);
	BRAIN_CK_PRT((! is_c || ! ck_chg || na_candidate_qua->has_charge()), 
		brain& brn = get_brn();
		DBG_PRT_ABORT(brn);
		brn.print_trail(os);
		os << " call=" << dbg_id << "\n";
		os << " ck_chg=" << ck_chg << "\n";
		if(na_candidate_qua != NULL_PT){
			os << " has_chg=" << na_candidate_qua->has_charge() << "\n";
		}
		os << " cand_q=" << na_candidate_qua << "\n";
		os << " b_lv=" << brn.level() << "\n";
	);
	return is_c;
}

neuromap*
brain::get_last_cand(dbg_call_id dbg_id){
	if(br_candidate_nmp_lvs.is_empty()){
		return NULL_PT;
	}
	neuromap* l_cand = br_candidate_nmp_lvs.last();
	BRAIN_CK(l_cand != NULL_PT);
	BRAIN_CK(l_cand->nmp_is_cand(true, dbg_id));
	return l_cand;
}

void
quanton::set_candidate(neuromap& nmp){
	//BRAIN_CK(is_qu_end_of_neuromap(nmp.get_brn()));
	BRAIN_CK(qu_candidate_nmp == NULL_PT);
	qu_candidate_nmp = &nmp;
	BRAIN_CK(nmp.na_candidate_qua == NULL_PT);
	nmp.na_candidate_qua = this;
}

void
quanton::reset_candidate(){
	if(qu_candidate_nmp != NULL_PT){
		BRAIN_CK(qu_candidate_nmp->na_candidate_qua == this);
		qu_candidate_nmp->na_candidate_qua = NULL_PT;
	}
	qu_candidate_nmp = NULL_PT;
}

bool
quanton::has_candidate(brain& brn){
	bool h_c = (qu_candidate_nmp != NULL_PT);
	BRAIN_CK(! h_c || (qu_candidate_nmp->na_candidate_qua == this));
	BRAIN_CK(! h_c || in_root_qlv() || is_qu_end_of_neuromap(brn));
	return h_c;
}

void
brain::use_next_cand(quanton& qua){
	if(br_candidate_next != NULL_PT){
		neuromap& nxt_nmp = *(br_candidate_next);
		
		BRAIN_CK(nxt_nmp.nmp_is_cand(false));
		BRAIN_CK(nxt_nmp.na_orig_cho != NULL_PT);
		BRAIN_CK(nxt_nmp.na_orig_cho == nxt_nmp.na_candidate_qua);
		BRAIN_DBG(quanton& lst_qu = trail_last());
		BRAIN_CK(&lst_qu == &qua);
		
		nxt_nmp.na_candidate_qua->reset_candidate();
		qua.set_candidate(nxt_nmp);
		
		br_candidate_nmp_lvs.push(&nxt_nmp);
		br_candidate_next = NULL_PT;
		
		DBG_PRT(106, os << " use.next_cand");
	}
}

void
brain::reset_cand_next(){
	if(br_candidate_next != NULL_PT){
		br_candidate_next->release_candidate();
		br_candidate_next = NULL_PT;
	}
}

void
brain::pop_cand_lvs_until(quanton& qua){
	BRAIN_DBG(brain& brn = *this);
	BRAIN_CK(qua.is_pos());
	BRAIN_CK(qua.in_root_qlv() || qua.is_qu_end_of_neuromap(brn));
	neuromap* lst_cand = NULL_PT;
	while((lst_cand = get_last_cand()) != NULL_PT){
		BRAIN_CK(lst_cand != NULL_PT);
		bool is_q = (lst_cand->na_candidate_qua == &qua);
		if(is_q){
			break;
		}
		pop_cand_lv(true);
	}
}

void
brain::candidates_before_analyse(){
	
	if(! br_candidate_nxt_nmp_lvs.is_empty()){
		neuromap* tl_nmp = br_candidate_nxt_nmp_lvs.last();
		BRAIN_CK(tl_nmp != NULL_PT);
		
		init_cand_propag(*tl_nmp, NULL_PT);
		
		neuromap* hd_nmp = br_candidate_nxt_nmp_lvs.first();
		BRAIN_CK(hd_nmp != NULL_PT);
		BRAIN_CK(hd_nmp->na_is_head);
		hd_nmp->nmp_set_all_num_sub();
		
		hd_nmp->nmp_fill_all_upper_covs();
		hd_nmp->nmp_set_all_cand_tk();
		
		BRAIN_DBG(dbg_reset_all_na_creat_flags(br_candidate_nxt_nmp_lvs));
		
		br_candidate_nxt_nmp_lvs.append_to(br_candidate_nmp_lvs);
		br_candidate_nxt_nmp_lvs.clear();
	}
	DBG_PRT(101, os << "BEFORE_ANALYSE.\n"; dbg_prt_all_cands(os, false);
		print_trail(os);
		os << "\n end_of BEFORE_ANALYSE\n\n";
	);
	BRAIN_CK(dbg_ck_candidates(false));
}

neuromap*
quanton::get_candidate_to_fill(brain& brn){
	BRAIN_CK(has_charge());
	long lv = qlevel();
	if(lv == ROOT_LEVEL){
		return NULL_PT;
	}
	
	leveldat& lv_dat = brn.get_leveldat(lv);
	BRAIN_CK_PRT((lv_dat.ld_chosen != NULL_PT), 
		os << "\n_________________\n ABORT_DATA\n";
		brn.dbg_prt_margin(os);
		os << " lv=" << lv;
		os << " qua=" << this;
	);
	
	quanton& cho = *(lv_dat.ld_chosen);
	BRAIN_CK(cho.is_pos());
	if(! cho.has_candidate(brn)){
		return NULL_PT;
	}
	
	neuromap& nmp = *(cho.qu_candidate_nmp);
	bool in_creat = ((nmp.na_orig_lv == cho.qlevel()) && (nmp.na_orig_rnd == brn.br_round));
	BRAIN_CK(in_creat == nmp.na_dbg_creating);
	if(! in_creat){
		return NULL_PT;
	}
	
	return &nmp;
}

neuromap*
neuromap::nmp_init_with(quanton& qua){
	brain& brn = get_brn();
	neuromap* nxt_nmp = this;
	
	BRAIN_DBG(na_dbg_cand_sys = true);
	
	BRAIN_CK(! na_dbg_creating);
	BRAIN_DBG(na_dbg_creating = true);
	
	na_orig_rnd = brn.br_round;
	na_orig_lv = qua.qlevel();
	na_orig_ti = qua.qu_tier;
	na_orig_cho = &qua;
	na_orig_cy_ki = qua.get_cy_kind();
	
	BRAIN_CK(brn.level() == na_orig_lv);
	BRAIN_CK(na_guide_col.is_co_virgin());
	
	qua.set_candidate(*nxt_nmp);
	
	//BRAIN_CK(na_all_propag.is_empty());
	//prop_signal& fst_ps = na_all_propag.inc_sz();
	//fst_ps.init_qua_signal(qua);
	
	long prv_lv = INVALID_LEVEL;
	
	neuromap* pnt_nmp = NULL_PT;
	na_is_head = brn.br_candidate_nxt_nmp_lvs.is_empty();
	if(! na_is_head){
		pnt_nmp = brn.br_candidate_nxt_nmp_lvs.last();
		BRAIN_CK(pnt_nmp != NULL_PT);
		pnt_nmp->na_submap = nxt_nmp;
		
		BRAIN_CK(pnt_nmp->na_orig_cho != NULL_PT);
		if(pnt_nmp->na_orig_cho->is_opp_mono()){
			BRAIN_CK(! na_orig_cho->is_opp_mono());
			pnt_nmp->na_nxt_no_mono = nxt_nmp;
		}
		
		prv_lv = pnt_nmp->na_candidate_tk.tk_level;
	}
	
	if(na_is_head){
		prv_lv = brn.get_lst_cand_lv();
	}
	
	BRAIN_CK(na_candidate_tk.is_tk_virgin());
	na_candidate_tk.tk_level = prv_lv + 1;
	na_candidate_tk.tk_recoil = brn.recoil();
	
	BRAIN_DBG(na_dbg_candidate_tk = na_candidate_tk);
	
	brn.br_candidate_nxt_nmp_lvs.push(nxt_nmp);
	DBG_PRT(106, os << " PUSH_CAND." << brn.br_candidate_nxt_nmp_lvs.size();
		os << "." << nxt_nmp);
	//DBG_PRT(101, os << " PUSH_CAND." << brn.br_candidate_nxt_nmp_lvs.size();
	//	os << nxt_nmp->dbg_na_id();
	
	return pnt_nmp;
}

long
brain::get_lst_cand_lv(){
	long prv_lv = 0;
	neuromap* lst_cand = get_last_cand();
	if(lst_cand != NULL_PT){
		prv_lv = lst_cand->na_candidate_tk.tk_level;
		BRAIN_CK(prv_lv > 0);
	}
	return prv_lv;
}

bool operator != (const prop_signal& ps1, const prop_signal& ps2){
	return (dbg_cmp_ps(ps1, ps2) != 0);
}

void
brain::reset_chg_cands_update(quanton& qua){
	BRAIN_DBG(if(br_dbg.dbg_old_deduc){ return; })
	BRAIN_CK(! br_dbg.dbg_old_deduc);
	
	brain& brn = *this;
	
	if(! qua.has_candidate(brn)){
		return;
	}
	BRAIN_CK(qua.has_charge());
	BRAIN_CK(qua.has_candidate(brn));
	pop_cand_lvs_until(qua);
	
	bool is_eonmp = qua.is_qu_end_of_neuromap(brn);
	if(! is_eonmp){
		return;
	}
	
	if(br_candidate_rsn_lv == INVALID_LEVEL){
		BRAIN_DBG(
			neuromap* nmp = br_candidate_nxt_nmp_lvs.last();
			if(nmp != NULL_PT){
				BRAIN_CK(nmp->nmp_is_cand());
				BRAIN_CK(nmp->na_orig_cho == nmp->na_candidate_qua);
				BRAIN_CK_PRT((nmp->na_orig_lv < qua.qlevel()), 
					os << "\n_________________\n ABORT_DATA\n";
					dbg_prt_margin(os);
					os << " nmp=" << nmp << "\n";
					os << " qua=" << &qua;
				);
			} else {
				neuromap* l_cand = get_last_cand();
				BRAIN_CK(l_cand != NULL_PT);
				BRAIN_CK(l_cand->nmp_is_cand());
				BRAIN_CK(l_cand->na_candidate_qua->qlevel() < qua.qlevel());
			}
		);
		
		return;
	}	
	
	BRAIN_CK(br_candidate_nxt_nmp_lvs.is_empty());
	//BRAIN_CK(qua.has_candidate(brn));
	//pop_cand_lvs_until(qua);
	
	neuromap* lst_cand = get_last_cand();
	BRAIN_CK(lst_cand != NULL_PT);
	BRAIN_CK(lst_cand->na_candidate_qua == &qua);
	
	bool upd_next = (	(br_candidate_nxt_rc < recoil()) && 
						! qua.has_learned_source() &&
						(qua.qlevel() == br_candidate_rsn_lv)
					);
	
	if(upd_next){
		reset_cand_next();
		br_candidate_nxt_rc = recoil();
		br_candidate_next = lst_cand;

		DBG_PRT(101, os << " UPD_NEXT (during reverse).\n";
			os << " " << lst_cand->dbg_na_id() << "\n"; 
			os << " rev_qua=" << &qua << "\n";
			os << " na_orig_cho=" << lst_cand->na_orig_cho; 
			if(lst_cand->na_orig_cho != NULL_PT){
				os << " na_cho_lv=" << lst_cand->na_orig_cho->qlevel(); 
			}
			os << " na_orig_lv=" << lst_cand->na_orig_lv; 
			os << "\n";
			os << " upd_next=" << upd_next << "\n";
			os << " br_candidate_nxt_rc=" << br_candidate_nxt_rc;
			os << " br_rc=" << recoil() << "\n";
			os << " qu_lnd=" << qua.has_learned_source() << "\n";
			os << " qu_lv=" << qua.qlevel();
			os << " br_candidate_rsn_lv=" << br_candidate_rsn_lv;
		);
		
	}
	DBG_PRT(101, os << " POPING_CAND." << br_candidate_nmp_lvs.size();
		neuromap* lst_cand = get_last_cand();
		if(lst_cand != NULL_PT){
			os << lst_cand->dbg_na_id();
		}
		os << " qua=" << &qua << " q_tk=" << qua.qu_candidate_tk;
	);
	
	pop_cand_lv(! upd_next);
}

void
brain::candidates_before_reverse(reason& rsn){
	BRAIN_CK(br_candidate_rsn_lv == INVALID_LEVEL);
	BRAIN_CK(rsn.rs_forced != NULL_PT);
	
	br_candidate_rsn_lv = rsn.rs_target_level + 1;
	BRAIN_CK(br_candidate_rsn_lv != INVALID_LEVEL);

	DBG_PRT(101, os << " CANDS_BEF_REV."; 
		dbg_prt_all_cands(os);
		os << " rsn=" << rsn;
		os << " br_candidate_rsn_lv=" << br_candidate_rsn_lv;
	);
}

void
brain::candidates_after_reverse(){
	BRAIN_CK(br_candidate_rsn_lv != INVALID_LEVEL);
	br_candidate_rsn_lv = INVALID_LEVEL;

	DBG_PRT(101, os << " CANDS_after_rev."; dbg_prt_all_cands(os, true));
	
}

void
brain::pop_all_nxt_cand_lvs(long tg_lv){
	while(! br_candidate_nxt_nmp_lvs.is_empty()){
		neuromap* nmp = br_candidate_nxt_nmp_lvs.last();
		BRAIN_CK(nmp != NULL_PT);
		BRAIN_CK(nmp->nmp_is_cand());
		BRAIN_CK(nmp->na_candidate_qua == nmp->na_orig_cho);
		BRAIN_CK(nmp->na_orig_cho != NULL_PT);
		BRAIN_CK(nmp->na_orig_cho->is_pos());
		
		bool in_tg = (tg_lv != INVALID_LEVEL) && (nmp->na_orig_cho->qlevel() < tg_lv);
		if(in_tg){
			break;
		}
		
		pop_cand_lv_in(br_candidate_nxt_nmp_lvs, true, true);
	}
}

// CALC_VS_CAND

bool
ticket::is_older_than(ticket& nmp_tk){
	if(tk_recoil < nmp_tk.tk_recoil){
		return true;
	}
	if(tk_level < nmp_tk.tk_level){
		return true;
	}
	return false;
}

bool
ticket::is_older_than(neuromap* nmp){
	if(nmp == NULL_PT){
		return false;
	}
	ticket& nmp_tk = nmp->na_candidate_tk;
	return is_older_than(nmp_tk);
}

bool
neuromap::dbg_ck_cand(bool nw_cands){
#ifdef FULL_DEBUG
	neuromap* nmp = this;
	for(long aa = 0; aa < na_propag.size(); aa++){
		quanton* qua = na_propag[aa].ps_quanton;
		BRAIN_CK(qua != NULL_PT);
		
		ticket q_tk = qua->qu_candidate_tk;
		BRAIN_CK(! q_tk.is_tk_virgin());
		BRAIN_CK(! q_tk.is_older_than(nmp));
	}
#endif
return true;
}

void
dbg_reset_all_na_creat_flags(row_neuromap_t& all_nmp){
#ifdef FULL_DEBUG
	for(long aa = 0; aa < all_nmp.size(); aa++){
		BRAIN_CK(all_nmp[aa] != NULL_PT);
		neuromap& nmp = *(all_nmp[aa]);
		BRAIN_CK(nmp.na_dbg_creating);
		nmp.na_dbg_creating = false;
	}
#endif
}

bool
neuron::dbg_ck_min_pos_idx(long min_ti_pos_idx){
#ifdef FULL_DEBUG
	quanton* f_pos = dbg_find_first_pos();
	bool ck_m_idx = (	(f_pos != NULL_PT) ==
						ne_fibres.is_valid_idx(min_ti_pos_idx));
	BRAIN_CK(ck_m_idx);
	if(f_pos != NULL_PT){
		BRAIN_CK(ne_fibres.is_valid_idx(min_ti_pos_idx));
		BRAIN_CK(ne_fibres[min_ti_pos_idx] == f_pos);
	}
#endif
	return true;
}

void
brain::dbg_prt_cand_info(bj_ostream& os, neuron& neu){
#ifdef FULL_DEBUG
	brain& brn = *this;
	ticket& src_tk = neu.ne_candidate_tk;
	DBG_PRT_ABORT(brn);
	os << "\n";
	os << " neu="<< &neu << "\n";
	neuromap* lst_cand = get_last_cand();
	os << " rnd=" << br_round << "\n";
	os << " prv_rc=" << br_prv_round_last_rc << "\n";
	os << " src_tk=" << src_tk << "\n";
	if(lst_cand != NULL_PT){
		os << " na_idx=" << lst_cand->na_index << "\n";
		os << " na_candidate_tk=" << lst_cand->na_candidate_tk;
		os << " in_rnd=" << in_current_round(lst_cand->na_candidate_tk) << "\n";
	}
	os << " candida_tk=" << neu.ne_candidate_tk;
	os << " in_rnd=" << in_current_round(neu.ne_candidate_tk) << "\n";
#endif
}

void
brain::set_chg_cands_update(quanton& qua){
	BRAIN_DBG(if(br_dbg.dbg_old_deduc){ return; })
	BRAIN_CK(! br_dbg.dbg_old_deduc);
	
	brain& brn = *this;
	BRAIN_CK(qua.is_pos());
	BRAIN_DBG(ticket& q_tk = qua.qu_candidate_tk);
	
	BRAIN_DBG(
		long dbg_trl_sz = -1;
		MARK_USED(dbg_trl_sz);
	);
	DBG_COMMAND(106, dbg_trl_sz = br_charge_trail.get_tot_quantons());
	DBG_PRT(106, os << " upd_cands. QUA=" << &qua << " c-tk=" << q_tk << " rc=" << recoil()
		<< " dbg_trl_sz=" << dbg_trl_sz;
	);
	
	BRAIN_CK_PRT((qua.qu_candidate_nmp == NULL_PT), 
		DBG_PRT_ABORT(brn);
		brn.print_trail(os);
		os << " qua=" << &qua << "\n";
		os << " nmp=" << qua.qu_candidate_nmp << "\n";
	);
	
	qua.make_qu_dominated(brn);

	if(qua.has_source()){
		neuron* src = qua.get_source();
		BRAIN_CK(src != NULL_PT);
		src->make_ne_dominated(brn);
	}

	if(qua.qlevel() == ROOT_LEVEL){
		use_next_cand(qua);
		return;
	}

	if(qua.is_qu_end_of_neuromap(brn)){
		if(qua.has_learned_source()){
			BRAIN_CK(br_candidate_nxt_nmp_lvs.is_empty());
			use_next_cand(qua);
			DBG_PRT(106, os << " upd_cands_LRN_eonmp");
			
		} else {
			neuromap& nxt_nmp = brn.locate_neuromap();
			neuromap* pnt_nmp = nxt_nmp.nmp_init_with(qua);
			if(pnt_nmp != NULL_PT){
				init_cand_propag(*pnt_nmp, &qua);
			}
			
			DBG_PRT(53, os << " LOCATED nmp=" << nxt_nmp.dbg_na_id() << "\n";
				os << " phi_id=" << nxt_nmp.na_dbg_phi_id << "\n";
			);
			DBG_PRT(106, os << " upd_cands_CHO_eonmp nmp=" << nxt_nmp.dbg_na_id());
		}
	}
	
	DBG_PRT(106, os << " end_upd_cands. QUA=" << &qua << " c-tk=" << q_tk;
		os << " rc=" << recoil();
		os << " dbg_trl_sz=" << dbg_trl_sz;
		os << "\nlst_cand=\n" << get_last_cand(dbg_call_3);
	);
	BRAIN_CK(! q_tk.is_older_than(get_last_cand(dbg_call_4)));
}

bool
brain::dbg_ck_candidates(bool nw_cands){
#ifdef FULL_DEBUG
	row_neuromap_t& all_cand = br_candidate_nmp_lvs;
	for(long aa = 0; aa < all_cand.size(); aa++){
		neuromap* nmp = all_cand[aa];
		BRAIN_CK(nmp != NULL_PT);
		BRAIN_CK(nmp->nmp_is_cand());
		BRAIN_CK(nmp->dbg_ck_cand(nw_cands));
		if(aa == 0){
			continue;
		}
		neuromap* prv = all_cand[aa - 1];
		BRAIN_CK(prv != NULL_PT);
		
		ticket& n_tk = nmp->na_candidate_tk;
		BRAIN_CK(! n_tk.is_tk_virgin());

		BRAIN_CK(! n_tk.is_older_than(prv));
	}
#endif
return true;
}

bool
quanton::is_qu_dominated(brain& brn){
	bool is_dom = ! qu_candidate_tk.is_older_than(brn.get_last_cand());
	return is_dom;
}

void
quanton::make_qu_dominated(brain& brn){
	ticket& q_tk = qu_candidate_tk;
	while(q_tk.is_older_than(brn.get_last_cand(dbg_call_2))){
		BRAIN_DBG(neuromap* lst_cand = brn.get_last_cand(dbg_call_3));
		DBG_PRT(101, os << " POPING_BAD_CAND." << brn.br_candidate_nmp_lvs.size();
			if(lst_cand != NULL_PT){
				os << lst_cand->dbg_na_id();
			}
			os << " qua=" << this << " q_tk=" << q_tk;
		);
		DBG_PRT(106, os << " POP_CAND.";
			os << brn.br_candidate_nmp_lvs.size();
			if(lst_cand != NULL_PT){
				os << " na_idx=" << lst_cand->na_index;
			}
			os << " qua=" << this << " q_tk=" << q_tk;
		);
		brn.pop_cand_lv(true);
	}
	BRAIN_CK(! q_tk.is_older_than(brn.get_last_cand(dbg_call_4)));
}

bool
neuron::is_ne_dominated(brain& brn){
	if(! ne_original){
		return true;
	}
	bool is_dom = ! ne_candidate_tk.is_older_than(brn.get_last_cand());
	return is_dom;
}

void
neuron::make_ne_dominated(brain& brn){
	if(! ne_original){
		return;
	}
	
	ticket& src_tk = ne_candidate_tk;
	while(src_tk.is_older_than(brn.get_last_cand())){
		BRAIN_DBG(neuromap* lst_cand = brn.get_last_cand());
		BRAIN_CK(lst_cand != NULL_PT);
		DBG_PRT(106, os << " POP_CAND_BY_SRC.";
			os << brn.br_candidate_nmp_lvs.size();
			os << " na_idx=" << lst_cand->na_index;
			os << " src=" << this << " src_tk=" << src_tk << "\n";
			brn.print_trail(os);
			os << "\n";
			lst_cand->map_dbg_update_html_file("POP_BY_SRC");
		);
		brn.pop_cand_lv(true);
	}
	BRAIN_CK_PRT((! src_tk.is_older_than(brn.get_last_cand())), 
		brn.dbg_prt_cand_info(os, *this)
	);
}

bool
quanton::is_smaller_source(neuron& neu, long qti){
	bool can_upd = neu.ne_original && is_pos() && has_source() && (qti == qu_tier);
	if(! can_upd){
		return false;
	}
	
	bool is_smlr = (neu.fib_sz() < get_source()->fib_sz());
	if(! is_smlr){
		return false;
	}
	BRAIN_CK(get_source()->ne_original);
	
	return true;
}

void
quanton::update_source(brain& brn, neuron& neu){
	neuron* old_src = get_source();
	neuromap* curr_nmp = get_candidate_to_fill(brn);
	
	if(curr_nmp != NULL_PT){
		curr_nmp->na_all_cov.push(old_src);
	}
	set_source(&neu);
	neu.make_ne_dominated(brn);
}

bool
neuron::is_ne_source(){
	quanton* q0 = ne_fibres[0];
	BRAIN_CK(q0 != NULL_PT);
	bool is_src = q0->is_pos() && (q0->qu_source == this);
	return is_src;
}

bool
brain::ck_write_quas(reason& rsn){
#ifdef FULL_DEBUG
	row_quanton_t& wrt_quas = rsn.rs_motives;
	brain& brn = *this;
	for(long aa = 0; aa < wrt_quas.size(); aa++){
		quanton* qua = wrt_quas[aa];
		BRAIN_CK(qua != NULL_PT);
		BRAIN_CK(qua->has_charge());
		BRAIN_CK_PRT((! qua->has_source() || ! qua->get_source()->is_ne_to_wrt()), 
			DBG_PRT_ABORT(brn);
			os << " qua=" << qua << "\n";
			os << " src=" << qua->get_source() << "\n";
			os << " qu_lv=" << qua->qlevel() << "\n";
			os << " br_lv=" << level() << "\n";
			os << " rsn=" << rsn << "\n";
		);
		BRAIN_CK(qua->is_qu_to_upd_wrt_tk());
	}
#endif
	return true;
}

long
brain::get_min_trainable_num_sub(){
	long min_sub = MIN_TRAINABLE_NUM_SUB;
	DBG_COMMAND(7, 
		if(! dbg_as_release()){
			min_sub = br_dbg_min_trainable_num_sub;
		}
	);
	return min_sub;
}

void
neuron::set_cand_tk(ticket& n_tk){
	ne_candidate_tk = n_tk;
}

void
neuron::neu_tunnel_signals(brain& brn, quanton& r_qua){
	BRAIN_CK(fib_sz() >= 2);
	BRAIN_CK(! is_ne_virgin());
	BRAIN_CK(brn.br_dbg_keeping_learned || ne_original || 
		(brn.br_dbg.dbg_last_recoil_lv == brn.level())
	);
	
	quanton* qua = &r_qua;
	BRAIN_CK(qua->get_charge() != cg_neutral);
	DBG_PRT(17, os << "tunneling " << qua << " in " << this);
	BRAIN_CK(! ne_fibres.is_empty());
	BRAIN_CK(ne_fibres.size() > 1);
	BRAIN_CK(ck_tunnels());

	bool q_neg = (qua->is_neg());
	charge_t chg_op2 = cg_positive;

	if((q_neg && (ne_fibres[0] == qua)) || (! q_neg && (ne_fibres[1] == qua))){
		swap_fibres_0_1();
		BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
		BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
	}
	BRAIN_DBG(
		if(q_neg){
			BRAIN_CK(ne_fibres[1] == qua);
			BRAIN_CK(fib1().is_neg());
		} else {
			BRAIN_CK(ne_fibres[0] == qua);
			BRAIN_CK(fib0().is_pos());
		}
	);

	charge_t chg0 = ne_fibres[0]->get_charge();
	charge_t chg1 = ne_fibres[1]->get_charge();
	bool been_forced = (! q_neg && ne_fibres[1]->is_neg());
	long min_ti_pos_idx = INVALID_IDX;
	long max_lv_idx = INVALID_IDX;
	
	max_lv_idx = get_max_lv_idx(max_lv_idx, 0);
	max_lv_idx = get_max_lv_idx(max_lv_idx, 1);

	if((chg0 != cg_neutral) && (chg1 != cg_neutral)){
		BRAIN_CK(fib0().has_charge());
		BRAIN_CK(fib1().has_charge());
		
		if(! been_forced){
			chg_op2 = cg_neutral;
		}
		if(chg0 == cg_positive){ min_ti_pos_idx = get_min_ti_idx(min_ti_pos_idx, 0); }
		if(chg1 == cg_positive){ min_ti_pos_idx = get_min_ti_idx(min_ti_pos_idx, 1); }
	}
	BRAIN_DBG(
		if(been_forced){
			// INVARIANT_1: 
			// r_qua (fib0) was forced and is been charged now. so:
			BRAIN_CK(fib0().is_pos());
			BRAIN_CK(ck_all_neg(1));
		}
	);

	long old_max = fib_sz() - 1;
	long max_tier = INVALID_TIER;

	if(q_neg || (chg_op2 == cg_neutral)){
		BRAIN_CK(! been_forced);
		BRAIN_DBG(
			long new_max = old_max;
			long max_lev = ne_fibres[new_max]->qlevel();
			long qua_lev = qua->qlevel();
			bool hf_pos = false;
		);

		long ii = old_max;
		for(ii = old_max; ii > 1; ii--){
			charge_t fib_chg = ne_fibres[ii]->get_charge();

			max_lv_idx = get_max_lv_idx(max_lv_idx, ii);
			max_tier = max_val(max_tier, ne_fibres[ii]->qu_tier);

			bool f_pos = (chg_op2 == cg_neutral) && (fib_chg == cg_positive);
			if(f_pos){
				BRAIN_CK(fib0().has_charge());
				BRAIN_CK(fib1().has_charge());
				BRAIN_CK(q_neg || fib0().is_pos());
				BRAIN_CK(q_neg || fib1().is_pos()); // BECAUSE INVARIANT_1 !!

				min_ti_pos_idx = get_min_ti_idx(min_ti_pos_idx, ii);
				
				if(q_neg){
					BRAIN_REL_CK(ne_fibres[1]->is_neg());
					BRAIN_CK(fib1().is_neg());
					BRAIN_CK(! fib0().has_charge() != fib0().is_pos());
					
					neu_swap_edge(brn, ii, max_lv_idx); // will be pos1 if nil is not found.
					if(min_ti_pos_idx == ii){ min_ti_pos_idx = 1; }
					
					BRAIN_CK(fib1().is_pos());  
					BRAIN_DBG(hf_pos = true);
				}
			}

			if((fib_chg == cg_neutral) || (fib_chg == chg_op2)){
				BRAIN_CK(! f_pos);
				BRAIN_CK(fib_chg != cg_negative);
				BRAIN_CK(max_lev <= qua_lev);

				if(q_neg){
					BRAIN_CK(hf_pos || fib1().is_neg());
					neu_swap_edge(brn, ii, max_lv_idx);
					BRAIN_CK(! fib1().is_neg());
				} else {
					BRAIN_CK(chg_op2 == cg_neutral);
					BRAIN_CK(fib0().is_pos()); 
					BRAIN_CK(fib1().is_pos()); // BECAUSE INVARIANT_1 !!
					swap_fibres_0(ii, max_lv_idx);
					BRAIN_CK(fib0().is_nil());
				}

				BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
				BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
				BRAIN_CK_2(ne_fibres[ii]->ck_all_tunnels());
				
				break;
			}
			BRAIN_DBG(
				if(ne_fibres[ii]->qlevel() > max_lev){
					new_max = ii;
					max_lev = ne_fibres[new_max]->qlevel();
				}
			);

		}
	}

	charge_t cg0 = ne_fibres[0]->get_charge();
	charge_t cg1 = ne_fibres[1]->get_charge();

	bool dbg1 = false;
	bool dbg2 = false;
	MARK_USED(dbg1);
	MARK_USED(dbg2);
	if(q_neg && (cg1 == cg_negative)){
		BRAIN_CK(ck_all_neg(1));

		for(long aa = fib_sz() - 1; aa > old_max; aa--){
			BRAIN_CK(false);
			max_tier = max_val(max_tier, ne_fibres[aa]->qu_tier);
		}

		max_tier = max_val(max_tier, ne_fibres[1]->qu_tier);
		
		BRAIN_CK(max_tier == find_max_tier(ne_fibres, 1));

		quanton* forced_qua = forced_quanton();
		BRAIN_CK(forced_qua != NULL_PT);

		brn.send_psignal(*forced_qua, this, max_tier + 1);

		BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
		BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
		dbg1 = true;

		DBG_PRT(31, os << "forcing " << this);
	}

	if((cg0 != cg_neutral) && (cg1 != cg_neutral)){
		// ALL_FILLED code:
		BRAIN_CK(ne_fibres.is_valid_idx(max_lv_idx));
		BRAIN_CK(ck_all_has_charge(max_lv_idx));
		
		if(max_lv_idx > 1){
			long m_idx = get_max_lv_idx(0, max_lv_idx);
			BRAIN_CK((m_idx == max_lv_idx) || 
				(ne_fibres[0]->qlevel() == ne_fibres[max_lv_idx]->qlevel()));
			if(m_idx == max_lv_idx){
				BRAIN_CK(! been_forced);
				BRAIN_CK(! is_ne_source());
				if(min_ti_pos_idx == 0){
					swap_fibres_0_1();
					min_ti_pos_idx = 1;
				}
				swap_fibres_0(max_lv_idx, max_lv_idx);
				BRAIN_CK(max_lv_idx == 0);
			}
		}
		
		if(ne_original){
			BRAIN_DBG(r_qua.qu_dbg_num_fill_by_qua++);
			BRAIN_CK(dbg_ck_min_pos_idx(min_ti_pos_idx));
			
			quanton* min_pos_qua = &r_qua;
			bool has_min = ne_fibres.is_valid_idx(min_ti_pos_idx);
			if(has_min){
				min_pos_qua = ne_fibres[min_ti_pos_idx];
			}
			
			BRAIN_CK(min_pos_qua != NULL_PT);
			neuromap* creat_cand = min_pos_qua->get_candidate_to_fill(brn);
			if(creat_cand != NULL_PT){
				update_create_cand(brn, r_qua, *creat_cand, been_forced);
			}
			DBG_PRT(32, os << "qua=" << &r_qua << " filled orig " << this);
		}
		dbg2 = true;
	}

	BRAIN_CK(! q_neg || dbg1 || ! ck_all_neg(1));
	BRAIN_CK(dbg2 || ! ck_all_has_charge(INVALID_IDX));
}

comparison
brain::select_propag_side(bool cnfl1, long sz1, row_long_t& all_sz1, 
						bool cnfl2, long sz2, row_long_t& all_sz2)
{
	if(cnfl1 && ! cnfl2){
		return -1;
	}
	if(! cnfl1 && cnfl2){
		return 1;
	}
	/*if(sz1 > sz2){
		return -1;
	}
	if(sz1 < sz2){
		return 1;
	}*/
	long resp = 0;
	long ii = 0;
	while(true){
		bool ok1 = all_sz1.is_valid_idx(ii);
		bool ok2 = all_sz2.is_valid_idx(ii);
		if(! ok1){
			resp = 1;
			break;
		}
		if(! ok2){
			resp = -1;
			break;
		}
		long v1 = all_sz1[ii];
		long v2 = all_sz2[ii];
		if(v1 != v2){
			resp = cmp_long(v1, v2);
			break;
		}
		ii++;
	}
	return resp;
}

void // NEW
brain::start_propagation(quanton& nxt_qua){ // NEW
	BRAIN_CK(get_leveldat().ld_idx == level());
	
	quanton& qua = nxt_qua;
	
	BRAIN_CK(! qua.is_mono() || qua.is_opp_mono());

	inc_level(qua);

	BRAIN_DBG(long prv_lv = level());
	BRAIN_CK(get_leveldat().ld_chosen == &qua);

	BRAIN_CK(! has_psignals());
	send_psignal(qua, NULL, tier() + 1);
	BRAIN_CK(has_psignals());

	//return; // debug purposes
	if(qua.is_mono() || qua.is_opp_mono()){
		return;
	}

	row_quanton_t& all_impl_cho = br_tmp_all_impl_cho;
	all_impl_cho.clear();
	
	quanton& opp = qua.opposite();
	
	row_long_t& all_sz1 = br_tmp_all_sz_side1;
	row_long_t& all_sz2 = br_tmp_all_sz_side2;
	
	all_sz1.clear();
	all_sz2.clear();

	brn_tunnel_signals(true, all_impl_cho);
	//update_monos();
	//BRAIN_CK(ck_mono_propag());
	BRAIN_CK(all_impl_cho.is_empty());
	BRAIN_CK(curr_cho() == &qua);
	BRAIN_CK(! get_leveldat().has_learned());
	long sz1 = get_last_lv_all_trail_sz(all_sz1);
	bool cnfl1 = found_conflict();

	replace_choice(qua, opp, dbg_call_1);
	BRAIN_CK(prv_lv == level());

	brn_tunnel_signals(true, all_impl_cho);
	//update_monos();
	//BRAIN_CK(ck_mono_propag());
	BRAIN_CK(all_impl_cho.is_empty());
	BRAIN_CK(curr_cho() == &opp);
	BRAIN_CK(! get_leveldat().has_learned());
	long sz2 = get_last_lv_all_trail_sz(all_sz2);
	bool cnfl2 = found_conflict();
	
	comparison nxt_side = select_propag_side(cnfl1, sz1, all_sz1, cnfl2, sz2, all_sz2);
	
	if(nxt_side < 0){
		replace_choice(opp, qua, dbg_call_2);
	}
	
	BRAIN_CK(prv_lv == level());
}

void
brain::init_cand_propag(neuromap& nmp, quanton* cur_qua){
	quanton* nmp_cho = nmp.na_orig_cho;
	BRAIN_CK(nmp_cho != NULL_PT);
	BRAIN_CK(nmp_cho->is_pos());
	
	qlayers_ref qlr;
	qlr.init_qlayers_ref(&(br_charge_trail));
	qlr.reset_curr_quanton();
	
	quanton* prop_qu = qlr.get_curr_quanton();
	BRAIN_CK(prop_qu != NULL_PT);
	
	if(cur_qua != NULL_PT){
		BRAIN_CK(cur_qua->is_pos());
		BRAIN_CK(prop_qu == cur_qua);
		prop_qu = qlr.dec_curr_quanton();
	}
	
	if(! nmp.na_propag.is_empty()){
		BRAIN_DBG(
			for(long aa = 0; aa < nmp.na_propag.size(); aa++){
				BRAIN_CK(prop_qu != NULL_PT);
				prop_signal aux_ps;
				aux_ps.init_qua_signal(*prop_qu);
				BRAIN_CK(aux_ps.equal_ps_to(nmp.na_propag[aa]));
				prop_qu = qlr.dec_curr_quanton();
			}
		);
		return;
	}
	
	BRAIN_CK_PRT((nmp.na_propag.is_empty()), 
		os << "\n_________________\n ABORT_DATA\n";
		dbg_prt_margin(os);
		os << nmp.dbg_na_id();
	);
	
	while((prop_qu != NULL_PT) && (prop_qu != nmp_cho)){
		prop_signal& nxt_ps = nmp.na_propag.inc_sz();
		nxt_ps.init_qua_signal(*prop_qu);
		
		prop_qu = qlr.dec_curr_quanton();
	}
	
	BRAIN_CK((prop_qu == NULL_PT) || (prop_qu == nmp_cho));
	if(prop_qu == nmp_cho){
		prop_signal& nxt_ps = nmp.na_propag.inc_sz();
		nxt_ps.init_qua_signal(*nmp_cho);
	}
	
	/*
	BRAIN_DBG(
		brain& brn = *this;
		row<prop_signal>& all_dbg_p = brn.br_dbg_propag;
		all_dbg_p.clear(true, true);
		nmp.na_all_propag.append_to(all_dbg_p, 0 , -1 , true);
	);
	//BRAIN_CK((nmp.na_propag.equal_to_diff(dbg_cmp_ps, all_dbg_p) == INVALID_IDX) ==
	//	nmp.na_propag.equal_to(all_dbg_p)); // OK.
	//BRAIN_CK_PRT(nmp.na_propag.equal_to(all_dbg_p)); // NOT ok.
	*/
}

ch_string
ticket::get_str(){
	bj_ostr_stream the_str;
	BRAIN_CK(tk_recoil >= 0);
	
	long lv_val = tk_level;
	ch_string inv_val = "";
	if(tk_level < 0){
		BRAIN_CK(tk_level == INVALID_LEVEL);
		lv_val = 0;
		inv_val = "IL";
	}
	
	the_str << "rc_" << tk_recoil << "_lv_" << inv_val << lv_val;
	return the_str.str();
}

bool
reason::is_root_confl(){
	if(! is_dt_singleton()){
		return false;
	}
	if(rs_forced == NULL_PT){
		return false;
	}
	if(! rs_forced->is_neg()){
		return false;
	}
	if(! rs_forced->in_root_qlv()){
		return false;
	}
	return true;
}

bool
deduction::can_go_on(){
	bool gon = (! dt_found_top && ! dt_rsn.is_root_confl());
	return gon;
}

