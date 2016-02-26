

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

class json_dat;

ch_string 	proof_get_unirons_path(skeleton_glb& skg);
void		proof_create_final_unirons_path(skeleton_glb& skg, ch_string end_dir_pth);

ch_string proof_add_paths(ch_string pth1, ch_string pth2);
ch_string proof_get_nmp_proof_path(neuromap& the_nmp);
ch_string proof_get_tk_dir_path(ch_string pth_pref, ticket& pf_tk);
ch_string proof_get_tk_file_name(ticket& pf_tk);
ch_string proof_get_tk_html_file_name(ticket& pf_tk);

void proof_do_res_step(brain& brn, row_quanton_t& curr_res, prop_signal& curr_ps);

long proof_get_trace_idx_of(deduction& dct, long to_wrt_idx);
void proof_write_all_json_files_for(deduction& dct);
void proof_write_json_file_for(deduction& dct, long to_wrt_idx, long prv_wrt_idx);
void proof_append_ps(brain& brn, row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
			ch_string& pth_pref, ticket& pf_tk, row<ch_string>& all_to_move);
void proof_append_uniron(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
			ch_string& pth_pref, ticket& pf_tk, row<ch_string>& all_to_move);
void proof_append_neu_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas);
void proof_append_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas);
void proof_append_neu(brain& brn, row<char>& json_str, neuron* neu, 
			ch_string& pth_pref, ticket& pf_tk, row<ch_string>& all_to_move);
void proof_append_qua(row<char>& json_str, quanton* qua);

//void proof_write_html_file_for(ch_string end_dir_pth, deduction& dct, ticket& pf_tk);

void proof_write_proof_json_file_for(neuromap& nmp, ch_string os_end_jsn_pth, ticket& pf_tk);

void proof_move_all_to_mv(deduction& dct, ch_string& pf_dir_pth, row<ch_string>& all_to_move);

void proof_write_top_html_file(ch_string the_pth);

#endif		// PROOF_FUNCS_H


