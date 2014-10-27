

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

solver.h

the solver wrapper.

--------------------------------------------------------------*/

#ifndef SOLVER_H
#define SOLVER_H

#include "instance_info.h"
#include "skeleton.h"

#define SOLVER_CK(prm) 	DBG_BJ_LIB_CK(prm)

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
	
	solver(){
		init_solver();
	}

	~solver(){
	}
	
	void	init_solver(){
	}
	
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


