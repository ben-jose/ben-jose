

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
#include "dbg_config.h"
#include "dbg_strings_html.h"

DEFINE_GET_DBG_SLV(quanton)
DEFINE_GET_DBG_SLV(neuron)
DEFINE_GET_DBG_SLV(prop_signal)
DEFINE_GET_DBG_SLV(deduction)
DEFINE_GET_DBG_SLV(coloring)
DEFINE_GET_DBG_SLV(neuromap)
DEFINE_GET_DBG_SLV(neurolayers)
DEFINE_GET_DBG_SLV(qulayers)
DEFINE_GET_DBG_SLV(notekeeper)
DEFINE_GET_DBG_SLV(analyser)
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
				qua->qu_all_neus.push(neu);
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

		DBG_PRT(31, os << "forcing " << this);
	}

	if((cg0 != cg_neutral) && (cg1 != cg_neutral)){
		BRAIN_DBG(long npos = 0);
		BRAIN_CK(ck_all_has_charge(npos));
		//if(ne_original && (cg0 == cg_positive) && (cg1 == cg_positive)){
		if(ne_original){
			BRAIN_DBG(ne_dbg_filled_tk.update_ticket(brn));
			r_qua.qu_full_charged.push(this);
			DBG_PRT(32, os << "qua=" << &r_qua << " filled orig " << this);
		}
		dbg2 = true;
	}

	BRAIN_CK(! q_neg || dbg1 || ! ck_all_charges(&brn, 1));
	BRAIN_DBG(long npos2 = 0; bool ck2 = ck_all_has_charge(npos2); );
	BRAIN_CK(dbg2 || ! ck2);
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
	
	BRAIN_DBG(
		br_pt_brn = NULL_PT;
		br_dbg_round = 0;
		br_dbg_num_phi_grps = INVALID_NATURAL;
		strset_clear(br_dbg_phi_wrt_ids);
	);
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

	br_tot_cy_sigs = 0;
	br_tot_cy_nmps = 0;
	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	br_num_active_neurons = 0;
	br_num_active_alert_rels = 0;
	br_num_active_neuromaps = 0;

	BRAIN_DBG(
		init_all_dbg_brn();  // sets br_pt_brn indicating it is readi for DBG_PRT
		//dbg_init_dbg_conf(get_solver().slv_dbg_conf_info);
		get_solver().slv_dbg_brn = this;
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

	reset_cicle_src();
	
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

		//qu_charge_tk.tk_recoil = brn.recoil();

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
brain::brn_tunnel_signals(bool only_in_dom, row_quanton_t& all_impl_cho){
	brain& brn = *this;
	MARK_USED(brn);
	BRAIN_CK(br_ne_tot_tag0 == 0);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK_PRT((! found_conflict()), os << "_______________\n ABORT_DATA\n";
		os << " ALL_CONFL=\n";
		br_all_conflicts_found.print_row_data(os, true, "\n");
	);
	BRAIN_CK(ck_trail());
	BRAIN_DBG(
		ticket tk1;
		tk1.update_ticket(brn);
		BRAIN_CK(! found_conflict());
	);
	
	set_all_note0(brn, all_impl_cho);

	long num_psigs = 0;

	quanton* cho = curr_cho();

	BRAIN_DBG(quanton* l_trl = br_charge_trail.last_quanton());
	DBG_PRT(102, os << "Starting tunnel" << " trail_last=" << l_trl << "\n";
		print_trail(os);
	);
	
	while(has_psignals()){
		quanton* qua = receive_psignal(only_in_dom);
		if(qua == NULL_PT){
			BRAIN_CK((curr_cho() == NULL_PT) || curr_cho()->has_charge());
			DBG_PRT(102, os << "\n\n null receive" << " lv=" << level() << " ti=" << tier();
				os << " trail_last=" << l_trl << "\n\n";
			);
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
		
		if((cho != NULL_PT) && cho->is_opp_mono()){
			//BRAIN_CK(cho == qua);
			send_next_mono();
		}
	} // end while

	reset_all_note0(brn, all_impl_cho);
	
	BRAIN_CK_0(! has_psignals());
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
		DBG_PRT(43, os << "DEACT_IN_CHO nmp=" << (void*)pt_mpp);
		deactivate_last_map();
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

	BRAIN_CK(br_tot_cy_sigs == 0);
	BRAIN_CK(br_tot_cy_nmps == 0);
	BRAIN_CK(br_tot_qu_dots == 0);
	BRAIN_CK(br_tot_qu_marks == 0);
	BRAIN_CK(br_tot_ne_spots == 0);

	init_all_tots();

	BRAIN_DBG(
		br_dbg.dbg_all_chosen.clear();
	)
}

neuron*
brain::learn_mots(deduction& dct){
	
	row_quanton_t& the_mots = dct.dt_motives;
	quanton& forced_qua = *dct.dt_forced;
	
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

	BRAIN_CK(neu.fib_sz() > 0);
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
		DBG_COMMAND(12, just_read = true);
		if(just_read){
			set_result(bjr_error);
			DBG_PRT(12, os << "____\nFULL_BRAIN_STAB=\n"; dbg_prt_full_stab());
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
	}

	br_tmp_assig_quantons.clear();

	if(inst_info.ist_with_assig){
		row_quanton_t& the_assig = br_tmp_assig_quantons;
		br_charge_trail.get_all_ordered_quantons(the_assig);
		get_ids_of(the_assig, inst_info.ist_assig);
		inst_info.ist_assig.push(0);	// means the last lit
	}
	
	BRAIN_DBG(
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

	close_all_maps();

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
brain::deactivate_all_maps(){
	while(! br_maps_active.is_empty()){
		deactivate_last_map();
	}
	BRAIN_CK(br_maps_active.is_empty());
}

void
brain::close_all_maps(){
	deactivate_all_maps();
	release_all_neuromaps();
	BRAIN_CK_PRT((br_num_active_neuromaps == 0), 
			os << "_______\n#act_nmps=" << br_num_active_neuromaps
				<< " #fr_nmps=" << br_free_neuromaps.size() 
				<< " #nmps=" << br_neuromaps.size() 
				<< bj_eol;
				dbg_prt_all_nmps(os);
	);
}

void
notekeeper::set_motive_notes(row_quanton_t& rr_qua, long from, long until){
	BRAIN_CK(ck_funcs());
	BRAIN_CK(dk_tot_noted >= 0);
	BRAIN_CK(dk_num_noted_in_layer >= 0);

	brain& brn = get_dk_brain();

	DBG_PRT(33, os << "making notes " << rr_qua);

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
	leveldat& lv = data_level();
	BRAIN_CK(lv.ld_idx == level());
	
	brain& brn = *this;
	lv.reset_monos(brn);
	lv.release_learned(brn);
	lv.let_maps_go(brn);

	br_current_ticket.tk_level--;
	leveldat* pt_lv = br_data_levels.pop();
	leveldat::release_leveldat(pt_lv);
}

/*
void
brain::write_all_lv_retracted(){
	row<neuromap*>& to_wrt = br_tmp_maps_to_write;
	leveldat& lv_dat = data_level();
	
	BRAIN_CK(to_wrt.is_empty());
	
	to_wrt.clear();
	lv_dat.ld_nmp_setup.append_all_as<neuromap>(to_wrt);
	write_all_neuromaps(to_wrt);
}*/

void
brain::retract_to(long tg_lv, bool full_reco)
{
	brain& brn = *this;
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
		
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}
	inc_recoil();
	
	DBG_PRT(103, os << "inc_recoil \n" << STACK_STR);
	
	BRAIN_CK((tg_lv == INVALID_LEVEL) || (level() == tg_lv));
	BRAIN_CK(full_reco || (tg_lv == INVALID_LEVEL) || (trail_level() == tg_lv));
	BRAIN_CK(! full_reco || (tg_lv == INVALID_LEVEL) || ((trail_level() + 1) == tg_lv));
}

quanton*
brain::receive_psignal(bool only_in_dom){
	//BRAIN_CK(br_qu_tot_note0 == 0);
	//BRAIN_CK(! found_conflict()); // simple_propag
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
	
	DBG_PRT(21, os << sgnl << " odom=" << only_in_dom; os << " HIT ENTER TO CONTINUE...");
	DBG_COMMAND(21, getchar());
	
	if(! qua.has_charge() && opp.is_note0()){
		//DBG_PRT(102, os << " CICLE qua skiped" << &qua);
		//BRAIN_CK(false);
		return NULL_PT;
	}
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
	
	qua.update_cicle_srcs(brn, neu);

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
			DBG_PRT_COND(35, (br_all_conflicts_found.size() > 1), 
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

		qua.set_charge(brn, neu, cg_positive, sg_tier);

		DBG_PRT_COND(30, (neu != NULL_PT), os << "qua=" << &qua << " SRC=" << neu);

		BRAIN_CK((qua.qu_source == neu) || 
			((level() == ROOT_LEVEL) && (qua.qu_source == NULL_PT)));
		BRAIN_CK((qua.qu_tier == sg_tier) || 
			((level() == ROOT_LEVEL) && (qua.qu_tier == 0)));
	}
	
	DBG_PRT(21, os << "init sgnl" << sgnl);
	sgnl.init_prop_signal();

	//DBG_PRT_COND(102, (pt_qua == NULL_PT), os << " CHARGED qua skiped " << &qua);
	return pt_qua;
}

bool
brain::ck_mono_propag(){
#ifdef FULL_DEBUG
	if(level() == ROOT_LEVEL){
		return true;
	}
	
	if(! data_level().is_ld_mono()){
		return true;
	}
	
	quanton* l_qua = br_charge_trail.last_quanton();
	
	BRAIN_CK(l_qua != NULL_PT);
	BRAIN_CK(l_qua->is_opp_mono());
	//BRAIN_CK(data_level().ld_chosen == l_qua);
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
		quanton* dbg_cho1 = NULL_PT;
		quanton* dbg_cho2 = NULL_PT;
		quanton* dbg_cho3 = NULL_PT;
	);
	
	row_quanton_t& all_impl_cho = br_tmp_all_impl_cho;
	all_impl_cho.clear();

	while(true){
		BRAIN_CK((dbg_cho1 == NULL_PT) == (dbg_cho2 == NULL_PT));
		BRAIN_CK((dbg_cho2 == NULL_PT) == (dbg_cho3 == NULL_PT));
		
		num_psigs1 = brn_tunnel_signals(true, all_impl_cho);
		
		BRAIN_CK_PRT(((dbg_cho1 == NULL_PT) || dbg_cho1->is_pos()),
				os << "_______________\n ABORT_DATA \n";
				dbg_prt_margin(os);
				os << "\n";
				print_trail(os);
				os << " dbg_cho1=" << dbg_cho1 << "\n";
				os << " dbg_cho2=" << dbg_cho2 << "\n";
				os << " dbg_cho3=" << dbg_cho3 << "\n";
		);
		BRAIN_CK((dbg_cho2 == NULL_PT) || dbg_cho2->is_pos());
		BRAIN_CK((dbg_cho3 == NULL_PT) || dbg_cho3->is_pos());
		BRAIN_DBG(
			dbg_cho1 = NULL_PT;
			dbg_cho2 = NULL_PT;
			dbg_cho3 = NULL_PT;
		);
		
		nw_cho = get_cicles_common_cho(old_cho, all_impl_cho);
		if(nw_cho == NULL_PT){
			break;
		}
		if(nw_cho != NULL_PT){
			BRAIN_CK(! nw_cho->is_opp_mono());
			BRAIN_CK(! curr_choice().is_opp_mono());
			BRAIN_DBG(
				dbg_cho1 = data_level().ld_chosen;
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
				os << " dbg_cho1=" << dbg_cho1 << "\n";
				os << " dbg_cho2=" << dbg_cho2 << "\n";
				os << " dbg_cho3=" << dbg_cho3 << "\n";
				os << " nw_cho=" << nw_cho << "\n";
				os << " old_cho=" << old_cho << "\n"
			);
			BRAIN_CK(old_cho->qlevel() < level());
			BRAIN_REL_CK(old_cho->qlevel() < level());
			
			replace_choice(*old_cho, *nw_cho);
			if(found_conflict()){
				reset_conflict();
			}
			//all_impl_cho.clear();
			//break;
		} 
	}
	
	all_impl_cho.clear();

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

	update_monos();
	
	BRAIN_CK(ck_mono_propag());
	
	long tot_psigs = num_psigs1 + num_psigs2;
	return tot_psigs;
}

void
brain::start_propagation(quanton& nxt_qua){
	BRAIN_CK(data_level().ld_idx == level());
	
	quanton& qua = nxt_qua;
	
	BRAIN_CK(! qua.is_mono() || qua.is_opp_mono());

	inc_level(qua);

	BRAIN_DBG(long prv_lv = level());
	BRAIN_CK(data_level().ld_chosen == &qua);

	BRAIN_CK(! has_psignals());
	send_psignal(qua, NULL, tier() + 1);
	BRAIN_CK(has_psignals());

	if(qua.is_mono() || qua.is_opp_mono()){
		return;
	}

	long num1 = propagate_signals();
	BRAIN_DBG(long old_lv = level());
	BRAIN_CK(old_lv <= prv_lv);
	quanton& prv_cho1 = curr_choice();
	BRAIN_CK(num1 > 0);

	if(! found_conflict()){
		quanton& opp_cho1 = prv_cho1.opposite();
		replace_choice(prv_cho1, opp_cho1);
		
		long num2 = propagate_signals();
		quanton& prv_cho2 = curr_choice();
		BRAIN_CK(num2 > 0);

		if(! found_conflict() && (&opp_cho1 == &prv_cho2)){
			if(num1 > num2){
				replace_choice(prv_cho2, prv_cho1);
				BRAIN_CK(old_lv == level());
			}

			if((num1 == num2) && prv_cho1.qu_has_been_cho){
				replace_choice(prv_cho2, prv_cho1);
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
		DBG(
			ch_string htm_str = "";
			ch_string rc_str = recoil().get_str();
			htm_str += "BRN_recoil=" + rc_str + "<br>";
		);
		DBG_PRT(151, os << "bef_htm=" << bj_eol; print_trail(os);
			os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
			<< " br_ti=" << tier();
		);
		DBG_COMMAND(151, dbg_update_html_cy_graph(CY_IC_KIND, NULL_PT, htm_str));
	
		br_last_retract = (level() == ROOT_LEVEL);
		if(level() == ROOT_LEVEL){ 
			set_result(bjr_no_satisf);
			return;
		}
		//dbg_old_reverse();
		bool go_on = deduce_and_reverse();
		if(! go_on){
			//BRAIN_CK(false);
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
				cho.qu_dbg_fst_lap_cho = brn.br_current_ticket.tk_recoil;
				br_dbg.dbg_all_chosen.push(&cho);
			)
		}
		BRAIN_DBG(if(cho.opposite().qu_has_been_cho){ cho.qu_dbg_num_laps_cho++; });
		DBG_PRT(25, os << "**CHOICE** " << cho);
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

/*
void
brain::retract_choice(){
	BRAIN_CK(! found_conflict());

	quanton* chosen_qua = data_level().ld_chosen;
	BRAIN_CK(chosen_qua != NULL_PT);
	BRAIN_CK(chosen_qua->qlevel() == level());
	
	quanton& opp = chosen_qua->opposite();
	
	replace_choice(*chosen_qua, opp);
}*/

void
brain::replace_choice(quanton& cho, quanton& nw_cho){
	BRAIN_CK(! has_psignals());
	brain& brn = *this;

	BRAIN_CK_0(level() != ROOT_LEVEL);

	DBG_PRT(14, print_trail(os); os << "chosen " << br_chosen);

	long tgt_lvl = cho.qlevel();
	
	// retract loop

	retract_to(tgt_lvl, true);

	BRAIN_CK((level() == ROOT_LEVEL) || ((trail_level() + 1) == tgt_lvl));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == tgt_lvl));

	// resend opp chosen

	data_level().reset_monos(brn);

	DBG_PRT(25, os << "**NEW_CHO=" << &nw_cho);

	data_level().ld_chosen = &nw_cho;

	BRAIN_CK(! has_psignals());
	send_psignal(nw_cho, NULL, tier() + 1);
	BRAIN_CK(has_psignals());
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
	ch_string js_dir = rn_dir + "../" + CY_LIB_DIR;
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

	BRAIN_DBG(bool init_htm = false);
	DBG_COMMAND(45, init_htm = true);
	DBG_COMMAND(150, init_htm = true);
	DBG_COMMAND(151, init_htm = true);
	BRAIN_DBG(if(init_htm){ dbg_init_html(); });
	
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

	BRAIN_DBG(if(init_htm){ dbg_finish_html(); });
	
	return o_info.bjo_result;
}

bool
brain::deduce_and_reverse(){
	bool go_on = true;
	BRAIN_CK(! has_psignals());
	
	BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(br_qu_tot_note0 == 0);
	BRAIN_CK(found_conflict());

	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());
	
	// analyse
	
	BRAIN_DBG(if(level() > br_dbg.dbg_max_lv){ br_dbg.dbg_max_lv = level(); });
	
	deduction& dct = br_retract_dct;

	bool in_full_anls = true;
	DBG_COMMAND(2, in_full_anls = false); // ONLY_DEDUC
	if(in_full_anls){
		go_on = analyse(br_all_conflicts_found, dct);
	} else {
		DBG_PRT(40, os << "bef_ana=" << bj_eol; print_trail(os);
			os << " num_conf=" << br_all_conflicts_found.size() << " br_lv=" << level()
			<< " br_ti=" << tier();
		);
		
		br_deducer_anlsr.set_conflicts(br_all_conflicts_found);
		br_deducer_anlsr.deduction_analysis(br_deducer_anlsr.get_first_causes(), dct);
	}
	
	reverse(dct);
	return go_on;
}

void
brain::reverse(deduction& dct){
	// retract
	retract_to(dct.dt_target_level, false);
	
	// some checks

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);
	BRAIN_DBG(long rr_lv = trail_level());
	
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == dct.dt_target_level));
	BRAIN_CK(dct.dt_forced != NULL_PT);

	// learn motives
	
	neuron* lnd_neu = learn_mots(dct);
	
	// send forced learned

	quanton* nxt_qua = dct.dt_forced;
	if(! dct.dt_motives.is_empty()){
		BRAIN_CK(nxt_qua != NULL_PT);
		send_psignal(*nxt_qua, lnd_neu, tier() + 1);
	}

	// finish reverse

	BRAIN_DBG(
		br_dbg_round++;
		dbg_update_config_entries(get_solver().slv_dbg_conf_info, br_dbg_round);
	);
	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());
	BRAIN_CK((level() == ROOT_LEVEL) || ! data_level().is_ld_mono());

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
	
	neus_srg.stab_mutual(quas_srg);
	
	//bj_out << "ALL_CCL=\n";
	
	canon_cnf& the_cnf = neus_srg.get_final_cnf(skg, "TEST_STAB", true);

	bj_out << "THE_CNF=" << bj_eol;
	bj_out << the_cnf;
	bj_out << "END_of_cnf=" << bj_eol;
	
	coloring finl_col;
	finl_col.save_colors_from(neus_srg, quas_srg, tid_tee_consec);
	
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
		
		BRAIN_CK(qu_all_neus.is_valid_idx(qu_alert_neu_idx));
		BRAIN_CK(qu_all_neus[qu_alert_neu_idx] != NULL_PT);
		BRAIN_CK(qu_all_neus[qu_alert_neu_idx]->is_ne_alert());
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
		//ar_qu_alert->qu_mono_alerts.bind_to_my_right(ar_alert);
		ar_qu_alert->qu_mono_refs.bind_to_my_right(ar_ref);
	}
	
	if(ar_qu_ref != NULL_PT){
		//ar_qu_ref->qu_mono_refs.bind_to_my_right(ar_ref);
		ar_qu_ref->qu_mono_alerts.bind_to_my_right(ar_alert);
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
			BRAIN_CK_PRT(neu->is_ne_alert(), os << "_______________\n";
				brn.dbg_prt_margin(os);
				os << " lvs_sz=" << brn.br_data_levels.size();
				os << " ld_idx=" << ld_idx << " qua=" << &qua << " neu=" << neu;
			);
		);
	}

	if(brn.data_level().ld_bak_mono_idx != INVALID_IDX){
		brn.br_last_monocho_idx = brn.data_level().ld_bak_mono_idx;
	} else {
		brn.br_last_monocho_idx = 0;
	}
	BRAIN_CK(brn.ck_prev_monos());
}

void
brain::fill_mono_dct(deduction& dct, row_quanton_t& mots, quanton& mono){
	BRAIN_DBG(brain& brn = *this);
	
	dct.init_deduction();
	mots.move_to(dct.dt_motives);
	dct.dt_target_level = find_max_level(dct.dt_motives);
	dct.dt_forced = &mono;
	
	bool br_in_root = (level() == ROOT_LEVEL);

	BRAIN_CK(ck_motives(brn, dct.dt_motives));

	DBG_PRT(20, os << "find_dct of deduction=" << dct);
	
	if(br_in_root){
		dct.dt_motives.clear();
		BRAIN_CK(dct.dt_target_level == ROOT_LEVEL);
		BRAIN_CK(dct.dt_motives.is_empty());
		dct.dt_target_level = INVALID_LEVEL;
	}
}

bj_ostream&
brain::dbg_print_all_qua_rels(bj_ostream& os){
	row<alert_rel*> all_alert;
	row<alert_rel*> all_refs;
	
	os << "ALL_RELS\n";
		
	//brain& brn = *this;
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
	br_mono_dct.init_deduction();
	
	if(data_level().ld_bak_mono_idx == INVALID_IDX){
		data_level().ld_bak_mono_idx = br_last_monocho_idx;
	}
	
	quanton* qua = get_curr_mono();
	while((qua != NULL_PT) && qua->has_charge()){
		br_last_monocho_idx++;
		qua = get_curr_mono();
	}
	//BRAIN_CK(ck_prev_monos());
	if(qua == NULL_PT){
		DBG_PRT(103, os << "null cho_mono \n";
			os << " br_last_monocho_idx=" << br_last_monocho_idx << "\n";
			os << " br_monos_sz=" << br_monos.size() << "\n";
			os << " br_monos=" << br_monos << "\n";
		);
		return NULL_PT;
	}
	BRAIN_CK(ck_prev_monos());
	
	long alrt_idx = qua->find_alert_idx(true, br_tmp_mono_all_neg);
	MARK_USED(alrt_idx);
	BRAIN_CK(alrt_idx == INVALID_IDX);
	BRAIN_CK(br_tmp_mono_all_neg.size() == qua->qu_all_neus.size());
	
	BRAIN_CK(qua->is_mono());
	quanton& opp = qua->opposite();
	
	fill_mono_dct(br_mono_dct, br_tmp_mono_all_neg, opp);
	
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
	dbg_init_dbg_conf(slv_dbg_conf_info);
}

void			
quanton::set_source(brain& brn, neuron* neu){
	BRAIN_CK((qu_source == NULL_PT) || (qu_inverse->qu_source == NULL_PT));
	BRAIN_CK((qu_inverse->qu_source == NULL_PT) || (neu == NULL_PT));
	BRAIN_CK((neu == NULL_PT) || neu->ne_original || ! neu->ne_dbg_used_no_orig);

	BRAIN_DBG(
		if((neu != NULL_PT) && ! neu->ne_original){
			BRAIN_CK(neu->ne_dbg_used_no_orig == false);
			neu->ne_dbg_used_no_orig = true;
		}

		if(! has_charge()){
			BRAIN_CK(neu == NULL_PT);
			BRAIN_CK(qu_inverse->qu_source == NULL_PT);
			if((qu_source != NULL_PT) && ! qu_source->ne_original){
				qu_source->ne_dbg_used_no_orig = false;
			}		
		}
	)
	
	qu_source = neu;
	if(neu == NULL_PT){ 
		qu_inverse->qu_source = NULL_PT;
		return; 
	}

	BRAIN_CK_0(qu_source != NULL_PT);
	BRAIN_CK(qu_source->ck_all_charges(&brn, 1));
}

void
brain::get_bineu_sources(quanton& cho, quanton& qua, row_quanton_t& all_biqus)
{
	brain& brn = *this;
	
	BRAIN_CK(cho.is_pos());
	BRAIN_CK(qua.is_pos());

	if(! is_ticket_eq(qua.qu_cicle_tk, br_current_ticket)){
		DBG_PRT(68, 
			os << " RESET CICLES qua=" << &qua << "\n";
			os << " clc_tk=" << qua.qu_cicle_tk << "\n";
			os << " br_tk=" << br_current_ticket << "\n";
			os << " clc_srcs=" << qua.qu_all_cicle_sources << "\n";
		);
		qua.reset_cicle_src();
	}
	
	row_neuron_t& all_src = qua.qu_all_cicle_sources;

	DBG_PRT(68, 
		os << ".cho=" << &cho << "\n";
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
	BRAIN_CK(br_qu_tot_note1 == 0);
	
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
			//get_bineu_sources(cho, opp, nxt_biqus);
		}
	}
	prv_biqus.append_to(all_src);
	prv_biqus.clear();
	
	reset_all_note1(brn, all_src);
	BRAIN_CK(br_qu_tot_note1 == 0);
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
		if(is_n){
			long q_lv = qua->qlevel();
			bool is_biqu = (b_lv == q_lv);
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
		if(! biqu->has_note1()){
			biqu->set_note1(brn);
			all_biqus.push(biqu);
			DBG_PRT(68, os << " bineu=" << this);
		}
	}
}

void
brain::get_all_cicle_cho(row_quanton_t& all_cicl){
	brain& brn = *this;
	
	all_cicl.clear();
	
	BRAIN_CK(! is_curr_cho_mono());
	
	BRAIN_CK(br_qu_tot_note1 == 0);
	set_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
	
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
		leveldat& lv_dat = get_data_level(qlv);
		BRAIN_CK(lv_dat.ld_chosen == &qua);
		if(lv_dat.has_learned()){
			break;
		}
		BRAIN_CK(qua.is_cicle_choice());

		all_cicl.push(&qua);
	}

	reset_ps_all_note1_n_tag1(brn, br_all_conflicts_found, true, false);
	BRAIN_CK(br_qu_tot_note1 == 0);
}

void
quanton::update_cicle_srcs(brain& brn, neuron* neu){
	if(is_cicle_choice() && (neu != NULL_PT) && is_pos()){
		if(! is_ticket_eq(qu_cicle_tk, brn.br_current_ticket)){
			DBG_PRT_COND(68, ((abs_id() == 8) || (abs_id() == 9)),
				os << " RESET CICLES qua=" << this << "\n";
				os << " clc_tk=" << qu_cicle_tk << "\n";
				os << " br_tk=" << brn.br_current_ticket << "\n";
				os << " clc_srcs=" << qu_all_cicle_sources << "\n";
			);
			reset_cicle_src();
		}
		if(qu_cicle_tk.is_tk_virgin()){
			qu_cicle_tk.update_ticket(brn);
		}
		//DBG_PRT(DBG_ALL_LVS, os << " neu=" << neu);
		qu_all_cicle_sources.push(neu);
	}
}

quanton*
brain::get_cicles_common_cho(quanton*& old_cho, row_quanton_t& all_impl_cho)
{
	all_impl_cho.clear();
	old_cho = NULL_PT;
	
	if(is_curr_cho_mono()){
		return NULL_PT;
	}
	
	row_quanton_t& all_cicl = br_tmp_all_cicles;
	get_all_cicle_cho(all_cicl);
	
	if(all_cicl.size() < 2){
		return NULL_PT;
	}
	
	brain& brn = *this;
	quanton* nw_cho = NULL_PT;
	
	DBG_PRT(68, os << "Entering get_cicles_common.cho");
	
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
		return NULL_PT;
	}
	
	BRAIN_CK(! bqs_1.is_empty());
	BRAIN_DBG(long cho_lv = cho_1.qlevel());
	
	for(long aa = 1; aa < all_cicl.size(); aa++){
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
			
		BRAIN_CK(br_qu_tot_note1 == 0);
		set_all_note1(brn, bqs_1);
		append_all_has_note1(brn, bqs_2, bqs_3);
		reset_all_note1(brn, bqs_1);
		BRAIN_CK(br_qu_tot_note1 == 0);

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
	
	DBG_PRT(68, os << "FINISHING get_cicles_common.cho");
	
	return nw_cho;
}

void
quanton::reset_cicle_src(){
	qu_cicle_tk.init_ticket();
	opposite().qu_cicle_tk.init_ticket();
	
	qu_all_cicle_sources.clear();
	opposite().qu_all_cicle_sources.clear();
}

bool
quanton::is_learned_choice(){
	neuron* neu = get_source();
	if(neu == NULL_PT){
		return false;
	}
	if(neu->ne_original){
		return false;
	}
	return true;
}

leveldat*
quanton::qlv_dat(brain& brn){
	if(! has_charge()){
		return NULL_PT;
	}
	long lv = qlevel();
	leveldat& lv_dat = brn.get_data_level(lv);
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
	update_monos();
	quanton* qua = choose_mono();
	if(qua == NULL_PT){
		DBG_PRT(102, os << "send_mono. null choose_mono \n";
			os << " br_last_monocho_idx=" << br_last_monocho_idx << "\n";
			os << " br_monos_sz=" << br_monos.size() << "\n";
			os << " br_monos=" << br_monos << "\n";
			os << " curr_cho=" << curr_cho() << "\n";
			os << " trail_last=" << &(trail_last()) << "\n";
			os << STACK_STR << "\n\n"
		);
		return;
	}
	DBG_PRT(102, os << "send_mono. br_monos_sz=" << br_monos.size() 
		<< " br_monos=" << br_monos << "\n NEXT mono=" << qua);
	
	BRAIN_CK(qua->is_opp_mono());
	send_psignal(*qua, NULL, tier());
	BRAIN_CK(has_psignals());
}

