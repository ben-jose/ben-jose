

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

print_macros.h

macros to declare and define print operators.

--------------------------------------------------------------*/

#ifndef PRINT_MACROS_H
#define PRINT_MACROS_H



//=================================================================
// printing declarations

#define DECLARE_PRINT_FUNCS(obj_t) \
std::ostream& operator << (std::ostream& os, obj_t& obj1); \
std::ostream& operator << (std::ostream& os, obj_t* obj1); \
\

// end_of_define

#define DEFINE_PRINT_FUNCS(obj_t) \
inline \
std::ostream& operator << (std::ostream& os, obj_t& obj1){ \
	obj1.print_##obj_t(os); \
	os.flush(); \
	return os; \
} \
\
inline \
std::ostream& operator << (std::ostream& os, obj_t* obj1){ \
	if(obj1 == NULL_PT){ \
		os << "NULL_" << #obj_t; \
	} else { \
		obj1->print_##obj_t(os, true); \
	} \
	os.flush(); \
	return os; \
} \
\

// end_of_define


#endif		// PRINT_MACROS_H


