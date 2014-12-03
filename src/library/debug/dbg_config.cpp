

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

dbg_config.cpp  

Functions to read and parse debug config files.

--------------------------------------------------------------*/

#include <fstream>

#include "file_funcs.h"
#include "parse_funcs.h"
#include "brain.h"
#include "dbg_config.h"

void
config_reader::parse_debug_line(row<long>& dbg_line, ch_string& str_ln){
#ifdef FULL_DEBUG
	const char* pt_in = str_ln.c_str();

	dbg_line.clear();
	
	long num_ln = 0;

	if(isalnum(*pt_in)){
		skip_whitespace(pt_in, num_ln);
		while(isdigit(*pt_in)){
			long val = parse_int(pt_in, num_ln); 
			skip_whitespace(pt_in, num_ln);
	
			dbg_line.push(val);
		}
	} else {
		skip_line(pt_in, num_ln);
	}
#endif
}

void	
config_reader::add_config_line(brain& brn, ch_string& str_ln){
#ifdef FULL_DEBUG
	dbg_inst_info& dbg_info = brn.br_dbg;
	bj_ostream& os = bj_out;
	MARK_USED(os);
	row<long>& dbg_ln = dbg_config_line;
	parse_debug_line(dbg_ln, str_ln);

	//os << "Entendi:<<" << dbg_ln << ">>" << bj_eol;

	if(! dbg_ln.is_empty()){
		debug_entry& start_dbg = dbg_info.dbg_start_dbg_entries.inc_sz();

		long debug_id = dbg_ln[0];
		start_dbg.dbg_id = debug_id;

		if(dbg_ln.size() > 1){
			start_dbg.dbg_round = dbg_ln[1];
		}

		if(dbg_ln.size() > 2){
			debug_entry& stop_dbg = dbg_info.dbg_stop_dbg_entries.inc_sz();
			stop_dbg.dbg_id = debug_id;
			stop_dbg.dbg_round = dbg_ln[2];
		}
	}
#endif
}

void
config_reader::read_config(brain& brn, const char* file_nm){
#ifdef FULL_DEBUG
	dbg_inst_info& dbg_info = brn.br_dbg;
	bj_ostream& os = bj_out;
	CONFIG_CK(file_nm != NULL_PT);

	CONFIG_CK(dbg_info.dbg_start_dbg_entries.is_empty());
	CONFIG_CK(dbg_info.dbg_stop_dbg_entries.is_empty());
	CONFIG_CK(dbg_config_line.is_empty());

	//mini_test();

	std::ifstream in_stm;

	in_stm.open(file_nm, std::ios::binary);
	if(!in_stm.good() || !in_stm.is_open()){
		os << "NO " << file_nm << " FILE FOUND." << bj_eol;
		return;
	}

	ch_string str_ln;

	while(! in_stm.eof()){
		std::getline(in_stm, str_ln);
		
		//os << "Lei:<<" << str_ln << ">>" << bj_eol;
		add_config_line(brn, str_ln);
	}
	in_stm.close();

	dbg_config_line.clear(false, true);

	dbg_info.dbg_start_dbg_entries.mix_sort(cmp_dbg_entries);
	dbg_info.dbg_stop_dbg_entries.mix_sort(cmp_dbg_entries);

	//os << "start_dbgs=" << dbg_info.dbg_start_dbg_entries << bj_eol;
	//os << "stop_dbgs=" << dbg_info.dbg_stop_dbg_entries << bj_eol;
#endif
}

void	dbg_init_dbg_conf(brain& brn){
#ifdef FULL_DEBUG
	dbg_inst_info& dbg_info = brn.br_dbg;
	config_reader conf_rdr;
	conf_rdr.read_config(brn, "yosoy.conf");

	dbg_info.dbg_current_start_entry = 0;
	dbg_info.dbg_current_stop_entry = 0;
	dbg_update_config_entries(brn);

	/*
	DBG_COMMAND(37, os << "PRINT_FULL_INFO" << bj_eol; 
		dbg_info.dbg_skip_print_info = true);

	DBG_COMMAND(40, os << "SET IC GEN JPG" << bj_eol; 
		dbg_info.dbg_ic_gen_jpg = true);

	dbg_ic_max_seq = -1;
	dbg_ic_seq = 0;
	*/
#endif
}

void
dbg_update_config_entries(brain& brn){
#ifdef FULL_DEBUG
	
	dbg_inst_info& dbg_info = brn.br_dbg;
	row<bool>& dbg_arr = dbg_info.dbg_levs_arr;
	
	bj_ostream& os = bj_out;
	MARK_USED(os);

	recoil_counter_t curr_round = brn.recoil();

	long& start_idx = dbg_info.dbg_current_start_entry;
	long& stop_idx = dbg_info.dbg_current_stop_entry;

	row<debug_entry>& start_lst = dbg_info.dbg_start_dbg_entries;
	row<debug_entry>& stop_lst = dbg_info.dbg_stop_dbg_entries;

	while(	(start_idx < start_lst.size()) && 
		(start_lst[start_idx].dbg_round <= curr_round))
	{
		long start_dbg_id = start_lst[start_idx].dbg_id;
		CONFIG_CK(dbg_arr.is_valid_idx(start_dbg_id));
		dbg_arr[start_dbg_id] = true;
		start_idx++;
	} 

	while(	(stop_idx < stop_lst.size()) && 
		(stop_lst[stop_idx].dbg_round < curr_round))
	{
		long stop_dbg_id = stop_lst[stop_idx].dbg_id;
		CONFIG_CK(dbg_arr.is_valid_idx(stop_dbg_id));
		dbg_arr[stop_dbg_id] = false;
		stop_idx++;
	} 
#endif
}

