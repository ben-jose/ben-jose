

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
neuron::map_set_dbg_ccl(mem_op_t mm, brain& brn){
#ifdef FULL_DEBUG
	ne_dbg_ccl.cc_clear(false);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);

		if(qua.has_mark()){ 
			ne_dbg_ccl.cc_push(qua.qu_id);
		}
	}

	ne_dbg_ccl.cc_mix_sort(cmp_canon_ids);
#endif
}

void
neuron::add_dbg_ccl(brain& brn, row<canon_clause*>& the_ccls, 
					row<neuron*>& the_neus, dima_dims& dims)
{
#ifdef FULL_DEBUG
	canon_clause& ccl = ne_dbg_ccl;
	if(! ne_spot){
		set_spot(brn);
		the_ccls.push(&ccl);
		the_neus.push(this);

		if(ccl.cc_size() == 2){ dims.dd_tot_twolits++; }
		dims.dd_tot_lits += ccl.cc_size();
	}
#endif
}

void
memap::map_set_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls, 
					   row<neuron*>& the_neus, dima_dims& dims)
{
#ifdef FULL_DEBUG
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

	neuron* cfl_neu = ma_confl.ps_source;
	BRAIN_CK(cfl_neu != NULL_PT);
	cfl_neu->map_set_dbg_ccl(mm, brn);
	cfl_neu->add_dbg_ccl(brn, the_ccls, the_neus, dims);
	
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
#endif
}

bool
memap::map_ck_simple_no_satisf(mem_op_t mm, brain& brn){
#ifdef FULL_DEBUG
	//long tot_vars = brn.br_positons.size();
	//long tot_lits = 0;
	//long tot_twolits = 0;
	row<neuron*>& dbg_neus = brn.br_dbg.dbg_simple_neus;
	row<canon_clause*>& dbg_ccls = brn.br_dbg.dbg_ccls;
	skeleton_glb& skg = brn.get_skeleton();
	canon_cnf& dbg_cnf = brn.br_dbg.dbg_cnf;

	dima_dims dbg_dims;

	map_set_dbg_cnf(mm, brn, dbg_ccls, dbg_neus, dbg_dims);
	//dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, 
	//		INVALID_NATURAL, INVALID_NATURAL, false);
	dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, INVALID_NATURAL, INVALID_NATURAL);

	BRAIN_CK(dbg_cnf.cf_dims.dd_tot_ccls == dbg_cnf.cf_clauses.size());

	//DBG_PRT(106, os << "MAP NEURONS=" << bj_eol; 
	//		dbg_print_ccls_neus(os, dbg_cnf.cf_clauses));
	//DBG_PRT(106, os << "tot_ccls=" << dbg_cnf.cf_dims.dd_tot_ccls);

	ch_string dbg_cnf_nm = skg.kg_root_path + "/DBG_CNF_CK_SAVE.yos";
	write_file(dbg_cnf_nm, dbg_cnf.cf_chars, false);

	BRAIN_CK(dbg_cnf.cf_dims.dd_tot_ccls == dbg_cnf.cf_clauses.size());

	DBG_CHECK_SAVED(dbg_run_satex_on(brn, dbg_cnf_nm));

	//DBG_PRT(106, 
	//	os << "dbg_cnf_nm=" << dbg_cnf_nm << bj_eol;
		//map_dbg_prt(os, mm, brn);
	//);

	dbg_cnf.clear_cnf();
#endif
	return true;
}

void
dbg_prepare_used_dbg_ccl(row_quanton_t& rr_qua, canon_clause& dbg_ccl){
#ifdef FULL_DEBUG
	dbg_ccl.cc_clear(false);


	for(long aa = 0; aa < rr_qua.size(); aa++){
		BRAIN_CK(rr_qua[aa] != NULL_PT);
		quanton& qua = *(rr_qua[aa]);

		if(qua.has_mark()){ 
			dbg_ccl.cc_push(qua.qu_id);
		}
	}
#endif
}

void
memap::dbg_prepare_used_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls){
#ifdef FULL_DEBUG
	long trace_sz = get_trace_sz(mm);
	long filled_sz = get_filled_sz(mm);
	MARK_USED(filled_sz);

	row<prop_signal>& trace = ma_dotted;
	BRAIN_CK((trace_sz == trace.size()) || trace.is_valid_idx(trace_sz));

	BRAIN_CK(brn.br_tot_ne_spots == 0);
	BRAIN_CK(brn.br_tot_qu_marks == 0);
	set_marks_of(brn, trace, 0, trace_sz);

	the_ccls.clear();

	row<neuron*>& all_orig = brn.br_dbg.dbg_original_used;

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
#endif
}

bool
memap::dbg_ck_used_simple_no_satisf(mem_op_t mm, brain& brn){
#ifdef FULL_DEBUG
	row<canon_clause*>& dbg_ccls = brn.br_dbg.dbg_ccls;
	skeleton_glb& skg = brn.get_skeleton();
	canon_cnf& dbg_cnf = brn.br_dbg.dbg_cnf;

	dbg_prepare_used_dbg_cnf(mm, brn, dbg_ccls);

	if(dbg_ccls.is_empty()){
		return true;
	}

	dbg_cnf.init_with(skg, dbg_ccls, INVALID_NATURAL, INVALID_NATURAL, 
					  INVALID_NATURAL, false);

	ch_string dbg_cnf_nm = skg.kg_root_path + "/DBG_CNF_CK_USED.yos";
	write_file(dbg_cnf_nm, dbg_cnf.cf_chars, false);

	DBG_CHECK_SAVED(dbg_run_satex_on(brn, dbg_cnf_nm));

	//DBG_PRT(106, os << "dbg_cnf_nm=" << dbg_cnf_nm);

	dbg_cnf.clear_cnf();

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
}
#endif

bool
memap::ck_guide_idx(coloring& guide_col, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	long g_idx = guide_col.co_szs_idx;
	if(g_idx == INVALID_IDX){
		BRAIN_CK(guide_col.is_co_virgin());
		return true;
	}
	BRAIN_CK(ma_szs_dotted.is_valid_idx(g_idx));
	long qua_sz = ma_szs_dotted[g_idx];
	MARK_USED(qua_sz);

	DBG_PRT_COND(DBG_ALL_LVS, ! ((qua_sz * 2) == guide_col.co_quas.size()) ,
		os << "ABORTING_DATA " << bj_eol;
		os << " dbg_id=" << dbg_id << bj_eol;
		os << " qua_sz=" << qua_sz << bj_eol;
		os << " guide_sz=" << guide_col.co_quas.size() << bj_eol;
		os << " guide=" << guide_col << bj_eol;
		os << " map=" << *this << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	BRAIN_CK((qua_sz * 2) == guide_col.co_quas.size());
#endif
	return true;
}

bool
memap::ck_map_guides(dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	BRAIN_CK(ck_guide_idx(ma_save_guide_col, dbg_id));
	BRAIN_CK(ck_guide_idx(ma_find_guide_col, dbg_id));
#endif
	return true;
}

bool
memap::map_ck_contained_in(brain& brn, coloring& colr, dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	long szs_idx = colr.co_szs_idx;
	row<neuron*>& all_neus = colr.co_neus;

	DBG_PRT_COND(DBG_ALL_LVS, ! (ma_szs_dotted.is_valid_idx(szs_idx)) ,
		os << "ABORTING_DATA " << bj_eol;
		os << " dbg_id=" << dbg_id << bj_eol;
		os << " szs_idx=" << szs_idx << bj_eol;
		os << " ma_szs_dotted=" << ma_szs_dotted << bj_eol;
	);
	BRAIN_CK(ma_szs_dotted.is_valid_idx(szs_idx));
	BRAIN_CK(brn.br_tot_ne_spots == 0);

	for(long ii = 0; ii < all_neus.size(); ii++){
		neuron& fll_neu = *(all_neus[ii]);
		fll_neu.set_spot(brn);
		DBG_PRT(103, os << "spot " << &fll_neu);
	}
	
	neuron* cfl_neu = ma_confl.ps_source;
	BRAIN_CK(cfl_neu != NULL_PT);

	DBG_PRT(103, os << "CK spot confl " << cfl_neu);
	BRAIN_CK(cfl_neu->ne_spot);

	long trace_sz = ma_szs_dotted[szs_idx];
	row<prop_signal>& trace = ma_dotted;

	BRAIN_CK((trace_sz == trace.size()) || trace.is_valid_idx(trace_sz));
	for(long ii = 0; ii < trace_sz; ii++){
		prop_signal& q_sig = trace[ii];
		if(q_sig.ps_source != NULL_PT){
			DBG_PRT(103, os << "CK spot " << q_sig.ps_source);
			BRAIN_CK(q_sig.ps_source->ne_spot);
		}
	}

	for(long ii = 0; ii < all_neus.size(); ii++){
		neuron& fll_neu = *(all_neus[ii]);
		fll_neu.reset_spot(brn);
	}

	BRAIN_CK(brn.br_tot_ne_spots == 0);
#endif
	return true;
}

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
memap::map_dbg_print(bj_ostream& os, mem_op_t mm, brain& brn){
#ifdef FULL_DEBUG
	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	//os << STACK_STR << bj_eol;
	os << "DBG_PRT=" << bj_eol;
	os << this << bj_eol;
	os << "brn_tk=" << brn.br_current_ticket << bj_eol;
	if(mm == mo_save){ os << "SAVE "; }
	if(mm == mo_find){ os << "FIND "; }

	//os << "CERO FILLED___________________________________________ " << bj_eol;
	sort_glb& tauto_srg = brn.br_tauto_neus_srg;
	os << " sg_dbg_cnf_tot_onelit=" << tauto_srg.sg_dbg_cnf_tot_onelit << bj_eol;

	os << " TATUTO_STEP_SORTEES (after step)=" << bj_eol;
	for(long aa = 0; aa < tauto_srg.sg_step_sortees.size(); aa++){
		os << *(tauto_srg.sg_step_sortees[aa]) << bj_eol;
	}

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
		os << "NA(" << (void*)this <<")";
		os << " cho=" << na_orig_cho;
		os.flush();
		return os;
	}
	os << "MEMAP(" << (void*)this <<")={ " << bj_eol;
	
	os << " active=" << na_active << bj_eol;
	os << " na_deact_tier=" << na_deact_tier << bj_eol;
	os << " cho=" << na_orig_cho << bj_eol;
	os << " na_submap=" << na_submap << bj_eol;
	
	os << " na_forced=" << bj_eol;
	os << na_forced << bj_eol;
	os << " na_non_forced=" << bj_eol;
	na_non_forced.print_row_data(os, true, "\n");
	
	os << " ma_guide_col=" << bj_eol;
	os << ma_guide_col << bj_eol;
	
	os << " na_all_filled_in_propag=" << bj_eol;
	na_all_filled_in_propag.print_row_data(os, true, "\n");
	os << "}";
	os.flush();
#endif
	return os;
}
	
bj_ostream&
memap::print_memap(bj_ostream& os, bool from_pt){
#ifdef FULL_DEBUG
	MARK_USED(from_pt);
	if(from_pt){
		os << "MM(" << (void*)this <<")";
		os << " cho=" << ma_cho;
		os << " szs_dotted=";
		os << ma_szs_dotted;
		os << " bef_rtct_tk=" << ma_before_retract_tk;
		os << " aft_rtct_tks=" << ma_after_retract_tks;
		os.flush();
		return os;
	}
	os << "MEMAP(" << (void*)this <<")={ " << bj_eol;
	os << " cho=" << ma_cho << bj_eol;
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
	os << "bef_rtct_tk=" << ma_before_retract_tk << bj_eol;
	os << "aft_rtct_tks=" << ma_after_retract_tks << bj_eol;
	os << "confl=" << ma_confl << bj_eol;
	os << "active=" << ma_active << bj_eol;
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
	bool has_src = has_source();
	
	ch_string lft_bra = "(";
	ch_string rgt_bra = ")";
	if(! has_src){
		lft_bra = "{";
		rgt_bra = "}";
	}

	bool dominated = false;
	if(pt_brn != NULL_PT){
		dominated = in_qu_dominated(*(pt_brn));
	}

	neuron* neu = qu_source;

	if(from_pt){
		os << lft_bra; 
		os << qu_id; 
		/*DBG(
			os << "." << qu_dbg_fst_lap_cho; 
			os << "." << qu_dbg_num_laps_cho; 
		)*/
		os << rgt_bra; 
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

		if(! qu_tee.is_unsorted()){ os << ".q" << qu_tee.so_qua_id; }
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
	//os << "\n choices:"; br_choices.print_row_data(os, true, " ", br_choices_lim, br_choices_lim); os << bj_eol;
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
		if(lv.ld_map0.ma_active){
			os << "1.";
		} else {
			os << "0.";
		}
	}
	os << "]";
#endif
}

void 
brain::dbg_prt_lvs_virgin(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "vrgn=[";
	
	row<leveldat*>& all_lv = br_data_levels;
	for(int aa = 0; aa < all_lv.size(); aa++){
		leveldat& lv = *(all_lv[aa]);
		if(lv.ld_map0.is_ma_virgin()){
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
		quanton* ch2 = lv.ld_map0.ma_cho;
		os << lv.ld_chosen;
		if(ch2 != NULL){ os << "."; } else { os << ","; }
		if((ch2 != NULL) && (ch2 != lv.ld_chosen)){
			os << "\n\n\n" << ch2 << " != " << lv.ld_chosen << "!!!!!\n\n\n";
		}
	}
	os << "]";
#endif
}

