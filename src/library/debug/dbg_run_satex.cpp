

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

#include <fstream>


#include "bj_stream.h"
#include "ch_string.h"
#include "file_funcs.h"
#include "brain.h"
#include "dbg_run_satex.h"

void
read_batch_instances(ch_string file_nm, row<instance_info>& f_insts){
	bj_ostream& os = bj_out;

	std::ifstream in_stm;

	in_stm.open(file_nm.c_str(), std::ios::binary);
	if(! in_stm.good() || ! in_stm.is_open()){
		os << "NO " << file_nm << " FILE FOUND." << bj_eol;
		return;
	}

	ch_string str_ln;
	long num_ln = 0;

	f_insts.clear();

	while(! in_stm.eof()){
		std::getline(in_stm, str_ln);
		num_ln++;

		if(! str_ln.empty()){
			instance_info& n_inst = f_insts.inc_sz();
		
			//os << "Lei:<<" << str_ln << ">>" << bj_eol;
			n_inst.parse_instance(str_ln, num_ln);
		}
	}
	in_stm.close();
}

bool
all_results_batch_instances(ch_string file_nm, bj_satisf_val_t r_val){
	row<instance_info> f_insts;
	read_batch_instances(file_nm, f_insts);
	bool all_ok = true;

	for(long aa = 0; aa < f_insts.size(); aa++){
		instance_info& inst_info = f_insts[aa];
		if(inst_info.ist_out.bjo_result != r_val){
			all_ok = false;
		}
	}
	return all_ok;
}

bool
dbg_run_satex_on(brain& brn, ch_string f_nam){
#ifdef FULL_DEBUG
	if(file_exists(f_nam)){
		ch_string o_str = "satex -s " + f_nam;

		system_exec(o_str);
		ch_string lg_nm = get_log_name(f_nam, LOG_NM_RESULTS);

		BRAIN_CK(file_exists(lg_nm));
		bool is_no_sat = all_results_batch_instances(lg_nm, k_no_satisf);
		MARK_USED(is_no_sat);

		DBG_COND_COMM(! is_no_sat ,
			os << "ABORTING_DATA " << bj_eol;
			//os << "mmap_before_tk=" << ma_before_retract_tk << bj_eol;
			//os << "mmap_after_tks=" << ma_after_retract_tks << bj_eol;
			os << " brn_tk=" << brn.br_current_ticket << bj_eol;
			os << "	LV=" << brn.level() << bj_eol;
			os << " f_nam=" << f_nam << bj_eol;
			os << " save_consec=" << brn.br_dbg.dbg_canon_save_id << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		BRAIN_CK(is_no_sat);
	}
#endif
	return true;
}

void system_exec(ch_string& comm){
	const char* comm_cstr = comm.c_str();
	system(comm_cstr);
}


