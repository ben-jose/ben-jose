

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

Copyright (C) 2011, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

dbg_run_satex.cpp  

dbg_run_satex func.

--------------------------------------------------------------*/

#include <fstream>


#include "bj_stream.h"
#include "ch_string.h"
#include "file_funcs.h"
//include "brain.h"
//include "batch_log.h"
#include "ben_jose.h"
#include "dbg_run_satex.h"

//=================================================================
// defs

#define LOG_SATEX_NM_RESULTS	"results.log"
#define RESULT_SATEX_FIELD_SEP_CHAR '|'

#define RES_SATEX_UNKNOWN_STR "unknown"
#define RES_SATEX_YES_SATISF_STR "yes_satisf"
#define RES_SATEX_NO_SATISF_STR "no_satisf"
#define RES_SATEX_ERROR_STR "error"

inline
bj_satisf_val_t
satex_as_satisf(ch_string str_ln){
	bj_satisf_val_t the_val = bjr_unknown_satisf;
	if(str_ln == RES_SATEX_UNKNOWN_STR){
		the_val = bjr_unknown_satisf;
	} else if(str_ln == RES_SATEX_YES_SATISF_STR){
		the_val = bjr_yes_satisf;
	} else if(str_ln == RES_SATEX_NO_SATISF_STR){
		the_val = bjr_no_satisf;
	} else if(str_ln == RES_SATEX_ERROR_STR){
		the_val = bjr_error;
	}
	return the_val;
}


//=================================================================
// decl

ch_string 
satex_parse_field(const char*& pt_in);

//=================================================================
// satex_entry

class satex_entry {
public:
	ch_string 		ist_file_path;
	bj_output_t 	ist_out;
	
	satex_entry(){
		init_satex_entry();
	}
	
	~satex_entry(){
	}
	
	void init_satex_entry(){
		ist_file_path = "";
		bj_init_output(&ist_out);
	}

	void
	parse_entry(ch_string str_ln, long line){
		init_satex_entry();

		const char* pt_in = str_ln.c_str();
		ist_file_path = satex_parse_field(pt_in);
		ch_string r_fi = satex_parse_field(pt_in);
		ist_out.bjo_result = satex_as_satisf(r_fi);
	}
};

//=================================================================
// aux funcs

ch_string 
satex_parse_field(const char*& pt_in){
	char sep = RESULT_SATEX_FIELD_SEP_CHAR;
	char eol = '\n';

	ch_string the_field = "";
	const char* pt_0 = pt_in;

	while(*pt_in != 0){
		if(*pt_in == sep){ 
			// get the field
			char* pt_1 = (char*)pt_in;
			(*pt_1) = 0;
			the_field = pt_0;
			(*pt_1) = sep;
			RSATX_CK((*pt_in) == sep);

			pt_in++; 
			break;
		}
		if(! isprint(*pt_in) || (*pt_in == eol)){
			break;
		}
		pt_in++; 
	}
	return the_field;
}


ch_string
get_satex_log_name(ch_string f_nam, ch_string sufix){
	ch_string lg_nm = f_nam + "_" + sufix;
	return lg_nm;
}

void
read_batch_instances(ch_string file_nm, row<satex_entry>& f_insts){
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
			satex_entry& n_inst = f_insts.inc_sz();
		
			//os << "Lei:<<" << str_ln << ">>" << bj_eol;
			n_inst.parse_entry(str_ln, num_ln);
		}
	}
	in_stm.close();
}

bool
all_results_batch_instances(ch_string file_nm, bj_satisf_val_t r_val){
	row<satex_entry> f_insts;
	read_batch_instances(file_nm, f_insts);
	bool all_ok = true;

	for(long aa = 0; aa < f_insts.size(); aa++){
		satex_entry& inst_info = f_insts[aa];
		if(inst_info.ist_out.bjo_result != r_val){
			all_ok = false;
		}
	}
	return all_ok;
}

bool
dbg_run_satex_is_no_sat(ch_string f_nam){
	bool is_no_sat = true;
#ifdef FULL_DEBUG

	if(file_exists(f_nam)){
		ch_string o_str = "satex -s " + f_nam;

		system_exec(o_str);
		ch_string lg_nm = get_satex_log_name(f_nam, LOG_SATEX_NM_RESULTS);

		RSATX_CK(file_exists(lg_nm));
		is_no_sat = all_results_batch_instances(lg_nm, bjr_no_satisf);
		MARK_USED(is_no_sat);
	}
#endif
	return is_no_sat;
}

void system_exec(ch_string& comm){
	const char* comm_cstr = comm.c_str();
	system(comm_cstr);
}


