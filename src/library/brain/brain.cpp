

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
#include "dbg_run_satex.h"
#include "dbg_prt.h"
//include "dbg_ic.h"
#include "dbg_config.h"
#include "strings_html.h"

//br_qu_tot_note0;

DEFINE_NI_FLAG_FUNCS(qu_flags, note0, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note1, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note2, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note3, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note4, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note5, true);

DEFINE_NI_FLAG_ALL_FUNCS(note0);
DEFINE_NI_FLAG_ALL_FUNCS(note1);
DEFINE_NI_FLAG_ALL_FUNCS(note2);
DEFINE_NI_FLAG_ALL_FUNCS(note3);
DEFINE_NI_FLAG_ALL_FUNCS(note4);
DEFINE_NI_FLAG_ALL_FUNCS(note5);


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

DEFINE_NA_FLAG_FUNCS(na_flags, tags0_n_notes0);
DEFINE_NA_FLAG_FUNCS(na_flags, tags1_n_notes1);
DEFINE_NA_FLAG_FUNCS(na_flags, tags2_n_notes2);
DEFINE_NA_FLAG_FUNCS(na_flags, tags3_n_notes3);
DEFINE_NA_FLAG_FUNCS(na_flags, tags4_n_notes4);
DEFINE_NA_FLAG_FUNCS(na_flags, tags5_n_notes5);

#define PRINT_PERIOD			4.0
#define SOLVING_TIMEOUT			0.0		// 0.0 if no timeout

//============================================================
// static vars

char*	alert_rel::CL_NAME = as_pt_char("{alert_rel}");
char*	quanton::CL_NAME = as_pt_char("{quanton}");
char*	neuron::CL_NAME = as_pt_char("{neuron}");

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
// check SAT result

long	set_dots_of(brain& brn, row_quanton_t& quans){
	long num_qua = 0;
	for(long ii = 0; ii < quans.size(); ii++){
		quanton* qua = quans[ii];
		BRAIN_CK(qua != &(brn.br_top_block));
		BRAIN_CK_0(! qua->has_dot());
		if(! qua->has_dot()){
			qua->set_dot(brn);
			num_qua++;
		}
	}
	return num_qua;
}

long	reset_dots_of(brain& brn, row_quanton_t& quans){
	long resetted = 0;
	for(long ii = 0; ii < quans.size(); ii++){
		quanton* qua = quans[ii];
		BRAIN_CK(qua != &(brn.br_top_block));
		if(qua->has_dot()){
			qua->reset_dot(brn);
			resetted++;
		}
	}
	return resetted;
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
			//long qtrl_sz = INVALID_IDX;
			//long lst_trl_sz = INVALID_IDX;
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
				//qtrl_sz = qua->qu_charge_tk.tk_trail_sz;
				//BRAIN_CK((lst_trl_sz == INVALID_IDX) || (lst_trl_sz > qtrl_sz));
				//lst_trl_sz = qtrl_sz;
			);

			charge_t q_chg = qua->get_charge();

			if(q_chg == cg_negative){
				num_neg_chgs++;
			} 
			if(q_chg == cg_neutral){
				num_neutral++;
			}

			if(orig){
				qua->qu_neus.push(neu);
			}
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
neuron::neu_swap_edge(brain& brn, long ii){
	charge_t qua_chg = ne_fibres[1]->get_charge();

	swap_fibres_1(ii);

	if(qua_chg == cg_negative){
		ne_edge = ii;
		ne_edge_tk.update_ticket(brn);

		BRAIN_CK(is_ticket_eq(ne_edge_tk, brn.br_current_ticket));
		//BRAIN_CK(ne_edge_tk.is_active(brn));

		BRAIN_CK(ne_fibres[ii]->is_neg());
	}
}

void
neuron::neu_tunnel_signals(brain& brn, quanton& r_qua){
	BRAIN_CK(fib_sz() >= 2);
	BRAIN_CK(! is_ne_virgin());
	BRAIN_CK(ne_original || (brn.br_dbg.dbg_last_recoil_lv == brn.level()));

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
	BRAIN_CK(! q_neg || (ne_fibres[1] == qua));
	BRAIN_CK(q_neg || (ne_fibres[0] == qua));

	charge_t chg0 = ne_fibres[0]->get_charge();
	charge_t chg1 = ne_fibres[1]->get_charge();

	if((chg0 != cg_neutral) && (chg1 != cg_neutral)){
		chg_op2 = cg_neutral;
	}

	long old_max = fib_sz() - 1;
	long max_tier = INVALID_TIER;

	if(q_neg || (chg_op2 == cg_neutral)){

		BRAIN_DBG(
			long new_max = old_max;
			long max_lev = ne_fibres[new_max]->qlevel();
			long qua_lev = qua->qlevel()
		);

		long ii = old_max;
		for(ii = old_max; ii > 1; ii--){
			charge_t fib_chg = ne_fibres[ii]->get_charge();

			max_tier = max_val(max_tier, ne_fibres[ii]->qu_tier);

			bool f_pos = (chg_op2 == cg_neutral) && (fib_chg == cg_positive);
			if(f_pos){
				charge_t chag1 = ne_fibres[1]->get_charge();
				bool neg1 = (chag1 == cg_negative);
				if(q_neg && neg1){
					neu_swap_edge(brn, ii);
				}
				if(! q_neg){
					charge_t chag0 = ne_fibres[0]->get_charge();
					bool pos0 = (chag0 == cg_positive);
					bool neg0 = (chag0 == cg_negative);
					if(neg0){ swap_fibres_0(ii); }
					if(pos0 && neg1){ neu_swap_edge(brn, ii); }
				}
			}

			if((fib_chg == cg_neutral) || (fib_chg == chg_op2)){
				BRAIN_CK((chg_op2 != cg_positive) || (fib_chg != cg_negative));
				BRAIN_CK(max_lev <= qua_lev);

				if(q_neg){
					neu_swap_edge(brn, ii);
				} else {
					swap_fibres_0(ii);
				}

				BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
				BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
				BRAIN_CK_2(ne_fibres[ii]->ck_all_tunnels());

				break;	// neu_is_satisf
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
		BRAIN_CK(ck_all_charges(&brn, 1));

		for(long aa = fib_sz() - 1; aa > old_max; aa--){
			max_tier = max_val(max_tier, ne_fibres[aa]->qu_tier);
		}

		max_tier = max_val(max_tier, ne_fibres[1]->qu_tier);

		quanton* forced_qua = forced_quanton();
		BRAIN_CK(forced_qua != NULL_PT);

		brn.send_psignal(*forced_qua, this, max_tier + 1);

		BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
		BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
		dbg1 = true;

		DBG_PRT(102, os << "forcing " << this);
	}

	if((cg0 != cg_neutral) && (cg1 != cg_neutral)){
		BRAIN_DBG(long npos = 0);
		BRAIN_CK(ck_all_has_charge(npos));
		//if(ne_original && (cg0 == cg_positive) && (cg1 == cg_positive)){
		if(ne_original){
			BRAIN_DBG(ne_dbg_filled_tk.update_ticket(brn));
			r_qua.qu_full_charged.push(this);
			DBG_PRT(102, os << "qua=" << &r_qua << " filled orig " << this);
		}
		dbg2 = true;
	}

	update_uncharged(brn, &r_qua);

	BRAIN_CK(! q_neg || dbg1 || ! ck_all_charges(&brn, 1));
	BRAIN_DBG(long npos2 = 0; bool ck2 = ck_all_has_charge(npos2); );
	BRAIN_CK(dbg2 || ! ck2);
}


//============================================================
// brain methods

brain::brain(solver& ss) {
	init_brain(ss);
}

brain::~brain(){
	DBG_PRT(6, os << "releasing brain 0");

	release_brain();

	DBG_PRT(6, os << "RELEASING brain 1");
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
	
	BRAIN_DBG(br_pt_brn = NULL);
	br_pt_slvr = &ss;
	
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
	br_choice_spin = cg_neutral;
	br_choice_order = k_invalid_order;
	
	br_last_retract = false;
	
	br_last_monocho_idx = 0;
	
	//br_choices_lim = INVALID_IDX;
	br_charge_trail.init_qulayers(this);

	br_first_psignal = 0;
	br_last_psignal = 0;

	br_dbg_retract_nke0.init_notekeeper(this);
	br_dbg_retract_nke0.init_funcs(&br_qu_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0,
								&append_all_not_note0, &same_quantons_note0
  							);
	
	br_dbg_full_col.init_coloring();

	br_deducer_anlsr.init_analyser(this);
	br_neuromaper_anlsr.init_analyser(this);
	
	br_deducer_anlsr.init_nk_with_note0(br_deducer_anlsr.de_nkpr, brn);
	br_neuromaper_anlsr.init_nk_with_note5(br_neuromaper_anlsr.de_nkpr, brn);
	
	reset_conflict();

	br_num_memo = 0;

	br_top_block.init_quanton(this, cg_neutral, INVALID_IDX, NULL);
	
	DBG_CK(br_top_block.qu_tee.is_alone());

	//br_tot_qu_spot0 = 0;
	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	br_num_active_neurons = 0;
	br_num_active_alert_rels = 0;
	br_num_active_neuromaps = 0;

	BRAIN_DBG(
		init_all_dbg_brn();  // sets br_pt_brn indicating it is readi for DBG_PRT
		dbg_init_dbg_conf(br_dbg.dbg_conf_info);
	);
	BRAIN_CK(br_dbg.dbg_tot_nmps == 0);
}

void
brain::release_all_sortors(){
	br_forced_srg.release_all();
	br_filled_srg.release_all();

	br_guide_neus_srg.release_all();
	br_guide_quas_srg.release_all();

	br_compl_neus_srg.release_all();
	br_compl_quas_srg.release_all();
	
	br_tauto_neus_srg.release_all();
	br_tauto_quas_srg.release_all();
}

void
brain::release_brain(){
	br_last_retract = true;
	
	BRAIN_DBG(br_pt_brn = NULL);
	get_skeleton().set_dbg_brn(NULL);

	bool has_chgs = (br_charge_trail.get_tot_quantons() > 0);
	if(has_chgs){
		retract_to(INVALID_LEVEL);
	}
	BRAIN_CK(br_charge_trail.get_tot_quantons() == 0);
	if(level() != ROOT_LEVEL){
		retract_to();
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
	br_tmp_edge.clear(false, true);

	// state attributes
	//satisfying.clear(true, true);
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

	BRAIN_CK(br_tot_qu_dots == 0);

	BRAIN_CK(br_dbg.dbg_tot_nmps == 0);
	BRAIN_CK(br_num_active_neuromaps == 0);
}

void
quanton::qua_tunnel_signals(brain* brn){
	BRAIN_CK(get_charge() != cg_neutral);
	BRAIN_CK(qlevel() != INVALID_LEVEL);

	for(long ii = qu_tunnels.size() - 1; (ii >= 0); ii--){
		BRAIN_CK(qu_tunnels[ii] != NULL);
		neuron& neu = *(qu_tunnels[ii]);
		neu.neu_tunnel_signals(*brn, *this);
	} // end for (ii)
	BRAIN_CK_2(ck_all_tunnels());
}

void		
quanton::set_charge(brain& brn, neuron* neu, charge_t cha, long n_tier){
	BRAIN_CK(ck_charge(brn));
	BRAIN_CK(this != &(brn.br_top_block));

	BRAIN_CK((cha == cg_positive) || (cha == cg_neutral));
	BRAIN_CK(! is_neg());
	BRAIN_CK(! is_pos() || (cha == cg_neutral));
	BRAIN_CK(has_charge() || (cha == cg_positive));

	BRAIN_DBG(bool is_ending = brn.br_last_retract);
	bool with_src_before = has_source();

	qu_charge = cha;
	qu_inverse->qu_charge = negate_trinary(cha);

	set_source(brn, neu);

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

		qu_charge_tk.tk_recoil = brn.recoil();

		qu_full_charged.clear();
		qu_inverse->qu_full_charged.clear();

	} else {
		BRAIN_CK(qu_full_charged.is_empty());
		BRAIN_CK(qu_inverse->qu_full_charged.is_empty());

		quanton& mot = *this;
		brn.br_charge_trail.add_motive(mot, n_tier);

		qu_charge_tk.update_ticket(brn);
		qu_inverse->qu_charge_tk.update_ticket(brn);

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
}

long
brain::brn_tunnel_signals(bool only_in_dom){
	brain& brn = *this;
	MARK_USED(brn);
	BRAIN_CK(br_ne_tot_tag0 == 0);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK(br_shadow_quas.is_empty());
	BRAIN_CK(! found_conflict());
	BRAIN_CK(ck_trail());
	BRAIN_DBG(
		ticket tk1;
		tk1.update_ticket(brn);
		BRAIN_CK(! found_conflict());
	);

	long num_psigs = 0;

	while(has_psignals()){
		quanton* qua = receive_psignal(only_in_dom);
		if(qua == NULL_PT){
			continue;
		}

		quanton* inv = qua->qu_inverse;
		BRAIN_CK_0(inv != NULL_PT);

		BRAIN_CK(qua->get_charge() == cg_positive);
		BRAIN_CK(inv->get_charge() == cg_negative);

		qua->qua_tunnel_signals(this);
		inv->qua_tunnel_signals(this);

		DBG_PRT(19, os << "finished tunnelling " << inv);
	
		num_psigs++;
	} // end while
	
	BRAIN_CK_0(! has_psignals());
	reset_psignals();
	
	reset_all_note0(brn, br_shadow_quas);
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
		tk2.update_ticket(brn);
		BRAIN_CK_0(is_ticket_eq(tk1, tk2));
	);

	return num_psigs;
}

quanton*	
brain::choose_quanton(){ 
	BRAIN_CK(ck_trail());

	//quanton* qua = NULL;
	quanton* qua = choose_mono();
	if(qua != NULL_PT){
		return qua;
	}

	while(! br_maps_active.is_empty()){
		neuromap* pt_mpp = br_maps_active.last();
		BRAIN_CK(pt_mpp != NULL_PT);
		neuromap& mpp_dom = *pt_mpp;
		qua = mpp_dom.map_choose_quanton();
		if(qua != NULL_PT){
			return qua;
		}
		BRAIN_CK(qua == NULL_PT);
		deactivate_last_map();
		DBG_PRT(122, os << "deac_lst (choose)");
	}

	for(long ii = 0; ii < br_choices.size(); ii++){
		quanton* qua_ii = br_choices[ii];
		if(qua_ii->get_charge() == cg_neutral){
			if(! qua_ii->is_bimon()){
				qua = qua_ii; 
				break;
			}
			if(qua == NULL_PT){ qua = qua_ii; }
		}
	}

	BRAIN_CK_0((qua == NULL) || (qua->get_charge() == cg_neutral));
	BRAIN_CK_0(	(qua == NULL) || 
			(qua->qu_spin == cg_positive) || 
			(qua->qu_spin == cg_negative));

	if((qua != NULL) && (br_choice_spin == cg_negative)){
		qua = qua->qu_inverse;
	}

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

	br_choices.clear();

	br_choices.set_cap(num_qua);
	br_choices.fill(NULL, num_qua);

	// reset quantons

	br_top_block.qu_inverse = &(br_top_block);
	BRAIN_CK(br_top_block.get_source() == NULL_PT);

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
		qua_pos->qu_choice_idx = idx;
		qua_neg->qu_choice_idx = idx;
		
		BRAIN_DBG(br_all_cy_quas.push(qua_pos));
	}
	BRAIN_CK_0(br_positons.size() == num_qua);
	BRAIN_CK_0(br_negatons.size() == num_qua);
	BRAIN_CK_0(br_choices.size() == num_qua);

	br_tmp_fixing_quantons.set_cap(num_qua);
	br_tmp_motives.set_cap(num_qua);

	//br_choices_lim = 0;

	br_current_ticket.tk_recoil = 1;
	br_current_ticket.tk_level = 0;

	BRAIN_CK(br_data_levels.is_empty());
	inc_data_levels();

	br_top_block.qu_charge_tk.update_ticket(brn);

	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	init_all_tots();

	BRAIN_DBG(
		br_dbg.dbg_all_chosen.clear();
	)
}

neuron*
brain::learn_mots(row_quanton_t& the_mots, quanton& forced_qua){
	DBG_PRT(23, os << "**LEARNING** mots=" << the_mots << " forced=" 
		<< &forced_qua);

	neuron* the_neu = NULL_PT;

	BRAIN_CK(! the_mots.is_empty() || (level() == ROOT_LEVEL));

	if(! the_mots.is_empty()){
		//BRAIN_CK(ck_motives(*this, the_mots));
		//BRAIN_CK(the_mots.last() != &forced_qua);
		the_mots.push(&forced_qua);

		quanton* f_qua = NULL_PT;
		neuron& added_neu = add_neuron(the_mots, f_qua, false);
		the_neu = &added_neu;

		data_level().ld_learned.push(the_neu);
	}

	DBG_PRT(23, os << "added_forced quanton: " << forced_qua;
		if(the_neu == NULL_PT){ os << " IS_SINGLETON"; } 
	);

	if(the_mots.is_empty()){
		//long nxt_tir = 0;
		long nxt_tir = tier() + 1;
		BRAIN_CK(level() == ROOT_LEVEL);
		send_psignal(forced_qua, the_neu, nxt_tir);
	}

	//	if(br_dbg.dbg_ic_active && br_dbg.dbg_ic_after){
	return the_neu;
}

neuron&
brain::add_neuron(row_quanton_t& quans, quanton*& forced_qua, bool orig){
	brain& brn = *this;
	neuron& neu = locate_neuron();
	neu.ne_original = orig;

	forced_qua = neu.update_fibres(quans, orig);

	neu.update_uncharged(brn, NULL_PT);
	BRAIN_CK(neu.fib_sz() > 0);
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[0]->ck_uncharged_partner_neu());
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[1]->ck_uncharged_partner_neu());
	BRAIN_DBG(neu.ne_dbg_creation_tk.update_ticket(brn));

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
	DBG_PRT(29, os << "ADDING NEU=" << neu_quas);

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

void
brain::init_uncharged(){
	brain& brn = *this;
	for(long ii = 0; ii < br_positons.size(); ii++){
		quanton& qua_pos = br_positons[ii];
		if(qua_pos.get_uncharged_partner_neu(dbg_call_1) == NULL_PT){
			qua_pos.set_uncharged_partner_neu(brn, qua_pos.find_uncharged_partner_neu(), 
											  100, NULL_PT);
		}
	}

	for(long ii = 0; ii < br_negatons.size(); ii++){
		quanton& qua_neg = br_negatons[ii];
		if(qua_neg.get_uncharged_partner_neu(dbg_call_2) == NULL_PT){
			qua_neg.set_uncharged_partner_neu(brn, qua_neg.find_uncharged_partner_neu(), 
											  101, NULL_PT);
		}
	}
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
	init_uncharged();

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
brain::write_all_active(){
	// write all pending
	row<neuromap*>& to_wrt = br_tmp_maps_to_write;
	to_wrt.clear();
	
	BRAIN_CK(! br_data_levels.is_empty());
	BRAIN_CK(level() == ROOT_LEVEL);
	leveldat& lv = get_data_level(ROOT_LEVEL);
	
	lv.ld_nmps_to_write.append_all_as<neuromap>(to_wrt);
	write_all_neuromaps(to_wrt);
}

void 
brain::find_result(){
	instance_info& inst_info = get_my_inst();
	bj_output_t& o_info = get_out_info();
	
	get_skeleton().kg_instance_file_nam = inst_info.get_f_nam() + "\n";
	
	if(o_info.bjo_result != bjr_unknown_satisf){ 
		return;
	}

	BRAIN_DBG(
		DBG_PRT(140, br_dbg.dbg_just_read = true);
		if(br_dbg.dbg_just_read){
			set_result(bjr_error);
			DBG_PRT(140, os << "____\nFULL_BRAIN_STAB=\n"; dbg_prt_full_stab());
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
	
	bj_satisf_val_t resp_solv = get_result();
	if(resp_solv == bjr_no_satisf){
		write_all_active();
		/*
		// write all pending
		row<neuromap*>& to_wrt = br_tmp_maps_to_write;
		to_wrt.clear();
		
		BRAIN_CK(! br_data_levels.is_empty());
		BRAIN_CK(level() == ROOT_LEVEL);
		leveldat& lv = get_data_level(ROOT_LEVEL);
		
		lv.ld_nmps_to_write.append_all_as<neuromap>(to_wrt);
		write_all_neuromaps(to_wrt);
		*/
	}

	br_tmp_assig_quantons.clear();

	if(inst_info.ist_with_assig){
		row_quanton_t& the_assig = br_tmp_assig_quantons;
		br_charge_trail.get_all_ordered_quantons(the_assig);
		get_ids_of(the_assig, inst_info.ist_assig);
		inst_info.ist_assig.push(0);	// means the last lit
	}
	
	BRAIN_DBG(
		DBG_PRT(116, dbg_prt_all_cho(*this));
		DBG_PRT(32, os << "BRAIN=" << bj_eol;
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

	close_all_maps();

	if(level() != ROOT_LEVEL){
		retract_to();
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
brain::close_all_maps(){
	while(! br_maps_active.is_empty()){
		deactivate_last_map();
	}
	BRAIN_CK(br_maps_active.is_empty());
	release_all_neuromaps();
	BRAIN_CK_PRT((br_num_active_neuromaps == 0), 
			os << "_______\n#act_nmps=" << br_num_active_neuromaps
				<< " #fr_nmps=" << br_free_neuromaps.size() 
				<< " #nmps=" << br_neuromaps.size() 
				<< bj_eol;
	);
}

void
notekeeper::set_motive_notes(row_quanton_t& rr_qua, long from, long until){
	BRAIN_CK(ck_funcs());
	BRAIN_CK(dk_tot_noted >= 0);
	BRAIN_CK(dk_num_noted_in_layer >= 0);

	brain& brn = get_dk_brain();

	DBG_PRT(104, os << "making notes " << rr_qua);

	if(from < 0){ from = 0; }
	if(until > rr_qua.size()){ until = rr_qua.size(); }
	
	bool br_in_root = (brn.level() == ROOT_LEVEL);

	long ii = from;
	for(; ii < until; ii++){
		BRAIN_CK(rr_qua[ii] != NULL_PT);
		quanton& qua = *(rr_qua[ii]);
		BRAIN_CK(qua.get_charge() == cg_negative);

		bool qu_in_root = (qua.qlevel() == ROOT_LEVEL);
		
		bool to_note = (! qu_in_root || br_in_root);
		//to_note = true;
		
		bool has_note = (qua.*dk_has_note_fn)();
		if(! has_note && to_note){
			dk_tot_noted++;
			(qua.*dk_set_note_fn)(brn);

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
			DBG_PRT(20, os << qua << " num_notes_in_lv " << dk_num_noted_in_layer 
					<< "lv=" << dk_note_layer << bj_eol
					<< "motives_by_lv= " << dk_quas_lyrs.dk_quas_by_layer);
			
		}
	}
}

// levels

void
leveldat::release_learned(brain& brn){
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
	row_row_quanton_t& all_qu_lyrs = dk_quas_lyrs.dk_quas_by_layer;
	
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
	br_current_ticket.tk_level--;

	brain& brn = *this;
	leveldat& lv = data_level();
	lv.reset_monos(brn);
	lv.reset_bimons(brn);
	lv.release_learned(brn);
	lv.let_maps_go(brn);

	leveldat* pt_lv = br_data_levels.pop();
	leveldat::release_leveldat(pt_lv);
}

void
brain::retract_to(long tg_lv){
	BRAIN_CK(br_bimons_to_update.is_empty());

	brain& brn = *this;
	while(level() >= 0){
		bool end_of_lev = (trail_level() != level());
		if(end_of_lev){
			BRAIN_CK((trail_level() + 1) == level());
			BRAIN_CK(level() != ROOT_LEVEL);
		
			dec_level();
		}

		if(level() == tg_lv){
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

		quanton& qua = trail_last();
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}
	BRAIN_CK((tg_lv == INVALID_LEVEL) || (level() == tg_lv));
	BRAIN_CK((tg_lv == INVALID_LEVEL) || (trail_level() == tg_lv));
	
	update_bimons();
}

bool
brain::dbg_ck_deducs(deduction& dct1, deduction& dct2){
	long lv1 = dct1.dt_target_level;
	long lv2 = dct2.dt_target_level;

	if(lv1 == INVALID_LEVEL){ lv1 = ROOT_LEVEL; }
	if(lv2 == INVALID_LEVEL){ lv2 = ROOT_LEVEL; }

	bool c1 = (dct1.dt_motives.equal_to(dct2.dt_motives));
	bool c2 = (dct1.dt_forced == dct2.dt_forced);
	bool c3 = (lv1 == lv2);

	MARK_USED(c1);
	MARK_USED(c2);
	MARK_USED(c3);

	DBG_COND_COMM(! (c1 && c2 && c3) ,
		os << "ABORTING_DATA " << bj_eol;
		os << "  c1=" << c1 << "  c2=" << c2 << "  c3=" << c3 << bj_eol;
		os << "dct1=" << dct1 << bj_eol;
		os << "dct2=" << dct2 << bj_eol;
		print_trail(os);
		os << "END_OF_aborting_data" << bj_eol;
	);

	BRAIN_CK(c1);
	BRAIN_CK(c2);

	BRAIN_CK(c3);
	return true;
}

quanton*
brain::receive_psignal(bool only_in_dom){
	//BRAIN_CK(! found_conflict()); // simple_propag
	BRAIN_CK(has_psignals());
	brain& brn = *this;
	prop_signal& sgnl = pick_psignal();

	BRAIN_CK(sgnl.ps_quanton != NULL_PT);
	quanton* pt_qua = sgnl.ps_quanton;

	quanton& qua = *(pt_qua);
	neuron* neu = sgnl.ps_source;
	long sg_tier = sgnl.ps_tier;
	BRAIN_CK(sg_tier != INVALID_TIER);
	
	DBG_PRT(21, os << sgnl << " odom=" << only_in_dom; os << " HIT ENTER TO CONTINUE...");
	DBG_COMMAND(21, getchar());
	
	/*
	if(found_conflict()){
		prop_signal& f_cfl = first_conflict();
		if(! qua.has_charge() && (sg_tier >= f_cfl.ps_tier)){
			DBG_PRT(21, os << "\n o_cfl=" << f_cfl << "\n new_ps=" << sgnl);
			return NULL_PT;
		}
	}

	if(only_in_dom && ! qua.in_qu_dominated(brn)){
		br_delayed_psignals.push(sgnl);
		DBG_PRT(21, os << " no qu_dom" << qua);
		return NULL_PT;
	}

	if(only_in_dom && (neu != NULL_PT) && neu->ne_original && ! neu->in_ne_dominated(brn)){
		br_delayed_psignals.push(sgnl);
		DBG_PRT(21, os << " no ne_dom" << neu);
		return NULL_PT;
	}*/

	if(qua.has_charge()){
		if(qua.is_neg()){
			BRAIN_REL_CK(neu != NULL_PT);
			BRAIN_CK(neu != NULL_PT);
			//BRAIN_CK((neu != NULL_PT) || (level() == ROOT_LEVEL));
			//long cnfl_ti = sg_tier;
			
			long cnfl_ti = tier() + 1;
			BRAIN_CK(sg_tier <= cnfl_ti);
			
			if(! neu->has_tag0()){
				neu->set_tag0(brn);
				prop_signal& nxt_cfl = br_all_conflicts_found.inc_sz();
				nxt_cfl.init_prop_signal(pt_qua, neu, cnfl_ti);
				DBG_PRT(18, os << "**confict** " << nxt_cfl;
					os << "\n sg_tier=" << sg_tier << " cnfl_ti=" << cnfl_ti);
			}
			
			//reset_psignals();	// simple_propag
			//BRAIN_CK(! has_psignals()); // simple_propag
			BRAIN_CK(found_conflict());
			//BRAIN_CK(ck_confl_ti());
			DBG_PRT_COND(136, (br_all_conflicts_found.size() > 1), 
					os << "num_confl=" << br_all_conflicts_found.size()
			);
		} 
		pt_qua = NULL_PT;
	} else {
		BRAIN_CK_PRT((! only_in_dom || (sg_tier == tier()) || (sg_tier == (tier() + 1))),
				os << "____________________\n";
				brn.dbg_prt_margin(os);
				os << " only_in_dom=" << only_in_dom << "\n";
				os << " ti=" << tier() << "\n";
				os << " sg_tier=" << sg_tier << "\n";
				os << " neu=" << neu << "\n";
				os << " blv=" << brn.level() << "\n";
				os << " sgnl=" << sgnl << "\n";
				os << " qua=" << &qua << "\n";
				brn.print_trail(os);
		);

		BRAIN_CK(qua.ck_uncharged_partner_neu());
		BRAIN_CK(qua.opposite().ck_uncharged_partner_neu());

		qua.set_charge(brn, neu, cg_positive, sg_tier);

		DBG_PRT_COND(64, (neu != NULL_PT), os << "qua=" << &qua << " SRC=" << neu);

		BRAIN_CK((qua.qu_source == neu) || 
			((level() == ROOT_LEVEL) && (qua.qu_source == NULL_PT)));
		BRAIN_CK((qua.qu_tier == sg_tier) || 
			((level() == ROOT_LEVEL) && (qua.qu_tier == 0)));
	}
	
	if(found_conflict()){
		update_cfls_shadow(sgnl);
	}
	
	DBG_PRT(21, os << "init sgnl" << sgnl);
	sgnl.init_prop_signal();

	return pt_qua;
}

void
brain::update_cfls_shadow(prop_signal& sgnl){
	brain& brn = *this;
	
	BRAIN_CK(sgnl.ps_quanton != NULL_PT);
	quanton& qua = *(sgnl.ps_quanton);
	if(qua.has_note0()){
		return;
	}
	
	if(qua.has_charge() && qua.is_neg()){
		qua.set_note0(brn);
		br_shadow_quas.push(&qua);
		prop_signal& nxt_ps = br_shadow_ps.inc_sz();
		nxt_ps = sgnl;
		return;
	} 
	
	neuron* neu = sgnl.ps_source;
	if(neu == NULL_PT){
		return;
	}
	row_quanton_t& all_fib = neu->ne_fibres;
	for(long aa = 0; aa < all_fib.size(); aa++){
		BRAIN_CK(all_fib[aa] != NULL_PT);
		quanton& fib = *(all_fib[aa]);
		if(fib.has_note0()){
			qua.set_note0(brn);
			br_shadow_quas.push(&qua);
			prop_signal& nxt_ps = br_shadow_ps.inc_sz();
			nxt_ps = sgnl;
			break;
		}
	}
}

long
brain::propagate_signals(){
	if(found_conflict() || ! has_psignals()){
		return 0;
	}

	BRAIN_CK(br_delayed_psignals.is_empty());

	long num_psigs1 = brn_tunnel_signals(true);
	long num_psigs2 = 0;

	if(! found_conflict() && ! br_delayed_psignals.is_empty()){
		BRAIN_CK(! has_psignals());
		BRAIN_CK(has_reset_psignals());
		send_row_psignals(br_delayed_psignals);
		br_delayed_psignals.clear(true, true);

		num_psigs2 = brn_tunnel_signals(false);

		BRAIN_CK(br_delayed_psignals.is_empty());
	}

	br_delayed_psignals.clear(true, true);

	BRAIN_CK(! has_psignals());
	BRAIN_CK(num_psignals() == 0);

	update_monos();
	
	long tot_psigs = num_psigs1 + num_psigs2;
	return tot_psigs;
}

void
brain::start_propagation(quanton& nxt_qua){
	BRAIN_CK(data_level().ld_idx == level());
	
	quanton* pt_qua = &nxt_qua;
	if(pt_qua->is_bimon()){
		pt_qua = pt_qua->get_bimon();
	}
	quanton& qua = *pt_qua;

	inc_level(qua);

	BRAIN_DBG(long old_lv = level());
	BRAIN_CK(data_level().ld_chosen == &qua);

	BRAIN_CK(! has_psignals());
	send_psignal(qua, NULL, tier() + 1);
	BRAIN_CK(has_psignals());

	if(qua.is_bimon()){
		return;
	}
	if(qua.is_mono() || qua.opposite().is_mono()){
		return;
	}

	long num1 = propagate_signals();
	BRAIN_CK(num1 > 0);

	if(! found_conflict()){
		retract_choice();
		long num2 = propagate_signals();
		BRAIN_CK(num2 > 0);

		if(! found_conflict()){
			if(num1 > num2){
				retract_choice();
				BRAIN_CK(old_lv == level());
			}

			if((num1 == num2) && qua.qu_has_been_cho){
				retract_choice();
				BRAIN_CK(old_lv == level());
			}
		}
	}
}


void
brain::pulsate(){
	propagate_signals();

	BRAIN_CK(ck_trail());

	if(found_conflict()){
		br_last_retract = (level() == ROOT_LEVEL);
		if(level() == ROOT_LEVEL){ 
			set_result(bjr_no_satisf);
			return;
		}
		//dbg_old_reverse();
		reverse();
		/*if(br_last_retract){ 
			set_result(bjr_no_satisf);
			return;
		}*/
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

		quanton* cho = curr_choice();
		BRAIN_CK((cho == &qua) || (&(qua.opposite()) == cho));
		
		if(! cho->qu_has_been_cho){ 
			cho->qu_has_been_cho = true; 
			BRAIN_DBG(
				cho->qu_dbg_fst_lap_cho = brn.br_current_ticket.tk_recoil;
				br_dbg.dbg_all_chosen.push(cho);
			)
		}
		BRAIN_DBG(if(cho->opposite().qu_has_been_cho){ cho->qu_dbg_num_laps_cho++; });
		DBG_PRT(25, os << "**CHOICE** " << cho);
	}
}

long
quanton::find_uncharged_partner_neu(){
	quanton& qua = *this;
	long unchg_idx = INVALID_IDX;
	long min_lv_idx = INVALID_IDX;
	long min_par_lv = 0;
	for(long ii = 0; ii < qu_tunnels.size(); ii++){
		BRAIN_CK(qu_tunnels[ii] != NULL_PT);
		neuron& neu = *(qu_tunnels[ii]);

		if(! neu.ne_original){
			continue;
		}

		quanton& par = neu.partner_fib(qua);
		if(! par.has_charge()){
			unchg_idx = ii;
			break;
		} else {
			long plv = par.qlevel();
			if(has_charge() && (plv >= qlevel()) && 
				((min_lv_idx == INVALID_IDX) || (plv < min_par_lv))
			){
				min_lv_idx = ii;
				min_par_lv = plv;
			}
		}
	}
	if((unchg_idx == INVALID_IDX) && (min_lv_idx != INVALID_IDX)){
		unchg_idx = min_lv_idx;
	}
	return unchg_idx;
}

void
neuron::update_uncharged(brain& brn, quanton* pt_chg_qua){
	if(! ne_original){
		return;
	}

	BRAIN_CK(fib_sz() > 1);

	quanton& qua0 = *(ne_fibres[0]);
	quanton& qua1 = *(ne_fibres[1]);

	BRAIN_CK((pt_chg_qua == NULL_PT) || pt_chg_qua->has_charge());
	if((pt_chg_qua != NULL_PT) && (pt_chg_qua != &qua0) && (pt_chg_qua != &qua1)){
		quanton& chg_qua = *pt_chg_qua;
		neuron* uchg_q = chg_qua.get_uncharged_partner_neu(dbg_call_1);
		if(uchg_q == this){
			chg_qua.set_uncharged_partner_neu(brn, chg_qua.find_uncharged_partner_neu(), 
											  1, this);
		}
	}

	bool has0 = qua0.has_charge();
	bool has1 = qua1.has_charge();

	neuron* uchg0 = qua0.get_uncharged_partner_neu(dbg_call_2);
	neuron* uchg1 = qua1.get_uncharged_partner_neu(dbg_call_3);

	if(! has0 && ! has1){
		if(uchg0 == NULL_PT){
			BRAIN_CK(qua0.qu_tunnels[ne_fibre_0_idx] == this);
			qua0.set_uncharged_partner_neu(brn, ne_fibre_0_idx, 2, this);
			BRAIN_CK(qua0.get_uncharged_partner_neu(dbg_call_4) == this);
		}
		if(uchg1 == NULL_PT){
			BRAIN_CK(qua1.qu_tunnels[ne_fibre_1_idx] == this);
			qua1.set_uncharged_partner_neu(brn, ne_fibre_1_idx, 3, this);
			BRAIN_CK(qua1.get_uncharged_partner_neu(dbg_call_5) == this);
		}
	}

	if(has0 && ! has1){

		if(uchg0 == NULL_PT){
			BRAIN_CK(qua0.qu_tunnels[ne_fibre_0_idx] == this);
			qua0.set_uncharged_partner_neu(brn, ne_fibre_0_idx, 4, this);
			BRAIN_CK(qua0.get_uncharged_partner_neu(dbg_call_6) == this);
		}
		if((uchg1 == this) || (uchg1 == NULL_PT)){
			qua1.set_uncharged_partner_neu(brn, qua1.find_uncharged_partner_neu(), 5, this);
		}
	}

	if(! has0 && has1){

		if((uchg0 == this) || (uchg0 == NULL_PT)){
			qua0.set_uncharged_partner_neu(brn, qua0.find_uncharged_partner_neu(), 6, this);
		}
		if(uchg1 == NULL_PT){
			BRAIN_CK(qua1.qu_tunnels[ne_fibre_1_idx] == this);
			qua1.set_uncharged_partner_neu(brn, ne_fibre_1_idx, 7, this);
			BRAIN_CK(qua1.get_uncharged_partner_neu(dbg_call_7) == this);
		}
	}

	if(has0 && has1){
		if((uchg0 == this) || (uchg0 == NULL_PT)){
			qua0.set_uncharged_partner_neu(brn, qua0.find_uncharged_partner_neu(), 8, this);
		}
		if((uchg1 == this) || (uchg1 == NULL_PT)){
			qua1.set_uncharged_partner_neu(brn, qua1.find_uncharged_partner_neu(), 9, this);
		}
	}

	//BRAIN_CK(qua0.ck_uncharged_partner_neu());
	//BRAIN_CK(qua1.ck_uncharged_partner_neu());
	//BRAIN_CK((pt_chg_qua == NULL_PT) || pt_chg_qua->ck_uncharged_partner_neu());
}

neuron*
quanton::get_uncharged_partner_neu(dbg_call_id dbg_call){
	BRAIN_CK((qu_uncharged_partner_neu == NULL_PT) || 
			qu_uncharged_partner_neu->has_qua(*this));
	return qu_uncharged_partner_neu;
}

void
quanton::add_to_bimons_lv(brain& brn){
	leveldat& lv = brn.data_level();
	BRAIN_CK(lv.ld_idx == brn.level());
	lv.ld_bimons_to_update.push(this);
	
	qu_bimon_lv = lv.ld_idx;
	qu_bimon_idx = lv.ld_bimons_to_update.last_idx();
	
}

bool
quanton::ck_bimons_lv(brain& brn){
#ifdef FULL_DEBUG
	if(qu_bimon_lv == INVALID_LEVEL){
		BRAIN_CK(qu_bimon_idx == INVALID_IDX);
		return true;
	}
	leveldat& lv = brn.get_data_level(qu_bimon_lv);
	BRAIN_CK(lv.ld_bimons_to_update.is_valid_idx(qu_bimon_idx));
	BRAIN_CK(lv.ld_bimons_to_update[qu_bimon_idx] == this);
#endif
	return true;
}

bool
quanton::in_bimons_lv(){
	bool in_sm = (qu_bimon_lv != INVALID_LEVEL);
	BRAIN_CK(! in_sm || (qu_bimon_idx != INVALID_IDX));
	//BRAIN_CK(ck_bimons_lv(brn));
	return in_sm;
}

void
quanton::set_uncharged_partner_neu(brain& brn, long uidx, long dbg_call, neuron* dbg_neu){
	if(uidx == INVALID_IDX){
		qu_bak_uncharged_partner_neu = qu_uncharged_partner_neu;
		add_to_bimons_lv(brn);

		qu_uncharged_partner_neu = NULL_PT;
		BRAIN_CK(ck_uncharged_partner_neu());
		return;
	}

	BRAIN_CK(qu_tunnels.is_valid_idx(uidx));
	BRAIN_CK(qu_tunnels[uidx] != NULL_PT);

	qu_uncharged_partner_neu = qu_tunnels[uidx];

	BRAIN_CK(qu_uncharged_partner_neu->ne_original);
	BRAIN_CK(qu_uncharged_partner_neu->is_partner_fib(*this));
	BRAIN_CK(ck_uncharged_partner_neu());
}

void
quanton::reset_uncharged_partner_neu(brain& brn){

	if((qu_uncharged_partner_neu == NULL_PT) && (qu_bak_uncharged_partner_neu != NULL_PT)){
		qu_uncharged_partner_neu = qu_bak_uncharged_partner_neu;
	}

	quanton& qua = *this;
	neuron* neu = qu_uncharged_partner_neu;
	if((neu == NULL_PT) || ! neu->is_partner_fib(qua) || neu->partner_fib(qua).has_charge()){
		set_uncharged_partner_neu(brn, find_uncharged_partner_neu(), 200, NULL_PT);
	}
	qu_bak_uncharged_partner_neu = NULL_PT;

	BRAIN_CK(ck_uncharged_partner_neu());
}


void
leveldat::reset_bimons(brain& brn){
	while(! ld_bimons_to_update.is_empty()){
		quanton& qua = *(ld_bimons_to_update.pop());
		qua.reset_qu_bimon_lv();
		
		if(! qua.qu_in_bimons_to_update){
			brn.br_bimons_to_update.push(&qua);
			qua.qu_in_bimons_to_update = true;
		}
	}
}

void
brain::update_bimons(){
	row_quanton_t& to_upd = br_bimons_to_update;
	
	brain& brn = *this;
	data_level().reset_bimons(brn);
	while(! to_upd.is_empty()){
		quanton& qua = *(to_upd.pop());
		BRAIN_CK(qua.qu_in_bimons_to_update);
		qua.qu_in_bimons_to_update = false;
		qua.reset_uncharged_partner_neu(brn);
	}
	BRAIN_CK(to_upd.is_empty());
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
brain::retract_choice(){
	BRAIN_CK(! found_conflict());
	BRAIN_CK(! has_psignals());
	BRAIN_CK(br_bimons_to_update.is_empty());

	brain& brn = *this;

	long tgt_lvl = level();
	MARK_USED(tgt_lvl);

	BRAIN_CK_0(level() != ROOT_LEVEL);

	DBG_PRT(14, print_trail(os));
	DBG_PRT(14, os << "chosen " << br_chosen);

	// retract loop

	quanton* chosen_qua = NULL_PT;

	while(br_charge_trail.last_qlevel() == level()){
		BRAIN_CK(level() != ROOT_LEVEL);

		quanton& qua = trail_last();
		BRAIN_CK(qua.is_pos());

		if(! qua.has_source()){	chosen_qua = &qua; }
		
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}

	BRAIN_CK((br_charge_trail.last_qlevel() + 1) == level());

	BRAIN_DBG(long rr_lv = br_charge_trail.last_qlevel());
	BRAIN_CK((level() == ROOT_LEVEL) || ((rr_lv + 1) == tgt_lvl));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == tgt_lvl));

	// resend opp chosen

	data_level().reset_monos(brn);
	update_bimons();

	BRAIN_CK(chosen_qua != NULL_PT);
	BRAIN_CK(data_level().ld_chosen == chosen_qua);

	quanton& opp = chosen_qua->opposite();

	DBG_PRT(25, os << "**RETRACT TO OPPOSITE=" << &opp);

	data_level().ld_chosen = &opp;

	BRAIN_CK(! has_psignals());
	send_psignal(opp, NULL, tier() + 1);
	BRAIN_CK(has_psignals());
} // retract_choice

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
	};
}

bj_satisf_val_t
brain::solve_instance(bool load_it){
	bj_output_t& o_info = get_out_info();
	try{
		if(load_it){
			bool all_ok = load_instance();
			if(! all_ok){
				throw instance_exception(inx_bad_lit);
			}
		}
		find_result();
	} catch (skeleton_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_invalid_root_directory;
	} catch (file_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		set_file_err(ex1, o_info);
	} catch (parse_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_parse_bad_number;
	} catch (dimacs_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		set_dimacs_err(ex1, o_info);
	} catch (instance_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_instance_cannot_load;
	} catch (mem_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_memout;
	} catch (top_exception& ex1){
		print_ex(ex1);
		o_info.bjo_result = bjr_error;
		o_info.bjo_error = bje_internal_ex;
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

	return o_info.bjo_result;
}

void
brain::reverse(){
	BRAIN_CK(! has_psignals());
	
	DBG_PRT(122, print_trail(os));
	
	BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK(br_bimons_to_update.is_empty());
	BRAIN_CK(found_conflict());

	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());
	
	// analyse
	
	BRAIN_DBG(if(level() > br_dbg.dbg_max_lv){ br_dbg.dbg_max_lv = level(); });
	
	deduction& dct = br_retract_dct;

	// JLQ
	bool in_full_anls = true;
	DBG_COMMAND(141, in_full_anls = false);
	if(in_full_anls){
		analyse(br_all_conflicts_found, dct);
	} else {
		br_deducer_anlsr.set_conflicts(br_all_conflicts_found);
		br_deducer_anlsr.deduction_analysis(br_deducer_anlsr.get_first_causes(), dct);
	}
	DBG_PRT(122, os << "AFTE_ANALYSE \ncfl=" << first_conflict() << "\ndct=" << dct);

	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, dbg_prt_lvs_have_learned(os));
	
	// retract
	retract_to(dct.dt_target_level);
	
	// some checks

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);
	BRAIN_DBG(long rr_lv = trail_level());
	
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == dct.dt_target_level));
	BRAIN_CK(dct.dt_forced != NULL_PT);

	// learn motives
	
	neuron* lnd_neu = learn_mots(dct.dt_motives, *dct.dt_forced);
	
	// send forced learned

	quanton* nxt_qua = dct.dt_forced;
	if(! dct.dt_motives.is_empty()){
		BRAIN_CK(nxt_qua != NULL_PT);
		send_psignal(*nxt_qua, lnd_neu, tier() + 1);
	}

	// inc recoil

	inc_recoil();

	BRAIN_DBG(
		dbg_update_config_entries(br_dbg.dbg_conf_info, recoil());
	);
	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());

	DBG_PRT(131, 
			print_trail(os);
			dbg_prt_lvs_cho(os);
			os << "tr_lv=" << trail_level() << " tg_lv=" << dct.dt_target_level;
			os << " f_qu=" << nxt_qua; 
			//DO_GETCHAR()
	);
	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, print_trail(os); os << dct << bj_eol);

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
	
	dima_dims dims0;
	full_col.load_colors_into(brn, neus_srg, quas_srg, dims0);

	neus_srg.sg_cnf_clauses.clear();
	
	neus_srg.stab_mutual(quas_srg);
	
	//bj_out << "ALL_CCL=\n";
	
	canon_cnf& the_cnf = neus_srg.get_final_cnf(skg, "TEST_STAB", true);

	bj_out << "THE_CNF=" << bj_eol;
	bj_out << the_cnf;
	bj_out << "END_of_cnf=" << bj_eol;
	
	coloring finl_col;
	finl_col.save_colors_from(neus_srg, quas_srg, false);
	
	BRAIN_CK(finl_col.co_quas.size() == full_col.co_quas.size());
	BRAIN_CK(finl_col.co_neus.size() == full_col.co_neus.size());
	
	release_all_sortors();
#endif
}

#ifdef FULL_DEBUG
ch_string
get_cy_name(brain& brn, ch_string cy_kk, long num_step){
	ch_string num_stp = long_to_str(num_step);
	ch_string pth_nm = brn.br_dbg.dbg_cy_prefix + cy_kk + num_stp + ".js";
	return pth_nm;
}

ch_string
get_cy_dir(brain& brn){
	ch_string htm_pth = brn.get_solver().slv_dbg2.dbg_html_out_path;
	ch_string ic_dir = htm_pth + "/" + brn.br_dbg.dbg_cy_prefix;
	return ic_dir;
}

ch_string
get_cy_rel_path(brain& brn, ch_string cy_kk, long num_step){
	ch_string the_nm = get_cy_name(brn, cy_kk, num_step);
	ch_string rel_pth = brn.br_dbg.dbg_cy_prefix + "/" + the_nm;
	return rel_pth;
}

ch_string
get_cy_htm_nm(brain& brn, ch_string cy_kk){
	ch_string pth = brn.br_dbg.dbg_cy_prefix + "_all" + cy_kk + "steps.htm";
	return pth;
}

ch_string
get_cy_htm_path(brain& brn, ch_string cy_kk){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_htm_nm(brn, cy_kk);
	return pth;
}

ch_string
get_cy_path(brain& brn, ch_string cy_kk, long num_step){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_rel_path(brn, cy_kk, num_step);
	return pth;
}
#endif

void
brain::dbg_update_html_cy_graph(ch_string cy_kk, coloring* the_col, ch_string htm_str){
#ifdef FULL_DEBUG
	brain& brn = *this;
	
	ch_string sub_dir = get_cy_dir(brn) + "/" + brn.br_dbg.dbg_cy_prefix;
	if(! file_exists(sub_dir)){
		path_create(sub_dir);
		bj_out << "Created dir: '" << sub_dir << "'\n";
	}
	
	long n_stp = -1;
	ch_string js_grph_var_pref = "";
	ch_string js_plays_var_pref = "";
	ch_string js_fn_nm = "";
	ch_string js_tit_suf = "";
	bool is_ic = false;
	if(cy_kk == CY_IC_KIND){
		js_tit_suf = " trail CNF graphs";
		is_ic = true;
		js_grph_var_pref = "nmp_grph_";
		js_plays_var_pref = "all_plays_";
		js_fn_nm = "show_cnf";
		br_dbg.dbg_cy_ic_step++;
		n_stp = br_dbg.dbg_cy_ic_step;
	}
	if(cy_kk == CY_NMP_KIND){
		js_tit_suf = " sub CNF graphs";
		js_grph_var_pref = "nmp_grph_";
		js_fn_nm = "show_cnf";
		br_dbg.dbg_cy_nmp_step++;
		n_stp = br_dbg.dbg_cy_nmp_step;
	}
	
	ch_string htm_tit = brn.br_dbg.dbg_cy_prefix + js_tit_suf;
	
	BRAIN_CK(n_stp != -1);
	if(n_stp > MAX_CY_STEPS){
		return;
	}

	ch_string js_txt_var_pref = "comment_";
	
	ch_string& layo_str = br_dbg.dbg_cy_layout;
	
	ch_string stp_str = long_to_str(n_stp);
	ch_string stp_pth = get_cy_path(brn, cy_kk, n_stp);
	ch_string stp_js_grph_var_nm = js_grph_var_pref + stp_str;
	ch_string stp_js_plays_var_nm = js_plays_var_pref + stp_str;
	ch_string stp_js_txt_var_nm = js_txt_var_pref + stp_str;
	
	if(the_col == NULL_PT){
		if(br_dbg_full_col.is_co_virgin()){
			br_dbg_full_col.init_coloring(&brn);
			br_dbg_full_col.dbg_set_brain_coloring();
		}
		the_col = &br_dbg_full_col;
	}
	BRAIN_CK(the_col != NULL_PT);
	BRAIN_CK(the_col->get_dbg_brn() != NULL_PT);
	
	bj_ofstream of_st;
	dbg_prt_open(stp_pth, of_st);
	
	of_st << stp_js_grph_var_nm << " = {" << bj_eol;
	the_col->dbg_print_col_cy_graph(of_st, is_ic);
	of_st << "};" << bj_eol;
	
	if(is_ic){
		of_st << stp_js_plays_var_nm << " = [" << bj_eol;
		dbg_print_cy_graph_node_plays(of_st);
		of_st << "];" << bj_eol;
	} 

	//htm_str = "<p>texto prueba</p>";
	if(htm_str.size() > 0){
		of_st << stp_js_txt_var_nm << " = '" << htm_str << "';" << bj_eol;
	}
	
	of_st.flush();
	of_st.close();

	bj_out << "Writed file: '" << stp_pth << "'\n";
	
	ch_string pth = get_cy_htm_path(brn, cy_kk);
	ch_string pth_nm =  get_cy_htm_nm(brn, cy_kk);;
	
	bj_ofstream of;
	dbg_prt_open(pth, of);
	
	of << HTMi_html << bj_eol;
	of << "\t" << HTMi_head << bj_eol;
	of << "\t\t" << HTMi_title << htm_tit << HTMe_title << bj_eol;
	
	for(long aa = 1; aa <= n_stp; aa++){
		ch_string rel_aa_pth = get_cy_rel_path(brn, cy_kk, aa);
		of << "\t\t " << HTMi_src << rel_aa_pth << HTMe_src << bj_eol;
	}
	of << "\t\t " << HTMi_src << "drw_grph_files/jquery.js" << HTMe_src << bj_eol;
	of << "\t\t " << HTMi_src << "drw_grph_files/cytoscape.js" << HTMe_src << bj_eol;
	of << "\t\t " << HTMi_src << "drw_grph_files/show_cnf_fn.js" << HTMe_src << bj_eol;
	
	of << "\t" << HTMe_head << bj_eol;
	
	of << "\t" << HTMi_body << bj_eol;
	for(long aa = 1; aa <= n_stp; aa++){
		ch_string stp_aa_str = long_to_str(aa);
		ch_string h1_title = "Step " + stp_aa_str;
		of << "\t\t " << HTMi_h1 << h1_title << HTMe_h1 << bj_eol;

		ch_string div_txt_nm = "cnf_text_" + stp_aa_str;
		of << "\t\t " << HTM_div(div_txt_nm) << bj_eol;
		
		ch_string div_nm = "cnf_graph_" + stp_aa_str;
		ch_string div_str = HTM_cy_div(div_nm);
		of << "\t\t " << div_str << bj_eol;
		
		ch_string js_grph_var_aa_nm = js_grph_var_pref + stp_aa_str;
		ch_string js_plays_var_aa_nm = js_plays_var_pref + stp_aa_str;
		ch_string js_txt_var_aa_nm = js_txt_var_pref + stp_aa_str;
		
		of << "\t\t " << HTMi_script << js_fn_nm << "('" << div_nm << "'";
		of << ", " << js_grph_var_aa_nm << ", " << layo_str;
		if(is_ic){
			of << ", " << js_plays_var_aa_nm;
		}
		of << ");" << bj_eol;
		
		of << "\t\t " << JS_inner_htm(div_txt_nm, js_txt_var_aa_nm) << bj_eol;
		
		of << "\t\t " << HTMe_script << bj_eol;
	}
	of << "\t" << HTMe_body << bj_eol;
	
	of << HTMe_html << bj_eol;
	
	of.flush();
	of.close();

	bj_out << "Updated file: '" << pth << "'\n";
#endif
}

void
brain::dbg_print_cy_graph_node_plays(bj_ostream& os){
#ifdef FULL_DEBUG
	bool is_fst = true;
	for(long aa = 0; aa < br_positons.size(); aa++){
		quanton& qua = br_positons[aa];
		BRAIN_CK(qua.qu_id > 0);
		if(qua.is_neg()){
			os << "\t";
			if(! is_fst){ os << ","; } else { is_fst = false; }
			os << "'d" << qua.qu_id << "'" << bj_eol;
		}
	}
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

void
brain::dbg_br_init_all_cy_pos(){
#ifdef FULL_DEBUG
#endif
}

void
qulayers::dbg_ql_init_all_cy_pos(){
#ifdef FULL_DEBUG
	brain& brn = get_ql_brain();
	row_quanton_t& all_quas = brn.br_dbg_quly_cy_quas;
	get_all_ordered_quantons(all_quas);
	
	long lst_ti = INVALID_TIER;
	long num_ti = 0;
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		if(lst_ti != qua.qu_tier){
			lst_ti = qua.qu_tier;
			num_ti = 0;
		}
		num_ti++;
		
		long lv = qua.qlevel();
		ch_string pnt = "l" + lv;
		
		long x_pos = num_ti * 100;
		long y_pos = qua.qu_tier * 100;
		
		qua.qu_dbg_drw_x_pos = x_pos;
		qua.qu_dbg_drw_y_pos = y_pos;
	}
#endif
}

quanton*
brain::choose_bimon(){
	quanton* cho = NULL_PT;
	row<leveldat*>& all_lv = br_data_levels;
	for(long aa = 0; aa < all_lv.size(); aa++){
		BRAIN_CK(all_lv[aa] != NULL_PT);
		leveldat& lv_dat = *(all_lv[aa]);
		row_quanton_t& all_sm = lv_dat.ld_bimons_to_update;
		
		for(long bb = 0; bb < all_sm.size(); bb++){
			BRAIN_CK(all_sm[bb] != NULL_PT);
			quanton& qua = *(all_sm[bb]);
			if(! qua.has_charge()){
				cho = &qua;
				break;
			}
		}
		
		if(cho != NULL_PT){
			break;
		}
	}
	return cho;
}

long
quanton::find_alert_idx(bool is_init){
	row_neuron_t& all_neus = qu_neus;
	long fst_idx = qu_alert_neu_idx;
	
	if(all_neus.is_empty()){
		return INVALID_IDX;
	}
	
	if(is_init){
		fst_idx = 0;
	} 
	
	BRAIN_CK_PRT(all_neus.is_valid_idx(fst_idx), os << "_______\n" 
		<< "fst_idx=" << fst_idx
	);
	BRAIN_CK(all_neus[fst_idx] != NULL_PT);
	
	if(! is_init){
		BRAIN_CK(all_neus[fst_idx]->is_ne_inert());
		fst_idx++;
	}
	
	long found_idx = INVALID_IDX;
	for(long aa = fst_idx; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		if(neu.is_ne_alert()){
			found_idx = aa;
			break;
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

bool
quanton::ck_mono(){
#ifdef FULL_DEBUG
	BRAIN_CK(find_alert_idx(true) == INVALID_IDX);
#endif
	return true;
}

void
quanton::update_alert_neu(brain& brn, bool is_init){
	quanton& qua = *this;
	
	BRAIN_CK(! has_charge());
	BRAIN_CK(qu_lv_mono == INVALID_LEVEL);
	BRAIN_CK(! is_init || (qu_alert_neu_idx == INVALID_IDX));
	BRAIN_CK(! is_init || (brn.level() == ROOT_LEVEL));

	long found_idx = find_alert_idx(is_init);
	if(found_idx != INVALID_IDX){
		set_alert_neu(brn, found_idx);
		
		BRAIN_CK(qu_neus.is_valid_idx(qu_alert_neu_idx));
		BRAIN_CK(qu_neus[qu_alert_neu_idx] != NULL_PT);
		BRAIN_CK(qu_neus[qu_alert_neu_idx]->is_ne_alert());
		return;
	}
	qu_lv_mono = brn.level();
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
	
	qlayers_ref qtrl;
	qtrl.init_qlayers_ref(&br_charge_trail);
	qtrl.reset_curr_quanton();

	row_quanton_t& all_quas = br_tmp_uncharged_in_alert_neus;
	all_quas.clear();
	
	BRAIN_CK(br_qu_tot_note1 == 0);
	
	while(qtrl.get_curr_qlevel() == lv){
		quanton* qua = qtrl.get_curr_quanton();
		BRAIN_CK(qua != NULL_PT);
		
		//DBG_PRT_COND(147, (qua->qu_id == 5), os << 
		qua->append_all_to_alert(brn, all_quas);
		
		qtrl.dec_curr_quanton();
	}
	
	for(long bb = 0; bb < all_quas.size(); bb++){
		BRAIN_CK(all_quas[bb] != NULL_PT);
		quanton& qua = *(all_quas[bb]);
		BRAIN_CK(qua.has_note1());
		qua.reset_note1(brn);
		qua.update_alert_neu(brn, false);
	}

	BRAIN_CK(br_qu_tot_note1 == 0);
	all_quas.clear();
}

neuron*
quanton::get_alert_neu(){
	row_neuron_t& all_neus = qu_neus;
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
	if(ar_qu_alert != NULL_PT){
		ar_qu_alert->qu_mono_alerts.bind_to_my_right(ar_alert);
	}
	
	ar_ref.let_go();
	if(ar_qu_ref != NULL_PT){
		ar_qu_ref->qu_mono_refs.bind_to_my_right(ar_ref);
	}
}

void
quanton::set_alert_neu(brain& brn, long the_idx){
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
		
		alert_rel& rr = brn.locate_alert_rel();
		rr.init_alert_rel(&the_alert, &qua);
	}
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
		
		if(! qua.has_note1() && ! qua.has_charge() && ! qua.is_mono()){
			qua.set_note1(brn);
			all_quas.push(&qua);
		}
	}
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
	data_level().ld_bak_mono_idx = br_last_monocho_idx;
	
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
	BRAIN_CK(qua->ck_mono());
	
	quanton& opp = qua->opposite();
	BRAIN_CK(! opp.has_charge());
	return &opp;
}

bool
brain::ck_prev_monos(){
#ifdef FULL_DEBUG
	for(long aa = 0; aa < br_last_monocho_idx; aa++){
		BRAIN_CK(br_monos.is_valid_idx(aa));
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
			BRAIN_CK(neu->is_ne_alert());
		);
	}

	brn.br_last_monocho_idx = brn.data_level().ld_bak_mono_idx;
	BRAIN_CK(brn.ck_prev_monos());
}

/*
void
brain::update_curr_mono(){
	if(br_monos.is_empty()){
		br_last_monocho_idx = 0;
		return;
	}
	if(! br_monos.is_valid_idx(br_last_monocho_idx)){
		br_last_monocho_idx = br_monos.last_idx();
	}
	quanton* qua = get_curr_mono();
	while((qua != NULL_PT) && ! qua->has_charge()){
		br_last_monocho_idx--;
		qua = get_curr_mono();
	}
	BRAIN_CK(ck_prev_monos());
}
*/
