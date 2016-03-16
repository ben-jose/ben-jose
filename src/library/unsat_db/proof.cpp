

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

proof.cpp  

proof writing funcs.

--------------------------------------------------------------*/

#include "proof.h"
#include "html_strings.h"

ch_string top_show_proof_html_code = 
"<html> \n"
"	<head> \n"
"		<title>Show Ben Jose Proof Script</title> \n"
"		 <script src='show_proof_js_lib/show_proof.js'></script> \n"
"		 <link rel='stylesheet' type='text/css' \n"
"			 href='./show_proof_js_lib/sw_proof.css' media='screen' /> \n"
"	</head> \n"
"	<body> \n"
"		<h1>Show proof</h1> \n"
"		<textarea id='main_jsn_file' rows='3' \n"
"		cols='70'>/SKELETON/CNF/replace/this/with/a/path/to/a/file/cnf_proof.jsn \n"
"		</textarea><br> \n"
"		(the path must start with '/SKELETON/CNF/' and end with 'cnf_proof.jsn').<br> \n"
"		<ul class='collapsibleList'> \n"
"			<li> \n"
"				<label for='pru_lb'><a id='main_cnf_title'></a></label> \n"
"				<input type='checkbox' id='pru_lb' /> \n"
"				<ul><li id='main_cnf_cont'></li></ul> \n"
"			</li> \n"
"		</ul> \n"
"		<ul class='collapsibleList'> \n"
"			<li> \n"
"				<label for='main_proof_lb' onclick=\"populate_main_ul_with_txt_area()\"> \n"
"					<a id='proof_label'>LOAD FILE</a> \n"
"				</label> \n"
"				<input type='checkbox' id='main_proof_lb' /> \n"
"				<ul id='main_proof_ul'> \n"
"				</ul> \n"
"			</li> \n"
"		</ul> \n"
"	<script>populate_main_ul_with_param()</script> \n"
"	</body> \n"
"</html> \n"
;

ch_string
proof_tk::get_str(){
	bj_ostr_stream the_str;
	BRAIN_CK(pt_brn_tk.tk_recoil >= 0);
	
	long lv_val = pt_brn_tk.tk_level;
	ch_string inv_val = "";
	if(lv_val < 0){
		BRAIN_CK(lv_val == INVALID_LEVEL);
		lv_val = 0;
		inv_val = "IL";
	}
	
	the_str << "rc_" << pt_brn_tk.tk_recoil << "_lv_" << inv_val << lv_val;
	
	if(pt_wrt_idx != INVALID_IDX){
		the_str << "_" << pt_wrt_idx;
	}
	return the_str.str();
}

void
proof_do_res_step(brain& brn, row_quanton_t& curr_res, row_quanton_t& all_quas, quanton* qua)
{
	if(! brn.in_root_lv()){
		return;
	}
	
	if(curr_res.is_empty() && ! all_quas.is_empty()){
		all_quas.copy_to(curr_res);
		return;
	}
	
	if(qua == NULL_PT){
		return;
	}
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	qua->set_note1(brn);
	
	row_quanton_t nxt_res;
	append_all_not_note1(brn, curr_res, nxt_res);
	append_all_not_note1(brn, all_quas, nxt_res);
	
	qua->reset_note1(brn);
	reset_all_note1(brn, nxt_res);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
	nxt_res.move_to(curr_res);
}

void
proof_write_top_html_file(ch_string os_htm_pth){
	set_fstr(os_htm_pth, top_show_proof_html_code);
}

long
proof_max_end_idx(deduction& dct){
	//brain& brn = dct.get_brn();
	row<prop_signal>& trace = dct.dt_all_noted;
	long end_idx = trace.size() - 1;
	
	if(! trace.is_empty()){
		quanton* end_qua = trace.last().ps_quanton;
		if((end_qua != NULL_PT) && end_qua->in_root_qlv()){
			end_idx = trace.size();
		}
	}
	//if(brn.in_root_lv()){ end_idx = trace.size(); }
	return end_idx;
}

ch_string 
proof_add_paths(ch_string pth1, ch_string pth2){
	BRAIN_CK(*(pth1.rbegin()) != '/');
	ch_string pth3 = pth1 + "/" + pth2;
	return pth3;
}

ch_string 
proof_get_nmp_proof_path(neuromap& w_nmp){
	BRAIN_CK(w_nmp.na_tauto_oper_ok);
	ch_string tau_pth = w_nmp.na_tauto_pth;
	BRAIN_CK(*(tau_pth.rbegin()) == '/');
	ch_string pth_pref = tau_pth + SKG_PROOF_SUBDIR;
	return pth_pref;
}

ch_string 
proof_get_tk_dir_path(ch_string pth_pref, proof_tk& pf_tk){
	ch_string tk_str = pf_tk.get_str();
	if(pth_pref == SKG_INVALID_PTH){
		return tk_str;
	}
	ch_string pf_nm = proof_add_paths(pth_pref, tk_str);
	return pf_nm;
}

ch_string 
proof_get_tk_file_name(proof_tk& pf_tk){
	ch_string pf_f_nm = "proof_" + pf_tk.get_str() + ".jsn";
	return pf_f_nm;
}

ch_string 
proof_get_tk_html_file_name(proof_tk& pf_tk){
	ch_string pf_f_nm = "proof_" + pf_tk.get_str() + ".htm";
	return pf_f_nm;
}

long
proof_get_trace_idx_of(deduction& dct, long to_wrt_idx){
	row<prop_signal>& trace = dct.dt_all_noted;
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	long nmp_trce_idx = INVALID_IDX;
	
	if(all_to_wrt.is_valid_idx(to_wrt_idx)){
		BRAIN_CK(all_to_wrt[to_wrt_idx] != NULL_PT);
		neuromap& w_nmp = *(all_to_wrt[to_wrt_idx]);
		
		BRAIN_CK(w_nmp.na_dbg_real_cand != NULL_PT);
		BRAIN_CK(w_nmp.na_dbg_real_cand->nmp_is_cand());
		//BRAIN_CK(w_nmp.nmp_is_cand());
		BRAIN_DBG(quanton& cand_qua = *(w_nmp.na_dbg_real_cand->na_candidate_qua));
		BRAIN_CK(cand_qua.is_pos());
		
		nmp_trce_idx = w_nmp.na_to_wrt_trace_idx;
		
		BRAIN_CK((nmp_trce_idx != INVALID_IDX) || (cand_qua.qlevel() == ROOT_LEVEL));
		BRAIN_CK_PRT((nmp_trce_idx == INVALID_IDX) || (trace.is_valid_idx(nmp_trce_idx)), 
			DBG_PRT_ABORT(dct.get_brn());
			os << " nmp_trce_idx=" << nmp_trce_idx << "\n";
			os << " na_tauto_oper_ok=" << w_nmp.na_tauto_oper_ok << "\n";
		);
		
		DBG(
			brain& brn = dct.get_brn();
			DBG_PRT_COND_WITH(115, brn, brn.in_root_lv(),
				os << " THE_NMP=" << &w_nmp << "\n";
				os << " wrt_trce_idx=" << w_nmp.na_to_wrt_trace_idx << "\n";
				os << " nmp_trce_idx=" << nmp_trce_idx << "\n";
				if(trace.is_valid_idx(nmp_trce_idx)){
					os << " end_trace_ps=" << trace[nmp_trce_idx] << "\n";
				}
			);
		);
		
		if(nmp_trce_idx != INVALID_IDX){
			nmp_trce_idx++;
		}
	}
	return nmp_trce_idx;
}

void
proof_append_ps(brain& brn, row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				ch_string& prv_pf_pth, proof_tk& pf_tk, row<ch_string>& all_to_move)
{
	if(is_fst_ps){
		is_fst_ps = false;
	} else {
		canon_string_append(json_str, ",");
	}
			
	canon_string_append(json_str, "{");
	proof_append_neu(brn, json_str, the_sig.ps_source, prv_pf_pth, pf_tk, all_to_move);
	canon_string_append(json_str, "\"va_r\": ");
	proof_append_qua(json_str, the_sig.ps_quanton);
	canon_string_append(json_str, "}\n");
}

void 
proof_append_qua(row<char>& json_str, quanton* qua){
	if(qua == NULL_PT){
		canon_string_append(json_str, "\"0\"\n");
		return;
	}
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, qua->qu_id);
	canon_string_append(json_str, "\"\n");
}

void 
proof_append_neu_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas)
{
	canon_string_append(json_str, "\"neu_lits\": ");
	proof_append_lits(brn, json_str, all_quas);
	canon_string_append(json_str, ", \n");
}

void 
proof_append_lits(brain& brn, row<char>& json_str, row_quanton_t& all_quas)
{
	canon_string_append(json_str, "[");
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		if(aa > 0){
			canon_string_append(json_str, ",");
		}
		canon_string_append(json_str, " \"");
		canon_long_append(json_str, qua.qu_id);
		canon_string_append(json_str, "\"");
	}
	canon_string_append(json_str, "]");
}

void 
proof_append_neu(brain& brn, row<char>& json_str, neuron* neu, ch_string& prv_pf_pth, 
				 proof_tk& pf_tk, row<ch_string>& all_to_move)
{
	ch_string invalid_pth = SKG_INVALID_PTH;
	
	if(neu == NULL_PT){
		bool has_pf = ! pf_tk.is_ptk_virgin();
		bool has_prv_pth = (prv_pf_pth != invalid_pth);
		
		if(! has_prv_pth){
			canon_string_append(json_str, "\"neu_type\": \"null_neu\", \n");
		}
		canon_string_append(json_str, "\"neu_lits\": ");
		canon_string_append(json_str, "[],\n");
		canon_string_append(json_str, "\"neu_idx\": \"0\",");
		
		BRAIN_CK(has_pf == has_prv_pth);
		if(has_prv_pth){
			ch_string neu_full_file_pth = proof_add_paths(pf_tk.get_str(), 
						proof_get_tk_file_name(pf_tk));
			
			DBG_PRT_WITH(115, brn, os << "LEARNING_PRV_NEU=\n";
				os << " neu_full_jsn_pth=\n" << prv_pf_pth << "\n";
				os << " neu_js_pth=" << neu_full_file_pth << "\n";
				os << " pf_tk=" << pf_tk.get_str() << "\n";
			);
			
			canon_string_append(json_str, "\"neu_type\": \"full\", \n");
			canon_string_append(json_str, "\"neu_full_jsn_pth\": \"");
			canon_string_append(json_str, prv_pf_pth);
			canon_string_append(json_str, "\",\n");

			canon_string_append(json_str, "\"neu_jsn_pth\": \"");
			canon_string_append(json_str, neu_full_file_pth);
			canon_string_append(json_str, "\",\n");
			
		}
		return;
	}
			
	canon_string_append(json_str, "\"neu_type\": \"norm\", \n");
	
	proof_append_neu_lits(brn, json_str, neu->ne_fibres);
	
	canon_string_append(json_str, "\"neu_idx\": ");
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, neu->ne_index);
	canon_string_append(json_str, "\",\n");
	
	if(! neu->ne_original){
		proof_tk sub_pf_tk;
		sub_pf_tk.init_with(neu->ne_proof_tk, INVALID_IDX);
		ch_string neu_full_dir_pth = sub_pf_tk.get_str(); 
		ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
					proof_get_tk_file_name(sub_pf_tk));
		
		BRAIN_CK(! sub_pf_tk.is_ptk_virgin());
		BRAIN_CK(pf_tk.is_ptk_virgin());
		BRAIN_CK(prv_pf_pth == invalid_pth);
		
		canon_string_append(json_str, "\"neu_jsn_pth\": ");
		canon_string_append(json_str, "\"");
		canon_string_append(json_str, neu_full_file_pth);
		canon_string_append(json_str, "\",\n");

		all_to_move.push(neu_full_dir_pth);
		
		DBG_PRT_WITH(115, brn, os << "LEARNED_neu=";
			os << " neu=" << neu << "\n";
			os << " neu_js_pth=" << neu_full_file_pth << "\n";
			os << " pf_tk=" << pf_tk.get_str() << "\n";
			os << " sub_pf_tk=" << sub_pf_tk.get_str() << "\n";
			os << " ne_proof_tk=" << neu->ne_proof_tk.get_str() << "\n";
			os << " ne_candidate_tk=" << neu->ne_candidate_tk.get_str() << "\n";
		);
	}
}

void
proof_write_all_json_files_for(deduction& dct){
	BRAIN_CK(dct.ck_dbg_srcs());
	brain& brn = dct.get_brn();
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	
	DBG_PRT_WITH(115, brn, os << " num_to_WRT=" << all_to_wrt.size() << "\n";
		brn.dbg_prt_all_nmp(os, all_to_wrt, true);
		os << "\n";
	);
	
	dct.update_all_to_wrt_for_proof();
	
	long pp = INVALID_IDX;
	long aa = 0;
	for(aa = 0; aa < all_to_wrt.size(); aa++){
		BRAIN_CK(all_to_wrt[aa] != NULL_PT);
		neuromap& w_nmp = *(all_to_wrt[aa]);
		//neuromap& nmp = w_nmp;
		
		neuromap& nmp = w_nmp.map_to_write();
		BRAIN_CK(! nmp.is_na_mono());
		
		BRAIN_DBG(ch_string tg_minisha_str = "6aa80ae677");
		DBG_PRT_COND_WITH(115, brn, (nmp.na_dbg_diff_min_sha_str == tg_minisha_str),
			os << "TG_NMP_WRITING_JSON nmp=\n" << &nmp
		); // 6aa80ae677
		if(nmp.na_tauto_oper_ok){
			proof_write_json_file_for(dct, aa, pp);
			pp = aa;
		}
		DBG(
			if(! nmp.na_tauto_oper_ok){
				BRAIN_CK(nmp.na_dbg_real_cand != NULL_PT);
				BRAIN_CK(nmp.na_dbg_real_cand->nmp_is_cand());
				quanton& cand_qua = *(nmp.na_dbg_real_cand->na_candidate_qua);
				
				//BRAIN_CK(nmp.nmp_is_cand());
		
				ticket pf_tk = dct.dt_rsn.rs_tk;
				ticket nmp_pf_tk = cand_qua.qu_proof_tk;
				DBG_PRT_WITH(115, brn, os << "SKIPED_NMP=" << nmp.dbg_na_id() << "\n";
					os << " CAND_QUA=" << &cand_qua << "\n";
					os << " nmp_pf_tk=" << nmp_pf_tk.get_str() << "\n";
					os << " pf_tk=" << pf_tk.get_str() << "\n";
					os << " aa=" << aa << "\n";
					os << " pp=" << pp << "\n";
				);
			}
		);
	}
	proof_write_json_file_for(dct, INVALID_IDX, pp);
}

void
proof_append_uniron(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				row<ch_string>& all_to_move)
{
	if(is_fst_ps){
		is_fst_ps = false;
	} else {
		canon_string_append(json_str, ",");
	}
			
	canon_string_append(json_str, "{");
	
	quanton* uqu = the_sig.ps_quanton;
	BRAIN_CK(uqu != NULL_PT);
	
	proof_tk sub_pf_tk;
	sub_pf_tk.init_with(uqu->qu_proof_tk, INVALID_IDX);
	
	BRAIN_CK(! sub_pf_tk.is_ptk_virgin());	
	
	canon_string_append(json_str, "\"neu_type\": \"uniron\", \n");
	
	canon_string_append(json_str, "\"neu_lits\": [\"");
	canon_long_append(json_str, uqu->qu_id);
	canon_string_append(json_str, "\"], \n");
	
	canon_string_append(json_str, "\"neu_idx\": \"0\", \n");
	
	canon_string_append(json_str, "\"neu_jsn_pth\": ");
	canon_string_append(json_str, "\"");
	
	ch_string neu_full_dir_pth = sub_pf_tk.get_str(); 
	ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
				proof_get_tk_file_name(sub_pf_tk));
	canon_string_append(json_str, neu_full_file_pth);
	canon_string_append(json_str, "\",\n");
	
	canon_string_append(json_str, "\"va_r\": ");
	proof_append_qua(json_str, uqu);
	canon_string_append(json_str, "}\n");

	all_to_move.push(neu_full_dir_pth);
}

void proof_write_permutation(row<char>& json_str, neuromap& nmp)
{
	brain& brn = nmp.get_brn();
	coloring& col = nmp.na_tauto_col;
	BRAIN_CK(col.ck_cols());
	
	row_quanton_t& all_qua = col.co_quas;
	row_long_t& all_qu_col = col.co_qua_colors;
	
	row_neuron_t& all_neu = col.co_neus;
	
	DBG_PRT_COND_WITH(115, brn, (all_qua.is_empty() || all_neu.is_empty()), 
		os << "WRITING_BAD_PERMUTATION=\n";
		os << " all_qua=\n" << all_qua << "\n";
		os << " all_neu=\n";
		all_neu.print_row_data(os, true, "\n");
		os << "\n";
	);
	
	
	canon_string_append(json_str, "\t\"neuromap_ccls\" : [\n");
	for(long aa = 0; aa < all_neu.size(); aa++){
		BRAIN_CK(all_neu[aa] != NULL_PT);
		neuron& neu = *(all_neu[aa]);
		canon_string_append(json_str, "\t\t");
		if(aa > 0){
			canon_string_append(json_str, ",");
		}
		proof_append_lits(brn, json_str, neu.ne_fibres);
		canon_string_append(json_str, "\n");
	}
	canon_string_append(json_str, "\t],\n");
	
	ch_string var_perm_fld = "\t\"vars_permutation\" : [\n";
	canon_string_append(json_str, var_perm_fld);
	bool is_fst_qu = true;
	for(long aa = 0; aa < all_qua.size(); aa++){
		long q_col = all_qu_col[aa];
		if(q_col <= 0){
			continue;
		}
		BRAIN_CK(all_qua[aa] != NULL_PT);
		quanton& qua = *(all_qua[aa]);
		
		long qu_id = qua.qu_id;
		BRAIN_CK(qu_id != 0);
		if(qu_id < 0){
			qu_id = -qu_id;
			q_col = -q_col;
		}
		
		canon_string_append(json_str, "\t\t");
		if(! is_fst_qu){
			canon_string_append(json_str, ",");
		}
		if(is_fst_qu){ is_fst_qu = false; }
		
		canon_string_append(json_str, "[\"");
		canon_long_append(json_str, qu_id);
		canon_string_append(json_str, "\", \"");
		canon_long_append(json_str, q_col);
		canon_string_append(json_str, "\"]\n");
	}
	canon_string_append(json_str, "\t],\n");
	
	ch_string cla_perm_fld = "\t\"ccls_permutation\" : [\n";
	canon_string_append(json_str, cla_perm_fld);
	bool is_fst_ne = true;
	for(long aa = 0; aa < all_neu.size(); aa++){
		BRAIN_CK(all_neu[aa] != NULL_PT);
		neuron& neu = *(all_neu[aa]);
		
		long ne_id = neu.ne_index;
		//long n_col = all_ne_col[aa];

		canon_string_append(json_str, "\t\t");
		if(! is_fst_ne){
			canon_string_append(json_str, ",");
		}
		if(is_fst_ne){ is_fst_ne = false; }
		
		canon_string_append(json_str, "[\"");
		canon_long_append(json_str, ne_id);
		canon_string_append(json_str, "\", \"");
		canon_long_append(json_str, aa);
		canon_string_append(json_str, "\"]\n");
	}
	canon_string_append(json_str, "\t]\n");
}

void
proof_write_bj_proof_for(neuromap& nmp, proof_tk& pf_tk)
{
	brain& brn = nmp.get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	
	ch_string rel_pf_dir_pth = pf_tk.get_str();
	ch_string rel_pf_pth = proof_add_paths(rel_pf_dir_pth, proof_get_tk_file_name(pf_tk));
	
	row<char> json_str;
	canon_string_append(json_str, "{\n");
	
	canon_string_append(json_str, "\t\"neuromap_proof\" : \"");
	canon_string_append(json_str, rel_pf_pth);
	canon_string_append(json_str, "\",\n");

	proof_write_permutation(json_str, nmp);
	
	canon_string_append(json_str, "}\n\n");

	ch_string os_bj_nmp_pf_dir_pth = skg.as_full_path(proof_get_nmp_proof_path(nmp));
	ch_string os_bj_nmp_pf_pth = proof_add_paths(os_bj_nmp_pf_dir_pth, SKG_END_JSN_NAME);
	
	write_file(os_bj_nmp_pf_pth, json_str, true); //OS_OPER
	
	DBG_PRT_WITH(115, brn, os << "Wrote FINAL_PROOF_JSN=\n" << os_bj_nmp_pf_pth << "\n";
		os << " FOR_TAUTO=\n" << brn.br_tmp_wrt_tauto_cnf << "\n";
		//os << " FOR_NMP=\n" << nmp << "\n";
	);
}

void
proof_move_all_to_mv(deduction& dct, ch_string& pf_dir_pth, 
					 row<ch_string>& all_to_move, proof_tk& pf_tk)
{
	brain& brn = dct.get_brn();
	skeleton_glb& skg = brn.get_skeleton();

	BRAIN_CK(file_exists(skg.as_full_path(skg.kg_tmp_proof_path)));
	ch_string os_dir_pth = skg.as_full_path(pf_dir_pth);
	
	bool exted = file_exists(os_dir_pth);
	
	DBG_PRT_WITH(115, brn, 
		os << " all_to_mv >>>>>\n";
		all_to_move.print_row_data(os, true, "\n");
		os << " all_to_mv <<<<<\n";
		if(exted){
			os << " EXISTED=" << os_dir_pth << "\n";
		}
	);
	
	if(! exted){
		BRAIN_CK_PRT(dct.dt_found_top || (brn.recoil() == pf_tk.pt_brn_tk.tk_recoil), 
			DBG_PRT_ABORT(brn);
			os << " br_reco=" << brn.recoil() << "\n";
			os << " pf_tk_roce=" << pf_tk.pt_brn_tk.tk_recoil << "\n";
			os << " pf_tk_tk=" << pf_tk.pt_brn_tk << "\n";
			os << " pf_tk_tk_virg=" << pf_tk.pt_brn_tk.is_tk_virgin() << "\n";
			os << " pf_tk_idx=" << pf_tk.pt_wrt_idx << "\n";
			os << " os_dir_pth=\n" << os_dir_pth << "\n";
			os << " pf_dir_pth=\n" << pf_dir_pth << "\n";
		);
		path_create(os_dir_pth); //OS_OPER
		DBG_PRT_WITH(115, brn, 
			os << "Created_path=\n" << os_dir_pth << "\n";
			os << " pf_dir_pth=" << pf_dir_pth << "\n";
		);
	}
	
	
	for(long aa = 0; aa < all_to_move.size(); aa++){
		ch_string pth_mv = all_to_move[aa];
		ch_string src_pth_mv = proof_add_paths(skg.kg_tmp_proof_path, pth_mv);
		ch_string dst_pth_mv = proof_add_paths(pf_dir_pth, pth_mv);
		ch_string os_src_pth_mv = skg.as_full_path(src_pth_mv);
		ch_string os_dst_pth_mv = skg.as_full_path(dst_pth_mv);
		
		BRAIN_CK_PRT((file_exists(os_src_pth_mv)), 
			DBG_PRT_ABORT(brn);
			os << " RENAME1\n";
			os << " os_src_pth_mv=\n" << os_src_pth_mv << "\n";
			os << " os_dst_pth_mv=\n" << os_dst_pth_mv << "\n";
		);
		BRAIN_CK(file_exists(skg.as_full_path(pf_dir_pth)));
		
		bool ok1 = rename_file(os_src_pth_mv, os_dst_pth_mv); //OS_OPER
		BRAIN_CK_PRT(ok1, 
			DBG_PRT_ABORT(brn);
			os << " RENAME1\n";
			os << " os_src_pth_mv=\n" << os_src_pth_mv << "\n";
			os << " os_dst_pth_mv=\n" << os_dst_pth_mv << "\n";
			os << " pth_mv=" << pth_mv << "\n";
		);
		DBG_PRT_WITH(115, brn, os << "MOVED1\n";
			os << " src_pth=\n" << os_src_pth_mv << "\n";
			os << " dst_pth=\n" << os_dst_pth_mv << "\n";
		);
	}
	
}

void
proof_write_json_file_for(deduction& dct, long to_wrt_idx, long prv_wrt_idx)
{
	brain& brn = dct.get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	ch_string invalid_pth = SKG_INVALID_PTH;
	
	row<prop_signal>& trace = dct.dt_all_noted;
	reason& rsn = dct.dt_rsn;
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	
	bool has_prv = (prv_wrt_idx != INVALID_IDX);
	
	neuromap* pt_nmp = NULL_PT;
	
	DBG_PRT_WITH(115, brn, 
		os << "=================================================================\n";
		os << "JSON_FILE_FOR wrt_idx=" << to_wrt_idx << " prv_idx=" << prv_wrt_idx;
		os << " has_prv=" << has_prv;
		os << " in_root=" << brn.in_root_lv() << "\n";
		brn.print_trail(os);
		os << "\n ALL_CONF=\n";
		brn.br_all_conflicts_found.print_row_data(os, true, "\n");
		os << "\n LAST_NMP_FOUND=\n" << dct.dt_last_found << "\n";
		os << "\n FIRST_CAUSES=\n";
		os << dct.dt_first_causes << "\n";
		os << "\n ALL_NOTED=\n";
		trace.print_row_data(os, true, "\n");
		os << "\n REASON=\n";
		os << rsn << "\n";
		os << "learned_unirons=" << brn.br_learned_unit_neurons << "\n";
	);
	
	long end_trace_idx = proof_max_end_idx(dct);
	
	proof_tk invalid_tk;
	proof_tk pf_tk;
	pf_tk.init_with(rsn.rs_tk, to_wrt_idx);
	
	ch_string end_dir_pth = SKG_INVALID_PTH;
	if(all_to_wrt.is_valid_idx(to_wrt_idx)){
		end_trace_idx = proof_get_trace_idx_of(dct, to_wrt_idx);
		
		pt_nmp = all_to_wrt[to_wrt_idx];
		BRAIN_CK(pt_nmp != NULL_PT);
		
		neuromap& w_nmp = *pt_nmp;
		
		BRAIN_CK(w_nmp.na_dbg_real_cand != NULL_PT);
		BRAIN_CK(w_nmp.na_dbg_real_cand->nmp_is_cand());
		//BRAIN_CK(w_nmp.nmp_is_cand());
		end_dir_pth = proof_get_nmp_proof_path(w_nmp);
		
		BRAIN_DBG(quanton& cand_qua = *(w_nmp.na_dbg_real_cand->na_candidate_qua));
		//pf_tk = cand_qua.qu_proof_tk;
		
		DBG_PRT_WITH(115, brn,
			os << " WRT_NMP=" << &w_nmp << "\n";
			os << " CAND_QUA=" << &cand_qua << "\n";
			proof_tk cand_pf_tk(cand_qua.qu_proof_tk);
			ch_string cand_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, cand_pf_tk);
			os << " CAND_QUA_pf_tk=" << cand_pf_tk.get_str() << "\n";
			os << " CAND_QUA_pf_pth=" << cand_pth << "\n";
			os << " end_trace_idx=" << end_trace_idx << "\n";
			if(trace.is_valid_idx(end_trace_idx)){
				os << " end_trace_ps=" << trace[end_trace_idx] << "\n";
			}
		);
		DBG_PRT_WITH(115, brn,
			if(end_trace_idx == INVALID_IDX){
				os << " nmp_trc_idx=" << w_nmp.na_to_wrt_trace_idx << "\n";
				os << " nmp_lv=" << cand_qua.qlevel() << "\n";
			}
		);
	}
	
	if(brn.in_root_lv() && (to_wrt_idx == INVALID_IDX)){
	}
	
	row<ch_string> all_to_move;
	
	//BRAIN_CK(to_wrt->na_candidate_qua == this);
	
	ch_string proof_final_skl_pth = proof_get_tk_dir_path(end_dir_pth, pf_tk);
	
	row<char> json_str;	
	bool is_fst_ps = true;
	
	canon_string_append(json_str, "{\n");
	canon_string_append(json_str, "\"name\": \"" + proof_final_skl_pth + "\",\n");
	canon_string_append(json_str, "\"chain\": [\n");
	
	if(! has_prv){
		canon_string_append(json_str, "{");
		if(dct.dt_confl != NULL_PT){
			BRAIN_CK(dct.dt_confl->ne_original);
			proof_append_neu(brn, json_str, dct.dt_confl, 
							 invalid_pth, invalid_tk, all_to_move);
			BRAIN_CK(all_to_move.is_empty());
		} else {
			proof_write_ref_bj_proof_for(json_str, dct);
		}
		canon_string_append(json_str, "\"va_r\": ");
		proof_append_qua(json_str, NULL_PT);
		canon_string_append(json_str, "}\n");
		is_fst_ps = false;
	}
	
	long ini_trace_idx = 0;
	if(has_prv){
		ini_trace_idx = proof_get_trace_idx_of(dct, prv_wrt_idx);
		if((ini_trace_idx != INVALID_IDX) && (end_trace_idx == INVALID_IDX)){
			end_trace_idx = proof_max_end_idx(dct);
			//end_trace_idx = trace.size();
		}
		if(ini_trace_idx == INVALID_IDX){
			BRAIN_CK_PRT((	(end_trace_idx == INVALID_IDX) || 
							(end_trace_idx == proof_max_end_idx(dct))
						 ), 
				DBG_PRT_ABORT(brn);
				os << " to_wrt_sz=" << all_to_wrt.size() << "\n";
				os << " end_trace_idx=" << end_trace_idx << "\n";
				os << " max_end=" << proof_max_end_idx(dct) << "\n";
			);
			ini_trace_idx = trace.size();
		}
		
		BRAIN_CK(all_to_wrt.is_valid_idx(prv_wrt_idx));
		neuromap* pt_prv_nmp = all_to_wrt[prv_wrt_idx];
		BRAIN_CK(pt_prv_nmp != NULL_PT);
		
		neuromap& prv_nmp = *pt_prv_nmp;
		
		BRAIN_CK(prv_nmp.na_dbg_real_cand != NULL_PT);
		BRAIN_CK(prv_nmp.na_dbg_real_cand->nmp_is_cand());
		//BRAIN_CK(prv_nmp.nmp_is_cand());
		BRAIN_CK(prv_nmp.na_tauto_oper_ok);
		ch_string end_prv_dir_pth = proof_get_nmp_proof_path(prv_nmp);

		proof_tk pf_prv_tk;
		pf_prv_tk.init_with(rsn.rs_tk, prv_wrt_idx);

		BRAIN_CK(is_fst_ps);
		prop_signal invalid_ps;
		proof_append_ps(brn, json_str, invalid_ps, is_fst_ps, 
						end_prv_dir_pth, pf_prv_tk, all_to_move);
	}

	if(! has_prv && (end_trace_idx == INVALID_IDX)){
		end_trace_idx = proof_max_end_idx(dct);
		//end_trace_idx = trace.size();
	}
	
	//BRAIN_CK(all_to_move.is_empty());
	
	DBG_PRT_WITH(115, brn, 
		os << " all_to_mv_BEFORE >>>>>\n";
		all_to_move.print_row_data(os, true, "\n");
		os << " all_to_mv <<<<<\n";
	);
	
	if(end_trace_idx != INVALID_IDX){
		BRAIN_CK(ini_trace_idx != INVALID_IDX);

		DBG_PRT_WITH(115, brn,
			os << " APPENDING_TRACE\n";
			os << " ini_trace_idx=" << ini_trace_idx << "\n";
			if(trace.is_valid_idx(ini_trace_idx)){
				os << " ini_trace_idx_ps=" << trace[ini_trace_idx] << "\n";
			}
			os << " end_trace_idx=" << end_trace_idx << "\n";
			if(trace.is_valid_idx(end_trace_idx)){
				os << " end_trace_idx_ps=" << trace[end_trace_idx] << "\n";
			}
			os << " trace_sz=" << trace.size() << "\n";
		);
		
		// APPEND_THE_TRACE
		for(long aa = ini_trace_idx; aa < end_trace_idx; aa++){
			prop_signal& nxt_sig = trace[aa];
			
			bool is_uron = nxt_sig.is_ps_uniron();

			if(is_uron){
				DBG_PRT_WITH(115, brn, 
					ticket& sub_pf_tk = nxt_sig.ps_quanton->qu_proof_tk;
					os << " UNIRON=" << nxt_sig;
					os << " q_pf_tk=" << sub_pf_tk.get_str() << "\n";
					os << " trc_sz=" << trace.size() << "\n";
					os << " end_idx=" << end_trace_idx << "\n";
					os << " pf_tk=" << pf_tk.get_str() << "\n";
					os << " sub_pf_tk=" << sub_pf_tk.get_str() << "\n";
				);
				proof_append_uniron(json_str, nxt_sig, is_fst_ps, all_to_move);
			} else {
				proof_append_ps(brn, json_str, nxt_sig, is_fst_ps, 
								invalid_pth, invalid_tk, all_to_move);
			}
		}
	}
	
	canon_string_append(json_str, "]\n");
	canon_string_append(json_str, "}\n");
	
	ch_string pf_dir_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, pf_tk);
	proof_move_all_to_mv(dct, pf_dir_pth, all_to_move, pf_tk);

	ch_string os_dir_pth = skg.as_full_path(pf_dir_pth);
	ch_string os_file_pth = proof_add_paths(os_dir_pth, proof_get_tk_file_name(pf_tk));
	write_file(os_file_pth, json_str, true); //OS_OPER
	DBG_PRT_WITH(115, brn, os << "Wrote file=\n" << os_file_pth);
	
	if(end_dir_pth != SKG_INVALID_PTH){
		ch_string os_end_dir_pth = skg.as_full_path(end_dir_pth);
		path_create(os_end_dir_pth);
		DBG_PRT_WITH(115, brn, 
			os << "Final_htm for dct=" << dct << " wrt_idx=" << to_wrt_idx;
			os << " prv_idx=" << prv_wrt_idx << ". END_PTH=\n";
			os << end_dir_pth;
			os << " pk_tk=" << pf_tk.get_str() << "\n";
			os << " trc_sz=" << trace.size() << "\n";
			os << " end_idx=" << end_trace_idx << "\n";
		);
		
		ch_string src_pth_mv = pf_dir_pth;
		ch_string dst_pth_mv = proof_get_tk_dir_path(end_dir_pth, pf_tk);
		ch_string os_src_pth_mv = skg.as_full_path(src_pth_mv);
		ch_string os_dst_pth_mv = skg.as_full_path(dst_pth_mv);
		
		BRAIN_CK(file_exists(os_src_pth_mv));
		BRAIN_CK(file_exists(os_end_dir_pth));
		
		bool ok2 = rename_file(os_src_pth_mv, os_dst_pth_mv); //OS_OPER
		BRAIN_CK_PRT(ok2,
			DBG_PRT_ABORT(brn);
			os << " RENAME2\n";
			os << " os_src_pth_mv=\n" << os_src_pth_mv << "\n";
			os << " os_dst_pth_mv=\n" << os_dst_pth_mv << "\n";
		);
		DBG_PRT_WITH(115, brn, os << "MOVED2\n";
			os << " src_pth=\n" << os_src_pth_mv << "\n";
			os << " dst_pth=\n" << os_dst_pth_mv << "\n";
		);

		BRAIN_CK(pt_nmp != NULL_PT);
		neuromap& w_nmp = *pt_nmp;
		
		ch_string os_end_jsn_pth = proof_add_paths(os_dst_pth_mv, SKG_END_JSN_NAME);
	
		proof_write_bj_proof_for(w_nmp, pf_tk);
	}
}

void
proof_write_ref_bj_proof_for(row<char>& json_str, deduction& dct){
	brain& brn = dct.get_brn();
	
	BRAIN_CK(dct.dt_last_found != NULL_PT);
	DBG_PRT_WITH(115, brn, os << "PROVED_CNF nmp=\n" << dct.dt_last_found); // 6aa80ae677
	
	neuromap& nmp = *(dct.dt_last_found);

	ch_string nmp_pf_dir_pth = proof_get_nmp_proof_path(nmp);
	ch_string pf_nm = SKG_END_JSN_NAME;

	DBG_PRT_WITH(115, brn, os << "LEARNING_SUBST_NEU=\n";
		os << " neu_full_jsn_pth=\n" << nmp_pf_dir_pth << "\n";
		os << " neu_js_pth=" << pf_nm << "\n";
	);
	
	canon_string_append(json_str, "\"neu_type\": \"subst\", \n");
	canon_string_append(json_str, "\"neu_full_jsn_pth\": \"");
	canon_string_append(json_str, nmp_pf_dir_pth);
	canon_string_append(json_str, "\",\n");

	canon_string_append(json_str, "\"neu_jsn_pth\": \"");
	canon_string_append(json_str, pf_nm);
	canon_string_append(json_str, "\",\n");
	
	
	proof_write_permutation(json_str, nmp);
	
	canon_string_append(json_str, ", \n");
	
	proof_append_neu_lits(brn, json_str, dct.dt_first_causes);
}

