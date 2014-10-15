

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
	the_brain.load_it();
	the_brain.solve_it();

	GLB().count_instance(inst_info);
}

void
do_cnf_file()
{
	instance_info& the_ans = GLB().get_curr_inst();
	MARK_USED(the_ans);

	DBG_PRT(0, os << "STARTING. batch_count=" 
		<< GLB().batch_consec << " of " 
		<< GLB().batch_num_files);

	ch_string f_nam = the_ans.get_f_nam();
	SUPPORT_CK(f_nam.size() > 0);
	DBG_PRT(0, os << "FILE=" << f_nam << bj_eol);

	call_and_handle_exceptions(call_solve_instance);

	DBG_PRT(0, os << "FINISHING. batch_count="
		<< GLB().batch_consec << " of " 
		<< GLB().batch_num_files << bj_eol;
		the_ans.print_headers(os);
		os << bj_eol << the_ans;
	);
}



