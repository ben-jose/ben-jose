

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

reverse.cpp  

funcs that implement reverse func.

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
	bool can_write = (dct.dt_forced_level == level());
	return can_write;
}

bool
brain::in_edge_of_target_lv(deduction& dct){
	if(dct.is_dt_virgin()){ return false; }
	long trl_lv = trail_level();
	bool in_tg_lv = (trl_lv < dct.dt_target_level);
	BRAIN_CK(! in_tg_lv || ((trl_lv + 1) == level()));
	return in_tg_lv;
}

void
brain::reverse(){
	DEDUC_DBG(deduction dct2);
	//long dbg_old_lv = level();

	BRAIN_CK(! has_psignals());
	brain& brn = *this;
	notekeeper& nke0 = br_retract_nke0;
	deduction& dct = br_retract_dct;
	memap& mpp0 = br_retract_map0;
	long& all_notes0 = br_qu_tot_note0;
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
	BRAIN_CK(br_semi_monos.is_empty());

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
	DBG_PRT(122, print_trail(os));
	DBG_PRT(14, print_trail(os));
	DBG_PRT(14, os << "chosen " << br_chosen);

	// START REVERSE (init mpp0 and nke0)

	BRAIN_CK(found_conflict());
	
	neuron& cfl = *br_conflict_found;

	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());

	DEDUC_DBG(br_dbg.dbg_br_deducer.dbg_find_dct_of(cfl, dct2));

	mpp0.ma_confl = &cfl;
	mpp0.ma_before_retract_tk.update_ticket(&brn);
	
	DBG_PRT_COND(122, ! cfl.in_ne_dominated(brn), os << "NOT_DOM cfl=" << cfl);
	cfl.set_motives(brn, nke0, true); // init nke0 with confl

	BRAIN_CK(all_notes0 > 0);

	BRAIN_CK(! mpp0.is_ma_virgin());

	BRAIN_CK(cfl.ne_original);
	if(! cfl.ne_original){ 
		mpp0.reset_memap(brn);
		DBG_PRT(121, os << "mpp0.reset_memap (! orig)");
	}

	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, dbg_prt_lvs_have_learned(os));
	//DBG_PRT(122, dbg_prt_lvs_virgin(os));
	
	// REVERSE LOOP

	quanton* chosen_qua = NULL_PT;
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
			//if(lv_map0.is_ma_virgin() && (mpp0.ma_anchor_idx != INVALID_IDX)){
			if(lv_map0.is_ma_virgin() && (mpp0.ma_anchor_idx != INVALID_IDX)){
				BRAIN_CK(mpp0.ck_last_szs());
				BRAIN_CK(! mpp0.ma_dotted.is_empty());

				DBG(br_dbg.dbg_find_id++; br_dbg.dbg_canon_find_id = br_dbg.dbg_find_id;)

				in_mm = mpp0.map_find(brn);
			}
			if(! in_mm){ 
				BRAIN_CK((br_charge_trail.last_qlevel() + 1) == level());
				DBG_PRT(122, os << "!ff." << vg_m0);
				break;
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
			BRAIN_CK(mpp0.map_ck_all_qu_dominated(brn));
			BRAIN_CK(mpp0.map_ck_all_ne_dominated(brn));

			BRAIN_CK(level() != ROOT_LEVEL);
			BRAIN_CK(nke0.dk_num_noted_in_layer == 0);
			BRAIN_CK(level() <= nke0.dk_note_layer);

			if(lv_has_learned()){
				mpp0.reset_memap(brn);
				DBG_PRT(121, os << "mpp0.reset_memap (has_learnd)");
			}
			
			DBG(bool wrote = false);
			memap& lv_map0 = data_level().ld_map0;
			
			DBG(bool m_act = lv_map0.ma_active);
			DBG(bool m_cw = false);
			if(lv_map0.ma_active){
				BRAIN_CK(lv_map0.ck_last_szs());
				BRAIN_CK(! lv_map0.is_ma_virgin());
				BRAIN_CK(lv_map0.map_ck_all_qu_dominated(brn));
				BRAIN_CK(lv_map0.map_ck_all_ne_dominated(brn));
				BRAIN_CK(lv_has_learned());
				bool can_write = can_write_reverse_map(dct);
				if(can_write){
					DBG(m_cw = true);
					//brn.init_forced_sorter();

					DBG(br_dbg.dbg_save_id++; br_dbg.dbg_canon_save_id = br_dbg.dbg_save_id);
					
					BRAIN_CK(lv_map0.dbg_ck_used_simple_no_satisf(mo_save, brn));
					BRAIN_CK(lv_map0.map_ck_simple_no_satisf(mo_save, brn));

					DBG_PRT(122, os << "SAVING MAP" << bj_eol;
						//os << (&(lv_map0)) << bj_eol;
						os << lv_map0 << bj_eol;
						brn.print_trail(os);
					);
					DBG_PRT(123, os << "HIT RETURN TO CONTINUE...");
					DBG_COMMAND(123, getchar());

					bool sv_ok = lv_map0.map_save(brn);
					if(sv_ok){
						DBG(wrote = true);
						brn.br_num_memo++;
					}

					DBG_PRT(54, os << "writing LV=" << level() << bj_eol;
						os << data_level()
					);
				}
				lv_map0.map_deactivate(brn);

				BRAIN_CK(mpp0.is_ma_virgin());
				BRAIN_CK(level() > dct.dt_target_level);

				row_quanton_t& bak_upper = data_level().ld_upper_quas;
				nke0.restart_with(brn, bak_upper);

				mpp0.reset_memap(brn);
				mpp0.map_replace_with(brn, lv_map0, dbg_call_1);
				BRAIN_CK(mpp0.ck_last_szs());

				DBG_PRT(120, os << "mpp0.reset_memap (end_lv)");
			} // lv_map0.ma_active

			dec_level();
			br_retract_is_first_lv = false;

			DBG(int vg2_m0 = ((mpp0.is_ma_virgin())?(1):(0)));
			DBG_PRT(122, if(wrote){ os << "WW."; } else { os << "!ww."; }
				os << vg_m0 << '.' << vg2_m0 << ".a." << m_act << ".c." << m_cw;
			);
		} // end_of_lev

		BRAIN_CK(! mpp0.ma_active);
		BRAIN_CK(br_charge_trail.last_qlevel() == level());

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
		if(qua.has_note3()){
			qua.reset_its_note3(brn);
			DBG(had_n3 = true);
			while(! qua.in_qu_dominated(brn)){
				DBG_PRT(112, os << "NOT dom (case 1) qua=" << &qua << 
						" up_dom=" << get_last_upper_map());
				DBG_PRT(54, os << "NOT dom (case 1) qua=" << &qua << bj_eol;
					print_trail(os);

					os << " up_dom=" << get_last_upper_map() 
					<< " qu_curr_map=" << qua.qu_curr_map << bj_eol
					<< " br_maps_active=" << br_maps_active
				);
				deactivate_last_map();
			}
			BRAIN_CK(qua.in_qu_dominated(brn));

			if(! qua.has_note0()){
				mpp0.reset_memap(brn);
				BRAIN_CK(mpp0.is_ma_virgin());
				DBG_PRT(120, os << "mpp0.reset_memap (has_3 && ! has_0)");
			}
		}

		if(qua.has_note0()){
			long qlv = qua.qlevel();
			nke0.update_notes_layer(qlv);
			BRAIN_CK(nke0.dk_num_noted_in_layer > 0);

			qua.reset_its_note0(brn);
			DBG(had_n0 = true);
			while(! qua.in_qu_dominated(brn)){
				DBG_PRT(112, os << "NOT dom (case 2) qua=" << &qua << 
						" up_dom=" << get_last_upper_map());
				DBG_PRT(54, os << "NOT dom (case 2) qua=" << &qua << bj_eol;
					print_trail(os);
					os << " up_dom=" << get_last_upper_map() 
					<< " qu_curr_map=" << qua.qu_curr_map << bj_eol
					<< " br_maps_active=" << br_maps_active
				);
				deactivate_last_map();
			}

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

			chosen_qua = &qua;
		}
		
		// reset charge

		BRAIN_CK(! qua.has_note3());
		BRAIN_CK(! qua.has_note0());
		BRAIN_CK(! (had_n3 || had_n0) || qua.in_qu_dominated(brn));

		qua.set_charge(brn, NULL_PT, cg_neutral, INVALID_TIER);

	} // true

	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);

	//BRAIN_CK(! has_in_mem);	// DBG purposes
	
	DEDUC_DBG(has_in_mem || dbg_ck_deducs(dct, dct2));
	DBG(long rr_lv = br_charge_trail.last_qlevel());
	BRAIN_CK((level() == ROOT_LEVEL) || ((rr_lv + 1) == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));

	// update leveldat
	
	memap& lv_map0 = data_level().ld_map0;
	if(! mpp0.is_ma_virgin() && lv_map0.is_ma_virgin()){
		ticket& n_tk = mpp0.ma_after_retract_tks.inc_sz();
		n_tk.update_ticket(&brn);

		BRAIN_CK(mpp0.ck_map_guides(dbg_call_2));
		lv_map0.map_replace_with(brn, mpp0, dbg_call_2);
		lv_map0.map_activate(brn);
		DBG_PRT(122, os << "Updated " << &(data_level()) << " with " << &lv_map0);
	}
	DBG(	
		if(! mpp0.is_ma_virgin() && ! lv_map0.is_ma_virgin()){
			BRAIN_CK(mpp0.map_ck_all_qu_dominated(brn));
			//BRAIN_CK(mpp0.map_ck_all_ne_dominated(brn));
		}
	);

	// update notes3

	row_quanton_t& rr_upper = data_level().ld_upper_quas;
	nke0.get_all_motives(rr_upper);
	BRAIN_CK(ck_motives(brn, rr_upper));
	set_all_note3(brn, rr_upper);

	DBG(long old_num3 = brn.br_qu_tot_note3);
	nke0.clear_all_motives();
	BRAIN_CK(old_num3 == brn.br_qu_tot_note3);
	BRAIN_CK(all_notes0 == 0);

	// learn motives

	BRAIN_CK(dct.dt_forced != NULL_PT);

	if(data_level().ld_first_learned == NULL_PT){
		data_level().ld_first_learned = dct.dt_forced;
	}
	learn_mots(dct.dt_motives, *dct.dt_forced);

	// resend chosen

	if(! dct.dt_motives.is_empty()){
		BRAIN_CK(chosen_qua != NULL_PT);
		BRAIN_CK(data_level().ld_chosen == chosen_qua);
		DBG_PRT(25, os << "**RETRACTED TO chosen=" << chosen_qua);
		send_psignal(*chosen_qua, NULL, tier() + 1);
	}

	update_semi_monos();

	// inc recoil

	inc_recoil();

	DBG(
		dbg_update_config_entries(*this);
	);
	check_timeout();

	BRAIN_CK((level() == ROOT_LEVEL) || lv_has_learned());

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

	br_conflict_found = NULL_PT;
	BRAIN_CK(! found_conflict());
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
coloring::save_colors_from(sort_glb& neus_srg, sort_glb& quas_srg){
	SORTER_CK(! neus_srg.sg_step_has_diff);
	SORTER_CK(! quas_srg.sg_step_has_diff);

	co_all_neu_consec = srt_row_as_colors<neuron>(neus_srg.sg_step_sortees, 
												  co_neus, co_neu_colors);
	co_all_qua_consec = srt_row_as_colors<quanton>(quas_srg.sg_step_sortees, 
												   co_quas, co_qua_colors);

	BRAIN_CK(ck_cols());
}

bool
coloring::has_diff_col(row<long>& the_colors, long col_idx){
	BRAIN_CK(the_colors.is_valid_idx(col_idx));
	bool diff_col = false;
	if(col_idx > 0){
		long col0 = the_colors[col_idx - 1];
		long col1 = the_colors[col_idx];
		BRAIN_CK(col0 >= 0);
		BRAIN_CK(col1 >= 0);
		if(col0 != col1){
			BRAIN_CK(col0 < col1);
			diff_col = true;
		}
	}
	return diff_col;
}

void
quanton::reset_and_add_tee(sort_glb& quas_srg, sort_id_t quas_consec){
	BRAIN_CK(qu_tee.so_related == &qu_reltee);
	qu_tee.reset_sort_info();
	qu_tee.sort_from(quas_srg, quas_consec);
}

void
coloring::load_colors_into(brain& brn, sort_glb& neus_srg, sort_glb& quas_srg, dima_dims& dims){
	BRAIN_CK(ck_cols());

	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row<neuron*>&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;

	//BRAIN_CK(qua_colors.is_sorted(cmp_long));
	//BRAIN_CK(neu_colors.is_sorted(cmp_long));

	BRAIN_CK(brn.br_qu_tot_note4 == 0);
	BRAIN_CK(neus_srg.sg_dbg_num_items == 0);
	BRAIN_CK(quas_srg.sg_dbg_num_items == 0);

	sort_id_t& quas_consec = neus_srg.sg_curr_stab_consec;
	quas_consec++;

	DBG(
		for(long aa = 0; aa < all_quas.size(); aa++){
			BRAIN_CK(all_quas[aa] != NULL_PT);
			quanton& qua = *(all_quas[aa]);
			BRAIN_CK(qua.qu_tee.is_unsorted());
		}
	);

	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);

		BRAIN_CK(! qua.is_note4());
		qua.set_note4(brn);

		bool inc_consec = has_diff_col(qua_colors, aa);
		if(inc_consec){ quas_consec++; }

		qua.reset_and_add_tee(quas_srg, quas_consec);
	}

	sort_id_t& neus_consec = quas_srg.sg_curr_stab_consec;
	neus_consec++;

	for(long bb = 0; bb < all_neus.size(); bb++){
		BRAIN_CK(all_neus[bb] != NULL_PT);
		neuron& neu = *(all_neus[bb]);

		bool inc_consec = has_diff_col(neu_colors, bb);
		if(inc_consec){ neus_consec++; }

		sortee& neu_tee = neu.ne_tee;
		neu_tee.so_ccl.cc_clear(false);
		neu_tee.so_tee_consec = 0;

		neu.fill_mutual_sortees(brn);

		row<sortee*>& neu_mates = neu.ne_reltee.so_mates;
		if(! neu_mates.is_empty()){
			BRAIN_CK(neu_tee.is_unsorted());
			neu_tee.sort_from(neus_srg, neus_consec);
		}
	}

	reset_all_note4(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note4 == 0);
	BRAIN_CK(ck_cols());
}

void
coloring::add_coloring(brain& brn, coloring& clr){
	BRAIN_CK(ck_cols());
	BRAIN_CK(clr.ck_cols());

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

		bool inc_consec = has_diff_col(qua_colors2, aa);
		if(inc_consec){ qua_col++; }

		BRAIN_CK(! qua.has_note1());

		all_quas1.push(&qua);
		qua_colors1.push(qua_col);
	}

	DBG(reset_all_note1(brn, all_quas1));
	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	row<neuron*>&	all_neus1 = co_neus;
	row<long>&	neu_colors1 = co_neu_colors;
	row<neuron*>&	all_neus2 = clr.co_neus;
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

		bool inc_consec = has_diff_col(neu_colors2, bb);
		if(inc_consec){ neu_col++; }

		all_neus1.push(&neu);
		neu_colors1.push(neu_col);
	}

	BRAIN_CK(ck_cols());
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
memap::map_get_layer_quas(brain& brn, row_quanton_t& all_quas, long lyr_idx1, long lyr_idx2){
	BRAIN_CK(ck_last_szs());
	BRAIN_CK(lyr_idx1 < lyr_idx2);
	BRAIN_CK((lyr_idx1 == INVALID_IDX) || ma_szs_dotted.is_valid_idx(lyr_idx1));
	BRAIN_CK(ma_szs_dotted.is_valid_idx(lyr_idx2));

	all_quas.clear();

	row<prop_signal>& dtrace = ma_dotted;
	long beg_sz = 0;
	if(ma_szs_dotted.is_valid_idx(lyr_idx1)){
		beg_sz = ma_szs_dotted[lyr_idx1];
	}
	long end_sz = ma_szs_dotted[lyr_idx2];

	BRAIN_CK(brn.br_qu_tot_note1 == 0);

	for(long ii = beg_sz; ii < end_sz; ii++){
		prop_signal& q_sig1 = dtrace[ii];

		BRAIN_CK(q_sig1.ps_quanton != NULL_PT);

		quanton& qua = *(q_sig1.ps_quanton);
		quanton& opp = qua.opposite();

		if(! qua.has_note1()){
			qua.set_note1(brn);

			all_quas.push(&qua);
			all_quas.push(&opp);
		}
	}

	reset_all_note1(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
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
			/*
			DBG_COND_COMM(! ck_val && (neu.get_dbg_brn() != NULL) &&
				(neu.ne_recoil_tk.tk_recoil != 0),
				//((ma_before_retract_tk.tk_recoil + 1) != neu.get_dbg_brn()->recoil()), 
					os << "tik_neu_lap=" << neu.ne_recoil_tk.tk_recoil << bj_eol;
					os << "tik_bef_lap=" << ma_before_retract_tk.tk_recoil << bj_eol;
					os << "tik_brn_lap=" << neu.get_dbg_brn()->recoil() << bj_eol;
			)*/
		}

		if(ck_val){ neus.push(&neu); }
	}
}

void
neuron::fill_mutual_sortees(brain& brn){
	row<sortee*>& neu_mates = ne_reltee.so_mates;

	neu_mates.clear();
	BRAIN_CK(ne_reltee.so_opposite == NULL_PT);

	for(long aa = 0; aa < fib_sz(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);

		if(qua.has_note4()){
			sortrel& qua_sre = qua.qu_reltee;
			row<sortee*>& qua_mates = qua_sre.so_mates;
			qua_mates.push(&ne_tee);
			neu_mates.push(&(qua.qu_tee));
		}
	}
}

void
memap::map_anchor_stab(brain& brn){
	//skeleton_glb& skg = get_skeleton();
	BRAIN_CK(ck_map_guides(dbg_call_1));
	BRAIN_CK(ma_anchor_idx != INVALID_IDX);

	coloring& guide_col = ma_anchor_col;
	long nxt_idx = ma_anchor_idx;

	sort_glb& neus_srg = brn.br_tauto_neus_srg;
	sort_glb& quas_srg = brn.br_tauto_quas_srg;

	sort_id_t quas_consec = quas_srg.sg_curr_stab_consec;
	sort_id_t neus_consec = neus_srg.sg_curr_stab_consec;

	quas_consec = 0;
	neus_consec = 0;

	dima_dims dims0;

	DBG(long old_quas_sz = guide_col.co_quas.size());
	DBG(long old_neus_sz = guide_col.co_neus.size());
	BRAIN_CK(old_quas_sz > 0);
	BRAIN_CK(old_neus_sz > 0);

	brn.all_mutual_init();
	guide_col.load_colors_into(brn, neus_srg, quas_srg, dims0);
	neus_srg.stab_mutual(quas_srg);
	guide_col.save_colors_from(neus_srg, quas_srg);
	guide_col.co_szs_idx = nxt_idx;

	BRAIN_CK(old_quas_sz == guide_col.co_quas.size());
	BRAIN_CK(old_neus_sz == guide_col.co_neus.size());

	DBG_PRT(110, os << "simple mutual sz=" << ma_szs_dotted.size());
	BRAIN_CK(ck_guide_idx(guide_col, dbg_call_2));

	BRAIN_CK(ck_map_guides(dbg_call_3));
}

void
memap::get_initial_anchor_coloring(brain& brn, coloring& ini_anc_clr, long lst_idx, 
								   long nxt_idx)
{
	BRAIN_CK(ck_map_guides(dbg_call_1));

	ini_anc_clr.init_coloring();

	row_quanton_t&	all_quas = ini_anc_clr.co_quas;
	row<long>&	qua_colors = ini_anc_clr.co_qua_colors;

	row<neuron*>&	all_neus = ini_anc_clr.co_neus;
	row<long>&	neu_colors = ini_anc_clr.co_neu_colors;

	BRAIN_CK(lst_idx < nxt_idx);
	BRAIN_CK(ma_szs_dotted.is_valid_idx(nxt_idx));

	map_get_layer_quas(brn, all_quas, lst_idx, nxt_idx);
	map_get_layer_neus(all_neus, lst_idx, nxt_idx, true);

	qua_colors.fill(1, all_quas.size());
	neu_colors.fill(1, all_neus.size());

	BRAIN_CK(ini_anc_clr.ck_cols());
	BRAIN_CK(ck_map_guides(dbg_call_2));
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

	mpp.ma_anchor_col.move_co_to(ma_anchor_col);
	ma_anchor_idx = mpp.ma_anchor_idx;

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
coloring::move_co_to(coloring& col2){
	BRAIN_CK(col2.is_co_virgin());

	co_quas.move_to(col2.co_quas);
	co_qua_colors.move_to(col2.co_qua_colors);
	col2.co_all_qua_consec = co_all_qua_consec;

	co_neus.move_to(col2.co_neus);
	co_neu_colors.move_to(col2.co_neu_colors);
	col2.co_all_neu_consec = co_all_neu_consec;

	col2.co_szs_idx = co_szs_idx;

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

	col2.co_szs_idx = co_szs_idx;
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

void
split_tees(sort_glb& srg, row<sortee*>& sorted_tees, row<sortee*>& sub_tees, 
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
		sorset& ss1 = rmv1.get_vessel();
		ss1.set_ss_mark(srg);	
	}

	long last_tee_id = INVALID_NATURAL;

	for(long bb = 0; bb < sorted_tees.size(); bb++){
		BRAIN_CK(sorted_tees[bb] != NULL_PT);
		sortee& tee1 = *(sorted_tees[bb]);
		
		if(tee1.so_tee_consec != last_tee_id){
			BRAIN_CK(last_tee_id < tee1.so_tee_consec);
			last_tee_id = tee1.so_tee_consec;

			sorset& ss2 = tee1.get_vessel();
			if(ss2.ss_mark){
				ccls_in.push(&(tee1.so_ccl));
			} else {
				ccls_not_in.push(&(tee1.so_ccl));
			}
		}
	}

	for(long cc = 0; cc < sub_tees.size(); cc++){
		BRAIN_CK(sub_tees[cc] != NULL_PT);
		sortee& rmv1 = *(sub_tees[cc]);
		sorset& ss1 = rmv1.get_vessel();
		ss1.reset_ss_mark(srg);	
	}

	BRAIN_CK(srg.sg_tot_ss_marks == 0);
}

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

	// calc anchor

	if(ma_anchor_idx == INVALID_IDX){
		// BJ_FIX_THIS
		BRAIN_CK((mm == mo_find) || (mm == mo_save));
		BRAIN_CK(guide_col.is_co_virgin());
		if(mm == mo_save){
			DBG_PRT(122, os << "map_prep=" << this);
			BRAIN_CK(ma_anchor_col.is_co_virgin());
			ma_anchor_idx = op_szs_idx;
			get_initial_anchor_coloring(brn, ma_anchor_col, INVALID_IDX, ma_anchor_idx);
		}
		BRAIN_CK(ck_map_guides(dbg_call_2));
		return false;
	}

	BRAIN_CK(ma_anchor_idx != INVALID_IDX);

	if(ma_anchor_col.co_szs_idx == INVALID_IDX){
		map_anchor_stab(brn);
		BRAIN_CK(ma_anchor_col.co_szs_idx == ma_anchor_idx);
		BRAIN_CK(ma_anchor_col.co_szs_idx != INVALID_IDX);
	}

	if(guide_col.co_szs_idx == INVALID_IDX){
		BRAIN_CK(guide_col.is_co_virgin());
		ma_anchor_col.copy_co_to(guide_col);
		BRAIN_CK(guide_col.co_szs_idx == ma_anchor_idx);
	}

	BRAIN_CK(op_szs_idx != INVALID_IDX);
	BRAIN_CK(ma_anchor_idx != INVALID_IDX);
	BRAIN_CK(guide_col.co_szs_idx != INVALID_IDX);

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
				neu.ne_recoil_tk.update_ticket(&brn);
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

