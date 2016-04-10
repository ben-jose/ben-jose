

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

top_exception.h

Top exception class.

--------------------------------------------------------------*/


#ifndef TOP_EXCEPTION_H
#define TOP_EXCEPTION_H

#include "platform.h"
#include "ch_string.h"

#define as_pt_char(the_str) (const_cast<char *>(the_str))

void abort_func(long val, const char* msg = as_pt_char("Aborting."));

class top_exception {
private:
	top_exception&  operator = (top_exception& other){
		abort_func(0);
		return (*this);
	}
	
public:
	long		ex_id;
	ch_string 	ex_stk;
	ch_string 	ex_assrt;
	
	top_exception(long the_id = 0, ch_string assrt_str = "");
	
	~top_exception(){
	}
};

#endif // TOP_EXCEPTION_H


