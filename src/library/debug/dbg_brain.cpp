

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

#include "brain.h"
#include "dbg_run_satex.h"

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

void
deducer::dbg_deduc_find_next_dotted(){
#ifdef FULL_DEBUG
	while(! get_curr_quanton().has_dot()){
		DBG_PRT(20, os << "NOT dotted " << get_curr_quanton() 
				<< " in deduc find next dotted");
		de_trl_idx--;
	}

	BRAIN_CK(de_noteke.dk_note_layer <= get_de_brain().level());
	quanton& nxt_qua = get_curr_quanton();
	long qlv = nxt_qua.qlevel();

	de_noteke.update_notes_layer(qlv);
#endif
}

void
deducer::dbg_deduc_find_next_source(){
#ifdef FULL_DEBUG

	brain& brn = get_de_brain();

	BRAIN_CK(tg_confl() != NULL_PT);
	bool is_first = (de_nxt_src == tg_confl());

	BRAIN_CK(! is_first || (de_nxt_src != NULL));
	DBG_PRT(20, os << "reasoning_cause_of_conflict " << de_nxt_src;
		os << " tot_dotted=" << de_noteke.dk_tot_noted;
		os << " num_dotted_in_lv=" << de_noteke.dk_num_noted_in_layer;
		os << " dotting level=" << de_noteke.dk_note_layer;
	);

	if(de_nxt_src != NULL){
		BRAIN_CK(! de_nxt_src->ne_fibres.is_empty());
		BRAIN_CK(is_first || (de_nxt_src->ne_fibres[0]->get_charge() == cg_positive) );
		BRAIN_CK(is_first || de_nxt_src->neu_compute_binary());

		if(! de_nxt_src->ne_original){
			de_all_original = false;
		}

		row_quanton_t& causes = de_nxt_src->ne_fibres;
		long from = (is_first)?(0):(1);
		long until = causes.size();
		//quanton* max_qua = 
		de_noteke.set_motive_notes(de_nxt_src->ne_fibres, from, until);
	}

	BRAIN_CK(de_noteke.dk_tot_noted > 0);

	dbg_deduc_find_next_dotted();

	quanton& nxt_qua = get_curr_quanton();
	DBG_PRT(20, os << "dotted found " << nxt_qua 
		<< " num_dotted_in_lv " << de_noteke.dk_num_noted_in_layer;
		os << " dotting level=" << de_noteke.dk_note_layer;
	);

	de_nxt_src = nxt_qua.get_source();

	nxt_qua.reset_dot(brn);
	BRAIN_CK(nxt_qua.qlevel() == de_noteke.dk_note_layer);
	BRAIN_CK(nxt_qua.is_pos());

	de_noteke.dec_notes();

	DBG_PRT(101, os << "qua=" << &nxt_qua << " filled APPEND=";
		nxt_qua.qu_full_charged.print_row_data(os, true, "\n");
	);
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
		BRAIN_CK(mot->qlevel() < brn.br_dbg.dbg_before_retract_lv);

		DBG_COND_COMM(! (mot->get_charge() == cg_negative) ,
			os << "ABORTING_DATA " << bj_eol;
			os << "mots=" << mots << bj_eol;
			os << "mot=" << mot << bj_eol;
			brn.print_trail(os);
			os << "END_OF_aborting_data" << bj_eol;
		);
		BRAIN_CK(mot->get_charge() == cg_negative);
	}
#endif
	return true;
}

void
deducer::dbg_find_dct_of(neuron& confl, deduction& dct){
#ifdef FULL_DEBUG
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

	DBG(row_quanton_t tmp_mots);
	DBG(de_noteke.get_all_motives(tmp_mots));
	BRAIN_CK(tmp_mots.is_empty());

	de_filled_in_lv.clear();
	de_trl_idx = trail_sz - 1;
	de_nxt_src = &confl;


	BRAIN_CK(de_noteke.dk_note_layer == brn.level());
	long deduc_lv = de_noteke.dk_note_layer;
	MARK_USED(deduc_lv);

	dbg_deduc_find_next_source();
	while(de_noteke.dk_num_noted_in_layer > 0){
		dbg_deduc_find_next_source();
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

	find_max_level(dct.dt_motives, dct.dt_target_level);

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
#endif
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
	canon_save(dbg_cnf_nm, dbg_cnf.cf_chars, false);

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
	canon_save(dbg_cnf_nm, dbg_cnf.cf_chars, false);

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
	dbg_ic_active = false;
	dbg_ic_after = false;
	dbg_just_read = false;
	dbg_clean_code = false;
	
	dbg_find_id = 0;
	dbg_save_id = 0;
	dbg_canon_find_id = 0;
	dbg_canon_save_id = 0;
	
	dbg_periodic_prt = true;

	dbg_ic_max_seq = -1;
	dbg_ic_seq = 0;
	dbg_ic_gen_jpg = false;
	
	dbg_all_chosen.clear();

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
	
	BRAIN_CK(ma_confl != NULL_PT);

	DBG_PRT(103, os << "CK spot confl " << ma_confl);
	BRAIN_CK(ma_confl->ne_spot);

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

bool
memap::ck_map_guides(dbg_call_id dbg_id){
#ifdef FULL_DEBUG
	BRAIN_CK(ck_guide_idx(ma_save_guide_col, dbg_id));
	BRAIN_CK(ck_guide_idx(ma_find_guide_col, dbg_id));
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

