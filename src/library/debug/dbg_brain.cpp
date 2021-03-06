

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

Copyright (C) 2007-2012, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

dbg_brain.cpp  

dbg_brain funcs.

--------------------------------------------------------------*/

#include "file_funcs.h"
#include "solver.h"
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
brain::dbg_prt_margin(bj_ostream& os, bool is_ck){
	ch_string f_nam = "INVALID_FILE_NAME";
//ifdef FULL_DEBUG
	instance_info& inst_info = get_my_inst();
	if(inst_info.ist_id >= 0){
		os << "#" << inst_info.ist_id << ".";
	}

	f_nam = inst_info.ist_file_path;
	
	recoil_counter_t the_rec = recoil();
	round_counter_t the_rnd = br_round;
	if(the_rnd >= 0){
		if(is_ck){ os << "LAP="; }
		os << the_rnd << "." << the_rec << ".";
	}
//endif
	return f_nam;
}

void
brain::init_all_dbg_brn(){
#ifdef FULL_DEBUG
	br_forced_srg.set_dbg_brn(this);
	br_filled_srg.set_dbg_brn(this);
	br_guide_neus_srg.set_dbg_brn(this);
	br_guide_quas_srg.set_dbg_brn(this);
	//br_compl_neus_srg.set_dbg_brn(this);
	//br_compl_quas_srg.set_dbg_brn(this);
	br_tauto_neus_srg.set_dbg_brn(this);
	br_tauto_quas_srg.set_dbg_brn(this);
	//br_clls_srg.set_dbg_brn(this);
	
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
		
		//if((qua->qu_source == NULL) && (qua->qlevel() != ROOT_LEVEL)){
		if(qua->is_lv_choice(brn)){
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

		if(	!cho && ! qua->has_source())
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

#ifdef FULL_DEBUG
void
dbg_inst_info::init_dbg_inst_info(){
	dbg_before_retract_lv = INVALID_LEVEL;
	dbg_last_recoil_lv = INVALID_LEVEL;
	
	dbg_simple_neus.clear();
	dbg_used_neus.clear();
	dbg_ccls.clear();
	dbg_cnf.clear_cnf();
	
	dbg_find_id = 0;
	dbg_save_id = 0;
	
	dbg_all_chosen.clear();
	
	dbg_clean_code = false;
	dbg_old_deduc = false;
	
	dbg_periodic_prt = true;
	
	dbg_cy_prefix = "no_file";
	dbg_cy_step = 0;
	dbg_cy_layout = CY_CIRCLE_LAYOUT;

	dbg_tot_nmps = 0;
	
	dbg_max_lv = 0;
	dbg_max_wrt_num_subnmp = 0;
	dbg_max_fnd_num_subnmp = 0;
	
	dbg_num_glb_cho = 0;
	dbg_num_loc_cho = 0;
}
#endif

void
dbg_find_not_in_rr1(brain& brn, row_neuron_t& rr1, row_neuron_t& rr2, 
					row_neuron_t& not_in_rr1){
#ifdef FULL_DEBUG
	not_in_rr1.clear();

	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	set_all_tag0(brn, rr1);
	//BRAIN_CK(brn.br_tot_ne_spots == 0);
	//set_spots_of(brn, rr1);

	for(long aa = 0; aa < rr2.size(); aa++){
		BRAIN_CK(rr2[aa] != NULL_PT);
		neuron& neu = *(rr2[aa]);

		/*if(! neu.ne_spot){
			not_in_rr1.push(&neu);
		} */
		if(! neu.has_tag0()){
			not_in_rr1.push(&neu);
		} 
	}

	//reset_spots_of(brn, rr1);
	//BRAIN_CK(brn.br_tot_ne_spots == 0);
	reset_all_tag0(brn, rr1);
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
#endif
}

void
dbg_find_diff_tauto_vs_simple_neus(brain& brn, row_neuron_t& not_in_tauto, 
								   row_neuron_t& not_in_simple)
{
#ifdef FULL_DEBUG
	row_neuron_t& dbg_simple_neus = brn.br_dbg.dbg_simple_neus;

	row_neuron_t dbg_tauto_neus;
	//srt_row_as<neuron>(brn.br_tauto_neus_srg.sg_step_sortees, dbg_tauto_neus);
	
	sort_glb& srg = brn.br_tauto_neus_srg;
	row<sortee*>& rr1 = brn.br_tauto_neus_srg.sg_step_sortees;
	row<long> cols;
	srt_row_as_colors<neuron>(srg, rr1, dbg_tauto_neus, cols, tid_sset_consec, false);

	dbg_find_not_in_rr1(brn, dbg_tauto_neus, dbg_simple_neus, not_in_tauto);
	dbg_find_not_in_rr1(brn, dbg_simple_neus, dbg_tauto_neus, not_in_simple);
#endif
}

bool
dbg_prt_diff_tauto_vs_simple_neus(bj_ostream& os, brain& brn){
#ifdef FULL_DEBUG
	row_neuron_t not_in_tauto;
	row_neuron_t not_in_simple;

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
	
	row_neuron_t& neus = br_tmp_ck_sat_neus;
	fill_with_origs(neus);

	if(! dbg_br_compute_binary(neus)){
		abort_func(1, "FATAL ERROR 001. Wrong is_sat answer !");
	}

	if(! dbg_br_compute_ck_sat_of(neus, the_assig)){
		abort_func(1, "FATAL ERROR 002. Wrong is_sat answer !");
	}

	DBG_PRT(36, os << "CHECKED_ASSIG=" << the_assig << bj_eol);

	//print_satifying(cho_nm);
#endif
}

bool
brain::dbg_br_compute_binary(row_neuron_t& neus){
#ifdef FULL_DEBUG
	long ii;
	for(ii = 0; ii < neus.size(); ii++){
		BRAIN_CK(neus[ii] != NULL_PT);
		neuron& neu = *(neus[ii]);

		if(! neu.ne_original){
			continue;
		}
		if(!(neu.is_ne_inert())){
			DBG_PRT(24, os << "FAILED compute neu=" << &(neu));
			return false;
		}
	}
#endif
	return true;
}

//for IS_SAT_CK
bool
neuron::dbg_ne_compute_ck_sat(){
#ifdef FULL_DEBUG
	for(long ii = 0; ii < fib_sz(); ii++){
		bool is_n6 = ne_fibres[ii]->is_note6();
		if(is_n6){
			return true;
		}
	}
#endif
	return false;
}
	
bool
brain::dbg_br_compute_ck_sat(row_neuron_t& neus){
#ifdef FULL_DEBUG
	long ii;
	for(ii = 0; ii < neus.size(); ii++){
		BRAIN_CK(neus[ii] != NULL_PT);
		neuron& neu = *(neus[ii]);

		if(! neu.ne_original){
			continue;
		}
		if(!(neu.dbg_ne_compute_ck_sat())){
			return false;
		}
	}
#endif
	return true;
}

//for IS_SAT_CK
bool
brain::dbg_br_compute_ck_sat_of(row_neuron_t& neus, row_quanton_t& assig){
	bool resp = true;
#ifdef FULL_DEBUG
	brain& brn = *this;
	BRAIN_CK(br_qu_tot_note6 == 0);
	set_all_binote6(brn, assig);

	bool all_ok = true;
	long ii = 0;
	while((ii < assig.size()) && (assig[ii]->qlevel() == ROOT_LEVEL)){
		bool is_n6 = assig[ii]->is_note6();
		if(! is_n6){
			all_ok = false;
			break;
		}
		ii++;
	}

	resp = all_ok && dbg_br_compute_ck_sat(neus);
	reset_all_its_note6(brn, assig);
	BRAIN_CK(br_qu_tot_note6 == 0);
	
#endif
	return resp;
}

//============================================================
// print methods

bj_ostream&
neuromap::print_all_subnmp(bj_ostream& os, bool only_pts){
#ifdef FULL_DEBUG
	print_subnmp(os, only_pts);
	if(has_submap()){
		os << " ->> ";
		na_submap->print_all_subnmp(os, only_pts);
	}
#endif
	return os;
}

bj_ostream&
neuromap::print_subnmp(bj_ostream& os, bool only_pts){
#ifdef FULL_DEBUG
	if(only_pts){
		os << dbg_na_id();
		os.flush();
		return os;
	}
	os << "_______________________________________________________\n";
	os << "NMP(" << (void*)this <<")={ \n";

	os << " na_idx=" << na_index;
	
	if(is_na_mono()){
		os << " mono";
	}
	
	os << " o_cho=" << na_orig_cho;
	os << " #sub=" << na_num_submap;
	os << " sub1=(" << (void*)na_submap << ")";
	os << " o_lv=" << na_orig_lv;
	os << " #qu=" << na_propag.size();
	os << "\n";

	os << "\n";
	
	os << " ---------------------------\n";
	
	os << "\t na_propag_quas=\n";
	os << na_propag << "\n";

	brain& brn = get_brn();
	row_neuron_t& prp_neus = brn.br_tmp_prt_nmp_neus;
	prp_neus.clear();
	append_all_trace_neus(na_propag, prp_neus);
	os << "\n";
	os << "\t na_propag_neus=\n";
	prp_neus.print_row_data(os, true, "\n");
	
	os << "\n";
	os << "\t na_all_cov=\n";
	na_all_cov.print_row_data(os, true, "\n");
	
	os << "\n";
	os << "\t na_to_write=\n";
	na_to_write.print_row_data(os, true, "\n");
	
	BRAIN_CK(brn.br_ne_tot_tag2 == 0);
	set_all_tag2(brn, na_to_write);
	
	row_neuron_t all_neu_diff;
	append_all_have_tag2(brn, na_all_cov, all_neu_diff, false);
	os << "\n";
	os << "\t diff=\n";
	all_neu_diff.print_row_data(os, true, "\n");
	
	reset_all_tag2(brn, na_to_write);
	BRAIN_CK(brn.br_ne_tot_tag2 == 0);
	
	os << "}\n";
	os.flush();
#endif
	return os;
}

bj_ostream&
neuromap::print_nmp_hd(bj_ostream& os){
#ifdef FULL_DEBUG
	bool h_cand = nmp_is_cand();
	//bool is_mono = is_na_mono();

	if(na_is_head){ os << "H."; }
	os << " " << dbg_na_id();
	if(! has_submap()){ os << "T."; }
	if(h_cand){ os << "C."; }
	//if(is_mono){ os << "M."; }
	os << "(" << (void*)this << ")";
	if(na_dbg_cand_sys){
		os << " cand_tk=" << na_candidate_tk;
	}
	os << " o_cho=" << na_orig_cho;
	os << " #sub=" << na_num_submap;
	os << " o_lv=" << na_orig_lv;
	if(h_cand){
		os << " cand=" << na_candidate_qua;
	}
	
	os << "\n all_sub=[";
	print_all_subnmp(os, true);
	os << "]\n";
#endif
	return os;
}

bj_ostream&
neuromap::print_neuromap(bj_ostream& os, bool from_pt){
#ifdef FULL_DEBUG
	if(na_brn == NULL_PT){
		bool is_vgn = is_na_virgin();
		if(is_vgn){
			os << "VIRGIN !!! ";
		}
		os << "NO BRN for nmp=" << (void*)this << " !!!!!!!!";
		os.flush();
		return os;
	}
	brain& brn = get_brn();
	MARK_USED(brn);
	MARK_USED(from_pt);
	if(from_pt){
		row<prop_signal>& all_ps = brn.br_tmp_prt_ps;
		map_get_all_propag_ps(all_ps);
		
		if(na_dbg_cand_sys){
			os << "ca{";
		} else {
			os << "na{";
		}
		print_nmp_hd(os);
		
		os << " #qu=" << all_ps.size();
		
		row_quanton_t all_f_qu;
		map_get_all_quas(all_f_qu);
		os << "\n nmp_quas=" << all_f_qu << "\n";
		
		row_neuron_t all_f_ne;
		map_get_all_neus(all_f_ne);
		os << "\n nmp_neus_idxs=[";
		for(long aa = 0; aa < all_f_ne.size(); aa++){
			neuron* neu = all_f_ne[aa];
			BRAIN_CK(neu != NULL_PT);
			os << neu->ne_index << ".";
		}
		os << "]\n";
		
		os << "}";
		os.flush();
		return os;
	}
	
	os << "######################################################\n";
	os << "NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP_NMP\n";
	os << "######################################################\n";
	
	print_nmp_hd(os);
	
	print_all_subnmp(os);
	
	os << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	os << "END_OF_NMP_END_OF_NMP_END_OF_NMP_END_OF_NMP_END_OF_NMP_\n";
	os << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	
#endif
	return os;
}
	
void
brain::print_trail(bj_ostream& os, bool no_src_only){
//ifdef FULL_DEBUG
	row_quanton_t& the_trl = br_tmp_trail;
	br_charge_trail.get_all_ordered_quantons(the_trl);
	os << "TRAIL=[";
	for(long kk = 0; kk < the_trl.size(); kk++){
		quanton* qua = the_trl[kk];
		if(no_src_only && qua->has_source()){
			continue;
		}

		qua->print_quanton(os, true);
		
		//os << qua->qu_source;
		os << " ";
		//os << bj_eol;
	}
	os << "]";
	os << bj_eol;
	os.flush();
//endif
}

bj_ostream& 
brain::print_all_quantons(bj_ostream& os, long ln_sz, ch_string ln_fd){
#ifdef FULL_DEBUG
	BRAIN_CK_0(br_choices.size() == br_positons.size());
	long num_no_src = 0;
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
		
		if(	! qua->has_source() && 
			(qua->qlevel() != 0) &&
			(chg != cg_neutral)
		){
			num_no_src++;
			os << "*";
		} else {
			os << " ";
		}
		os << pre << qua << suf << " ";

	}

	BRAIN_CK_0(num_no_src == level());
	os.flush();
#endif
	return os;
}

bj_ostream&
quanton::print_quanton(bj_ostream& os, bool from_pt){
	neuron* src = get_source();
	return print_quanton_base(os, from_pt, qu_tier, src, false);
}

bj_ostream&
neuron::print_neuron(bj_ostream& os, bool from_pt){
	return print_neu_base(os, from_pt, false, false);
}

bj_ostream&	
neuron::print_tees(bj_ostream& os){
	os << " tees[";
	for(long ii = 0; ii < fib_sz(); ii++){
		quanton* qua = ne_fibres[ii];
		BRAIN_CK(qua != NULL_PT);
		if(qua->qu_tee.is_unsorted()){
			continue;
		}
		
		neuron* src = qua->get_source();
		qua->print_quanton_base(os, true, qua->qu_tier, src, true);
		os << " ";
	}
	os << "]";
	os.flush();
	return os;
}

bj_ostream&	
neuron::print_neu_base(bj_ostream& os, bool from_pt, bool from_tee, bool sort_fib){
#ifdef FULL_DEBUG
	brain* pt_brn = get_dbg_brn();
	MARK_USED(pt_brn);
	
	bool tg0 = has_tag0();
	bool tg1 = has_tag1();
	bool tg2 = has_tag2();
	bool tg3 = has_tag3();
	bool tg4 = has_tag4();
	bool tg5 = has_tag5();

	if(from_pt){
		os << "ne={";
		//os << ((void*)(this));
		os << " ne_idx='" << ne_index << "' ";
		if(ne_original){
			os << "o";
		} else {
			os << "+";
		}
		if(tg0){ os << ".g0"; }
		if(tg1){ os << ".g1"; }
		if(tg2){ os << ".g2"; }
		if(tg3){ os << ".g3"; }
		if(tg4){ os << ".g4"; }
		if(tg5){ os << ".g5"; }
		
		if(! from_tee){
			os << ne_fibres;
		} else {
			print_tees(os);
		}
		//os << " w_tk=" << ne_to_wrt_tk;
		os << " pf_tk=" << ne_proof_tk.get_str();
		
		bool in_abort_nmp = false;
		if((pt_brn != NULL) && (pt_brn->br_dbg_abort_nmp != NULL_PT)){
			ticket& nmp_tk = pt_brn->br_dbg_abort_nmp->na_candidate_tk;
			in_abort_nmp = ! ne_to_wrt_tk.is_older_than(nmp_tk);
		}
		if(in_abort_nmp){
			os << ".in";
		}
		
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
	row_neuron_t& neus = br_tmp_prt_neus;
	fill_with_origs(neus);
	neus.print_row_data(os, false, "\n", -1, -1, true);
#endif
	return os;
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
brain::dbg_prt_lvs_cho(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "chos=[";
	
	row<leveldat*>& all_lv = br_data_levels;
	for(int aa = 0; aa < all_lv.size(); aa++){
		leveldat& lv = *(all_lv[aa]);
		os << lv.ld_chosen;
		os << ",";
	}
	os << "]";
#endif
}

/*void
dbg_print_neus_idxs(bj_ostream& os, row_neuron_t& all_neus){
#ifdef FULL_DEBUG
	for(long aa = 0; aa < all_neus.size(); aa++){
		
	}
#endif
}*/

bool
dbg_run_satex_on(brain& brn, ch_string f_nam, neuromap* dbg_nmp){
#ifdef FULL_DEBUG
	bool is_no = dbg_run_satex_is_no_sat(f_nam);
	bool has_nmp = (dbg_nmp != NULL_PT);
	bool is_min_wrt = (has_nmp && (dbg_nmp->na_upd_to_write));
	
	MARK_USED(is_no);
	DBG_COMM_WITH(70, brn, 
		if(! is_no){
			ch_string msg_htm = "ABORTING_WITH";
			if(has_nmp){
				msg_htm = dbg_nmp->map_dbg_html_data_str(msg_htm);
			}
			
			if(has_nmp){
				dbg_nmp->map_dbg_set_cy_maps();
			}
			brn.dbg_update_html_cy_graph(CY_NMP_KIND, 
						&(brn.br_tmp_ini_tauto_col), msg_htm);
			if(has_nmp){
				dbg_nmp->map_dbg_reset_cy_maps();
			}
		}
	);
	if(! is_no){
		bj_ostream& os = bj_dbg;
		
		brn.br_dbg_abort_nmp = dbg_nmp;
		
		os << "\n=========================================================\n";
		os << "\n=========================================================\n";
		os << "\n=========================================================\n";
		os << "ABORTING_DATA ";
		if(is_min_wrt){
			os << "(MINIMAL_WRITE)";
		}
		os << "\n";
		
		ch_string o_ff = brn.dbg_prt_margin(os);
		
		brn.print_trail(os);
		os << "\n";
		
		brn.dbg_prt_all_cands(os, true);
		brn.dbg_prt_all_nxt_cands(os, true);
		if(has_nmp){
			os << " nmp=\n" << *dbg_nmp << "\n\n";
			os << " na_idx=" << dbg_nmp->na_index;
		}
		os << " brn_tk=" << brn.br_curr_choice_tk << bj_eol;
		os << " brn_deduc=" << brn.br_pulse_deduc << bj_eol;
		os << "	LV=" << brn.level() << bj_eol;
		os << " CNF_FILE=\n" << f_nam << bj_eol;
		os << " \nduring ff=\n" << o_ff << bj_eol;
		
		if(is_min_wrt){
			os << "(MINIMAL_WRITE)";
			os << "END_OF_minimal_write_abort_data";
		}
		os << "\n";
		
		os << bj_eol;
		os.flush();

		if(is_min_wrt){
			BRAIN_CK(! dbg_nmp->na_dbg_is_no_abort_full_nmp);
			dbg_nmp->na_dbg_is_no_abort_full_nmp = true;
			
			dbg_nmp->map_write(true);
			
			ch_string o_ff2 = brn.dbg_prt_margin(os);
			os << "CNF_FULL_FILE=\n" << dbg_nmp->na_dbg_is_no_abort_full_wrt_pth << "\n";
			os << "CNF_DURING_MIN_FILE=\n" << f_nam << "\n";
			os << " \nduring ff=\n" << o_ff2 << "\n";
			os << "FULL_WRITE_OK !!!!\n\n";
		}
		
		if(dbg_nmp != NULL_PT){
			os << "NMP_ID=" << dbg_nmp->dbg_na_id() << "\n";
		}
		os << "\n\nEND_OF_aborting_data";
	}
	
	
	
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
brain::dbg_in_edge_of_target_lv(reason& rsn){
	if(rsn.is_rs_virgin()){ return false; }
	long trl_lv = trail_level();
	//bool in_tg_lv = (trl_lv < rsn.rs_target_level);
	bool in_tg_lv = (trl_lv <= rsn.rs_target_level);
	BRAIN_CK(! in_tg_lv || ((trl_lv + 1) == level()));
	return in_tg_lv;
}

void
reason::dbg_set_with(brain& brn, notekeeper& nke, quanton& nxt_qua){
#ifdef FULL_DEBUG
	if(! is_rs_virgin()){
		return;
	}

	quanton& opp_nxt = *(nxt_qua.qu_inverse);

	BRAIN_CK(opp_nxt.qlevel() == nke.dk_note_layer);
	BRAIN_CK(opp_nxt.get_charge() == cg_negative);

	nke.dk_quas_lyrs.get_all_ordered_quantons(rs_motives);

	DBG_PRT(51, os << " motives_by_lv= " << nke.dk_quas_lyrs.ql_quas_by_layer);
	DBG_PRT(52, os << "LV=" <<  nke.dk_note_layer << " motives " 
		<< rs_motives << " opp_nxt=" << &opp_nxt);

	rs_target_level = find_max_level(rs_motives);

	rs_forced = &opp_nxt;
	//rs_forced_level = opp_nxt.qlevel();

	BRAIN_CK(rs_target_level < nke.dk_note_layer);
	BRAIN_CK(ck_motives(brn, rs_motives));
	BRAIN_CK(! is_rs_virgin());
#endif
}

void
neuron::dbg_old_set_motives(brain& brn, notekeeper& nke, bool is_first){
#ifdef FULL_DEBUG
	neuron& neu = *this;

	BRAIN_CK(! ne_fibres.is_empty());
	BRAIN_CK(is_first || (ne_fibres[0]->get_charge() == cg_positive) );
	BRAIN_CK(is_first || is_ne_inert());

	row_quanton_t& causes = ne_fibres;
	BRAIN_CK(! causes.is_empty());

	long from = (is_first)?(0):(1);
	long until = causes.size();
	nke.set_motive_notes(neu.ne_fibres, from, until);
#endif
}

bool
brain::dbg_ck_rsns(reason& rsn1, reason& rsn2){
#ifdef FULL_DEBUG
	//BRAIN_CK(false);
	long lv1 = rsn1.rs_target_level;
	long lv2 = rsn2.rs_target_level;

	if(lv1 == INVALID_LEVEL){ lv1 = ROOT_LEVEL; }
	if(lv2 == INVALID_LEVEL){ lv2 = ROOT_LEVEL; }

	bool c1 = (rsn1.rs_motives.equal_to(rsn2.rs_motives));
	bool c2 = (rsn1.rs_forced == rsn2.rs_forced);
	bool c3 = (lv1 == lv2);

	MARK_USED(c1);
	MARK_USED(c2);
	MARK_USED(c3);

	DBG_COND_COMM(! (c1 && c2 && c3) ,
		os << "ABORTING_DATA " << bj_eol;
		os << "  c1=" << c1 << "  c2=" << c2 << "  c3=" << c3 << bj_eol;
		os << "rsn1=" << rsn1 << bj_eol;
		os << "rsn2=" << rsn2 << bj_eol;
		print_trail(os);
		os << "END_OF_aborting_data" << bj_eol;
	);

	BRAIN_CK(c1);
	BRAIN_CK(c2);

	BRAIN_CK(c3);
#endif
	return true;
}

void
brain::dbg_old_reverse_trail(){
#ifdef FULL_DEBUG
	BRAIN_CK(br_dbg.dbg_old_deduc);
	if(in_root_lv()){
		set_result(bjr_no_satisf);
		return;
	}
	
	reason& rsn1 = br_dbg.dbg_rsn1;
	reason& rsn2 = br_dbg.dbg_dct.dt_rsn;
	//long dbg_old_lv = level();

	BRAIN_CK(! has_psignals());
	brain& brn = *this;
	notekeeper& nke0 = br_dbg_retract_nke0;
	reason& rsn = rsn1;
	long& all_notes0 = br_qu_tot_note0;
	//row_quanton_t& all_rev = br_tmp_rever_quas;
	//MARK_USED(all_rev);
	MARK_USED(all_notes0);

	rsn.init_reason();
	nke0.init_notes(level());

	BRAIN_CK(rsn.is_rs_virgin());
	BRAIN_CK(nke0.dk_tot_noted == 0);
	BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(all_notes0 == 0);
	//BRAIN_CK(all_rev.is_empty());

	// START REVERSE (init nke0)

	BRAIN_CK(found_conflict());
	
	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());

	DBG(
		br_wrt_ref.reset_curr_quanton();
		write_get_tk(br_curr_write_tk);
		
		br_dedcer.set_conflicts(br_all_conflicts_found);
		row<neuromap*>& to_wrt = br_dbg.dbg_dct.dt_all_to_wrt;
		to_wrt.clear();
		br_dedcer.get_first_causes(br_dbg.dbg_dct);
		br_dedcer.deduce(br_dbg.dbg_dct);
	);
	BRAIN_CK_PRT(rsn2.rs_target_level >= ROOT_LEVEL, 
		os << recoil() << ".rsn2=" << rsn2
	);

	neuron& cfl = *(first_conflict().ps_source);
	cfl.dbg_old_set_motives(brn, nke0, true); // init nke0 with confl

	BRAIN_CK(all_notes0 > 0);

	// REVERSE LOOP

	while(true){
		bool end_of_recoil = dbg_in_edge_of_target_lv(rsn);
		if(end_of_recoil){
			BRAIN_CK(! rsn.is_rs_virgin());
			BRAIN_CK((trail_level() + 1) == level());
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
		BRAIN_CK(rsn.is_rs_virgin() || (level() >= rsn.rs_target_level));

		// notes

		//DBG(bool had_n3 = false);
		//DBG(bool had_n0 = false);

		if(qua.has_note0()){
			long qlv = qua.qlevel();
			nke0.update_notes_layer(qlv);
			BRAIN_CK(nke0.dk_num_noted_in_layer > 0);

			qua.reset_its_note0(brn);
			
			BRAIN_CK(qua.qlevel() == nke0.dk_note_layer);

			nke0.dec_notes();

			if(nke0.dk_num_noted_in_layer == 0){
				if(rsn.is_rs_virgin()){
					rsn.dbg_set_with(brn, nke0, qua);
				}

				BRAIN_CK(! rsn.is_rs_virgin());
			}

			if(src != NULL_PT){
				src->dbg_old_set_motives(brn, nke0, false);
				BRAIN_CK(nke0.dk_num_noted_in_layer > 0);
			} else {
				BRAIN_CK(! qua.has_source());
				BRAIN_CK(! rsn.is_rs_virgin());
				BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			}

		} // qua.has_note0()

		// chosen

		if(! qua.has_source()){
			BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			BRAIN_CK(! rsn.is_rs_virgin());

			//chosen_qua = &qua;
		}
		
		// reset charge

		BRAIN_CK(! qua.has_note0());

		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);
		//all_rev.push(&qua);

	} // true

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = rsn.rs_target_level);

	BRAIN_CK(dbg_ck_rsns(rsn, rsn2));
	DBG(long rr_lv = trail_level());
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == rsn.rs_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == rsn.rs_target_level));

	// update leveldat
	
	nke0.clear_all_quantons();
	BRAIN_CK(all_notes0 == 0);

	// learn motives

	BRAIN_CK(rsn.rs_forced != NULL_PT);

	neuron* lnd_neu = learn_mots(rsn);
	
	// send forced learned

	quanton* nxt_qua = rsn.rs_forced;
	if(! rsn.rs_motives.is_empty()){
		BRAIN_CK(nxt_qua != NULL_PT);
		send_psignal(*nxt_qua, lnd_neu, tier() + 1);
	}

	// inc recoil

	inc_recoil();

	DBG(
		dbg_update_config_entries(get_solver().slv_dbg_conf_info, recoil());
	);
	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());

	reset_conflict();
	BRAIN_CK(! found_conflict());
#endif
} // END of dbg_old_reverse

bj_ostream&
prop_signal::print_prop_signal(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	if(from_pt){
		os << " ps={";
		os << ps_quanton;
		os << "_";
		os << ps_tier;
		os << "}";
		os.flush();
		return os;
	}
	
	/*if(ps_quanton != NULL_PT){
		return ps_quanton->print_quanton_base(os, true, ps_tier, ps_source, false);
	}*/
	
	os << " ps{";
	os << ps_quanton;
	os << "." << ps_tier;
	//if(ps_source == NULL_PT){ os << ".NULL_SRC"; }
	//else { os << "." << ((void*)ps_source) << "." << ps_source->ne_index; }
	os << ".\n" << ps_source << "\n";
	os << "}";
	os.flush();
	return os;
}

bj_ostream&
qulayers::print_qulayers(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	os << "qlys={";
	ql_quas_by_layer.print_row_data(os, true, "\n");
	os << "}";
	os.flush();
	return os;
}

bj_ostream&
coloring::print_coloring(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);
	if(from_pt){
		dbg_print_qua_ids(os);
		return os;
	}
	
	os << "CO(" << (void*)this <<")={ " << bj_eol;
	os << " brn=" << (void*)co_brn;
	os << " qu_sz=" << co_quas.size();
	os << " quas=" << co_quas << bj_eol;
	os << " cols_quas=" << co_qua_colors << bj_eol;
	os << " ne_sz=" << co_neus.size();
	os << " neus=" << bj_eol;
	co_neus.print_row_data(os, true, "\n");
	os << " cols_neus=" << co_neu_colors << bj_eol;
	os << " all_qu_consc=" << co_all_qua_consec;
	os << " all_ne_consc=" << co_all_neu_consec;
	os << bj_eol;
	os << "}";
	os.flush();
	return os;
}

bj_ostream&	
leveldat::print_leveldat(bj_ostream& os, bool from_pt){
	if(from_pt){
		os << "[LV=" << ld_idx << ".cho=" << ld_chosen << "]";
		return os;
	}
	os << "LVDAT(" << (void*)this <<")={" << bj_eol;
	os << " ld_idx=" << ld_idx << bj_eol;
	os << " ld_chosen=" << ld_chosen << bj_eol;
	os << "}";
	os.flush();
	return os;
}

bj_ostream&	
deducer::print_deducer(bj_ostream& os, bool from_pt){
	row_quanton_t all_quas;
	append_all_trace_quas(de_all_noted, all_quas);
	
	os << "an{";
	os << all_quas;
	os << "}";
	
	os.flush();
	return os;
}

void
print_all_in_grip(bj_ostream& os, binder& grp){
	binder* fst_bdr = grp.bn_right;
	binder* lst_bdr = &(grp);
	for(binder* bdr_1 = fst_bdr; bdr_1 != lst_bdr; bdr_1 = bdr_1->bn_right){
		neuromap& nmp = rcp_as<neuromap>(bdr_1);
		os << "(" << ((void*)&nmp) << ")";
	}
}

bj_ostream&
quanton::print_quanton_base(bj_ostream& os, bool from_pt, long ps_ti, neuron* ps_src, 
							bool from_tee)
{
//ifdef FULL_DEBUG
	MARK_USED(from_pt);

	brain* pt_brn = get_dbg_brn();

	if((pt_brn != NULL_PT) && (this == &(pt_brn->br_top_block))){
		os << "TOP_BLOCK_QUA";
		os.flush();
		return os;
	}
	if(from_tee){
		os << qu_id; 
		os << ".ti." << ps_ti; 
		os << "|" << qu_tee.so_qua_id;
		os.flush();
		return os;
	}

	bool is_nega = is_neg();
	bool is_posi = is_pos();
	neuron* neu = ps_src;
	bool n0 = has_note0();
	MARK_USED(n0);
	bool n1 = has_note1();
	MARK_USED(n1);
	bool n2 = has_note2();
	MARK_USED(n2);
	bool n3 = has_note3();
	MARK_USED(n3);
	bool n4 = has_note4();
	MARK_USED(n4);
	bool h_src = (neu != NULL_PT);
	bool h_chg = has_charge();
	long qlv = qlevel();
	long qti = ps_ti;
	
	bool h_cand = (qu_candidate_nmp != NULL_PT);

	bool is_uron = is_qu_uniron();
	
	quanton& opp = opposite();
	
	bool is_mn = (qu_lv_mono != INVALID_LEVEL);
	bool is_opp_mn = (opp.qu_lv_mono != INVALID_LEVEL);
	bool hm = (is_mn || is_opp_mn);
	//bool hm = false;
	MARK_USED(hm);
	
	bool is_end_nmp = false;
	
	if(pt_brn != NULL_PT){
		//is_end_nmp = is_qu_end_of_neuromap(*pt_brn);
	}

	/*if(from_pt){
		if(neu != NULL_PT){
			if(neu->ne_original){ os << "o"; }
			else { os << "+"; }
			os << neu->ne_index;
		}
		if(! h_src && h_chg){ os << "L"; }
		os << "(" << qu_id; 
		os << ".t" << qti;
		if(is_mn){ os << ".M." << qu_lv_mono; }
		if(is_opp_mn){ os << ".m." << opp.qu_lv_mono; }
		os.flush();
		return os;
	}*/

	if(from_pt){
		if(qlv == 0){ os << "#"; }
		if(is_end_nmp){ os << "E"; }
		if(! h_src && h_chg){ os << "L" << qlv; }
		if(is_uron){
			os << "U"; 
		}
		if(h_cand){ 
			os << "C"; 
			os << qu_candidate_nmp->na_index;
		}
		//if(qu_block != NULL_PT){ os << "b"; }
		if(neu != NULL_PT){
			if(neu->ne_original){ os << "o"; }
			else { os << "+"; }
			os << neu->ne_index;
		}
		//if(! has_source() && has_charge()){ os << "*"; }
		/*if(! h_src && h_chg && (pt_brn != NULL_PT)){ 
			brain& brn = *pt_brn;
			
			leveldat& lv = brn.get_leveldat(qlv);
			bool is_qu = (this == lv.ld_chosen) || (qu_tier == 1);
			
			if(is_qu && lv.has_learned()){ 
				os << ".l"; 
				if(lv.ld_learned.size() > 1){
					os << lv.ld_learned.size(); 
				}
			}
			if(is_qu && (qlv != ROOT_LEVEL) && (lv.ld_chosen == NULL_PT)){
				os << "[NULL_CHO!!!]"; 
			}
		}*/

		if(! h_chg){ os << "("; }
		if(is_nega){ os << '\\';  }
		if(is_posi){ os << '/';  }
		os << qu_id; 
		if(is_posi){ os << '\\';  }
		if(is_nega){ os << '/';  }
		if(! h_chg){ os << ")"; }

		os << ".t" << qti;
		
		//if(n0){ os << ".n0"; }
		if(n1){ os << ".n1"; }
		if(n2){ os << ".n2"; }
		if(n3){ os << ".n3"; }
		if(n4){ os << ".n4"; }
		if(is_choice()){
			if(is_mn){ os << ".M." << qu_lv_mono; }
			if(is_opp_mn){ os << ".m." << opp.qu_lv_mono; }
		}
		/*
		if(! qu_tee.is_unsorted()){ os << ".q" << qu_tee.so_qua_id; }
		if(n1){ os << ".n1"; }
		*/

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
	//os << "st_" << qu_dbg_choice_idx << " ";
	os << "ps_src ";
		os << neu;
	os << "current_src ";
	if(has_source()){
		os << get_source();
	} else {
		os << "NULL";
	}
	os << "\n";
	os << "ne_tnn_" << qu_tunnels << "\n";
	os << "\n";
	os.flush();
//endif
	return os;
}

void
dbg_run_diff(ch_string fnm1, ch_string fnm2, ch_string dff_fnm){
#ifdef FULL_DEBUG
	if(! file_exists(fnm1)){
		return;
	}
	if(! file_exists(fnm2)){
		return;
	}

	ch_string o_str = "diff " + fnm1 + " " + fnm2 + " > " + dff_fnm;
	system_exec(o_str);
#endif
}

void
neuromap::map_dbg_print(bj_ostream& os, mem_op_t mm){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	//os << STACK_STR << bj_eol;
	os << "DBG_PRT=" << bj_eol;
	os << this << bj_eol;
	os << "brn_tk=" << brn.br_curr_choice_tk << bj_eol;
	if(mm == mo_save){ os << "SAVE "; }
	if(mm == mo_find){ os << "FIND "; }

	//os << "CERO FILLED___________________________________________ " << bj_eol;
	sort_glb& tauto_srg = brn.br_tauto_neus_srg;
	os << " sg_dbg_cnf_tot_onelit=" << tauto_srg.sg_dbg_cnf_tot_onelit << bj_eol;

	os << " TAUTO_CNF=" << bj_eol;
	os << tmp_tauto_cnf << bj_eol;
	os << " DIFF_CNF=" << bj_eol;
	os << tmp_diff_cnf << bj_eol;
	os << " GUIDE_CNF=" << bj_eol;
	os << tmp_guide_cnf << bj_eol;

	os << bj_eol;
	BRAIN_DBG(os << " RECOIL_LV=" << brn.br_dbg.dbg_last_recoil_lv);

	os << bj_eol;
	os << brn.get_my_inst().get_f_nam() << bj_eol;
	os << "=========================================================" << bj_eol;
#endif
}

bool
neuromap::map_ck_contained_in(coloring& colr, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	
	// CHECK QUAS
	
	row_quanton_t& m_quas = brn.br_tmp_ck_col;
	map_get_all_quas(m_quas);
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	set_all_note1(brn, m_quas);
	
	row_quanton_t& all_quas = colr.co_quas;
	for(int ii = 0; ii < all_quas.size(); ii++){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		BRAIN_CK_PRT(all_quas[ii]->has_note1(), 
			DBG_PRT_ABORT(brn);
			os << *this << "\n++++++++++++++++++++\n\n";
			os << " qua=" << all_quas[ii] << "\n";
			os << " dbg_id=" << dbg_id << "\n";
		);
	}
	
	reset_all_note1(brn, m_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	// CHECK NEUS
	
	row_neuron_t& m_neus = brn.br_tmp_all_neus;
	map_get_all_neus(m_neus);
	
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	set_all_tag0(brn, m_neus);
	//BRAIN_CK(brn.br_tot_ne_spots == 0);
	//set_spots_of(brn, m_neus);
	
	row_neuron_t& all_neus = colr.co_neus;
	for(long ii = 0; ii < all_neus.size(); ii++){
		BRAIN_CK(all_neus[ii] != NULL_PT);
		neuron& neu = *(all_neus[ii]);
		//BRAIN_CK_PRT(neu.ne_spot, os << "_______\n neu=" << &neu);
		BRAIN_CK_PRT(neu.has_tag0(), os << "_______\n neu=" << &neu);
	}

	//reset_spots_of(brn, m_neus);
	//BRAIN_CK(brn.br_tot_ne_spots == 0);
	reset_all_tag0(brn, m_neus);
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
#endif
	return true;
}

void
brain::dbg_prt_br_neuromaps(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "ALL_NMPS=\n";
	for(long aa = 0; aa < br_neuromaps.size(); aa++){
		neuromap& nmp = br_neuromaps[aa];
		os << &nmp << "\n";
	}
	os << "\n";
#endif
}

void
brain::dbg_prt_all_nmp(bj_ostream& os, row_neuromap_t& all_cand, bool just_ids){
#ifdef FULL_DEBUG
	os << "[[\n";
	for(long aa = 0; aa < all_cand.size(); aa++){
		BRAIN_CK(all_cand[aa] != NULL_PT);
		neuromap& nmp = *(all_cand[aa]);
		if(just_ids){
			os << "(" << nmp.dbg_na_id();
			os << ".q_cand=" << nmp.na_candidate_qua;
			os << ".cand_tk=" << nmp.na_candidate_tk;
			os << ") \n";
		} else {
			os << &nmp << "\n";
		}
	}
	os << "\n]]";
	os << "\n";
#endif
}

void
brain::dbg_prt_all_cands(bj_ostream& os, bool just_ids){
#ifdef FULL_DEBUG
	os << "ALL_CANDS=\n";
	dbg_prt_all_nmp(os, br_candidate_nmp_lvs, just_ids);
#endif
}

void
brain::dbg_prt_all_nxt_cands(bj_ostream& os, bool just_ids){
#ifdef FULL_DEBUG
	os << "ALL_NXT_CANDS=\n";
	dbg_prt_all_nmp(os, br_candidate_nxt_nmp_lvs, just_ids);
#endif
}

comparison 
dbg_cmp_ps(prop_signal const & ps1, prop_signal const & ps2){
comparison vv = 0;
#ifdef FULL_DEBUG
	//vv = cmp_long(ps1.ps_tier, ps2.ps_tier);
	vv = cmp_long(ps2.ps_tier, ps1.ps_tier);
#endif
return vv;
}

bool
neuromap::map_dbg_ck_ord(row<prop_signal>& all_ps){
#ifdef FULL_DEBUG
	BRAIN_CK(all_ps.is_sorted(dbg_cmp_ps));
#endif
	return true;
}

ch_string
neuromap::dbg_na_id(){
	ch_string i_s = "INVALID_NA_ID";
//ifdef FULL_DEBUG
	bool is_mono = is_na_mono();
	bj_ostr_stream os;
	os << "{";
	if(is_mono){ os << "M."; }
	os << "na_idx=" << na_index << ".tk=" << na_candidate_tk;
	if(na_orig_cho != NULL_PT){
		os << ".cho=" << na_orig_cho->qu_id;
	} else {
		os << ".cho=null";
	}
	os << "}";
	
	i_s = os.str();
//endif
	return i_s;
}

ch_string
neuromap::map_dbg_get_phi_ids_str(){
	ch_string str_id = "";
#ifdef FULL_DEBUG
	bj_ostr_stream ss_msg;
	ss_msg << na_dbg_phi_id;
	
	str_id = ss_msg.str();
#endif
	return str_id;
}

bool
neuron::ck_all_neg(long from, bool ck_tunn_ord){
	bool all_ok = true;
#ifdef FULL_DEBUG
	for(long ii = from; ii < fib_sz(); ii++){
		all_ok = (all_ok && (ne_fibres[ii]->is_neg()));
	}
#endif
	return all_ok;
}

bool
neuron::ck_all_has_charge(long max_lv_idx){
	bool all_ok = true;
#ifdef FULL_DEBUG
	long max_lv = INVALID_LEVEL;
	bool ck_tunn_ord = (max_lv_idx != INVALID_IDX);
	if(ck_tunn_ord){
		BRAIN_CK(ne_fibres.is_valid_idx(max_lv_idx));
		BRAIN_CK(ne_fibres[max_lv_idx] != NULL_PT);
		max_lv = ne_fibres[max_lv_idx]->qlevel();
	}
	long npos = 0;
	for(long ii = 0; ii < fib_sz(); ii++){
		quanton& qua = *(ne_fibres[ii]);
		all_ok = (all_ok && qua.has_charge());
		all_ok = (all_ok && qua.has_tier());
		if(qua.is_pos()){ npos++; }
		if(ck_tunn_ord){
			BRAIN_CK(max_lv >= qua.qlevel());
		}
	}
#endif
	return all_ok;
}

long
neuromap::dbg_get_depth_in(neuromap& hd_nmp){
	neuromap* nxt_nmp = &hd_nmp;
	long dpth = 0;
	while(nxt_nmp != NULL_PT){
		dpth++;
		if(nxt_nmp == this){
			return dpth;
		}
		nxt_nmp = nxt_nmp->na_submap;
	}
	return 0;
}

bj_ostream&
neuromap::print_cand_id(bj_ostream& os){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	row_neuromap_t& all_cand = brn.br_candidate_nmp_lvs;
	os << "[";
	for(long aa = 0; aa < all_cand.size(); aa++){
		BRAIN_CK(all_cand[aa] != NULL_PT);
		neuromap& nmp = *(all_cand[aa]);
		long dpth = dbg_get_depth_in(nmp);
		os << dpth;
		if(dpth > 9){
			os << ".";
		}
	}
	os << "]";
#endif
	return os;
}

void
neuron::dbg_get_charges(row_long_t& chgs){
#ifdef FULL_DEBUG
	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);
		if(qua.is_pos()){
			chgs.push(1);
		}
		if(qua.is_neg()){
			chgs.push(0);
		}
		if(qua.is_nil()){
			chgs.push(3);
		}
		//chgs.push(qua.qlevel());
	}
#endif
}

