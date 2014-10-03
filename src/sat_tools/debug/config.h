

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

#include <fstream>
#include <sstream>

#include "tools.h"

// '\0'
#define END_OF_SEC	0

#define CONFIG_DBG(prm)	DBG(prm)
#define CONFIG_CK(prm) 	DBG_CK(prm)

//=================================================================
// parser funcs

std::ostringstream& parse_err_msg(ch_string hd_msg, long num_line, char ch_err, ch_string msg);

void skip_whitespace(const char*& pt_in, long& line);
void skip_line(const char*& pt_in, long& line);
ch_string read_text_line(const char*& pt_in, long& line);
integer parse_int(const char*& pt_in, long line);


//=================================================================
// config_reader

class config_reader {
	public:

	config_reader(){
	}

	~config_reader(){}

	void 	parse_debug_line(row<long>& dbg_line, ch_string& str_ln);
	void	add_config_line(ch_string& str_ln);
	void	read_config(const char* f_name);
};



#endif		// CONFIG_H

