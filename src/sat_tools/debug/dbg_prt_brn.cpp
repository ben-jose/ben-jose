

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

dbg_prt_brn.cpp  

dbg_prt_brn funcs.

--------------------------------------------------------------*/

#include <sstream>
#include <execinfo.h>
#include <cstdlib>

#include "bj_stream.h"
#include "ch_string.h"
#include "brain.h"
#include "dbg_run_satex.h"

void 
dbg_prt_all_cho(brain& brn){
	bool is_batch = false;
	ch_string f_nam = GLB().get_file_name(is_batch);
	ch_string cho_nam = f_nam + "_chosen.log";

	const char* log_nm = cho_nam.c_str();
	std::ofstream log_stm;
	//log_stm.open(log_nm, std::ios::app);
	log_stm.open(log_nm, std::ios::binary);
	if(! log_stm.good() || ! log_stm.is_open()){
		ch_string msg = "Could not open file " + cho_nam;
		abort_func(1, msg.c_str());
	}

	//dbg_get_all_chosen(br_dbg_all_chosen);

	log_stm << brn.br_dbg_all_chosen;
	log_stm << bj_eol; 
	log_stm.close();
}

