

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

class proof_tk;

ch_string 	proof_get_unirons_path(skeleton_glb& skg);
void		proof_create_final_unirons_path(skeleton_glb& skg, ch_string end_dir_pth);

long 		proof_max_end_idx(deduction& dct);

ch_string proof_add_paths(ch_string pth1, ch_string pth2);
ch_string proof_get_nmp_proof_path(neuromap& the_nmp);
ch_string proof_get_tk_dir_path(ch_string pth_pref, proof_tk& pf_tk);
ch_string proof_get_tk_file_name(proof_tk& pf_tk);
ch_string proof_get_tk_html_file_name(proof_tk& pf_tk);

void proof_do_res_step(brain& brn, row_quanton_t& curr_res, prop_signal& curr_ps);

long proof_get_trace_idx_of(deduction& dct, long to_wrt_idx);
void proof_write_all_json_files_for(deduction& dct);
void proof_write_json_file_for(deduction& dct, long to_wrt_idx, long prv_wrt_idx);
void proof_append_ps(brain& brn, row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
			ch_string& pth_pref, proof_tk& pf_tk, row<ch_string>& all_to_move);
void proof_append_uniron(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
			row<ch_string>& all_to_move);
void proof_append_neu_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas);
void proof_append_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas);
void proof_append_neu(brain& brn, row<char>& json_str, neuron* neu, 
			ch_string& pth_pref, proof_tk& pf_tk, row<ch_string>& all_to_move);
void proof_append_qua(row<char>& json_str, quanton* qua);

//void proof_write_html_file_for(ch_string end_dir_pth, deduction& dct, proof_tk& pf_tk);

void proof_write_permutation(row<char>& json_str, neuromap& nmp);
void proof_write_ref_bj_proof_for(row<char>& json_str, deduction& dct);
void proof_write_bj_proof_for(neuromap& nmp, proof_tk& pf_tk);

void proof_move_all_to_mv(deduction& dct, ch_string& pf_dir_pth, 
						  row<ch_string>& all_to_move, proof_tk& pf_tk);

void proof_write_top_html_file(ch_string the_pth);

bool is_ptk_equal(proof_tk& tk1, proof_tk& tk2);

//=============================================================================
// proof_tk

class proof_tk {
	public:
	ticket	pt_brn_tk;
	long	pt_wrt_idx;

	// methods

	proof_tk(){
		init_proof_tk();
	}

	proof_tk(ticket& tk, long w_idx = INVALID_IDX){
		init_with(tk, w_idx);
	}

	void	init_with(ticket& tk, long w_idx = INVALID_IDX){
		pt_brn_tk = tk;
		pt_wrt_idx = w_idx;
	}
	
	void	init_proof_tk(){
		pt_brn_tk.init_ticket();
		pt_wrt_idx = INVALID_IDX;
	}

	brain*	get_dbg_brn(){
		return NULL;
	}
	
	solver*	get_dbg_slv(){
		return NULL;
	}
		
	bool	is_ptk_virgin(){
		bool c1 = pt_brn_tk.is_tk_virgin();
		bool c2 = (pt_wrt_idx == INVALID_IDX);

		return (c1 && c2);
	}
	
	ch_string	get_str();

};


#endif		// PROOF_FUNCS_H


