

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

dbg_prt_brn.cpp  

dbg_prt_brn funcs.

--------------------------------------------------------------*/

#include "bj_stream.h"
#include "ch_string.h"
#include "solver.h"
#include "brain.h"
#include "dbg_config.h"
#include "dbg_prt.h"
#include "dbg_run_satex.h"

bool
dbg_check_lev(solver* slv, long lev){
	bool lv_ok = false;
#ifdef FULL_DEBUG
	if(slv == NULL){ return false; }
	if(lev < 0){ return true; }
	
	row<bool>& lvs_arr = slv->slv_dbg_conf_info.dbg_levs_arr;
	if(! lvs_arr.is_valid_idx(lev)){ return false; }
	lv_ok = lvs_arr[lev];
#endif
	return lv_ok;
}

bj_ostream&
dbg_get_out_stm(solver* slv){
#ifdef FULL_DEBUG
	if(slv == NULL_PT){
		return bj_dbg;
	}
	if(slv->slv_dbg_brn == NULL_PT){
		return bj_dbg;
	}
	bj_ostream* htm_os = NULL_PT;
	BRAIN_DBG(htm_os = &(slv->slv_dbg_brn->br_dbg_htm_os));
	if(htm_os == NULL_PT){
		return bj_dbg;
	}
	bj_ofstream& of = slv->slv_dbg_brn->br_dbg_htm_os;
	if(! of.good() || ! of.is_open()){
		return bj_dbg;
	}
	BRAIN_CK(htm_os != NULL_PT);
	return *(htm_os);
#else
	return bj_dbg;
#endif
}

void 
dbg_prt_all_cho(brain& brn){
#ifdef FULL_DEBUG
	//bool is_batch = false;
	ch_string f_nam = brn.get_my_inst().get_f_nam();
	ch_string cho_nam = f_nam + "_chosen.log";

	const char* log_nm = cho_nam.c_str();
	bj_ofstream log_stm;
	//log_stm.open(log_nm, std::ios::app);
	log_stm.open(log_nm, std::ios::binary);
	if(! log_stm.good() || ! log_stm.is_open()){
		ch_string msg = "Could not open file " + cho_nam;
		abort_func(1, msg.c_str());
	}

	log_stm << brn.br_dbg.dbg_all_chosen;
	log_stm << bj_eol; 
	log_stm.close();
#endif
}

void	dbg_print_left_margin(solver* slv, bj_ostream& os, long dbg_lv)
{
#ifdef FULL_DEBUG
	if(slv == NULL){
		return;
	}
	bool& bad_cy = slv->slv_dbg_conf_info.dbg_bad_cycle1;
	
	DBG_CK(! bad_cy);
	bad_cy = true;

	if(dbg_lv != INVALID_DBG_LV){
		if(dbg_lv == DBG_ALL_LVS){
			os << "ckALL.";
		} else {
			os << "ck" << dbg_lv << ".";
		}
	}
	
	if(slv->slv_dbg_brn != NULL){
		slv->slv_dbg_brn->dbg_prt_margin(os, false);
	}

	os.flush();

	bad_cy = false;
#endif
}

void
dbg_prt_open(ch_string& path, bj_ofstream& stm, bool append){
#ifdef FULL_DEBUG
	/*const char* log_nm = path.c_str();
	if(append){
		stm.open(log_nm, std::ios::app);
	} else {
		stm.open(log_nm, std::ios::binary);
	}
	if(! stm.good() || ! stm.is_open()){
		ch_string msg = "Could not open file " + path;
		abort_func(1, msg.c_str());
	}*/
	bj_ofstream_open(path, stm, append);
#endif
}

