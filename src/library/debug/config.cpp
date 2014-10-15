

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

config.cpp  

Functions to read and parse config files.

--------------------------------------------------------------*/

#include <fstream>

#include "file_funcs.h"
#include "config.h"
#include "support.h"


//======================================================================
// parse_exception

class parse_exception : public top_exception {
public:
	parse_exception(char* descr = as_pt_char("undefined parse exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

//======================================================================
// parse funcs

bj_ostr_stream& parse_err_msg(ch_string hd_msg, long num_line, char ch_err, ch_string msg)
{
	GLB().reset_err_msg();
	bj_ostr_stream& err_msg = GLB().error_stm;

	err_msg << hd_msg;
	if(num_line >= 0){
		err_msg << "line " << num_line << ". ";
	}
	if(ch_err >= 0){
		err_msg << "invalid character '" << ch_err << "'. ";
	}
	err_msg << msg;
	return err_msg;
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
			SUPPORT_CK((*pt_in) == '\n');

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

integer parse_int(const char*& pt_in, long line) {
	integer	val = 0;
	bool	neg = false;

	if(*pt_in == '-'){ neg = true; pt_in++; }
	else if(*pt_in == '+'){ pt_in++; }

	if( ! isdigit(*pt_in)){
		bj_ostr_stream& msg = 
			parse_err_msg("PARSE ERROR. ", line, (char)(*pt_in), "");
		MARK_USED(msg);

		char* parse_bad_int = as_pt_char("parsing exception. bad integer.");
		DBG_THROW_CK(parse_bad_int != parse_bad_int);
		throw parse_exception(parse_bad_int);
		abort_func(1, parse_bad_int);
	}
	while(isdigit(*pt_in)){
		val = val*10 + (*pt_in - '0');
		pt_in++;
	}
	return (neg)?(-val):(val);
}

void
config_reader::parse_debug_line(row<long>& dbg_line, ch_string& str_ln){
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
}

void	
config_reader::add_config_line(ch_string& str_ln){
	bj_ostream& os = bj_out;
	MARK_USED(os);
	row<long>& dbg_ln = GLB().dbg_config_line;
	parse_debug_line(dbg_ln, str_ln);

	//os << "Entendi:<<" << dbg_ln << ">>" << bj_eol;

	if(! dbg_ln.is_empty()){
		debug_entry& start_dbg = GLB().dbg_start_dbg_entries.inc_sz();

		long debug_id = dbg_ln[0];
		start_dbg.dbg_id = debug_id;

		if(dbg_ln.size() > 1){
			start_dbg.dbg_round = dbg_ln[1];
		}

		if(dbg_ln.size() > 2){
			debug_entry& stop_dbg = GLB().dbg_stop_dbg_entries.inc_sz();
			stop_dbg.dbg_id = debug_id;
			stop_dbg.dbg_round = dbg_ln[2];
		}
	}
}

void
config_reader::read_config(const char* file_nm){
	bj_ostream& os = bj_out;
	CONFIG_CK(file_nm != NULL_PT);

	SUPPORT_CK(GLB().dbg_start_dbg_entries.is_empty());
	SUPPORT_CK(GLB().dbg_stop_dbg_entries.is_empty());
	SUPPORT_CK(GLB().dbg_config_line.is_empty());

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
		add_config_line(str_ln);
	}
	in_stm.close();

	GLB().dbg_config_line.clear(false, true);

	GLB().dbg_start_dbg_entries.mix_sort(cmp_dbg_entries);
	GLB().dbg_stop_dbg_entries.mix_sort(cmp_dbg_entries);

	//os << "start_dbgs=" << GLB().dbg_start_dbg_entries << bj_eol;
	//os << "stop_dbgs=" << GLB().dbg_stop_dbg_entries << bj_eol;
}



