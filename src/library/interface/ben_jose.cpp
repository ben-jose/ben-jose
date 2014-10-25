

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

ben_jose.cpp  

ben_jose interface impl.

--------------------------------------------------------------*/

#include <cstdio>

#include "ben_jose.h"
#include "brain.h"
#include "dbg_prt.h"


void	bj_init_input(bj_input_t* in){
	if(in == NULL){
		return;
	}
	in->bji_with_assig = 0;
	
	in->bji_group_id = 0;
	in->bji_id = 0;
	
	in->bji_file_path = NULL;

	in->bji_data_sz = 0;
	in->bji_data = NULL;

	in->bji_num_vars = 0;
	in->bji_num_ccls = 0;

	in->bji_literals_sz = 0;
	in->bji_literals = NULL;
	
	in->bji_timeout = 0.0;
	in->bji_memout = 0.0;
}

void	bj_release_output_assig(bj_output_t* out){
	if(out == NULL){
		return;
	}
	
	tpl_free<long>(out->bjo_final_assig, out->bjo_final_assig_sz);
	
	out->bjo_final_assig = NULL;
	out->bjo_final_assig_sz = 0;
}

bj_uns_db_t bj_unsat_db_open(const char* bdb_dir_path){
	if(bdb_dir_path == NULL){
		return NULL;
	}
	skeleton_glb* nw_skl = skeleton_glb::create_skeleton_glb();
	if(nw_skl == NULL){
		return NULL;
	}
	nw_skl->kg_root_path = bdb_dir_path;
	nw_skl->init_paths();
	
	bj_uns_db_t udb = (bj_uns_db_t)nw_skl;
	return udb;
}

void 		bj_unsat_db_close(bj_uns_db_t bdb){
	if(bdb == NULL){
		return;
	}
	skeleton_glb* the_skl = (skeleton_glb*)bdb;
	skeleton_glb::release_skeleton_glb(the_skl);
}

const char* bj_unsat_db_path(bj_uns_db_t bdb){
	if(bdb == NULL){
		return NULL;
	}
	skeleton_glb* the_skl = (skeleton_glb*)bdb;
	const char* pth = the_skl->kg_root_path.c_str();
	return pth;
}

int 	bj_unsat_db_update(bj_uns_db_t dest, bj_uns_db_t src){
	return 0;
}

int 	bj_solve_file(bj_uns_db_t bdb, const char* f_path, bj_output_t* out){
	if(bdb == NULL){ return -1; }
	if(f_path == NULL){ return -1; }
	if(out == NULL){ return -1; }
	
	instance_info::init_output(*out);
	
	skeleton_glb* the_skl = (skeleton_glb*)bdb;
	
	instance_info inst_info;
	inst_info.ist_file_path = f_path;
	
	brain the_brain(*the_skl, inst_info);
	the_brain.solve_instance();
	
	(*out) = inst_info.ist_out;
	
	instance_info::init_output(inst_info.ist_out);
	return 0;
}

int 	bj_solve_data(bj_uns_db_t bdb, long dat_sz, char* dat, bj_output_t* out){
	return 0;
}

int 	bj_solve_literals(bj_uns_db_t bdb, long num_vars, long num_cls, 
						  long lits_sz, long* lits, bj_output_t* out)
{
	return 0;
}

int 	bj_solve_input(bj_uns_db_t bdb, bj_input_t* in, bj_output_t* out){
	return 0;
}

