

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

config.h

Declaration of functions to read and parse config files.

--------------------------------------------------------------*/

#ifndef CONFIG_H
#define CONFIG_H

#include "tools.h"

#define CONFIG_DBG(prm)	DBG(prm)
#define CONFIG_CK(prm) 	DBG_CK(prm)

class brain;

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
// config_reader

void	dbg_init_dbg_conf(brain& brn);
void	dbg_update_config_entries(brain& brn);

class config_reader {
	public:
	row<long>		dbg_config_line;

	config_reader(){
	}

	~config_reader(){}

	void 	parse_debug_line(row<long>& dbg_line, ch_string& str_ln);
	void	add_config_line(brain& brn, ch_string& str_ln);
	void	read_config(brain& brn, const char* f_name);
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

