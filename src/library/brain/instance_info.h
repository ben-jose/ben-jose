

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

//=================================================================
// decl
	
class instance_info;

DECLARE_PRINT_FUNCS(instance_info)

//=================================================================
// consecutive

//typedef long 	consecutive_t;
typedef bj_big_int_t 	consecutive_t;

#define INVALID_CONSECUTIVE	-1

#define INVALID_QUANTON_ID	0
#define INVALID_POLARITY	0
#define INVALID_LAYER		-1

//======================================================================
// instance_exception

typedef enum {
	inx_bad_eq_op,
	inx_bad_creat,
	inx_bad_lit
} in_ex_cod_t;

class instance_exception : public top_exception {
public:
	instance_exception(long the_id = 0) : top_exception(the_id)
	{}
};

//=================================================================
// instance_info

class instance_info {
private:
	instance_info&  operator = (instance_info& other){
		throw instance_exception(inx_bad_eq_op);
	}

	instance_info(instance_info& other){ 
		throw instance_exception(inx_bad_creat);
	}
	
public:
	bool			ist_with_assig;
	
	long			ist_group_id;
	long			ist_id;
	
	ch_string		ist_file_path;

	row<char> 		ist_data;

	long			ist_num_vars;
	long			ist_num_ccls;
	row<long> 		ist_ccls;
	
	double			ist_in_timeout;
	double			ist_in_memout;
	
	bj_output_t		ist_out;

	avg_stat		ist_num_variants_stat;
	row<long> 		ist_assig;
	
	instance_info(){
		init_instance_info(true);
	}

	void	init_instance_info(bool reset_lims = false, bool free_mem = true){
		ist_with_assig = false;
		
		ist_group_id = -1;
		ist_id = -1;
	
		ist_file_path = "";

		ist_data.clear(free_mem, free_mem);
		
		ist_num_vars = 0;
		ist_num_ccls = 0;
		ist_ccls.clear(free_mem, free_mem);
		
		if(reset_lims){
			ist_in_timeout = -1;
			ist_in_memout = -1;
		}
		
		init_output(ist_out);

		ist_assig.clear(free_mem, free_mem);
	}
	
	static
	void	init_output(bj_output_t& out){	
		out.bjo_result = bjr_unknown_satisf;
		
		out.bjo_solve_time = 0.0;
		out.bjo_num_vars = 0;
		out.bjo_num_ccls = 0;
		out.bjo_num_lits = 0;
		
		out.bjo_num_laps = 0.0;
		
		out.bjo_load_time = 0.0;
		out.bjo_saved_targets = 0.0;
		out.bjo_max_variants = 0.0;
		out.bjo_avg_variants = 0.0;
		out.bjo_num_finds = 0.0;
		out.bjo_quick_discards = 0.0;
		out.bjo_old_pth_hits = 0.0;
		out.bjo_new_pth_hits = 0.0;
		out.bjo_sub_cnf_hits = 0.0;
		out.bjo_eq_new_hits = 0.0;
		out.bjo_eq_old_hits = 0.0;
		out.bjo_sb_new_hits = 0.0;
		out.bjo_sb_old_hits = 0.0;
		
		out.bjo_error = bje_no_error;
		out.bjo_err_char = 0;
		out.bjo_err_line = -1;
		out.bjo_err_pos = -1;
		out.bjo_err_num_decl_cls = -1;
		out.bjo_err_num_decl_vars = -1;
		out.bjo_err_num_read_cls = -1;
		out.bjo_err_bad_lit = -1;
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
	
	bj_output_t& 	get_out_info(){
		return ist_out;
	}

	static
	bj_ostream& 	print_headers(bj_ostream& os);

	bj_ostream& 	print_instance_info(bj_ostream& os, bool from_pt = false);
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
ch_string
as_ist_satisf_str(bj_satisf_val_t vv){
	ch_string sf_str = RES_UNKNOWN_STR;
	switch(vv){
		case bjr_unknown_satisf:
			sf_str = RES_UNKNOWN_STR;
			break;
		case bjr_yes_satisf:
			sf_str = RES_YES_SATISF_STR;
			break;
		case bjr_no_satisf:
			sf_str = RES_NO_SATISF_STR;
			break;
		case bjr_error:
			sf_str = RES_ERROR_STR;
			break;
	}
	return sf_str;
}

inline
bj_ostream& 	
instance_info::print_instance_info(bj_ostream& os, bool from_pt){
	ch_string sep = RESULT_FIELD_SEP;
	os << ist_file_path << sep;
	os << as_ist_satisf_str(ist_out.bjo_result) << sep;
	os << ist_out.bjo_solve_time << sep;
	os << ist_out.bjo_num_vars << sep;
	os << ist_out.bjo_num_ccls << sep;
	os << ist_out.bjo_num_lits << sep;
	os << ist_out.bjo_num_laps << sep;
	return os;
}

DEFINE_PRINT_FUNCS(instance_info)


#endif		// INSTANCE_INF_H


