

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

dbg_config.cpp  

Functions to read and parse debug config files.

--------------------------------------------------------------*/

#include <fstream>

#include "file_funcs.h"
#include "parse_funcs.h"
#include "dbg_config.h"

void
config_reader::parse_debug_line(row<long>& dbg_line, ch_string& str_ln){
#ifdef FULL_DEBUG
	bj_ostream& os = bj_out;
	MARK_USED(os);
	
	const char* pt_in = str_ln.c_str();

	dbg_line.clear();
	
	long num_ln = 0;

	if(isalnum(*pt_in)){
		skip_whitespace(pt_in, num_ln);
		while(isdigit(*pt_in) || isspace(*pt_in)){
			if(isspace(*pt_in)){
				pt_in++;
				continue;
			}
			//os << pt_in << "$\n";
			
			long val = parse_int(pt_in, num_ln); 
			//skip_whitespace(pt_in, num_ln);
	
			dbg_line.push(val);
		}
	} else {
		skip_line(pt_in, num_ln);
	}
#endif
}

void	
config_reader::add_config_line(debug_info& dbg_info, ch_string& str_ln){
#ifdef FULL_DEBUG
	bj_ostream& os = bj_out;
	MARK_USED(os);
	row<long>& dbg_ln = dbg_config_line;
	parse_debug_line(dbg_ln, str_ln);
	
	//os << " dbg_ln=" << dbg_ln << "\n";

	if(! dbg_ln.is_empty()){
		debug_entry& start_dbg = dbg_info.dbg_start_dbg_entries.inc_sz();

		long debug_id = dbg_ln[0];
		start_dbg.dbg_id = debug_id;

		if(dbg_ln.size() > 1){
			start_dbg.dbg_recoil = dbg_ln[1];
		}

		if(dbg_ln.size() > 2){
			debug_entry& stop_dbg = dbg_info.dbg_stop_dbg_entries.inc_sz();
			stop_dbg.dbg_id = debug_id;
			stop_dbg.dbg_recoil = dbg_ln[2];
		}
	}
#endif
}

void
config_reader::read_config(debug_info& dbg_info, const char* file_nm){
#ifdef FULL_DEBUG
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
		
		add_config_line(dbg_info, str_ln);
	}
	in_stm.close();

	dbg_config_line.clear(false, true);

	dbg_info.dbg_start_dbg_entries.mix_sort(cmp_dbg_entries);
	dbg_info.dbg_stop_dbg_entries.mix_sort(cmp_dbg_entries);
	
	/*
	os << " FULL_DBG_CONFIG\n";
	os << dbg_info;
	os << " END_OF_FULL_DBG_CONFIG\n";
	*/
#endif
}

void	dbg_read_dbg_conf(debug_info& dbg_info){
#ifdef FULL_DEBUG
	config_reader conf_rdr;
	conf_rdr.read_config(dbg_info, "dbg_ben_jose.conf");
#endif
}

void	dbg_init_dbg_conf(debug_info& dbg_info){
#ifdef FULL_DEBUG
	dbg_info.dbg_levs_arr.fill(false, DBG_NUM_LEVS);

	dbg_info.dbg_current_start_idx = 0;
	dbg_info.dbg_current_stop_idx = 0;
	
	bj_big_int_t curr_reco = 0;
	dbg_update_config_entries(dbg_info, curr_reco);
#endif
}

void
dbg_update_config_entries(debug_info& dbg_info, bj_big_int_t curr_reco){
#ifdef FULL_DEBUG
	row<bool>& dbg_arr = dbg_info.dbg_levs_arr;
	
	bj_ostream& os = bj_out;
	MARK_USED(os);

	long& start_idx = dbg_info.dbg_current_start_idx;
	long& stop_idx = dbg_info.dbg_current_stop_idx;

	row<debug_entry>& start_lst = dbg_info.dbg_start_dbg_entries;
	row<debug_entry>& stop_lst = dbg_info.dbg_stop_dbg_entries;

	while(	(start_idx < start_lst.size()) && 
		(start_lst[start_idx].dbg_recoil <= curr_reco))
	{
		long start_dbg_id = start_lst[start_idx].dbg_id;
		CONFIG_CK(dbg_arr.is_valid_idx(start_dbg_id));
		dbg_arr[start_dbg_id] = true;
		start_idx++;
	} 

	while(	(stop_idx < stop_lst.size()) && 
		(stop_lst[stop_idx].dbg_recoil < curr_reco))
	{
		long stop_dbg_id = stop_lst[stop_idx].dbg_id;
		CONFIG_CK(dbg_arr.is_valid_idx(stop_dbg_id));
		dbg_arr[stop_dbg_id] = false;
		stop_idx++;
	} 
#endif
}


