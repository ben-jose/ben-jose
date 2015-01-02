

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

memap.cpp  

memap related funcs.

--------------------------------------------------------------*/

#include "stack_trace.h"
#include "dimacs.h"
#include "brain.h"
#include "solver.h"
#include "dbg_config.h"
#include "dbg_prt.h"

bool
memap::map_find(brain& brn){
	DBG(
		bool do_finds = (brn.get_solver().slv_dbg.F > 0);
		if(do_finds){
			return map_oper(mo_find, brn);
		}
		return false;
	)
	NOT_DBG(return map_oper(mo_find, brn);)
}

bool
memap::map_save(brain& brn){
	DBG(
		bool do_save = (brn.get_solver().slv_dbg.W > 0);
		if(do_save){
			return map_oper(mo_save, brn);
		}
		return false;
	)
	NOT_DBG(return map_oper(mo_save, brn);)
}

long
memap::get_save_idx(){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(ma_szs_dotted.size() > 1);
	long sv_idx = ma_szs_dotted.last_idx() - 1;
	BRAIN_CK(ma_szs_dotted.is_valid_idx(sv_idx));
	return sv_idx;
}

long
memap::get_find_idx(){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(! ma_szs_dotted.is_empty());
	long fn_idx = ma_szs_dotted.last_idx();
	BRAIN_CK(ma_szs_dotted.is_valid_idx(fn_idx));
	return fn_idx;
}

void
memap::map_inc_stab_guide(brain& brn, coloring& guide_col){
	BRAIN_CK(ck_map_guides(dbg_call_1));

	long lst_idx = guide_col.co_szs_idx;
	DBG(long old_idx = lst_idx);
	BRAIN_CK(ma_szs_dotted.is_valid_idx(lst_idx));

	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;
	coloring ini_guide_col(&brn);
	dima_dims dims0;

	long nxt_idx = lst_idx + 1;

	get_initial_guide_coloring(brn, ini_guide_col, nxt_idx);
	guide_col.add_coloring(brn, ini_guide_col);	

	DBG(long old_quas_sz = guide_col.co_quas.size());
	DBG(long old_neus_sz = guide_col.co_neus.size());

	brn.all_mutual_init();
	neus_srg.sg_one_ccl_per_ss = false;

	DBG_PRT(61, os << " bef_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " bef_load_col quas_srg=" << &quas_srg);
	guide_col.load_colors_into(brn, neus_srg, quas_srg, dims0);
	DBG_PRT(61, os << " aft_load_col neus_srg=" << &neus_srg);
	DBG_PRT(61, os << " aft_load_col quas_srg=" << &quas_srg);
	neus_srg.stab_mutual(quas_srg);
	guide_col.save_colors_from(neus_srg, quas_srg);
	guide_col.co_szs_idx = nxt_idx;

	BRAIN_CK(old_quas_sz == guide_col.co_quas.size());
	BRAIN_CK(old_neus_sz == guide_col.co_neus.size());

	BRAIN_CK(guide_col.co_szs_idx == (old_idx + 1));

	BRAIN_CK(ck_map_guides(dbg_call_2));
}

coloring&
memap::map_guide_coloring(mem_op_t mm){
	if(mm == mo_save){
		return ma_save_guide_col;
	} 
	BRAIN_CK(mm == mo_find);
	return ma_find_guide_col;
}

void
memap::get_initial_guide_coloring(brain& brn, coloring& clr, long idx_szs){
	BRAIN_CK(&ma_save_guide_col != &clr);
	BRAIN_CK(&ma_find_guide_col != &clr);
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(idx_szs > 0);

	clr.init_coloring();

	row_quanton_t&	all_quas = clr.co_quas;
	row<long>&	qua_colors = clr.co_qua_colors;

	row<neuron*>&	all_neus = clr.co_neus;
	row<long>&	neu_colors = clr.co_neu_colors;

	BRAIN_CK(ma_szs_dotted.is_valid_idx(idx_szs));

	all_quas.clear();
	all_neus.clear();

	row<prop_signal>& dtrace = ma_dotted;
	long beg_sz = ma_szs_dotted[idx_szs - 1];
	long end_sz = ma_szs_dotted[idx_szs];

	long sig_col = 1;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	for(long ii = beg_sz; ii < end_sz; ii++){
		prop_signal& q_sig1 = dtrace[ii];

		bool inc_col = false;
		if(ii > 0){
			prop_signal& q_sig0 = dtrace[ii - 1];
			
			// this two lines are the whole purpose of tiers. 
			// initialize the guide coloring (one color per tier).
			long col0 = q_sig0.ps_tier;
			long col1 = q_sig1.ps_tier;
			
			BRAIN_CK(col0 >= 0);
			BRAIN_CK(col1 >= 0);
			if(col0 != col1){
				BRAIN_CK(col0 > col1);
				inc_col = true;
			}
		}
	
		if(inc_col){ sig_col++; }
		
		BRAIN_CK(q_sig1.ps_quanton != NULL_PT);

		quanton& qua = *(q_sig1.ps_quanton);
		quanton& opp = qua.opposite();

		BRAIN_CK(! qua.has_note1());
		DBG(qua.set_note1(brn));
	
		all_quas.push(&qua);
		qua_colors.push(sig_col);

		all_quas.push(&opp);
		qua_colors.push(sig_col);

		if(q_sig1.ps_source != NULL_PT){
			neuron& neu = *(q_sig1.ps_source);

			all_neus.push(&neu); 
			neu_colors.push(sig_col);
		}
	}

	DBG(reset_all_note1(brn, all_quas));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(clr.ck_cols());
}

void
memap::map_record_szs(){
	ma_szs_dotted.push(ma_dotted.size());
	ma_szs_filled.push(ma_filled.size());
}

void
memap::map_get_layer_neus(row<neuron*>& neus, long lyr_idx1, long lyr_idx2, bool ck_tks){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(lyr_idx1 < lyr_idx2);
	BRAIN_CK((lyr_idx1 == INVALID_IDX) || ma_szs_filled.is_valid_idx(lyr_idx1));
	BRAIN_CK(ma_szs_filled.is_valid_idx(lyr_idx2));

	neus.clear();

	row<neuron*>& m_neus = ma_filled;
	long beg_sz = 0;
	if(ma_szs_filled.is_valid_idx(lyr_idx1)){
		beg_sz = ma_szs_filled[lyr_idx1];
	}
	long end_sz = ma_szs_filled[lyr_idx2];

	for(long bb = beg_sz; bb < end_sz; bb++){
		BRAIN_CK(m_neus[bb] != NULL_PT);
		neuron& neu = *(m_neus[bb]);

		bool ck_val = true;
		if(ck_tks){ 
			ck_val = neu.recoiled_in_or_after(ma_before_retract_tk); 
		}

		if(ck_val){ neus.push(&neu); }
	}
}

void
memap::map_replace_with(brain& brn, memap& mpp, dbg_call_id call_id){

	DBG_PRT(113, os << "before_replace call_id=" << call_id << 
			" tk=" << brn.br_current_ticket << bj_eol; 
	);
	BRAIN_CK(mpp.ck_map_guides(dbg_call_1));
	BRAIN_CK(is_ma_virgin());
	
	ma_brn = &brn;

	ma_before_retract_tk = mpp.ma_before_retract_tk;
	mpp.ma_after_retract_tks.move_to(ma_after_retract_tks);

	ma_confl = mpp.ma_confl;
	ma_cho = mpp.ma_cho; 

	mpp.ma_dotted.move_to(ma_dotted);
	mpp.ma_filled.move_to(ma_filled);

	mpp.ma_szs_dotted.move_to(ma_szs_dotted);
	mpp.ma_szs_filled.move_to(ma_szs_filled);

	//ma_sz_dotted = mpp.ma_sz_dotted;
	//ma_sz_filled = mpp.ma_sz_filled;

	mpp.ma_fll_in_lv.move_to(ma_fll_in_lv);
	mpp.ma_discarded.move_to(ma_discarded);

	mpp.ma_save_guide_col.move_co_to(ma_save_guide_col);
	mpp.ma_find_guide_col.move_co_to(ma_find_guide_col);

	BRAIN_CK(mpp.ma_dotted.is_empty());
	BRAIN_CK(mpp.ma_filled.is_empty());
	BRAIN_CK(mpp.ma_fll_in_lv.is_empty());
	BRAIN_CK(mpp.ma_discarded.is_empty());

	mpp.reset_memap(brn);
	BRAIN_CK(mpp.is_ma_virgin());
	DBG_PRT(113, os << "AFTER_REPLACE call_id=" << call_id << 
			" tk=" << brn.br_current_ticket << bj_eol; 
		//os << " the_mpp" << *this
	);
	BRAIN_CK(ck_map_guides(dbg_call_2));
}

void
memap::get_initial_tauto_coloring(brain& brn, coloring& stab_guide_clr, 
								  coloring& base_tauto_clr, bool ck_tks)
{
	BRAIN_CK(&stab_guide_clr != &base_tauto_clr);

	base_tauto_clr.init_coloring();

	stab_guide_clr.get_initial_sorting_coloring(brn, base_tauto_clr, false);

	long guide_idx = stab_guide_clr.co_szs_idx;

	row<neuron*>&	all_neus = base_tauto_clr.co_neus;
	row<long>&	neu_colors = base_tauto_clr.co_neu_colors;

	map_get_layer_neus(all_neus, INVALID_IDX, guide_idx, ck_tks);
	neu_colors.fill(1, all_neus.size());

	base_tauto_clr.co_szs_idx = guide_idx;

	BRAIN_CK(base_tauto_clr.ck_cols());
}

bool
memap::map_prepare_mem_oper(mem_op_t mm, brain& brn){
	DBG_PRT(110, os << "map_mem_oper=" << ((void*)this));
	BRAIN_CK(ck_map_guides(dbg_call_1));
	DBG_PRT(110, os << "map_mem_oper=" << this);

	coloring& guide_col = map_guide_coloring(mm);

	long op_szs_idx = get_save_idx();
	if(mm == mo_find){
		op_szs_idx = get_find_idx();
	}

	BRAIN_CK(op_szs_idx != INVALID_IDX);
	BRAIN_CK((guide_col.co_szs_idx != INVALID_IDX) || guide_col.is_co_virgin());

	// update guide

	while(guide_col.co_szs_idx < op_szs_idx){
		map_inc_stab_guide(brn, guide_col);
	}
	BRAIN_CK(guide_col.co_szs_idx == op_szs_idx);
	BRAIN_CK(ck_map_guides(dbg_call_3));
	BRAIN_CK(map_ck_contained_in(brn, guide_col, dbg_call_1));

	// stab uni_guide
	
	skeleton_glb& skg = brn.get_skeleton();
	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;

	canon_cnf& cnf1 = neus_srg.stab_mutual_get_cnf(skg, PHASE_1_COMMENT, false);

	BRAIN_CK(! cnf1.cf_phdat.has_ref());

	ref_strs phtd;
	row_str_t dbg_shas;

	phtd.pd_ref1_nam = cnf1.get_ref_path();	// stab guide 
	phtd.pd_ref2_nam = cnf1.get_lck_path();
	
	dbg_shas.push(cnf1.cf_sha_str + "\n");

	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();
		
		ch_string find_ref = phtd.pd_ref1_nam;
		ch_string pth1 = skg.as_full_path(find_ref);
		bool found1 = skg.find_skl_path(pth1, &iinfo);
		
		bj_output_t& o_info = brn.get_out_info();
		if(! found1){ 
			o_info.bjo_quick_discards++;
			return false; 
		}
		o_info.bjo_num_finds++;
	}

	neus_srg.stab_mutual_unique(quas_srg);
	//canon_cnf& cnf2 = neus_srg.stab_mutual_get_cnf(skg, PHASE_2_COMMENT, false);

	coloring uni_guide_col(&brn);
	uni_guide_col.save_colors_from(neus_srg, quas_srg);
	BRAIN_CK(uni_guide_col.co_all_qua_consec);
	BRAIN_CK(uni_guide_col.co_all_neu_consec);

	uni_guide_col.co_szs_idx = op_szs_idx; 	// used in tauto_coloring

	BRAIN_CK(map_ck_contained_in(brn, uni_guide_col, dbg_call_2));

	//DBG_PRT_COND(67, (mm == mo_find), map_dbg_prt(os, mm, brn));
	row<sortee*>& guide_tees = brn.br_tmp_wrt_guide_tees;
	neus_srg.sg_step_sortees.move_to(guide_tees);

	// stab uni_colors

	dima_dims dims1;
	coloring ini_tau_col(&brn);

	sort_glb& fnl_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& fnl_qu_srg = brn.br_tauto_quas_srg;

	get_initial_tauto_coloring(brn, uni_guide_col, ini_tau_col, (mm == mo_save));

	BRAIN_CK(map_ck_contained_in(brn, ini_tau_col, dbg_call_3));

	brn.all_mutual_init();
	ini_tau_col.load_colors_into(brn, fnl_ne_srg, fnl_qu_srg, dims1);

	fnl_ne_srg.stab_mutual(fnl_qu_srg);
	BRAIN_CK(fnl_qu_srg.sg_step_all_consec);

	canon_cnf& tauto_cnf = fnl_ne_srg.stab_mutual_get_cnf(skg, FINAL_COMMENT, true);

	dbg_shas.push(tauto_cnf.cf_sha_str + "\n");

	row<sortee*>& tauto_tees = brn.br_tmp_wrt_tauto_tees;
	fnl_ne_srg.sg_step_sortees.move_to(tauto_tees);

	// init write ccls 

	brn.init_mem_tmps();

	row<canon_clause*>& 	tmp_tauto_ccls = brn.br_tmp_wrt_tauto_ccls;
	row<canon_clause*>& 	tmp_guide_ccls = brn.br_tmp_wrt_guide_ccls;
	row<canon_clause*>& 	tmp_diff_ccls = brn.br_tmp_wrt_diff_ccls;

	split_tees(fnl_ne_srg, tauto_tees, guide_tees, tmp_guide_ccls, tmp_diff_ccls);
	tauto_cnf.cf_clauses.move_to(tmp_tauto_ccls);

	// init write cnfs

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	tmp_tauto_cnf.init_with(skg, tmp_tauto_ccls);
	tmp_diff_cnf.init_with(skg, tmp_diff_ccls);
	tmp_guide_cnf.init_with(skg, tmp_guide_ccls);

	tmp_diff_cnf.cf_phdat = phtd;
	dbg_shas.move_to(tmp_diff_cnf.cf_dbg_shas);

	BRAIN_CK(tmp_tauto_ccls.is_empty());
	BRAIN_CK(tmp_diff_ccls.is_empty());

	// final checks

	row<canon_clause*>& ccls_tauto_cnf = tmp_tauto_cnf.cf_clauses;
	row<canon_clause*>& ccls_diff_cnf = tmp_diff_cnf.cf_clauses;

	MARK_USED(ccls_tauto_cnf);
	MARK_USED(ccls_diff_cnf);

	DBG_PRT_COND(100, (mm == mo_save), map_dbg_print(os, mm, brn));
	DBG_PRT_COND(100, (mm == mo_save), 
		os << "GUIDE=" << bj_eol;
		tmp_guide_ccls.print_row_data(os, true, "\n");
		os << "TAUTO=" << bj_eol;
		ccls_tauto_cnf.print_row_data(os, true, "\n");
		os << "DIFF=" << bj_eol;
		ccls_diff_cnf.print_row_data(os, true, "\n");
	);

	DBG(
		bool are_eq = false;
		bool is_sub = false;
		cmp_is_sub cmp_resp = k_no_is_sub;

		cmp_resp = ccls_tauto_cnf.sorted_set_is_subset(ccls_diff_cnf, cmp_clauses, are_eq);
		is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));
		BRAIN_CK(is_sub);

		are_eq = false;
		is_sub = false;
		cmp_resp = k_no_is_sub;
		cmp_resp = ccls_tauto_cnf.sorted_set_is_subset(tmp_guide_ccls, cmp_clauses, are_eq);
		is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));
		BRAIN_CK(is_sub);

	);
	BRAIN_CK(! tmp_tauto_cnf.has_phase_path());
	BRAIN_CK(tmp_diff_cnf.has_phase_path());
	BRAIN_CK(! tmp_guide_cnf.has_phase_path());

	return true;
}

bool
memap::map_oper(mem_op_t mm, brain& brn){
	brn.init_mem_tmps();

	bool prep_ok = map_prepare_mem_oper(mm, brn);

	if(! prep_ok){
		DBG_PRT(122, os << "map_oper skip (prepare == false)");
		return false;
	}
	DBG_PRT(122, os << "map_oper_go");

	skeleton_glb& skg = brn.get_skeleton();

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	tmp_diff_cnf.cf_dbg_shas.push(tmp_tauto_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_diff_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_guide_cnf.cf_sha_str + "\n");

	BRAIN_CK(! tmp_tauto_cnf.has_phase_path());
	BRAIN_CK(tmp_diff_cnf.has_phase_path());
	BRAIN_CK(! tmp_guide_cnf.has_phase_path());

	bj_output_t& o_info = brn.get_out_info();
	
	bool oper_ok = false;
	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();

		long fst_idx = tmp_diff_cnf.first_vnt_i_super_of(skg, &iinfo);

		///////  start of debug of NO DEBUG

		DBG(
			if(brn.br_dbg.dbg_clean_code && (brn.br_dbg.dbg_canon_find_id == 10)){
				bj_ostream& os = bj_dbg;

				os << "TRYING to find cnf=" << bj_eol << tmp_diff_cnf << bj_eol
					<< "SHAS=" << bj_eol << tmp_diff_cnf.cf_dbg_shas << bj_eol
					//<< "fst_vpth='" << fst_vpth << "'" << bj_eol
					<< "fst_idx= " << fst_idx << bj_eol
					<< "find_id= " << brn.br_dbg.dbg_canon_find_id << bj_eol;
			}
		)

		///////  end of debug of NO DEBUG

		oper_ok = (fst_idx != INVALID_NATURAL);
		if(oper_ok){
			ch_string fst_vpth = tmp_diff_cnf.get_variant_path(skg, fst_idx, skg.in_verif());
			DBG_PRT(115, 
				os << "found cnf=" << bj_eol << tmp_diff_cnf << "FOUND CNF" << bj_eol
				<< "SHAS=" << bj_eol << tmp_diff_cnf.cf_dbg_shas << bj_eol
				<< "fst_vpth='" << fst_vpth << "'" << bj_eol
				<< "find_id= " << brn.br_dbg.dbg_canon_find_id);
			DBG_COMMAND(115, getchar());
			
			o_info.bjo_sub_cnf_hits++;

			row<neuron*>& all_tmp_found = brn.br_tmp_found_neus;
			all_tmp_found.clear();

			// BJ_FIX_THIS
			ccl_row_as<neuron>(tmp_diff_cnf.cf_clauses, all_tmp_found, true);
			
			for(long aa = 0; aa < all_tmp_found.size(); aa++){
				BRAIN_CK(all_tmp_found[aa] != NULL_PT);
				neuron& neu = *(all_tmp_found[aa]);			
				neu.ne_recoil_tk.update_ticket(brn);
			}			
		}
	} else {
		ref_strs& phd = tmp_diff_cnf.cf_phdat;
		
		BRAIN_CK(mm == mo_save);
		BRAIN_CK(! skg.kg_save_canon || phd.has_ref());

		ch_string lk_dir = phd.lck_nam();
		int fd_lk = skg.get_write_lock(lk_dir);

		if(fd_lk != -1){
			ch_string sv_pth1 = tmp_tauto_cnf.get_cnf_path() + SKG_CANON_NAME;
			ch_string sv_pth2 = tmp_tauto_cnf.get_cnf_path() + SKG_DIFF_NAME;
			ch_string sv_pth3 = tmp_tauto_cnf.get_cnf_path() + SKG_GUIDE_NAME;

			tmp_tauto_cnf.save_cnf(skg, sv_pth1);
			oper_ok = tmp_diff_cnf.save_cnf(skg, sv_pth2);
			tmp_guide_cnf.save_cnf(skg, sv_pth3);

			o_info.bjo_saved_targets++;
			
			//BRAIN_CK(! oper_ok || srg_forced.base_path_exists(skg));
			
			skg.drop_write_lock(lk_dir, fd_lk);

			ch_string pth1 = phd.pd_ref1_nam;
			ch_string pth2 = phd.pd_ref2_nam;

			BRAIN_CK((pth1 == "") || skg.find_skl_path(skg.as_full_path(pth1)));
			BRAIN_CK((pth2 == "") || skg.find_skl_path(skg.as_full_path(pth2)));

			DBG_CHECK_SAVED(
				if(oper_ok){
					ch_string sv1_name = skg.as_full_path(sv_pth1);
					dbg_run_satex_on(brn, sv1_name);
				}
			);			
		}
	}
	return oper_ok;
}

bool
brain::in_edge_of_level(){
	long trl_lv = trail_level();
	long b_lv = level();
	bool in_edge = (trl_lv != b_lv);
	BRAIN_CK(! in_edge || ((trl_lv + 1) == b_lv));
	return in_edge;
}

bool
brain::can_write_reverse_map(deduction& dct){
	if(dct.is_dt_virgin()){ return false; }
	//bool can_write = (dct.dt_forced_level == level());
	//return can_write;
	return false;
}

bool
brain::in_edge_of_target_lv(deduction& dct){
	if(dct.is_dt_virgin()){ return false; }
	long trl_lv = trail_level();
	//bool in_tg_lv = (trl_lv < dct.dt_target_level);
	bool in_tg_lv = (trl_lv <= dct.dt_target_level);
	BRAIN_CK(! in_tg_lv || ((trl_lv + 1) == level()));
	return in_tg_lv;
}

void
brain::reverse(){
	DEDUC_DBG(deduction& dct2 = br_dbg.dbg_deduc);
	//long dbg_old_lv = level();

	BRAIN_CK(! has_psignals());
	brain& brn = *this;
	notekeeper& nke0 = br_retract_nke0;
	deduction& dct = br_retract_dct;
	memap& mpp0 = br_retract_map0;
	long& all_notes0 = br_qu_tot_note0;
	//row_quanton_t& all_rev = br_tmp_rever_quas;
	//MARK_USED(all_rev);
	MARK_USED(all_notes0);

	br_retract_is_first_lv = true;

	dct.init_deduction();
	nke0.init_notes(level());
	mpp0.reset_memap(brn);

	BRAIN_CK(dct.is_dt_virgin());
	BRAIN_CK(nke0.dk_tot_noted == 0);
	BRAIN_CK(mpp0.is_ma_virgin());
	BRAIN_CK(level() != ROOT_LEVEL);
	BRAIN_CK(all_notes0 == 0);
	BRAIN_CK(br_semi_monos_to_update.is_empty());
	//BRAIN_CK(all_rev.is_empty());

	DBG_PRT(110, os << "BEFORE_REVERSE " << br_current_ticket << bj_eol; 
		print_trail(os);
		os << " trl_lv" << trail_level()
		<< " brn_lv" << level()
	);
	DBG_PRT(112,
		os << "BEFORE_REVERSE" << bj_eol;
		print_trail(os);
		os << "brn_tk=" << br_current_ticket << bj_eol;
		os << "learned=" << bj_eol;
		data_level().ld_learned.print_row_data(os, true, "\n");
		os << " trl_lv" << trail_level() << " brn_lv" << level();
		os << "cfl=" << br_conflict_found << bj_eol;
		//os << "pulsate. BEFORE conflit treatment. Type ENTER to continue..." << bj_eol;
		//DO_GETCHAR
	);
	DBG_PRT(131, print_trail(os));
	DBG_PRT(122, print_trail(os));
	DBG_PRT(14, print_trail(os));
	DBG_PRT(14, os << "chosen " << br_chosen);

	// START REVERSE (init mpp0 and nke0)

	BRAIN_CK(found_conflict());
	
	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());

	DEDUC_DBG(br_deducer.deduction_analysis(br_conflict_found, dct2));
	BRAIN_CK_PRT(dct2.dt_target_level >= ROOT_LEVEL, 
		os << recoil() << ".dct2=" << dct2
	);

	mpp0.ma_confl = br_conflict_found;
	mpp0.ma_before_retract_tk.update_ticket(brn);
	
	neuron& cfl = *(br_conflict_found.ps_source);
	DBG_PRT_COND(122, ! cfl.in_ne_dominated(brn), os << "NOT_DOM cfl=" << cfl);
	cfl.set_motives(brn, nke0, true); // init nke0 with confl

	BRAIN_CK(all_notes0 > 0);

	BRAIN_CK(! mpp0.is_ma_virgin());

	//BRAIN_CK(cfl.ne_original);
	if(! cfl.ne_original){ 
		mpp0.reset_memap(brn);
		DBG_PRT(117, os << "mpp0.reset_memap (! orig)");
	}

	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, dbg_prt_lvs_have_learned(os));
	//DBG_PRT(122, dbg_prt_lvs_virgin(os));
	
	// REVERSE LOOP

	//quanton* chosen_qua = NULL_PT;
	bool has_in_mem = false;
	MARK_USED(has_in_mem);

	while(true){
		BRAIN_CK(! mpp0.ma_active);
		BRAIN_CK(mpp0.ck_map_guides(dbg_call_1));
		
		DBG(int vg_m0 = ((mpp0.is_ma_virgin())?(1):(0)));

		bool end_of_recoil = in_edge_of_target_lv(dct);
		if(end_of_recoil){
			DBG_PRT(110, os << "end_of_recoil " << br_current_ticket 
				<< " trl_lv" << trail_level()
				<< " brn_lv" << level()
			);
			BRAIN_CK(! dct.is_dt_virgin());
			bool in_mm = false;
			memap& lv_map0 = data_level().ld_map0;
			if(lv_map0.is_ma_virgin() && ! mpp0.ma_dotted.is_empty()){
				BRAIN_CK(mpp0.ck_last_szs());
				//BRAIN_CK(! mpp0.ma_dotted.is_empty());

				DBG(br_dbg.dbg_find_id++; br_dbg.dbg_canon_find_id = br_dbg.dbg_find_id;)

				in_mm = mpp0.map_find(brn);
			}
			if(! in_mm){ 
				BRAIN_CK((trail_level() + 1) == level());
				DBG_PRT(122, os << "!ff." << vg_m0);
				//break;
			} else {
				DBG_PRT(122, os << "FF." << vg_m0);
				has_in_mem = true;
				dct.init_deduction();
				BRAIN_CK(dct.is_dt_virgin());
			}
		}
		
		BRAIN_CK(level() != ROOT_LEVEL);

		bool end_of_lev = in_edge_of_level();
		if(end_of_lev){
			DBG_PRT(110, os << "end_of_lev " << br_current_ticket 
				<< " trl_lv" << trail_level()
				<< " brn_lv" << level()
				<< " " << dct
			);
			BRAIN_CK(level() != ROOT_LEVEL);
			BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			BRAIN_CK(level() <= nke0.dk_note_layer);

			DBG(bool wrote = false);
			memap& lv_map0 = data_level().ld_map0;
			
			DBG(bool m_act = lv_map0.ma_active);
			DBG(bool m_cw = false);
			if(lv_map0.ma_active){
				BRAIN_CK(false);
				// OLD WRITE MAMAP
			} // lv_map0.ma_active

			dec_level();
			br_retract_is_first_lv = false;

			DBG(int vg2_m0 = ((mpp0.is_ma_virgin())?(1):(0)));
			DBG_PRT(122, if(wrote){ os << "WW."; } else { os << "!ww."; }
				os << vg_m0 << '.' << vg2_m0 << ".a." << m_act << ".c." << m_cw;
			);

			
		} // end_of_lev

		BRAIN_CK(! mpp0.ma_active);
		BRAIN_CK(trail_level() == level());

		if(end_of_recoil){ break; }
		
		if(level() == ROOT_LEVEL){
			break;
		}

		if(lv_has_learned()){
			mpp0.reset_memap(brn);
			DBG_PRT(121, os << "mpp0.reset_memap (has_learnd)");
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
		quanton& opp = *(qua.qu_inverse);

		neuron* src = qua.get_source();

		BRAIN_CK(qua.is_pos());
		BRAIN_CK(dct.is_dt_virgin() || (level() >= dct.dt_target_level));

		// notes

		//BRAIN_CK((dbg_old_lv != level()) || qua.has_source() || qua.has_note0());
		BRAIN_CK(! br_retract_is_first_lv || qua.has_source() || qua.has_note0());

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

			if(! mpp0.is_ma_virgin()){
				qua.qu_full_charged.append_to(mpp0.ma_fll_in_lv);
				opp.qu_full_charged.append_to(mpp0.ma_fll_in_lv);

				prop_signal& n_sig = mpp0.ma_dotted.inc_sz();
				n_sig.init_qua_signal(qua);

				BRAIN_CK(! qua.has_source() || (! src->ne_original) || 
						has_neu(mpp0.ma_fll_in_lv, src));
			}

			if(nke0.dk_num_noted_in_layer == 0){
				DBG_PRT(14, os << "SETTING DCT=" << &qua);
				if(dct.is_dt_virgin()){
					dct.set_with(brn, nke0, qua);
				}

				BRAIN_CK(! dct.is_dt_virgin());
			}

			BRAIN_CK(mpp0.is_ma_virgin() || (mpp0.ma_dotted.last().ps_quanton == &qua));

			if(src != NULL_PT){
				BRAIN_CK(mpp0.is_ma_virgin() || (! src->ne_original) || 
						has_neu(mpp0.ma_fll_in_lv, src));
				if(! src->ne_original){ 
					mpp0.reset_memap(brn);
					DBG_PRT(121, os << "mpp0.reset_memap (! orig)");
					//all_src_orig = false; 
				}

				src->set_motives(brn, nke0, false);
				BRAIN_CK(nke0.dk_num_noted_in_layer > 0);
				BRAIN_CK(! src->ne_original || has_neu(qua.qu_full_charged, src));
			} else {
				BRAIN_CK(! qua.has_source());
				BRAIN_CK(! dct.is_dt_virgin());
				BRAIN_CK(nke0.dk_num_noted_in_layer == 0);

				if(! mpp0.is_ma_virgin()){

					BRAIN_CK(! mpp0.ma_dotted.is_empty());
					BRAIN_CK(! mpp0.ma_dotted.last().ps_quanton->has_source());
					BRAIN_CK(mpp0.ma_dotted.last().ps_quanton == &qua);

					mpp0.ma_cho = &qua;
					mpp0.set_filled(brn); // old update_filled

					DBG_PRT(110, os << "rec_szs " << br_current_ticket << " " << &mpp0);
					mpp0.map_record_szs();
				}
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

	DBG_PRT(131, 
			print_trail(os);
			dbg_prt_lvs_cho(os);
			os << "tr_lv=" << trail_level() << " tg_lv=" << dct.dt_target_level;
	);
	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);

	//BRAIN_CK(! has_in_mem);	// DBG purposes
	
	DEDUC_DBG(has_in_mem || dbg_ck_deducs(dct, dct2));
	DBG(long rr_lv = trail_level());
	//BRAIN_CK((level() == ROOT_LEVEL) || ((rr_lv + 1) == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == dct.dt_target_level));

	// update leveldat
	
	memap& lv_map0 = data_level().ld_map0;
	if(! mpp0.is_ma_virgin() && lv_map0.is_ma_virgin()){
		ticket& n_tk = mpp0.ma_after_retract_tks.inc_sz();
		n_tk.update_ticket(brn);

		BRAIN_CK(mpp0.ck_map_guides(dbg_call_2));
		//lv_map0.map_replace_with(brn, mpp0, dbg_call_2);
		//lv_map0.map_activate(brn);
		DBG_PRT(122, os << "Updated " << &(data_level()) << " with " << &lv_map0);
	}

	nke0.clear_all_quantons();
	BRAIN_CK(all_notes0 == 0);

	// learn motives

	BRAIN_CK(dct.dt_forced != NULL_PT);

	if(data_level().ld_first_learned == NULL_PT){
		data_level().ld_first_learned = dct.dt_forced;
	}
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

	DBG_PRT(131, 
			os << " f_qu=" << nxt_qua; 
			DO_GETCHAR()
	);
	DBG_PRT(111, os << "AFTER_REVERSE" << bj_eol; 
		print_trail(os);
		//os << " br_maps_active=" << br_maps_active << bj_eol;
		os << "brn_tk=" << br_current_ticket << bj_eol;
		os << "lv_map=" << data_level().ld_map0 << bj_eol;
		os << "learned=" << bj_eol;
		data_level().ld_learned.print_row_data(os, true, "\n");
		os << "dct=" << bj_eol;
		os << dct;
		os << "AFTER_REVERSE. Type ENTER to continue..." << bj_eol;
		DO_GETCHAR();
	);
	DBG_PRT(110, os << "AFTER_REVERSE " << br_current_ticket << bj_eol;
		print_trail(os);
		os << " trl_lv" << trail_level()
		<< " brn_lv" << level()
	);
	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, print_trail(os); os << dct << bj_eol);

	reset_conflict();
	BRAIN_CK(! found_conflict());
} // END of reverse

