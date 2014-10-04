

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

stack_trace.h

func to print a stack trace.

--------------------------------------------------------------*/

#ifndef STACK_TRACE_H
#define STACK_TRACE_H

#include "ch_string.h"

ch_string	get_stack_trace( const ch_string & file, int line );

#define STACK_STR get_stack_trace(__FILE__, __LINE__)


#endif		// STACK_TRACE_H

