

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

central.cpp  

funcs that implement top level funcs.

--------------------------------------------------------------*/

#include "support.h"
#include "dimacs.h"
#include "brain.h"

//============================================================
// code for support


void
print_op_cnf(){
	//bj_ostream& os = bj_out;
}

void
call_solve_instance(){
	skeleton_glb& the_skl = GLB().gg_skeleton;
	instance_info& inst_info = GLB().get_curr_inst();
	
	brain the_brain(the_skl, inst_info);
	the_brain.solve_instance();

}

void
do_cnf_file()
{
	skeleton_glb& the_skl = GLB().gg_skeleton;
	instance_info& inst_info = GLB().get_curr_inst();

	bj_ostr_stream msg_err;
	try{
		
		brain the_brain(the_skl, inst_info);
		the_brain.solve_instance();
		GLB().count_instance(inst_info);
		
	} catch (top_exception& ex1){
		ch_string ex_msg = ex1.get_str();
		DBG(
			ch_string ex_stk = ex1.get_stk();
			ex_msg += "\n" + ex_stk;
		)
		ex1.release_strings();
		
		DBG(
			bj_err << ex_msg << bj_eol;
			abort_func(0);
		)
	}
	catch (...) {
		bj_out << "INTERNAL ERROR !!! (call_and_handle_exceptions)" << bj_eol;
		bj_out << STACK_STR << bj_eol;
		bj_out.flush();
		abort_func(0);
	}
}


