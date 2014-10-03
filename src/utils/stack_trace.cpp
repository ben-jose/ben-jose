

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

stack_trace.cpp  

stack_trace funcs.

--------------------------------------------------------------*/

#include <sstream>
#include <execinfo.h>
#include <cstdlib>

#include "bj_stream.h"
#include "ch_string.h"
#include "stack_trace.h"

bool
print_backtrace( const ch_string & file, int line ){
	std::cout << get_stack_trace(file, line) << bj_eol;
	return true;
}

/*
ch_string 
demangle_cxx_name( const ch_string &stack_string ){
	const size_t k_max_nm_len = 4096;
	char function[k_max_nm_len];
	size_t start = stack_string.find( '(' );
	size_t end = stack_string.find( '+' );

	if( ch_string::npos == start || ch_string::npos == end ){
		return stack_string;
	}

	int status;

	++start; // puts us pass the '('

	size_t n = end - start;
	size_t len;
	ch_string mangled = stack_string.substr( start, n );
	char *ret = abi::__cxa_demangle( mangled.c_str(), function, &len, &status );
	MARK_USED(ret);

	if( 0 == status ){ // demanging successful
		return ch_string( function );
	}

	// If demangling fails, returned mangled name with some parens
	return mangled + "()";
}
*/

// USE:        std::cout << get_stack_trace( __FILE__, __LINE__ ) << bj_eol;

ch_string 
get_stack_trace( const ch_string & file, int line ){
	std::stringstream result;
	result << "Call Stack from " << file << ":" << line << "\n";
	const size_t k_max_depth = 100;
	void *stack_addrs[k_max_depth];
	size_t stack_depth;
	char **stack_strings;

	stack_depth = backtrace( stack_addrs, k_max_depth );
	stack_strings = backtrace_symbols( stack_addrs, stack_depth );
	for( size_t i = 1; i < stack_depth; ++i ) {
		//result << "   " << demangle_cxx_name( stack_strings[i] ) << "\n";
		result << "   " << stack_strings[i] << "\n";
	}
	std::free( stack_strings );

	return result.str();
}


