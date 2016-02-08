

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

proof.h

proof writing function declarations.

--------------------------------------------------------------*/

#ifndef PROOF_FUNCS_H
#define PROOF_FUNCS_H

#include "brain.h"

#define SHOW_PROOF_LIB_DIR "show_proof_js_lib"

ch_string proof_add_paths(ch_string pth1, ch_string pth2);
ch_string proof_get_nmp_proof_path(neuromap& the_nmp);
ch_string proof_get_tk_dir_path(ch_string pth_pref, ticket& pf_tk);
ch_string proof_get_tk_file_name(ticket& pf_tk);

long proof_get_trace_idx_of(deduction& dct, long to_wrt_idx);
void proof_write_all_json_files_for(deduction& dct);
void proof_write_json_file_for(deduction& dct, long to_wrt_idx, long prv_wrt_idx);
void proof_append_ps(row<char>& json_str, prop_signal& the_sig, ch_string& pth_pref, 
			row<ch_string>& all_to_move);
void proof_append_lits(row<char>& json_str, row_quanton_t& all_quas);
void proof_append_neu(row<char>& json_str, neuron* neu, ch_string& pth_pref, 
			row<ch_string>& all_to_move);
void proof_append_qua(row<char>& json_str, quanton* qua);

#endif		// PROOF_FUNCS_H


