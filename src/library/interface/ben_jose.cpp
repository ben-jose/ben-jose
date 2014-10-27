

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
#include "solver.h"
#include "dbg_prt.h"


bj_solver_t bj_solver_create(const char* bjs_dir_path){
	if(bjs_dir_path == NULL){
		return NULL;
	}
	solver* nw_slv = solver::create_solver();
	if(nw_slv == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)nw_slv);
	the_slvr.slv_skl.kg_root_path = bjs_dir_path;
	the_slvr.slv_skl.init_paths();
	
	bj_solver_t bjs = (bj_solver_t)nw_slv;
	return bjs;
}

void 		bj_solver_release(bj_solver_t bjs){
	if(bjs == NULL){
		return;
	}
	solver* the_slvr = (solver*)bjs;
	solver::release_solver(the_slvr);
}

int 	bj_update(bj_solver_t dest, bj_solver_t src){
	return 0;
}

const char* bj_get_path(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	const char* pth = the_slvr.slv_skl.kg_root_path.c_str();
	return pth;
}

const long* bj_get_assig(bj_solver_t bjs){
	long* assig_arr = NULL;
	return assig_arr;
}

bj_satisf_val_t 	bj_solve_file(bj_solver_t bjs, const char* f_path){
	if(bjs == NULL){ return k_error; }
	if(f_path == NULL){ return k_error; }
	
	solver& the_slvr = *((solver*)bjs);
	
	the_slvr.slv_inst.init_instance_info(false, false);	
	the_slvr.slv_inst.ist_file_path = f_path;
	
	brain the_brain(the_slvr);
	bj_satisf_val_t res = the_brain.solve_instance();
	
	return res;
}

bj_satisf_val_t 	bj_solve_data(bj_solver_t bjs, long dat_sz, char* dat){
	bj_satisf_val_t res = k_error;
	return res;
}

bj_satisf_val_t 	bj_solve_literals(bj_solver_t bjs, long num_vars, long num_cls, 
						  long lits_sz, long* lits)
{
	bj_satisf_val_t res = k_error;
	return res;
}


