

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

dbg_run_satex.cpp  

dbg_run_satex func.

--------------------------------------------------------------*/

#include <sstream>
#include <execinfo.h>
#include <cstdlib>

#include "bj_stream.h"
#include "ch_string.h"
#include "brain.h"
#include "dbg_run_satex.h"

bool
dbg_run_satex_on(brain& brn, ch_string f_nam){
	if(path_exists(f_nam)){
		ch_string o_str = "satex -s " + f_nam;

		system_exec(o_str);
		ch_string lg_nm = get_log_name(f_nam, LOG_NM_RESULTS);

		BRAIN_CK(path_exists(lg_nm));
		bool is_no_sat = all_results_batch_instances(lg_nm, k_no_satisf);
		MARK_USED(is_no_sat);

		DBG_PRT_COND(DBG_ALL_LVS, ! is_no_sat ,
			os << "ABORTING_DATA " << bj_eol;
			//os << "mmap_before_tk=" << ma_before_retract_tk << bj_eol;
			//os << "mmap_after_tks=" << ma_after_retract_tks << bj_eol;
			os << " brn_tk=" << brn.br_current_ticket << bj_eol;
			os << "	LV=" << brn.level() << bj_eol;
			os << " f_nam=" << f_nam << bj_eol;
			os << " save_consec=" << skg_dbg_canon_save_id << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		BRAIN_CK(is_no_sat);
	}
	return true;
}

void system_exec(ch_string& comm){
	const char* comm_cstr = comm.c_str();
	system(comm_cstr);
}


