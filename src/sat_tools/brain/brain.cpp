

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

#include <execinfo.h>	// trace stack 
#include <cxxabi.h>	// trace stack 
#include <cstring>

#include "stack_trace.h"
#include "support.h"
#include "sortor.h"
#include "brain.h"

DEFINE_NI_FLAG_FUNCS(qu_flags, note0, br_qu_tot_note0, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note1, br_qu_tot_note1, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note2, br_qu_tot_note2, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note3, br_qu_tot_note3, true);
DEFINE_NI_FLAG_FUNCS(qu_flags, note4, br_qu_tot_note4, false);
DEFINE_NI_FLAG_FUNCS(qu_flags, note5, br_qu_tot_note5, false);

DEFINE_NI_FLAG_ALL_FUNCS(note0);
DEFINE_NI_FLAG_ALL_FUNCS(note1);
DEFINE_NI_FLAG_ALL_FUNCS(note2);
DEFINE_NI_FLAG_ALL_FUNCS(note3);
DEFINE_NI_FLAG_ALL_FUNCS(note4);
DEFINE_NI_FLAG_ALL_FUNCS(note5);


//============================================================
// static vars

char*	quanton::CL_NAME = as_pt_char("{quanton}");
char*	neuron::CL_NAME = as_pt_char("{neuron}");

//============================================================
// random generator

long gen_random_num(long min, long max);

// search dbg_ic to config it

//============================================================
// Some code for debug purposes

ch_string	dbg_name(ch_string pref, long seq, ch_string suf){
	std::ostringstream stm1;
	long ancho = 3;
	char ch_cero = '0';
	stm1 << DBG_DIR << pref;
	stm1.width(ancho);
	stm1.fill(ch_cero);
	stm1 << seq << suf;
	ch_string nm = stm1.str();
	return nm;
}

//============================================================
// aux funcs

ch_string	
trinary_to_str(charge_t obj){
	if(obj == cg_positive){
		return "pos";
	} else if(obj == cg_negative){
		return "neg";
	} else {
		BRAIN_CK_0(obj == cg_neutral);
		return "ntr";
	}
	return "???";
}

bool
has_neu(row<neuron*>& rr_neus, neuron* neu){
	for(long aa = 0; aa < rr_neus.size(); aa++){
		if(rr_neus[aa] == neu){
			return true;
		}
	}
	return false;
}

// ck funcs

bool	
quanton::ck_charge(brain& brn){
	if((qlevel() == ROOT_LEVEL) && (get_source() != NULL)){
		ch_string ab_mm = "case0." + brn.br_file_name;
		abort_func(-1, ab_mm.c_str());
	}
	BRAIN_CK_0(	(is_pos()) || 
			(! has_charge()) || 
			(is_neg()) );
	BRAIN_CK_0(	(qu_inverse->is_pos()) || 
			(! qu_inverse->has_charge()) || 
			(qu_inverse->is_neg()) );
	BRAIN_CK_0(negate_trinary(get_charge()) == qu_inverse->get_charge());
	return true;
}

bool
brain::ck_trail(){	
	bj_ostream& os = *(GLB().dbg_os);
	brain& brn = *this;

	row<quanton*>& the_trl = br_tmp_trail;
	br_charge_trail.get_all_ordered_motives(the_trl);

	long num_null_src = 0;
	BRAIN_CK(br_tot_qu_dots == 0);
	quanton* last_choice = NULL_PT;
	quanton* prev_qua = NULL_PT;
	MARK_USED(prev_qua);
	
	ch_string ab_mm;	
	long ch_idx = 0;
	long prev_tier = INVALID_TIER;
	for(long ii = 0; ii < the_trl.size(); ii++){
		quanton* qua = the_trl[ii];

		if(qua == NULL_PT){
			ab_mm = "NULL qua !!." + br_file_name;
			abort_func(-1, ab_mm.c_str());
		}
		
		if((prev_tier != INVALID_TIER) && (prev_tier > qua->qu_tier)){
			os << "qua= " << qua << bj_eol;
			print_trail(os);
			ab_mm = "case0." + br_file_name;
			abort_func(-1, ab_mm.c_str());
		}
		prev_tier = qua->qu_tier;

		if((qua->qlevel() == ROOT_LEVEL) && (qua->get_source() != NULL)){
			print_trail(os);
			ab_mm = "case1." + br_file_name;
			abort_func(-1, ab_mm.c_str());
		}

		if((qua->qu_source == NULL) && (qua->qlevel() != ROOT_LEVEL)){
			num_null_src++;
		}
		if(qua->get_charge() == cg_neutral){
			print_trail(os);
			ab_mm = "case2." + br_file_name;
			abort_func(-1, ab_mm.c_str());
		}
		if(qua->has_note0()){
			print_trail(os);
			ab_mm = "case3." + br_file_name;
			abort_func(-1, ab_mm.c_str());
		}
		qua->ck_charge(brn);

		bool cho = qua->is_choice();
		if(cho){
			last_choice = qua;
			if((ch_idx >= br_chosen.size()) || (br_chosen[ch_idx] != qua)){
				os << "qua= " << qua << bj_eol;
				if(ch_idx < br_chosen.size()){
					os << "chosen= " << br_chosen[ch_idx] << bj_eol;
				}
				print_trail(os);
				os << "chosen" << bj_eol;
				os << br_chosen << bj_eol;
			}
			BRAIN_CK_0(ch_idx < br_chosen.size());
			BRAIN_CK_0(br_chosen[ch_idx] == qua);
			ch_idx++;
		}

		if(	!cho && (qua->get_source() == NULL))
		{
			quanton* cls = qua;
			BRAIN_CK_0(cls->qlevel() == qua->qlevel());
			if((cls != last_choice) && (cls->qlevel() != 0)){
				print_trail(os);
				ab_mm = "case5." + br_file_name;
				abort_func(-1, ab_mm.c_str());
			}
		}
		prev_qua = qua;
		//prev_tk = qua->qu_charge_tk;
	}

	if((num_null_src != level()) && ((num_null_src + 1) != level())){
		os << "num_null_src=" << num_null_src << bj_eol;
		os << "lv=" << level() << bj_eol;
		print_trail(os);
		ab_mm = "case6." + br_file_name;
		abort_func(-1, ab_mm.c_str());
	}
	return true;
}


//============================================================
// print methods

bj_ostream&
memap::print_memap(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	if(from_pt){
		os << "MM(" << (void*)this <<")";
		os << " szs_dotted=";
		os << ma_szs_dotted;
		os << " bef_rtct_tk=" << ma_before_retract_tk;
		os << " aft_rtct_tks=" << ma_after_retract_tks;
		os.flush();
		return os;
	}
	os << "MEMAP(" << (void*)this <<")={ du=" << ma_dual << bj_eol;
	os << " dotted=" << bj_eol;
	os << ma_dotted << bj_eol;
	os << " filled=" << bj_eol;
	ma_filled.print_row_data(os, true, "\n");
	os << " szs_dotted=" << bj_eol;
	os << ma_szs_dotted << bj_eol;
	os << " szs_filled=" << bj_eol;
	os << ma_szs_filled << bj_eol;
	os << " fll_in_lv=" << bj_eol;
	ma_fll_in_lv.print_row_data(os, true, "\n");
	os << " save_guide_col=" << bj_eol;
	os << ma_save_guide_col;
	os << " find_guide_col=" << bj_eol;
	os << ma_find_guide_col;
	os << " anchor_col=" << bj_eol;
	os << ma_anchor_col;
	os << " ma_anchor_idx=" << ma_anchor_idx << bj_eol;
	os << "bef_rtct_tk=" << ma_before_retract_tk << bj_eol;
	os << "aft_rtct_tks=" << ma_after_retract_tks << bj_eol;
	os << "confl=" << ma_confl << bj_eol;
	os << "active=" << ma_active << bj_eol;
	os << "}";
	os.flush();
	return os;
}

void
brain::print_trail(bj_ostream& os, bool no_src_only){
	row<quanton*>& the_trl = br_tmp_trail;
	br_charge_trail.get_all_ordered_motives(the_trl);
	os << "TRAIL=[";
	for(long kk = 0; kk < the_trl.size(); kk++){
		quanton* qua = the_trl[kk];
		if(no_src_only && qua->has_source()){
			continue;
		}

		the_trl[kk]->print_quanton(os, true);
		os << " ";
	}
	os << "]";
	os << bj_eol;
	os.flush();
}

bj_ostream& 
brain::print_all_quantons(bj_ostream& os, long ln_sz, ch_string ln_fd){
	BRAIN_CK_0(br_choices.size() == br_positons.size());
	long num_null_src = 0;
	for(long ii = 0; ii < br_choices.size(); ii++){
		if((ii > 0) && ((ii % ln_sz) == 0)){
			os << ln_fd;
		}

		quanton* qua = br_choices[ii];
		ch_string pre = " ";
		ch_string suf = " ";
		charge_t chg = qua->get_charge();
		if(chg == cg_negative){
			pre = "[";
			suf = "]";
		} else if(chg == cg_neutral){
			pre = "";
			suf = "_?";
		}
		if(qua->qlevel() == ROOT_LEVEL){
			os << "r";
		}
		if(	(qua->get_source() == NULL) && 
			(qua->qlevel() != 0) &&
			(chg != cg_neutral)
		){
			num_null_src++;
			os << "*";
		} else {
			os << " ";
		}
		os << pre << qua << suf << " ";

	}






	BRAIN_CK_0(num_null_src == level());
	os.flush();
	return os;
}

//============================================================
// check SAT result

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

long	set_dots_of(brain& brn, row<quanton*>& quans){
	long num_qua = 0;
	for(long ii = 0; ii < quans.size(); ii++){
		quanton* qua = quans[ii];
		BRAIN_CK(qua != &(brn.br_conflict_quanton));
		BRAIN_CK(qua != &(brn.br_top_block));
		BRAIN_CK_0(! qua->has_dot());
		if(! qua->has_dot()){
			qua->set_dot(brn);
			num_qua++;
		}
	}
	return num_qua;
}

long	reset_dots_of(brain& brn, row<quanton*>& quans){
	long resetted = 0;
	for(long ii = 0; ii < quans.size(); ii++){
		quanton* qua = quans[ii];
		BRAIN_CK(qua != &(brn.br_conflict_quanton));
		BRAIN_CK(qua != &(brn.br_top_block));
		if(qua->has_dot()){
			qua->reset_dot(brn);
			resetted++;
		}
	}
	return resetted;
}

void	set_marks_of(brain& brn, row<prop_signal>& trace, long first_idx, long last_idx, bool with_related){
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx))

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		quanton* qua = q_sig.ps_quanton;
		quanton* opp = qua->qu_inverse;

		BRAIN_CK(qua != &(brn.br_conflict_quanton));
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

void	reset_marks_of(brain& brn, row<prop_signal>& trace, long first_idx, long last_idx, bool with_related){
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx))

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		quanton* qua = q_sig.ps_quanton;
		quanton* opp = qua->qu_inverse;

		BRAIN_CK(qua != &(brn.br_conflict_quanton));
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

bool
brain::brn_compute_binary(row<neuron*>& neus){
	long ii;
	for(ii = 0; ii < neus.size(); ii++){
		BRAIN_CK(neus[ii] != NULL_PT);
		neuron& neu = *(neus[ii]);

		if(! neu.ne_original){
			continue;
		}
		if(!(neu.neu_compute_binary())){
			DBG_PRT(49, os << "FAILED compute neu=" << &(neu));
			return false;
		}
	}
	return true;
}

//for IS_SAT_CK
bool
brain::brn_compute_dots(row<neuron*>& neus){
	long ii;
	for(ii = 0; ii < neus.size(); ii++){
		BRAIN_CK(neus[ii] != NULL_PT);
		neuron& neu = *(neus[ii]);

		if(! neu.ne_original){
			continue;
		}
		if(!(neu.neu_compute_dots())){
			return false;
		}
	}
	return true;
}

//for IS_SAT_CK
bool
brain::brn_compute_dots_of(row<neuron*>& neus, row<quanton*>& assig){
	brain& brn = *this;
	BRAIN_CK(br_tot_qu_dots == 0);
	set_dots_of(brn, assig);

	long ii = 0;
	while((ii < assig.size()) && (assig[ii]->qlevel() == ROOT_LEVEL)){
		if(assig[ii]->qu_dot != cg_positive){
			return false;
		}
		ii++;
	}

	bool resp = brn_compute_dots(neus);
	reset_dots_of(brn, assig);
	BRAIN_CK(br_tot_qu_dots == 0);
	return resp;
}

//============================================================
// quanton methods

bj_ostream&
quanton::print_quanton(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);

	brain* pt_brn = GLB().pt_brain;

	if((pt_brn != NULL_PT) && (this == &(pt_brn->br_conflict_quanton))){
		os << "CONFL_QUA";
		os.flush();
		return os;
	}

	if((pt_brn != NULL_PT) && (this == &(pt_brn->br_top_block))){
		os << "TOP_BLOCK_QUA";
		os.flush();
		return os;
	}

	bool is_nega = is_neg();
	bool is_posi = is_pos();
	bool with_dot = has_dot();
	bool with_mark = has_mark();

	bool dominated = false;
	if(pt_brn != NULL_PT){
		dominated = in_qu_dominated(*(GLB().pt_brain));
	}

	neuron* neu = qu_source;

	if(from_pt){
		os << "("; 
		os << qu_id; 
		os << "." << qu_dbg_fst_lap_cho; 
		os << "." << qu_dbg_num_laps_cho; 
		os << ")"; 
		os.flush();
		return os;
	}

	if(from_pt){
		//if(qu_block != NULL_PT){ os << "b"; }
		if(with_dot){ os << "d"; }
		if(with_mark){ os << "m"; }
		if((neu != NULL_PT) && ! neu->ne_original){ os << "+"; }
		if((neu != NULL_PT) && neu->ne_original){ os << "o"; }
		if(! has_source() && has_charge()){ os << "*"; }

		if(! has_charge()){ os << "("; }
		if(is_nega){ os << '\\';  }
		if(is_posi){ os << '/';  }
		if(qlevel() == 0){ os << "#"; }
		os << qu_id; 

		long the_tee_consec = qu_tee.so_tee_consec;
		if(the_tee_consec == 0){ the_tee_consec = -(qu_inverse->qu_tee.so_tee_consec); } 

		//if(! qu_tee.is_unsorted()){ os << ".i" << the_tee_consec; }
		if(! qu_tee.is_unsorted()){ os << ".q" << qu_tee.so_qua_id; }
		//if(qlevel() != INVALID_LEVEL){ os << ".l" << qlevel(); }
		//if(qu_tier != INVALID_TIER){ os << ".t" << qu_tier; }
		if(is_posi){ os << '\\';  }
		if(is_nega){ os << '/';  }
		if(! has_charge()){ os << ")"; }

		if(dominated){ os << "DOM"; }

		os.flush();
		return os;
	}

	os << "\n";
	os << "QID " << qu_id << " ";
	os << "i" << qu_index << " ";
	os << "INV " << qu_inverse->qu_id << "\n";
	os << "CHG " << trinary_to_str(qu_charge) << " ";
	os << "chtk{" << qu_charge_tk << "} ";
	os << "sp_" << trinary_to_str(qu_spin) << " ";
	os << "st_" << qu_choice_idx << " ";
	os << "d_" << trinary_to_str(qu_dot) << "\n";
	os << "src ";
	if(get_source() != NULL){
		os << get_source();
	} else {
		os << "NULL";
	}
	os << "\n";
	os << "ne_tnn_" << qu_tunnels << "\n";
	os << "\n";
	os.flush();
	return os;
}

//============================================================
// neuron methods

quanton*
neuron::update_fibres(row<quanton*>& synps, bool orig){
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
			bj_ostream& os = *(GLB().dbg_os);
			if(GLB().pt_brain != NULL){
				GLB().pt_brain->print_trail(os);
			}
			os << this << bj_eol;;
			os << ne_fibres[0] << bj_eol;;
		}
		BRAIN_CK_0(! ne_fibres[0]->is_neg());
		forced_qua = forced_quanton();







	}

	ne_edge_tk.init_ticket();
	ne_recoil_tk.init_ticket();

	return forced_qua;
}

bool






quanton::ck_all_tunnels(){
	for(long ii = 0; ii < qu_tunnels.size(); ii++){
		neuron* neu = qu_tunnels[ii];
		neu->ck_tunnels();
	}
	return true;
}

void
neuron::neu_swap_edge(brain* brn, long ii){
	charge_t qua_chg = ne_fibres[1]->get_charge();

	swap_fibres_1(ii);

	if(qua_chg == cg_negative){
		ne_edge = ii;
		ne_edge_tk.update_ticket(brn);

		BRAIN_CK(is_ticket_eq(ne_edge_tk, brn->br_current_ticket));
		//BRAIN_CK(ne_edge_tk.is_active(brn));

		BRAIN_CK(ne_fibres[ii]->is_neg());
	}
}

void
neuron::neu_tunnel_signals(brain& brn, quanton& r_qua){
	BRAIN_CK(fib_sz() >= 2);
	BRAIN_CK(! is_ne_virgin());
	BRAIN_CK(ne_original || (brn.br_dbg_last_recoil_lv == brn.level()));


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
					neu_swap_edge(&brn, ii);
				}
				if(! q_neg){
					charge_t chag0 = ne_fibres[0]->get_charge();
					bool pos0 = (chag0 == cg_positive);
					bool neg0 = (chag0 == cg_negative);
					if(neg0){ swap_fibres_0(ii); }
					if(pos0 && neg1){ neu_swap_edge(&brn, ii); }
				}
			}

			if((fib_chg == cg_neutral) || (fib_chg == chg_op2)){
				BRAIN_CK((chg_op2 != cg_positive) || (fib_chg != cg_negative));
				BRAIN_CK(max_lev <= qua_lev);

				if(q_neg){
					neu_swap_edge(&brn, ii);
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
			DBG(ne_dbg_filled_tk.update_ticket(&brn));
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


bj_ostream&
neuron::print_neuron(bj_ostream& os, bool from_pt){
	return print_neu_base(os, ! from_pt, true, true);
}

bj_ostream&	
neuron::print_neu_base(bj_ostream& os, bool detail, bool prt_src, bool sort_fib){

	if(! detail){
		os << ((void*)(this)) << " ";
		os << ne_index << " ";
		if(ne_original){
			os << "o";
		}
		os << ne_fibres;
		//os << " recoil_tk=" << ne_recoil_tk.tk_recoil;
		//os << bj_eol;
		//os << " ne_dbg_ccl=" << ne_dbg_ccl;
		//os << bj_eol;
		//os << " ne_tee so_ccl=" << ne_tee.so_ccl;
		//os << " num_remote_tees=" << ne_num_remote_tees;

		/*
		if(! ne_fibres.is_empty() && (ne_fibres.first()->qu_source == this)){
			os << "src_of:" << ne_fibres.first();
		}
		if((ne_fibres.size() > 1) && (ne_fibres[1]->qu_source == this)){
			os << "src_of:" << ne_fibres[1] << " !!!!!!";
		}*/
		//os << bj_eol;

		os.flush();
		return os;
	}

	os << "\n";
	os << "pt=" << ((void*)(this)) << bj_eol;
	os << "INDEX " << ne_index << " ";
	os << "orig=" << ((ne_original)?("yes"):("no")) << "\n";
	os << "fz=" << fib_sz() << " ";
	os << "fb[ ";
	for(long ii = 0; ii < fib_sz(); ii++){
		os << ne_fibres[ii] << " ";
	}
	os << "] ";
	ck_tunnels();
	os << "f0i=" << ne_fibre_0_idx << " ";
	os << "f1i=" << ne_fibre_1_idx << " ";
	os << "\n";

	os << "eg=" << ne_edge << " ";
	os << "egtk{" << ne_edge_tk << "} ";
	os << "src_of:";

	if(! ne_fibres.is_empty() && (ne_fibres.first()->get_source() == this)){
		os << ne_fibres.first();
	}
	os << "\n";

	os << "syns" << ne_fibres << "\n";
	os << "\n";

	os.flush();
	return os;
}

//============================================================
// brain methods

void
brain::init_brain(){
	br_pt_inst = NULL_PT;
	br_prt_timer.init_timer(PRINT_PERIOD, SOLVING_TIMEOUT);

	br_start_load_tm = 0.0;

	// debug attributes

	br_dbg_last_recoil_lv = INVALID_LEVEL;

	// temp attributes

	// config attributes
	br_file_name = "<file_name>";
	br_file_name_in_ic = "<file_name_in_ic>";

	// state attributes
	br_choice_spin = cg_neutral;
	br_choice_order = k_invalid_order;
	//br_choices_lim = INVALID_IDX;

	br_first_psignal = 0;
	br_last_psignal = 0;

	br_deducer.de_brain = this;

	br_deducer.de_noteke.init_notekeeper(this);
	br_deducer.de_noteke.init_funcs(&br_tot_qu_dots, &quanton::has_dot, &quanton::set_dot, &quanton::reset_dot, 
		&set_dots_of, &reset_dots_of);

	br_noteke.init_notekeeper(this);
	br_noteke.init_funcs(&br_tot_qu_dots, &quanton::has_dot, &quanton::set_dot, &quanton::reset_dot, 
		&set_dots_of, &reset_dots_of);


	br_retract_nke0.init_notekeeper(this);
	br_retract_nke0.init_funcs(&br_qu_tot_note0, &quanton::has_note0, &quanton::set_note0, &quanton::reset_its_note0, 
		&set_all_note0, &reset_all_its_note0);

	br_retract_is_first_lv = false;

	br_dbg_find_id = 0;
	br_dbg_save_id = 0;

	br_conflict_found = NULL_PT;

	br_num_memo = 0;

	BRAIN_CK(br_conflict_quanton.qu_tee.is_alone());
	BRAIN_CK(br_top_block.qu_tee.is_alone());

	//br_tot_qu_spot0 = 0;
	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	init_tots_notes();

	br_dbg_all_chosen.clear();
}

void
brain::release_brain(){

	if(level() != ROOT_LEVEL){
		retract_all();
	}

	// reset neurons
	br_neurons.clear(true);

	// temporal attributes
	br_tmp_fixing_quantons.clear(false, true);

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

	br_conflict_quanton.init_quanton(cg_neutral, 0, NULL);
	br_top_block.init_quanton(cg_neutral, 0, NULL);

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
	BRAIN_CK(this != &(brn.br_conflict_quanton));
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

		qu_charge_tk.update_ticket(&brn);
		qu_inverse->qu_charge_tk.update_ticket(&brn);

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
	BRAIN_CK(! found_conflict());
	BRAIN_CK(ck_trail());
	DBG(
		ticket tk1;
		tk1.update_ticket(this);
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
		tk2.update_ticket(this);
		BRAIN_CK_0(is_ticket_eq(tk1, tk2));
	);

	return num_psigs;
}

quanton*	
brain::choose_quanton(){ 
	BRAIN_CK(ck_trail());

	quanton* qua = NULL;
	brain& brn = *this;

	while(! br_maps_active.is_empty()){
		memap* pt_mpp = br_maps_active.last();
		BRAIN_CK(pt_mpp != NULL_PT);
		memap& mpp_dom = *pt_mpp;
		qua = mpp_dom.map_choose_quanton(brn);
		if(qua != NULL_PT){
			return qua;
		}
		BRAIN_CK(qua == NULL_PT);
		deactivate_last_map();
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
	skeleton_glb& skg = get_skeleton();
	skg.start_local();

	br_neurons.set_cap(2 * num_neu);
	br_neurons.clear(true); 

	br_unit_neurons.clear();

	br_choices.clear();

	br_choices.set_cap(num_qua);
	br_choices.fill(NULL, num_qua);

	// reset quantons
	br_conflict_quanton.qu_inverse = &(br_conflict_quanton);
	BRAIN_CK(br_conflict_quanton.get_source() == NULL_PT);

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

		qua_pos->init_quanton(cg_positive, ii, qua_neg);
		qua_neg->init_quanton(cg_negative, ii, qua_pos);

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

	br_data_levels.clear(true, true);
	br_data_levels.inc_sz();

	br_conflict_quanton.qu_charge_tk.update_ticket(this);
	br_top_block.qu_charge_tk.update_ticket(this);

	br_tot_qu_dots = 0;
	br_tot_qu_marks = 0;
	br_tot_ne_spots = 0;

	init_tots_notes();

	br_dbg_all_chosen.clear();
}

void
brain::learn_mots(row<quanton*>& the_mots, quanton& forced_qua, long the_tier){
	DBG_PRT(23, os << "**LEARNING** mots=" << the_mots << " forced=" << &forced_qua << " tier=" << the_tier);

	neuron* the_neu = NULL_PT;

	//BRAIN_CK(the_mots.is_empty() || (tier() == the_tier));

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
		//long nxt_tir = the_tier + 1;
		long nxt_tir = 0;
		BRAIN_CK(level() == ROOT_LEVEL);
		send_psignal(forced_qua, the_neu, nxt_tir);
	}

	if(GLB().dbg_ic_active && GLB().dbg_ic_after){
		row<quanton*>& the_trl = br_tmp_trail;
		br_charge_trail.get_all_ordered_motives(the_trl);
		dbg_ic_print(the_trl);
	}
}

neuron*
brain::add_neuron(row<quanton*>& quans, quanton*& forced_qua, bool orig){
	brain& brn = *this;
	neuron& neu = locate_neuron();
	neu.ne_original = orig;

	forced_qua = neu.update_fibres(quans, orig);

	neu.update_uncharged(brn, NULL_PT);
	BRAIN_CK(neu.fib_sz() > 0);
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[0]->ck_uncharged_tunnel());
	BRAIN_CK(! neu.ne_original || neu.ne_fibres[1]->ck_uncharged_tunnel());

	DBG_PRT(26, os << "adding " << neu);
	return &neu;
}

bj_ostream&
brain::print_brain(bj_ostream& os){

	os << bj_eol;
	os << "cho_spin_" << (long)br_choice_spin << bj_eol;
	os << "cho_ord_" << (long)br_choice_order << bj_eol;

	os << bj_eol << "NEURONS:" << bj_eol;
	print_all_original(os);
	os << bj_eol << "POSITONS:" << bj_eol; 
	os << br_positons << bj_eol;
	os << bj_eol << "NEGATONS:" << bj_eol; 
	os << br_negatons << bj_eol;
	print_trail(os);
	os << "signals:" << bj_eol;
	print_psignals(os);
	os << bj_eol;
	os << bj_eol;
	//os << "choices_lim:" << br_choices_lim << bj_eol;
	//os << "\n choices:"; br_choices.print_row_data(os, true, " ", br_choices_lim, br_choices_lim); os << bj_eol;
	os << "\n choices:"; br_choices.print_row_data(os, true, " "); os << bj_eol;
	//os << "\n satisfying:" << satisfying << bj_eol;
	os << bj_eol;

	os.flush();
	return os;
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

	if(GLB().dbg_ic_active){
		std::ostringstream o_str;
		o_str << "rm " << DBG_DIR << "*.dot";
		system_exec(o_str);
	}

	if(GLB().dbg_ic_gen_jpg){
		std::ostringstream o_str;
		o_str << "rm " << DBG_DIR << "all_dot_to_jpg.bat";
		system_exec(o_str);
	}
}

void system_exec(std::ostringstream& strstm){
	ch_string comm = strstm.str();
	const char* comm_cstr = comm.c_str();
	//std::cout << comm_cstr << bj_eol;
	system(comm_cstr);
}

void
brain::load_it(){
	instance_info& inst_info = get_my_inst();

	br_start_load_tm = run_time();

	ch_string f_nam = inst_info.get_f_nam();

	// dimacs loading

	dimacs_loader	the_loader;
	row<long> inst_ccls;
	the_loader.parse_file(f_nam, inst_ccls);

	// brain loading

	long num_neu = the_loader.ld_num_ccls;
	long num_var = the_loader.ld_num_vars;

	load_instance(num_neu, num_var, inst_ccls);
}

void
brain::set_result(satisf_val re){
	instance_info& inst_info = get_my_inst();
	satisf_val& the_result = inst_info.ist_result;

	BRAIN_CK(re != k_unknown_satisf);
	BRAIN_CK((the_result != k_yes_satisf) || (re != k_no_satisf));
	BRAIN_CK((the_result != k_no_satisf) || (re != k_yes_satisf));

	the_result = re;

	DBG_PRT(27, os << "RESULT " << satisf_val_nams[the_result]);
	DBG_PRT(28, os << "HIT ENTER TO CONTINUE...");
	DBG_COMMAND(28, getchar());
}

void
brain::get_quantons_from_lits(row_long_t& all_lits, long first, long last, row<quanton*>& neu_quas){
	neu_quas.clear();

	for(long ii = first; ii < last; ii++){
		BRAIN_CK(all_lits.is_valid_idx(ii));
		long nio_id = all_lits[ii];

		quanton* qua = get_quanton(nio_id);
		BRAIN_CK_0(qua != NULL_PT);
		neu_quas.push(qua);
	}
}

void
brain::add_neuron_from_lits(row_long_t& all_lits, long first, long last){
	row<quanton*>& quas = br_tmp_fixing_quantons;

	quas.clear();
	get_quantons_from_lits(all_lits, first, last, quas);

	DBG_PRT(29, os << "ADDING NEU=" << quas);

	BRAIN_CK_0(quas.size() > 0);
	if(quas.size() > 1){
		bool is_orig = true;
		quanton* forced_qua = NULL;
		add_neuron(quas, forced_qua, is_orig);
		BRAIN_CK_0(forced_qua == NULL);
	} else {
		BRAIN_CK_0(quas.size() == 1);
		BRAIN_CK_0(level() == ROOT_LEVEL);

		neuron& neu = locate_neuron();
		neu.ne_original = false;
		neu.ne_fibres.push(quas.first());
		br_unit_neurons.push(&neu);

		send_psignal(*(quas.first()), NULL, 0);
	}
}

void
brain::init_uncharged(){
	brain& brn = *this;
	for(long ii = 0; ii < br_positons.size(); ii++){
		quanton& qua_pos = br_positons[ii];
		if(qua_pos.get_uncharged_tunnel(dbg_call_1) == NULL_PT){
			qua_pos.set_uncharged_tunnel(brn, qua_pos.find_uncharged_tunnel(), 100, NULL_PT);
		}
	}

	for(long ii = 0; ii < br_negatons.size(); ii++){
		quanton& qua_neg = br_negatons[ii];
		if(qua_neg.get_uncharged_tunnel(dbg_call_2) == NULL_PT){
			qua_neg.set_uncharged_tunnel(brn, qua_neg.find_uncharged_tunnel(), 110, NULL_PT);
		}
	}
}

void
brain::load_instance(long num_neu, long num_var, row_long_t& load_ccls){
	instance_info& inst_info = get_my_inst();
	inst_info.ist_num_vars = num_var;

	init_loading(num_var, num_neu);

	BRAIN_CK(load_ccls.last() == 0);
	BRAIN_CK(num_var > 0);

	br_neurons.set_cap(num_neu);

	long num_neu_added = 0;
	long num_den_added = 0;

	long max_neu_sz = 0;

	long ii = 0;
	long first = 0;
	for(ii = 0; ii < load_ccls.size(); ii++){
		long nio_id = load_ccls[ii];
		if(nio_id == 0){
			num_neu_added++;
			long num_dens = ii - first;

			BRAIN_CK(num_dens > 0);
			num_den_added += num_dens;

			if(num_dens > max_neu_sz){
				max_neu_sz = num_dens;
			}

			add_neuron_from_lits(load_ccls, first, ii);
			if(inst_info.ist_result != k_unknown_satisf){ 
				break; 
			}

			first = ii + 1;
		}
	}

	//BRAIN_CK(net.nt_neurons.size() == num_neu_added);

	init_uncharged();

	inst_info.ist_num_ccls = num_neu_added;
	inst_info.ist_num_lits = num_den_added;

	double end_load_tm = run_time();
	double ld_tm = (end_load_tm - br_start_load_tm);
	GLB().batch_stat_load_tm.add_val(ld_tm);

	ch_string f_nam = inst_info.get_f_nam();
	PRT_OUT(1,
	os << bj_eol;
	os << "***********************************************";
	os << bj_eol;
	os << "LOADED " << f_nam <<
		" " << GLB().batch_consec << " of " << GLB().batch_num_files <<
		bj_eol;
	);
}

void	due_periodic_prt(void* pm, double curr_secs){
	MARK_USED(pm);
	if(GLB().out_os != NULL_PT){
		PRT_OUT(0, GLB().print_stats(*(GLB().out_os), curr_secs));
	}
}

void
brain::check_timeout(){
	if(br_prt_timer.check_period(due_periodic_prt)){
		set_result(k_timeout);
	}
}

bj_ostream&
brain::print_all_original(bj_ostream& os){
	row<neuron*>& neus = br_tmp_prt_neus;
	fill_with_origs(neus);
	neus.print_row_data(os, false, "\n", -1, -1, true);
	return os;
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
brain::check_sat_assig(){
	row<neuron*>& neus = br_tmp_ck_sat_neus;
	fill_with_origs(neus);

	if(! brn_compute_binary(neus)){
		std::cerr << "FATAL ERROR 001. Wrong is_sat answer !" << bj_eol;
		abort_func(1);
	}

	//row<long>& the_chosen = GLB().final_chosen_ids;
	row<quanton*>& the_assig = GLB().final_assig;
	the_assig.clear();

	br_charge_trail.get_all_ordered_motives(the_assig);

	if(! brn_compute_dots_of(neus, the_assig)){
		std::cerr << "FATAL ERROR 002. Wrong is_sat answer !" << bj_eol;
		abort_func(1);
	}

	DBG_PRT(4, os << "CHECKED_ASSIG=" << the_assig << bj_eol);

	//print_satifying(cho_nm);

}

void 
brain::dbg_prt_all_cho(){
	bool is_batch = false;
	ch_string f_nam = GLB().get_file_name(is_batch);
	ch_string cho_nam = f_nam + "_chosen.log";

	const char* log_nm = cho_nam.c_str();
	std::ofstream log_stm;
	//log_stm.open(log_nm, std::ios::app);
	log_stm.open(log_nm, std::ios::binary);
	if(! log_stm.good() || ! log_stm.is_open()){
		std::cerr << "Could not open file " << log_nm << ".";
		return;
	}

	//dbg_get_all_chosen(br_dbg_all_chosen);

	log_stm << br_dbg_all_chosen;
	log_stm << bj_eol; 
	log_stm.close();
}

void 
brain::solve_it(){
	instance_info& inst_info = get_my_inst();

	get_skeleton().kg_instance_file_nam = inst_info.get_f_nam() + "\n";
	
	/*
	// ONLY_SAVE (NEVER FIND)
		br_forced_srg.sg_quick_find = true;
		br_forced_srg.sg_dbg_always_find_filled = false;

		br_filled_srg.sg_quick_find = true;
		br_filled_srg.sg_dbg_always_find_filled = false;

		GLB().gg_skeleton.kg_only_save = true;
		GLB().gg_skeleton.kg_save_canon = true;
		GLB().gg_skeleton.kg_verifying = false;
		GLB().gg_skeleton.kg_local_verifying = false;
		
	// ALWAYS FIND (NEVER SAVE)
		br_forced_srg.sg_quick_find = false;
		br_forced_srg.sg_dbg_always_find_filled = true;

		br_filled_srg.sg_quick_find = false;
		br_filled_srg.sg_dbg_always_find_filled = true;

		GLB().gg_skeleton.kg_only_save = false;
		GLB().gg_skeleton.kg_save_canon = false;
		GLB().gg_skeleton.kg_verifying = false;
		GLB().gg_skeleton.kg_local_verifying = false;
	*/

	if(inst_info.ist_result != k_unknown_satisf){ 
		return;
	}

	if(GLB().op_just_read){
		set_result(k_timeout);
		return;
	} 


	//BRAIN_CK(! GLB().is_finishing());

	inst_info.ist_solve_time = run_time();

	ch_string f_nam = inst_info.get_f_nam();

	config_brain(f_nam.c_str());
	br_choice_spin = cg_negative;
	br_choice_order = k_right_order;

	while(inst_info.ist_result == k_unknown_satisf){
		pulsate();
	}

	GLB().batch_stat_saved_targets.add_val(br_num_memo);

	BRAIN_CK(recoil() == (inst_info.ist_num_laps + 1));

	DBG(
		DBG_PRT(116, dbg_prt_all_cho());
		DBG_PRT(32, os << "BRAIN=" << bj_eol;
			print_brain(os); 
		);

		br_final_msg << f_nam << " ";

		satisf_val resp_solv = inst_info.ist_result;
		if(resp_solv == k_yes_satisf){
			check_sat_assig();
			br_final_msg << "IS_SAT_INSTANCE";
		} else if(resp_solv == k_no_satisf){
			br_final_msg << "IS_UNS_INSTANCE";
		} else if(resp_solv == k_timeout){
			br_final_msg << "HAS_TIMEOUT";
		}

		br_final_msg << " " << recoil();

		std::cout << br_final_msg.str() << bj_eol; 
		std::cout.flush();

		//br_final_msg << " " << stats.num_start_syn;
		//br_final_msg << " " << solve_time();
		//std::cout << "recoils= " << recoil() << bj_eol; 
		//std::cout << "num_batch_file= " << stats.batch_consec << bj_eol; 
		//std::cout << bj_eol;
		//std::cout << ".";

		if(GLB().dbg_ic_active){
			row<quanton*>& the_trl = br_tmp_trail;
			br_charge_trail.get_all_ordered_motives(the_trl);
			dbg_ic_print(the_trl);
		}
	);

	close_all_maps();

	if(level() != ROOT_LEVEL){
		retract_all();
	}

	br_psignals.clear(true, true);
	br_delayed_psignals.clear(true, true);

	br_forced_srg.release_all();
	br_filled_srg.release_all();

	all_mutual_init();
}

quanton*
notekeeper::set_motive_notes(row<quanton*>& rr_qua, long from, long until){
	BRAIN_CK(ck_funcs());
	BRAIN_CK(dk_tot_noted >= 0);
	BRAIN_CK(dk_num_noted_in_layer >= 0);

	brain& brn = get_dk_brain();

	DBG_PRT(104, os << "making notes " << rr_qua);

	long max_tier = -1;
	long num_max = 0;
	quanton* max_qua = NULL_PT;

	if(from < 0){ from = 0; }
	if(until > rr_qua.size()){ until = rr_qua.size(); }

	long ii = from;
	for(; ii < until; ii++){
		BRAIN_CK(rr_qua[ii] != NULL_PT);
		quanton& qua = *(rr_qua[ii]);
		BRAIN_CK(qua.get_charge() == cg_negative);


		long qti = qua.qu_tier;
		if(qti == max_tier){ num_max++; }
		if(qti > max_tier){ max_tier = qti; num_max = 1; max_qua = &qua; }

		bool has_note = (qua.*dk_has_note_fn)();
		if(! has_note && (qua.qlevel() != ROOT_LEVEL)){
			dk_tot_noted++;
			(qua.*dk_set_note_fn)(brn);

			BRAIN_CK(dk_note_layer != INVALID_LEVEL)
			DBG(ch_string dbg_msg = "");
			long q_lv = qua.qlevel();

			add_motive(qua, q_lv);

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
					<< "lv=" << dk_note_layer << "ti=" << qua.qu_tier << bj_eol
					<< "motives_by_lv= " << dk_motives_by_layer);
			
		}
	}

	BRAIN_CK((from == 0) || (rr_qua.size() < 2) || ((max_tier + 1) == rr_qua.first()->qu_tier));
	BRAIN_CK(num_max > 0);

	if(num_max == 1){ return max_qua; }
	return NULL_PT;
}

void
deducer::init_deducer(brain* brn, neuron* confl, long tg_lv)
{
	de_brain = brn;
	//de_trail = trl;
	
	long* pt_tot_dots = NULL_PT;
	if(brn != NULL_PT){
		pt_tot_dots = &(brn->br_tot_qu_dots);
	}

	de_noteke.init_notekeeper(brn, tg_lv);
	de_noteke.init_funcs(pt_tot_dots, &quanton::has_dot, &quanton::set_dot, &quanton::reset_dot, 
		&set_dots_of, &reset_dots_of);

	de_all_original = true;
	de_all_dom = true;

	de_analysed_level = INVALID_LEVEL;

	de_target_dct.init_deduction();
	BRAIN_CK(de_target_dct.is_dt_virgin());

	de_searched_dotted_sz = 0;
	de_searched_filled_sz = 0;

	de_tmp_selected.clear();
	de_filled_in_lv.clear();

	de_trl_idx = INVALID_IDX;

	tg_confl() = confl;
	de_nxt_src = confl;
}

void
deducer::deduc_find_next_dotted(){
	while(! get_curr_quanton().has_dot()){
		DBG_PRT(20, os << "NOT dotted " << get_curr_quanton() << " in deduc find next dotted");
		de_trl_idx--;
	}

	BRAIN_CK(de_noteke.dk_note_layer <= get_de_brain().level());
	quanton& nxt_qua = get_curr_quanton();
	long qlv = nxt_qua.qlevel();

	de_noteke.update_notes_layer(qlv);
}

void
deducer::deduc_find_next_source(){

	brain& brn = get_de_brain();

	BRAIN_CK(tg_confl() != NULL_PT);
	bool is_first = (de_nxt_src == tg_confl());

	BRAIN_CK(! is_first || (de_nxt_src != NULL));
	DBG_PRT(20, os << "reasoning_cause_of_conflict " << de_nxt_src;
		os << " tot_dotted=" << de_noteke.dk_tot_noted;
		os << " num_dotted_in_lv=" << de_noteke.dk_num_noted_in_layer;
		os << " dotting level=" << de_noteke.dk_note_layer;
		os << " ti=" << get_curr_quanton().qu_tier;
	);

	if(de_nxt_src != NULL){
		BRAIN_CK(! de_nxt_src->ne_fibres.is_empty());
		BRAIN_CK(is_first || (de_nxt_src->ne_fibres[0]->get_charge() == cg_positive) );
		BRAIN_CK(is_first || de_nxt_src->neu_compute_binary());

		if(! de_nxt_src->ne_original){
			de_all_original = false;
		}

		row<quanton*>& causes = de_nxt_src->ne_fibres;
		long from = (is_first)?(0):(1);
		long until = causes.size();
		//quanton* max_qua = 
		de_noteke.set_motive_notes(de_nxt_src->ne_fibres, from, until);
	}

	BRAIN_CK(de_noteke.dk_tot_noted > 0);

	deduc_find_next_dotted();

	quanton& nxt_qua = get_curr_quanton();
	DBG_PRT(20, os << "dotted found " << nxt_qua << " num_dotted_in_lv " << de_noteke.dk_num_noted_in_layer;
		os << " dotting level=" << de_noteke.dk_note_layer;
		os << " ti=" << get_curr_quanton().qu_tier;
	);

	de_nxt_src = nxt_qua.get_source();

	nxt_qua.reset_dot(brn);
	BRAIN_CK(nxt_qua.qlevel() == de_noteke.dk_note_layer);
	BRAIN_CK(nxt_qua.is_pos());

	de_noteke.dec_notes();

	DBG_PRT(101, os << "qua=" << &nxt_qua << " filled APPEND=";
		nxt_qua.qu_full_charged.print_row_data(os, true, "\n");
	);
}

bool
ck_motives(brain& brn, row<quanton*>& mots){
	for(long ii = 0; ii < mots.size(); ii++){
		quanton* mot = mots[ii];
		MARK_USED(mot);
		BRAIN_CK(mot != NULL_PT);
		BRAIN_CK(mot->qlevel() <= brn.level());
		BRAIN_CK(mot->qlevel() < brn.br_dbg_before_retract_lv);

		DBG_PRT_COND(DBG_ALL_LVS, ! (mot->get_charge() == cg_negative) ,
			os << "ABORTING_DATA " << bj_eol;
			os << "mots=" << mots << bj_eol;
			os << "mot=" << mot << bj_eol;
			brn.print_trail(os);
			os << "END_OF_aborting_data" << bj_eol;
		);
		BRAIN_CK(mot->get_charge() == cg_negative);
	}
	return true;
}

void
deducer::dbg_find_dct_of(neuron& confl, deduction& dct){
	brain& brn = get_de_brain();

	brn.br_charge_trail.get_all_ordered_motives(de_charge_trail);

	de_all_original = true;
	de_all_dom = true;

	de_noteke.init_notes(brn.level());	

	BRAIN_CK(brn.br_tot_qu_dots == 0);
	BRAIN_CK(de_noteke.dk_note_layer != INVALID_LEVEL);
	BRAIN_CK(de_noteke.dk_note_layer != ROOT_LEVEL);
	BRAIN_CK(de_noteke.dk_note_layer > 0);
	BRAIN_CK(de_noteke.dk_note_layer <= get_de_brain().level());
	BRAIN_CK(! confl.ne_fibres.is_empty());

	tg_confl() = &confl;



	long trail_sz = get_trail().size();

	DBG(row<quanton*> tmp_mots);
	DBG(de_noteke.get_all_motives(tmp_mots));
	BRAIN_CK(tmp_mots.is_empty());

	de_filled_in_lv.clear();
	de_trl_idx = trail_sz - 1;
	de_nxt_src = &confl;


	BRAIN_CK(de_noteke.dk_note_layer == brn.level());
	long deduc_lv = de_noteke.dk_note_layer;
	MARK_USED(deduc_lv);

	deduc_find_next_source();
	while(de_noteke.dk_num_noted_in_layer > 0){
		deduc_find_next_source();
	}

	quanton& nxt_qua = get_curr_quanton();
	quanton* opp_nxt = nxt_qua.qu_inverse; 
	BRAIN_CK(opp_nxt != NULL);

	BRAIN_CK(opp_nxt->qlevel() == deduc_lv);
	BRAIN_CK(opp_nxt->get_charge() == cg_negative);

	dct.init_deduction();
	dct.dt_motives.set_cap(de_noteke.dk_tot_noted + 1);

	de_noteke.get_all_motives(dct.dt_motives);

	// close deduction

	find_max_level_and_tier(dct.dt_motives, dct.dt_target_level, dct.dt_target_tier);

	BRAIN_CK(! opp_nxt->has_dot());
	dct.dt_forced = opp_nxt;

	BRAIN_CK(dct.dt_target_level < de_noteke.dk_note_layer);
	BRAIN_CK(ck_motives(brn, dct.dt_motives));

	DBG_PRT(20, os << "dbg_find_dct_of deduction=" << dct);

	BRAIN_CK(trail_sz == get_trail().size());

	// reset all

	de_noteke.clear_all_motives();

	DBG(de_noteke.get_all_motives(tmp_mots));
	BRAIN_CK(tmp_mots.is_empty());

	BRAIN_CK(brn.br_tot_qu_dots == 0);
}

// blocks

void
leveldat::release_learned(brain& brn){
	for(long aa = 0; aa < ld_learned.size(); aa++){
		BRAIN_CK(ld_learned[aa] != NULL_PT);
		neuron& neu = *(ld_learned[aa]);
		BRAIN_CK(! neu.ne_original);

		brn.release_neuron(neu);
	}
}

void
notekeeper::clear_all_motives(long lim_lv, bool reset_notes){
	BRAIN_CK(ck_funcs());

	brain& brn = get_dk_brain();
	if(lim_lv < 0){
		lim_lv = dk_motives_by_layer.size();
	}
	BRAIN_CK(lim_lv <= dk_motives_by_layer.size());
	for(long aa = 0; aa < lim_lv; aa++){
		BRAIN_CK(dk_motives_by_layer.is_valid_idx(aa));
		row_quanton_t& mots = dk_motives_by_layer[aa];

		if(reset_notes){
			long num_re = (*dk_reset_all_fn)(brn, mots);
			dk_tot_noted -= num_re;
		}
		mots.clear();
	}
	DBG(
		for(long bb = lim_lv; bb < dk_motives_by_layer.size(); bb++){
			BRAIN_CK(dk_motives_by_layer.is_valid_idx(bb));
			row_quanton_t& mots = dk_motives_by_layer[bb];
			BRAIN_CK(mots.is_empty());
		}
	);

	BRAIN_CK(dk_tot_noted == 0);
	BRAIN_CK(dk_num_noted_in_layer == 0);
}

void
neuron::mem_ne_fill_remote_sortees(mem_op_t mm, brain& brn){

	DBG(quanton& cf_qu = brn.br_conflict_quanton);

	ne_num_remote_tees = 0;

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);
		quanton& opp = *(qua.qu_inverse);

		bool base_qua = false;

		BRAIN_CK(&cf_qu != &qua);
		BRAIN_CK(&cf_qu != &opp);

		if(qua.has_pos_mark()){ 
			BRAIN_CK(qua.has_mark());
			BRAIN_CK(qua.qu_mark_idx != INVALID_IDX);
			if(! base_qua){
				sortrel& sre = qua.qu_reltee;
				row<sortee*>& all_pos = sre.so_positive;
				all_pos.push(&ne_tee);
				ne_num_remote_tees++;
			}
		}
	
		if(opp.has_pos_mark()){ 
			BRAIN_CK(qua.has_mark());
			BRAIN_CK(qua.qu_mark_idx != INVALID_IDX);
			if(! base_qua){
				sortrel& sre = opp.qu_reltee;
				row<sortee*>& all_neg = sre.so_negative;
				all_neg.push(&ne_tee);
				ne_num_remote_tees++;
			}
		}
	}

	BRAIN_CK(cf_qu.qu_inverse != NULL_PT);
	BRAIN_CK(cf_qu.qu_reltee.so_positive.is_empty());
	BRAIN_CK(cf_qu.qu_reltee.so_negative.is_empty());
	BRAIN_CK(cf_qu.qu_inverse->qu_reltee.so_positive.is_empty());
	BRAIN_CK(cf_qu.qu_inverse->qu_reltee.so_negative.is_empty());
}

void
deduction::set_with(brain& brn, notekeeper& nke, quanton& nxt_qua){
	if(! is_dt_virgin()){
		return;
	}

	quanton& opp_nxt = *(nxt_qua.qu_inverse);

	BRAIN_CK(opp_nxt.qlevel() == nke.dk_note_layer);
	BRAIN_CK(opp_nxt.get_charge() == cg_negative);

	nke.get_all_motives(dt_motives);

	DBG_PRT(51, os << " motives_by_lv= " << nke.dk_motives_by_layer);
	DBG_PRT(52, os << "LV=" <<  nke.dk_note_layer << " motives " << dt_motives << " opp_nxt=" << &opp_nxt);

	find_max_level_and_tier(dt_motives, dt_target_level, dt_target_tier);

	dt_forced = &opp_nxt;
	dt_forced_level = opp_nxt.qlevel();

	BRAIN_CK(dt_target_level < nke.dk_note_layer);
	BRAIN_CK(ck_motives(brn, dt_motives));
	BRAIN_CK(! is_dt_virgin());
}

quanton*
neuron::set_motives(brain& brn, notekeeper& nke, bool is_first){
	neuron& neu = *this;

	BRAIN_CK(! ne_fibres.is_empty());
	BRAIN_CK(is_first || (ne_fibres[0]->get_charge() == cg_positive) );
	BRAIN_CK(is_first || neu_compute_binary());

	ne_recoil_tk.update_ticket(&brn);

	if(ne_original){
		while(! in_ne_dominated(brn)){
			DBG_PRT(112, os << "NOT NE dom neu=" << this << bj_eol;
				brn.print_trail(os);

				os << " up_dom=" << brn.get_last_upper_map() 
				<< " ne_curr_map=" << ne_curr_map << bj_eol
				<< " br_maps_active=" << brn.br_maps_active
			);
			DBG_PRT(112, os << "HIT RETURN TO CONTINUE..." << " dbg_id=" << skg_dbg_canon_save_id);
			DBG_COMMAND(112, getchar());

			brn.deactivate_last_map();
		}
		BRAIN_CK(in_ne_dominated(brn));
	}

	DBG(brn.dbg_add_to_used(neu));

	row<quanton*>& causes = ne_fibres;
	BRAIN_CK(! causes.is_empty());

	long from = (is_first)?(0):(1);
	long until = causes.size();
	quanton* max_qua = nke.set_motive_notes(neu.ne_fibres, from, until);

	return max_qua;
}

void
brain::retract_all(){
	BRAIN_CK(br_semi_monos.is_empty());

	brain& brn = *this;
	deduction dct;
	while(br_charge_trail.has_motives()){
		bool end_of_lev = (br_charge_trail.last_qlevel() != level());
		if(end_of_lev){
			BRAIN_CK_0((br_charge_trail.last_qlevel() + 1) == level());
			BRAIN_CK_0(level() != ROOT_LEVEL);
		
			dec_level();
		}

		if(level() == ROOT_LEVEL){
			break;
		}

		quanton& qua = trail_last();
		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
	}
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
	bool c4 = (dct1.dt_target_tier == dct2.dt_target_tier);

	MARK_USED(c1);
	MARK_USED(c2);
	MARK_USED(c3);
	MARK_USED(c4);

	DBG_PRT_COND(DBG_ALL_LVS, ! (c1 && c2 && c3 && c4) ,
		os << "ABORTING_DATA " << bj_eol;
		os << "  c1=" << c1 << "  c2=" << c2 << "  c3=" << c3 << "  c4=" << c4 << bj_eol;
		os << "dct1=" << dct1 << bj_eol;
		os << "dct2=" << dct2 << bj_eol;
		print_trail(os);
		os << "END_OF_aborting_data" << bj_eol;
	);

	BRAIN_CK(c1);
	BRAIN_CK(c2);

	BRAIN_CK(c3);
	BRAIN_CK(c4);

	//DBG_PRT(DBG_ALL_LVS, os << "CK_DCTS_OK"); 
	return true;
}

void
neuron::map_set_dbg_ccl(mem_op_t mm, brain& brn){
	ne_dbg_ccl.cc_clear(false);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);

		if(qua.has_mark()){ 
			ne_dbg_ccl.cc_push(qua.qu_id);
		}
	}

	ne_dbg_ccl.cc_mix_sort(cmp_canon_ids);
}

void
neuron::add_dbg_ccl(brain& brn, row<canon_clause*>& the_ccls, row<neuron*>& the_neus, dima_dims& dims){
	canon_clause& ccl = ne_dbg_ccl;
	if(! ne_spot){
		set_spot(brn);
		the_ccls.push(&ccl);
		the_neus.push(this);

		if(ccl.cc_size() == 2){ dims.dd_tot_twolits++; }
		dims.dd_tot_lits += ccl.cc_size();
	}
}

void
memap::map_set_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls, row<neuron*>& the_neus,
		dima_dims& dims)
{
	the_ccls.clear();
	the_neus.clear();

	dims.init_dima_dims(0);

	long trace_sz = get_trace_sz(mm);
	long filled_sz = get_filled_sz(mm);

	row<prop_signal>& trace = ma_dotted;
	BRAIN_CK((trace_sz == trace.size()) || trace.is_valid_idx(trace_sz));

	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	set_marks_of(brn, trace, 0, trace_sz);

	BRAIN_CK(ma_confl != NULL_PT);
	ma_confl->map_set_dbg_ccl(mm, brn);
	ma_confl->add_dbg_ccl(brn, the_ccls, the_neus, dims);
	
	for(long aa = 0; aa < trace_sz; aa++){
		prop_signal& q_sig = trace[aa];
		BRAIN_CK(q_sig.ps_quanton != NULL_PT);

		if(q_sig.ps_source != NULL_PT){
			neuron& src = *(q_sig.ps_source);

			src.map_set_dbg_ccl(mm, brn);
			src.add_dbg_ccl(brn, the_ccls, the_neus, dims);
		}
	}

	row<neuron*>& filled = ma_filled;
	BRAIN_CK((filled_sz == filled.size()) || filled.is_valid_idx(filled_sz));

	for(long bb = 0; bb < filled_sz; bb++){
		BRAIN_CK(filled[bb] != NULL_PT);
		neuron& neu = *(filled[bb]);

		neu.map_set_dbg_ccl(mm, brn);
		neu.add_dbg_ccl(brn, the_ccls, the_neus, dims);
	}

	for(long cc = 0; cc < the_neus.size(); cc++){
		BRAIN_CK(the_neus[cc] != NULL_PT);
		neuron& neu = *(the_neus[cc]);
		BRAIN_CK(neu.ne_spot);
		neu.reset_spot(brn);
	}

	reset_marks_of(brn, trace, 0, trace_sz);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_tot_qu_marks == 0);

	the_ccls.mix_sort(cmp_clauses);
}

void
memap::map_activate(brain& brn){

	BRAIN_CK(! is_ma_virgin());
	BRAIN_CK(! ma_active);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	brn.br_maps_active.push(this);
	ma_active = true;

	map_set_all_qu_curr_dom(brn);
	map_set_all_ne_curr_dom(brn);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	BRAIN_CK(! brn.br_maps_active.is_empty());
	BRAIN_CK(brn.br_maps_active.last() == this);

	DBG_PRT(110, os << "ACTIVATING " << this);
}

void
memap::map_deactivate(brain& brn){
	//DBG_PRT(110, os << "DEACTivating " << this << bj_eol << STACK_STR);
	DBG_PRT(110, os << "DEACTivating " << this);

	BRAIN_CK(ma_active);
	BRAIN_CK(! brn.br_maps_active.is_empty());

	BRAIN_CK(brn.br_maps_active.last() == this);
	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));

	brn.br_maps_active.pop();
	ma_active = false;

	map_reset_all_qu_curr_dom(brn);
	map_reset_all_ne_curr_dom(brn);

	BRAIN_CK(map_ck_all_qu_dominated(brn));
	BRAIN_CK(map_ck_all_ne_dominated(brn));
}

void
brain::deactivate_last_map(){
	BRAIN_CK(! br_maps_active.is_empty());
	brain& brn = *this;
	memap* pt_mpp = br_maps_active.last();
	BRAIN_CK(pt_mpp != NULL_PT);
	memap& mpp = *pt_mpp;

	DBG_PRT(112, os << "DEACTIVATING MEMAP=" << bj_eol;
		os << (void*)(&(mpp))
	);

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
brain::print_active_maps(bj_ostream& os){
	os << "[";
	for(long aa = 0; aa < br_maps_active.size(); aa++){
		memap* mpp = br_maps_active[aa];
		os << " " << (void*)(mpp);
	}
	os << " ]";
	os << bj_eol;

	os.flush();
}

void
memap::map_set_all_qu_curr_dom(brain& brn){
	row<prop_signal>& sgls = ma_dotted;
	for(long aa = 0; aa < sgls.size(); aa++){
		prop_signal& q_sig = sgls[aa];

		quanton* qua = q_sig.ps_quanton;
		BRAIN_CK(qua != NULL_PT);
		//BRAIN_CK(qua->is_pos());

		quanton* inv = qua->qu_inverse;

		BRAIN_CK(qua->qu_curr_map != this);
		BRAIN_CK(inv->qu_curr_map != this);

		qua->qu_curr_map = this;
		inv->qu_curr_map = this;
	}
}

void
memap::map_reset_all_qu_curr_dom(brain& brn){
	memap* upper_map = brn.get_last_upper_map();
	row<prop_signal>& sgls = ma_dotted;
	for(long aa = 0; aa < sgls.size(); aa++){
		prop_signal& q_sig = sgls[aa];
		quanton* qua = q_sig.ps_quanton;
		BRAIN_CK(qua != NULL_PT);

		quanton* inv = qua->qu_inverse;

		BRAIN_CK(qua->qu_curr_map != upper_map);
		BRAIN_CK(inv->qu_curr_map != upper_map);

		qua->qu_curr_map = upper_map;
		inv->qu_curr_map = upper_map;
	}
}

void
memap::map_set_all_ne_curr_dom(brain& brn){
	row<neuron*>& filled = ma_filled;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			BRAIN_CK(fll_neu.ne_curr_map != this);
			fll_neu.ne_curr_map = this;
		}
	}
}

void
memap::map_reset_all_ne_curr_dom(brain& brn){
	memap* upper_map = brn.get_last_upper_map();
	row<neuron*>& filled = ma_filled;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			BRAIN_CK(fll_neu.ne_curr_map != upper_map);
			fll_neu.ne_curr_map = upper_map;
		}
	}
}

bool
memap::map_ck_all_qu_dominated(brain& brn){
	row<prop_signal>& sgls = ma_dotted;
	for(long aa = 0; aa < sgls.size(); aa++){
		quanton* qua = sgls[aa].ps_quanton;
		MARK_USED(qua);
		BRAIN_CK(qua != NULL_PT);
		BRAIN_CK(qua->in_qu_dominated(brn));
	}
	return true;
}

bool
memap::map_ck_all_ne_dominated(brain& brn){
	row<neuron*>& filled = ma_filled;
	for(long ii = 0; ii < filled.size(); ii++){
		BRAIN_CK(filled[ii] != NULL_PT);
		neuron& fll_neu = *(filled[ii]);
		if(fll_neu.ne_original){
			MARK_USED(fll_neu);

			DBG_PRT_COND(DBG_ALL_LVS, ! (fll_neu.in_ne_dominated(brn)) ,
				os << "ABORTING_DATA " << bj_eol;
				os << " br_maps_active=" << brn.br_maps_active << bj_eol;
				os << " THIS_MEMAP=" << this << bj_eol;
				brn.print_trail(os);
				os << " up_dom=" << (void*)(brn.get_last_upper_map()) << bj_eol;
				os << " this_map=" << (void*)(this) << bj_eol;
				os << " NO_DOM neu==" << &fll_neu << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			BRAIN_CK(fll_neu.in_ne_dominated(brn));
		}
	}
	return true;
}

bool
quanton::in_qu_dominated(brain& brn){
	memap* up_dom = brn.get_last_upper_map();
	bool in_dom = (up_dom == qu_curr_map);

	return in_dom;
}

bool
neuron::in_ne_dominated(brain& brn){
	memap* up_dom = brn.get_last_upper_map();
	bool in_dom = (up_dom == ne_curr_map);
	return in_dom;
}

void
memap::set_filled(brain& brn){
	//memap& mpp = *this;

	row<neuron*>& all_neus = ma_fll_in_lv;
	row<neuron*>& sel_neus = brn.br_tmp_selected;
	row<neuron*>& not_sel_neus = brn.br_tmp_not_selected;
	row<neuron*>& disca_neus = brn.br_tmp_discarded;


	BRAIN_CK(brn.br_tot_qu_marks == 0);

	quanton& nxt_qua = map_last_dotted();
	MARK_USED(nxt_qua);

	BRAIN_CK(nxt_qua.is_pos());
	BRAIN_CK(! nxt_qua.has_source());

	sel_neus.clear();
	not_sel_neus.clear();
	disca_neus.clear();

	set_marks_of(brn, ma_dotted);

	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);

		BRAIN_CK(neu.ne_original);

		bool is_fll = neu.is_filled_of_marks(nxt_qua);
		bool in_dom = neu.in_ne_dominated(brn);
		//bool in_dom = true;

		if(is_fll){
			if(in_dom){
				sel_neus.push(&neu);
			} else {
				disca_neus.push(&neu);
			}
		} else {
			not_sel_neus.push(&neu);
		}
	}

	reset_marks_of(brn, ma_dotted);

	BRAIN_CK(brn.br_tot_qu_marks == 0);

	// finalize

	disca_neus.move_to(ma_discarded);

	not_sel_neus.move_to(ma_fll_in_lv);


	sel_neus.append_to(ma_filled);

	//BRAIN_CK(map_ck_dotted_src_in_filled(mo_find, brn));
}

bool
neuron::is_filled_of_marks(quanton& nxt_qua){
	BRAIN_CK(nxt_qua.is_pos());
	BRAIN_CK(! nxt_qua.has_source());

	bool is_fll = true;
	DBG(long dbg_num_after = 0);
	for(long ii = 0; ii < fib_sz(); ii++){

		BRAIN_CK(ne_fibres[ii] != NULL_PT);
		quanton& qua = *(ne_fibres[ii]);
		quanton& opp = *(qua.qu_inverse);

		bool has_chg = qua.has_charge();
		bool is_nxt = ((&qua == &nxt_qua) || (&opp == &nxt_qua));
		bool is_part = (! has_chg || is_nxt);

		DBG(if(qua.has_mark() && is_part){ dbg_num_after++; });

		if(! qua.has_mark() && is_part){
			is_fll = false;
			break;
		}

		if(qua.is_pos() && ! is_part){
			is_fll = false;
			break;
		}
	}

	BRAIN_CK(! is_fll || (dbg_num_after >= 2));
	return is_fll;
}

void
notekeeper::restart_with(brain& brn, row<quanton*>& bak_upper){
	BRAIN_CK(dk_num_noted_in_layer == 0);
	BRAIN_CK(get_layer_motives(dk_note_layer).is_empty());
	BRAIN_CK(dk_note_layer >= brn.level());
	BRAIN_CK(dk_has_note_fn == &quanton::has_note0);

	row<quanton*> rr_upper;
	get_all_motives(rr_upper);
	BRAIN_CK(ck_motives(brn, rr_upper));
	set_all_note3(brn, rr_upper);

	DBG(long old_num3 = brn.br_qu_tot_note3);
	clear_all_motives();
	BRAIN_CK(old_num3 == brn.br_qu_tot_note3);

	BRAIN_CK(brn.br_qu_tot_note0 == 0);
	BRAIN_CK(get_note_counter() == 0);

	long n_note_lv = brn.level() - 1;
	update_notes_layer(n_note_lv);
	BRAIN_CK(dk_note_layer == n_note_lv);

	if(! bak_upper.is_empty()){
		long from = 0;
		long until = bak_upper.size();
		set_motive_notes(bak_upper, from, until);
	}
}

quanton*	
memap::map_choose_quanton(brain& brn){ 
	row<prop_signal>& trace = ma_dotted;
	quanton* qua = NULL;

	long max_ii = trace.last_idx();
	for(long ii = max_ii; ii >= 0; ii--){
		quanton* qua_ii = trace[ii].ps_quanton;
		if(qua_ii->get_charge() == cg_neutral){
			if(! qua_ii->is_semi_mono()){
				qua = qua_ii; 
				break;
			}
			if(qua == NULL_PT){ qua = qua_ii; }
		}
	}

	BRAIN_CK_0((qua == NULL) || (qua->get_charge() == cg_neutral));
	BRAIN_CK_0((qua == NULL) || (qua->qu_spin == cg_positive) || (qua->qu_spin == cg_negative));
	return qua;
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

	sgnl.init_prop_signal();

	if(qua.has_charge()){
		if(qua.is_neg()){
			BRAIN_CK((neu != NULL_PT) || (level() == ROOT_LEVEL));
			if(neu != NULL_PT){
				br_conflict_found = neu;
			} else {
				BRAIN_CK(level() == ROOT_LEVEL);
				br_conflict_found = &br_root_conflict;
			}
			DBG_PRT(18, os << "**confict** " << neu);
			reset_psignals();
			BRAIN_CK(! has_psignals());
			BRAIN_CK(found_conflict());
		} 
		pt_qua = NULL_PT;
	} else {
		BRAIN_CK(! qua.has_note3());
		BRAIN_CK(! only_in_dom || (sg_tier == tier()) || (sg_tier == (tier() + 1)));

		BRAIN_CK(qua.ck_uncharged_tunnel());
		BRAIN_CK(qua.opposite().ck_uncharged_tunnel());

		qua.set_charge(brn, neu, cg_positive, sg_tier);

		DBG_PRT_COND(64, (neu != NULL_PT), os << "qua=" << &qua << " SRC=" << neu);

		BRAIN_CK((qua.qu_source == neu) || ((level() == ROOT_LEVEL) && (qua.qu_source == NULL_PT)));
		BRAIN_CK((qua.qu_tier == sg_tier) || ((level() == ROOT_LEVEL) && (qua.qu_tier == 0)));
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
brain::pulsate(){
	propagate_signals();

	BRAIN_CK(ck_trail());

	if(found_conflict()){
		if(level() == ROOT_LEVEL){ 
			set_result(k_no_satisf);
			return;
		}
		//retract();
		reverse();
		BRAIN_CK(has_psignals());

	} else {
		BRAIN_CK(! found_conflict());

		brain& brn = *this;

		quanton* pt_qua = NULL;
		pt_qua = choose_quanton();
		if(pt_qua == NULL){
			set_result(k_yes_satisf);
			return;
		}

		quanton& qua = *pt_qua;

		start_propagation(qua);

		quanton* cho = curr_choice();
		BRAIN_CK((cho == &qua) || (&(qua.opposite()) == cho));

		if(! cho->is_note5()){ 
			cho->set_note5(brn); 
			cho->qu_dbg_fst_lap_cho = brn.br_current_ticket.tk_recoil;
			br_dbg_all_chosen.push(cho);
		}
		if(cho->opposite().is_note5()){ 
			cho->qu_dbg_num_laps_cho++;
		}

		DBG_PRT(25, os << "**CHOICE** " << cho << " in tier=" << tier());

	}
}

long
quanton::find_uncharged_tunnel(){
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
		neuron* uchg_q = chg_qua.get_uncharged_tunnel(dbg_call_1);
		if(uchg_q == this){
			chg_qua.set_uncharged_tunnel(brn, chg_qua.find_uncharged_tunnel(), 1, this);
		}
	}

	bool has0 = qua0.has_charge();
	bool has1 = qua1.has_charge();

	neuron* uchg0 = qua0.get_uncharged_tunnel(dbg_call_2);
	neuron* uchg1 = qua1.get_uncharged_tunnel(dbg_call_3);

	if(! has0 && ! has1){
		if(uchg0 == NULL_PT){
			BRAIN_CK(qua0.qu_tunnels[ne_fibre_0_idx] == this);
			qua0.set_uncharged_tunnel(brn, ne_fibre_0_idx, 2, this);
			BRAIN_CK(qua0.get_uncharged_tunnel(dbg_call_4) == this);
		}
		if(uchg1 == NULL_PT){
			BRAIN_CK(qua1.qu_tunnels[ne_fibre_1_idx] == this);
			qua1.set_uncharged_tunnel(brn, ne_fibre_1_idx, 3, this);
			BRAIN_CK(qua1.get_uncharged_tunnel(dbg_call_5) == this);
		}
	}

	if(has0 && ! has1){

		if(uchg0 == NULL_PT){
			BRAIN_CK(qua0.qu_tunnels[ne_fibre_0_idx] == this);
			qua0.set_uncharged_tunnel(brn, ne_fibre_0_idx, 4, this);
			BRAIN_CK(qua0.get_uncharged_tunnel(dbg_call_6) == this);
		}
		if((uchg1 == this) || (uchg1 == NULL_PT)){
			qua1.set_uncharged_tunnel(brn, qua1.find_uncharged_tunnel(), 5, this);
		}
	}

	if(! has0 && has1){

		if((uchg0 == this) || (uchg0 == NULL_PT)){
			qua0.set_uncharged_tunnel(brn, qua0.find_uncharged_tunnel(), 6, this);
		}
		if(uchg1 == NULL_PT){
			BRAIN_CK(qua1.qu_tunnels[ne_fibre_1_idx] == this);
			qua1.set_uncharged_tunnel(brn, ne_fibre_1_idx, 7, this);
			BRAIN_CK(qua1.get_uncharged_tunnel(dbg_call_7) == this);
		}
	}

	if(has0 && has1){
		if((uchg0 == this) || (uchg0 == NULL_PT)){
			qua0.set_uncharged_tunnel(brn, qua0.find_uncharged_tunnel(), 8, this);
		}
		if((uchg1 == this) || (uchg1 == NULL_PT)){
			qua1.set_uncharged_tunnel(brn, qua1.find_uncharged_tunnel(), 9, this);
		}
	}

	//BRAIN_CK(qua0.ck_uncharged_tunnel());
	//BRAIN_CK(qua1.ck_uncharged_tunnel());
	//BRAIN_CK((pt_chg_qua == NULL_PT) || pt_chg_qua->ck_uncharged_tunnel());
}

bool
quanton::ck_uncharged_tunnel(){
	if(has_charge()){
		return true;
	}
	quanton& qua = *this;
	neuron* neu = get_uncharged_tunnel(dbg_call_1);
	if(neu == NULL_PT){
		long uch_idx = find_uncharged_tunnel();

		DBG_PRT_COND(DBG_ALL_LVS, ! (uch_idx == INVALID_IDX) ,
			os << "ABORTING_DATA " << bj_eol;
			os << "qua=" << &qua << bj_eol;
			neuron* tnn = qu_tunnels[uch_idx];
			os << "neu=" << tnn << bj_eol;
			os << "par=" << &(tnn->partner_fib(qua)) << bj_eol;
			brain* pt_brn = GLB().pt_brain;
			if(pt_brn != NULL_PT){
				os << " lv=" << pt_brn->level() << bj_eol;
				os << " trail_sz=" << pt_brn->br_charge_trail.get_num_motives() << bj_eol;
			}
			os << "END_OF_aborting_data" << bj_eol;
		);

		bool ok_uchg1 = (uch_idx == INVALID_IDX);
		BRAIN_CK(ok_uchg1);
		return ok_uchg1;
	}

	BRAIN_CK(neu->ne_original);
	BRAIN_CK(neu == qu_uncharged_tunnel);

	DBG_PRT_COND(DBG_ALL_LVS, ! (neu->is_partner_fib(qua)),
		os << "ABORTING_DATA x1 " << bj_eol;
		os << "qua=" << &qua << bj_eol;
		os << "neu=" << qu_uncharged_tunnel << bj_eol;
		os << "neu2=" << qua.qu_uncharged_tunnel << bj_eol;
		brain* pt_brn = GLB().pt_brain;
		if(pt_brn != NULL_PT){
			os << " lv=" << pt_brn->level() << bj_eol;
			os << " trail_sz=" << pt_brn->br_charge_trail.get_num_motives() << bj_eol;
		}
		os << "END_OF_aborting_data" << bj_eol;
	);

	BRAIN_CK(neu->is_partner_fib(qua));
	quanton& par = neu->partner_fib(qua);

	bool ok_uchg2 = (! par.has_charge());
	BRAIN_CK(ok_uchg2);
	return ok_uchg2;
}


neuron*
quanton::get_uncharged_tunnel(dbg_call_id dbg_call){
	DBG_PRT_COND(DBG_ALL_LVS, ! ((qu_uncharged_tunnel == NULL_PT) || qu_uncharged_tunnel->has_qua(*this)),
		os << "ABORTING_DATA " << bj_eol;
		os << "dbg_call=" << dbg_call << bj_eol;
		os << "qua=" << this << bj_eol;
		os << "neu=" << qu_uncharged_tunnel << bj_eol;
		brain* pt_brn = GLB().pt_brain;
		if(pt_brn != NULL_PT){
			os << " lv=" << pt_brn->level() << bj_eol;
			os << " trail_sz=" << pt_brn->br_charge_trail.get_num_motives() << bj_eol;
		}
		os << "END_OF_aborting_data" << bj_eol;
	);
	BRAIN_CK((qu_uncharged_tunnel == NULL_PT) || qu_uncharged_tunnel->has_qua(*this));
	return qu_uncharged_tunnel;
}

void
quanton::set_uncharged_tunnel(brain& brn, long uidx, long dbg_call, neuron* dbg_neu){
	if(uidx == INVALID_IDX){
		qu_bak_uncharged_tunnel = qu_uncharged_tunnel;
		brn.data_level().ld_semi_monos.push(this);		

		DBG_PRT_COND(105, (qu_id == 5), os << "qua=" << this << " setting uncharged to NULL" << bj_eol
			<< " LV=" << brn.level());

		qu_uncharged_tunnel = NULL_PT;
		BRAIN_CK(ck_uncharged_tunnel());
		return;
	}

	BRAIN_CK(qu_tunnels.is_valid_idx(uidx));
	BRAIN_CK(qu_tunnels[uidx] != NULL_PT);

	qu_uncharged_tunnel = qu_tunnels[uidx];

	BRAIN_CK(qu_uncharged_tunnel->ne_original);
	BRAIN_CK(qu_uncharged_tunnel->is_partner_fib(*this));
	BRAIN_CK(ck_uncharged_tunnel());
}

void
quanton::reset_uncharged_tunnel(brain& brn){

	if((qu_uncharged_tunnel == NULL_PT) && (qu_bak_uncharged_tunnel != NULL_PT)){
		qu_uncharged_tunnel = qu_bak_uncharged_tunnel;
	}

	quanton& qua = *this;
	neuron* neu = qu_uncharged_tunnel;
	if((neu == NULL_PT) || ! neu->is_partner_fib(qua) || neu->partner_fib(qua).has_charge()){
		long uch_idx = find_uncharged_tunnel();
		if(uch_idx != INVALID_IDX){
			qu_uncharged_tunnel = qu_tunnels[uch_idx];
		} else {
			qu_uncharged_tunnel = NULL_PT;
			brn.data_level().ld_semi_monos.push(this);		
		}

	}
	qu_bak_uncharged_tunnel = NULL_PT;

	DBG_PRT_COND(105, (qu_id == 5), os << "qua=" << this << "RESETTING uncharged to " << qu_uncharged_tunnel);

	BRAIN_CK(ck_uncharged_tunnel());
}


void
leveldat::reset_semi_monos(brain& brn){
	while(! ld_semi_monos.is_empty()){
		quanton& qua = *(ld_semi_monos.pop());
		if(! qua.qu_in_uncharged_to_update){
			brn.br_semi_monos.push(&qua);
			qua.qu_in_uncharged_to_update = true;
		}
	}
}

void
brain::update_semi_monos(){
	brain& brn = *this;
	data_level().reset_semi_monos(brn);
	while(! br_semi_monos.is_empty()){
		quanton& qua = *(br_semi_monos.pop());
		BRAIN_CK(qua.qu_in_uncharged_to_update);
		qua.qu_in_uncharged_to_update = false;
		qua.reset_uncharged_tunnel(brn);
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
brain::retract_choice(){
	BRAIN_CK(! found_conflict());
	BRAIN_CK(! has_psignals());
	BRAIN_CK(br_semi_monos.is_empty());

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
}

bool
dbg_run_satex_on(brain& brn, ch_string f_nam){
	if(path_exists(f_nam)){
		std::ostringstream o_str;
		o_str << "satex -s " << f_nam;

		system_exec(o_str);
		ch_string lg_nm = get_log_name(f_nam, LOG_NM_RESULTS);

		BRAIN_CK(path_exists(lg_nm));
		bool is_no_sat = all_results_batch_instances(lg_nm, k_no_satisf);
		MARK_USED(is_no_sat);

		DBG_PRT_COND(DBG_ALL_LVS, ! is_no_sat ,
			os << "ABORTING_DATA " << bj_eol;
			//os << "mmap_before_tk=" << ma_before_retract_tk << bj_eol;
			//os << "mmap_after_tks=" << ma_after_retract_tks << bj_eol;
			os << " brn_tk=" << brn.br_current_ticket << bj_eol;
			os << "	LV=" << brn.level() << bj_eol;
			os << " f_nam=" << f_nam << bj_eol;
			os << " save_consec=" << skg_dbg_canon_save_id << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		BRAIN_CK(is_no_sat);
	}
	return true;
}

bool
memap::map_ck_simple_no_satisf(mem_op_t mm, brain& brn){
	//long tot_vars = brn.br_positons.size();
	//long tot_lits = 0;
	//long tot_twolits = 0;
	row<neuron*>& dbg_neus = brn.br_dbg_simple_neus;
	row<canon_clause*>& dbg_ccls = brn.br_dbg_ccls;
	skeleton_glb& skg = brn.get_skeleton();
	canon_cnf& dbg_cnf = brn.br_dbg_cnf;

	dima_dims dbg_dims;

	map_set_dbg_cnf(mm, brn, dbg_ccls, dbg_neus, dbg_dims);
	//dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, INVALID_NATURAL, INVALID_NATURAL, false);
	dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, INVALID_NATURAL, INVALID_NATURAL);

	//DBG_PRT_COND(115, false, 
	//	os << " dbg_simple_cnf=" << bj_eol << dbg_cnf << bj_eol 
	//	<< "<<<<<<<<<<" << bj_eol << dbg_cnf.cf_sha_str << " sv_id" << skg_dbg_canon_save_id
	//);

	BRAIN_CK(dbg_cnf.cf_dims.dd_tot_ccls == dbg_cnf.cf_clauses.size());

	//DBG_PRT(106, os << "MAP NEURONS=" << bj_eol; dbg_print_ccls_neus(os, dbg_cnf.cf_clauses));
	//DBG_PRT(106, os << "tot_ccls=" << dbg_cnf.cf_dims.dd_tot_ccls);

	ch_string dbg_cnf_nm = skg.kg_running_path + "/DBG_CNF_CK_SAVE.yos";
	canon_save(dbg_cnf_nm, dbg_cnf.cf_chars, false);

	BRAIN_CK(dbg_cnf.cf_dims.dd_tot_ccls == dbg_cnf.cf_clauses.size());

	DBG_CHECK_SAVED(dbg_run_satex_on(brn, dbg_cnf_nm));

	//DBG_PRT(106, 
	//	os << "dbg_cnf_nm=" << dbg_cnf_nm << bj_eol;
		//map_dbg_prt(os, mm, brn);
	//);

	dbg_cnf.clear_cnf();

	return true;
}

void
dbg_prepare_used_dbg_ccl(row_quanton_t& rr_qua, canon_clause& dbg_ccl){
	dbg_ccl.cc_clear(false);


	for(long aa = 0; aa < rr_qua.size(); aa++){
		BRAIN_CK(rr_qua[aa] != NULL_PT);
		quanton& qua = *(rr_qua[aa]);

		if(qua.has_mark()){ 
			dbg_ccl.cc_push(qua.qu_id);
		}
	}
}

void
memap::dbg_prepare_used_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls){
	long trace_sz = get_trace_sz(mm);
	long filled_sz = get_filled_sz(mm);
	MARK_USED(filled_sz);

	row<prop_signal>& trace = ma_dotted;
	BRAIN_CK((trace_sz == trace.size()) || trace.is_valid_idx(trace_sz));

	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	set_marks_of(brn, trace, 0, trace_sz);

	the_ccls.clear();

	row<neuron*>& all_orig = brn.br_dbg_original_used;

	for(long aa = 0; aa < all_orig.size(); aa++){
		BRAIN_CK(all_orig[aa] != NULL_PT);
		neuron& neu = *(all_orig[aa]);
		canon_clause& dbg_ccl = neu.ne_dbg_ccl;

		dbg_prepare_used_dbg_ccl(neu.ne_fibres, dbg_ccl);
		if(! dbg_ccl.cc_size() == 0){
			the_ccls.push(&dbg_ccl);
		}
	}

	reset_marks_of(brn, trace, 0, trace_sz);

	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
}

bool
memap::dbg_ck_used_simple_no_satisf(mem_op_t mm, brain& brn){

	row<canon_clause*>& dbg_ccls = brn.br_dbg_ccls;
	skeleton_glb& skg = brn.get_skeleton();
	canon_cnf& dbg_cnf = brn.br_dbg_cnf;

	dbg_prepare_used_dbg_cnf(mm, brn, dbg_ccls);

	if(dbg_ccls.is_empty()){
		return true;
	}

	dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, INVALID_NATURAL, INVALID_NATURAL, false);

	//DBG_PRT(106, os << "USED NEURONS=" << bj_eol; dbg_print_ccls_neus(os, dbg_cnf.cf_clauses));

	//DBG_PRT_COND(115, ((skg_dbg_canon_save_id == 2) || (skg_dbg_canon_save_id == 22)), 
	//	os << " dbg_used_cnf=" << bj_eol << dbg_cnf << bj_eol 
	//	<< "<<<<<<<<<<" << bj_eol << dbg_cnf.cf_sha_str << " sv_id" << skg_dbg_canon_save_id
	//);

	ch_string dbg_cnf_nm = skg.kg_running_path + "/DBG_CNF_CK_USED.yos";
	canon_save(dbg_cnf_nm, dbg_cnf.cf_chars, false);

	DBG_CHECK_SAVED(dbg_run_satex_on(brn, dbg_cnf_nm));

	//DBG_PRT(106, os << "dbg_cnf_nm=" << dbg_cnf_nm);

	dbg_cnf.clear_cnf();

	return true;
}

void
brain::dbg_add_to_used(neuron& neu){
	if(! neu.ne_dbg_in_used){
		neu.ne_dbg_in_used = true;
		if(neu.ne_original){
			br_dbg_original_used.push(&neu);
		} 
	}
}

//============================================================
// map oper funcs

/*
void
neuron::mem_ne_fill_sortees(brain& brn, quanton& aft_qua){

	BRAIN_CK(aft_qua.qu_inverse != NULL_PT);
	DBG(quanton& cf_qu = brn.br_conflict_quanton);

	row<sortee*>& all_after = aft_qua.qu_reltee.so_after;
	BRAIN_CK(all_after.is_empty());
	DBG(long aft_ti = aft_qua.qu_dbg_tee_ti);
	BRAIN_CK(aft_ti > 0);

	DBG(bool found_it = false);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);
		quanton& opp = *(qua.qu_inverse);

		BRAIN_CK(&cf_qu != &qua);
		BRAIN_CK(&cf_qu != &opp);

		if(&qua == &aft_qua){ 
			DBG(found_it = true);
			continue;	
		}
		BRAIN_CK(&opp != &aft_qua);
		//BRAIN_CK(! qua.has_pos_mark());

		//if(! opp.has_pos_mark()){ 
		if(! opp.has_mark()){ 
			continue; 
		}
		BRAIN_CK(opp.has_pos_mark());
		BRAIN_CK(qua.has_neg_mark());
	
		sortee& srt = opp.qu_tee;

		BRAIN_CK(qua.qu_dbg_tee_ti == INVALID_NATURAL);
		DBG(long qu_ti = opp.qu_dbg_tee_ti);
		BRAIN_CK(qu_ti > 0);
		BRAIN_CK(aft_ti < qu_ti);

		all_after.push(&srt);

		row<sortee*>& all_before = opp.qu_reltee.so_before;
		all_before.push(&(aft_qua.qu_tee));
	}

	BRAIN_CK((&aft_qua == &cf_qu) || found_it);
}
*/

long
memap::get_trace_sz(mem_op_t mm){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(ma_szs_dotted.size() > 1);

	long t_sz = ma_dotted.size();
	if(mm == mo_save){
		long sz_idx = ma_szs_dotted.last_idx() - 1;
		BRAIN_CK(ma_szs_dotted.is_valid_idx(sz_idx));
		t_sz = ma_szs_dotted[sz_idx];

		BRAIN_CK(t_sz >= 0);
		BRAIN_CK(t_sz <= t_sz);
	} 
	BRAIN_CK((mm != mo_find) || (t_sz > ma_szs_dotted.first()));

	return t_sz;
}

long 
memap::get_filled_sz(mem_op_t mm){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(ma_szs_filled.size() > 1);

	long f_sz = ma_filled.size();
	if(mm == mo_save){
		long sz_idx = ma_szs_filled.last_idx() - 1;
		BRAIN_CK(ma_szs_filled.is_valid_idx(sz_idx));
		f_sz = ma_szs_filled[sz_idx];

		BRAIN_CK(f_sz >= 0);
		BRAIN_CK(f_sz <= f_sz);
	} 

	return f_sz;
}

/*
void
brain::dbg_get_all_chosen(row<quanton*>& all_cho){
	all_cho.clear();

	for(long ii = 0; ii < br_positons.size(); ii++){
		quanton& qua_pos = br_positons[ii];
		if(qua_pos.is_note5()){
			all_cho.push(&qua_pos);
		}
	}

	for(long ii = 0; ii < br_negatons.size(); ii++){
		quanton& qua_neg = br_negatons[ii];
		if(qua_neg.is_note5()){
			all_cho.push(&qua_neg);
		}
	}
}
*/


