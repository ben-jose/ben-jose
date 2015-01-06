

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

#include "file_funcs.h"
#include "stack_trace.h"
#include "dbg_run_satex.h"

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
		//if(ck_tks){ 
		//	ck_val = neu.recoiled_in_or_after(ma_before_retract_tk); 
		//}

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

/*
void
coloring::get_initial_sorting_coloring(brain& brn, coloring& ini_clr, bool fill_neus){
	BRAIN_CK(this != &ini_clr);
	BRAIN_CK(ck_cols());

	ini_clr.init_coloring();

	row_quanton_t&	my_quas = co_quas;

	row_quanton_t&	all_quas = ini_clr.co_quas;
	row<long>&	qua_colors = ini_clr.co_qua_colors;

	row<neuron*>&	all_neus = ini_clr.co_neus;
	row<long>&	neu_colors = ini_clr.co_neu_colors;

	row_quanton_t	all_opp;

	BRAIN_CK(ck_cols());
	BRAIN_CK(all_quas.is_empty());
	BRAIN_CK(all_neus.is_empty());
	
	all_quas.clear();
	all_neus.clear();

	// select quas

	BRAIN_CK(brn.br_qu_tot_note2 == 0);

	long curr_col = 0;

	for(long aa = 0; aa < my_quas.size(); aa++){
		BRAIN_CK(my_quas[aa] != NULL_PT);
		quanton& qua = *(my_quas[aa]);

		if(! qua.has_note2()){
			qua.set_note2(brn);

			quanton& opp = qua.opposite();
			all_quas.push(&qua);
			all_opp.push(&opp);

			curr_col++;
			qua_colors.push(curr_col);
			curr_col++;
			qua_colors.push(curr_col);
		}
	}

	reset_all_note2(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note2 == 0);

	all_opp.append_to(all_quas);

	if(fill_neus){
		row<neuron*>&	my_neus = co_neus;
		my_neus.copy_to(all_neus);
		neu_colors.fill(1, all_neus.size());
	}

	BRAIN_CK(ini_clr.ck_cols());
}*/

void
memap::get_initial_tauto_coloring(brain& brn, coloring& stab_guide_clr, 
								  coloring& base_tauto_clr, bool ck_tks)
{
	BRAIN_CK(&stab_guide_clr != &base_tauto_clr);

	base_tauto_clr.init_coloring();

	//stab_guide_clr.get_initial_sorting_coloring(brn, base_tauto_clr, false);

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

void
memap::reset_memap(brain& brn){
	//BRAIN_CK(map_ck_all_qu_dominated(brn));
	init_memap(&brn);
}

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

void
memap::set_filled(brain& brn){
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

		if(! neu.ne_original){
			disca_neus.push(&neu);
			continue;
		}

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
}

bool
neuron::is_filled_of_marks(quanton& nxt_qua){
	// nxt_qua is cho of level
	BRAIN_CK(nxt_qua.is_pos());
	BRAIN_CK(! nxt_qua.has_source());

	bool is_fll = true;
	DBG(long dbg_num_after = 0);
	for(long ii = 0; ii < fib_sz(); ii++){
		BRAIN_CK(ne_fibres[ii] != NULL_PT);
		
		// qua was charged in last propagate.
		quanton& qua = *(ne_fibres[ii]);
		quanton& opp = *(qua.qu_inverse);

		bool has_chg = qua.has_charge();
		bool is_nxt = ((&qua == &nxt_qua) || (&opp == &nxt_qua));
		
		// is_part tells if qua is in candidate subset
		bool is_part = (! has_chg || is_nxt);  

		DBG(if(qua.has_mark() && is_part){ dbg_num_after++; });

		if(! qua.has_mark() && is_part){
			// qua is not in memap (! has_mark) so candidate subset is not in memap
			is_fll = false;
			break;
		}

		if(qua.is_pos() && ! is_part){
			// neu is sat in upper level so candidate subset is not in memap
			is_fll = false;
			break;
		}
	}

	BRAIN_CK(! is_fll || (dbg_num_after >= 2));
	return is_fll;
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

/*
//=============================================================================
// memap

class memap {
	public:
		
	brain*			ma_brn;

	ticket			ma_before_retract_tk;
	row<ticket>		ma_after_retract_tks;

	long			ma_tier;
	prop_signal		ma_confl;
	quanton*		ma_cho;

	row<prop_signal>	ma_dotted;
	row<neuron*>		ma_filled;

	row<long>		ma_szs_dotted;
	row<long>		ma_szs_filled;

	row<neuron*>		ma_fll_in_lv;
	row<neuron*>		ma_discarded;

	coloring		ma_save_guide_col;
	coloring		ma_find_guide_col;

	bool			ma_active;

	memap() {
		ma_active = false;
		init_memap();
	}

	~memap(){
		init_memap();
	}

	void	init_memap(brain* pt_brn = NULL){
		BRAIN_CK(! ma_active);
		
		ma_brn = pt_brn;

		ma_before_retract_tk.init_ticket();
		
		ma_tier = INVALID_TIER;
		ma_confl.init_prop_signal();
		ma_cho = NULL_PT;

		ma_dotted.clear();
		ma_filled.clear();

		ma_szs_dotted.clear();
		ma_szs_filled.clear();

		ma_fll_in_lv.clear();
		ma_discarded.clear();

		ma_save_guide_col.init_coloring(pt_brn);
		ma_find_guide_col.init_coloring(pt_brn);

		ma_active = false;
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = ma_brn);
		return the_brn;
	}

	void	reset_memap(brain& brn);

	bool	is_ma_virgin(){
		bool c2 = ! ma_before_retract_tk.is_valid();
		bool c3 = (ma_confl.is_ps_virgin());
		bool c4 = (ma_cho == NULL_PT);

		bool c5 = (ma_dotted.is_empty());
		bool c6 = (ma_filled.is_empty());

		bool c7 = (ma_szs_dotted.is_empty());
		bool c8 = (ma_szs_filled.is_empty());

		bool c9 = (ma_fll_in_lv.is_empty());
		bool c10 = (ma_discarded.is_empty());

		bool c11 = (ma_save_guide_col.is_co_virgin());
		bool c12 = (ma_find_guide_col.is_co_virgin());

		bool c13 = (ma_active == false);

		bool is_vg = (c2 && c3 && c4 && c5 && c6 && c7 && 
			c8 && c9 && c10 && c11 && c12 && c13);
	
		return is_vg;
	}

	quanton&	map_last_dotted(){
		BRAIN_CK(! ma_dotted.is_empty());
		quanton* pt_nxt_qua = ma_dotted.last().ps_quanton;
		BRAIN_CK(pt_nxt_qua != NULL_PT);

		quanton& nxt_qua = *pt_nxt_qua;
		return nxt_qua;
	}

	void	move_data_to(memap& mpp);

	void	map_record_szs();

	void	set_filled(brain& brn);

	void	map_replace_with(brain& brn, memap& mpp, dbg_call_id dbg_id);

	void	map_set_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls,
							row<neuron*>& the_neus, dima_dims& dims);

	bool	map_ck_simple_no_satisf(mem_op_t mm, brain& brn);
	void	dbg_prepare_used_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls);
	bool	dbg_ck_used_simple_no_satisf(mem_op_t mm, brain& brn);

	void	map_assemble_tees_related(mem_op_t mm, brain& brn);
	void	map_assemble_forced_sorter(mem_op_t mm, brain& brn, long first_idx);

	coloring&	map_guide_coloring(mem_op_t mm);

	void	map_inc_stab_guide(brain& brn, coloring& guide_col);
	bool 	map_prepare_mem_oper(mem_op_t mm, brain& brn);

	void	map_prepare_tees_related(mem_op_t mm, brain& brn);
	void	map_prepare_forced_sorter(mem_op_t mm, brain& brn, long first_idx);

	bool	map_find(brain& brn);
	bool	map_save(brain& brn);
	bool	map_oper(mem_op_t mm, brain& brn);
	bool	map_ck_contained_in(brain& brn, coloring& colr, dbg_call_id dbg_id);

	void	map_dbg_print(bj_ostream& os, mem_op_t mm, brain& brn);

	void	map_get_layer_neus(row<neuron*>& neus, long lyr_idx1, long lyr_idx2, 
							   bool ck_tks);


	void	get_initial_guide_coloring(brain& brn, coloring& clr, long idx_szs);
	void	get_initial_tauto_coloring(brain& brn, coloring& stab_guide_clr, 
									   coloring& base_final_clr, bool ck_tks);

	long	get_save_idx();
	long	get_find_idx();

	long 	get_last_trace_sz(){
		long t_sz = 0;
		if(! ma_szs_dotted.is_empty()){
			t_sz = ma_szs_dotted.last();
		}
		return t_sz;
	}

	long 	get_last_filled_sz(){
		long f_sz = 0;
		if(! ma_szs_filled.is_empty()){
			f_sz = ma_szs_filled.last();
		}
		return f_sz;
	}

	long 	get_trace_sz(mem_op_t mm);
	long 	get_filled_sz(mem_op_t mm);

	bool	in_last2(){
		return (ma_szs_dotted.size() == 2);
	}

	bool 	ck_last_szs(){
		BRAIN_CK(ma_szs_dotted.size() == ma_szs_filled.size());
		BRAIN_CK(get_last_trace_sz() == ma_dotted.size());
		BRAIN_CK(get_last_filled_sz() == ma_filled.size());
		return true;
	}

	bool 	ck_guide_idx(coloring& guide_col, dbg_call_id id);
	bool 	ck_map_guides(dbg_call_id dbg_id);

	bj_ostream&	print_memap(bj_ostream& os, bool from_pt = false);
};
*/

