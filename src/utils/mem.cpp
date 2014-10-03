

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

#include <fstream>
#include <cassert>
#include <map>

#include "mem.h"
#include "bj_stream.h"
#include "ch_string.h"
#include "stack_trace.h"

typedef std::map<long, ch_string> dbg_ptdir_t;

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
	std::cout << "DBG_MEM_PTDIR=[" << bj_eol;
	MEM_CK(sizeof(long) == sizeof(void*));
	for(dbg_ptdir_t::iterator aa = DBG_MEM_PTDIR.begin(); aa != DBG_MEM_PTDIR.end(); aa++){
		long kk = (*aa).first;
		void* pt = (void*)kk;
		ch_string sta_str = (*aa).second;
		std::cout << "pt=" << pt << " stack=" << sta_str << bj_eol;
	}
	std::cout << "]" << bj_eol;
}

bool 
call_assert(bool vv_ck, const char* file, int line, const char* ck_str){
	if(! vv_ck){
		std::cout << "ASSERT '" << ck_str << "' FAILED" << bj_eol;
		std::cout << get_stack_trace(file, line) << bj_eol;
	}
	assert(vv_ck);
	return vv_ck;
}

void abort_func(long val, const char* msg){
	std::cerr << bj_eol << "ABORTING! " << msg << bj_eol; 
	std::cerr << "Type ENTER.\n";
	getchar();
	exit(val);
}

