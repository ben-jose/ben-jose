

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

support.h

Declaration of classes that support and assist the system.

--------------------------------------------------------------*/

#ifndef SUPPORT_H
#define SUPPORT_H

//=================================================================
// configuration defs

//define DO_GETCHAR	
#define DO_GETCHAR			getchar();
#define DO_FINAL_GETCHAR
//define DO_FINAL_GETCHAR		DO_GETCHAR
//define SOLVING_TIMEOUT		10.5	// 0.0 if no timeout
#define SOLVING_TIMEOUT			0.0		// 0.0 if no timeout
#define RECEIVE_TIMEOUT			10.0	// for timed_receive
#define PRINT_PROGRESS			true	// or if (SOLVING_TIMEOUT > 1.0)
#define PRINT_PERIOD			4.0
#define PRINT_TOTALS_PERIOD 		10.0
//define MAX_CONFLICTS			0		// all

#define LOG_NM_ERROR	"error.log"
#define LOG_NM_RESULTS	"results.log"
#define LOG_NM_STATS	"stats.log"
#define LOG_NM_ASSIGS	"assigs.log"

//=================================================================

#include "tools.h"
#include "util_funcs.h"

#include "ben_jose.h"
#include "instance_info.h"
#include "dbg_prt.h"
#include "skeleton.h"

#define SUPPORT_CK(prm) DBG_CK(prm)


//=================================================================
// pre-configuration decl

#define init_nams(nams, fst_idx, lst_idx) \
	for(long nams##kk = fst_idx; nams##kk < lst_idx; nams##kk++){ \
		nams[nams##kk] = "invalid_name !!!"; \
	} \

// end_define

#define	OUT_COND(lv_arr, lev, cond)	\
	(	(	(lev < 0) || \
			((lev >= 0) && lv_arr[lev]) \
		) && \
		(cond) \
	) \

//--end_of_def

#define PRT_OUT(lev, comm) \
	DO_PRINTS( \
		if(OUT_COND(GLB().out_lev, lev, true)) \
		{ \
			bj_ostream& os = bj_out; \
			comm; \
			os.flush(); \
		} \
	) \
			
//--end_of_def


//=================================================================
// typedefs and defines

class brain;

typedef	int	location;

//define INVALID_NION_ID		0
#define INVALID_QUANTON_ID	0
#define INVALID_POLARITY	0
#define INVALID_LAYER		-1

#define OUT_NUM_LEVS 10

//=================================================================
// global_data

class quanton;
class global_data;

extern global_data*	GLB_DATA_PT;

global_data&	GLB();
void		glb_set_memout();
long		get_free_mem_kb();

//typedef	void 	(global_data::*dbg_info_fn_t)();

class runoptions {
public:
	bool	ro_just_read;
	bool	ro_local_verifying;
	bool	ro_only_save;
	bool	ro_keep_skeleton;

	ch_string	ro_skeleton_root_path;

	runoptions(){
		init_runoptions();
	}

	~runoptions(){
	}

	void	init_runoptions(){
		ro_just_read = false;
		ro_local_verifying = false;
		ro_only_save = false;
		ro_keep_skeleton = false;

		ro_skeleton_root_path = "";
	}
};

class global_data {
public:
	bool			using_mem_ctrl;

	ch_string		help_str;
	ch_string		version_str;

	bool			op_debug_clean_code;
	bool			op_just_read;

	runoptions		gg_options;

	row<long>		final_trail_ids;
	row<long>		final_chosen_ids;

	bj_ostream*		out_os;
	row<bool>		out_lev;

	mem_size 		dbg_mem_at_start;

	//ch_string		dbg_file_name;
	//std::ofstream	dbg_file;

	bool			dbg_skip_print_info;
	
	long			dbg_num_laps;

	bj_ostr_stream	error_stm;
	long			error_cod;

	ch_string		input_file_nm;
	//ch_string		output_file_nm;

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

	avg_stat		batch_stat_choices;
	avg_stat		batch_stat_laps;
	avg_stat		batch_stat_load_tm;
	avg_stat		batch_stat_solve_tm;
	avg_stat		batch_stat_mem_used;

	avg_stat		batch_stat_direct_hits;
	avg_stat		batch_stat_equ_hits;
	avg_stat		batch_stat_sub_hits;
	avg_stat		batch_stat_saved_targets;
	avg_stat		batch_stat_conflicts;

	double			batch_start_time;
	double			batch_end_time;
	timer			batch_prt_totals_timer;

	row<instance_info>	batch_instances;

	ch_string		gg_file_name;

	skeleton_glb		gg_skeleton;

	void 		init_global_data();
	void 		finish_global_data();

	global_data();
	~global_data();

	void		reset_global(){
		reset_err_msg();
		error_cod = -1;
	}

	bool		get_args(int argc, char** argv);
	void		set_input_name();

	instance_info&	get_curr_inst(){
		long batch_idx = batch_consec - 1;
		DBG_CK(batch_instances.is_valid_idx(batch_idx));
		instance_info& the_ans = batch_instances[batch_idx];
		return the_ans;
	}

	bool		in_valid_inst(){
		long batch_idx = batch_consec - 1;
		return (batch_instances.is_valid_idx(batch_idx));
	}

	consecutive_t	get_curr_lap(){
		if(batch_instances.is_empty()){
			return 0;
		}
		instance_info& inst_info = get_curr_inst();
		return inst_info.ist_out.iot_num_laps;
	}

	satisf_val&	result(){
		instance_info& inst_info = get_curr_inst();
		return inst_info.ist_out.iot_result;
	}

	void		init_paths();

	bool		is_finishing(){
		return (result() != k_unknown_satisf);
	}

	bool	is_here(location rk){
		MARK_USED(rk);
		return true;
	}

	ch_string	init_log_name(ch_string sufix);

	void 	dbg_update_config_entries();

	void	dbg_default_info(){
		bj_ostream& os = bj_dbg;
	
		os << "NO DBG INFO AVAILABLE " << 
		"(define a func for this error code)" << bj_eol; 
	}

	ch_string	get_curr_f_nam(){
		if(batch_instances.is_empty()){
			return "UKNOWN FILE NAME";
		}
		instance_info& inst_info = get_curr_inst();
		return inst_info.get_f_nam();
	}

	ch_string	get_file_name(bool& is_batch){
		ch_string f_nam = gg_file_name;
		is_batch = false;
		if(batch_name.size() > 0){
			is_batch = true;
			f_nam = batch_name;
		}
		return f_nam;
	}

	bj_ostream&	get_os(){
		if(out_os != NULL_PT){
			return *out_os;
		}
		return bj_out;
	}


	void	set_active_out_levs();

	void	reset_err_msg(){
		error_stm.clear();
		error_stm.str() = "";
		error_stm.flush();
	}

	double	mem_percent_used();

	void	print_final_assig();
	void	count_instance(instance_info& inst_info);

	//int	walk_neuron_tree(ch_string& dir_nm);

	bj_ostream&	print_mini_stats(bj_ostream& os);
	bj_ostream& 	print_stats(bj_ostream& os, double current_secs = 0.0);

	bj_ostream&	print_mem_used(bj_ostream& os);
	bj_ostream&	print_totals(bj_ostream& os, double curr_tm = 0.0);
	bj_ostream&	print_final_totals(bj_ostream& os);
	void		print_batch_consec();

};

//=================================================================
// global functions

typedef void (*core_func_t)(void);

ch_string	get_log_name(ch_string f_nam, ch_string sufix);

void	init_dbg_conf();
void	log_message(const ch_string& msg_log);
void	log_batch_info();
void	call_and_handle_exceptions(core_func_t the_func);
void	chomp_string(ch_string& s1);
void	read_batch_file(row<ch_string*>& names);
void	read_batch_instances(ch_string file_nm, row<instance_info>& f_insts);
bool	all_results_batch_instances(ch_string file_nm, satisf_val r_val);
void	get_enter(bj_ostream& os, ch_string msg);
void	do_all_instances();
int		tests_main_(int argc, char** argv);
int		solver_main(int argc, char** argv);

//=================================================================
// implemented in brain.cpp

void	print_op_cnf();
void	do_cnf_file();

#endif		// SUPPORT_H
