

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

void 		bj_init_output(bj_output_t* the_out){
	if(the_out == NULL){
		return;
	}
	instance_info::init_output(*the_out);
}

bj_solver_t bj_solver_create(const char* bjs_dir_path){
	if(bjs_dir_path == NULL){
		return NULL;
	}
	ch_string root_pth = bjs_dir_path;
	bool is_no_pth = (root_pth.empty() || (root_pth == ""));
	NOT_DBG(if(is_no_pth){ return NULL; })
	
	solver* nw_slv = solver::create_solver();
	if(nw_slv == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)nw_slv);
	the_slvr.slv_skl.kg_root_path = root_pth;	

	DBG(if(is_no_pth){ the_slvr.slv_skl.kg_keep_skeleton = false; })
	the_slvr.slv_skl.init_paths();
	
	bj_solver_t bjs = (bj_solver_t)nw_slv;

	WF_DBG(bj_out << "NOT FIND (JUST AS DEFAULT_DBG)" << bj_eol);
	DBG_CHECK_SAVED(
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
	);
	BRAIN_CK((bj_out << "doing CKs (plain CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing CK_2s" << bj_eol) && true);
	return bjs;
}

void 		bj_solver_release(bj_solver_t bjs){
	if(bjs == NULL){
		return;
	}
	solver* the_slvr = (solver*)bjs;
	solver::release_solver(the_slvr);
	
	BRAIN_CK((bj_out << "doing CKs (plain CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing CK_2s" << bj_eol) && true);
	DBG_CHECK_SAVED(
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
	);
	WF_DBG(bj_out << bj_eol << "NOT FIND (JUST AS DEFAULT_DBG)" << bj_eol);
}

const char* bj_get_path(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	const char* pth = the_slvr.slv_skl.kg_root_path.c_str();
	return pth;
}

bj_satisf_val_t 	bj_solve_file(bj_solver_t bjs, const char* f_path){
	if(bjs == NULL){ return bjr_error; }
	if(f_path == NULL){ return bjr_error; }
	
	solver& the_slvr = *((solver*)bjs);
	instance_info& inst = the_slvr.slv_inst;
	
	long nxt_id = inst.ist_id;
	if(nxt_id >= 0){ nxt_id++; }
	//DBG(bj_out << bj_eol << "PREV_ID=" << inst.ist_id << bj_eol);
	
	inst.init_instance_info(false, false);
	inst.ist_file_path = f_path;
	inst.ist_id = nxt_id;
	//DBG(bj_out << bj_eol << "SOLVING_INSTANCE=" << inst.ist_id << bj_eol );
	
	brain the_brain(the_slvr);
	bj_satisf_val_t res = the_brain.solve_instance();
	
	return res;
}

bj_satisf_val_t 	bj_solve_data(bj_solver_t bjs, long dat_sz, char* dat){
	bj_satisf_val_t res = bjr_error;
	return res;
}

bj_satisf_val_t 	bj_solve_literals(bj_solver_t bjs, long num_vars, long num_cls, 
						  long lits_sz, long* lits)
{
	bj_satisf_val_t res = bjr_error;
	return res;
}

bj_satisf_val_t 	bj_get_result(bj_solver_t bjs){
	if(bjs == NULL){
		return bjr_error;
	}
	solver& the_slvr = *((solver*)bjs);
	return the_slvr.slv_inst.ist_out.bjo_result;
}

bj_output_t 		bj_get_output(bj_solver_t bjs){
	if(bjs == NULL){
		bj_output_t oo;
		instance_info::init_output(oo);
		return oo;
	}
	solver& the_slvr = *((solver*)bjs);
	return the_slvr.slv_inst.ist_out;
}

const long* bj_get_assig(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	row<long>& assig = the_slvr.slv_inst.ist_assig;
	if(assig.is_empty()){
		return NULL;
	}
	if(assig.last() != 0){
		return NULL;
	}
	const long* assig_arr = assig.get_c_array();
	return assig_arr;
}

void				bj_restart(bj_solver_t bjs){
	if(bjs == NULL){
		return;
	}
	solver& the_slvr = *((solver*)bjs);
	instance_info& inst = the_slvr.slv_inst;
	
	inst.init_instance_info(true, true);
	inst.ist_id = 0;
}

int 	bj_update(bj_solver_t dest, bj_solver_t src){
	return 0;
}

void		bj_print_paths(bj_solver_t bjs){
	if(bjs == NULL){ return; }
	solver& the_slvr = *((solver*)bjs);
	the_slvr.slv_skl.print_paths(bj_out);
}

bj_dbg_t*	bj_debug(bj_solver_t bjs){
	bj_dbg_t* dd = NULL;
	DBG(
		if(bjs == NULL){ return NULL; }
		solver& the_slvr = *((solver*)bjs);
		dd = &(the_slvr.slv_dbg);
	)
	return dd;
}

