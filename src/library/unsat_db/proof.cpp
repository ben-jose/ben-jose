

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
"		cols='70'>../CNF/replace/this/with/a/path/to/a/file/cnf_proof.jsn \n"
"		</textarea><br> \n"
"		(the path must start with '../CNF/' and end with 'cnf_proof.jsn').<br> \n"
"		<ul class='collapsibleList'> \n"
"			<li> \n"
"				<label for='pru_lb'><a id='main_cnf_title'></a></label> \n"
"				<input type='checkbox' id='pru_lb' /> \n"
"				<ul><li id='main_cnf_cont'></li></ul> \n"
"			</li> \n"
"		</ul> \n"
"		<ul class='collapsibleList'> \n"
"			<li> \n"
"				<label for='main_proof_lb' \n"
"						onclick=\"populate_main_ul_2('main_proof_ul', 'main_jsn_file')\"> \n"
"					<a id='proof_label'>LOAD FILE</a> \n"
"				</label> \n"
"				<input type='checkbox' id='main_proof_lb' /> \n"
"				<ul id='main_proof_ul'> \n"
"				</ul> \n"
"			</li> \n"
"		</ul> \n"
"	</body> \n"
"</html> \n"
;

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

ch_string 
proof_add_paths(ch_string pth1, ch_string pth2){
	BRAIN_CK(*(pth1.rbegin()) != '/');
	ch_string pth3 = pth1 + "/" + pth2;
	return pth3;
}

ch_string 
proof_get_nmp_proof_path(neuromap& the_nmp){
	ch_string tau_pth = the_nmp.na_tauto_pth;
	BRAIN_CK(*(tau_pth.rbegin()) == '/');
	ch_string pth_pref = tau_pth + SKG_PROOF_SUBDIR;
	return pth_pref;
}

ch_string 
proof_get_tk_dir_path(ch_string pth_pref, ticket& pf_tk){
	ch_string tk_str = pf_tk.get_str();
	if(pth_pref == SKG_INVALID_PTH){
		return tk_str;
	}
	ch_string pf_nm = proof_add_paths(pth_pref, tk_str);
	return pf_nm;
}

ch_string 
proof_get_tk_file_name(ticket& pf_tk){
	ch_string pf_f_nm = "proof_" + pf_tk.get_str() + ".jsn";
	return pf_f_nm;
}

ch_string 
proof_get_tk_html_file_name(ticket& pf_tk){
	ch_string pf_f_nm = "proof_" + pf_tk.get_str() + ".htm";
	return pf_f_nm;
}

long
proof_get_trace_idx_of(deduction& dct, long to_wrt_idx){
	row<prop_signal>& trace = dct.dt_all_noted;
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	long nmp_trce_idx = INVALID_IDX;
	
	if(all_to_wrt.is_valid_idx(to_wrt_idx)){
		neuromap* the_nmp = all_to_wrt[to_wrt_idx];
		BRAIN_CK(the_nmp != NULL_PT);
		BRAIN_CK(the_nmp->nmp_is_cand());
		BRAIN_DBG(quanton& qua = *(the_nmp->na_candidate_qua));
		BRAIN_CK(qua.is_pos());
		
		nmp_trce_idx = the_nmp->na_to_wrt_trace_idx;
		
		BRAIN_CK((nmp_trce_idx != INVALID_IDX) || (qua.qlevel() == ROOT_LEVEL));
		BRAIN_CK_PRT((nmp_trce_idx == INVALID_IDX) || (trace.is_valid_idx(nmp_trce_idx)), 
			DBG_PRT_ABORT(dct.get_brn());
			os << " nmp_trce_idx=" << nmp_trce_idx << "\n";
			os << " na_wrt_ok=" << the_nmp->na_wrt_ok << "\n";
		);
		
		DBG(
			brain& brn = dct.get_brn();
			if(brn.in_root_lv()){
				DBG_PRT_WITH(115, brn, 
					os << " THE_NMP=" << the_nmp << "\n";
					os << " wrt_trce_idx=" << the_nmp->na_to_wrt_trace_idx << "\n";
					os << " nmp_trce_idx=" << nmp_trce_idx << "\n";
					if(trace.is_valid_idx(nmp_trce_idx)){
						os << " end_trace_ps=" << trace[nmp_trce_idx] << "\n";
					}
				);
			}
		);
		
		if(nmp_trce_idx != INVALID_IDX){
			nmp_trce_idx++;
		}
	}
	return nmp_trce_idx;
}

void
proof_append_ps(brain& brn, row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				ch_string& pth_pref, ticket& pf_tk, row<ch_string>& all_to_move)
{
	if(is_fst_ps){
		is_fst_ps = false;
	} else {
		canon_string_append(json_str, ",");
	}
			
	canon_string_append(json_str, "{");
	proof_append_neu(brn, json_str, the_sig.ps_source, pth_pref, pf_tk, all_to_move);
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
proof_append_neu(brain& brn, row<char>& json_str, neuron* neu, ch_string& pth_pref, 
				 ticket& pf_tk, row<ch_string>& all_to_move)
{
	if(neu == NULL_PT){
		canon_string_append(json_str, "\"neu_lits\": ");
		canon_string_append(json_str, "[],\n");
		canon_string_append(json_str, "\"neu_idx\": \"0\",");
		return;
	}
	//BRAIN_DBG(brain* pt_brn = neu->get_dbg_brn());
	//BRAIN_CK(pt_brn != NULL_PT);
	//BRAIN_DBG(brain& brn = *pt_brn);
			
	proof_append_neu_lits(brn, json_str, neu->ne_fibres);
	
	canon_string_append(json_str, "\"neu_idx\": ");
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, neu->ne_index);
	canon_string_append(json_str, "\",\n");
	
	ch_string invalid_pth = SKG_INVALID_PTH;
	
	if(! neu->ne_original){
		ticket sub_pf_tk = neu->ne_proof_tk;
		
		if(! pf_tk.is_tk_virgin()){
			sub_pf_tk = pf_tk;
		}
		
		canon_string_append(json_str, "\"neu_jsn\": ");
		canon_string_append(json_str, "\"");
		//ch_string neu_full_dir_pth = proof_get_tk_dir_path(pth_pref, sub_pf_tk); 
		ch_string neu_full_dir_pth = sub_pf_tk.get_str(); 
		ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
					proof_get_tk_file_name(sub_pf_tk));
		canon_string_append(json_str, neu_full_file_pth);
		canon_string_append(json_str, "\",\n");
		
		if(! is_tk_equal(pf_tk, sub_pf_tk)){
			all_to_move.push(neu_full_dir_pth);
		}
		
		DBG_PRT_WITH(115, brn, os << "LEARNED_neu_in=\n";
			os << neu_full_file_pth << "\n";
			os << " pf_tk=" << pf_tk.get_str() << "\n";
			os << " sub_pf_tk=" << sub_pf_tk.get_str() << "\n";
		);
	}
}

void
proof_write_all_json_files_for(deduction& dct){
	BRAIN_CK(dct.ck_dbg_srcs());
	brain& brn = dct.get_brn();
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;

	DBG_PRT_WITH(115, brn, os << "NUM_TO_WRT=" << all_to_wrt.size());
	
	long pp = INVALID_IDX;
	long aa = 0;
	for(aa = 0; aa < all_to_wrt.size(); aa++){
		neuromap& nmp = *(all_to_wrt[aa]);
		if(nmp.na_wrt_ok){
			proof_write_json_file_for(dct, aa, pp);
			pp = aa;
		}
		DBG(
			if(! nmp.na_wrt_ok){
				BRAIN_CK(nmp.nmp_is_cand());
		
				ticket pf_tk = dct.dt_rsn.rs_tk;
				ticket nmp_pf_tk = nmp.na_candidate_qua->qu_proof_tk;
				DBG_PRT_WITH(115, brn, os << "SKIPED_NMP=" << nmp.dbg_na_id() << "\n";
					os << " CAND_QUA=" << nmp.na_candidate_qua << "\n";
					os << " nmp_pf_tk=" << nmp_pf_tk.get_str() << "\n";
					os << " pf_tk=" << pf_tk.get_str() << "\n";
					os << " aa=" << aa << "\n";
					os << " pp=" << pp << "\n";
				);
			}
		);
	}
	proof_write_json_file_for(dct, INVALID_IDX, pp);
	
	//br_charge_trail.get_qu_layer(
}

void
proof_append_uniron(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				ch_string& pth_pref, ticket& pf_tk, row<ch_string>& all_to_move)
{
	if(is_fst_ps){
		is_fst_ps = false;
	} else {
		canon_string_append(json_str, ",");
	}
			
	canon_string_append(json_str, "{");
	
	quanton* uqu = the_sig.ps_quanton;
	BRAIN_CK(uqu != NULL_PT);
	
	BRAIN_CK(! uqu->qu_proof_tk.is_tk_virgin());	
	ticket& sub_pf_tk = uqu->qu_proof_tk;
	
	canon_string_append(json_str, "\"neu_lits\": [\"");
	canon_long_append(json_str, uqu->qu_id);
	canon_string_append(json_str, "\"], \n");
	
	canon_string_append(json_str, "\"neu_idx\": \"0\", \n");
	
	canon_string_append(json_str, "\"neu_jsn\": ");
	canon_string_append(json_str, "\"");
	//ch_string neu_full_dir_pth = proof_get_tk_dir_path(pth_pref, sub_pf_tk); 
	ch_string neu_full_dir_pth = sub_pf_tk.get_str(); 
	ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
				proof_get_tk_file_name(sub_pf_tk));
	canon_string_append(json_str, neu_full_file_pth);
	canon_string_append(json_str, "\",\n");
	
	canon_string_append(json_str, "\"va_r\": ");
	proof_append_qua(json_str, uqu);
	canon_string_append(json_str, "}\n");

	if(! is_tk_equal(pf_tk, sub_pf_tk)){
		all_to_move.push(neu_full_dir_pth);
	}
}

/*
void
proof_write_html_file_for(ch_string end_dir_pth, deduction& dct, ticket& pf_tk){
	brain& brn = dct.get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	
	ch_string os_end_dir_pth = skg.as_full_path(end_dir_pth);
	ch_string os_htm_pth = proof_add_paths(os_end_dir_pth, 
							proof_get_tk_html_file_name(pf_tk));

	ch_string loc_sh_pf_js_pth = proof_add_paths(end_dir_pth, SKG_SHOW_PROOF_JS_SUBDIR);
	ch_string rel_js_pth = get_relative_path(end_dir_pth, skg.kg_show_proof_js_path);
	
	ch_string os_js_pth = skg.as_full_path(loc_sh_pf_js_pth);
	if(! file_exists(os_js_pth)){
		bool lk_ok = path_create_link(rel_js_pth, os_js_pth);
		SKELETON_CK(lk_ok);
	}
	
	bj_ofstream of;
	bj_ofstream_open(os_htm_pth, of, false);
	
	ch_string htm_tit = "tit_html";
	
	of << HTMi_html << bj_eol;
	of << "\t" << HTMi_head << bj_eol;
	of << "\t\t" << HTMi_title << htm_tit << HTMe_title << bj_eol;
	
	ch_string dir_str = SKG_SHOW_PROOF_JS_SUBDIR;
	of << "\t\t " << HTMi_src << dir_str << "/show_proof.js" << HTMe_src << bj_eol;
	of << "\t\t " << HTM_css(dir_str) << "\n";
	
	of << "\t" << HTMe_head << bj_eol;
	of << "\t" << HTMi_body << bj_eol;
	
	of << "\t" << HTMi_proof_ul << "\n";
	
	// THE BODY
	ch_string id_str = pf_tk.get_str();
	ch_string pf_f_nm = proof_get_tk_file_name(pf_tk);
	
	ch_string nd_id = "nd_" + id_str + "_id";
	ch_string nd_lst = "nd_" + id_str + "_lst";
	ch_string jsn_file = id_str + "/" + pf_f_nm;
	ch_string lbl_str = "UNSATISFABILITY PROOF";
	of << HTM_proof_li(nd_id, nd_lst, jsn_file, lbl_str) << "\n";

	of << "\t" << HTMe_proof_ul << "\n";
	
	of << "\t" << HTMe_body << bj_eol;
	of << HTMe_html << bj_eol;
	
	of.flush();
	of.close();

	DBG_PRT_WITH(115, brn, os << "Wrote_htm_file=\n" << os_htm_pth);
}
 
ch_string
proof_get_unirons_path(skeleton_glb& skg, bool& is_tmp){
	ch_string unirons_pth = proof_add_paths(skg.kg_tmp_proof_path, SKG_UNIRONS_SUBDIR);
	ch_string os_unirons_pth = skg.as_full_path(unirons_pth);
	is_tmp = true;
	if(file_exists(os_unirons_pth)){
		ch_string unirons_ref = proof_add_paths(skg.kg_tmp_proof_path, SKG_UNIRONS_REF_FNAM);
		ch_string os_unirons_ref = skg.as_full_path(unirons_ref);
		if(file_exists(os_unirons_ref)){
			is_tmp = false;
			unirons_pth = skg.ref_read(unirons_ref);
		}
	} else {
		path_create(os_unirons_pth);
	}
	return unirons_pth;
}

void
proof_create_final_unirons_path(skeleton_glb& skg, ch_string end_dir_pth){
	ch_string unirons_pth = proof_add_paths(skg.kg_tmp_proof_path, SKG_UNIRONS_SUBDIR);
	ch_string os_unirons_pth = skg.as_full_path(unirons_pth);

	BRAIN_CK(file_exists(os_unirons_pth));
	
	ch_string final_unirons_pth = proof_add_paths(end_dir_pth, SKG_UNIRONS_SUBDIR);
	ch_string os_final_unirons_pth = skg.as_full_path(final_unirons_pth);

	BRAIN_CK(file_exists(skg.as_full_path(end_dir_pth)));
	bool ok1 = rename_file(os_unirons_pth, os_final_unirons_pth); //OS_OPER
	MARK_USED(ok1);
	BRAIN_CK(ok1);
	
	bool lk_ok = path_create_link(os_final_unirons_pth, os_unirons_pth);
	SKELETON_CK(lk_ok);
	
	ch_string unirons_ref = proof_add_paths(skg.kg_tmp_proof_path, SKG_UNIRONS_REF_FNAM);
	ch_string os_unirons_ref = skg.as_full_path(unirons_ref);
	
	BRAIN_CK(! file_exists(os_unirons_ref));
	skg.ref_write(unirons_ref, final_unirons_pth);
	BRAIN_CK(file_exists(os_unirons_ref));
}*/

void
proof_move_all_to_mv(deduction& dct, ch_string& pf_dir_pth, row<ch_string>& all_to_move){
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
		path_create(os_dir_pth); //OS_OPER
		DBG_PRT_WITH(115, brn, os << "Created_path=\n" << os_dir_pth << "\n";
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
		os << " ALL_CONF=\n";
		brn.br_all_conflicts_found.print_row_data(os, true, "\n");
		os << " ALL_NOTED=\n";
		trace.print_row_data(os, true, "\n");
		os << " REASON=\n";
		os << rsn << "\n";
		os << "learned_unirons=" << brn.br_learned_unit_neurons << "\n";
	);
	
	long end_trace_idx = trace.size() - 1;
	if(brn.in_root_lv()){ end_trace_idx++; }
	
	//end_trace_idx = trace.size();
	ticket invalid_tk;
	ticket pf_tk = rsn.rs_tk;
	
	ch_string end_dir_pth = SKG_INVALID_PTH;
	if(all_to_wrt.is_valid_idx(to_wrt_idx)){
		end_trace_idx = proof_get_trace_idx_of(dct, to_wrt_idx);
		
		pt_nmp = all_to_wrt[to_wrt_idx];
		BRAIN_CK(pt_nmp != NULL_PT);
		
		neuromap& the_nmp = *pt_nmp;
		BRAIN_CK(the_nmp.nmp_is_cand());
		BRAIN_CK(the_nmp.na_wrt_ok);
		end_dir_pth = proof_get_nmp_proof_path(the_nmp);
		
		//pf_tk = the_nmp.na_candidate_qua->qu_proof_tk;
		
		DBG_PRT_WITH(115, brn,
			os << " WRT_NMP=" << &the_nmp << "\n";
			quanton* nmp_cand_qu = the_nmp.na_candidate_qua;
			os << " CAND_QUA=" << nmp_cand_qu << "\n";
			if(nmp_cand_qu != NULL_PT){
				ticket& cand_pf_tk = nmp_cand_qu->qu_proof_tk;
				ch_string cand_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, cand_pf_tk);
				os << " CAND_QUA_pf_tk=" << cand_pf_tk << "\n";
				os << " CAND_QUA_pf_pth=" << cand_pth << "\n";
				os << " end_trace_idx=" << end_trace_idx << "\n";
				if(trace.is_valid_idx(end_trace_idx)){
					os << " end_trace_ps=" << trace[end_trace_idx] << "\n";
				}
			}
		);
		DBG_PRT_WITH(115, brn,
			if(end_trace_idx == INVALID_IDX){
				os << " nmp_trc_idx=" << the_nmp.na_to_wrt_trace_idx << "\n";
				if(the_nmp.na_candidate_qua != NULL_PT){
					os << " nmp_lv=" << the_nmp.na_candidate_qua->qlevel() << "\n";
				}
			}
		);
	}
	
	if(brn.in_root_lv() && (to_wrt_idx == INVALID_IDX)){
	}
	
	long ini_trace_idx = 0;
	ch_string ini_dir_pth = SKG_INVALID_PTH;
	if(has_prv){
		ini_trace_idx = proof_get_trace_idx_of(dct, prv_wrt_idx);
		neuromap& the_nmp = *(all_to_wrt[prv_wrt_idx]);
		BRAIN_CK(the_nmp.na_wrt_ok);
		ini_dir_pth = proof_get_nmp_proof_path(the_nmp);

		if((ini_trace_idx != INVALID_IDX) && (end_trace_idx == INVALID_IDX)){
			end_trace_idx = trace.size();
		}
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
			BRAIN_CK(dct.dt_last_found != NULL_PT);
			proof_append_neu_lits(brn, json_str, dct.dt_first_causes);
		}
		canon_string_append(json_str, "\"va_r\": ");
		proof_append_qua(json_str, NULL_PT);
		canon_string_append(json_str, "}\n");
		is_fst_ps = false;
	}
	
	if(has_prv){
		if(ini_trace_idx == INVALID_IDX){
			BRAIN_CK_PRT((end_trace_idx == INVALID_IDX) || (end_trace_idx == trace.size()), 
				DBG_PRT_ABORT(brn);
				os << " to_wrt_sz=" << all_to_wrt.size() << "\n";
				os << " end_trace_idx=" << end_trace_idx << "\n";
			);
			ini_trace_idx = trace.size();
		}
		long prv_lst_idx = ini_trace_idx - 1;
		if(trace.is_valid_idx(prv_lst_idx)){
			BRAIN_CK_PRT((trace.is_valid_idx(prv_lst_idx)), 
				DBG_PRT_ABORT(brn);
				os << " prv_lst_idx=" << prv_lst_idx << "\n";
				os << " ini_trace_idx=" << ini_trace_idx << "\n";
				os << " to_wrt_sz=" << all_to_wrt.size() << "\n";
				os << " trace_sz=" << trace.size() << "\n";
			);
			prop_signal& prv_lst = trace[prv_lst_idx];
			BRAIN_CK(ini_dir_pth != SKG_INVALID_PTH);
			proof_append_ps(brn, json_str, prv_lst, is_fst_ps, 
							ini_dir_pth, pf_tk, all_to_move);
			//BRAIN_CK(all_to_move.is_empty());
		}
	}

	if(! has_prv && (end_trace_idx == INVALID_IDX)){
		end_trace_idx = trace.size();
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
			if(trace.is_valid_idx(end_trace_idx)){
				os << " ini_trace_idx=" << trace[ini_trace_idx] << "\n";
			}
			os << " end_trace_idx=" << end_trace_idx << "\n";
			if(trace.is_valid_idx(end_trace_idx)){
				os << " end_trace_ps=" << trace[end_trace_idx] << "\n";
			}
		);
		
		// APPEND_THE_TRACE
		for(long aa = ini_trace_idx; aa < end_trace_idx; aa++){
			prop_signal& nxt_sig = trace[aa];
			
			bool is_uron = nxt_sig.is_ps_uniron();

			if(is_uron){
				DBG_PRT_WITH(115, brn, 
					ticket& sub_pf_tk = nxt_sig.ps_quanton->qu_proof_tk;
					os << " UNIRON=" << nxt_sig;
					os << " q_pf_tk=" << sub_pf_tk << "\n";
					os << " trc_sz=" << trace.size() << "\n";
					os << " end_idx=" << end_trace_idx << "\n";
					os << " pf_tk=" << pf_tk << "\n";
					os << " sub_pf_tk=" << sub_pf_tk << "\n";
				);
				proof_append_uniron(json_str, nxt_sig, is_fst_ps, 
								invalid_pth, invalid_tk, all_to_move);
			} else {
				proof_append_ps(brn, json_str, nxt_sig, is_fst_ps, 
								invalid_pth, invalid_tk, all_to_move);
			}
		}
	}
	
	canon_string_append(json_str, "]\n");
	canon_string_append(json_str, "}\n");
	
	ch_string pf_dir_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, pf_tk);
	proof_move_all_to_mv(dct, pf_dir_pth, all_to_move);

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
			os << " pk_tk=" << pf_tk << "\n";
			os << " trc_sz=" << trace.size() << "\n";
			os << " end_idx=" << end_trace_idx << "\n";
		);
		
		//proof_write_html_file_for(end_dir_pth, dct, pf_tk);
		
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

		BRAIN_CK(! file_exists(os_src_pth_mv));
		bool lk2_ok = path_create_link(os_dst_pth_mv, os_src_pth_mv);
		SKELETON_CK(lk2_ok);
		BRAIN_CK(file_exists(os_src_pth_mv));
		
		BRAIN_CK(pt_nmp != NULL_PT);
		neuromap& the_nmp = *pt_nmp;
		
		ch_string os_end_jsn_pth = proof_add_paths(os_dst_pth_mv, SKG_END_JSN_NAME);
		ch_string os_end_file_pth = proof_add_paths(os_dst_pth_mv,
													proof_get_tk_file_name(pf_tk));
	
		/*BRAIN_CK(! file_exists(os_end_jsn_pth));
		bool lk1_ok = path_create_link(os_end_file_pth, os_end_jsn_pth);
		SKELETON_CK(lk1_ok);*/
		
		proof_write_proof_json_file_for(the_nmp, os_end_jsn_pth, pf_tk);
	}
}

void
proof_write_proof_json_file_for(neuromap& nmp, ch_string os_end_jsn_pth, ticket& pf_tk)
{
	brain& brn = nmp.get_brn();
	coloring& col = nmp.na_wrt_col;
	row_neuron_t& all_neu = col.co_neus;
	
	BRAIN_CK(col.ck_cols());
	
	row<char> json_str;
	canon_string_append(json_str, "{\n");
	canon_string_append(json_str, "\t\"cnf\" : [\n");
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
	
	canon_string_append(json_str, "\t\"ben_jose_proof\" : \"");
	ch_string pf_nm = proof_get_tk_file_name(pf_tk);
	canon_string_append(json_str, pf_nm);
	canon_string_append(json_str, "\",\n");

	row_quanton_t& all_qua = col.co_quas;
	row_long_t& all_qu_col = col.co_qua_colors;
	
	canon_string_append(json_str, "\t\"vars_permutation\" : [\n");
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
	
	//row_long_t& all_ne_col = col.co_neu_colors;
	
	canon_string_append(json_str, "\t\"ccls_permutation\" : [\n");
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
	
	canon_string_append(json_str, "}\n\n");
	write_file(os_end_jsn_pth, json_str, true); //OS_OPER
	
	DBG_PRT_WITH(115, brn, os << "Wrote FINAL_PROOF_JSN=\n" << os_end_jsn_pth);
}

