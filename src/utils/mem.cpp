

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

mem.cpp  

mem trace funcs and other.

--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <cassert>
#include <cxxabi.h>
#include <sstream>
#include <execinfo.h>
#include <map>

#include "mem.h"

typedef std::map<long, std::string> dbg_ptdir_t;

bool		dbg_keeping_ptdir = false;
dbg_ptdir_t	DBG_MEM_PTDIR;
glb_mem_data 	MEM_STATS;

void
dbg_add_to_ptdir(void* pt_val){
	if(! dbg_keeping_ptdir){ return; }
	long kk = (long)(pt_val);	
	DBG_MEM_PTDIR[kk] = STACK_STR;
}

void
dbg_del_from_ptdir(void* pt_val){
	if(! dbg_keeping_ptdir){ return; }
	long kk = (long)(pt_val);
	DBG_MEM_PTDIR.erase(kk);
}

void
dbg_print_ptdir(){
	std::cout << "DBG_MEM_PTDIR=[" << std::endl;
	MEM_CK(sizeof(long) == sizeof(void*));
	for(dbg_ptdir_t::iterator aa = DBG_MEM_PTDIR.begin(); aa != DBG_MEM_PTDIR.end(); aa++){
		long kk = (*aa).first;
		void* pt = (void*)kk;
		std::string sta_str = (*aa).second;
		std::cout << "pt=" << pt << " stack=" << sta_str << std::endl;
	}
	std::cout << "]" << std::endl;
}

bool 
call_assert(bool vv_ck, const std::string & file, int line, std::string ck_str){
	if(! vv_ck){
		std::cout << "ASSERT '" << ck_str << "' FAILED" << std::endl;
		std::cout << get_stack_trace(file, line) << std::endl;
	}
	assert(vv_ck);
	return vv_ck;
}

void abort_func(long val, std::string msg){
	std::cerr << std::endl << "ABORTING! " << msg << std::endl; 
	std::cerr << "Type ENTER.\n";
	getchar();
	exit(val);
}

bool
print_backtrace( const std::string & file, int line ){
	std::cout << get_stack_trace(file, line) << std::endl;
	return true;
}

/*
std::string 
demangle_cxx_name( const std::string &stack_string ){
	const size_t k_max_nm_len = 4096;
	char function[k_max_nm_len];
	size_t start = stack_string.find( '(' );
	size_t end = stack_string.find( '+' );

	if( std::string::npos == start || std::string::npos == end ){
		return stack_string;
	}

	int status;

	++start; // puts us pass the '('

	size_t n = end - start;
	size_t len;
	std::string mangled = stack_string.substr( start, n );
	char *ret = abi::__cxa_demangle( mangled.c_str(), function, &len, &status );
	MARK_USED(ret);

	if( 0 == status ){ // demanging successful
		return std::string( function );
	}

	// If demangling fails, returned mangled name with some parens
	return mangled + "()";
}
*/

// USE:        std::cout << get_stack_trace( __FILE__, __LINE__ ) << std::endl;

std::string 
get_stack_trace( const std::string & file, int line ){
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


