

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

parse_funcs.cpp  

parse funcs.

--------------------------------------------------------------*/

#include "tools.h"

#include "parse_funcs.h"
#include "file_funcs.h"


ch_string get_parse_err_msg(ch_string hd_msg, long num_line, char ch_err, ch_string msg)
{
	bj_ostr_stream err_msg;

	err_msg << hd_msg;
	if(num_line >= 0){
		err_msg << "line " << num_line << ". ";
	}
	if(ch_err >= 0){
		err_msg << "invalid character '" << ch_err << "'. ";
	}
	err_msg << msg;
	return err_msg.str();
}

void skip_whitespace(const char*& pt_in, long& line){
	while(	(! isalnum(*pt_in) || isspace(*pt_in)) && 
			(*pt_in != '-') && (*pt_in != '+') && (*pt_in != END_OF_SEC))
	{ 
		if(*pt_in == '\n'){ 
			line++; 
		}
		pt_in++; 
	}
}

void skip_line(const char*& pt_in, long& line){
	while(*pt_in != END_OF_SEC){
		if(*pt_in == '\n'){ 
			line++; 
			pt_in++; 
			return; 
		}
		pt_in++; 
	}
}

ch_string 
read_text_line(const char*& pt_in, long& line){
	ch_string the_ln = "";
	const char* pt_0 = pt_in;
	bool all_prt = true;

	while(*pt_in != END_OF_SEC){
		if(*pt_in == '\n'){ 
			if(all_prt){
				char* pt_1 = (char*)pt_in;
				(*pt_1) = '0';
				the_ln = pt_0;
				(*pt_1) = '\n';
			}
			PARSE_FN_CK((*pt_in) == '\n');

			line++; 
			pt_in++; 
			return the_ln; 
		}
		if(! isprint(*pt_in)){
			all_prt = false;
		}
		pt_in++; 
	}
	return the_ln;
}

long parse_long(const char*& pt_in, long line) {
	long	val = 0;
	bool	neg = false;

	if(*pt_in == '-'){ neg = true; pt_in++; }
	else if(*pt_in == '+'){ pt_in++; }

	if( ! isdigit(*pt_in)){
		throw parse_exception(pax_bad_int, (char)(*pt_in), line);
	}
	while(isdigit(*pt_in)){
		val = val*10 + (*pt_in - '0');
		pt_in++;
	}
	return (neg)?(-val):(val);
}

long parse_long_str(ch_string& in_str){
	long line = 0;
	const char* pt_in = in_str.c_str();
	long vv = parse_long(pt_in, line);
	return vv;
}
