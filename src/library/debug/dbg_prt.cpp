

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

dbg_prt_brn.cpp  

dbg_prt_brn funcs.

--------------------------------------------------------------*/

#include <fstream>

#include "bj_stream.h"
#include "ch_string.h"
#include "brain.h"
#include "dbg_config.h"
#include "dbg_prt.h"
#include "dbg_run_satex.h"

bool
dbg_check_lev(brain* brn, long lev){
	bool lv_ok = false;
#ifdef FULL_DEBUG
	if(brn == NULL){ return false; }
	brain* pt_b = brn->get_dbg_brn();
	if(pt_b == NULL){ return false; }
	if(lev < 0){ return true; }
	row<bool>& lvs_arr = pt_b->br_dbg.dbg_levs_arr;
	if(! lvs_arr.is_valid_idx(lev)){ return false; }
	lv_ok = lvs_arr[lev];
#endif
	return lv_ok;
}

void 
dbg_prt_all_cho(brain& brn){
#ifdef FULL_DEBUG
	//bool is_batch = false;
	ch_string f_nam = brn.get_my_inst().get_f_nam();
	ch_string cho_nam = f_nam + "_chosen.log";

	const char* log_nm = cho_nam.c_str();
	std::ofstream log_stm;
	//log_stm.open(log_nm, std::ios::app);
	log_stm.open(log_nm, std::ios::binary);
	if(! log_stm.good() || ! log_stm.is_open()){
		ch_string msg = "Could not open file " + cho_nam;
		abort_func(1, msg.c_str());
	}

	//dbg_get_all_chosen(br_dbg.dbg_all_chosen);

	log_stm << brn.br_dbg.dbg_all_chosen;
	log_stm << bj_eol; 
	log_stm.close();
#endif
}

bool	dbg_print_cond_func(brain* brn, bool prm, bool is_ck, const ch_string fnam, int lnum,
		const ch_string prm_str, long dbg_lv)
{
	bool resp = true;
#ifdef FULL_DEBUG
	if(brn == NULL){
		return false;
	}
	bool& bad_cy = brn->br_dbg.dbg_bad_cycle1;
	
	DBG_CK(! bad_cy);
	bad_cy = true;

	if(prm){
		bj_ostream& os = bj_dbg;
		if(dbg_lv != INVALID_DBG_LV){
			if(dbg_lv == DBG_ALL_LVS){
				os << "ckALL.";
			} else {
				os << "ck" << dbg_lv << ".";
			}
		}
		
		ch_string f_nam = "UNKNOWN_FILE_NAME";
		if(brn != NULL){
			f_nam = brn->dbg_prt_margin(os, is_ck);
		}

		if(is_ck){
			os << "ASSERT '" << prm_str << "' FAILED (";
			os << f_nam;
			os << ")";
			os << " in " << fnam << " at " << lnum;
			os << bj_eol;
		}
		os.flush();
		resp = (! is_ck);
	}

	bad_cy = false;
#endif
	return resp;
}

