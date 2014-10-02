

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

dimacs_exception.h

dimacs_exception class.

--------------------------------------------------------------*/


#ifndef DIMACS_EXCEPTION_H
#define DIMACS_EXCEPTION_H

#include "top_exception.h"

class dimacs_exception : public top_exception{
public:
	dimacs_exception(char* descr = as_pt_char("undefined dimacs exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

#endif // DIMACS_EXCEPTION_H


