

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

dbg_prt_brn.cpp  

dbg_prt_brn funcs.

--------------------------------------------------------------*/

#include <fstream>

#include "bj_stream.h"
#include "ch_string.h"
#include "config.h"
#include "brain.h"
#include "support.h"
#include "dbg_prt.h"
#include "dbg_run_satex.h"

row<bool>* dbg_lv_arr = NULL_PT;
bool	dbg_bad_cycle1 = false;		// dbg_print_cond_func

bool
dbg_has_lv_arr(){
	return (dbg_lv_arr != NULL_PT);
}

row<bool>&
dbg_get_lv_arr(){
	if(! dbg_has_lv_arr()){
		abort_func(0, "ASKING FOR NULL dbg_lv_arr");
	}
	return (*dbg_lv_arr);
}

void
dbg_init_lv_arr(row<bool>& nw_lv_arr){
	dbg_lv_arr = &nw_lv_arr;
	
	long num_levs = DBG_NUM_LEVS;
	nw_lv_arr.set_cap(num_levs);
	for(int ii = 0; ii < num_levs; ii++){
		nw_lv_arr.inc_sz() = false;
	}
}

void 
dbg_prt_all_cho(brain& brn){
	bool is_batch = false;
	ch_string f_nam = GLB().get_file_name(is_batch);
	ch_string cho_nam = f_nam + "_chosen.log";

	const char* log_nm = cho_nam.c_str();
	std::ofstream log_stm;
	//log_stm.open(log_nm, std::ios::app);
	log_stm.open(log_nm, std::ios::binary);
	if(! log_stm.good() || ! log_stm.is_open()){
		ch_string msg = "Could not open file " + cho_nam;
		abort_func(1, msg.c_str());
	}

	//dbg_get_all_chosen(br_dbg_all_chosen);

	log_stm << brn.br_dbg_all_chosen;
	log_stm << bj_eol; 
	log_stm.close();
}

bool	dbg_print_cond_func(bool prm, bool is_ck, const ch_string fnam, int lnum,
		const ch_string prm_str, long dbg_lv)
{
	DBG_CK(! dbg_bad_cycle1);
	dbg_bad_cycle1 = true;

	bool resp = true;
	if(prm){
		bj_ostream& os = bj_dbg;
		if(dbg_lv != INVALID_DBG_LV){
			if(dbg_lv == DBG_ALL_LVS){
				os << "ckALL.";
			} else {
				os << "ck" << dbg_lv << ".";
			}
		}
		if(GLB().batch_num_files > 1){
			os << "#" << GLB().batch_consec << ".";
		}

		consecutive_t the_lap = GLB().get_curr_lap();
		if(the_lap > 0){
			if(is_ck){ os << "LAP="; }
			os << the_lap << ".";
		}

		if(is_ck){
			os << "ASSERT '" << prm_str << "' FAILED (";
			os << GLB().get_curr_f_nam();
			os << ")";
			os << " in " << fnam << " at " << lnum;
			os << bj_eol;
		}
		os.flush();
		resp = (! is_ck);
	}

	dbg_bad_cycle1 = false;
	return resp;
}

