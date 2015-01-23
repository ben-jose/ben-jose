

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

brain.cpp  

Classes and that implement the neural network.

--------------------------------------------------------------*/

#include "stack_trace.h"
#include "file_funcs.h"
#include "parse_funcs.h"
#include "sortor.h"
#include "brain.h"
#include "solver.h"
#include "dimacs.h"
#include "dbg_run_satex.h"
#include "dbg_prt.h"
#include "dbg_ic.h"
#include "dbg_config.h"

//br_qu_tot_note0;

DEFINE_NI_FLAG_FUNCS(qu_flags, note0, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note1, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note2, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note3, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note4, false);
DEFINE_NI_FLAG_FUNCS(qu_flags, note5, false);

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


#define PRINT_PERIOD			4.0
#define SOLVING_TIMEOUT			0.0		// 0.0 if no timeout

//============================================================
// static vars

char*	quanton::CL_NAME = as_pt_char("{quanton}");
char*	neuron::CL_NAME = as_pt_char("{neuron}");

//============================================================
// random generator

long gen_random_num(long min, long max);

// search dbg_ic to config it

//============================================================
// aux funcs

bool
has_neu(row<neuron*>& rr_neus, neuron* neu){
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
		//BRAIN_CK(qua != &(brn.br_conflict_quanton));
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
		//BRAIN_CK(qua != &(brn.br_conflict_quanton));
		BRAIN_CK(qua != &(brn.br_top_block));
		if(qua->has_dot()){
			qua->reset_dot(brn);
			resetted++;
		}
	}
	return resetted;
}

void	set_marks_of(brain& brn, row<prop_signal>& trace, long first_idx, long last_idx, 
					 bool with_related){
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		quanton* qua = q_sig.ps_quanton;
		quanton* opp = qua->qu_inverse;

		//BRAIN_CK(qua != &(brn.br_conflict_quanton));
		BRAIN_CK(qua != &(brn.br_top_block));

		BRAIN_CK_0(! qua->has_mark());
		qua->set_mark(brn);

		if(with_related){
			qua->qu_reltee.init_sortrel();
			opp->qu_reltee.init_sortrel();

			if(qua->qu_mark_idx == INVALID_IDX){
				BRAIN_CK(opp->qu_mark_idx == INVALID_IDX);
				qua->qu_mark_idx = ii;
				opp->qu_mark_idx = ii;
			}
			if(q_sig.ps_source != NULL_PT){
				q_sig.ps_source->set_spot(brn);
			}
		}
	}
}

void	reset_marks_of(brain& brn, row<prop_signal>& trace, long first_idx, long last_idx, 
					   bool with_related){
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		quanton* qua = q_sig.ps_quanton;
		quanton* opp = qua->qu_inverse;

		//BRAIN_CK(qua != &(brn.br_conflict_quanton));
		BRAIN_CK(qua != &(brn.br_top_block));

		BRAIN_CK(qua->has_pos_mark());
		qua->reset_mark(brn);

		if(with_related){
			if(qua->qu_mark_idx != INVALID_IDX){
				BRAIN_CK(opp->qu_mark_idx != INVALID_IDX);
				qua->qu_mark_idx = INVALID_IDX;
				opp->qu_mark_idx = INVALID_IDX;
			}
			if(q_sig.ps_source != NULL_PT){
				q_sig.ps_source->reset_spot(brn);
			}
		}

		BRAIN_CK(qua->qu_mark_idx == INVALID_IDX);
		BRAIN_CK(opp->qu_mark_idx == INVALID_IDX);
	}
}

//============================================================
// neuron methods

quanton*
neuron::update_fibres(row_quanton_t& synps, bool orig){
	long num_neutral = 0;
	long num_neg_chgs = 0;

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

		DBG(
			//long qtrl_sz = INVALID_IDX;
			//long lst_trl_sz = INVALID_IDX;
			long id0 = INVALID_IDX;
			long id1 = INVALID_IDX;
		);
		BRAIN_CK(fib_sz() >= 2);

		for(long ii = 0; ii < fib_sz(); ii++){
			quanton* qua = ne_fibres[ii];
			DBG(
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
	ne_recoil_tk.init_ticket();
	ne_deduc_tk.init_ticket();

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

		DBG(
			long new_max = old_max;
			long max_lev = ne_fibres[new_max]->qlevel();
			long qua_lev = qua->qlevel()
		);

		long ii = old_max;
		for(ii = old_max; ii > 1; ii--){
			charge_t fib_chg = ne_fibres[ii]->get_charge();

			max_tier = max(max_tier, ne_fibres[ii]->qu_tier);

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
			DBG(
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
			max_tier = max(max_tier, ne_fibres[aa]->qu_tier);
		}

		max_tier = max(max_tier, ne_fibres[1]->qu_tier);

		quanton* forced_qua = forced_quanton();
		BRAIN_CK(forced_qua != NULL_PT);

		brn.send_psignal(*forced_qua, this, max_tier + 1);

		BRAIN_CK_2(ne_fibres[0]->ck_all_tunnels());
		BRAIN_CK_2(ne_fibres[1]->ck_all_tunnels());
		dbg1 = true;

		DBG_PRT(102, os << "forcing " << this);
	}

	if((cg0 != cg_neutral) && (cg1 != cg_neutral)){
		DBG(long npos = 0);
		BRAIN_CK(ck_all_has_charge(npos));
		//if(ne_original && (cg0 == cg_positive) && (cg1 == cg_positive)){
		if(ne_original){
			DBG(ne_dbg_filled_tk.update_ticket(brn));
			r_qua.qu_full_charged.push(this);
			DBG_PRT(102, os << "qua=" << &r_qua << " filled orig " << this);
		}
		dbg2 = true;
	}

	update_uncharged(brn, &r_qua);

	BRAIN_CK(! q_neg || dbg1 || ! ck_all_charges(&brn, 1));
	DBG(long npos2 = 0; bool ck2 = ck_all_has_charge(npos2); );
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
	BRAIN_DBG(br_pt_brn = NULL);
	br_pt_slvr = &ss;
	
	br_prt_timer.init_timer(PRINT_PERIOD, SOLVING_TIMEOUT);

	br_start_load_tm = 0.0;

	// debug attributes

	DBG(br_dbg.dbg_last_recoil_lv = INVALID_LEVEL);

	// temp attributes

	// config attributes
	br_file_name = "<file_name>";
	br_file_name_in_ic = "<file_name_in_ic>";

	// state attributes
	br_choice_spin = cg_neutral;
	br_choice_order = k_invalid_order;
	//br_choices_lim = INVALID_IDX;
	
	br_charge_trail.init_qulayers(this);

	br_first_psignal = 0;
	br_last_psignal = 0;

	br_retract_nke0.init_notekeeper(this);
	br_retract_nke0.init_funcs(&br_qu_tot_note0, &quanton::has_note0, 
							   &quanton::set_note0, &quanton::reset_its_note0, 
								&set_all_note0, &reset_all_its_note0,
								&append_all_not_note0, &same_quantons_note0
  							);

	br_deducer_anlsr.init_analyser(this);
	br_neuromaper_anlsr.init_analyser(this);
	
	reset_conflict();

	br_num_memo = 0;

	//br_conflict_quanton.init_quanton(this, cg_neutral, INVALID_IDX, NULL);
	br_top_block.init_quanton(this, cg_neutral, INVALID_IDX, NULL);
	
	//DBG_CK(br_conflict_quanton.qu_tee.is_alone());
	DBG_CK(br_top_block.qu_tee.is_alone());

	//br_tot_qu_spot0 = 0;
	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	init_tots_notes();
	init_tots_tags();

	DBG(
		init_all_dbg_brn();  // sets br_pt_brn indicating it is readi for DBG_PRT
		dbg_init_dbg_conf(*this);	
	);
}

void
brain::release_brain(){
	BRAIN_DBG(br_pt_brn = NULL);
	get_skeleton().set_dbg_brn(NULL);

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

	br_forced_srg.release_all();
	br_filled_srg.release_all();

	br_guide_neus_srg.release_all();
	br_guide_quas_srg.release_all();

	br_tauto_neus_srg.release_all();
	br_tauto_quas_srg.release_all();

	get_skeleton().clear_all();

	//br_conflict_quanton.init_quanton(this, cg_neutral, INVALID_IDX, NULL);
	br_top_block.init_quanton(this, cg_neutral, INVALID_IDX, NULL);

	BRAIN_CK(br_tot_qu_dots == 0);
	//BRAIN_CK(br_tot_qu_marks == 0);
	//BRAIN_CK(br_tot_ne_spots == 0);

	//DBG_PRT(DBG_ALL_LVS, os << "release_brain 4");
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
	//BRAIN_CK(this != &(brn.br_conflict_quanton));
	BRAIN_CK(this != &(brn.br_top_block));

	BRAIN_CK((cha == cg_positive) || (cha == cg_neutral));
	BRAIN_CK(! is_neg());
	BRAIN_CK(! is_pos() || (cha == cg_neutral));
	BRAIN_CK(has_charge() || (cha == cg_positive));

	bool in_root = (brn.level() == ROOT_LEVEL);
	bool with_src_before = has_source();

	if(in_root){ 
		BRAIN_CK(cha != cg_neutral);
		neu = NULL; 

		n_tier = 0;
	}

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
		BRAIN_CK(qlevel() != ROOT_LEVEL);

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
	BRAIN_CK(! found_conflict());
	BRAIN_CK(ck_trail());
	DBG(
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

	DBG(
		ticket tk2;
		tk2.update_ticket(brn);
		BRAIN_CK_0(is_ticket_eq(tk1, tk2));
	);

	return num_psigs;
}

quanton*	
brain::choose_quanton(){ 
	BRAIN_CK(ck_trail());

	quanton* qua = NULL;
	//brain& brn = *this;

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

	//BRAIN_CK(ck_choices());

	for(long ii = 0; ii < br_choices.size(); ii++){
		quanton* qua_ii = br_choices[ii];
		if(qua_ii->get_charge() == cg_neutral){
			if(! qua_ii->is_semi_mono()){
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

	//BRAIN_CK(ck_choices(true));

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
	//br_conflict_quanton.qu_inverse = &(br_conflict_quanton);
	//BRAIN_CK(br_conflict_quanton.get_source() == NULL_PT);

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

	//br_conflict_quanton.qu_charge_tk.update_ticket(brn);
	br_top_block.qu_charge_tk.update_ticket(brn);

	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	init_tots_notes();
	init_tots_tags();

	DBG(
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
		the_neu = add_neuron(the_mots, f_qua, false);
		BRAIN_CK(the_neu != NULL_PT);
		//BRAIN_CK(f_qua == &forced_qua);

		data_level().ld_learned.push(the_neu);
	}

	DBG_PRT(23, os << "added_forced quanton: " << forced_qua;
		if(the_neu == NULL_PT){ os << " IS_SINGLETON"; } 
	);

	if(the_mots.is_empty()){
		long nxt_tir = 0;
		BRAIN_CK(level() == ROOT_LEVEL);
		send_psignal(forced_qua, the_neu, nxt_tir);
	}

	DBG(
		if(br_dbg.dbg_ic_active && br_dbg.dbg_ic_after){
			row_quanton_t& the_trl = br_tmp_trail;
			br_charge_trail.get_all_ordered_quantons(the_trl);
			dbg_ic_print(*this, the_trl);
		}
	)
	return the_neu;
}

neuron*
brain::add_neuron(row_quanton_t& quans, quanton*& forced_qua, bool orig){
	brain& brn = *this;
	neuron& neu = locate_neuron();
	neu.ne_original = orig;

	forced_qua = neu.update_fibres(quans, orig);

	neu.update_uncharged(brn, NULL_PT);
	BRAIN_CK(neu.fib_sz() > 0);
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[0]->ck_uncharged_partner_neu());
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[1]->ck_uncharged_partner_neu());
	DBG(neu.ne_dbg_creation_tk.update_ticket(brn));

	DBG_PRT(26, os << "adding " << neu);
	return &neu;
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

	dbg_reset_ic_files(*this);
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

void
brain::load_neuron(row_quanton_t& neu_quas){
	DBG_PRT(29, os << "ADDING NEU=" << neu_quas);

	BRAIN_CK_0(neu_quas.size() > 0);
	if(neu_quas.size() > 1){
		bool is_orig = true;
		quanton* forced_qua = NULL;
		add_neuron(neu_quas, forced_qua, is_orig);
		BRAIN_CK_0(forced_qua == NULL);
	} else {
		BRAIN_CK_0(neu_quas.size() == 1);
		BRAIN_CK_0(level() == ROOT_LEVEL);

		neuron& neu = locate_neuron();
		neu.ne_original = false;
		neu.ne_fibres.push(neu_quas.first());
		br_unit_neurons.push(&neu);

		send_psignal(*(neu_quas.first()), NULL, 0);
	}
	neu_quas.clear();
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

			load_neuron(neu_quas);
			if(get_out_info().bjo_result != bjr_unknown_satisf){ 
				break; 
			}

			//first = ii + 1;
			
			neu_quas.clear();
		}
	}

	//BRAIN_CK(net.nt_neurons.size() == num_neu_added);

	init_uncharged();

	double end_load_tm = run_time();
	double ld_tm = (end_load_tm - br_start_load_tm);	
	get_out_info().bjo_load_time = ld_tm;

	ch_string f_nam = inst_info.get_f_nam();
	return true;
}

void
brain::fill_with_origs(row<neuron*>& neus){
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
brain::aux_solve_instance(){
	instance_info& inst_info = get_my_inst();
	bj_output_t& o_info = get_out_info();
	
	bool all_ok = load_instance();
	if(! all_ok){
		throw instance_exception(inx_bad_lit);
	}

	get_skeleton().kg_instance_file_nam = inst_info.get_f_nam() + "\n";
	
	if(o_info.bjo_result != bjr_unknown_satisf){ 
		return;
	}

	DBG(
		if(br_dbg.dbg_just_read){
			set_result(bjr_error);
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
	
	DBG(
		DBG_PRT(116, dbg_prt_all_cho(*this));
		DBG_PRT(32, os << "BRAIN=" << bj_eol;
			print_brain(os); 
		);

		br_final_msg << f_nam << " ";

		bj_satisf_val_t resp_solv = o_info.bjo_result;
		if(resp_solv == bjr_yes_satisf){
			dbg_check_sat_assig();
			br_final_msg << "IS_SAT_INSTANCE";
		} else if(resp_solv == bjr_no_satisf){
			br_final_msg << "IS_UNS_INSTANCE";
		}

		bj_out << br_final_msg.str() << bj_eol; 
		bj_out.flush();

		if(br_dbg.dbg_ic_active){
			row_quanton_t& the_trl = br_tmp_trail;
			br_charge_trail.get_all_ordered_quantons(the_trl);
			dbg_ic_print(*this, the_trl);
		}
	);

	close_all_maps();

	if(level() != ROOT_LEVEL){
		retract_to();
	}

	br_psignals.clear(true, true);
	br_delayed_psignals.clear(true, true);

	br_forced_srg.release_all();
	br_filled_srg.release_all();

	all_mutual_init();
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

	long ii = from;
	for(; ii < until; ii++){
		BRAIN_CK(rr_qua[ii] != NULL_PT);
		quanton& qua = *(rr_qua[ii]);
		BRAIN_CK(qua.get_charge() == cg_negative);

		bool has_note = (qua.*dk_has_note_fn)();
		if(! has_note && (qua.qlevel() != ROOT_LEVEL)){
			dk_tot_noted++;
			(qua.*dk_set_note_fn)(brn);

			BRAIN_CK(dk_note_layer != INVALID_LEVEL);
			DBG(ch_string dbg_msg = "");
			long q_lv = qua.qlevel();

			dk_quas_lyrs.add_motive(qua, q_lv);

			BRAIN_CK(q_lv <= dk_note_layer);
			if(q_lv == dk_note_layer){
				DBG(dbg_msg = "making note ");
				dk_num_noted_in_layer++;
			} 
			DBG(
				if(q_lv != dk_note_layer){
					BRAIN_CK(q_lv < dk_note_layer);
					DBG(dbg_msg = "motive ");
				}
			);
			DBG_PRT(20, os << dbg_msg << qua << " num_notes_in_lv " << dk_num_noted_in_layer 
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
	DBG(
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
deduction::set_with(brain& brn, notekeeper& nke, quanton& nxt_qua){
	if(! is_dt_virgin()){
		return;
	}

	quanton& opp_nxt = *(nxt_qua.qu_inverse);

	BRAIN_CK(opp_nxt.qlevel() == nke.dk_note_layer);
	BRAIN_CK(opp_nxt.get_charge() == cg_negative);

	nke.dk_quas_lyrs.get_all_ordered_quantons(dt_motives);

	DBG_PRT(51, os << " motives_by_lv= " << nke.dk_quas_lyrs.dk_quas_by_layer);
	DBG_PRT(52, os << "LV=" <<  nke.dk_note_layer << " motives " 
		<< dt_motives << " opp_nxt=" << &opp_nxt);

	dt_target_level = find_max_level(dt_motives);

	dt_forced = &opp_nxt;
	//dt_forced_level = opp_nxt.qlevel();

	BRAIN_CK(dt_target_level < nke.dk_note_layer);
	BRAIN_CK(ck_motives(brn, dt_motives));
	BRAIN_CK(! is_dt_virgin());
}

void
neuron::old_set_motives(brain& brn, notekeeper& nke, bool is_first){
	neuron& neu = *this;

	BRAIN_CK(! ne_fibres.is_empty());
	BRAIN_CK(is_first || (ne_fibres[0]->get_charge() == cg_positive) );
	BRAIN_CK(is_first || neu_compute_binary());

	ne_recoil_tk.update_ticket(brn);

	DBG(brn.dbg_add_to_used(neu));

	row_quanton_t& causes = ne_fibres;
	BRAIN_CK(! causes.is_empty());

	long from = (is_first)?(0):(1);
	long until = causes.size();
	nke.set_motive_notes(neu.ne_fibres, from, until);
}

void
brain::retract_to(long tg_lv){
	BRAIN_CK(br_semi_monos_to_update.is_empty());

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
		
		BRAIN_CK_PRT(br_charge_trail.has_motives(),
			os << recoil() << ".lv=" << level() << " tg_lv=" << tg_lv;
			os << " trail_lv=" << trail_level() << bj_eol;
			print_trail(os)
		);

		quanton& qua = trail_last();
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}
	BRAIN_CK(level() == tg_lv);
	BRAIN_CK(trail_level() == tg_lv);
	
	update_semi_monos();
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
	BRAIN_CK(! found_conflict());
	BRAIN_CK(has_psignals());
	brain& brn = *this;
	prop_signal& sgnl = pick_psignal();

	BRAIN_CK(sgnl.ps_quanton != NULL_PT);
	quanton* pt_qua = sgnl.ps_quanton;

	quanton& qua = *(pt_qua);
	neuron* neu = sgnl.ps_source;

	if(only_in_dom && ! qua.in_qu_dominated(brn)){
		br_delayed_psignals.push(sgnl);
		return NULL_PT;
	}

	if(only_in_dom && (neu != NULL_PT) && neu->ne_original && ! neu->in_ne_dominated(brn)){
		br_delayed_psignals.push(sgnl);
		return NULL_PT;
	}

	long sg_tier = sgnl.ps_tier;
	BRAIN_CK(sg_tier != INVALID_TIER);

	sgnl.init_prop_signal();

	if(qua.has_charge()){
		if(qua.is_neg()){
			BRAIN_CK((neu != NULL_PT) || (level() == ROOT_LEVEL));
			br_conflict_found.init_prop_signal(pt_qua, neu, sg_tier);
			DBG_PRT(18, os << "**confict** " << neu);
			reset_psignals();
			BRAIN_CK(! has_psignals());
			BRAIN_CK(found_conflict());
		} 
		pt_qua = NULL_PT;
	} else {
		BRAIN_CK(! qua.has_note3());
		BRAIN_CK(! only_in_dom || (sg_tier == tier()) || (sg_tier == (tier() + 1)));

		BRAIN_CK(qua.ck_uncharged_partner_neu());
		BRAIN_CK(qua.opposite().ck_uncharged_partner_neu());

		qua.set_charge(brn, neu, cg_positive, sg_tier);

		DBG_PRT_COND(64, (neu != NULL_PT), os << "qua=" << &qua << " SRC=" << neu);

		BRAIN_CK((qua.qu_source == neu) || 
			((level() == ROOT_LEVEL) && (qua.qu_source == NULL_PT)));
		BRAIN_CK((qua.qu_tier == sg_tier) || 
			((level() == ROOT_LEVEL) && (qua.qu_tier == 0)));
	}
	return pt_qua;
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
		br_delayed_psignals.clear();

		num_psigs2 = brn_tunnel_signals(false);

		BRAIN_CK(br_delayed_psignals.is_empty());
	}

	br_delayed_psignals.clear();

	BRAIN_CK(! has_psignals());
	BRAIN_CK(num_psignals() == 0);

	long tot_psigs = num_psigs1 + num_psigs2;
	return tot_psigs;
}

void
brain::start_propagation(quanton& nxt_qua){
	quanton* pt_qua = &nxt_qua;
	if(pt_qua->is_semi_mono()){
		pt_qua = pt_qua->get_semi_mono();
	}
	quanton& qua = *pt_qua;

	inc_level(qua);

	DBG(long old_lv = level());
	BRAIN_CK(data_level().ld_chosen == &qua);

	BRAIN_CK(! has_psignals());
	send_psignal(qua, NULL, tier() + 1);
	BRAIN_CK(has_psignals());

	if(qua.is_semi_mono()){
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

			if((num1 == num2) && qua.is_note5()){
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
		if(level() == ROOT_LEVEL){ 
			set_result(bjr_no_satisf);
			return;
		}
		//dbg_old_reverse();
		reverse();
		BRAIN_CK(has_psignals());

	} else {
		BRAIN_CK(! found_conflict());

		brain& brn = *this;

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

		if(! cho->is_note5()){ 
			cho->set_note5(brn); 
			DBG(
				cho->qu_dbg_fst_lap_cho = brn.br_current_ticket.tk_recoil;
				br_dbg.dbg_all_chosen.push(cho);
			)
		}
		if(cho->opposite().is_note5()){ 
			DBG(cho->qu_dbg_num_laps_cho++);
		}

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
			if(has_charge() && (par.qlevel() >= qlevel()) && 
				((min_lv_idx == INVALID_IDX) || (par.qlevel() < min_par_lv))
			){
				min_lv_idx = ii;
				min_par_lv = par.qlevel();
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
quanton::set_uncharged_partner_neu(brain& brn, long uidx, long dbg_call, neuron* dbg_neu){
	if(uidx == INVALID_IDX){
		qu_bak_uncharged_partner_neu = qu_uncharged_partner_neu;
		brn.data_level().ld_semi_monos_to_update.push(this);

		DBG_PRT_COND(105, (qu_id == 5), os << "qua=" << this 
			<< " setting uncharged to NULL" << bj_eol
			<< " LV=" << brn.level());

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

	DBG_PRT_COND(105, (qu_id == 5), os << "qua=" << this 
			<< "RESETTING uncharged to " << qu_uncharged_partner_neu);

	BRAIN_CK(ck_uncharged_partner_neu());
}


void
leveldat::reset_semi_monos(brain& brn){
	while(! ld_semi_monos_to_update.is_empty()){
		quanton& qua = *(ld_semi_monos_to_update.pop());
		if(! qua.qu_in_semi_monos_to_update){
			brn.br_semi_monos_to_update.push(&qua);
			qua.qu_in_semi_monos_to_update = true;
		}
	}
}

void
brain::update_semi_monos(){
	row_quanton_t& to_upd = br_semi_monos_to_update;
	
	brain& brn = *this;
	data_level().reset_semi_monos(brn);
	while(! to_upd.is_empty()){
		quanton& qua = *(to_upd.pop());
		BRAIN_CK(qua.qu_in_semi_monos_to_update);
		qua.qu_in_semi_monos_to_update = false;
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
	BRAIN_CK(br_semi_monos_to_update.is_empty());

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

	DBG(long rr_lv = br_charge_trail.last_qlevel());
	BRAIN_CK((level() == ROOT_LEVEL) || ((rr_lv + 1) == tgt_lvl));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == tgt_lvl));

	// resend opp chosen

	update_semi_monos();

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
	DBG(
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
brain::solve_instance(){
	bj_output_t& o_info = get_out_info();
	try{
		aux_solve_instance();
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
		DBG(
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
	BRAIN_CK(br_semi_monos_to_update.is_empty());
	BRAIN_CK(found_conflict());
	BRAIN_CK(br_conflict_found.ps_source != NULL_PT);
	BRAIN_CK(br_conflict_found.ps_source->ne_original);

	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());
	
	// analyse
	
	deduction& dct = br_retract_dct;

	//br_deducer.deduction_analysis(br_conflict_found, dct);
	analyse(br_conflict_found, dct);

	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, dbg_prt_lvs_have_learned(os));
	
	// retract
	retract_to(dct.dt_target_level);
	
	// some checks

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);
	DBG(long rr_lv = trail_level());
	
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

	DBG(
		dbg_update_config_entries(*this);
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

