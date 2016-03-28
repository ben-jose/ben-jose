

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

Copyright (C) 2011, 2014-2015. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
email: joseluisquirogabeltran@gmail.com

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

ben_jose.cpp  

ben_jose interface impl.

--------------------------------------------------------------*/

#include <cstdio>

#include "ben_jose.h"
#include "brain.h"
#include "solver.h"
#include "file_funcs.h"
#include "dbg_prt.h"

char*	solver::CL_NAME = as_pt_char("{solver}");

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
	DBG(
		ch_string rn_pth = path_get_running_path();
	);
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

	dbg_init_dbg_conf(the_slvr.slv_dbg_conf_info);

	bool wrt_proofs = nw_slv->slv_prms.sp_write_proofs;
	MARK_USED(wrt_proofs);
	DBG_PRT_SLV(nw_slv, bj_out, DBG_ALL_LVS, wrt_proofs, os << "WRITING_PROOFS");
	
	DBG_PRT_SLV(nw_slv, bj_out, 1, true, os << "KEEP_LEARNED");
	DBG_PRT_SLV(nw_slv, bj_out, 2, true, os << "ONLY_DEDUC");
	DBG_PRT_SLV(nw_slv, bj_out, 3, true, os << "VERIF_WRITE");
	DBG_PRT_SLV(nw_slv, bj_out, 4, true, os << "NEVER_FIND");
	DBG_PRT_SLV(nw_slv, bj_out, 5, true, os << "NEVER_WRITE");
	DBG_PRT_SLV(nw_slv, bj_out, 6, true, os << "ONLY_READ");

	BRAIN_CK((bj_out << "doing CKs (plain BRN_CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing BRN_CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing BRN_CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing BRN_CK_2s" << bj_eol) && true);
	return bjs;
}

void 		bj_solver_release(bj_solver_t bjs){
	if(bjs == NULL){
		return;
	}
	solver* the_slvr = (solver*)bjs;
	if(the_slvr == NULL_PT){
		return;
	}
	
	SOLVER_CK(the_slvr->get_cls_name() == solver::CL_NAME);
	SOLVER_REL_CK(the_slvr->get_cls_name() == solver::CL_NAME);
	
	BRAIN_CK(
		(bj_out << "BATCH_DBG=\n" << "MAX_lv=" << the_slvr->slv_dbg2.dbg_max_lv
		<< " MAX_wrt_num_subnmp=" << the_slvr->slv_dbg2.dbg_max_wrt_num_subnmp
		<< " MAX_fnd_num_subnmp=" << the_slvr->slv_dbg2.dbg_max_fnd_num_subnmp
		<< bj_eol) && 
		true
	);

	bool wrt_proofs = the_slvr->slv_prms.sp_write_proofs;
	MARK_USED(wrt_proofs);
	DBG_PRT_SLV(the_slvr, bj_out, DBG_ALL_LVS, wrt_proofs, os << "WRITING_PROOFS");
	
	DBG_PRT_SLV(the_slvr, bj_out, 1, true, os << "KEEP_LEARNED");
	DBG_PRT_SLV(the_slvr, bj_out, 2, true, os << "ONLY_DEDUC");
	DBG_PRT_SLV(the_slvr, bj_out, 3, true, os << "VERIF_WRITE");
	DBG_PRT_SLV(the_slvr, bj_out, 4, true, os << "NEVER_FIND");
	DBG_PRT_SLV(the_slvr, bj_out, 5, true, os << "NEVER_WRITE");
	DBG_PRT_SLV(the_slvr, bj_out, 6, true, os << "ONLY_READ");
	
	BRAIN_CK((bj_out << "doing CKs (plain BRN_CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing BRN_CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing BRN_CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing BRN_CK_2s" << bj_eol) && true);

	DBG_PRT_SLV(the_slvr, bj_out, -1, (the_slvr != NULL_PT), 
		os << the_slvr->slv_dbg2.dbg_avg_num_filled << "\n";
		os << the_slvr->slv_dbg2.dbg_avg_neu_sz << "\n";
	);
	
	solver::release_solver(the_slvr);
}

void
bj_set_param_char(bj_solver_t bjs, bj_param_t prm, char val){
	if(bjs == NULL){
		return;
	}
	solver& the_slvr = *((solver*)bjs);
	switch(prm){
		case bjp_as_release:
			if(val == 0){
				the_slvr.slv_prms.sp_as_release = false;
			} else {
				the_slvr.slv_prms.sp_as_release = true;
			}
			break;
		case bjp_write_proofs:
			if(val == 0){
				the_slvr.slv_prms.sp_write_proofs = false;
			} else {
				the_slvr.slv_prms.sp_write_proofs = true;
			}
			break;
		default:
			break;
	}
	DBG(bj_out << "SET_PARAM " << prm << " TO " << (int)val << "\n");
}

char
bj_get_param_char(bj_solver_t bjs, bj_param_t prm){
	if(bjs == NULL){
		return 0;
	}
	solver& the_slvr = *((solver*)bjs);
	char rr = 0;
	switch(prm){
		case bjp_write_proofs:
			if(the_slvr.slv_prms.sp_write_proofs){
				rr = 1;
			} else {
				rr = 0;
			}
			break;
		default:
			break;
	}
	return rr;
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
	bj_satisf_val_t res = the_brain.solve_instance(true /* load_it */);
	
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

const char* bj_get_last_proof_path(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	ch_string pth = the_slvr.slv_inst.ist_last_proof_path;
	if(pth == INVALID_PATH){
		return NULL;
	}
	const char* pf_pth = pth.c_str();
	return pf_pth;
}

const char* bj_get_error_stack_str(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	const char* assrt_str = the_slvr.slv_inst.ist_err_assrt_str.c_str();
	return assrt_str;
}

const char* bj_get_error_assert_str(bj_solver_t bjs){
	if(bjs == NULL){
		return NULL;
	}
	solver& the_slvr = *((solver*)bjs);
	const char* stck_str = the_slvr.slv_inst.ist_err_stack_str.c_str();
	return stck_str;
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

const char* bj_error_str(bj_error_t bje){
	const char* e_str = "Invalid_bj_error_string !!!";
	switch(bje){
		case bje_no_error:
			e_str = "bje_no_error";
			break;
		case bje_internal:
			e_str = "bje_internal";
			break;
		case bje_internal_ex:
			e_str = "bje_internal_ex";
			break;
		case bje_memout:
			e_str = "bje_memout";
			break;
		case bje_timeout:
			e_str = "bje_timeout";
			break;
		case bje_instance_cannot_load:
			e_str = "bje_instance_cannot_load";
			break;
		case bje_file_cannot_open:
			e_str = "bje_file_cannot_open";
			break;
		case bje_file_corrupted:
			e_str = "bje_file_corrupted";
			break;
		case bje_file_too_big:
			e_str = "bje_file_too_big";
			break;
		case bje_path_too_long:
			e_str = "bje_path_too_long";
			break;
		case bje_invalid_root_directory:
			e_str = "bje_invalid_root_directory";
			break;
		case bje_parse_bad_number:
			e_str = "bje_parse_bad_number";
			break;
		case bje_dimacs_no_cnf_declaration:
			e_str = "bje_dimacs_no_cnf_declaration";
			break;
		case bje_dimacs_bad_cls_num:
			e_str = "bje_dimacs_bad_cls_num";
			break;
		case bje_dimacs_format_err:
			e_str = "bje_dimacs_format_err";
			break;
		case bje_dimacs_zero_vars:
			e_str = "bje_dimacs_zero_vars";
			break;
		case bje_dimacs_zero_clauses:
			e_str = "bje_dimacs_zero_clauses";
			break;
		case bje_dimacs_bad_literal:
			e_str = "bje_dimacs_bad_literal";
			break;
		case bje_dimacs_clause_too_long:
			e_str = "bje_dimacs_clause_too_long";
			break;
	}
	return e_str;
}
