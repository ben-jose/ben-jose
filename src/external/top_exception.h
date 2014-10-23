

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

top_exception.h

Top exception class.

--------------------------------------------------------------*/


#ifndef TOP_EXCEPTION_H
#define TOP_EXCEPTION_H

#define as_pt_char(the_str) (const_cast<char *>(the_str))

void abort_func(long val, const char* msg = as_pt_char("Aborting."));

class top_exception {
private:
	top_exception&  operator = (top_exception& other){
		char* top_ex_bad_eq_op = as_pt_char("operator = not allowed in bit_row");
		abort_func(0, top_ex_bad_eq_op);
		return (*this);
	}
	
public:
	char* 	ex_nm;
	long	ex_id;
	char* 	ex_stk;
	
	top_exception(char* descr = as_pt_char("undefined top exception"), long the_id = 0);
	
	~top_exception(){
	}
		
	void release_strings();
	
	char* get_str(){
		return ex_nm;
	}

	char* get_stk(){
		return ex_stk;
	}
};

#endif // TOP_EXCEPTION_H


