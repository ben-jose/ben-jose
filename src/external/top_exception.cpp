

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

top_exception.cpp  

top_exception impl.

--------------------------------------------------------------*/

#include <cstdlib>

#include "stack_trace.h"
#include "top_exception.h"

#define MAX_STR_SZ 30000

// the idea is not to depend on ch_string. except with STACK_STR.

long
c_str_sz(const char *src){
	if(src == NULL){
		return -1;
	}
	long aa = 0;
	for(aa = 0; (aa < MAX_STR_SZ) && (src[aa] != '\0'); aa++){}
	if(aa == MAX_STR_SZ){
		return -1;
	}
	return aa;
}

char*
malloc_copy_c_str(const char* src)
{
	if(src == NULL){
		return NULL;
	}
	long src_sz = c_str_sz(src);
	if(src_sz == -1){
		return NULL;
	}
	char* dest = (char*)malloc(sizeof(char) * (src_sz + 1));
	
	long aa;
	for (aa = 0; aa < src_sz; aa++){
		dest[aa] = src[aa];
	}
	dest[src_sz] = '\0';

	return dest;
}

top_exception::top_exception(char* descr, long the_id){
	ex_nm = malloc_copy_c_str(descr);
	ex_id = 0;
	const char* val_stk = STACK_STR.c_str();
	ex_stk = malloc_copy_c_str(val_stk);
}

void
top_exception::release_strings(){
	if(ex_nm != NULL){
		free(ex_nm);
	}
	if(ex_stk != NULL){
		free(ex_stk);
	}
}

