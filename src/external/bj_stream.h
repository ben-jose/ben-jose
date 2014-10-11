

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

bj_stream.h

Wrapper for ostream class.

--------------------------------------------------------------*/


#ifndef BJ_STREAM_H
#define BJ_STREAM_H

#include <iostream>
#include <sstream>

#define CARRIAGE_RETURN		((char)13)

#define bj_eol std::endl
#define bj_out std::cout
#define bj_err std::cerr
#define bj_dbg std::cout

#define bj_fixed std::fixed

typedef std::ostream bj_ostream;

typedef std::ostringstream bj_ostr_stream;

#endif // BJ_STREAM_H


