

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

batch_solver.cpp  

Global classes and functions that batch_solver and assist the system.

--------------------------------------------------------------*/

#include <fstream>

#include "top_exception.h"
#include "file_funcs.h"
#include "stack_trace.h"

#include "batch_solver.h"

/*

==============================================================
method pointers

DECL example
char (the_class::*the_method_pt)(int param1);

ASSIG example
the_method_pt = &the_class::a_method_of_the_class;

CALL Example
char cc = (an_object_of_the_class.*the_method_pt)(3);


Note that "::*", ".*" and "->*" are actually 
separate C++ operators.  

	reason, reasoning, reasoned, reasoner

		inductive, induce, induced, inducing, inducible

		deductive, deduce, deduced, deducing, deducible

		learn, learned, learning, learnable, learner

		specify, specified, specifying, specifiable, 
		specifier, specifies, specification

	study, studying, studied, studiable, studies

		analytic, analyze, analysed, analysing, analyzable
		analyser, analysis

		synthetic, synthesize, synthesized, synthesizing, 
		synthesizer, synthesis

		memorize, memorized, memorizing, memorizable, memorizer

		simplify, simplified, simplifying, simplificative, 
		simplifier, simplifies, simplification


	specify induce deduce simplify learn analyze synthesize

*/


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
	//bj_ostream& os = bj_out;

	long free_mem_kb = 0;
	
	ch_string str_mem_free ("MemFree:");
	long str_mem_free_ln = str_mem_free.size();
	
	ch_string str_ln;

	std::ifstream proc_meminfo;
	proc_meminfo.open("/proc/meminfo");
	if(proc_meminfo.good() && proc_meminfo.is_open()){
		while(! proc_meminfo.eof()){
			std::getline(proc_meminfo, str_ln);
			
			//os << "Lei:<<" << str_ln << ">>" << bj_eol;
			
			if(str_ln.compare(0, str_mem_free_ln, str_mem_free) == 0){
				long pos = find_first_digit(str_ln);
				ch_string str_val = str_ln.substr(pos);
				pos = find_first_digit(str_val, false);
				str_val = str_val.substr(0, pos);
				
				free_mem_kb = atol(str_val.c_str());
				
				//os << "FOUND IT = >" << str_val << "<" << bj_eol;
				break;
			}
		}
		proc_meminfo.close();
	}
	return free_mem_kb;
}

//============================================================
// batch_solver

batch_solver::batch_solver(){
	dbg_mem_at_start = 0;
	init_batch_solver();
}

batch_solver::~batch_solver(){
	//finish_batch_solver();
}

void
batch_solver::init_batch_solver(){

	using_mem_ctrl = false;

	help_str =
		"[<file_name[.lst]> -h -v -paths "
		DBG("[-rr] [+w | -w] [+f | -f] ")
		"-root <root_path> -max_mem <max_kb_to_use>] \n"
		"\n"
		"-h : print help.\n"
		"-v : print version.\n"
		"-paths : print the paths to be used.\n"
		"-root : set the root path to <root_path>.\n"
		"-only_deduc : only do deduction do not use training at all.\n"
		"-write_proof : if unsat write the proof.\n"
		"-test_result : compare and test the result against DB of results.\n"
		"-max_mem : set max number of RAM Kbytes to use while running.\n"
		"\n"
		;

	version_str =
		"v_alpha_0.1\n"
		"(c) 2015. QUIROGA BELTRAN, Jose Luis. c.c. 79523732. Bogota - Colombia.\n"
		;
		
	result_titles_str = "invalid_result_titles_str";

	op_only_deduc = false;
	op_write_proof = false;
	op_test_result = false;
	op_test_save = false;

	op_dbg_as_release = false;
	
	MEM_CTRL(using_mem_ctrl = true);

	dbg_skip_print_info = false;

	reset_err_msg();

	error_cod = -1;

	input_file_nm = "";
	//output_file_nm = "";

	batch_name = "";
	batch_log_errors_pth = "";
	batch_log_results_pth = "";
	batch_log_stats_pth = "";
	batch_log_assigs_pth = "";
	batch_num_files = 0;
	batch_consec = 0;

	batch_num_unknown_satisf = 0;
	batch_num_yes_satisf = 0;
	batch_num_no_satisf = 0;
	batch_num_timeout = 0;
	batch_num_memout = 0;
	batch_num_error = 0;

	batch_stat_lits.vs_nam = "LITS";
	batch_stat_laps.vs_nam = "LAPS";
	batch_stat_solve_tm.vs_nam = "SOLVE SEGs";
	batch_stat_mem_used.vs_nam = "BYTES USED";

	batch_stat_load_tm.vs_nam = "LOAD SEGs";
	batch_stat_num_cnf_saved.vs_nam = "NUM_SAVED";
	batch_stat_num_cnf_finds.vs_nam = "NUM_FINDS";
	batch_stat_quick_discards.vs_nam = "QUICK_DISCARDS";
	

	batch_start_time = 0.0;
	batch_end_time = 0.0;

	batch_instances.clear(true, true);
	batch_test_entries.clear(true, true);
	
	batch_test_has_errors = false;
	
	gg_file_name = "";
	
	bc_slvr_path = "";
	NOT_DBG(bc_slvr_path = path_to_absolute_path(".");)
	
	bc_solver = NULL;
}

void
batch_solver::finish_batch_solver(){
}

bj_ostream&
batch_solver::print_mem_used(bj_ostream& os){
	if(using_mem_ctrl){
		os << batch_stat_mem_used;
	}
	return os;
}

bj_ostream&
batch_solver::print_totals(bj_ostream& os, double curr_tm){

	os << bj_eol;
	if(in_valid_inst()){
		batch_entry& inst_info = get_curr_inst();
		os << "CURR_LAPS=" << inst_info.be_out.bjo_num_laps << " ";
	}
	os << "YES_SAT=" << batch_num_yes_satisf << " ";
	os << "NO_SAT=" << batch_num_no_satisf << " ";
	os << "TMOUT=" << batch_num_timeout << " ";

	os << "MMOUT=" << batch_num_memout << " ";
	os << "ERR=" << batch_num_error << " ";
	os << "UNKNOWN_SAT=" << batch_num_unknown_satisf << " ";

	if(curr_tm != 0.0){
		double full_tm = curr_tm - batch_start_time;
		os << bj_fixed;
		os.precision(2);
		os << "SECS=" << full_tm << " ";
	}

	os << bj_eol;
	os << batch_stat_solve_tm;
	if(using_mem_ctrl){
		os << batch_stat_mem_used;
	}

	os << bj_eol;
	return os;
}

bj_ostream&
batch_solver::print_final_totals(bj_ostream& os){
	os << bj_eol;
	//os << bj_fixed;
	//os.precision(2);

	os << batch_stat_lits;
	os << batch_stat_laps;
	os << batch_stat_num_cnf_saved;
	os << batch_stat_num_cnf_finds;
	
	os << batch_stat_load_tm;
	os << batch_stat_solve_tm;
	MEM_CTRL(
		os << batch_stat_mem_used;
	);

	double tot_tm = batch_end_time - batch_start_time;
	os << "TOTAL TIME = " << tot_tm << bj_eol;
	return os;
}

void			
batch_solver::print_batch_consec(){
	double perc = 0.0;
	if(batch_num_files > 0){
		perc = (batch_consec * 100.0) / batch_num_files;
	}

	PRT_OUT_0( 
	os << CARRIAGE_RETURN;
	//os.width(15);
	//os.fill(' ');
	os << bj_fixed;
	os.precision(2);
	os << batch_consec << " of " << batch_num_files << " (" << perc << " %)";
	os.flush();
	);
}

void
batch_solver::print_final_assig(){
	if(batch_log_assigs_pth.size() == 0){
		return;
	}

	batch_entry& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.be_ff_nam;
	//row_long_t& ff_assg = inst_info.ist_out.bjo_final_assig;

	const char* f_nm = batch_log_assigs_pth.c_str();
	std::ofstream log_stm;
	log_stm.open(f_nm, std::ios::app);
	if(log_stm.good() && log_stm.is_open()){
		log_stm << "c ====================" << bj_eol;
		log_stm << "c " << f_nam << bj_eol;
		const long* assg = bj_get_assig(bc_solver);
		if(assg != NULL){
			long aa = 0;
			while(assg[aa] != 0){
				log_stm << assg[aa] << " ";
				if((aa % 80) == 0){
					log_stm << bj_eol;
				}
			}
		}
		log_stm << " 0" << bj_eol;
	}
	log_stm.close();
}

void
batch_solver::count_instance(batch_entry& inst_info){
	bj_output_t& o_info = inst_info.be_out;
	
	batch_stat_lits.add_val(o_info.bjo_num_lits);
	batch_stat_laps.add_val(o_info.bjo_num_laps);
	batch_stat_load_tm.add_val(o_info.bjo_load_time);
	batch_stat_solve_tm.add_val(o_info.bjo_solve_time);

	batch_stat_num_cnf_saved.add_val(o_info.bjo_num_cnf_saved);
	batch_stat_num_cnf_finds.add_val(o_info.bjo_num_cnf_finds);
	batch_stat_quick_discards.add_val(o_info.bjo_quick_discards);
	
	MEM_CTRL(
		if(using_mem_ctrl){
			long tot_byt = mem_get_num_by_available();
			MARK_USED(tot_byt);
			long by_in_use = mem_get_num_by_in_use();

			batch_stat_mem_used.add_val(by_in_use);
		}
	)

	PRT_OUT_1( print_stats(bj_out));

	PRT_OUT_1( os << "FINISHING" << bj_eol);

	bj_satisf_val_t inst_res = o_info.bjo_result;

	if(inst_res == bjr_unknown_satisf){
		batch_num_unknown_satisf++;
	} else if(inst_res == bjr_yes_satisf){
		PRT_OUT_1( print_final_assig());
		batch_num_yes_satisf++;
	} else if(inst_res == bjr_no_satisf){
		batch_num_no_satisf++;
	} else if(inst_res == bjr_error){
		if(o_info.bjo_error == bje_memout){
			batch_num_memout++;
		} else
		if(o_info.bjo_error == bje_timeout){
			batch_num_timeout++;
		} else {
			batch_num_error++;
		}
	} else {
		BATCH_CK(inst_res == bjr_error);
		batch_num_error++;
	}

	PRT_OUT_1( os << inst_info << bj_eol);

	print_batch_consec();

	//PRT_OUT_0( os << " laps=" << inst_info.ist_num_laps; os.flush(););
}

bj_ostream&
batch_solver::print_mini_stats(bj_ostream& os){
	batch_entry& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.be_ff_nam;

	os << CARRIAGE_RETURN;
	os << "'" << f_nam << "' ";
	//dbg_print_cond_func(NULL, true, false, "NO_NAME", 0, "true", INVALID_DBG_LV);
	return os; 
}

bj_ostream&
batch_solver::print_stats(bj_ostream& os, double current_secs){

	if(current_secs == 0.0){
		current_secs = run_time();
	}

	batch_entry& inst_info = get_curr_inst();
	ch_string f_nam = inst_info.be_ff_nam;

	os << bj_eol;
	os << "file_name: '" << f_nam << "'" << bj_eol;
	
	print_totals(os, current_secs);
	os << bj_eol << bj_eol;

	os.flush();

	return os;
}

void	
batch_solver::log_error_message(const ch_string& msg_log){
	if(batch_log_errors_pth.size() == 0){
		return;
	}

	const char* log_nm = batch_log_errors_pth.c_str();
	std::ofstream log_stm;
	log_stm.open(log_nm, std::ios::app);
	if(log_stm.good() && log_stm.is_open()){
		log_stm << msg_log << bj_eol; 
		log_stm.close();
	}
}

void
batch_solver::log_batch_info(ch_string& the_log_nm){
	if(the_log_nm.size() == 0){
		return;
	}

	const char* log_nm = the_log_nm.c_str();
	std::ofstream log_stm;
	log_stm.open(log_nm, std::ios::app);
	if(! log_stm.good() || ! log_stm.is_open()){
		reset_err_msg();
		error_stm << "Could not open file " << log_nm << ".";
		
		bj_ostr_stream msg_log;
		msg_log << error_stm.str();
		bj_err << msg_log.str() << bj_eol;
		log_error_message(msg_log.str());
		return;
	}

	batch_instances.print_row_data(log_stm, false, "\n");
	log_stm << bj_eol; 
	log_stm.close();
}

void
batch_solver::print_end_msg(){
	if(batch_log_stats_pth.size() == 0){
		return;
	}

	const char* msg_nm = batch_log_stats_pth.c_str();
	std::ofstream msg_stm;
	msg_stm.open(msg_nm, std::ios::app);
	if(! msg_stm.good() || ! msg_stm.is_open()){
		reset_err_msg();
		error_stm << "Could not open file " << msg_nm << ".";
		
		bj_ostr_stream msg_log;
		msg_log << error_stm.str();
		bj_err << msg_log.str() << bj_eol;
		log_error_message(msg_log.str());
		return;
	}

	print_totals(msg_stm);
	print_final_totals(msg_stm);
}

void	
batch_solver::do_all_instances(){

	bj_ostr_stream msg_err;
	try{
		work_all_instances();
	} catch (top_exception& ex1){
		ch_string ex_msg = "got top_exception.";
		DBG(
			ch_string ex_stk = ex1.ex_stk;
			ex_msg += "\n" + ex_stk;
		)
		
		bj_err << ex_msg << bj_eol;
		log_error_message(ex_msg);
		abort_func(0);
	}
	catch (...) {
		bj_out << "INTERNAL ERROR !!! (do_all_instances)" << bj_eol;
		bj_out << STACK_STR << bj_eol;
		bj_out.flush();
		abort_func(0);
	}

	reset_global();
}

void	chomp_string(ch_string& s1){
	long ii = s1.length() - 1;
	//while((ii > 0) && ! isalnum(s1[ii])){
	while((ii > 0) && ! isprint(s1[ii])){
		ii--;
	}
	s1.resize(ii + 1);
}

void	
batch_solver::read_batch_file(row<batch_entry>& names){
	PRT_OUT_0( os << "Loading batch file '" 
		<< batch_name << bj_eol); 

	std::ifstream in_stm;
	in_stm.open(batch_name.c_str());

	if(! in_stm.good() || ! in_stm.is_open()){
		throw file_exception(flx_cannot_open, batch_name);
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
			batch_entry& ist = names.inc_sz();
			ist.be_ff_nam = my_str;
		}
		if((! adding) && (my_str == "*/")){
			adding = true;
		}
	} 

	PRT_OUT_0( os << CARRIAGE_RETURN << names.size());

	if(in_stm.bad()){
		bj_err << in_stm.rdstate() << bj_eol;
	}

	in_stm.close();
	PRT_OUT_0( 
		os << bj_eol
		<< "Read " << names.size() << " file names." << bj_eol
		<< "Processing:" << bj_eol
	);
}

void	print_periodic_totals(void* pm, double curr_tm){
	DBG_CK(pm != NULL);
	batch_solver& gg = *((batch_solver*)pm);
	PRT_OUT_0( gg.print_totals(os, curr_tm));
}

ch_string
batch_solver::init_log_name(ch_string sufix){
	bool is_batch = false;
	ch_string f_nam = get_file_name(is_batch);
	ch_string log_nm = get_log_name(f_nam, sufix);

	remove(log_nm.c_str());
	return log_nm;
}

void	
batch_solver::work_all_instances(){
	batch_start_time = run_time();
	batch_prt_totals_timer.init_timer(PRINT_TOTALS_PERIOD);

	bool is_batch = false;
	ch_string f_nam = get_file_name(is_batch);

	batch_log_errors_pth = init_log_name(LOG_NM_ERROR);
	batch_log_results_pth = init_log_name(LOG_NM_RESULTS);
	batch_log_stats_pth = init_log_name(LOG_NM_STATS);
	batch_log_assigs_pth = init_log_name(LOG_NM_ASSIGS);

	row<batch_entry>& all_insts = batch_instances;
	BATCH_CK(all_insts.is_empty());
	if(is_batch){
		read_batch_file(all_insts);
		bj_restart(bc_solver);
	} else {
		batch_entry& ist = all_insts.inc_sz();
		ist.be_ff_nam = f_nam;
	} 

	batch_num_files = all_insts.size();

	for(long ii = 0; ii < batch_num_files; ii++){
		batch_consec = ii + 1;
		ch_string inst_nam = all_insts[ii].be_ff_nam;

		if(inst_nam.size() > 0){
			batch_prt_totals_timer.
				check_period(print_periodic_totals, this);

			MEM_CTRL(mem_size mem_in_u = mem_get_num_by_in_use());
			MEM_CTRL(MARK_USED(mem_in_u));
			MEM_PT_DIR(dbg_keeping_ptdir = true);

			do_cnf_file();

			MEM_CTRL(
				DBG_COND_COMM(! (mem_in_u == mem_get_num_by_in_use()) ,
					os << "ABORTING_DATA " << bj_eol;
					os << "mem_in_u=" << mem_in_u << bj_eol;
					os << "mem_get_num_by_in_use()=" << mem_get_num_by_in_use() << bj_eol;
					os << "DIFF=" << (mem_get_num_by_in_use() -  mem_in_u) << bj_eol;
					MEM_PT_DIR(dbg_print_ptdir());
					os << "END_OF_aborting_data" << bj_eol;
				)
				BATCH_CK(mem_in_u == mem_get_num_by_in_use());
			)
			MEM_PT_DIR(dbg_keeping_ptdir = false);
		}
	}

	batch_end_time = run_time();
	
	ch_string t_pth = INVALID_TEST_FILE;
	bool do_test = false;
	if(op_test_result){
		t_pth = get_test_file_path();
		do_test = file_exists(t_pth);
		if(! do_test){
			bj_out << "\n\nWARNING!!. Running with test flag.\nBUT FILE=\n\t";
			bj_out << t_pth << "\nNOT FOUND.\nCREATING ONE\n";
			log_batch_info(t_pth);
		}
	}

	log_batch_info(batch_log_results_pth);
	PRT_OUT_0(print_end_msg());
	
	if(do_test){
		read_test_file(batch_test_entries, t_pth);
		test_result_entries();
	}
	
	all_insts.clear(true, true);
}

void
batch_solver::set_input_name(){
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
		BATCH_CK(batch_name.size() == 0);
		batch_name = input_file_nm;
	} else {
		BATCH_CK(gg_file_name.size() == 0);
		gg_file_name = input_file_nm;
	}

}

bool
batch_solver::get_args(int argc, char** argv)
{
	bj_ostream& os = bj_out;
	MARK_USED(os);

	bool prt_help = false;
	bool prt_version = false;
	bool prt_paths = false;
	
	op_dbg_as_release = false;
	
	for(long ii = 1; ii < argc; ii++){
		ch_string the_arg = argv[ii];
		if(the_arg == "-h"){
			prt_help = true;
		} else if(the_arg == "-v"){
			prt_version = true;
		} else if(the_arg == "-paths"){
			prt_paths = true;
		} else if(the_arg == "-only_deduc"){
			op_only_deduc = true;
		} else if(the_arg == "-proof"){
			op_write_proof = true;
		} else if(the_arg == "-test"){
			op_test_result = true;
		} else if(the_arg == "-test-save"){
			op_test_save = true;
		} else if(the_arg == "-rr"){
			op_dbg_as_release = true;
			NOT_DBG(os << "running RELEASE exe. ignoring debug op '-rr'" << bj_eol;)
		} else if((the_arg == "-root") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			ch_string the_pth = argv[kk_idx];
			bc_slvr_path = path_to_absolute_path(the_pth);
		} else if((the_arg == "-max_mem") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			long max_mem = atol(argv[kk_idx]);
			max_mem = (max_mem * NUM_BYTES_IN_KBYTE);
			MEM_CTRL(
				if(max_mem > 0){
					mem_set_num_by_available(max_mem);
				}
			)

		} else if(input_file_nm.size() == 0){
			input_file_nm = argv[ii];
		}
	}
	
	DBG(
		if(op_dbg_as_release && bc_slvr_path.empty()){
			bc_slvr_path = path_to_absolute_path(".");
		}
	)

	if(prt_help){
		os << argv[0] << " " << help_str << bj_eol;
		return false;
	}
	if(prt_version){
		os << argv[0] << " " << version_str << bj_eol;
		return false;
	}
	if(prt_paths){
		const char* pth = bc_slvr_path.c_str();
		bc_solver = bj_solver_create(pth);
		bj_print_paths(bc_solver);
		bj_solver_release(bc_solver);
		return false;
	}
	if(input_file_nm.size() == 0){
		os << argv[0] << " " << help_str << bj_eol;
		os << argv[0] << " " << version_str << bj_eol;
		return false;
	}


	set_input_name();

	return true;
}

int	solver_main(int argc, char** argv){
	batch_solver top_dat;
	
	MEM_CTRL(top_dat.dbg_mem_at_start = mem_get_num_by_in_use());
	DBG(bj_out << "FULL_DEBUG is defined" << bj_eol);
	MEM_CTRL(bj_out << "MEM_CONTROL is defined" << bj_eol);

	BATCH_CK(sizeof(t_1byte) == 1);
	BATCH_CK(sizeof(long) == sizeof(void*));

	int resp = 0;
	
	bool with_proof = false;
	ch_string last_proof_pth = "INVALID_PROOF_PTH";

	MEM_CTRL(mem_size mem_in_u = mem_get_num_by_in_use());
	MEM_CTRL(MARK_USED(mem_in_u));

	MEM_CTRL(mem_set_num_by_available(get_free_mem_kb() * NUM_BYTES_IN_KBYTE));

	bool args_ok = top_dat.get_args(argc, argv);
	if(args_ok){
		const char* pth = top_dat.bc_slvr_path.c_str();
		top_dat.bc_solver = bj_solver_create(pth);
		
		if(top_dat.op_only_deduc){
			bj_set_param_char(top_dat.bc_solver, bjp_only_deduc, 1);
		}
		if(top_dat.op_write_proof){
			bj_set_param_char(top_dat.bc_solver, bjp_write_proofs, 1);
		}
		DBG(
			if(top_dat.op_dbg_as_release){ 
				bj_set_param_char(top_dat.bc_solver, bjp_as_release, 1);
			}
		);

		PRT_OUT_1( os << ".STARTING AT " << run_time() << bj_eol);

		MEM_CTRL(
			double tot_mem = (double)(mem_get_num_by_available());
			top_dat.batch_stat_mem_used.vs_max_val = tot_mem;

			PRT_OUT_0( os << "Starting with "
				<< mem_get_num_by_available() << " bytes available" 
				<< bj_eol); 
		)
		DBG(PRT_OUT_1( os << "DEBUG_BRAIN activated" 
			<< bj_eol));
		top_dat.do_all_instances();

		PRT_OUT_1( os << ".ENDING AT " << run_time() << bj_eol);

		if(top_dat.op_write_proof){
			with_proof = true;
			const char* pf_pth = bj_get_last_proof_path(top_dat.bc_solver);
			if(pf_pth != NULL_PT){
				last_proof_pth = pf_pth;
			}
		}
		
		bj_solver_release(top_dat.bc_solver);
	}
	
	MEM_CTRL(BATCH_CK(mem_in_u == mem_get_num_by_in_use()));
	
	if(args_ok){
		PRT_OUT_0( 
			top_dat.print_totals(os);
			top_dat.print_final_totals(os);
			os << "ROOT_DIR=" << top_dat.bc_slvr_path << bj_eol;
		);
		
		if(with_proof){
			PRT_OUT_0(os << "\n\nLAST_PROOF_PATH=\n" << last_proof_pth << "\n\n\n");
		}
	}
	
	if(top_dat.batch_test_has_errors){
		PRT_OUT_0(
			os << "\n\nTEST_HAS_ERRORS !!!!\nTEST_HAS_ERRORS !!!!\nTEST_HAS_ERRORS !!!!\n";
		);
	}

	MEM_CTRL(bj_out << "MEM_CONTROL is defined" << bj_eol);
	DBG(bj_out << "FULL_DEBUG is defined" << bj_eol);

	MEM_CTRL(
		DBG_CK_2(top_dat.dbg_mem_at_start == mem_get_num_by_in_use(), 
			os << "dbg_mem_at_start=" << top_dat.dbg_mem_at_start << bj_eol;
			os << "mem_get_num_by_in_use()=" << mem_get_num_by_in_use() << bj_eol
		);
	)
	return resp;
}

void
batch_solver::do_cnf_file()
{
	BATCH_CK(bc_solver != NULL);

	batch_entry& curr_inst = get_curr_inst();
	const char* ff = curr_inst.be_ff_nam.c_str();
	
	bj_solve_file(bc_solver, ff);
	
	if(op_write_proof){
		const char* pf_pth = bj_get_last_proof_path(bc_solver);
		if(pf_pth != NULL_PT){
			PRT_OUT_0(os << "LAST_PROOF_PATH=\n" << pf_pth << "\n");
		}
	}
	
	curr_inst.be_out = bj_get_output(bc_solver);
	curr_inst.be_result_str = bj_get_result_string(bc_solver);
	//bj_restart(bc_solver);

	if(op_write_proof){
		bj_out << "SAVED_IST=" << curr_inst.be_out.bjo_num_cnf_saved << "\n";
		bj_out << "NUM_FINDS_IST=" << curr_inst.be_out.bjo_num_cnf_finds << "\n";
	}

	count_instance(curr_inst);
}

int	main(int argc, char** argv){
	MEM_CTRL(mem_start_stats());
	//row<bool> dbg_arr;
	//dbg_init_lv_arr(dbg_arr);
	
	int rr = solver_main(argc, argv);
	
	//dbg_arr.clear(true, true);
	MEM_CTRL(mem_finish_stats());
	return rr;
	//return 0;
}

ch_string
batch_solver::get_test_file_path(){
	ch_string inv_pth = INVALID_TEST_FILE;
	if(! op_test_result){
		return inv_pth;
	}
	
	bool is_batch = false;
	ch_string f_nam = get_file_name(is_batch);
	
	row<batch_entry>& all_insts = batch_instances;
	if(all_insts.is_empty()){
		return inv_pth;
	}
	
	batch_entry& fst_ee = all_insts.first();
	bj_output_t& out = fst_ee.be_out;
	ch_string ee = "";
	ch_string conf_ss = ee +
		cbool_to_str(out.bjo_dbg_enabled) +
		cbool_to_str(out.bjo_dbg_never_write) +
		cbool_to_str(out.bjo_dbg_never_find) +
		cbool_to_str(out.bjo_dbg_min_trainable) +
		cbool_to_str(out.bjo_dbg_as_release);
		
	ch_string sufix = conf_ss + TEST_SUF;
	
	ch_string t_pth = get_log_name(f_nam, sufix);
	
	return t_pth;
}

void
batch_solver::read_test_file(row<batch_entry>& test_entries, ch_string& file_nm_str){
	test_entries.clear(true, true);
	
	const char* file_nm = file_nm_str.c_str();
	bj_ostream& os = bj_out;
	BATCH_CK(file_nm != NULL_PT);

	std::ifstream in_stm;

	in_stm.open(file_nm, std::ios::binary);
	if(!in_stm.good() || !in_stm.is_open()){
		os << "TEST_FAILED. CANNOT READ FILE '" << file_nm << "'.\n";
		return;
	}
	
	result_titles_str = bj_get_result_titles_string(bc_solver);

	ch_string str_ln;
	while(! in_stm.eof()){
		std::getline(in_stm, str_ln);
		
		const char* the_line = str_ln.c_str();
		bj_parse_result_string(bc_solver, the_line);
		
		batch_entry& ee = test_entries.inc_sz();
		ee.be_ff_nam = bj_get_solve_file_path(bc_solver);
		ee.be_out = bj_get_output(bc_solver);
		ee.be_result_str = bj_get_result_string(bc_solver);
	}
	in_stm.close();

}

void
batch_solver::test_result_entries(){
	row<batch_entry>& result_entries = batch_instances;
	row<batch_entry>& test_entries = batch_test_entries;
	
	for(long aa = 0; aa < result_entries.size(); aa++){
		batch_entry& rr = result_entries[aa];
		if(! test_entries.is_valid_idx(aa)){
			reset_err_msg();
			error_stm << "TEST_FAILED. More results than test entries\n";
			
			bj_ostr_stream msg_log;
			msg_log << error_stm.str();
			bj_err << msg_log.str() << bj_eol;
			log_error_message(msg_log.str());
			
			batch_test_has_errors = true;
			break;
		}
		batch_entry& tt = test_entries[aa];
		bool ee_ok = test_entry(rr, tt);
		if(! ee_ok){
			reset_err_msg();
			error_stm << "TEST_FAILED. Entries differ\n";
			error_stm << "fi=" << result_titles_str << "\n";
			error_stm << "rr=" << rr << "\n";
			error_stm << "tt=" << tt << "\n";
			
			bj_ostr_stream msg_log;
			msg_log << error_stm.str();
			bj_err << msg_log.str() << bj_eol;
			log_error_message(msg_log.str());

			batch_test_has_errors = true;
		}
	}
}

bool
batch_solver::test_entry(batch_entry& rr, batch_entry& tt){
	bool c1 = (rr.be_ff_nam == tt.be_ff_nam);
	
	bj_output_t& rrd = rr.be_out;
	bj_output_t& ttd = tt.be_out;
	
	bool c2 = (rrd.bjo_result == ttd.bjo_result);
	bool c3 = (rrd.bjo_num_laps == ttd.bjo_num_laps);
	bool c4 = (rrd.bjo_num_recoils == ttd.bjo_num_recoils);
	bool c5 = (rrd.bjo_num_cnf_saved == ttd.bjo_num_cnf_saved);
	bool c6 = (rrd.bjo_num_cnf_finds == ttd.bjo_num_cnf_finds);
	bool c7 = (rrd.bjo_num_lits == ttd.bjo_num_lits);
	bool c8 = (rrd.bjo_num_vars == ttd.bjo_num_vars);
	bool c9 = (rrd.bjo_num_ccls == ttd.bjo_num_ccls);
	
	bool all_ok = (c1 && c2 && c3 && c4 && c5 && c6 && c7 && c8 && c9);
	
	return all_ok;
}

