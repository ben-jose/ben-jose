

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

instance_info.h

all info to keep or return of an instance cnf to solve.

--------------------------------------------------------------*/

#ifndef INSTANCE_INF_H
#define INSTANCE_INF_H

#include "bj_big_number.h"
#include "tools.h"
#include "ch_string.h"
#include "ben_jose.h"


//=================================================================
// defs

#define INSTANCE_CK(prm) DBG_CK(prm)


#define RESULT_FIELD_SEP		"|"
#define RESULT_FIELD_SEP_CHAR		'|'

//=================================================================
// decl
	
class instance_info;

DECLARE_PRINT_FUNCS(instance_info)

//=================================================================
// consecutive

//typedef long 	consecutive_t;
typedef bj_big_int_t 	consecutive_t;

#define INVALID_CONSECUTIVE	-1

//======================================================================
// instance_exception

class instance_exception : public top_exception {
public:
	instance_exception(char* descr = as_pt_char("undefined instance exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

//=================================================================
// aux funcs

#define RES_UNKNOWN_STR "unknown"
#define RES_YES_SATISF_STR "yes_satisf"
#define RES_NO_SATISF_STR "no_satisf"
#define RES_TIMEOUT_STR "timeout"
#define RES_MEMOUT_STR "memout"
#define RES_ERROR_STR "error"

inline
satisf_val
as_satisf(ch_string str_ln){
	satisf_val the_val = k_unknown_satisf;
	if(str_ln == RES_UNKNOWN_STR){
		the_val = k_unknown_satisf;
	} else if(str_ln == RES_YES_SATISF_STR){
		the_val = k_yes_satisf;
	} else if(str_ln == RES_NO_SATISF_STR){
		the_val = k_no_satisf;
	} else if(str_ln == RES_TIMEOUT_STR){
		the_val = k_timeout;
	} else if(str_ln == RES_MEMOUT_STR){
		the_val = k_memout;
	} else if(str_ln == RES_ERROR_STR){
		the_val = k_error;
	}
	return the_val;
}

inline
ch_string
as_satisf_str(satisf_val vv){
	ch_string sf_str = RES_UNKNOWN_STR;
	switch(vv){
		case k_unknown_satisf:
			sf_str = RES_UNKNOWN_STR;
			break;
		case k_yes_satisf:
			sf_str = RES_YES_SATISF_STR;
			break;
		case k_no_satisf:
			sf_str = RES_NO_SATISF_STR;
			break;
		case k_timeout:
			sf_str = RES_TIMEOUT_STR;
			break;
		case k_memout:
			sf_str = RES_MEMOUT_STR;
			break;
		case k_error:
			sf_str = RES_ERROR_STR;
			break;
	}
	return sf_str;
}

//=================================================================
// inst_out_info

class inst_out_info {
public:
	satisf_val		iot_result;
	double			iot_solve_time;
	long			iot_num_vars;
	long			iot_num_ccls;
	long			iot_num_lits;
	consecutive_t	iot_num_laps;

	
	double			iot_load_tm;
	double			iot_saved_targets;
	double			iot_old_hits;
	double			iot_old_sub_hits;
	double			iot_new_hits;
	double			iot_new_sub_hits;
	
	row<long>		iot_final_assig;

	inst_out_info(){
		init_inst_out_info();
	}

	void	init_inst_out_info(){
		iot_result = k_unknown_satisf;
		iot_solve_time = 0.0;
		iot_num_vars = 0;
		iot_num_ccls = 0;
		iot_num_lits = 0;
		iot_num_laps = 0;
		
		iot_load_tm = 0.0;
		iot_saved_targets = 0.0;
		iot_old_hits = 0.0;
		iot_old_sub_hits = 0.0;
		iot_new_hits = 0.0;
		iot_new_sub_hits = 0.0;
	}

	bj_ostream& 	
	print_inst_out_info(bj_ostream& os, bool from_pt){
		ch_string sep = RESULT_FIELD_SEP;
		os << as_satisf_str(iot_result) << sep;
		os << iot_solve_time << sep;
		os << iot_num_vars << sep;
		os << iot_num_ccls << sep;
		os << iot_num_lits << sep;
		os << iot_num_laps << sep;
		return os;
	}
};

//=================================================================
// instance_info

class instance_info {
private:
	instance_info&  operator = (instance_info& other){
		MARK_USED(other);
		char* inst_bad_eq_op = as_pt_char("operator = not allowed in instance_info");
		DBG_THROW_CK(inst_bad_eq_op != inst_bad_eq_op);
		throw instance_exception(inst_bad_eq_op);
		abort_func(0, inst_bad_eq_op);
		return (*this);
	}

	instance_info(instance_info& other){ 
		MARK_USED(other);
		char* inst_bad_creat = as_pt_char("copy creator instance_info not allowed");
		DBG_THROW_CK(inst_bad_creat != inst_bad_creat);
		throw instance_exception(inst_bad_creat);
		abort_func(0, inst_bad_creat);
	}
	
public:
	long			ist_group_id;
	long			ist_id;
	
	ch_string		ist_file_path;

	row<char> 		ist_data;

	long			ist_num_vars;
	long			ist_num_ccls;
	row<long> 		ist_ccls;
	
	inst_out_info	ist_out;
	
	instance_info(){
		init_instance_info();
	}

	void	init_instance_info(){
		ist_group_id = -1;
		ist_id = -1;
	
		ist_file_path = "Unknown path";

		ist_data.clear();
		
		ist_num_vars = 0;
		ist_num_ccls = 0;
		ist_ccls.clear();		
	}
	
	ch_string&	get_f_nam(){
		return ist_file_path;
	}
	
	bool is_read(){
		return (! ist_data.is_empty());
	}

	bool is_parsed(){
		bool c1 = ! ist_ccls.is_empty();
		bool c2 = (ist_num_vars >= 0);
		bool c3 = (ist_num_ccls >= 0);
		bool is_p = (c1 && c2 && c3);
		return is_p;
	}

	static
	bj_ostream& 	print_headers(bj_ostream& os);

	bj_ostream& 	print_instance_info(bj_ostream& os, bool from_pt = false);

	void		parse_instance(ch_string str_ln, long line);
	ch_string	parse_field(const char*& pt_in);
};


//=================================================================
// funcs


inline
bj_ostream& 	
instance_info::print_headers(bj_ostream& os){
	ch_string sep = RESULT_FIELD_SEP;
	os << "file_path" << sep;
	os << "solve_time" << sep;
	os << "result" << sep;
	os << "#vars" << sep;
	os << "#ccls" << sep;
	os << "#lits" << sep;
	os << "#laps" << sep;
	return os;
}

inline
bj_ostream& 	
instance_info::print_instance_info(bj_ostream& os, bool from_pt){
	ch_string sep = RESULT_FIELD_SEP;
	os << ist_file_path << sep;
	os << as_satisf_str(ist_out.iot_result) << sep;
	os << ist_out.iot_solve_time << sep;
	os << ist_out.iot_num_vars << sep;
	os << ist_out.iot_num_ccls << sep;
	os << ist_out.iot_num_lits << sep;
	os << ist_out.iot_num_laps << sep;
	return os;
}

DEFINE_PRINT_FUNCS(instance_info)


#endif		// INSTANCE_INF_H

