

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

dbg_brain.cpp  

dbg_brain funcs.

--------------------------------------------------------------*/

#include "file_funcs.h"
#include "brain.h"
#include "dbg_run_satex.h"

ch_string	
dbg_trinary_to_str(charge_t obj){
#ifdef FULL_DEBUG
	if(obj == cg_positive){
		return "pos";
	} else if(obj == cg_negative){
		return "neg";
	} else {
		BRAIN_CK_0(obj == cg_neutral);
		return "ntr";
	}
#endif
	return "???";
}

bool
quanton::ck_charge(brain& brn){
#ifdef FULL_DEBUG
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
#endif
	return true;
}

ch_string
brain::dbg_prt_margin(bool is_ck){
	ch_string f_nam = "INVALID_FILE_NAME";
#ifdef FULL_DEBUG
	bj_ostream& os = bj_dbg;
	instance_info& inst_info = get_my_inst();
	if(inst_info.ist_group_id > 1){
		os << "#" << inst_info.ist_id << ".";
	}

	f_nam = inst_info.ist_file_path;
	
	recoil_counter_t the_lap = recoil();
	if(the_lap > 0){
		if(is_ck){ os << "LAP="; }
		os << the_lap << ".";
	}
#endif
	return f_nam;
}

void
brain::init_all_dbg_brn(){
#ifdef FULL_DEBUG
	br_forced_srg.set_dbg_brn(this);
	br_filled_srg.set_dbg_brn(this);
	br_guide_neus_srg.set_dbg_brn(this);
	br_guide_quas_srg.set_dbg_brn(this);
	br_tauto_neus_srg.set_dbg_brn(this);
	br_tauto_quas_srg.set_dbg_brn(this);
	br_clls_srg.set_dbg_brn(this);
	
	br_dbg.dbg_cnf.set_dbg_brn(this);
	br_tmp_wrt_tauto_cnf.set_dbg_brn(this);
	br_tmp_wrt_diff_cnf.set_dbg_brn(this);
	br_tmp_wrt_guide_cnf.set_dbg_brn(this);
	
	get_skeleton().set_dbg_brn(this);
	br_pt_brn = this; // this tells all dbg funcs that brain is ready for dbg.
#endif
}

bool
brain::ck_trail(){	
#ifdef FULL_DEBUG
	bj_ostream& os = bj_dbg;
	brain& brn = *this;

	row_quanton_t& the_trl = br_tmp_trail;
	br_charge_trail.get_all_ordered_quantons(the_trl);

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
		dbg_prt_lvs_cho(os);
		ab_mm = "case6." + br_file_name;
		abort_func(-1, ab_mm.c_str());
	}
#endif
	return true;
}

void	due_periodic_prt(void* pm, double curr_secs){
#ifdef FULL_DEBUG
	bj_out << "due_periodic_prt=" << curr_secs << bj_eol;
	bj_out.flush();
	
	brain& brn = *((brain*)pm);
	if(brn.br_dbg.dbg_periodic_prt){
		instance_info& inst_info = brn.get_my_inst();
		bj_out << inst_info << bj_eol;
	}
#endif
}

void
brain::check_timeout(){
#ifdef FULL_DEBUG
	if(br_prt_timer.check_period(due_periodic_prt, this)){
		throw timeout_exception();
	}
#endif
}

bool
ck_motives(brain& brn, row_quanton_t& mots){
#ifdef FULL_DEBUG
	for(long ii = 0; ii < mots.size(); ii++){
		quanton* mot = mots[ii];
		MARK_USED(mot);
		BRAIN_CK(mot != NULL_PT);
		BRAIN_CK(mot->qlevel() <= brn.level());

		BRAIN_CK(mot->get_charge() == cg_negative);
	}
#endif
	return true;
}

void
brain::dbg_add_to_used(neuron& neu){
#ifdef FULL_DEBUG
	if(! neu.ne_dbg_in_used){
		neu.ne_dbg_in_used = true;
		if(neu.ne_original){
			br_dbg.dbg_original_used.push(&neu);
		} 
	}
#endif
}

#ifdef FULL_DEBUG
void
dbg_inst_info::init_dbg_inst_info(){
	dbg_before_retract_lv = INVALID_LEVEL;
	dbg_last_recoil_lv = INVALID_LEVEL;
	
	dbg_simple_neus.clear();
	dbg_used_neus.clear();
	dbg_ccls.clear();
	dbg_cnf.clear_cnf();
	
	dbg_do_finds = false;
	
	dbg_find_id = 0;
	dbg_save_id = 0;
	dbg_canon_find_id = 0;
	dbg_canon_save_id = 0;
	
	dbg_original_used.clear();
	dbg_all_chosen.clear();
	
	dbg_ic_active = false;
	dbg_ic_after = false;
	dbg_ic_max_seq = -1;
	dbg_ic_seq = 0;
	dbg_ic_gen_jpg = false;
	
	dbg_just_read = false;
	dbg_clean_code = false;
	
	dbg_periodic_prt = true;

	dbg_start_dbg_entries.clear();
	dbg_stop_dbg_entries.clear();
	dbg_current_start_entry = 0;
	dbg_current_stop_entry = 0;
	
	dbg_bad_cycle1 = false;
	dbg_levs_arr.fill(false, DBG_NUM_LEVS);
	
	dbg_tot_nmps = 0;
}
#endif

void
dbg_find_not_in_rr1(brain& brn, row<neuron*>& rr1, row<neuron*>& rr2, 
					row<neuron*>& not_in_rr1){
#ifdef FULL_DEBUG
	not_in_rr1.clear();

	BRAIN_CK(brn.br_tot_ne_spots == 0);
	set_spots_of(brn, rr1);

	for(long aa = 0; aa < rr2.size(); aa++){
		BRAIN_CK(rr2[aa] != NULL_PT);
		neuron& neu = *(rr2[aa]);

		if(! neu.ne_spot){
			not_in_rr1.push(&neu);
		} 
	}

	reset_spots_of(brn, rr1);
	BRAIN_CK(brn.br_tot_ne_spots == 0);
#endif
}

void
dbg_find_diff_tauto_vs_simple_neus(brain& brn, row<neuron*>& not_in_tauto, 
								   row<neuron*>& not_in_simple)
{
#ifdef FULL_DEBUG
	row<neuron*>& dbg_simple_neus = brn.br_dbg.dbg_simple_neus;

	row<neuron*> dbg_tauto_neus;
	srt_row_as<neuron>(brn.br_tauto_neus_srg.sg_step_sortees, dbg_tauto_neus);

	dbg_find_not_in_rr1(brn, dbg_tauto_neus, dbg_simple_neus, not_in_tauto);
	dbg_find_not_in_rr1(brn, dbg_simple_neus, dbg_tauto_neus, not_in_simple);
#endif
}

bool
dbg_prt_diff_tauto_vs_simple_neus(bj_ostream& os, brain& brn){
#ifdef FULL_DEBUG
	row<neuron*> not_in_tauto;
	row<neuron*> not_in_simple;

	dbg_find_diff_tauto_vs_simple_neus(brn, not_in_tauto, not_in_simple);

	os << "not_in_tauto=" << bj_eol;
	os << not_in_tauto << bj_eol;
	os << "not_in_simple=" << bj_eol;
	os << not_in_simple << bj_eol;

#endif
	return true;
}

void
brain::dbg_check_sat_assig(){
#ifdef FULL_DEBUG
	row_quanton_t& the_assig = br_tmp_assig_quantons;
	if(the_assig.is_empty()){
		br_charge_trail.get_all_ordered_quantons(the_assig);
	}
	
	row<neuron*>& neus = br_tmp_ck_sat_neus;
	fill_with_origs(neus);

	if(! brn_dbg_compute_binary(neus)){
		abort_func(1, "FATAL ERROR 001. Wrong is_sat answer !");
	}

	if(! brn_dbg_compute_dots_of(neus, the_assig)){
		abort_func(1, "FATAL ERROR 002. Wrong is_sat answer !");
	}

	DBG_PRT(4, os << "CHECKED_ASSIG=" << the_assig << bj_eol);

	//print_satifying(cho_nm);
#endif
}

bool
brain::brn_dbg_compute_binary(row<neuron*>& neus){
#ifdef FULL_DEBUG
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
#endif
	return true;
}

//for IS_SAT_CK
bool
brain::brn_dbg_compute_dots(row<neuron*>& neus){
#ifdef FULL_DEBUG
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
#endif
	return true;
}

//for IS_SAT_CK
bool
brain::brn_dbg_compute_dots_of(row<neuron*>& neus, row_quanton_t& assig){
	bool resp = true;
#ifdef FULL_DEBUG
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

	resp = brn_dbg_compute_dots(neus);
	reset_dots_of(brn, assig);
	BRAIN_CK(br_tot_qu_dots == 0);
#endif
	return resp;
}

//============================================================
// print methods

bj_ostream&
neuromap::print_neuromap(bj_ostream& os, bool from_pt){
#ifdef FULL_DEBUG
	MARK_USED(from_pt);
	if(from_pt){
		os << "NA";
		if(na_is_head){ os << ".h"; }
		os << "(" << (void*)this << ")";
		os << " o_lv=" << na_orig_lv;
		os << " cho=" << na_orig_cho;
		os.flush();
		return os;
	}
	os << "NEUMAP(" << (void*)this <<")={ " << bj_eol;
	
	os << " active=" << na_active << bj_eol;
	os << " na_deact_tier=" << na_deact_tier << bj_eol;
	os << " cho=" << na_orig_cho << bj_eol;
	os << " na_submap=" << na_submap << bj_eol;
	
	os << " na_forced=" << bj_eol;
	os << na_forced << bj_eol;
	os << " na_non_forced=" << bj_eol;
	na_non_forced.print_row_data(os, true, "\n");
	
	os << " ma_guide_col=" << bj_eol;
	os << na_guide_col << bj_eol;
	
	os << " na_all_filled_in_propag=" << bj_eol;
	na_all_filled_in_propag.print_row_data(os, true, "\n");
	os << "}";
	os.flush();
#endif
	return os;
}
	
void
brain::print_trail(bj_ostream& os, bool no_src_only){
#ifdef FULL_DEBUG
	row_quanton_t& the_trl = br_tmp_trail;
	br_charge_trail.get_all_ordered_quantons(the_trl);
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
#endif
}

bj_ostream& 
brain::print_all_quantons(bj_ostream& os, long ln_sz, ch_string ln_fd){
#ifdef FULL_DEBUG
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
#endif
	return os;
}

bj_ostream&
quanton::print_quanton(bj_ostream& os, bool from_pt){
#ifdef FULL_DEBUG
	MARK_USED(from_pt);

	brain* pt_brn = get_dbg_brn();

	/*if((pt_brn != NULL_PT) && (this == &(pt_brn->br_conflict_quanton))){
		os << "CONFL_QUA";
		os.flush();
		return os;
	}*/

	if((pt_brn != NULL_PT) && (this == &(pt_brn->br_top_block))){
		os << "TOP_BLOCK_QUA";
		os.flush();
		return os;
	}

	bool is_nega = is_neg();
	bool is_posi = is_pos();
	bool with_dot = has_dot();
	bool with_mark = has_mark();
	//bool has_src = has_source();
	bool dominated = false;
	MARK_USED(dominated);
	neuron* neu = qu_source;
	bool n0 = has_note0();
	bool h_src = (neu != NULL_PT);
	bool h_chg = has_charge();
	long qlv = qlevel();
	
	if(pt_brn != NULL_PT){
		dominated = in_qu_dominated(*(pt_brn));
	}

	/*if(from_pt){
		os << qu_id; 
		os.flush();
		return os;
	}*/

	if(from_pt){
		//if(qu_block != NULL_PT){ os << "b"; }
		if((neu != NULL_PT) && ! neu->ne_original){ os << "+"; }
		//if((neu != NULL_PT) && neu->ne_original){ os << "o"; }
		//if(! has_source() && has_charge()){ os << "*"; }
		if(qlv == 0){ os << "#"; }
		if(! h_src && h_chg){ os << "L" << qlv; }

		if(! h_chg){ os << "("; }
		if(is_nega){ os << '\\';  }
		if(is_posi){ os << '/';  }
		os << qu_id; 
		if(is_posi){ os << '\\';  }
		if(is_nega){ os << '/';  }
		if(! h_chg){ os << ")"; }

		//os << ".t" << qu_tier;
		if((! h_src) && (pt_brn != NULL_PT) && h_chg){ 
			brain& brn = *pt_brn;
			
			leveldat& lv = brn.get_data_level(qlv);
			if(lv.has_setup_neuromap()){ os << ".s"; }
			if(lv.has_to_write_neuromaps()){ os << ".w"; }
			if(lv.has_learned()){ 
				os << ".l"; 
				if(lv.ld_learned.size() > 1){
					os << "l"; 
				}
			}
			if((qlv != ROOT_LEVEL) && (lv.ld_chosen == NULL_PT)){
				os << "[NULL_CHO!!!]"; 
			}
		}
		
		if(n0){ os << ".n0"; }
		if(with_dot){ os << ".d"; }
		if(with_mark){ os << ".m"; }

		if(! qu_tee.is_unsorted()){ os << ".q" << qu_tee.so_qua_id; }

		//if(dominated){ os << ".DOM"; }

		os.flush();
		return os;
	}

	os << "\n";
	os << "QID " << qu_id << " ";
	os << "i" << qu_index << " ";
	os << "INV " << qu_inverse->qu_id << "\n";
	os << "CHG " << dbg_trinary_to_str(qu_charge) << " ";
	os << "chtk{" << qu_charge_tk << "} ";
	os << "sp_" << dbg_trinary_to_str(qu_spin) << " ";
	os << "st_" << qu_choice_idx << " ";
	os << "d_" << dbg_trinary_to_str(qu_dot) << "\n";
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
#endif
	return os;
}

bj_ostream&
neuron::print_neuron(bj_ostream& os, bool from_pt){
	return print_neu_base(os, ! from_pt, true, true);
}

bj_ostream&	
neuron::print_neu_base(bj_ostream& os, bool detail, bool prt_src, bool sort_fib){
#ifdef FULL_DEBUG
	if(! detail){
		os << "ne={";
		os << ((void*)(this)) << " ";
		os << ne_index << " ";
		os << "nmp=" << ((void*)ne_curr_nemap) << " ";
		if(ne_original){
			os << "o";
		}
		os << ne_fibres;
		os << "}";

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
#endif
	return os;
}

bj_ostream&
brain::print_brain(bj_ostream& os){
#ifdef FULL_DEBUG
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
	//os << "\n choices:"; 
	//br_choices.print_row_data(os, true, " ", br_choices_lim, br_choices_lim); 
	//os << bj_eol;
	os << "\n choices:"; br_choices.print_row_data(os, true, " "); os << bj_eol;
	//os << "\n satisfying:" << satisfying << bj_eol;
	os << bj_eol;

	os.flush();
#endif
	return os;
}

bj_ostream&
brain::print_all_original(bj_ostream& os){
#ifdef FULL_DEBUG
	row<neuron*>& neus = br_tmp_prt_neus;
	fill_with_origs(neus);
	neus.print_row_data(os, false, "\n", -1, -1, true);
#endif
	return os;
}

void
brain::print_active_maps(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "[";
	for(long aa = 0; aa < br_maps_active.size(); aa++){
		neuromap* mpp = br_maps_active[aa];
		os << " " << (void*)(mpp);
	}
	os << " ]";
	os << bj_eol;

	os.flush();
#endif
}

bool
quanton::ck_all_tunnels(){
#ifdef FULL_DEBUG
	for(long ii = 0; ii < qu_tunnels.size(); ii++){
		neuron* neu = qu_tunnels[ii];
		neu->ck_tunnels();
	}
#endif
	return true;
}

bool
quanton::ck_uncharged_partner_neu(){
	bool ok_uchg2 = true;
#ifdef FULL_DEBUG
	if(has_charge()){
		return true;
	}
	quanton& qua = *this;
	neuron* neu = get_uncharged_partner_neu(dbg_call_1);
	if(neu == NULL_PT){
		long uch_idx = find_uncharged_partner_neu();
		bool ok_uchg1 = (uch_idx == INVALID_IDX);
		BRAIN_CK(ok_uchg1);
		return ok_uchg1;
	}

	BRAIN_CK(neu->ne_original);
	BRAIN_CK(neu == qu_uncharged_partner_neu);
	BRAIN_CK(neu->is_partner_fib(qua));
	quanton& par = neu->partner_fib(qua);

	ok_uchg2 = (! par.has_charge());
	BRAIN_CK(ok_uchg2);
#endif
	return ok_uchg2;
}

bool
neuron::ck_all_charges(brain* brn, long from){
	bool all_ok = true;
#ifdef FULL_DEBUG
	for(long ii = from; ii < fib_sz(); ii++){
		all_ok = (all_ok && (ne_fibres[ii]->is_neg()));
	}
#endif
	return all_ok;
}

bool
neuron::ck_all_has_charge(long& npos){
	bool all_ok = true;
#ifdef FULL_DEBUG
	npos = 0;
	for(long ii = 0; ii < fib_sz(); ii++){
		quanton& qua = *(ne_fibres[ii]);
		all_ok = (all_ok && qua.has_charge());
		all_ok = (all_ok && qua.has_tier());
		if(qua.is_pos()){ npos++; }
	}
#endif
	return all_ok;
}

bool
neuron::ck_no_source_of_any(){
#ifdef FULL_DEBUG
	for(long ii = 0; ii < fib_sz(); ii++){
		quanton* qua = ne_fibres[ii];
		MARK_USED(qua);
		BRAIN_CK_0(qua->get_source() != this);
	}
#endif
	return true;
}

void 
brain::dbg_prt_lvs_have_learned(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "lrnd=[";
	
	row<leveldat*>& all_lv = br_data_levels;
	for(int aa = 0; aa < all_lv.size(); aa++){
		leveldat& lv = *(all_lv[aa]);
		if(lv.has_learned()){
			os << "1.";
		} else {
			os << "0.";
		}
	}
	os << "]";
#endif
}

void
brain::dbg_prt_lvs_active(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "actv=[";
	
	row<leveldat*>& all_lv = br_data_levels;
	for(int aa = 0; aa < all_lv.size(); aa++){
		leveldat& lv = *(all_lv[aa]);
		if(lv.has_active_neuromaps()){
			os << "1.";
		} else {
			os << "0.";
		}
	}
	os << "]";
#endif
}

void 
brain::dbg_prt_lvs_cho(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "chos=[";
	
	row<leveldat*>& all_lv = br_data_levels;
	for(int aa = 0; aa < all_lv.size(); aa++){
		leveldat& lv = *(all_lv[aa]);
		quanton* ch2 = NULL_PT;
		if(lv.has_setup_neuromap()){
			neuromap& nmp1 = lv.get_setup_neuromap();
			ch2 = nmp1.na_orig_cho;
		}
		os << lv.ld_chosen;
		if(ch2 != NULL){ os << "."; } else { os << ","; }
		if((ch2 != NULL) && (ch2 != lv.ld_chosen)){
			os << "\n\n\n" << ch2 << " != " << lv.ld_chosen << "!!!!!\n\n\n";
		}
	}
	os << "]";
#endif
}

bool
dbg_run_satex_on(brain& brn, ch_string f_nam){
#ifdef FULL_DEBUG
	bool is_no = dbg_run_satex_is_no_sat(f_nam);
	MARK_USED(is_no);
	DBG_COND_COMM(! is_no ,
		os << "ABORTING_DATA " << bj_eol;
		//os << "mmap_before_tk=" << ma_before_retract_tk << bj_eol;
		//os << "mmap_after_tks=" << ma_after_retract_tks << bj_eol;
		os << " brn_tk=" << brn.br_current_ticket << bj_eol;
		os << "	LV=" << brn.level() << bj_eol;
		os << " f_nam=" << f_nam << bj_eol;
		os << " save_consec=" << brn.br_dbg.dbg_canon_save_id << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	BRAIN_CK(is_no);
#endif
	return true;
}

bool
brain::dbg_in_edge_of_level(){
	long trl_lv = trail_level();
	long b_lv = level();
	bool in_edge = (trl_lv != b_lv);
	BRAIN_CK(! in_edge || ((trl_lv + 1) == b_lv));
	return in_edge;
}

bool
brain::dbg_in_edge_of_target_lv(deduction& dct){
	if(dct.is_dt_virgin()){ return false; }
	long trl_lv = trail_level();
	//bool in_tg_lv = (trl_lv < dct.dt_target_level);
	bool in_tg_lv = (trl_lv <= dct.dt_target_level);
	BRAIN_CK(! in_tg_lv || ((trl_lv + 1) == level()));
	return in_tg_lv;
}

void
brain::dbg_old_reverse(){
	DEDUC_DBG(deduction& dct2 = br_dbg.dbg_deduc);
	//long dbg_old_lv = level();

	BRAIN_CK(! has_psignals());
	brain& brn = *this;
	notekeeper& nke0 = br_retract_nke0;
	deduction& dct = br_retract_dct;
	long& all_notes0 = br_qu_tot_note0;
	//row_quanton_t& all_rev = br_tmp_rever_quas;
	//MARK_USED(all_rev);
	MARK_USED(all_notes0);

	dct.init_deduction();
	nke0.init_notes(level());

	BRAIN_CK(dct.is_dt_virgin());
	BRAIN_CK(nke0.dk_tot_noted == 0);
	BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(all_notes0 == 0);
	BRAIN_CK(br_semi_monos_to_update.is_empty());
	//BRAIN_CK(all_rev.is_empty());

	// START REVERSE (init nke0)

	BRAIN_CK(found_conflict());
	
	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());

	DEDUC_DBG(br_deducer_anlsr.deduction_analysis(br_conflict_found, dct2));
	BRAIN_CK_PRT(dct2.dt_target_level >= ROOT_LEVEL, 
		os << recoil() << ".dct2=" << dct2
	);

	neuron& cfl = *(br_conflict_found.ps_source);
	cfl.old_set_motives(brn, nke0, true); // init nke0 with confl

	BRAIN_CK(all_notes0 > 0);

	// REVERSE LOOP

	//quanton* chosen_qua = NULL_PT;
	bool has_in_mem = false;
	MARK_USED(has_in_mem);

	while(true){
		bool end_of_recoil = dbg_in_edge_of_target_lv(dct);
		if(end_of_recoil){
			BRAIN_CK(! dct.is_dt_virgin());
			bool in_mm = false;
			if(! in_mm){ 
				BRAIN_CK((trail_level() + 1) == level());
				//break;
			} else {
				has_in_mem = true;
				dct.init_deduction();
				BRAIN_CK(dct.is_dt_virgin());
			}
		}
		
		BRAIN_CK(level() != ROOT_LEVEL);

		bool end_of_lev = dbg_in_edge_of_level();
		if(end_of_lev){
			BRAIN_CK(level() != ROOT_LEVEL);
			BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			BRAIN_CK(level() <= nke0.dk_note_layer);

			dec_level();
		} // end_of_lev

		BRAIN_CK(trail_level() == level());

		if(end_of_recoil){ break; }
		
		if(level() == ROOT_LEVEL){
			break;
		}

		if(! br_charge_trail.has_motives()){
			ch_string ab_mm = brn.br_file_name;
			abort_func(-1, ab_mm.c_str());
			break;
		}
		
		// set qua
		quanton& qua = trail_last();
		
		BRAIN_CK(qua.has_charge());
		BRAIN_CK(qua.has_tier());

		BRAIN_CK(qua.qu_inverse != NULL_PT);
		//quanton& opp = *(qua.qu_inverse);

		neuron* src = qua.get_source();

		BRAIN_CK(qua.is_pos());
		BRAIN_CK(dct.is_dt_virgin() || (level() >= dct.dt_target_level));

		// notes

		DBG(bool had_n3 = false);
		DBG(bool had_n0 = false);

		if(qua.has_note0()){
			long qlv = qua.qlevel();
			nke0.update_notes_layer(qlv);
			BRAIN_CK(nke0.dk_num_noted_in_layer > 0);

			qua.reset_its_note0(brn);
			
			BRAIN_CK(qua.in_qu_dominated(brn));
		
			BRAIN_CK(qua.qlevel() == nke0.dk_note_layer);

			nke0.dec_notes();

			if(nke0.dk_num_noted_in_layer == 0){
				if(dct.is_dt_virgin()){
					dct.set_with(brn, nke0, qua);
				}

				BRAIN_CK(! dct.is_dt_virgin());
			}

			if(src != NULL_PT){
				src->old_set_motives(brn, nke0, false);
				BRAIN_CK(nke0.dk_num_noted_in_layer > 0);
				BRAIN_CK(! src->ne_original || has_neu(qua.qu_full_charged, src));
			} else {
				BRAIN_CK(! qua.has_source());
				BRAIN_CK(! dct.is_dt_virgin());
				BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			}

		} // qua.has_note0()

		// chosen

		if(! qua.has_source()){
			BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			BRAIN_CK(! dct.is_dt_virgin());

			//chosen_qua = &qua;
		}
		
		// reset charge

		BRAIN_CK(! qua.has_note0());
		BRAIN_CK(! (had_n3 || had_n0) || qua.in_qu_dominated(brn));

		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
		//all_rev.push(&qua);

	} // true

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);

	//BRAIN_CK(! has_in_mem);	// DBG purposes
	
	DEDUC_DBG(has_in_mem || dbg_ck_deducs(dct, dct2));
	DBG(long rr_lv = trail_level());
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == dct.dt_target_level));

	// update leveldat
	
	nke0.clear_all_quantons();
	BRAIN_CK(all_notes0 == 0);

	// learn motives

	BRAIN_CK(dct.dt_forced != NULL_PT);

	neuron* lnd_neu = learn_mots(dct.dt_motives, *dct.dt_forced);
	
	// send forced learned

	quanton* nxt_qua = dct.dt_forced;
	if(! dct.dt_motives.is_empty()){
		BRAIN_CK(nxt_qua != NULL_PT);
		send_psignal(*nxt_qua, lnd_neu, tier() + 1);
	}

	update_semi_monos();

	// inc recoil

	inc_recoil();

	DBG(
		dbg_update_config_entries(*this);
	);
	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());

	reset_conflict();
	BRAIN_CK(! found_conflict());
} // END of reverse

bj_ostream&
prop_signal::print_prop_signal(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	os << "ps={";
	os << ps_quanton;
	//if(ps_source == NULL_PT){ os << ".NULL_SRC"; }
	//else { os << "." << ((void*)ps_source) << "." << ps_source->ne_index; }
	os << "." << ps_source << ".";
	os << ps_tier;
	os << "}";
	os.flush();
	return os;
}

bj_ostream&
qulayers::print_qulayers(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	os << "qlys={";
	dk_quas_by_layer.print_row_data(os, true, "\n");
	os << "}";
	os.flush();
	return os;
}

bj_ostream&
neurolayers::print_neurolayers(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	os << "nlys={";
	nl_neus_by_layer.print_row_data(os, true, "\n");
	os << "}";
	os.flush();
	return os;
}


