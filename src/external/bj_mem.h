

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

bj_mem.h

Declaration of mem trace funcs and other.

--------------------------------------------------------------*/

#ifndef JLQ_MEM_H
#define JLQ_MEM_H

#include <cstring>
#include <cstdlib>

#include "platform.h"
#include "top_exception.h"
#include "bj_stream.h"


/*
include "stack_trace.h"
define DBG_TPL_ALLOC() \
	DBG_COND_COMM(true, os << STACK_STR << "the_size=" << the_size << bj_eol)

end_of_def
*/

#define DBG_TPL_ALLOC() /**/
	
#define NULL_PT		NULL

//define MEM_PT_DIR(prm)	prm
#define MEM_PT_DIR(prm)	;

template <bool> struct ILLEGAL_USE_OF_OBJECT;
template <> struct ILLEGAL_USE_OF_OBJECT<true>{};
#define OBJECT_COPY_ERROR ILLEGAL_USE_OF_OBJECT<false>()

extern 	bool	dbg_keeping_ptdir;

void	dbg_add_to_ptdir(void* pt_val);
void	dbg_del_from_ptdir(void* pt_val);
void	dbg_print_ptdir();

void abort_func(long val, const char* msg = as_pt_char("<msg>"));

bool 
call_assert(bool vv_ck, const char* file, int line, 
			const char* ck_str, const char* msg = NULL_PT);


inline
void* bj_memcpy(void *dest, const void *src, size_t n){
	return memcpy(dest, src, n);
}

inline
void* bj_memset(void *s, int c, size_t n){
	return memset(s, c, n);
}

#define glb_assert(vv) call_assert(vv, as_pt_char(__FILE__), __LINE__, as_pt_char(#vv))

#define glb_assert_2(vv, ostmsg) \
	call_assert(vv, as_pt_char(__FILE__), __LINE__, as_pt_char(#vv), (ostmsg))

#ifdef FULL_DEBUG
#define DBG(prm) prm
#else
#define DBG(prm) /**/ \

// end_of_def
#endif

#define DBG_MARK_USED(X)  DBG(MARK_USED(X))

#define	DBG_COND_COMM(cond, comm)	\
	DBG( \
		if(cond){ \
			bj_ostream& os = bj_dbg; \
			comm; \
			os << bj_eol; \
			os.flush(); \
		} \
	) \

//--end_of_def

#define DBG_CK(prm)	   	DBG(glb_assert(prm))

#define DBG_CK_2(prm, comms1) \
	DBG_COND_COMM((! (prm)), \
		comms1; \
		glb_assert(prm); \
	) \
	
//--end_of_def


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

enum dbg_call_id { 
	dbg_call_1 = 201,
	dbg_call_2,
	dbg_call_3,
	dbg_call_4,
	dbg_call_5,
	dbg_call_6,
	dbg_call_7,
	dbg_call_8,
	dbg_call_9,
	dbg_call_10,
};


typedef long			error_code_t;
typedef unsigned long		mem_size;
typedef char			t_1byte;
typedef unsigned int		t_4byte;
typedef t_4byte			t_dword;

#define MAX_UTYPE(type)		((type)(-1))

#define MAX_MEM_SZ		MAX_UTYPE(mem_size)

//======================================================================
// mem_exception

class mem_exception : public top_exception {
public:
	mem_exception(char* descr = as_pt_char("undefined mem exception"), long the_id = 0) :
		top_exception(descr, the_id)
	{}
};

//======================================================================
// glb_mem_data


class glb_mem_data;

extern glb_mem_data* glb_pt_mem_stat;
extern glb_mem_data MEM_STATS;

typedef void (*memout_func_t)();

class glb_mem_data {
public:

	mem_size 		num_bytes_in_use;
	mem_size 		num_bytes_available;
	memout_func_t	set_memout_func;

	glb_mem_data(){
		num_bytes_in_use = 0;
		num_bytes_available = 0;
		set_memout_func = NULL_PT;
	}

	~glb_mem_data(){
	}
};

#define MEM_CK(prm) DBG(if(glb_pt_mem_stat != NULL){DBG_CK(prm);})

inline
void
mem_start_stats(){
	if(glb_pt_mem_stat != NULL){
		abort_func(0);
	}
	glb_pt_mem_stat = &MEM_STATS;
	MEM_CK(glb_pt_mem_stat->num_bytes_in_use == 0);
}

inline
void
mem_finish_stats(){
	if(glb_pt_mem_stat == NULL){
		abort_func(0);
	}
	MEM_CK(glb_pt_mem_stat->num_bytes_in_use == 0);
	glb_pt_mem_stat = NULL_PT;
}

inline
mem_size
mem_get_num_by_in_use(){
	if(glb_pt_mem_stat == NULL_PT){
		return 0;
	}
	return glb_pt_mem_stat->num_bytes_in_use;
}

inline
void
mem_inc_num_by_in_use(mem_size val){
	if(glb_pt_mem_stat == NULL_PT){
		return;
	}
	glb_pt_mem_stat->num_bytes_in_use += val;
}

inline
void
mem_dec_num_by_in_use(mem_size val){
	if(glb_pt_mem_stat == NULL_PT){
		return;
	}
	glb_pt_mem_stat->num_bytes_in_use -= val;
}

inline
mem_size
mem_get_num_by_available(){
	if(glb_pt_mem_stat == NULL_PT){
		return 0;
	}
	return glb_pt_mem_stat->num_bytes_available;
}

inline
void
mem_set_num_by_available(mem_size val){
	if(glb_pt_mem_stat == NULL_PT){
		return;
	}
	glb_pt_mem_stat->num_bytes_available = val;
}

inline
memout_func_t
mem_get_memout_fn(){
	if(glb_pt_mem_stat == NULL_PT){
		return NULL_PT;
	}
	return glb_pt_mem_stat->set_memout_func;
}

inline
void
mem_set_memout_fn(memout_func_t ff){
	if(glb_pt_mem_stat == NULL_PT){
		return;
	}
	glb_pt_mem_stat->set_memout_func = ff;
}



//======================================================================
// 'mem_alloc()'-style memory allocation -- never returns NULL_PT; aborts instead:

template<class obj_t> static inline obj_t* 
tpl_malloc(size_t the_size = 1){
	DBG_TPL_ALLOC();
	mem_size mem_sz = the_size * sizeof(obj_t);
	MEM_CTRL(
		MEM_CK((MAX_MEM_SZ - mem_sz) > mem_get_num_by_in_use());
		mem_inc_num_by_in_use(mem_sz);

		if(	(mem_get_num_by_available() > 0) && 
			(mem_get_num_by_in_use() > mem_get_num_by_available()) )
		{
			memout_func_t mof = mem_get_memout_fn();
			if(mof != NULL_PT){
				(*mof)();
			} else {
				char* mem_out_in_mem_alloc = as_pt_char("Memory exhausted in tpl-mem-alloc");
				DBG_THROW_CK(mem_out_in_mem_alloc != mem_out_in_mem_alloc);
				throw mem_exception(mem_out_in_mem_alloc);
				abort_func(0, mem_out_in_mem_alloc);
			}
		}
	);

	obj_t*   tmp = (obj_t*)malloc(mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* mem_out_in_mem_alloc_2 = as_pt_char("Memory exhausted in tpl-mem-alloc case 2");
		DBG_THROW_CK(mem_out_in_mem_alloc_2 != mem_out_in_mem_alloc_2);
		throw mem_exception(mem_out_in_mem_alloc_2);
		abort_func(0, mem_out_in_mem_alloc_2);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));
	return tmp; 
}

template<class obj_t> static inline obj_t* 
tpl_secure_realloc(obj_t* ptr, size_t old_size, size_t the_size){
	DBG_TPL_ALLOC();
	MEM_CK(the_size > old_size);

	mem_size mem_sz = the_size * sizeof(obj_t);
	obj_t*   tmp = (obj_t*)malloc(mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* memout_in_sec_re_alloc = as_pt_char("Memory exhausted in tpl-sec-re-alloc");
		DBG_THROW_CK(memout_in_sec_re_alloc != memout_in_sec_re_alloc);
		throw mem_exception(memout_in_sec_re_alloc);
		abort_func(0, memout_in_sec_re_alloc);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));

	if(ptr != NULL_PT){
		mem_size old_mem_sz = old_size * sizeof(obj_t);
		bj_memcpy(tmp, ptr, old_mem_sz);
		bj_memset(ptr, 0, old_mem_sz);
		free(ptr);
		MEM_PT_DIR(dbg_del_from_ptdir(ptr));
	}
	return tmp; 
}

template<class obj_t> static inline obj_t* 
tpl_realloc(obj_t* ptr, size_t old_size, size_t the_size){
	DBG_TPL_ALLOC();
	mem_size mem_sz = the_size * sizeof(obj_t);
	MEM_CTRL(
		mem_size old_mem_sz = old_size * sizeof(obj_t);
		MEM_CK(mem_get_num_by_in_use() >= old_mem_sz);
		mem_dec_num_by_in_use(old_mem_sz);
		MEM_CK((MAX_MEM_SZ - mem_sz) > mem_get_num_by_in_use());
		mem_inc_num_by_in_use(mem_sz);

		if(	(mem_get_num_by_available() > 0) && 
			(mem_get_num_by_in_use() > mem_get_num_by_available()) )
		{
			memout_func_t mof = mem_get_memout_fn();
			if(mof != NULL_PT){
				(*mof)();
			} else {
				char* mem_out_in_re_alloc = as_pt_char("Memory exhausted in tpl-re-alloc");
				DBG_THROW_CK(mem_out_in_re_alloc != mem_out_in_re_alloc);
				throw mem_exception(mem_out_in_re_alloc);
				abort_func(0, mem_out_in_re_alloc);
			}
		}
	);
	MEM_PT_DIR(dbg_del_from_ptdir(ptr));
	obj_t*   tmp = (obj_t*)realloc((void*)ptr, mem_sz);
	if((tmp == NULL_PT) && (the_size != 0)){
		char* mem_out_in_re_alloc_2 = as_pt_char("Memory exhausted in tpl-re-alloc case 2");
		DBG_THROW_CK(mem_out_in_re_alloc_2 != mem_out_in_re_alloc_2);
		throw mem_exception(mem_out_in_re_alloc_2);
		abort_func(0, mem_out_in_re_alloc_2);
	}
	MEM_PT_DIR(dbg_add_to_ptdir(tmp));
	return tmp; 
}

template<class obj_t> static inline void 
tpl_free(obj_t*& ptr, size_t the_size = 1){
	DBG_TPL_ALLOC();
	if(ptr != NULL_PT){ 
		MEM_SRTY(
			mem_size s_old_mem_sz = the_size * sizeof(obj_t);
		);
		free(ptr); 
		MEM_PT_DIR(dbg_del_from_ptdir(ptr));
		ptr = NULL_PT;
	}
	MEM_CTRL(
		mem_size old_mem_sz = the_size * sizeof(obj_t);
		MEM_CK(mem_get_num_by_in_use() >= old_mem_sz);
		mem_dec_num_by_in_use(old_mem_sz);
	);
}




#endif // JLQ_MEM_H


