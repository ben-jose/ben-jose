

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

parse_funcs.h

parse funcs.

--------------------------------------------------------------*/

#ifndef PARSE_FUNCS_H
#define PARSE_FUNCS_H

#include "top_exception.h"

//======================================================================
// parse_exception

typedef enum {
	pax_bad_int
} pa_ex_cod_t;

class parse_exception : public top_exception {
public:
	char	val;
	long 	line;
	parse_exception(long the_id = 0, char vv = 0, long ll = 0) : top_exception(the_id)
	{
		val = vv;
		line = ll;
	}
};


#endif		// PARSE_FUNCS_H


