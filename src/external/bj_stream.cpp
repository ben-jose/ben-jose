

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

bj_stream.cpp  

bj_stream funcs.

--------------------------------------------------------------*/

#include "top_exception.h"
#include "bj_stream.h"

void
bj_ofstream_open(ch_string& path, bj_ofstream& stm, bool append){
	const char* log_nm = path.c_str();
	if(append){
		stm.open(log_nm, std::ios::app);
	} else {
		stm.open(log_nm, std::ios::binary);
	}
	if(! stm.good() || ! stm.is_open()){
		ch_string msg = "Could not open file " + path;
		abort_func(1, msg.c_str());
	}
}

