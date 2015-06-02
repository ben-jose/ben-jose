

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

config.h

Declaration of functions to read and parse config files.

--------------------------------------------------------------*/

#ifndef CONFIG_H
#define CONFIG_H

#include "tools.h"

#define CONFIG_DBG(prm)	DBG(prm)
#define CONFIG_CK(prm) 	DBG_CK(prm)

#define DBG_NUM_LEVS 200

//=================================================================
// debug_entry

class debug_entry {
	public:
	long		dbg_round;
	long		dbg_id;

	debug_entry(){
		dbg_round = -1;
		dbg_id = -1;
	}

	~debug_entry(){
	}

	bj_ostream& 	print_debug_entry(bj_ostream& os);

};

inline
comparison	cmp_dbg_entries(debug_entry const & e1, debug_entry const & e2){
	return cmp_long(e1.dbg_round, e2.dbg_round);
}

//=================================================================
// debug_info

class debug_info {
	public:
	row<debug_entry>	dbg_start_dbg_entries;
	row<debug_entry>	dbg_stop_dbg_entries;
	long			dbg_current_start_entry;
	long			dbg_current_stop_entry;
	row<bool>	dbg_levs_arr;
	bool 		dbg_bad_cycle1;

	debug_info(){
		init_debug_info();
	}

	~debug_info(){
	}
	
	void init_debug_info(){
		dbg_start_dbg_entries.clear();
		dbg_stop_dbg_entries.clear();
		dbg_current_start_entry = 0;
		dbg_current_stop_entry = 0;
		dbg_levs_arr.fill(false, DBG_NUM_LEVS);
		dbg_bad_cycle1 = false;
	}

	void	dbg_lv_on(long lv_idx){
		CONFIG_CK(dbg_levs_arr.is_valid_idx(lv_idx));
		dbg_levs_arr[lv_idx] = true;
	}

	void	dbg_lv_off(long lv_idx){
		CONFIG_CK(dbg_levs_arr.is_valid_idx(lv_idx));
		dbg_levs_arr[lv_idx] = false;
	}
	
	//bj_ostream& 	print_debug_info(bj_ostream& os);

};

//=================================================================
// config_reader

void	dbg_init_dbg_conf(debug_info& dbg_inf);
void	dbg_update_config_entries(debug_info& dbg_inf, bj_big_int_t curr_round);

class config_reader {
	public:
	row<long>		dbg_config_line;

	config_reader(){
	}

	~config_reader(){}

	void 	parse_debug_line(row<long>& dbg_line, ch_string& str_ln);
	void	add_config_line(debug_info& dbg_inf, ch_string& str_ln);
	void	read_config(debug_info& dbg_inf, const char* f_name);
};

//void rm_ic_files();

//=================================================================
// FUNCTION

inline
bj_ostream& 	
debug_entry::print_debug_entry(bj_ostream& os){
	os << "dbg(p=" << dbg_round << ", i=" << dbg_id << ")";
	return os;
}

inline
bj_ostream& operator << (bj_ostream& os, debug_entry& dbg_ety){
	return dbg_ety.print_debug_entry(os);
}


#endif		// CONFIG_H

