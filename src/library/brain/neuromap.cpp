

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
	if(na_found_in_skl != mf_invalid){
		if(na_found_in_skl == mf_found){
			return true;
		}
		BRAIN_CK(na_found_in_skl == mf_not_found);
		return false;
	}
	bool op_resp = map_oper(mo_find);
	return op_resp;
}
//TODO: get rid of NOT_DBG macro

void
dbg_aux_prt_neus(bj_ostream& os, ch_string tit, row_neuron_t& all_neus, ticket& nmp_tk){
	os << "\n" << tit << "=\n";
	os << "[";
	for(long aa = 0; aa < all_neus.size(); aa++){
		neuron* neu = all_neus[aa];
		BRAIN_CK(neu != NULL_PT);
		bool to_wrt = ! neu->ne_to_wrt_tk.is_older_than(nmp_tk);
		os << neu->ne_index;
		if(to_wrt){ os << "|"; } else { os << "."; }
	}
	os << "]";
}

bool
neuromap::map_write(bool force_full){
	DBG_COMMAND(5, return false);
	IF_NEVER_WRITE(return false);
	
	BRAIN_CK(is_ticket_eq(na_candidate_tk, na_dbg_candidate_tk));
	nmp_reset_write();
	//force_full = true; // when debugging
	if(! force_full){
		nmp_update_all_to_write(na_candidate_tk);
		BRAIN_CK(na_upd_to_write);
	}
	bool w_ok = false;
	if(is_na_mono()){ 
		BRAIN_CK(has_submap());
		w_ok = na_submap->map_oper(mo_save);
	} else {
		w_ok = map_oper(mo_save);
	}
	return w_ok;
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
neuromap::map_get_all_neus(row_neuron_t& all_neus, bool only_found, mem_op_t mm){
	all_neus.clear();
	
	brain& brn = get_brn();	
	row<prop_signal>& all_ps = brn.br_tmp_nmp_get_all_ps;
	map_get_all_propag_ps(all_ps);
	append_all_trace_neus(all_ps, all_neus);
	
	if(only_found){
		na_all_found.append_to(all_neus);
	} else {
		map_get_all_cov_neus(all_neus, false, false, mm);
	}
}
	
void
neuromap::map_get_all_cov_neus(row_neuron_t& all_neus, bool with_clear, bool skip_tail, 
							   mem_op_t mm)
{
	if(! has_submap()){
		if(with_clear){
			all_neus.clear();
		}
	} else {
		na_submap->map_get_all_cov_neus(all_neus, with_clear, skip_tail, mm);
	}
	
	if(skip_tail && ! has_submap()){
		return;
	}
	
	if(na_upd_to_write && (mm == mo_save)){
		na_to_write.append_to(all_neus);
		return;
	}
	
	na_all_cov.append_to(all_neus);
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
	BRAIN_CK(is_ticket_eq(na_candidate_tk, na_dbg_candidate_tk));
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
	BRAIN_CK(is_ticket_eq(na_candidate_tk, na_dbg_candidate_tk));
	BRAIN_CK(! nmp_is_cand(false));

	DBG_PRT(53, os << " RELEASED nmp=" << dbg_na_id() << "\n";
		os << " phi_id=" << na_dbg_phi_id << "\n";
	);
	
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
		co_all_neu_consec = srt_row_as_colors<neuron>(neus_srg, neus_srg.sg_step_sortees, 
												co_neus, co_neu_colors, consec_kk, 
												unique_ccls);
	}
	co_all_qua_consec = srt_row_as_colors<quanton>(quas_srg, quas_srg.sg_step_sortees, 
												   co_quas, co_qua_colors, consec_kk, false);

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
		quanton& opp = qua.opposite();

		row<sortee*>& qua_mates = qua.qu_reltee.so_mates;
		qua_mates.clear();
	
		row<sortee*>& opp_mates = opp.qu_reltee.so_mates;
		opp_mates.clear();
		
		qua.reset_qu_tee();
		opp.reset_qu_tee();
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

	DBG_PRT_WITH(43, brn, 
		os <<  " ++++++++++++++++++++++++++++++++++++++++++++++++++++. \n";
		os <<  " sort_qu_tees. \n";
		os << " all_quas=" << all_quas;
	);
	
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
			DBG_PRT_WITH(43, brn, os << " ! is_unsorted qua=" << &qua);
			continue;
		}

		row<sortee*>& qua_mates = qua.qu_reltee.so_mates;
		row<sortee*>& opp_mates = opp.qu_reltee.so_mates;
		if((pend_col != NULL_PT) && qua_mates.is_empty() && opp_mates.is_empty()){
			pend_col->co_quas.push(&qua);
			pend_col->co_qua_colors.push(qua_colors[kk]);
			//pend_col->co_qua_colors.push(1);
			DBG_PRT_WITH(43, brn, os << " pend qua=" << &qua);
			DBG_PRT_WITH(105, brn, os << " pend qua=" << &qua);
			continue;
		}
		DBG_PRT_COND_WITH(43, brn, (qua_mates.is_empty() || opp_mates.is_empty()),
			os <<  " HALF pend!!!. qua=" << &qua << "\n";
			os << " qua_mates=\n"; qua_mates.print_row_data(os, true, "\n");
			os << " \n";
			os << " opp_mates=\n"; opp_mates.print_row_data(os, true, "\n");
			os << " \n";
		);
		
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
	
	BRAIN_DBG(
		for(long aa = 0; aa < all_quas.size(); aa++){
			BRAIN_CK(all_quas[aa] != NULL_PT);
			quanton& qua = *(all_quas[aa]);
			quanton& opp = qua.opposite();
			BRAIN_CK(qua.qu_tee.is_unsorted());
			
			row<sortee*>& qua_mates = qua.qu_reltee.so_mates;
			row<sortee*>& opp_mates = opp.qu_reltee.so_mates;
			BRAIN_CK(qua_mates.is_empty());
			BRAIN_CK(opp_mates.is_empty());
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
			if(nmp != NULL_PT){
				os << " nmp=\n" << nmp << "\n";
			}
			os << " all_neus=\n";
			all_neus.print_row_data(os, true, "\n");
			os << " \n\n";
			os << " neu=" << &neu << "\n";
			os << " is_src=" << neu.is_ne_source() << "\n";
					os << " dbg_id=" << dbg_id << "\n";
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
	
	DBG_PRT_COND(105, ((nmp != NULL_PT) && ! nmp->na_pend_col.co_quas.is_empty()), 
		 os << " nmp_has_pend=" << nmp->dbg_na_id() << "\n";
		 os << " pend_col=" << nmp->na_pend_col << "\n";
		 os << "COL_NEUS="; all_neus.print_row_data(os, true, "\n");
	);
	
	BRAIN_CK(ck_cols());
}

void
coloring::add_coloring(coloring& clr, neuromap* dbg_nmp){
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
	BRAIN_DBG(set_all_note1(brn, all_quas1));

	for(long aa = 0; aa < all_quas2.size(); aa++){
		BRAIN_CK(all_quas2[aa] != NULL_PT);
		quanton& qua = *(all_quas2[aa]);

		bool inc_consec = has_diff_col_than_prev(qua_colors2, aa, aa-1);
		if(inc_consec){ qua_col++; }

		BRAIN_CK_PRT((! qua.has_note1()), os << "___________________\n";
			os << " all_quas1=" << all_quas1 << "\n";
			os << " all_quas2=" << all_quas2 << "\n";
			os << " qua=" << &qua << "\n";
			os << " dbg_nmp=" << dbg_nmp << "\n";
		);

		all_quas1.push(&qua);
		qua_colors1.push(qua_col);
	}

	BRAIN_DBG(reset_all_note1(brn, all_quas1));
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

	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	BRAIN_DBG(set_all_tag0(brn, all_neus1));
	
	for(long bb = 0; bb < all_neus2.size(); bb++){
		BRAIN_CK(all_neus2[bb] != NULL_PT);
		neuron& neu = *(all_neus2[bb]);

		bool inc_consec = has_diff_col_than_prev(neu_colors2, bb, bb-1);
		if(inc_consec){ neu_col++; }
		
		BRAIN_CK_PRT(! neu.has_tag0(),
			DBG_PRT_ABORT(brn);
			os << " NMP=\n";
			if(dbg_nmp != NULL_PT){
				os << *dbg_nmp << "\n";
				//coloring dbg_smpl_col;
				//dbg_nmp->map_get_simple_coloring(dbg_smpl_col););
				//os << " simpl_colo=\n" << dbg_smpl_col << "\n";
			}
			os << " all_neus1=\n";
			all_neus1.print_row_data(os, true, "\n");
			os << "\n";
			os << " all_neus2=\n";
			all_neus2.print_row_data(os, true, "\n");
			os << "\n";
			os << " NEU=" << &neu << "\n";
		);
		BRAIN_DBG(neu.set_tag0(brn));

		all_neus1.push(&neu);
		neu_colors1.push(neu_col);
	}

	BRAIN_DBG(reset_all_tag0(brn, all_neus1));
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	BRAIN_CK(ck_cols());
}

void
coloring::join_coloring(coloring& clr){
	BRAIN_CK(ck_cols());
	BRAIN_CK(clr.ck_cols());
	
	brain& brn = get_brn();

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
	set_all_note1(brn, all_quas1);

	for(long aa = 0; aa < all_quas2.size(); aa++){
		BRAIN_CK(all_quas2[aa] != NULL_PT);
		quanton& qua = *(all_quas2[aa]);

		bool inc_consec = has_diff_col_than_prev(qua_colors2, aa, aa-1);
		if(inc_consec){ qua_col++; }

		if(qua.has_note1()){
			continue;
		}
		qua.set_note1(brn);

		all_quas1.push(&qua);
		qua_colors1.push(qua_col);
	}

	reset_all_note1(brn, all_quas1);
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

	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	set_all_tag0(brn, all_neus1);
	
	for(long bb = 0; bb < all_neus2.size(); bb++){
		BRAIN_CK(all_neus2[bb] != NULL_PT);
		neuron& neu = *(all_neus2[bb]);

		bool inc_consec = has_diff_col_than_prev(neu_colors2, bb, bb-1);
		if(inc_consec){ neu_col++; }
		
		if(neu.has_tag0()){
			continue;
		}
		neu.set_tag0(brn);

		all_neus1.push(&neu);
		neu_colors1.push(neu_col);
	}

	reset_all_tag0(brn, all_neus1);
	BRAIN_CK(brn.br_ne_tot_tag0 == 0);
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
	
	BRAIN_CK(! is_na_mono());

	brn.init_mem_tmps();

	bool prep_ok = map_prepare_mem_oper(mm);

	if(! prep_ok){
		DBG_PRT(47, os << "map_oper skip (prepare == false) nmp=" << dbg_na_id());
		return false;
	}
	DBG_PRT(47, os << "map_oper_go nmp=" << dbg_na_id());

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
		na_dbg_guide_sha_str = tmp_guide_cnf.cf_sha_str;
		na_dbg_quick_sha_str = tmp_diff_cnf.get_ref3_nam();
		na_dbg_tauto_pth = tmp_tauto_cnf.get_cnf_path();
	);
	
	bool oper_ok = false;
	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();
		ch_string gui_sha_str = tmp_guide_cnf.cf_sha_str;

		// This func sets cc_spot==true to tmp_diff_cnf clauses in the found vnt.
		long fst_idx = tmp_diff_cnf.first_vnt_i_super_of(skg, gui_sha_str, &iinfo);

		na_found_in_skl = mf_not_found;
		
		oper_ok = (fst_idx != INVALID_NATURAL);
		if(oper_ok){
			
			na_found_in_skl = mf_found;
			
			ch_string fst_vpth = 
				tmp_diff_cnf.get_variant_path(skg, fst_idx, skg.in_dbg_verif());
				
			o_info.bjo_sub_cnf_hits++;

			row_neuron_t& all_neus_in_vnt = na_all_found;
			all_neus_in_vnt.clear();
			
			bool only_with_spot = true; // only clauses with cc_spot==true
			ccl_row_as<neuron>(tmp_diff_cnf.cf_clauses, all_neus_in_vnt, only_with_spot);

			// force all_neus_in_vnt to be all neus of nmp ??????
		}
	} else {
		BRAIN_CK(mm == mo_save);

		ch_string lk_dir = tmp_diff_cnf.get_lck_nam();
		int fd_lk = skg.get_write_lock(lk_dir);

		if(fd_lk != -1){
			ch_string tau_pth = tmp_tauto_cnf.get_cnf_path();
			
			BRAIN_DBG(tmp_tauto_cnf.cf_dbg_orig_nmp = ((void*)this));
			
			SKELETON_CK(! tmp_tauto_cnf.is_empty());
			//SKELETON_CK(! tmp_diff_cnf.is_empty());
			SKELETON_CK(! tmp_guide_cnf.is_empty());
			
			SKELETON_CK(! tmp_tauto_cnf.has_phase_path());
			SKELETON_CK(tmp_diff_cnf.has_phase_path());
			SKELETON_CK(! tmp_guide_cnf.has_phase_path());
			
			row<char>& comm_chrs = tmp_diff_cnf.cf_comment_chars;
			skg.add_comment_chars_to(brn, tmp_diff_cnf, tau_pth, comm_chrs);
			
			tmp_tauto_cnf.save_cnf(skg, tau_pth);
			oper_ok = tmp_diff_cnf.save_cnf(skg, tau_pth);
			tmp_guide_cnf.save_cnf(skg, tau_pth);

			if(oper_ok){
				o_info.bjo_saved_targets++;
			}
			
			//BRAIN_CK(! oper_ok || srg_forced.base_path_exists(skg));
			
			skg.drop_write_lock(lk_dir, fd_lk);

			ch_string pth1 = tmp_diff_cnf.get_ref1_nam();
			ch_string pth2 = tmp_diff_cnf.get_ref2_nam();

			BRAIN_DBG(bool tail_case = tmp_diff_cnf.is_empty());
			BRAIN_CK((pth1 == "") || tail_case || skg.find_skl_path(skg.as_full_path(pth1)));
			BRAIN_CK((pth2 == "") || tail_case || skg.find_skl_path(skg.as_full_path(pth2)));

			DBG_COMMAND(3, 
				ch_string sv_tau_pth1 = tau_pth + tmp_tauto_cnf.get_kind_name();
				ch_string sv1_name = skg.as_full_path(sv_tau_pth1);
				if(na_dbg_is_no_abort_full_nmp){
					na_dbg_is_no_abort_full_wrt_pth = sv1_name;
				}
				
				if(oper_ok){
					dbg_run_satex_on(brn, sv1_name, this);
				}
			);			
		}
	}
	
	DBG_PRT(102, 
		if(brn.br_dbg_found_top){ os << STACK_STR << "\n"; }
		ch_string op_ok_str = (oper_ok)?("YES"):("no");
		ch_string op_str = (na_dbg_nmp_mem_op == mo_find)?("Find"):("Write");
		op_str += op_ok_str;
		os << op_str << " na_idx=" << na_index << "\n";
	);
	DBG_PRT(70,
		if(brn.br_dbg_found_top){ os << STACK_STR << "\n"; }
		ch_string op_ok_str = (oper_ok)?("YES"):("no");
		ch_string op_str = (na_dbg_nmp_mem_op == mo_find)?("Find"):("Write");
		op_str += op_ok_str;
		
		ch_string msg_htm = map_dbg_html_data_str(op_str);;
		
		map_dbg_set_cy_maps();
		brn.dbg_update_html_cy_graph(CY_NMP_KIND, &(brn.br_tmp_ini_tauto_col), msg_htm);
		map_dbg_reset_cy_maps();
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
				DBG_PRT(71, 
					os << "ADDED wrt_id=" << na_dbg_phi_id << "\n";
					os << " wrt_phi_id_str='" << str_id << "'\n";
				);
				BRAIN_CK((old_ids.find(str_id) != old_ids.end()));
			}
			DBG_PRT_COND(71, id_was_wrt, 
				os << "REPEATED_WRITE for wrt_id=" << na_dbg_phi_id << "\n";
				os << " old_sha=" << old_ids[str_id] << "\n";
				os << " cur_sha=" << na_dbg_tauto_sha_str << "\n";
				os << " tau_sha=" << tmp_tauto_cnf.cf_sha_str << "\n";
				os << " wrt_phi_id_str='" << str_id << "'\n";
			);
			DBG_PRT_COND(71, ! id_was_wrt, 
				os << "FIRST_WRITE_OK for wrt_id=" << na_dbg_phi_id << "\n";
				os << " wrt_phi_id_str='" << str_id << "'\n";
				os << " cur_sha=" << na_dbg_tauto_sha_str << "\n";
			);
		}
	);
	
	return oper_ok;
}

void
coloring::init_with_trace(brain& brn, row<prop_signal>& dtrace, 
						  long first_idx, long last_idx)
{
	if(last_idx < 0){ last_idx = dtrace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK((first_idx == dtrace.size()) || dtrace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == dtrace.size()) || dtrace.is_valid_idx(last_idx));

	init_coloring(&brn);
	
	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row_neuron_t&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;

	all_quas.clear();
	all_neus.clear();
	qua_colors.clear();
	neu_colors.clear();
	
	long col_qu = 1;
	long col_ne = 0;
	bool inc_col_ne_pend = true;

	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	for(long ii = first_idx; ii < last_idx; ii++){
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

		bool has_neu = (q_sig1.ps_source != NULL_PT);
		if(has_neu){
			neuron& neu = *(q_sig1.ps_source);
			BRAIN_CK(neu.ne_original);

			if(inc_col_ne_pend){ 
				inc_col_ne_pend = false;
				col_ne++; 
			}
			
			all_neus.push(&neu); 
			neu_colors.push(col_ne);
		}
	}

	DBG(reset_all_its_note1(brn, all_quas));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(ck_cols());
}

/*
void
neuromap::map_get_ini_guide_col(coloring& clr)
{
	brain& brn = get_brn();
	row<prop_signal>& dtrace = na_propag;
	MARK_USED(brn);
	
	clr.init_coloring(&brn);
	
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

		bool has_neu = (q_sig1.ps_source != NULL_PT);
		if(has_neu){
			neuron& neu = *(q_sig1.ps_source);
			BRAIN_CK(neu.ne_original);

			if(inc_col_ne_pend){ 
				inc_col_ne_pend = false;
				col_ne++; 
			}
			
			all_neus.push(&neu); 
			neu_colors.push(col_ne);
		}
	}

	DBG(reset_all_its_note1(brn, all_quas));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	BRAIN_CK(clr.ck_cols());
}
*/

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
	neus_srg.stab_mutual(quas_srg, false);
	na_guide_tot_vars = quas_srg.sg_step_sortees.size() / 2;
	
	BRAIN_CK(! brn.br_guide_neus_srg.sg_cnf_clauses.is_empty());
	
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
	
	BRAIN_CK(na_orig_cho != NULL_PT);
	BRAIN_CK(na_orig_cho->has_charge());
	//ticket& nmp_wrt_tk = na_candidate_tk;
	ticket& nmp_wrt_tk = brn.br_curr_write_tk;

	all_upper_quas.clear();
	
	row_quanton_t& all_qua = brn.br_tmp_nmp_quas_for_upper_qu;
	map_get_all_quas(all_qua);

	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	set_all_note1(brn, all_qua);
	
	row_neuron_t& all_neus = brn.br_tmp_nmp_neus_for_upper_qu;
	all_neus.clear();
	map_get_all_neus(all_neus, true);
	//map_get_all_neus(all_neus, false);
	
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		
		neu.update_ne_to_wrt_tk(brn, nmp_wrt_tk);
		
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
		
		BRAIN_CK(! qua.has_note1());
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
neuromap::map_get_simple_coloring(coloring& clr, mem_op_t mm){
	brain& brn = get_brn();
	clr.init_coloring(&brn);
	
	row_quanton_t& all_quas = clr.co_quas;
	all_quas.clear();
	map_get_all_quas(all_quas);
	append_missing_opps(brn, all_quas);
	
	row_neuron_t& all_neus = clr.co_neus;
	all_neus.clear();
	map_get_all_neus(all_neus, false, mm);
	
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
	
	guide_col.add_coloring(ini_guide_col, this);
	
	map_stab_guide_col();
}

void
neuromap::map_get_initial_tauto_coloring(coloring& prv_clr, coloring& tauto_clr, 
										 mem_op_t mm)
{
	brain& brn = get_brn();
	BRAIN_CK(&prv_clr != &tauto_clr);
	
	tauto_clr.init_coloring(&brn);

	if(! has_submap()){
		map_get_simple_coloring(tauto_clr, mm);
		BRAIN_CK(tauto_clr.ck_cols());
		return;
	}
	
	coloring& tmp_co = brn.br_tmp_tauto_col;
	tmp_co.init_coloring(&brn);
	
	row_neuron_t& all_co_neus = tmp_co.co_neus;
	
	all_co_neus.clear();
	//map_get_all_cov_neus(all_co_neus, true, true, mm);
	map_get_all_cov_neus(all_co_neus, true, false, mm);
	tmp_co.co_neu_colors.fill(1, all_co_neus.size());

	prv_clr.copy_co_to(tauto_clr);
	tauto_clr.add_coloring(tmp_co, this);
	
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
		
		//BRAIN_CK(tee1.so_tee_consec != last_tee_id);
		if(tee1.so_tee_consec != last_tee_id){
			BRAIN_CK(last_tee_id < tee1.so_tee_consec);
			last_tee_id = tee1.so_tee_consec;

			BRAIN_CK(tee1.has_vessel());
			sorset& ss2 = tee1.get_vessel();
			if(ss2.ss_mark){
				BRAIN_CK(last_ss != &ss2);
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

	brain& brn = get_brn();
	if(has_submap()){
		//map_get_ini_guide_col(clr);
		clr.init_with_trace(brn, na_propag);
	} else {
		//map_get_simple_guide_col(clr);
		
		clr.init_with_trace(brn, na_propag);
		clr.reset_as_simple_col();
		
		//map_get_simple_coloring(clr);
	}
}

void
neuromap::nmp_fill_all_upper_covs(long dbg_idx){
	if(has_submap()){
		na_submap->nmp_fill_all_upper_covs(dbg_idx + 1);
	}
	BRAIN_DBG(
		brain& brn = get_brn();
		BRAIN_CK(brn.br_candidate_nxt_nmp_lvs.is_valid_idx(dbg_idx));
		neuromap* dbg_nmp = brn.br_candidate_nxt_nmp_lvs[dbg_idx];
		BRAIN_CK(dbg_nmp == this);
	);
	BRAIN_CK(na_all_qua.is_alone());
	BRAIN_CK(na_all_neu.is_alone());
	
	nmp_fill_upper_covs();
}

void
neuromap::nmp_fill_upper_covs(){
	//BRAIN_DBG(brain& brn = get_brn());
	for(long aa = 0; aa < na_all_centry.size(); aa++){
		cov_entry& cty = na_all_centry[aa];
		neuromap* nmp = cty.ce_nmp;
		neuron* neu = cty.ce_neu;
		
		BRAIN_CK(nmp != NULL_PT);
		BRAIN_CK(neu != NULL_PT);
		BRAIN_CK(nmp->na_dbg_cand_sys);
		BRAIN_CK(nmp->na_dbg_creating);
		BRAIN_CK(nmp->nmp_is_cand());
		BRAIN_CK(! nmp->na_candidate_tk.is_tk_virgin());
		
		neu->set_cand_tk(nmp->na_candidate_tk);
		//neu->set_first_cand_tk();
		nmp->na_all_cov.push(neu);
	}
}

quanton*	
neuromap::map_choose_propag_qua(){ 
	//quanton* qua = NULL;
	long max_ii = na_propag.last_idx();
	for(long ii = max_ii; ii >= 0; ii--){
		prop_signal& nxt_ps = na_propag[ii];
		quanton* qua = nxt_ps.ps_quanton;
		BRAIN_CK(qua != NULL_PT);
		if(! qua->has_charge()){
			DBG_PRT(105, os << "NMP_CHOICE qua=" << qua << " cand=";
				print_cand_id(os);
			);
			return qua;
		}
	}
	return NULL;
}

quanton*	
neuromap::map_choose_quanton(){ 
	quanton* qua = NULL_PT;
	qua = map_choose_propag_qua();
	if((qua == NULL_PT) && has_submap()){
		qua = na_submap->map_choose_quanton();
	}
	return qua;
}

void
neuromap::nmp_set_quas_cand_tk(){
	brain& brn = get_brn();
	MARK_USED(brn);
	
	BRAIN_CK(! na_candidate_tk.is_tk_virgin());
	for(long aa = 0; aa < na_propag.size(); aa++){
		prop_signal& nxt_ps = na_propag[aa];
		quanton* pt_qua = nxt_ps.ps_quanton;
		BRAIN_CK(pt_qua != NULL_PT);
		
		quanton& qua = *(pt_qua);
		quanton& opp = qua.opposite();

		//qua.make_qu_dominated(brn);
		//opp.make_qu_dominated(brn);
		
		BRAIN_CK(! qua.qu_candidate_tk.is_older_than(brn.get_last_cand()));
		BRAIN_CK(! opp.qu_candidate_tk.is_older_than(brn.get_last_cand()));
		
		qua.qu_candidate_tk = na_candidate_tk;
		opp.qu_candidate_tk = na_candidate_tk;
	}
}

void
neuromap::nmp_set_neus_cand_tk(){
	brain& brn = get_brn();
	BRAIN_CK(! na_candidate_tk.is_tk_virgin());
	for(long aa = 0; aa < na_propag.size(); aa++){
		neuron* neu = na_propag[aa].ps_source;
		if(neu != NULL_PT){
			//neu->make_ne_dominated(brn);
			
			BRAIN_CK(! neu->ne_candidate_tk.is_older_than(brn.get_last_cand()));
			neu->set_cand_tk(na_candidate_tk);
			
			//neu->set_first_cand_tk();
		}
	}
	for(long aa = na_all_cov.last_idx(); aa >= 0; aa--){
		neuron* neu = na_all_cov[aa];
		BRAIN_CK(neu != NULL_PT);
		
		if(neu->is_ne_source()){
			na_all_cov.swapop(aa);
			continue;
		}
		
		neu->make_ne_dominated(brn);
		
		BRAIN_CK(! neu->ne_candidate_tk.is_older_than(brn.get_last_cand()));
		neu->set_cand_tk(na_candidate_tk);
		//neu->set_first_cand_tk();
	}
}

void
neuromap::nmp_set_all_cand_tk(){
	//BRAIN_DBG(brain& brn = get_brn());
	
	if(has_submap()){
		na_submap->nmp_set_all_cand_tk();
	}
	nmp_set_quas_cand_tk();
	nmp_set_neus_cand_tk();
}

void
neuromap::nmp_set_all_num_sub(){
	na_num_submap = 0;
	if(has_submap()){
		na_submap->nmp_set_all_num_sub();
		na_num_submap = na_submap->na_num_submap + 1;
	}
}

void
neuromap::nmp_update_to_write(row_neuron_t& upd_from, ticket& nmp_wrt_tk){
	BRAIN_DBG(brain& brn = get_brn());
	BRAIN_CK(na_orig_cho != NULL_PT);
	//ticket& nmp_wrt_tk = na_candidate_tk;

	for(long aa = 0; aa < upd_from.size(); aa++){
		BRAIN_CK(upd_from[aa] != NULL_PT);
		neuron* neu = upd_from[aa];
		bool is_ldr = neu->ne_to_wrt_tk.is_older_than(nmp_wrt_tk);
		
		if(! is_ldr){
			BRAIN_CK(! neu->has_tag0());
			BRAIN_DBG(neu->set_tag0(brn));
			na_to_write.push(neu);
		} else {
			na_not_to_write.push(neu);
		}
	}
}

bool
neuromap::nmp_is_min_simple(){
	BRAIN_CK(! na_is_min_simple);
	if(has_submap()){
		return false;
	}
	if(! na_upd_to_write){
		return false;
	}
	
	brain& brn = get_brn();
	long min_sub = brn.get_min_trainable_num_sub();
	if(min_sub < 2){ 
		return false;
	}
	return true;
}

void
neuromap::nmp_set_min_simple(){
	na_is_min_simple = false;
	if(nmp_is_min_simple()){
		na_is_min_simple = true;
	}
}

/*
void
neuromap::map_get_simple_guide_col(coloring& clr){
	brain& brn = get_brn();
	clr.init_coloring(&brn);
	
	row_quanton_t& all_quas = clr.co_quas;
	all_quas.clear();
	map_get_all_quas(all_quas);
	append_missing_opps(brn, all_quas);

	row_neuron_t& all_neus = clr.co_neus;
	row<prop_signal>& all_ps = brn.br_tmp_nmp_get_all_ps;
	map_get_all_propag_ps(all_ps);
	append_all_trace_neus(all_ps, all_neus);
	
	clr.reset_as_simple_col();
}*/

void
neuromap::nmp_update_all_to_write(ticket& nmp_wrt_tk){
	brain& brn = get_brn();
	
	if(has_submap()){
		na_submap->nmp_update_all_to_write(nmp_wrt_tk);
	}
	
	na_to_write.clear();
	na_not_to_write.clear();
	
	BRAIN_DBG(
		BRAIN_CK(brn.br_ne_tot_tag0 == 0);
		BRAIN_CK(na_to_write.is_empty());
		
		row<prop_signal>& all_ps = brn.br_dbg_all_ps_upd_wrt;
		map_get_all_propag_ps(all_ps);
		row_neuron_t& all_prop = brn.br_dbg_all_neu_upd_wrt;
		all_prop.clear();
		append_all_trace_neus(all_ps, all_prop);
		set_all_tag0(brn, all_prop);
	);
	
	nmp_update_to_write(na_all_cov, nmp_wrt_tk);
	if(has_submap()){
		row_neuron_t& upd_from = na_submap->na_not_to_write;
		nmp_update_to_write(upd_from, nmp_wrt_tk);
		upd_from.clear();
	}
	
	na_upd_to_write = true;
	
	BRAIN_DBG(
		reset_all_tag0(brn, all_prop);
		reset_all_tag0(brn, na_to_write);
		BRAIN_CK(brn.br_ne_tot_tag0 == 0);
	);
	DBG_PRT(42, os << " UPD_WRT NMP=\n" << this);
}

void
neuromap::nmp_reset_write(){
	if(has_submap()){
		na_submap->nmp_reset_write();
	}
	
	na_to_write.clear();
	na_not_to_write.clear();

	na_upd_to_write = false;
}

bool
neuromap::dbg_is_watched(){
	bool is_w = false;
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	bool is_w_1 = (brn.br_dbg_watched_nmp_idx == na_index);
	bool is_w_2 = is_ticket_eq(brn.br_dbg_watched_nmp_tk, na_candidate_tk);
	is_w = (is_w_1 && is_w_2);
#endif
	return is_w;
}

bj_ostream&
cov_entry::print_cov_entry(bj_ostream& os, bool from_pt){
#ifdef FULL_DEBUG
	os << "ce[";
	if(ce_nmp != NULL_PT){
		os << ce_nmp->dbg_na_id() << ".";
	} else {
		os << "NULL_NMP.";
	}
	if(ce_neu != NULL_PT){
		os << ce_neu->ne_index << ".";
	} else {
		os << "NULL_NEU.";
	}
	os << "]";
	os.flush();
#endif
	return os;
}

bool
neuromap::map_prepare_mem_oper(mem_op_t mm){
	brain& brn = get_brn();
	coloring& guide_col = na_guide_col;
	MARK_USED(guide_col);
	BRAIN_CK(brn.br_qu_tot_note0 == 0); // deducer note
	
	//BRAIN_DBG(coloring dbg_smpl_col;);
	//DBG_COMMAND(41, map_get_simple_coloring(dbg_smpl_col););
	
	DBG_PRT(41, os << " +++++++++++++++++++++++++\n";
		os << "map_mem_oper.\n";
		os << " mm=" << map_dbg_oper_str(mm) << " nmp=\n" << this << "\n";
		os << " \nguide_col=\n" << guide_col << "\n";
	);

	sort_glb& guide_ne_srg = brn.br_guide_neus_srg;
	sort_glb& guide_qu_srg = brn.br_guide_quas_srg;

	brn.all_mutual_init();
	
	BRAIN_DBG(ch_string gui_case_str = "NO_GUIDE");
	bool gui_loaded = false;
	if(! has_stab_guide()){
		BRAIN_DBG(gui_case_str = "CALC_GUIDE");
		
		map_set_stab_guide();
	} else {
		DBG_PRT(41, os << "HAD_STAB_GUIDE !!! nmp=" << this);
		BRAIN_DBG(gui_case_str = "LOADED_GUIDE");
		
		guide_col.load_colors_into(guide_ne_srg, guide_qu_srg, dbg_call_1, this);
		gui_loaded = true;
	}
	
	DBG_PRT_COND(115, (na_index == 8) && (na_candidate_tk.tk_recoil == 8),
		os << " NA_8. " << gui_case_str << " nmp=" << dbg_na_id() << "\n";
	);
	
	BRAIN_CK(! guide_col.is_co_virgin());
	BRAIN_CK(map_ck_contained_in(guide_col, dbg_call_1));

	// stab compl
	
	//BRAIN_CK(! brn.br_guide_neus_srg.sg_cnf_clauses.is_empty());
	BRAIN_DBG(
		coloring tmp_ck_guide_col(&brn);
		tmp_ck_guide_col.save_colors_from(guide_ne_srg, guide_qu_srg, tid_wlk_consec, false);
		BRAIN_CK_PRT((tmp_ck_guide_col.dbg_equal_co_to(guide_col, &(na_pend_col.co_quas))), 
				os << "tmp_col=" << tmp_ck_guide_col << "\n";
				os << "gui_col=" << guide_col << "\n";
		);
		//BRAIN_CK(nmp_ck_extra_quas());
	);
	//BRAIN_CK(! brn.br_guide_neus_srg.sg_cnf_clauses.is_empty());

	// final guide col
	
	guide_ne_srg.stab_mutual(guide_qu_srg, false);	// needed
	
	coloring final_guide_col(&brn);
	BRAIN_DBG(coloring old_uni_col(&brn));
	
	final_guide_col.save_colors_from(guide_ne_srg, guide_qu_srg, tid_tee_consec, false);

	BRAIN_CK(&(brn.br_guide_quas_srg) == &(guide_qu_srg));
	
	if(! na_pend_col.co_quas.is_empty()){
		DBG_PRT(105, os << "na_pend_col=" << na_pend_col << " nmp=" << dbg_na_id());
		BRAIN_DBG(final_guide_col.copy_co_to(old_uni_col));
		final_guide_col.add_coloring(na_pend_col);
	}

	//coloring& final_guide_col = guide_col;

	BRAIN_CK(dbg_has_simple_coloring_quas(final_guide_col));
	BRAIN_CK(map_ck_contained_in(final_guide_col, dbg_call_2));

	// set quick_find_ref and ck it
	
	skeleton_glb& skg = brn.get_skeleton();

	DBG_PRT_COND(115, (na_index == 8) && (na_candidate_tk.tk_recoil == 8),
		os << " NA_8. nmp=" << dbg_na_id() << "\n";
		os << " final_guide_col=\n";
		os << final_guide_col;
		os << " guide_tees=\n";
		os << guide_ne_srg.sg_step_sortees;
		os << " guide_ccls=\n";
		os << guide_ne_srg.sg_cnf_clauses;
		os << " ________________\n";
	);
	DBG_PRT_COND(115, guide_ne_srg.sg_cnf_clauses.is_empty(), 
		os << " nmp=" << dbg_na_id()
	);
	long num_var = na_guide_tot_vars;
	canon_cnf& gui_cnf = guide_ne_srg.get_final_cnf(skg, false, false);

	BRAIN_CK(gui_cnf.cf_dims.dd_tot_vars == 0);
	//BRAIN_CK(! gui_cnf.cf_phdat.has_ref());
	
	ch_string quick_find_ref = "";
	row_str_t dbg_shas;

	quick_find_ref = gui_cnf.get_ref_path();	// stab guide 
	
	dbg_shas.push(gui_cnf.cf_sha_str + "\n");
	
	// FIND_GUIDE
	bool in_fnd = (mm == mo_find);
	//bool in_fnd = (has_submap() && (mm == mo_find));
	if(in_fnd){
		instance_info& iinfo = brn.get_my_inst();
		
		ch_string pth1 = skg.as_full_path(quick_find_ref);
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
	
	map_get_initial_tauto_coloring(final_guide_col, ini_tau_col, mm);

	BRAIN_CK(map_ck_contained_in(ini_tau_col, dbg_call_3));

	brn.all_mutual_init();
	
	sort_glb& tauto_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& tauto_qu_srg = brn.br_tauto_quas_srg;

	BRAIN_CK(tauto_ne_srg.ck_stab_inited());
	BRAIN_CK(tauto_qu_srg.ck_stab_inited());
	
	tauto_ne_srg.sg_cnf_step.clear_cnf();
	tauto_qu_srg.sg_cnf_step.clear_cnf();
	
	ini_tau_col.load_colors_into(tauto_ne_srg, tauto_qu_srg, dbg_call_3, this, true);

	tauto_ne_srg.stab_mutual_unique(tauto_qu_srg, this);
	
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
		os << " uni_col=" << final_guide_col << "\n\n";
		os << " pre_tau_col=" << ini_tau_col << "\n\n";
		os << " pos_tau_col=" << pos_tau_col << "\n\n";
		os << " ids_tau_col=" << na_dbg_tauto_col << "\n\n";
		os << " nmp=" << this << "\n\n";
	);
	BRAIN_DBG(
		row_quanton_t r_bad;
		
		row_quanton_t r_qu1;
		tees_row_as<quanton>(brn.br_tauto_quas_srg.sg_step_sortees, r_qu1);
		
		row_quanton_t& r_qu2 = final_guide_col.co_quas;
	);
		
	BRAIN_CK_PRT(same_quantons_note0(brn, r_qu2, r_qu1, &r_bad),
		DBG_PRT_ABORT(brn);
		os << "r_qu1=\n" << r_qu1 << "\n";
		os << "r_qu2=\n" << r_qu2 << "\n";
		os << "r_bad=\n" << r_bad << "\n";
		os << "na_propag=\n" << na_propag << "\n";
		os << "nmp=\n" << this << "\n";
	);

	// finish prepare
	
	map_prepare_wrt_cnfs(mm, quick_find_ref, dbg_shas);

	return true;
}

void
neuromap::map_prepare_wrt_cnfs(mem_op_t mm, ch_string quick_find_ref, row_str_t& dbg_shas){
	brain& brn = get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	sort_glb& guide_ne_srg = brn.br_guide_neus_srg;
	sort_glb& tauto_ne_srg = brn.br_tauto_neus_srg;
	
	// init guide tees
	
	row<sortee*>& guide_tees = brn.br_tmp_wrt_guide_tees;
	guide_ne_srg.sg_step_sortees.move_to(guide_tees);
	
	// get final tauto cnf
	
	canon_cnf& tauto_cnf = tauto_ne_srg.get_final_cnf(skg, true, false);

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
	
	tmp_tauto_cnf.cf_kind = fk_canon;
	tmp_diff_cnf.cf_kind = fk_diff;
	tmp_guide_cnf.cf_kind = fk_guide;
	
	tmp_diff_cnf.cf_guide_cnf = &tmp_guide_cnf;
	tmp_diff_cnf.cf_tauto_cnf = &tmp_tauto_cnf;
	tmp_diff_cnf.cf_quick_find_ref = quick_find_ref;
	
	tmp_tauto_cnf.cf_diff_minisha_str = tmp_diff_cnf.cf_minisha_str;

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

