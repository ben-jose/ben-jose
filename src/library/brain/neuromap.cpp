

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

neuromap.cpp  

neuromap class.

--------------------------------------------------------------*/

#include "brain.h"
#include "solver.h"
#include "util_funcs.h"
#include "dbg_strings_html.h"

char* neuromap::CL_NAME = as_pt_char("{neuromap}");

bool
neuromap::map_find(){
	DBG_COMMAND(4, return false);
	IF_NEVER_FIND(return false);
	BRAIN_CK(! is_na_mono());
	return map_oper(mo_find);
}
//TODO: get rid of NOT_DBG macro

bool
neuromap::map_write(){
	DBG_COMMAND(5, return false);
	IF_NEVER_WRITE(return false);
	if(is_na_mono()){ 
		DBG_PRT(102, os << " NOT_WRT_mono. na_idx=" << na_index);
		return true; 
	}
	return map_oper(mo_save);
}

void
neuromap::map_get_all_propag_ps(row<prop_signal>& all_ps){
	map_rec_get_all_propag_ps(all_ps);
	BRAIN_CK(map_dbg_ck_ord(all_ps));
}

void
neuromap::map_rec_get_all_propag_ps(row<prop_signal>& all_ps){
	if(! has_submap()){
		all_ps.clear(true, true);
	} else {
		na_submap->map_rec_get_all_propag_ps(all_ps);
	}
	
	na_propag.append_to(all_ps);
}

void
neuromap::map_get_all_quas(row_quanton_t& all_quas){
	all_quas.clear();
	
	brain& brn = get_brn();
	row<prop_signal>& all_ps = brn.br_tmp_nmp_get_all_ps;
	map_get_all_propag_ps(all_ps);
	append_all_trace_quas(all_ps, all_quas);
}

void
neuromap::map_get_all_neus(row_neuron_t& all_neus){
	all_neus.clear();
	
	brain& brn = get_brn();	
	row<prop_signal>& all_ps = brn.br_tmp_nmp_get_all_ps;
	map_get_all_propag_ps(all_ps);
	append_all_trace_neus(all_ps, all_neus);
	
	map_get_all_cov_neus(all_neus, false, false);
}
	
void
neuromap::map_get_all_cov_neus(row_neuron_t& all_neus, bool with_clear, bool skip_tail){
	if(! has_submap()){
		if(with_clear){
			all_neus.clear();
		}
	} else {
		na_submap->map_get_all_cov_neus(all_neus, with_clear, skip_tail);
	}
	
	if(skip_tail && ! has_submap()){
		return;
	}
	
	na_all_cov.append_to(all_neus);
}

quanton*	
neuromap::map_choose_quanton(){ 
	brain& brn = get_brn();
	row_quanton_t& all_quas = brn.br_tmp_choose;
	all_quas.clear();
	map_get_all_quas(all_quas);
	
	quanton* qua = NULL;
	long max_ii = all_quas.last_idx();
	for(long ii = max_ii; ii >= 0; ii--){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		quanton& qua_ii = *(all_quas[ii]);
		if(qua_ii.get_charge() == cg_neutral){
			qua = &qua_ii;
			break;
		}
	}

	BRAIN_CK_0((qua == NULL) || (qua->get_charge() == cg_neutral));
	BRAIN_CK_0((qua == NULL) || 
			(qua->qu_spin == cg_positive) || (qua->qu_spin == cg_negative));
	return qua;
}

leveldat&
neuromap::map_get_data_level(){
	brain& brn = get_brn();
	BRAIN_CK(na_orig_lv != INVALID_LEVEL);
	BRAIN_CK(brn.br_data_levels.is_valid_idx(na_orig_lv));
	leveldat& lv_dat = brn.get_data_level(na_orig_lv);
	return lv_dat;
}

void
neuromap::release_candidate(){
	BRAIN_CK(na_candidate_qua != NULL_PT);
	BRAIN_CK(na_candidate_qua->qu_candidate_nmp == this);
	
	na_candidate_qua->reset_candidate();
	
	BRAIN_CK(! nmp_is_cand(false));
	BRAIN_CK(na_orig_cho != NULL_PT);
	
	if(! na_is_head){
		return;
	}
	
	DBG_PRT(101, os << " RELEASE_CAND.\n" << this);
	
	full_release();
}

void
neuromap::full_release(){
	BRAIN_CK(! nmp_is_cand(false));
	if(nmp_is_cand(false)){
		na_is_head = true;
		return;
	}
	
	if(has_submap()){
		na_submap->full_release();
		na_submap = NULL_PT;
	}
	
	brain& brn = get_brn();
	brn.release_neuromap(*this);
}

void
coloring::save_colors_from(sort_glb& neus_srg, sort_glb& quas_srg, tee_id_t consec_kk, 
							bool unique_ccls)
{
	init_coloring(co_brn);
	
	if(consec_kk == tid_wlk_consec){
		neus_srg.stab_mutual_walk();
		quas_srg.stab_mutual_walk();
	}

	if(consec_kk != tid_qua_id){
		co_all_neu_consec = srt_row_as_colors<neuron>(neus_srg.sg_step_sortees, 
												co_neus, co_neu_colors, consec_kk, 
												unique_ccls);
	}
	co_all_qua_consec = srt_row_as_colors<quanton>(quas_srg.sg_step_sortees, 
												   co_quas, co_qua_colors, consec_kk);

	BRAIN_CK(ck_cols());
}

bool
has_diff_col_than_prev(row<long>& the_colors, long col_idx, long prv_idx){
	BRAIN_CK(prv_idx < col_idx);
	BRAIN_CK(the_colors.is_valid_idx(col_idx));
	bool diff_col = false;
	if(the_colors.is_valid_idx(prv_idx)){
		long col0 = the_colors[prv_idx];
		long col1 = the_colors[col_idx];
		BRAIN_CK(col0 >= 0);
		BRAIN_CK(col1 >= 0);
		if(col0 != col1){
			diff_col = true;
		}
	}
	return diff_col;
}

void
quanton::reset_qu_tee(){
	BRAIN_CK(qu_tee.is_unsorted());
	BRAIN_CK(qu_tee.so_related == &qu_reltee);
	
	qu_tee.reset_sort_info();
}

void
neuron::fill_mutual_sortees(brain& brn){
	row<sortee*>& neu_mates = ne_reltee.so_mates;

	neu_mates.clear();
	BRAIN_CK(ne_reltee.so_opposite == NULL_PT);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);

		if(qua.has_note1()){
			sortrel& qua_sre = qua.qu_reltee;
			row<sortee*>& qua_mates = qua_sre.so_mates;
			qua_mates.push(&ne_tee);
			neu_mates.push(&(qua.qu_tee));
		}
	}
}

void
reset_all_qu_tees(row_quanton_t& all_quas){
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);

		row<sortee*>& qua_mates = qua.qu_reltee.so_mates;
		qua_mates.clear();
	
		qua.reset_qu_tee();
	}
}

void
sort_all_qu_tees(brain& brn, row_quanton_t& all_quas, row<long>& qua_colors, 
				 sort_glb& quas_srg, coloring* pend_col, row_long_t* dbg_phi_ids)
{
	sort_id_t& quas_consec = quas_srg.sg_curr_stab_consec;
	quas_consec++;
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	if(pend_col != NULL_PT){
		pend_col->init_coloring();
	}
	if(dbg_phi_ids != NULL_PT){
		dbg_phi_ids->clear();
		BRAIN_DBG(
			brn.br_dbg_phi_id_quas.clear();
			dbg_phi_ids->fill(0, brn.br_dbg_num_phi_grps)
		);
	}

	BRAIN_DBG(long num_qu_in = quas_srg.sg_dbg_num_items);
	for(long kk = 0; kk < all_quas.size(); kk++){
		BRAIN_CK(all_quas[kk] != NULL_PT);
		quanton& qua = *(all_quas[kk]);
		quanton& opp = qua.opposite();
		
		BRAIN_CK(qua_colors.is_valid_idx(kk));

		bool inc_consec = has_diff_col_than_prev(qua_colors, kk, kk-1);
		if(inc_consec){ quas_consec++; }
		
		BRAIN_CK((pend_col != NULL_PT) || qua.qu_tee.is_unsorted());
		if(! qua.qu_tee.is_unsorted()){
			continue;
		}

		row<sortee*>& qua_mates = qua.qu_reltee.so_mates;
		row<sortee*>& opp_mates = opp.qu_reltee.so_mates;
		if((pend_col != NULL_PT) && qua_mates.is_empty() && opp_mates.is_empty()){
			pend_col->co_quas.push(&qua);
			pend_col->co_qua_colors.push(qua_colors[kk]);
			continue;
		}
		
		BRAIN_DBG(num_qu_in++);
		qua.qu_tee.sort_from(quas_srg, quas_consec);
		
		BRAIN_CK(! qua.qu_tee.is_unsorted());
		BRAIN_CK(! qua.qu_tee.is_alone());
		
		BRAIN_DBG(
			if(dbg_phi_ids != NULL_PT){
				quanton& pos_qu = *(qua.get_positon());
				long grp_idx = pos_qu.qu_dbg_phi_grp - 1;
				bool idx_ok = dbg_phi_ids->is_valid_idx(grp_idx);
				if(idx_ok && ! pos_qu.has_note1()){
					pos_qu.set_note1(brn);
					brn.br_dbg_phi_id_quas.push(&pos_qu);
					((*dbg_phi_ids)[grp_idx])++;
				}
			}
		);
	}
	
	BRAIN_DBG(
		if(dbg_phi_ids != NULL_PT){
			reset_all_note1(brn, brn.br_dbg_phi_id_quas);
			
			row_long_t& the_id = *dbg_phi_ids;
			the_id.mix_sort(cmp_long);
		}
	);
	
	
	BRAIN_CK(quas_srg.sg_dbg_num_items == num_qu_in);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
}

void
coloring::load_colors_into(sort_glb& neus_srg, sort_glb& quas_srg, 
						   dbg_call_id dbg_id, neuromap* nmp, bool calc_phi_id) 
{
	BRAIN_CK(neus_srg.has_head());
	BRAIN_CK(quas_srg.has_head());
	BRAIN_CK(ck_cols());
	
	brain& brn = get_brn();

	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row_neuron_t&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;

	BRAIN_CK(qua_colors.is_sorted(cmp_long));
	BRAIN_CK(neu_colors.is_sorted(cmp_long));

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(neus_srg.sg_dbg_num_items == 0);
	BRAIN_CK(quas_srg.sg_dbg_num_items == 0);
	
	set_all_note1(brn, all_quas);
	reset_all_qu_tees(all_quas);
	
	DBG(
		for(long aa = 0; aa < all_quas.size(); aa++){
			BRAIN_CK(all_quas[aa] != NULL_PT);
			quanton& qua = *(all_quas[aa]);
			BRAIN_CK(qua.qu_tee.is_unsorted());
		}
	);
	DBG_PRT(42, os << "COL_QUAS=" << all_quas);

	sort_id_t& neus_consec = neus_srg.sg_curr_stab_consec;
	neus_consec++;

	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	
	DBG_PRT(42, os << "COL_NEUS="; all_neus.print_row_data(os, true, "\n"); );
	for(long bb = 0; bb < all_neus.size(); bb++){
		BRAIN_CK(all_neus[bb] != NULL_PT);
		neuron& neu = *(all_neus[bb]);

		BRAIN_CK_PRT((! neu.has_tag0()), 
			DBG_PRT_ABORT(brn);
			os << " neu=" << &neu << "\n";
		);
		DBG(neu.set_tag0(brn));
		
		bool inc_consec = has_diff_col_than_prev(neu_colors, bb, bb-1);
		if(inc_consec){ neus_consec++; }

		sortee& neu_tee = neu.ne_tee;
		neu_tee.so_ccl.cc_clear(false);
		neu_tee.so_tee_consec = 0;

		neu.fill_mutual_sortees(brn); // uses note1 of quas

		row<sortee*>& neu_mates = neu.ne_reltee.so_mates;
		BRAIN_CK_PRT(! neu_mates.is_empty(), 
					os << "______________\n ABORT_DATA \n";
					brn.dbg_prt_margin(os);
					os << " dbg_id=" << dbg_id;
					os << " m_sz=" << neu_mates.size() << "\n";
					os << " nmp=" << nmp << "\n";
					os << " neu=" << &neu << "\n";
					os << " all_quas=" << all_quas << "\n";
					if(nmp != NULL_PT){
						os << "all_sub=";
						nmp->print_all_subnmp(os, true);
						os << "\n";
						nmp->dbg_prt_simple_coloring(os);
					}
		);
		if(! neu_mates.is_empty()){
			BRAIN_CK_PRT((neu_mates.size() >= 2), 
					DBG_PRT_ABORT(brn);
					os << " dbg_id=" << dbg_id;
					os << " m_sz=" << neu_mates.size();
					os << " neu=" << &neu << "\n\n";
					os << " all_quas=" << all_quas << "\n\n";
					os << " nmp=" << nmp << "\n";
			);
			BRAIN_CK(neu_tee.is_unsorted());
			neu_tee.sort_from(neus_srg, neus_consec);
		}
	}
	BRAIN_CK(neus_srg.sg_dbg_num_items == all_neus.size());

	DBG(reset_all_tag0(brn, all_neus));
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);

	coloring* pend_col = NULL_PT;
	row_long_t* dbg_phi_id = NULL_PT;
	if(nmp != NULL_PT){ 
		nmp->na_pend_col.init_coloring();
		pend_col = &(nmp->na_pend_col); 
		BRAIN_DBG(
			if(calc_phi_id && (brn.br_dbg_num_phi_grps != INVALID_NATURAL)){
				dbg_phi_id = &(nmp->na_dbg_phi_id);
			}
		);
	}
	BRAIN_CK(all_qu_have_note1(brn, all_quas));
	reset_all_note1(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	sort_all_qu_tees(brn, all_quas, qua_colors, quas_srg, pend_col, dbg_phi_id);
	
	BRAIN_CK(ck_cols());
}

void
coloring::add_coloring(coloring& clr){
	BRAIN_CK(ck_cols());
	BRAIN_CK(clr.ck_cols());
	
	brain& brn = get_brn();
	MARK_USED(brn);

	row_quanton_t&	all_quas1 = co_quas;
	row<long>&	qua_colors1 = co_qua_colors;
	row_quanton_t&	all_quas2 = clr.co_quas;
	row<long>&	qua_colors2 = clr.co_qua_colors;

	long qua_col = 0;
	if(! co_qua_colors.is_empty()){
		qua_col = co_qua_colors.last();
	}

	qua_col++;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	DBG(set_all_note1(brn, all_quas1));

	for(long aa = 0; aa < all_quas2.size(); aa++){
		BRAIN_CK(all_quas2[aa] != NULL_PT);
		quanton& qua = *(all_quas2[aa]);

		bool inc_consec = has_diff_col_than_prev(qua_colors2, aa, aa-1);
		if(inc_consec){ qua_col++; }

		BRAIN_CK_PRT((! qua.has_note1()), os << "___________________\n";
			os << " all_quas1=" << all_quas1 << "\n";
			os << " all_quas2=" << all_quas2 << "\n";
			os << " qua=" << &qua << "\n";
		);

		all_quas1.push(&qua);
		qua_colors1.push(qua_col);
	}

	DBG(reset_all_note1(brn, all_quas1));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	row_neuron_t&	all_neus1 = co_neus;
	row<long>&	neu_colors1 = co_neu_colors;
	row_neuron_t&	all_neus2 = clr.co_neus;
	row<long>&	neu_colors2 = clr.co_neu_colors;
	BRAIN_CK(all_neus2.size() == neu_colors2.size());

	long neu_col = 0;
	if(! co_neu_colors.is_empty()){
		neu_col = co_neu_colors.last();
	}

	neu_col++;

	for(long bb = 0; bb < all_neus2.size(); bb++){
		BRAIN_CK(all_neus2[bb] != NULL_PT);
		neuron& neu = *(all_neus2[bb]);

		bool inc_consec = has_diff_col_than_prev(neu_colors2, bb, bb-1);
		if(inc_consec){ neu_col++; }

		all_neus1.push(&neu);
		neu_colors1.push(neu_col);
	}

	BRAIN_CK(ck_cols());
}

void
coloring::move_co_to(coloring& col2){
	BRAIN_CK(col2.is_co_virgin());

	co_quas.move_to(col2.co_quas);
	co_qua_colors.move_to(col2.co_qua_colors);
	col2.co_all_qua_consec = co_all_qua_consec;

	co_neus.move_to(col2.co_neus);
	co_neu_colors.move_to(col2.co_neu_colors);
	col2.co_all_neu_consec = co_all_neu_consec;

	init_coloring();
}

void
coloring::copy_co_to(coloring& col2){
	BRAIN_CK(col2.is_co_virgin());

	co_quas.copy_to(col2.co_quas);
	co_qua_colors.copy_to(col2.co_qua_colors);
	col2.co_all_qua_consec = co_all_qua_consec;

	co_neus.copy_to(col2.co_neus);
	co_neu_colors.copy_to(col2.co_neu_colors);
	col2.co_all_neu_consec = co_all_neu_consec;
}

// MAP_OPER_CODE

bool
neuromap::map_oper(mem_op_t mm){
	brain& brn = get_brn();

	DBG_PRT(43, os << "MAP_OPER NMP=" << this);
	
	brn.init_mem_tmps();

	bool prep_ok = map_prepare_mem_oper(mm);

	if(! prep_ok){
		DBG_PRT(47, os << "map_oper skip (prepare == false) nmp=" << (void*)this);
		return false;
	}
	DBG_PRT(47, os << "map_oper_go nmp=" << (void*)this);

	skeleton_glb& skg = brn.get_skeleton();

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;
	
	BRAIN_CK(tmp_tauto_cnf.get_dbg_brn() != NULL_PT);
	BRAIN_CK(tmp_diff_cnf.get_dbg_brn() != NULL_PT);
	BRAIN_CK(tmp_guide_cnf.get_dbg_brn() != NULL_PT);

	tmp_diff_cnf.cf_dbg_shas.push(tmp_tauto_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_diff_cnf.cf_sha_str + "\n");
	tmp_diff_cnf.cf_dbg_shas.push(tmp_guide_cnf.cf_sha_str + "\n");

	BRAIN_CK(! tmp_tauto_cnf.has_phase_path());
	BRAIN_CK(tmp_diff_cnf.has_phase_path());
	BRAIN_CK(! tmp_guide_cnf.has_phase_path());

	bj_output_t& o_info = brn.get_out_info();
	
	DBG(
		na_dbg_nmp_mem_op = mm;
		na_dbg_tauto_min_sha_str = tmp_tauto_cnf.cf_minisha_str;
		na_dbg_tauto_sha_str = tmp_tauto_cnf.cf_sha_str;
		na_dbg_tauto_pth = tmp_tauto_cnf.get_cnf_path();
	);
	
	bool oper_ok = false;
	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();

		// This func sets cc_spot==true to tmp_diff_cnf clauses in the found vnt.
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
			ch_string fst_vpth = 
				tmp_diff_cnf.get_variant_path(skg, fst_idx, skg.in_dbg_verif());
				
			DBG_PRT(49, 
				os << "found cnf=" << bj_eol << tmp_diff_cnf << "FOUND CNF" << bj_eol
				<< "SHAS=" << bj_eol << tmp_diff_cnf.cf_dbg_shas << bj_eol
				<< "fst_vpth='" << fst_vpth << "'" << bj_eol
				<< "find_id= " << brn.br_dbg.dbg_canon_find_id);
			DBG_COMMAND(49, getchar());
			
			o_info.bjo_sub_cnf_hits++;

			na_found_in_skl = true;
			
			/*row_neuron_t& all_neus_in_vnt = na_all_neus_in_vnt_found;
			all_neus_in_vnt.clear();
			
			bool only_with_spot = true; // only clauses with cc_spot==true
			ccl_row_as<neuron>(tmp_diff_cnf.cf_clauses, all_neus_in_vnt, only_with_spot);
			*/

			// force all_neus_in_vnt to be all neus of nmp ??????
		}
	} else {
		ref_strs& diff_phdat = tmp_diff_cnf.cf_phdat;
		
		BRAIN_CK(mm == mo_save);
		BRAIN_CK(! skg.kg_dbg_save_canon || diff_phdat.has_ref());

		ch_string lk_dir = diff_phdat.lck_nam();
		int fd_lk = skg.get_write_lock(lk_dir);

		if(fd_lk != -1){
			ch_string tau_pth = tmp_tauto_cnf.get_cnf_path();
			
			ch_string sv_pth1 = tau_pth + SKG_CANON_NAME;
			ch_string sv_pth2 = tau_pth + SKG_DIFF_NAME;
			ch_string sv_pth3 = tau_pth + SKG_GUIDE_NAME;
			
			BRAIN_DBG(tmp_tauto_cnf.cf_dbg_orig_nmp = ((void*)this));
			
			SKELETON_CK(! tmp_tauto_cnf.is_empty());
			//SKELETON_CK(! tmp_diff_cnf.is_empty());
			SKELETON_CK(! tmp_guide_cnf.is_empty());
			
			SKELETON_CK(! tmp_tauto_cnf.has_phase_path());
			SKELETON_CK(tmp_diff_cnf.has_phase_path());
			SKELETON_CK(! tmp_guide_cnf.has_phase_path());

			tmp_tauto_cnf.save_cnf(skg, sv_pth1);
			oper_ok = tmp_diff_cnf.save_cnf(skg, sv_pth2);
			tmp_guide_cnf.save_cnf(skg, sv_pth3);

			if(oper_ok){
				o_info.bjo_saved_targets++;
			}
			
			//BRAIN_CK(! oper_ok || srg_forced.base_path_exists(skg));
			
			skg.drop_write_lock(lk_dir, fd_lk);

			ch_string pth1 = diff_phdat.pd_ref1_nam;
			ch_string pth2 = diff_phdat.pd_ref2_nam;

			BRAIN_DBG(bool tail_case = tmp_diff_cnf.is_empty());
			BRAIN_CK((pth1 == "") || tail_case || skg.find_skl_path(skg.as_full_path(pth1)));
			BRAIN_CK((pth2 == "") || tail_case || skg.find_skl_path(skg.as_full_path(pth2)));

			DBG_COMMAND(3, 
				if(oper_ok){
					ch_string sv1_name = skg.as_full_path(sv_pth1);
					dbg_run_satex_on(brn, sv1_name, this);
				}
			);			
		}
	}
	
	DBG_PRT(102, 
		ch_string op_ok_str = (oper_ok)?("YES"):("no");
		ch_string op_str = (na_dbg_nmp_mem_op == mo_find)?("Find"):("Write");
		op_str += op_ok_str;
		os << op_str << " na_idx=" << na_index << "\n";
	);
	DBG_PRT(70, 
		ch_string op_ok_str = (oper_ok)?("YES"):("no");
		ch_string op_str = (na_dbg_nmp_mem_op == mo_find)?("Find"):("Write");
		op_str += op_ok_str;
		
		ch_string msg_htm = map_dbg_html_data_str(op_str);;
		
		brn.dbg_update_html_cy_graph(CY_NMP_KIND, 
					&(brn.br_tmp_ini_tauto_col), msg_htm);
	);
	BRAIN_DBG(
		bool has_phi = (brn.br_dbg_num_phi_grps != INVALID_NATURAL);
		if(has_phi && ! na_dbg_phi_id.is_empty() && 
			oper_ok && (na_dbg_nmp_mem_op == mo_save))
		{
			ch_string str_id = map_dbg_get_phi_ids_str();
			
			str_str_map_t& old_ids = brn.br_dbg_phi_wrt_ids;
			bool id_was_wrt = (old_ids.find(str_id) != old_ids.end());
			if(! id_was_wrt){
				old_ids[str_id] = na_dbg_tauto_sha_str;
				DBG_PRT_COND(71, id_was_wrt, 
					os << "ADDED wrt_id=" << na_dbg_phi_id
				);
				BRAIN_CK((old_ids.find(str_id) != old_ids.end()));
			}
			DBG_PRT_COND(71, id_was_wrt, 
				os << "REPEATED_WRITE for wrt_id=" << na_dbg_phi_id;
				os << " old_sha=" << old_ids[str_id];
			);
			DBG_PRT_COND(71, ! id_was_wrt, 
				os << "FIRST_WRITE_OK for wrt_id=" << na_dbg_phi_id 
					<< "\n wrt_is_str='" << str_id << "'"
			);
		}
	);
	
	return oper_ok;
}

bool
neuromap::map_prepare_mem_oper(mem_op_t mm){
	brain& brn = get_brn();
	coloring& guide_col = na_guide_col;
	MARK_USED(guide_col);
	BRAIN_CK(brn.br_qu_tot_note0 == 0); // deducer note
	
	//BRAIN_DBG(coloring dbg_smpl_col;);
	//DBG_COMMAND(41, map_get_simple_coloring(dbg_smpl_col););
	
	DBG_PRT(41, os << "map_mem_oper=" << this);

	sort_glb& guide_ne_srg = brn.br_guide_neus_srg;
	sort_glb& guide_qu_srg = brn.br_guide_quas_srg;

	brn.all_mutual_init();
	
	if(! has_stab_guide()){
		map_set_stab_guide();
	} else {
		DBG_PRT(41, os << "HAD_STAB_GUIDE !!! nmp=" << this);
		
		guide_col.load_colors_into(guide_ne_srg, guide_qu_srg, dbg_call_1, this);
	}
	
	BRAIN_CK(! guide_col.is_co_virgin());
	BRAIN_CK(map_ck_contained_in(guide_col, dbg_call_1));

	// stab compl
	
	BRAIN_DBG(
		coloring tmp_ck_guide_col(&brn);
		tmp_ck_guide_col.save_colors_from(guide_ne_srg, guide_qu_srg, tid_wlk_consec, false);
		BRAIN_CK_PRT((tmp_ck_guide_col.dbg_equal_co_to(guide_col, &(na_pend_col.co_quas))), 
				os << "tmp_col=" << tmp_ck_guide_col << "\n";
				os << "gui_col=" << guide_col << "\n";
		);
	);

	// stab uni_colors
	
	guide_ne_srg.stab_mutual_unique(guide_qu_srg);
	
	coloring uni_guide_col(&brn);
	BRAIN_DBG(coloring old_uni_col(&brn));
	
	uni_guide_col.save_colors_from(guide_ne_srg, guide_qu_srg, tid_tee_consec);
	
	BRAIN_CK_PRT(uni_guide_col.dbg_ck_consec_col(), 
		os << "\n_____________\n ABORT_DATA\n";
		os << "\nFILE=" << brn.dbg_prt_margin(os) << "\n";
		os << "uni_guide_col=\n" << uni_guide_col << "\n";
		os << "#sub=" << na_num_submap << "\n";
		os << "has_sub=" << has_submap() << "\n";
	);
	
	if(! na_pend_col.co_quas.is_empty()){
		BRAIN_DBG(uni_guide_col.copy_co_to(old_uni_col));
		uni_guide_col.add_coloring(na_pend_col);
	}
	
	BRAIN_CK(dbg_has_simple_coloring_quas(uni_guide_col));
	BRAIN_CK(uni_guide_col.co_all_qua_consec);
	BRAIN_CK(uni_guide_col.co_all_neu_consec);
	BRAIN_CK(map_ck_contained_in(uni_guide_col, dbg_call_2));

	// set nxt_diff_phdat and ck it
	
	skeleton_glb& skg = brn.get_skeleton();

	canon_cnf& gui_cnf = guide_ne_srg.get_final_cnf(skg, PHASE_1_COMMENT, false);

	BRAIN_CK(! gui_cnf.cf_phdat.has_ref());
	
	ref_strs nxt_diff_phdat;
	row_str_t dbg_shas;

	nxt_diff_phdat.pd_ref1_nam = gui_cnf.get_ref_path();	// stab guide 
	nxt_diff_phdat.pd_ref2_nam = gui_cnf.get_lck_path();
	
	dbg_shas.push(gui_cnf.cf_sha_str + "\n");

	// FIND_GUIDE
	//if(mm == mo_find){
	if(has_submap() && (mm == mo_find)){
		instance_info& iinfo = brn.get_my_inst();
		
		ch_string find_ref = nxt_diff_phdat.pd_ref1_nam;
		ch_string pth1 = skg.as_full_path(find_ref);
		bool found1 = skg.find_skl_path(pth1, &iinfo);
		
		bj_output_t& o_info = brn.get_out_info();
		if(! found1){ 
			o_info.bjo_quick_discards++;
			DBG_PRT(39, os << "quick_find failed nmp=" << (void*)this);
			return false; 
		}
		o_info.bjo_num_finds++;
	}
	
	// stab tauto

	coloring& ini_tau_col = brn.br_tmp_ini_tauto_col;
	
	map_get_initial_tauto_coloring(uni_guide_col, ini_tau_col);

	BRAIN_CK(map_ck_contained_in(ini_tau_col, dbg_call_3));

	brn.all_mutual_init();
	
	sort_glb& tauto_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& tauto_qu_srg = brn.br_tauto_quas_srg;

	BRAIN_CK(tauto_ne_srg.ck_stab_inited());
	BRAIN_CK(tauto_qu_srg.ck_stab_inited());
	
	ini_tau_col.load_colors_into(tauto_ne_srg, tauto_qu_srg, dbg_call_3, this, true);

	tauto_ne_srg.stab_mutual_unique(tauto_qu_srg);
	
	DBG(
		na_dbg_tauto_col.save_colors_from(tauto_ne_srg, tauto_qu_srg, tid_qua_id);
		coloring pos_tau_col(&brn);
		pos_tau_col.save_colors_from(tauto_ne_srg, tauto_qu_srg, tid_tee_consec);
		bool tau_consc = dbg_all_consec(pos_tau_col.co_qua_colors);
	);
	BRAIN_CK(tau_consc); 

	BRAIN_CK_PRT((tauto_qu_srg.sg_step_all_consec), 
		brn.dbg_prt_margin(os);
		os << "______________ \n ABORT_DATA \n"; 
		brn.print_trail(os);
		os << "\n";
		os << " pend_col=" << na_pend_col << "\n\n";
		os << " old_uni=" << old_uni_col << "\n\n";
		os << " uni_col=" << uni_guide_col << "\n\n";
		os << " pre_tau_col=" << ini_tau_col << "\n\n";
		os << " pos_tau_col=" << pos_tau_col << "\n\n";
		os << " ids_tau_col=" << na_dbg_tauto_col << "\n\n";
		os << " nmp=" << this << "\n\n";
	);

	// finish prepare
	
	map_prepare_wrt_cnfs(mm, nxt_diff_phdat, dbg_shas);

	return true;
}

void
neuromap::map_prepare_wrt_cnfs(mem_op_t mm, ref_strs& nxt_diff_phdat, row_str_t& dbg_shas){
	brain& brn = get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	sort_glb& guide_ne_srg = brn.br_guide_neus_srg;
	sort_glb& tauto_ne_srg = brn.br_tauto_neus_srg;
	
	// init guide tees
	
	row<sortee*>& guide_tees = brn.br_tmp_wrt_guide_tees;
	guide_ne_srg.sg_step_sortees.move_to(guide_tees);
	
	// get final tauto cnf
	
	canon_cnf& tauto_cnf = tauto_ne_srg.get_final_cnf(skg, FINAL_COMMENT, true);

	dbg_shas.push(tauto_cnf.cf_sha_str + "\n");

	// init tauto tees
	
	row<sortee*>& tauto_tees = brn.br_tmp_wrt_tauto_tees;
	tauto_ne_srg.sg_step_sortees.move_to(tauto_tees);

	// init write ccls 

	brn.init_mem_tmps();

	row<canon_clause*>& 	tmp_tauto_ccls = brn.br_tmp_wrt_tauto_ccls;
	row<canon_clause*>& 	tmp_guide_ccls = brn.br_tmp_wrt_guide_ccls;
	row<canon_clause*>& 	tmp_diff_ccls = brn.br_tmp_wrt_diff_ccls;

	split_tees(brn, tauto_ne_srg, tauto_tees, guide_tees, tmp_guide_ccls, tmp_diff_ccls);
	tauto_cnf.cf_clauses.move_to(tmp_tauto_ccls);

	// init write cnfs

	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf;
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf;
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf;

	tmp_tauto_cnf.init_with_ccls(skg, tmp_tauto_ccls);
	tmp_diff_cnf.init_with_ccls(skg, tmp_diff_ccls);
	tmp_guide_cnf.init_with_ccls(skg, tmp_guide_ccls);
	
	DBG_PRT_COND(112, (tmp_tauto_cnf.cf_minisha_str == "8b9adc25fa"), 
		os << " 8b9adc25fa nmp=" << this
	);
	
	tmp_tauto_cnf.cf_diff_minisha_str = tmp_diff_cnf.cf_minisha_str;
	tmp_diff_cnf.cf_taut_minisha_str = tmp_tauto_cnf.cf_minisha_str;

	tmp_diff_cnf.cf_phdat = nxt_diff_phdat;
	dbg_shas.move_to(tmp_diff_cnf.cf_dbg_shas);

	BRAIN_CK(tmp_tauto_ccls.is_empty());
	BRAIN_CK(tmp_diff_ccls.is_empty());

	// final checks

	row<canon_clause*>& ccls_tauto_cnf = tmp_tauto_cnf.cf_clauses;
	row<canon_clause*>& ccls_diff_cnf = tmp_diff_cnf.cf_clauses;

	MARK_USED(ccls_tauto_cnf);
	MARK_USED(ccls_diff_cnf);

	DBG_PRT_COND(48, (mm == mo_save), map_dbg_print(os, mm));
	DBG_PRT_COND(48, (mm == mo_save), 
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
}

void
neuromap::map_get_ini_guide_col(coloring& clr)
{
	brain& brn = get_brn();
	row<prop_signal>& dtrace = na_propag;
	MARK_USED(brn);
	
	clr.init_coloring(&brn);
	
	DBG_PRT_WITH(108, brn, os << " all_ps=" << dtrace);

	row_quanton_t&	all_quas = clr.co_quas;
	row<long>&	qua_colors = clr.co_qua_colors;

	row_neuron_t&	all_neus = clr.co_neus;
	row<long>&	neu_colors = clr.co_neu_colors;

	all_quas.clear();
	all_neus.clear();
	qua_colors.clear();
	neu_colors.clear();
	
	long col_qu = 1;
	long col_ne = 0;
	bool inc_col_ne_pend = true;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	for(long ii = 0; ii < dtrace.size(); ii++){
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
	
		if(inc_col){ col_qu++; }
		if(inc_col && ! inc_col_ne_pend){ inc_col_ne_pend = true; }
		
		BRAIN_CK(q_sig1.ps_quanton != NULL_PT);

		quanton& qua = *(q_sig1.ps_quanton);
		quanton& opp = qua.opposite();

		BRAIN_CK(! qua.has_note1());
		DBG(qua.set_note1(brn));
		
		all_quas.push(&qua);
		qua_colors.push(col_qu);

		all_quas.push(&opp);
		qua_colors.push(col_qu);

		if(q_sig1.ps_source != NULL_PT){
			neuron& neu = *(q_sig1.ps_source);
			BRAIN_CK(neu.ne_original);

			if(inc_col_ne_pend){ 
				inc_col_ne_pend = false;
				col_ne++; 
			}
			
			all_neus.push(&neu); 
			neu_colors.push(col_ne);
			
			BRAIN_DBG(bool is_bug_115 = ((na_index == 4) &&
				(neu.ne_index == 154));
			);
			DBG_PRT_COND(115, is_bug_115,
				os << " BUG." << " nmp=" << this << "\n";
				os << " neu=" << &neu << "\n";
			);
		}
	}

	DBG(reset_all_its_note1(brn, all_quas));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(clr.ck_cols());
}

bool
neuromap::has_stab_guide(){
	bool h_g =  ! na_guide_col.is_co_virgin();
	return h_g;
}

void
neuromap::map_stab_guide_col(){
	brain& brn = get_brn();
	coloring& guide_col = na_guide_col;
	
	DBG(long old_quas_sz = guide_col.co_quas.size());
	DBG(long old_neus_sz = guide_col.co_neus.size());

	brn.all_mutual_init();
	
	sort_glb& neus_srg = brn.br_guide_neus_srg;
	sort_glb& quas_srg = brn.br_guide_quas_srg;
	
	DBG_PRT(42, os << " bef_load_col neus_srg=" << neus_srg);
	DBG_PRT(42, os << " bef_load_col quas_srg=" << quas_srg);
	DBG_PRT(42, os << " bef_load_col qui_col=" << guide_col);
	guide_col.load_colors_into(neus_srg, quas_srg, dbg_call_4, this);
	DBG_PRT(42, os << " aft_load_col qui_col=" << guide_col);
	DBG_PRT(42, os << " aft_load_col neus_srg=" << neus_srg);
	DBG_PRT(42, os << " aft_load_col quas_srg=" << quas_srg);
	
	BRAIN_DBG(
		coloring tmp_ck_guide_col(&brn);
		tmp_ck_guide_col.save_colors_from(neus_srg, quas_srg, tid_wlk_consec, false);
		bool eq1 = tmp_ck_guide_col.dbg_equal_co_to(guide_col, &(na_pend_col.co_quas));
		BRAIN_CK_PRT(eq1, 
				os << "_______________\n";
				brn.dbg_prt_margin(os);
				os << "ABORT_DATA\n";
				os << "pend_quas=" << na_pend_col.co_quas << "\n";
				os << "tmp_col=" << tmp_ck_guide_col << "\n";
				os << "gui_col=" << guide_col << "\n";
		);
	);
	
	// THE STAB FOR THIS NMP BLOCK
	neus_srg.stab_mutual(quas_srg);
	
	DBG_PRT(42, os << " aft_stab neus_srg=" << neus_srg);
	DBG_PRT(42, os << " aft_stab quas_srg=" << quas_srg);
	
	DBG_PRT(42, os << " bef_guide_col=" << guide_col);
	guide_col.save_colors_from(neus_srg, quas_srg, tid_tee_consec, false);
	if(! na_pend_col.co_quas.is_empty()){
		guide_col.add_coloring(na_pend_col);
	}
	DBG_PRT(42, os << " AFT_guide_col=" << guide_col);
	
	BRAIN_CK(old_quas_sz >= guide_col.co_quas.size());
	BRAIN_CK_PRT((old_neus_sz == guide_col.co_neus.size()),
			os << "_____\n old_neus_sz=" << old_neus_sz << " nw_sz=" 
			<< guide_col.co_neus.size();
	);
}

void
coloring::dbg_set_tmp_colors(bool skip_all_n1){
#ifdef FULL_DEBUG
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas1 = co_quas;
	row<long>&	qua_colors1 = co_qua_colors;

	long tmp1_col = 1;
	long prv_aa = -1;
	for(long aa = 0; aa < all_quas1.size(); aa++){
		BRAIN_CK(all_quas1[aa] != NULL_PT);
		quanton& qua = *(all_quas1[aa]);
		
		if(skip_all_n1 && qua.has_note1()){
			continue;
		}
		
		bool inc_consec = has_diff_col_than_prev(qua_colors1, aa, prv_aa);
		if(inc_consec){ tmp1_col++; }
		prv_aa = aa;
		
		BRAIN_CK(qua.qu_tmp_col == INVALID_COLOR);
		qua.qu_tmp_col = tmp1_col;
		BRAIN_CK(qua.qu_tmp_col != INVALID_COLOR);
	}

	row_neuron_t&	all_neus1 = co_neus;
	row<long>&	neu_colors1 = co_neu_colors;

	for(long bb = 0; bb < all_neus1.size(); bb++){
		BRAIN_CK(all_neus1[bb] != NULL_PT);
		neuron& neu = *(all_neus1[bb]);

		BRAIN_CK(neu.ne_tmp_col == INVALID_COLOR);
		neu.ne_tmp_col = neu_colors1[bb];
		BRAIN_CK(neu.ne_tmp_col != INVALID_COLOR);
	}

	BRAIN_CK(ck_cols());
#endif
}

void
coloring::dbg_reset_tmp_colors(bool skip_all_n1){
#ifdef FULL_DEBUG
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas1 = co_quas;

	for(long aa = 0; aa < all_quas1.size(); aa++){
		BRAIN_CK(all_quas1[aa] != NULL_PT);
		quanton& qua = *(all_quas1[aa]);
		
		if(skip_all_n1 && qua.has_note1()){
			continue;
		}
		
		BRAIN_CK(qua.qu_tmp_col != INVALID_COLOR);
		qua.qu_tmp_col = INVALID_COLOR;
	}

	row_neuron_t&	all_neus1 = co_neus;

	for(long bb = 0; bb < all_neus1.size(); bb++){
		BRAIN_CK(all_neus1[bb] != NULL_PT);
		neuron& neu = *(all_neus1[bb]);

		BRAIN_CK(neu.ne_tmp_col != INVALID_COLOR);
		neu.ne_tmp_col = INVALID_COLOR;
	}

	BRAIN_CK(ck_cols());
#endif
}

bool
coloring::dbg_equal_to_tmp_colors(bool skip_all_n1){
	bool eq_all = true;
#ifdef FULL_DEBUG
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas1 = co_quas;
	row<long>&	qua_colors1 = co_qua_colors;

	long tmp1_col = 1;
	long prv_aa = -1;
	for(long aa = 0; aa < all_quas1.size(); aa++){
		BRAIN_CK(all_quas1[aa] != NULL_PT);
		quanton& qua = *(all_quas1[aa]);
		
		if(skip_all_n1 && qua.has_note1()){
			continue;
		}
		
		bool inc_consec = has_diff_col_than_prev(qua_colors1, aa, prv_aa);
		if(inc_consec){ tmp1_col++; }
		prv_aa = aa;
		
		long col_aa = tmp1_col;
		BRAIN_CK(col_aa != INVALID_COLOR);
		
		if(qua.qu_tmp_col != col_aa){
			DBG_PRT(67, os << " NOT_EQ_COL qua=" << &qua << " aa=" << aa;
				os << " c1=" << qua.qu_tmp_col;
				os << " c2=" << qua_colors1[aa] << "\n";
			);
			eq_all = false;
			break;
		}
	}
	
	if(! eq_all){
		return false;
	}

	row_neuron_t&	all_neus1 = co_neus;
	row<long>&	neu_colors1 = co_neu_colors;

	for(long bb = 0; bb < all_neus1.size(); bb++){
		BRAIN_CK(all_neus1[bb] != NULL_PT);
		neuron& neu = *(all_neus1[bb]);

		if(neu.ne_tmp_col != neu_colors1[bb]){
			DBG_PRT(67, os << " NOT_EQ_COL neu=" << &neu << " bb=" << bb);
			eq_all = false;
			break;
		}
	}

	BRAIN_CK(ck_cols());
	
#endif
	return eq_all;
}

void
neuromap::map_set_stab_guide(){
	if(has_stab_guide()){
		return;
	}
	if(has_submap()){
		na_submap->map_set_stab_guide();
	}
	map_init_stab_guide();
	BRAIN_CK(has_stab_guide());
}

void
neuromap::map_get_all_upper_quas(row_quanton_t& all_upper_quas){
	brain& brn = get_brn();
	MARK_USED(brn);

	all_upper_quas.clear();
	
	row_quanton_t& all_qua = brn.br_tmp_nmp_quas_for_upper_qu;
	map_get_all_quas(all_qua);

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	set_all_note1(brn, all_qua);
	
	row_neuron_t& all_neus = brn.br_tmp_nmp_neus_for_upper_qu;
	all_neus.clear();
	map_get_all_neus(all_neus);
	
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		append_all_not_note1(brn, neu.ne_fibres, all_upper_quas);
	}
	
	reset_all_note1(brn, all_qua);
	reset_all_note1(brn, all_upper_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(map_ck_all_upper_quas(all_upper_quas));
}

bool 
neuromap::map_ck_all_upper_quas(row_quanton_t& all_quas){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		BRAIN_CK_PRT((qua.qlevel() < na_orig_lv), 
			os << "\n_____________\n ABORT_DATA\n";
			brn.dbg_prt_margin(os);
			brn.print_trail(os);
			os << this << "\n";
			os << " qua=" << &qua;
			os << " na_cho=" << na_orig_cho;
			os << " na_orig_lv=" << na_orig_lv;
			os << " qua_tk=" << qua.qu_charge_tk;
			if(na_orig_cho != NULL_PT){
				os << " cho_tk=" << na_orig_cho->qu_charge_tk;
			}
		);
	}
#endif
return true;
}

bool
neuromap::map_ck_all_quas(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	row_quanton_t& all_quas = brn.br_tmp_ck_all_orig;
	all_quas.clear();
	map_get_all_quas(all_quas);
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		
		BRAIN_CK_PRT((! qua.has_note1()), os << "̣̣̣̣̣_____________________\n";
			brn.dbg_prt_margin(os);
			os << " na_propag=" << na_propag << "\n";
			if(has_submap()){
				os << " sub na_propag=" << na_submap->na_propag << "\n";
			}
			os << " all_quas=" << all_quas << "\n";
		);
		qua.set_note1(brn);
	}
	
	reset_all_note1(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
#endif
	return true;
}

void
coloring::reset_as_simple_col(){
	co_qua_colors.clear();
	co_qua_colors.fill(1, co_quas.size());
	
	co_neu_colors.clear();
	co_neu_colors.fill(1, co_neus.size());
	
	BRAIN_CK(ck_cols());
}

void
neuromap::dbg_prt_simple_coloring(bj_ostream& os){
#ifdef FULL_DEBUG
	coloring ss_col;
	map_get_simple_coloring(ss_col);
	os << " nmp=" << this << "\n"; 
	os << " s_col=\n" << ss_col;
	os.flush();
#endif
}

bool
neuromap::dbg_has_simple_coloring_quas(coloring& clr){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	coloring ss_col;
	map_get_simple_coloring(ss_col);

	BRAIN_CK(ss_col.ck_cols());
	BRAIN_CK(clr.ck_cols());
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	bool c1 = dbg_equal_positons(brn, ss_col.co_quas, clr.co_quas, false);
		
	DBG_PRT_COND(DBG_ALL_LVS, (! c1), 
			os << "ABORT_DATA\n";
			os << " nmp=" << this << "\n";
			os << " sz1." << ss_col.co_quas.size() 
				<< ".ss_quas= " << ss_col.co_quas << "\n\n";
			os << " sz2." << clr.co_quas.size() 
				<< ".cl_quas= " << clr.co_quas << "\n\n";
	);
	
	BRAIN_CK(c1);
	
	BRAIN_CK(ss_col.ck_cols());
	BRAIN_CK(clr.ck_cols());
#endif
	return true;
}

bool
dbg_equal_positons(brain& brn, row_quanton_t& quas1, row_quanton_t& quas2, bool skip_all_n1)
{
	bool all_eq = false;
#ifdef FULL_DEBUG
	// this function uses both note0 (set internally) & note1 (set externally)
	row_quanton_t& all_pos1 = brn.br_tmp_eq_nmp_all_pos1;
	dbg_get_all_positons(quas1, all_pos1, skip_all_n1); // note1
	
	row_quanton_t& all_pos2 = brn.br_tmp_eq_nmp_all_pos2;
	dbg_get_all_positons(quas2, all_pos2, skip_all_n1); // note1
	
	BRAIN_CK(brn.br_qu_tot_note0 == 0);
	bool c1 = same_quantons_note0(brn, all_pos1, all_pos2); // note0
	bool c2 = same_quantons_note0(brn, all_pos2, all_pos1); // note0
	BRAIN_CK(brn.br_qu_tot_note0 == 0);
	
	all_eq = (c1 && c2);
#endif
	return all_eq;
}

bool
coloring::dbg_equal_nmp_to(coloring& col2, bool skip_all_n1){
	bool all_eq =  false;
#ifdef FULL_DEBUG
	BRAIN_CK(ck_cols());
	BRAIN_CK(col2.ck_cols());
	BRAIN_CK(co_brn == col2.co_brn);
	
	brain& brn = get_brn() ;
	
	BRAIN_CK(brn.br_qu_tot_note0 == 0);
	BRAIN_CK(brn.br_ne_tot_tag5 == 0);
	
	bool c1 = dbg_equal_positons(brn, co_quas, col2.co_quas, skip_all_n1);
	bool c2 = (same_neurons_tag5(brn, co_neus, col2.co_neus) == NULL_PT);
	bool c3 = (same_neurons_tag5(brn, col2.co_neus, co_neus) == NULL_PT);
	
	all_eq = (c1 && c2 && c3);
	
	BRAIN_CK(brn.br_qu_tot_note0 == 0);
	BRAIN_CK(brn.br_ne_tot_tag5 == 0);
	
	BRAIN_CK(ck_cols());
	BRAIN_CK(col2.ck_cols());
#endif
	return all_eq;
}

void
append_missing_opps(brain& brn, row_quanton_t& all_quas){
	BRAIN_CK(brn.br_qu_tot_note6 == 0);
	set_all_binote6(brn, all_quas);
	
	for(long aa = all_quas.last_idx(); aa >= 0; aa--){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		quanton& opp = qua.opposite();
		if(! opp.is_note6()){
			opp.set_binote6(brn);
			all_quas.push(&opp);
		}
	}
	
	reset_all_its_note6(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note6 == 0);
}

void
neuromap::map_get_simple_coloring(coloring& clr){
	brain& brn = get_brn();
	clr.init_coloring(&brn);

	row_quanton_t& all_quas = clr.co_quas;
	all_quas.clear();
	map_get_all_quas(all_quas);
	append_missing_opps(brn, all_quas);
	
	row_neuron_t& all_neus = clr.co_neus;
	all_neus.clear();
	map_get_all_neus(all_neus);
	
	clr.reset_as_simple_col();
}

bool
coloring::dbg_equal_co_to(coloring& col2, row_quanton_t* skip_quas){
	bool all_eq = false;
#ifdef FULL_DEBUG
	BRAIN_REL_CK(co_brn != NULL_PT);
	brain& brn = get_brn();
	
	BRAIN_CK(ck_cols());
	BRAIN_CK(col2.ck_cols());
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(brn.br_ne_tot_tag5 == 0);
	BRAIN_CK(co_brn == col2.co_brn);
	
	bool skip_all_n1 = (skip_quas != NULL_PT);
	
	if(skip_quas != NULL_PT){ 
		set_all_note1(brn, *skip_quas); 
	}
	
	dbg_set_tmp_colors(skip_all_n1);
	bool c1 = col2.dbg_equal_to_tmp_colors(skip_all_n1);
	dbg_reset_tmp_colors(skip_all_n1);

	bool c2 = dbg_equal_nmp_to(col2, skip_all_n1);
	
	all_eq = (c1 && c2);
	
	if(skip_quas != NULL_PT){ 
		reset_all_note1(brn, *skip_quas); 
	}
	
	BRAIN_CK(brn.br_ne_tot_tag5 == 0);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(ck_cols());
	BRAIN_CK(col2.ck_cols());
#endif
	return all_eq;
}

void
dbg_get_all_positons(row_quanton_t& all_quas, row_quanton_t& all_pos, bool skip_all_n1)
{
#ifdef FULL_DEBUG
	//BRAIN_CK(! skip_all_n1);
	all_pos.clear();
	for(long ii = 0; ii < all_quas.size(); ii++){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		quanton* pos = all_quas[ii]->get_positon();
		BRAIN_CK(pos != NULL_PT);
		
		if(skip_all_n1 && pos->has_note1()){
			continue;
		}
		
		all_pos.push(pos);
	}
#endif
}

bool	dbg_all_consec(row<long>& rr1){
	bool all_consc = true;
#ifdef FULL_DEBUG
	long lst_val = -1;
	for(long aa = 0; aa < rr1.size(); aa++){
		long vv = rr1[aa];
		BRAIN_CK(vv > 0);
		DBG_COND_COMM((vv == lst_val), os << "dbg_all_consec FAILED !!!. aa=" << aa);
		all_consc = all_consc && (vv != lst_val);
		lst_val = vv;
	}
#endif
	return all_consc;
}

bool
coloring::dbg_ck_consec_col(){
	bool all_ok = true;
#ifdef FULL_DEBUG
	bool c1 = dbg_all_consec(co_qua_colors);
	bool c2 = dbg_all_consec(co_neu_colors);
	all_ok = (c1 && c2);
#endif
	return all_ok;
}

void
neuromap::map_init_stab_guide(){
	brain& brn = get_brn();

	coloring ini_guide_col(&brn);

	map_get_initial_guide_coloring(ini_guide_col);
	
	coloring& guide_col = na_guide_col;
	guide_col.init_coloring(&brn);
	
	DBG_PRT(42, os << "ini_col=\n" << ini_guide_col << "\nguide_col=\n" << guide_col
		<< "\nhas_sub=" << (has_submap())
	);
	
	if(has_submap()){
		na_submap->na_guide_col.copy_co_to(guide_col);
	}
	
	guide_col.add_coloring(ini_guide_col);
	
	map_stab_guide_col();
	
	DBG_PRT_COND(115, (na_index == 4), 
		os << " guide=" << na_guide_col << "\n";
		os << " full_nmp_4_BUG=\n" << *this << "\n";
	);
	
}

void
neuromap::map_get_initial_tauto_coloring(coloring& prv_clr, coloring& tauto_clr)
{
	brain& brn = get_brn();
	BRAIN_CK(&prv_clr != &tauto_clr);
	
	tauto_clr.init_coloring(&brn);

	if(! has_submap()){
		map_get_simple_coloring(tauto_clr);
		BRAIN_CK(tauto_clr.ck_cols());
		return;
	}
	
	coloring& tmp_co = brn.br_tmp_tauto_col;
	tmp_co.init_coloring(&brn);
	
	row_neuron_t& all_co_neus = tmp_co.co_neus;
	
	all_co_neus.clear();
	map_get_all_cov_neus(all_co_neus, true, true);
	tmp_co.co_neu_colors.fill(1, all_co_neus.size());

	prv_clr.copy_co_to(tauto_clr);
	tauto_clr.add_coloring(tmp_co);
	
	BRAIN_CK(tauto_clr.ck_cols());
}

// FIND_GUIDE

void
split_tees(brain& brn, sort_glb& srg, row<sortee*>& sorted_tees, row<sortee*>& sub_tees, 
		row<canon_clause*>& ccls_in, row<canon_clause*>& ccls_not_in)
{
	ccls_in.clear();
	ccls_not_in.clear();

	long d_sz = sorted_tees.size() - sub_tees.size();
	ccls_not_in.set_cap(d_sz);

	ccls_in.set_cap(sub_tees.size());

	BRAIN_CK(srg.sg_tot_ss_marks == 0);

	// result in all_ccls
	for(long aa = 0; aa < sub_tees.size(); aa++){
		BRAIN_CK(sub_tees[aa] != NULL_PT);
		sortee& rmv1 = *(sub_tees[aa]);
		if(! rmv1.has_vessel()){
			continue;
		}
		
		sorset& ss1 = rmv1.get_vessel();
		if(! ss1.ss_mark){
			ss1.set_ss_mark(srg);
		}
	}

	long last_tee_id = INVALID_NATURAL;
	sorset* last_ss = NULL_PT;

	for(long bb = 0; bb < sorted_tees.size(); bb++){
		BRAIN_CK(sorted_tees[bb] != NULL_PT);
		sortee& tee1 = *(sorted_tees[bb]);
		
		if(tee1.so_tee_consec != last_tee_id){
			BRAIN_CK(last_tee_id < tee1.so_tee_consec);
			last_tee_id = tee1.so_tee_consec;

			BRAIN_CK(tee1.has_vessel());
			sorset& ss2 = tee1.get_vessel();
			if(ss2.ss_mark){
				if(last_ss != &ss2){
					ccls_in.push(&(tee1.so_ccl));
				}
			} else {
				ccls_not_in.push(&(tee1.so_ccl));
			}
			last_ss = &ss2;
		}
	}

	for(long cc = 0; cc < sub_tees.size(); cc++){
		BRAIN_CK(sub_tees[cc] != NULL_PT);
		sortee& rmv1 = *(sub_tees[cc]);
		if(! rmv1.has_vessel()){
			continue;
		}
		
		sorset& ss1 = rmv1.get_vessel();
		if(ss1.ss_mark){
			ss1.reset_ss_mark(srg);
		}
	}

	BRAIN_CK(srg.sg_tot_ss_marks == 0);
}

void
neuromap::map_get_initial_guide_coloring(coloring& clr){
	BRAIN_CK(&na_guide_col != &clr);

	if(has_submap()){
		map_get_ini_guide_col(clr);
	} else {
		map_get_simple_coloring(clr);
	}
}

void
neuromap::nmp_fill_all_upper_covs(){
	if(na_fill_cov_ok){
		return;
	}
	if(has_submap()){
		na_submap->nmp_fill_all_upper_covs();
	}
	nmp_fill_upper_covs();
	na_fill_cov_ok = true;
}

void
neuromap::nmp_fill_upper_covs(){
	for(long aa = 0; aa < na_all_cov.size(); aa++){
		neuron* neu = na_all_cov[aa];
		BRAIN_CK(neu != NULL_PT);
		neu->set_ne_cand_tk(na_candidate_tk);
	}
	for(long aa = 0; aa < na_all_centry.size(); aa++){
		cov_entry& cty = na_all_centry[aa];
		neuromap* nmp = cty.ce_nmp;
		neuron* neu = cty.ce_neu;
		
		BRAIN_CK(nmp != NULL_PT);
		BRAIN_CK(neu != NULL_PT);
		BRAIN_CK(nmp->na_dbg_cand_sys);
		BRAIN_CK(nmp->na_dbg_creating);
		BRAIN_CK(nmp->nmp_is_cand());
		
		neu->set_ne_cand_tk(nmp->na_candidate_tk);
		nmp->na_all_cov.push(neu);
	}
}

