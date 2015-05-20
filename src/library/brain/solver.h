

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

solver.h

the solver wrapper.

--------------------------------------------------------------*/

#ifndef SOLVER_H
#define SOLVER_H

#include "instance_info.h"
#include "skeleton.h"
#include "ben_jose.h"

#define SOLVER_CK(prm) 	DBG_BJ_LIB_CK(prm)

DBG(
	class dbg_inst_info;
	class dbg_slvr_info;
)

//=================================================================
// dbg_slvr_info

#ifdef FULL_DEBUG

class dbg_slvr_info {
public:
	long	dbg_max_lv;
	long	dbg_max_wrt_num_subnmp;
	long	dbg_max_fnd_num_subnmp;
	
	ch_string	dbg_html_out_path;
	ch_string	dbg_cy_js_path;
	
	dbg_slvr_info(){
		init_dbg_slvr_info();
	}

	void	init_dbg_slvr_info(){
		dbg_max_lv = 0;
		dbg_max_wrt_num_subnmp = 0;
		dbg_max_fnd_num_subnmp = 0;
		
		dbg_html_out_path = ".";
		dbg_cy_js_path = 
		"/home/jose/devel/ben-jose/src/programs/tests/drw_cnf/drw_grph_files/";
	}
};

#endif

//=================================================================
// solver

class solver {
private:
	solver&  operator = (solver& other){
		throw instance_exception(inx_bad_eq_op);
	}

	solver(solver& other){ 
		throw instance_exception(inx_bad_creat);
	}
	
public:
	instance_info	slv_inst;
	skeleton_glb	slv_skl;
	DBG(bj_dbg_t		slv_dbg;)
	DBG(dbg_slvr_info 	slv_dbg2;)
	
	solver(){
		init_solver();
	}

	~solver(){
	}
	
	void	init_solver(){
		DBG(init_debug();)
	}
	
	DBG(
		void	init_debug(){
			slv_dbg.W = 1;
			slv_dbg.F = 0;
		}
		
		void	update_dbg2(dbg_inst_info& ist_info);
	);
	
	static
	solver* create_solver(){
		solver* lv = tpl_malloc<solver>();
		new (lv) solver();
		return lv;
	}

	static
	void release_solver(solver* lv){
		SOLVER_CK(lv != NULL_PT);
		lv->~solver();
		tpl_free<solver>(lv);
	}
	
};

#endif		// SOLVER_H


