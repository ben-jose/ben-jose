

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

mem.h

Declaration of mem trace funcs and other.

--------------------------------------------------------------*/

#ifndef JLQ_MEM_H
#define JLQ_MEM_H

#include <iostream>
#include <cstdlib>

#include "platform.h"
#include "top_exception.h"

//define MEM_PT_DIR(prm)	prm
#define MEM_PT_DIR(prm)	;

extern 	bool	dbg_keeping_ptdir;

void	dbg_add_to_ptdir(void* pt_val);
void	dbg_del_from_ptdir(void* pt_val);
void	dbg_print_ptdir();

void abort_func(long val, const char* msg = as_pt_char("<msg>"));

bool 
call_assert(bool vv_ck, const char* file, int line, const char* ck_str);

#define glb_assert(vv) call_assert(vv, as_pt_char(__FILE__), __LINE__, as_pt_char(#vv))

#define NULL_PT		NULL

#ifdef FULL_DEBUG
#define DBG(prm) prm
#else
#define DBG(prm) /**/ \

// end_of_def
#endif

#define DBG_CK(prm)	   	DBG(glb_assert(prm))
#define DBG_THROW(prm) 		DBG(prm)
//define DBG_THROW(prm) 		;
#define DBG_THROW_CK(prm) 	DBG_CK(prm)
//define DBG_THROW_CK(prm) 	;

#ifdef NO_MEM_CTRL
#define MEM_CTRL(prm) ;
#else
#define MEM_CTRL(prm) prm
#endif

#ifdef SECURE_MEM
#define MEM_SRTY(prm) prm
#else
#define MEM_SRTY(prm) ;
#endif

#define MEM_CK(prm)		DBG_CK(prm)

typedef long			error_code_t;
typedef unsigned long		mem_size;
typedef char			t_1byte;
typedef unsigned int		t_4byte;
typedef t_4byte			t_dword;

#define MAX_UTYPE(type)		((type)(-1))

#define MAX_MEM_SZ		MAX_UTYPE(mem_size)

//======================================================================
// bit_row_exception

class mem_exception : public top_exception {
public:
	mem_exception(char* descr = as_pt_char("undefined mem exception")){
		ex_nm = descr;
		ex_id = 0;
	}
};

//======================================================================
// glb_mem_data

class glb_mem_data;
extern glb_mem_data MEM_STATS;

//define DEFINE_MEM_STATS	glb_mem_data MEM_STATS

class glb_mem_data {
public:
	typedef void (*memout_func_t)();

	mem_size 		num_bytes_in_use;
	mem_size 		num_bytes_available;
	memout_func_t	set_memout_func;
	bool			use_secure_alloc;

	glb_mem_data(){
		num_bytes_in_use = 0;
		num_bytes_available = 0;
		set_memout_func = NULL_PT;
		use_secure_alloc = false;
	}

	~glb_mem_data(){
		MEM_CK(num_bytes_in_use == 0);
	}
};


//======================================================================
// 'malloc()'-style memory allocation -- never returns NULL_PT; aborts instead:

template<class obj_t> static inline obj_t* 
tpl_malloc(size_t the_size = 1){
	mem_size mem_sz = the_size * sizeof(obj_t);
	MEM_CTRL(
		MEM_CK((MAX_MEM_SZ - mem_sz) > MEM_STATS.num_bytes_in_use);
		MEM_STATS.num_bytes_in_use += mem_sz;

		if(	(MEM_STATS.num_bytes_available > 0) && 
			(MEM_STATS.num_bytes_in_use > MEM_STATS.num_bytes_available) )
		{
			if(MEM_STATS.set_memout_func != NULL_PT){
				(*MEM_STATS.set_memout_func)();
			} else {
				char* mem_out_in_malloc = as_pt_char("Memory exhausted in tpl_malloc");
				DBG_THROW_CK(mem_out_in_malloc != mem_out_in_malloc);
				throw mem_exception(mem_out_in_malloc);
				abort_func(0, mem_out_in_malloc);
			}
		}
	);

	obj_t*   tmp = (obj_t*)malloc(mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* mem_out_in_malloc_2 = as_pt_char("Memory exhausted in tpl_malloc case 2");
		DBG_THROW_CK(mem_out_in_malloc_2 != mem_out_in_malloc_2);
		throw mem_exception(mem_out_in_malloc_2);
		abort_func(0, mem_out_in_malloc_2);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));
	return tmp; 
}

template<class obj_t> static inline obj_t* 
tpl_secure_realloc(obj_t* ptr, size_t old_size, size_t the_size){
	MEM_CK(the_size > old_size);

	mem_size mem_sz = the_size * sizeof(obj_t);
	obj_t*   tmp = (obj_t*)malloc(mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* mem_out_in_sec_realloc = as_pt_char("Memory exhausted in tpl_secure_realloc");
		DBG_THROW_CK(mem_out_in_sec_realloc != mem_out_in_sec_realloc);
		throw mem_exception(mem_out_in_sec_realloc);
		abort_func(0, mem_out_in_sec_realloc);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));

	if(ptr != NULL_PT){
		mem_size old_mem_sz = old_size * sizeof(obj_t);
		memcpy(tmp, ptr, old_mem_sz);
		memset(ptr, 0, old_mem_sz);
		free(ptr);
		MEM_PT_DIR(dbg_del_from_ptdir(ptr));
	}
	return tmp; 
}

template<class obj_t> static inline obj_t* 
tpl_realloc(obj_t* ptr, size_t old_size, size_t the_size){
	mem_size mem_sz = the_size * sizeof(obj_t);
	MEM_CTRL(
		mem_size old_mem_sz = old_size * sizeof(obj_t);
		MEM_CK(MEM_STATS.num_bytes_in_use >= old_mem_sz);
		MEM_STATS.num_bytes_in_use -= old_mem_sz;
		MEM_CK((MAX_MEM_SZ - mem_sz) > MEM_STATS.num_bytes_in_use);
		MEM_STATS.num_bytes_in_use += mem_sz;

		if(	(MEM_STATS.num_bytes_available > 0) && 
			(MEM_STATS.num_bytes_in_use > MEM_STATS.num_bytes_available) )
		{
			if(MEM_STATS.set_memout_func != NULL_PT){
				(*MEM_STATS.set_memout_func)();
			} else {
				char* mem_out_in_realloc = as_pt_char("Memory exhausted in tpl_realloc");
				DBG_THROW_CK(mem_out_in_realloc != mem_out_in_realloc);
				throw mem_exception(mem_out_in_realloc);
				abort_func(0, mem_out_in_realloc);
			}
		}
	);
	MEM_SRTY(
		if(MEM_STATS.use_secure_alloc){
			return tpl_secure_realloc(ptr, old_size, the_size); 
		}
	);
	MEM_PT_DIR(dbg_del_from_ptdir(ptr));
	obj_t*   tmp = (obj_t*)realloc((void*)ptr, mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* mem_out_in_realloc_2 = as_pt_char("Memory exhausted in tpl_realloc case 2");
		DBG_THROW_CK(mem_out_in_realloc_2 != mem_out_in_realloc_2);
		throw mem_exception(mem_out_in_realloc_2);
		abort_func(0, mem_out_in_realloc_2);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));
	return tmp; 
}

template<class obj_t> static inline void 
tpl_free(obj_t*& ptr, size_t the_size = 1){
	if(ptr != NULL_PT){ 
		MEM_SRTY(
			mem_size s_old_mem_sz = the_size * sizeof(obj_t);
			if(MEM_STATS.use_secure_alloc){
				memset(ptr, 0, s_old_mem_sz);
			}
		);
		free(ptr); 
		MEM_PT_DIR(dbg_del_from_ptdir(ptr));
		ptr = NULL_PT;
	}
	MEM_CTRL(
		mem_size old_mem_sz = the_size * sizeof(obj_t);
		MEM_CK(MEM_STATS.num_bytes_in_use >= old_mem_sz);
		MEM_STATS.num_bytes_in_use -= old_mem_sz;
	);
}




#endif // JLQ_MEM_H


