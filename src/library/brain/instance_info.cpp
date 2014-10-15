

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

instance_info.cpp  

all info to keep or return of an instance cnf to solve.

--------------------------------------------------------------*/

#include "instance_info.h"

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
			INSTANCE_CK((*pt_in) == sep);

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


