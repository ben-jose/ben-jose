

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

void
brain::new_reverse(){
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
	BRAIN_CK(br_semi_monos_to_update.is_empty());

	DBG_PRT(122, print_trail(os));

	// START REVERSE (init mpp0 and nke0)

	BRAIN_CK(found_conflict());
	BRAIN_DBG(br_dbg.dbg_before_retract_lv = level());

	DBG(
		neuron* cfl = br_conflict_found.ps_source;
		BRAIN_CK(cfl != NULL_PT);
		if(! cfl->ne_original){ 
			DBG_PRT(117, os << "NOT_ORIG_CONFL=" << cfl 
				<< " creat_tk=" << cfl->ne_dbg_creation_tk);
		}
	)
	
	BRAIN_CK(br_conflict_found.ps_source != NULL_PT);
	BRAIN_CK(br_conflict_found.ps_source->ne_original);
	
	//mpp0.ma_confl = br_conflict_found;
	//mpp0.ma_before_retract_tk.update_ticket(brn);
	//BRAIN_CK(! mpp0.is_ma_virgin());
	
	// analize

	br_deducer.deduction_analysis(br_conflict_found, dct);

	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, dbg_prt_lvs_have_learned(os));
	
	//quanton* chosen_qua = NULL_PT;
	bool has_in_mem = false;
	MARK_USED(has_in_mem);

	// reverse loop
	retract_to(dct.dt_target_level);
	
	// some checks

	DBG_PRT(131, 
			print_trail(os);
			dbg_prt_lvs_cho(os);
			os << "tr_lv=" << trail_level() << " tg_lv=" << dct.dt_target_level;
	);
	BRAIN_DBG(br_dbg.dbg_last_recoil_lv = dct.dt_target_level);

	DBG(long rr_lv = trail_level());
	BRAIN_CK((level() == ROOT_LEVEL) || (level() == dct.dt_target_level));
	BRAIN_CK((level() == ROOT_LEVEL) || (rr_lv == dct.dt_target_level));

	// update leveldat
	
	memap& lv_map0 = data_level().ld_map0;
	if(! mpp0.is_ma_virgin() && lv_map0.is_ma_virgin()){
		ticket& n_tk = mpp0.ma_after_retract_tks.inc_sz();
		n_tk.update_ticket(brn);

		BRAIN_CK(mpp0.ck_map_guides(dbg_call_2));
		lv_map0.map_replace_with(brn, mpp0, dbg_call_2);
		lv_map0.map_activate(brn);
		DBG_PRT(122, os << "Updated " << &(data_level()) << " with " << &lv_map0);
	}
	DBG(	
		if(! mpp0.is_ma_virgin() && ! lv_map0.is_ma_virgin()){
			BRAIN_CK(mpp0.map_ck_all_qu_dominated(brn));
		}
	); 

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
	DBG_PRT(122, dbg_prt_lvs_active(os));
	DBG_PRT(122, print_trail(os); os << dct << bj_eol);

	reset_conflict();
	BRAIN_CK(! found_conflict());
}

