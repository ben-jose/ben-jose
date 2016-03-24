

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

batch_solver.h

Declaration of classes that batch solving.

--------------------------------------------------------------*/

#ifndef BATCH_SOLVER_H
#define BATCH_SOLVER_H

//=================================================================
// configuration defs

#define DO_FINAL_GETCHAR
#define PRINT_TOTALS_PERIOD 		10.0

//=================================================================

#include "tools.h"
#include "util_funcs.h"
#include "ben_jose.h"


#define BATCH_CK(prm) DBG_CK(prm)

#define LOG_NM_ERROR	"error.log"
#define LOG_NM_RESULTS	"results.log"
#define LOG_NM_STATS	"stats.log"
#define LOG_NM_ASSIGS	"assigs.log"

//=================================================================
// pre-configuration decl

#define DO_PRINTS(prm)		prm

#define PRT_OUT_0(comm) \
	DO_PRINTS( \
		{ \
			bj_ostream& os = bj_out; \
			comm; \
			os.flush(); \
		} \
	) \
			
//--end_of_def

#define PRT_OUT_1(comm) /**/

//=================================================================
// aux funcs

inline
ch_string
get_log_name(ch_string f_nam, ch_string sufix){
	ch_string lg_nm = f_nam + "_" + sufix;
	return lg_nm;
}

inline
bj_satisf_val_t
as_satisf(ch_string str_ln){
	bj_satisf_val_t the_val = bjr_unknown_satisf;
	if(str_ln == RES_UNKNOWN_STR){
		the_val = bjr_unknown_satisf;
	} else if(str_ln == RES_YES_SATISF_STR){
		the_val = bjr_yes_satisf;
	} else if(str_ln == RES_NO_SATISF_STR){
		the_val = bjr_no_satisf;
	} else if(str_ln == RES_ERROR_STR){
		the_val = bjr_error;
	}
	return the_val;
}

inline
ch_string
as_satisf_str(bj_satisf_val_t vv){
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

//=================================================================
// decl
	
class batch_entry;

DECLARE_PRINT_FUNCS(batch_entry)

//=================================================================
// batch_entry

class batch_entry {
public:
	ch_string	be_ff_nam;
	bj_output_t	be_out;
	
	batch_entry(){
		be_ff_nam = "";
		bj_init_output(&be_out);
	}
	
	~batch_entry(){
	}
	
	bj_ostream& 	print_batch_entry(bj_ostream& os, bool from_pt = false);
};

//=================================================================
// batch_solver

long		get_free_mem_kb();

class batch_solver {
public:
	bool			using_mem_ctrl;

	ch_string		help_str;
	ch_string		version_str;

	bool			op_debug_clean_code;
	bool			op_write_proof;

	mem_size 		dbg_mem_at_start;

	bool			dbg_skip_print_info;
	
	bj_ostr_stream	error_stm;
	long			error_cod;

	ch_string		input_file_nm;

	bool			batch_log_on;
	ch_string		batch_name;
	ch_string		batch_log_name;
	ch_string		batch_end_log_name;
	ch_string		batch_end_msg_name;
	ch_string		batch_answer_name;

	integer			batch_num_files;
	integer			batch_consec;
	integer			batch_num_unknown_satisf;
	integer			batch_num_yes_satisf;
	integer			batch_num_no_satisf;
	integer			batch_num_timeout;
	integer			batch_num_memout;
	integer			batch_num_error;

	avg_stat		batch_stat_laps;
	avg_stat		batch_stat_solve_tm;
	avg_stat		batch_stat_mem_used;

	avg_stat		batch_stat_load_tm;
	avg_stat		batch_stat_saved_targets;
	avg_stat		batch_stat_variants;
	avg_stat		batch_stat_num_finds;
	avg_stat		batch_stat_quick_discards;
	avg_stat		batch_stat_old_pth_hits;
	avg_stat		batch_stat_new_pth_hits;
	avg_stat		batch_stat_sub_cnf_hits;
	avg_stat		batch_stat_eq_new_hits;
	avg_stat		batch_stat_eq_old_hits;
	avg_stat		batch_stat_sb_new_hits;
	avg_stat		batch_stat_sb_old_hits;
	

	double			batch_start_time;
	double			batch_end_time;
	timer			batch_prt_totals_timer;

	row<batch_entry>	batch_instances;

	ch_string		gg_file_name;

	bool			bc_as_release;
	ch_string 		bc_slvr_path;
	bj_solver_t 	bc_solver;

	void 		init_batch_solver();
	void 		finish_batch_solver();

	batch_solver();
	~batch_solver();

	void		reset_global(){
		reset_err_msg();
		error_cod = -1;
	}

	bool		get_args(int argc, char** argv);
	void		set_input_name();

	batch_entry&	get_curr_inst(){
		long batch_idx = batch_consec - 1;
		DBG_CK(batch_instances.is_valid_idx(batch_idx));
		batch_entry& the_ans = batch_instances[batch_idx];
		return the_ans;
	}

	bool		in_valid_inst(){
		long batch_idx = batch_consec - 1;
		return (batch_instances.is_valid_idx(batch_idx));
	}

	ch_string	init_log_name(ch_string sufix);

	ch_string	get_file_name(bool& is_batch){
		ch_string f_nam = gg_file_name;
		is_batch = false;
		if(batch_name.size() > 0){
			is_batch = true;
			f_nam = batch_name;
		}
		return f_nam;
	}

	void	reset_err_msg(){
		error_stm.clear();
		error_stm.str() = "";
		error_stm.flush();
	}

	double	mem_percent_used();

	void	print_final_assig();
	void	count_instance(batch_entry& inst_info);

	bj_ostream&	print_mini_stats(bj_ostream& os);
	bj_ostream& 	print_stats(bj_ostream& os, double current_secs = 0.0);

	bj_ostream&	print_mem_used(bj_ostream& os);
	bj_ostream&	print_totals(bj_ostream& os, double curr_tm = 0.0);
	bj_ostream&	print_final_totals(bj_ostream& os);
	void		print_batch_consec();

	void	log_message(const ch_string& msg_log);
	void	log_batch_info();
	void	read_batch_file(row<batch_entry>& names);
	void	work_all_instances();
	void	do_all_instances();
	void	do_cnf_file();
	
};

//=================================================================
// print functions

inline
bj_ostream& 	
batch_entry::print_batch_entry(bj_ostream& os, bool from_pt){
	ch_string sep = RESULT_FIELD_SEP;
	os << be_ff_nam << sep;
	os << as_satisf_str(be_out.bjo_result) << sep;
	os << be_out.bjo_solve_time << sep;
	os << be_out.bjo_num_vars << sep;
	os << be_out.bjo_num_ccls << sep;
	os << be_out.bjo_num_lits << sep;
	os << be_out.bjo_num_laps << sep;
	return os;
}

DEFINE_PRINT_FUNCS(batch_entry)

//=================================================================
// global functions

void	chomp_string(ch_string& s1);
int		solver_main(int argc, char** argv);


#endif		// BATCH_SOLVER_H

