

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

support.cpp  

Global classes and functions that support and assist the system.

--------------------------------------------------------------*/

#include <limits.h>
#include <cstring>

#include "top_exception.h"
#include "stack_trace.h"
#include "support.h"
#include "config.h"
#include "sortor.h"

//DEFINE_MEM_STATS;

bool	dbg_bad_cycle1 = false;		// dbg_print_cond_func

ch_string	satisf_val_nams[k_last_satisf_val];

void	init_glb_nams(){
	init_nams(satisf_val_nams, k_first_satisf_val, k_last_satisf_val);
	satisf_val_nams[k_unknown_satisf] = "unknown";
	satisf_val_nams[k_yes_satisf] = "yes_satisf";
	satisf_val_nams[k_no_satisf] = "no_satisf";
	satisf_val_nams[k_timeout] = "timeout";
	satisf_val_nams[k_memout] = "memout";
	satisf_val_nams[k_error] = "error";
}

void	glb_set_memout(){
	if(GLB().has_brain()){
		GLB().result() = k_memout;
	}
}

long
find_first_digit(ch_string& the_str, bool dig = true){
	unsigned long aa = 0;
	if(dig){
		for(	aa = 0; 
			((aa < the_str.size()) && ! isdigit(the_str[aa])); 
			aa++)
		{}
	} else {
		for(	aa = 0; 
			((aa < the_str.size()) && isdigit(the_str[aa])); 
			aa++)
		{}
	}
	return aa;
}

long
get_free_mem_kb(){
	//std::ostream& os = std::cout;

	long free_mem_kb = 0;
	
	ch_string str_mem_free ("MemFree:");
	long str_mem_free_ln = str_mem_free.size();
	
	ch_string str_ln;

	std::ifstream proc_meminfo;
	proc_meminfo.open("/proc/meminfo");
	if(proc_meminfo.good() && proc_meminfo.is_open()){
		while(! proc_meminfo.eof()){
			std::getline(proc_meminfo, str_ln);
			
			//os << "Lei:<<" << str_ln << ">>" << std::endl;
			
			if(str_ln.compare(0, str_mem_free_ln, str_mem_free) == 0){
				long pos = find_first_digit(str_ln);
				ch_string str_val = str_ln.substr(pos);
				pos = find_first_digit(str_val, false);
				str_val = str_val.substr(0, pos);
				
				free_mem_kb = atol(str_val.c_str());
				
				//os << "FOUND IT = >" << str_val << "<" << std::endl;
				break;
			}
		}
		proc_meminfo.close();
	}
	return free_mem_kb;
}

//======================================================================
// file_exception

class file_exception : public top_exception {
public:
	file_exception(char* descr = as_pt_char("undefined file exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

//============================================================
// global_data

global_data 	the_glb_data;
global_data*	GLB_DATA_PT = &the_glb_data;

global_data&
GLB(){
	if(GLB_DATA_PT == NULL_PT){
		std::cout << STACK_STR << std::endl;
		std::cout << "GLB_DATA_PT is NULL !!!!" << std::endl;
		abort_func(0);
	}
	return *GLB_DATA_PT;
}

void
global_data::dbg_update_config_entries(){
	std::ostream& os = std::cout;
	MARK_USED(os);

	consecutive_t curr_round = get_curr_lap();

	long& start_idx = dbg_current_start_entry;
	long& stop_idx = dbg_current_stop_entry;

	row<debug_entry>& start_lst = dbg_start_dbg_entries;
	row<debug_entry>& stop_lst = dbg_stop_dbg_entries;

	while(	(start_idx < start_lst.size()) && 
		(start_lst[start_idx].dbg_round <= curr_round))
	{
		long start_dbg_id = start_lst[start_idx].dbg_id;
		SUPPORT_CK(GLB().dbg_lev.is_valid_idx(start_dbg_id));
		GLB().dbg_lev[start_dbg_id] = true;
		start_idx++;
	} 

	while(	(stop_idx < stop_lst.size()) && 
		(stop_lst[stop_idx].dbg_round < curr_round))
	{
		long stop_dbg_id = stop_lst[stop_idx].dbg_id;
		SUPPORT_CK(GLB().dbg_lev.is_valid_idx(stop_dbg_id));
		GLB().dbg_lev[stop_dbg_id] = false;
		stop_idx++;
	} 
}

void
global_data::init_global_data(){
	//std::ostream& os = std::cout;
	//os << "starting 'init_global data'  MEM_STATS.num_bytes_in_use = " << MEM_STATS.num_bytes_in_use << std::endl;

	using_mem_ctrl = false;

	help_str =
		"[<file_name[.lst]> -h -v -paths -verify -just_read -keep_skeleton -only_save "
		"-root <root_path> -max_mem <max_kb_to_use>] \n"
		"\n"
		"-h : print help.\n"
		"-v : print version.\n"
		"-paths : print the paths to be used.\n"
		"-verify : localy verify the  <file_name>.\n"
		"-root : set the root path to <root_path>.\n"
		"-max_mem : set max number of RAM Kbytes to use while running.\n"
		"\n"
		;

	version_str =
		"v0.1\n"
		"(c) 2009. QUIROGA BELTRAN, Jose Luis. c.c. 79523732. Bogota - Colombia.\n"
		;

	op_debug_clean_code = false;
	op_just_read = false;

	MEM_CTRL(using_mem_ctrl = true);

	MEM_STATS.set_memout_func = glb_set_memout;		

	pt_brain = NULL_PT;

	out_os = &(std::cout);
	long ii = 0;
	long num_out_levs = OUT_NUM_LEVS;
	out_lev.set_cap(num_out_levs);
	for(ii = 0; ii < num_out_levs; ii++){
		out_lev.inc_sz() = false;
	}

	dbg_file_name = "";
	dbg_os = &(std::cout);
	dbg_os_bak = NULL_PT;

	long num_dbg_levs = DBG_NUM_LEVS;
	dbg_lev.set_cap(num_dbg_levs);
	for(ii = 0; ii < num_dbg_levs; ii++){
		dbg_lev.inc_sz() = false;
	}

	dbg_skip_print_info = false;

	dbg_num_laps = 0;

	dbg_ic_active = false;
	dbg_ic_max_seq = -1;
	dbg_ic_seq = 0;
	dbg_ic_after = false;
	dbg_ic_gen_jpg = false;

	reset_err_msg();

	error_cod = -1;

	input_file_nm = "";
	//output_file_nm = "";

	batch_log_on = true;
	batch_name = "";
	batch_log_name = "";
	batch_end_log_name = "";
	batch_end_msg_name = "";
	batch_answer_name = "";
	batch_num_files = 0;
	batch_consec = 0;

	batch_num_unknown_satisf = 0;
	batch_num_yes_satisf = 0;
	batch_num_no_satisf = 0;
	batch_num_timeout = 0;
	batch_num_memout = 0;
	batch_num_error = 0;

	batch_stat_choices.vs_nam = "CHOICES";
	batch_stat_laps.vs_nam = "LAPS";
	batch_stat_load_tm.vs_nam = "LOAD SEGs";
	batch_stat_solve_tm.vs_nam = "SOLVE SEGs";
	batch_stat_mem_used.vs_nam = "BYTES USED";

	batch_stat_direct_hits.vs_nam = "DIRECT HITS";
	batch_stat_equ_hits.vs_nam = "EQU HITS";
	batch_stat_sub_hits.vs_nam = "SUB HITS";
	batch_stat_saved_targets.vs_nam = "SAVED";
	batch_stat_conflicts.vs_nam = "CNFLS";

	batch_start_time = 0.0;
	/*
	batch_max_mem_used = 0;
	batch_max_mem_used_perc = 0.0;
	*/
	batch_end_time = 0.0;

	gg_file_name = "";

	//gg_skeleton.init_paths();

	init_glb_nams();

	set_active_out_levs();

	//os << "ending 'init_global data'  MEM_STATS.num_bytes_in_use = " << MEM_STATS.num_bytes_in_use << std::endl;
}

void
global_data::finish_global_data(){
}

void	
global_data::set_active_out_levs(){

	//search PRT_OUT(val
	// max lev == 9

	out_lev[0] = true;	// batch data
	//out_lev[1] = true;	// file data

}

/*
double
global_data::mem_percent_used(){
	double perc_mem_used = 0.0;
	if(GLB().using_mem_ctrl && (MEM_STATS.num_bytes_available > 0)){
		double tot = (double)(MEM_STATS.num_bytes_available);
		double in_use = (double)(MEM_STATS.num_bytes_in_use);
		perc_mem_used = ((in_use / tot) * 100);
	}
	return perc_mem_used; 
}
*/

std::ostream&
global_data::print_mem_used(std::ostream& os){
	if(GLB().using_mem_ctrl){
		os << GLB().batch_stat_mem_used;
		/*
		long used_mem = (long)mem_percent_used();
		os << "MEM=" << MEM_STATS.num_bytes_in_use << " bytes of " 
			<< MEM_STATS.num_bytes_available << " (" << used_mem << "%)";
		*/
	}
	return os;
}

std::ostream&
global_data::print_totals(std::ostream& os, double curr_tm){

	os << std::endl;
	if(in_valid_inst()){
		instance_info& inst_info = get_curr_inst();
		os << "CURR_LAPS=" << inst_info.ist_num_laps << " ";
	}
	os << "YES_SAT=" << batch_num_yes_satisf << " ";
	os << "NO_SAT=" << batch_num_no_satisf << " ";
	os << "TMOUT=" << batch_num_timeout << " ";

	os << "MMOUT=" << batch_num_memout << " ";
	os << "ERR=" << batch_num_error << " ";
	os << "UNKNOWN_SAT=" << batch_num_unknown_satisf << " ";

	if(curr_tm != 0.0){
		double full_tm = curr_tm - batch_start_time;
		os << std::fixed;
		os.precision(2);
		os << "SECS=" << full_tm << " ";
	}

	os << std::endl;
	os << batch_stat_solve_tm;
	if(GLB().using_mem_ctrl){
		os << batch_stat_mem_used;
	}

	DBG_COMMAND(33, 
		os << batch_stat_laps;
	);

	os << std::endl;
	return os;
}

std::ostream&
global_data::print_final_totals(std::ostream& os){
	os << std::endl;
	//os << std::fixed;
	//os.precision(2);

	os << GLB().batch_stat_choices;
	os << GLB().batch_stat_laps;
	os << GLB().batch_stat_load_tm;
	os << GLB().batch_stat_solve_tm;
	os << GLB().batch_stat_mem_used;

	os << GLB().batch_stat_direct_hits;
	os << GLB().batch_stat_equ_hits;
	os << GLB().batch_stat_sub_hits;
	os << GLB().batch_stat_saved_targets;
	os << GLB().batch_stat_conflicts;

	/*
	os << "AVERAGE SOLVE TIME = " <<
		GLB().batch_avg_solve_time.avg << std::endl;
	os << "TOTAL SOLVE TIME = " << 
		GLB().batch_total_solve_time << std::endl;
	*/

	double tot_tm = GLB().batch_end_time - GLB().batch_start_time;
	os << "TOTAL TIME = " << tot_tm << std::endl;
	return os;
}

void			
global_data::print_batch_consec(){
	DBG_COMMAND(34, return);

	double perc = 0.0;
	if(batch_num_files > 0){
		perc = (batch_consec * 100.0) / batch_num_files;
	}

	PRT_OUT(0, 
	os << CARRIAGE_RETURN;
	//os.width(15);
	//os.fill(' ');
	os << std::fixed;
	os.precision(2);
	os << batch_consec << " of " << batch_num_files << " (" << perc << " %)";
	os.flush();
	);
}

void
global_data::print_final_assig(){
	if(GLB().batch_answer_name.size() == 0){
		return;
	}

	instance_info& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.get_f_nam();

	const char* f_nm = GLB().batch_answer_name.c_str();
	std::ofstream log_stm;
	log_stm.open(f_nm, std::ios::app);
	if(log_stm.good() && log_stm.is_open()){
		log_stm << "c ====================" << std::endl;
		log_stm << "c " << f_nam << std::endl;
		final_chosen_ids.print_row_data(log_stm, false, " ", -1, -1, false, 20);
		log_stm << " 0" << std::endl;
		final_assig.print_row_data(log_stm, false, " ", -1, -1, false, 20);
		log_stm << " 0" << std::endl;
	}
	log_stm.close();
}

void
global_data::count_instance(instance_info& inst_info){
	double end_time = run_time();
	double full_tm = end_time - inst_info.ist_solve_time;

	batch_stat_laps.add_val(inst_info.ist_num_laps);
	batch_stat_solve_tm.add_val(full_tm);

	/*
	batch_avg_solve_time.add_val(full_tm);
	batch_total_solve_time += full_tm;

	if(full_tm > batch_max_solve_time){
		batch_max_solve_time = full_tm;
	}
	*/

	if(using_mem_ctrl){
		long tot_byt = MEM_STATS.num_bytes_available;
		MARK_USED(tot_byt);
		long by_in_use = MEM_STATS.num_bytes_in_use;

		batch_stat_mem_used.add_val(by_in_use);

		/*
		if(by_in_use > batch_max_mem_used){
			batch_max_mem_used = by_in_use;
		}
		batch_avg_mem_used.add_val(by_in_use);

		double perc_mem_used = mem_percent_used();
		if(perc_mem_used > batch_max_mem_used_perc){
			batch_max_mem_used_perc = perc_mem_used;
		}
		batch_avg_mem_used_perc.add_val(perc_mem_used);
		*/
	}

	DBG_PRT(4, os << "FINAL_CHOSEN=" << final_chosen_ids << std::endl
		<< "FINAL_ASSIG=" << final_assig << std::endl 
		<< std::endl);

	if(out_os != NULL_PT){
		PRT_OUT(1, print_stats(*out_os));
	}

	PRT_OUT(1, os << "FINISHING" << std::endl);

	inst_info.ist_solve_time = full_tm;

	satisf_val inst_res = inst_info.ist_result;

	if(inst_res == k_unknown_satisf){
		batch_num_unknown_satisf++;
	} else if(inst_res == k_yes_satisf){
		//BRAIN_CK(! final_assig.is_empty());
		PRT_OUT(1, print_final_assig());
		final_assig.clear(true, true);	// So global memcheck says its OK.
		batch_num_yes_satisf++;
	} else if(inst_res == k_no_satisf){
		batch_num_no_satisf++;
	} else if(inst_res == k_timeout){
		batch_num_timeout++;
	} else if(inst_res == k_memout){
		batch_num_memout++;
	} else {
		BRAIN_CK(inst_res == k_error);
		batch_num_error++;
	}

	PRT_OUT(1, os << inst_info << std::endl);

	print_batch_consec();

	//PRT_OUT(0, os << " laps=" << inst_info.ist_num_laps; os.flush(););
}

//============================================================
// stats

std::ostream&
global_data::print_mini_stats(std::ostream& os){
	instance_info& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.get_f_nam();

	os << CARRIAGE_RETURN;
	os << "'" << f_nam << "'";
	dbg_print_cond_func(true, false, "NO_NAME", 0, "true", INVALID_DBG_LV);
	return os; 
}

std::ostream&
global_data::print_stats(std::ostream& os, double current_secs){
	DBG_COMMAND(35, return os);

	DBG_COMMAND(36,
		print_mini_stats(os);
		return os;
	);

	if(current_secs == 0.0){
		current_secs = run_time();
	}

	instance_info& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.get_f_nam();

	os << std::endl;
	os << "file_name: '" << f_nam << "'" << std::endl;
	
	DBG( dbg_print_cond_func(true, false, "NO_NAME", 0, "true", INVALID_DBG_LV) );

	print_totals(os, current_secs);
	os << std::endl << std::endl;

	os.flush();

	return os;
}

//============================================================
// global functions

bool	dbg_print_cond_func(bool prm, bool is_ck, const ch_string fnam, int lnum,
		const ch_string prm_str, long dbg_lv)
{
	DBG_CK(! dbg_bad_cycle1);
	dbg_bad_cycle1 = true;

	bool resp = true;
	if(prm){
		std::ostream& os = *(GLB().dbg_os);
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
			os << std::endl;
		}
		os.flush();
		resp = (! is_ck);
	}

	dbg_bad_cycle1 = false;
	return resp;
}

void	err_header(std::ostringstream& msg_err){
	msg_err.clear();
	//msg_err = "";
	msg_err.flush();
	msg_err << "file(" << GLB().batch_consec << "/"
		<< GLB().batch_num_files << ")='"
		<< GLB().get_curr_f_nam() << "'";
	//msg_err << std::endl;
}

void	log_message(const std::ostringstream& msg_log){

	if(! GLB().batch_log_on){
		return;
	}

	if(GLB().batch_log_name.size() == 0){
		return;
	}

	const char* log_nm = GLB().batch_log_name.c_str();
	std::ofstream log_stm;
	log_stm.open(log_nm, std::ios::app);
	if(log_stm.good() && log_stm.is_open()){
		log_stm << msg_log.str() << std::endl; 
		log_stm.close();
	}
}

void	log_batch_info(){
	if(! GLB().batch_log_on){
		return;
	}

	if(GLB().batch_end_log_name.size() == 0){
		return;
	}

	std::ostringstream msg_log;

	const char* log_nm = GLB().batch_end_log_name.c_str();
	std::ofstream log_stm;
	log_stm.open(log_nm, std::ios::app);
	if(! log_stm.good() || ! log_stm.is_open()){
		GLB().reset_err_msg();
		GLB().error_stm << "Could not open file " << log_nm << ".";
		msg_log << GLB().error_stm.str();
		std::cerr << msg_log.str() << std::endl;
		log_message(msg_log);
		return;
	}

	GLB().batch_instances.print_row_data(log_stm, false, "\n");
	log_stm << std::endl; 
	log_stm.close();

	if(GLB().batch_end_msg_name.size() == 0){
		return;
	}

	const char* msg_nm = GLB().batch_end_msg_name.c_str();
	std::ofstream msg_stm;
	msg_stm.open(msg_nm, std::ios::app);
	if(! msg_stm.good() || ! msg_stm.is_open()){
		GLB().reset_err_msg();
		GLB().error_stm << "Could not open file " << msg_nm << ".";
		msg_log << GLB().error_stm.str();
		std::cerr << msg_log.str() << std::endl;
		log_message(msg_log);
		return;
	}

	GLB().print_totals(msg_stm);
	GLB().print_final_totals(msg_stm);

}

void
read_batch_instances(ch_string file_nm, row<instance_info>& f_insts){
	std::ostream& os = std::cout;

	std::ifstream in_stm;

	in_stm.open(file_nm.c_str(), std::ios::binary);
	if(! in_stm.good() || ! in_stm.is_open()){
		os << "NO " << file_nm << " FILE FOUND." << std::endl;
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
		
			//os << "Lei:<<" << str_ln << ">>" << std::endl;
			n_inst.parse_instance(str_ln, num_ln);
		}
	}
	in_stm.close();
}

ch_string 
instance_info::parse_field(const char*& pt_in){
	char sep = RESULT_FIELD_SEP_CHAR;
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
			SUPPORT_CK((*pt_in) == sep);

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

satisf_val
as_satisf(ch_string str_ln){
	satisf_val the_val = k_unknown_satisf;
	if(str_ln == satisf_val_nams[k_unknown_satisf]){
		the_val = k_unknown_satisf;
	} else if(str_ln == satisf_val_nams[k_yes_satisf]){
		the_val = k_yes_satisf;
	} else if(str_ln == satisf_val_nams[k_no_satisf]){
		the_val = k_no_satisf;
	} else if(str_ln == satisf_val_nams[k_timeout]){
		the_val = k_timeout;
	} else if(str_ln == satisf_val_nams[k_memout]){
		the_val = k_memout;
	} else if(str_ln == satisf_val_nams[k_error]){
		the_val = k_error;
	}
	return the_val;
}

void
instance_info::parse_instance(ch_string str_ln, long line){
	init_instance_info();

	DBG_PRT(30, os << "LINE inst=" << str_ln);

	const char* pt_in = str_ln.c_str();
	ist_file_path = parse_field(pt_in);
	ch_string r_fi = parse_field(pt_in);
	ist_result = as_satisf(r_fi);

	DBG_PRT(30, os << "read inst=" << *this);

	/*
	parse_field(pt_in); // double

	ist_num_vars = parse_int(pt_in, line);
	parse_field(pt_in); // sep

	ist_num_ccls = parse_int(pt_in, line);
	parse_field(pt_in); // sep

	ist_num_lits = parse_int(pt_in, line);
	parse_field(pt_in); // sep
	*/
}

bool
all_results_batch_instances(ch_string file_nm, satisf_val r_val){
	row<instance_info> f_insts;
	read_batch_instances(file_nm, f_insts);
	bool all_ok = true;

	for(long aa = 0; aa < f_insts.size(); aa++){
		instance_info& inst = f_insts[aa];
		if(inst.ist_result != r_val){
			all_ok = false;
		}
	}
	return all_ok;
}

void	call_and_handle_exceptions(core_func_t the_func){

	std::ostringstream msg_err;
	try{
		(*the_func)();
		DBG_PRT(6, os << "AFTER THE_FUNC in call_and_handle_exceptions");
	} 
	catch (long code) {
		err_header(msg_err);
		msg_err << GLB().error_stm.str();
		std::cerr << msg_err.str() << std::endl;
		log_message(msg_err);
		abort_func(0);
	}
	catch (...) {
		std::cout << "INTERNAL ERROR !!! (call_and_handle_exceptions)" << std::endl;
		std::cout << STACK_STR << std::endl;
		std::cout.flush();
		abort_func(0);		
		
		//err_header(msg_err);
		//msg_err << "Unknown type exception in  ";
		//std::cerr << msg_err.str() << std::endl;
		//log_message(msg_err);
	}

	DBG_PRT(6, os << "AFTER try_catch in call_and_handle_exceptions");

	GLB().reset_global();
}

void	chomp_string(ch_string& s1){
	long ii = s1.length() - 1;
	//while((ii > 0) && ! isalnum(s1[ii])){
	while((ii > 0) && ! isprint(s1[ii])){
		ii--;
	}
	s1.resize(ii + 1);
}

void	read_batch_file(row<instance_info>& names){
	PRT_OUT(0, os << "Loading batch file '" 
		<< GLB().batch_name << std::endl); 

	std::ifstream in_stm;
	in_stm.open(GLB().batch_name.c_str());

	if(! in_stm.good() || ! in_stm.is_open()){
		GLB().reset_err_msg();
		GLB().error_stm << "Could not open file " << GLB().batch_name << ".";

		char* file_cannot_open = as_pt_char("Cannot open file exception");
		DBG_THROW_CK(file_cannot_open != file_cannot_open);
		throw file_exception(file_cannot_open);
		abort_func(1, file_cannot_open);
	}

	names.set_cap(1000);
	names.clear();
	bool adding = true;

	ch_string my_str;

	while(in_stm.good()){
		std::getline(in_stm, my_str);
		chomp_string(my_str);

		if(my_str == "/*"){
			adding = false;
		}
		if(adding && ! my_str.empty() && (my_str[0] != '#')){
			instance_info& ist = names.inc_sz();
			ist.ist_file_path = my_str;
			if(names.size() % 1000 == 0){
				PRT_OUT(0, os << CARRIAGE_RETURN << names.size());
				//PRT_OUT(0, os << ".");
			}
		}
		if((! adding) && (my_str == "*/")){
			adding = true;
		}
	} 

	PRT_OUT(0, os << CARRIAGE_RETURN << names.size());

	if(in_stm.bad()){
		std::cerr << in_stm.rdstate() << std::endl;
	}

	in_stm.close();
	PRT_OUT(0, 
		os << std::endl
		<< "Read " << names.size() << " file names." << std::endl
		<< "Processing:" << std::endl
	);
}

void	print_periodic_totals(void* pm, double curr_tm){
	MARK_USED(pm);
	PRT_OUT(0, GLB().print_totals(os, curr_tm));
}

void	get_enter(std::ostream& os, ch_string msg){
	os << "PRESS ENTER to continue. " << msg << std::endl;
	getchar();
}

void	init_dbg_conf(){
	config_reader conf_rdr;
	conf_rdr.read_config("yosoy.conf");

	GLB().dbg_current_start_entry = 0;
	GLB().dbg_current_stop_entry = 0;
	GLB().dbg_update_config_entries();

	//DBG_PRT(-1, os << "start_dbgs=" << GLB().dbg_start_dbg_entries << std::endl); 
	//DBG_PRT(-1, os << "stop_dbgs=" << GLB().dbg_stop_dbg_entries << std::endl); 
	//DBG_PRT(-1, os << "dbg_lev=" << GLB().dbg_lev << std::endl); 

	DBG_COMMAND(37, os << "PRINT_FULL_INFO" << std::endl; 
		GLB().dbg_skip_print_info = true);

	DBG_COMMAND(38, os << "SET IC ACTIVE" << std::endl; 
		GLB().dbg_ic_active = true);

	DBG_COMMAND(39, os << "SET IC AFTER" << std::endl; 
		GLB().dbg_ic_after = true);

	DBG_COMMAND(40, os << "SET IC GEN JPG" << std::endl; 
		GLB().dbg_ic_gen_jpg = true);

	/*
	dbg_ic_max_seq = -1;
	dbg_ic_seq = 0;
	*/
}

ch_string
get_log_name(ch_string f_nam, ch_string sufix){
	ch_string lg_nm = f_nam + "_" + sufix;
	return lg_nm;
}

ch_string
global_data::init_log_name(ch_string sufix){
	bool is_batch = false;
	ch_string f_nam = GLB().get_file_name(is_batch);
	ch_string log_nm = get_log_name(f_nam, sufix);

	/*
	std::ostringstream log_ss;
	log_ss << f_nam << "_";
	log_ss << sufix;
	log_nm = log_ss.str();
	*/
	remove(log_nm.c_str());
	return log_nm;
}

void	do_all_instances(){
	GLB().batch_start_time = run_time();
	GLB().batch_prt_totals_timer.init_timer(PRINT_TOTALS_PERIOD);

	bool is_batch = false;
	ch_string f_nam = GLB().get_file_name(is_batch);

	GLB().batch_log_name = GLB().init_log_name(LOG_NM_ERROR);
	GLB().batch_end_log_name = GLB().init_log_name(LOG_NM_RESULTS);
	GLB().batch_end_msg_name = GLB().init_log_name(LOG_NM_STATS);
	GLB().batch_answer_name = GLB().init_log_name(LOG_NM_ASSIGS);

	row<instance_info>& all_insts = GLB().batch_instances;
	SUPPORT_CK(all_insts.is_empty());
	if(is_batch){
		read_batch_file(all_insts);
	} else {
		instance_info& ist = all_insts.inc_sz();
		ist.ist_file_path = f_nam;
	} 

	GLB().batch_num_files = all_insts.size();

	print_op_cnf();

	for(long ii = 0; ii < GLB().batch_num_files; ii++){
		GLB().batch_consec = ii + 1;
		ch_string inst_nam = all_insts[ii].ist_file_path;
		//GLB().file_name = all_insts[ii].ist_file_path;

		std::ifstream i_ff;
		if(inst_nam.size() > 0){
			GLB().batch_prt_totals_timer.
				check_period(print_periodic_totals);

			MEM_CTRL(mem_size mem_in_u = MEM_STATS.num_bytes_in_use;)
			MEM_CTRL(MARK_USED(mem_in_u));
			MEM_CTRL(dbg_keeping_ptdir = true);

			do_cnf_file();

			DBG_PRT(6, os << "AFTER do_cnf_file");

			DBG_PRT_COND(DBG_ALL_LVS, ! (mem_in_u == MEM_STATS.num_bytes_in_use) ,
				os << "ABORTING_DATA " << std::endl;
				os << "mem_in_u=" << mem_in_u << std::endl;
				os << "MEM_STATS.num_bytes_in_use=" << MEM_STATS.num_bytes_in_use << std::endl;
				dbg_print_ptdir();
				os << "END_OF_aborting_data" << std::endl;
			);
			SUPPORT_CK(mem_in_u == MEM_STATS.num_bytes_in_use);
			MEM_CTRL(dbg_keeping_ptdir = false);
		}
	}

	print_op_cnf();

	GLB().batch_end_time = run_time();

	PRT_OUT(0, log_batch_info());
	all_insts.clear(true, true);

}

void
global_data::set_input_name(){
	if(input_file_nm.size() == 0){
		return;
	}

	bool is_batch = false;
	int f_ln = input_file_nm.size();
	if(f_ln > 4){
		const char* f_nm = input_file_nm.c_str();
		const char* f_ext = (f_nm + f_ln - 4);
		if(strcmp(f_ext, ".lst") == 0){
			is_batch = true;
		}
	}

	if(is_batch){
		SUPPORT_CK(batch_name.size() == 0);
		batch_name = input_file_nm;
	} else {
		SUPPORT_CK(gg_file_name.size() == 0);
		gg_file_name = input_file_nm;
	}

}

bool
global_data::get_args(int argc, char** argv)
{
	std::ostream& os = std::cout;
	MARK_USED(os);

	bool prt_help = false;
	bool prt_version = false;
	bool prt_headers = false;
	bool prt_paths = false;

	gg_skeleton.init_paths();

	for(long ii = 1; ii < argc; ii++){
		ch_string the_arg = argv[ii];
		if(the_arg == "-h"){
			prt_help = true;
		} else if(the_arg == "-v"){
			prt_version = true;
		} else if(the_arg == "-t"){
			prt_headers = true;
		} else if(the_arg == "-paths"){
			prt_paths = true;
		} else if(the_arg == "-verify"){
			gg_skeleton.kg_local_verifying = true;
		} else if(the_arg == "-only_save"){
			gg_skeleton.kg_only_save = true;
		} else if(the_arg == "-just_read"){
			op_just_read = true;
		} else if(the_arg == "-debug"){
			op_debug_clean_code = true;
		} else if(the_arg == "-keep_skeleton"){
			gg_skeleton.kg_keep_skeleton = true;
		} else if((the_arg == "-laps") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			dbg_num_laps = atol(argv[kk_idx]);

		} else if((the_arg == "-root") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			gg_skeleton.kg_root_path = argv[kk_idx];
			gg_skeleton.init_paths();

		} else if((the_arg == "-max_mem") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			long max_mem = atol(argv[kk_idx]);
			max_mem = (max_mem * NUM_BYTES_IN_KBYTE);
			if(max_mem > 0){
				MEM_STATS.num_bytes_available = max_mem;
			}

		} else if(input_file_nm.size() == 0){
			input_file_nm = argv[ii];
		}
	}

	if(prt_help){
		os << argv[0] << " " << help_str << std::endl;
		return false;
	}
	if(prt_version){
		os << argv[0] << " " << version_str << std::endl;
		return false;
	}
	if(prt_headers){
		instance_info::print_headers(os);
		os << std::endl;
		return false;
	}
	if(prt_paths){
		gg_skeleton.print_paths(os);
		return false;
	}
	if(input_file_nm.size() == 0){
		os << argv[0] << " " << help_str << std::endl;
		os << argv[0] << " " << version_str << std::endl;
		return false;
	}


	set_input_name();

	return true;
}

int	solver_main(int argc, char** argv){
	DBG_CHECK_SAVED(
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
	);
	DBG(std::cout << "FULL_DEBUG is defined" << std::endl);
	BRAIN_CK((std::cout << "doing CKs (plain CKs)" << std::endl) && true);
	BRAIN_CK_0((std::cout << "doing CK_0s" << std::endl) && true);
	BRAIN_CK_1((std::cout << "doing CK_1s" << std::endl) && true);
	BRAIN_CK_2((std::cout << "doing CK_2s" << std::endl) && true);

	SUPPORT_CK(sizeof(t_1byte) == 1);
	SUPPORT_CK(sizeof(long) == sizeof(void*));

	int resp = 0;

	SUPPORT_CK(GLB().dbg_start_dbg_entries.get_cap() == 0);
	SUPPORT_CK(GLB().dbg_stop_dbg_entries.get_cap() == 0);

	MEM_CTRL(mem_size mem_in_u = MEM_STATS.num_bytes_in_use;)
	MEM_CTRL(MARK_USED(mem_in_u));

	MEM_STATS.num_bytes_available = (get_free_mem_kb() * NUM_BYTES_IN_KBYTE);

	DBG(init_dbg_conf());

	bool args_ok = GLB().get_args(argc, argv);

	//double start_tm = 0.0;
	//start_tm = run_time();

	if(args_ok){
		PRT_OUT(1, os << ".STARTING AT " << run_time() << std::endl);

		double tot_mem = (double)(MEM_STATS.num_bytes_available);
		GLB().batch_stat_mem_used.vs_max_val = tot_mem;

		PRT_OUT(0, os << "Starting with "
			<< MEM_STATS.num_bytes_available << " bytes available" 
			<< std::endl); 
		DBG(PRT_OUT(1, os << "DEBUG_BRAIN activated" 
			<< std::endl));
		call_and_handle_exceptions(do_all_instances);

		PRT_OUT(1, os << ".ENDING AT " << run_time() << std::endl);
	}

	GLB().dbg_start_dbg_entries.clear(true, true);
	GLB().dbg_stop_dbg_entries.clear(true, true);

	SUPPORT_CK(mem_in_u == MEM_STATS.num_bytes_in_use);

	double end_tm = 0.0;
	MARK_USED(end_tm);
	if(args_ok){
		//end_tm = run_time();
		PRT_OUT(0, 
			GLB().print_totals(os);
			GLB().print_final_totals(os);
		);

		DO_FINAL_GETCHAR;
	}

	DBG(std::cout << "FULL_DEBUG is defined" << std::endl);
	BRAIN_CK((std::cout << "doing CKs (plain CKs)" << std::endl) && true);
	BRAIN_CK_0((std::cout << "doing CK_0s" << std::endl) && true);
	BRAIN_CK_1((std::cout << "doing CK_1s" << std::endl) && true);
	BRAIN_CK_2((std::cout << "doing CK_2s" << std::endl) && true);
	DBG_CHECK_SAVED(
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
		std::cout << "CAREFUL RUNNING SATEX !!!!!" << std::endl;
	);

	return resp;
}


