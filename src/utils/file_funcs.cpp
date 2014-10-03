

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

file_funcs.cpp  

file funcs.

--------------------------------------------------------------*/

#include <sstream>
#include <execinfo.h>
#include <cstdlib>
#include <fstream>

#include "tools.h"
#include "file_funcs.h"

void
read_file(ch_string f_nam, row<char>& f_data){
	const char* ff_nn = f_nam.c_str();
	std::ifstream istm;
	istm.open(ff_nn, std::ios::binary);
	if(! istm.good() || ! istm.is_open()){
		//bj_ostr_stream& msg = file_err_msg(0, -1, "Could not open file ");
		//msg << "'" << f_nam << "'.";

		//DBG_THROW(bj_out << msg.str() << bj_eol);

		char* file_cannot_open = as_pt_char("Cannot open file exception");
		DBG_THROW_CK(file_cannot_open != file_cannot_open);
		throw file_exception(file_cannot_open);
		abort_func(1, file_cannot_open);
	}

	// get size of file:
	istm.seekg (0, std::ios::end);
	long file_sz = istm.tellg();
	istm.seekg (0, std::ios::beg);

	if(file_sz < 0){
		//bj_ostr_stream& msg = file_err_msg(0, -1,
		//	"Could not compute file size. ");
		//msg << "'" << f_nam << "'.";

		char* file_cannot_size = as_pt_char("Cannot compute file size exception");
		DBG_THROW_CK(file_cannot_size != file_cannot_size);
		throw file_exception(file_cannot_size);
		abort_func(1, file_cannot_size);
	}

	DBG_CK(sizeof(char) == 1);

	char* file_content = tpl_malloc<char>(file_sz + 1);
	istm.read(file_content, file_sz);
	long num_read = istm.gcount();
	if(num_read != file_sz){
		tpl_free<char>(file_content, file_sz + 1);

		//bj_ostr_stream& msg = file_err_msg(0, -1,
		//	"Could not read full file into memory. ");
		//msg << "'" << f_nam << "'.";

		char* file_cannot_fit = as_pt_char("Cannot fit file in memory exception");
		DBG_THROW_CK(file_cannot_fit != file_cannot_fit);
		throw file_exception(file_cannot_fit);
		abort_func(1, file_cannot_fit);
	}
	file_content[file_sz] = END_OF_SEC;

	s_row<char> tmp_rr;
	tmp_rr.init_data(file_content, file_sz + 1);

	f_data.clear();
	tmp_rr.move_to(f_data);

	DBG_CK(f_data.last() == END_OF_SEC);
}



