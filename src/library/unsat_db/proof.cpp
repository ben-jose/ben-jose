

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
		if(nmp_trce_idx != INVALID_IDX){
			nmp_trce_idx++;
		}
	}
	return nmp_trce_idx;
}

void
proof_append_ps(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				ch_string& pth_pref, row<ch_string>& all_to_move)
{
	if(is_fst_ps){
		is_fst_ps = false;
	} else {
		canon_string_append(json_str, ",");
	}
			
	canon_string_append(json_str, "{");
	proof_append_neu(json_str, the_sig.ps_source, pth_pref, all_to_move);
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
proof_append_lits(row<char>& json_str, row_quanton_t& all_quas)
{
	canon_string_append(json_str, "\"neu_lits\": ");
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
proof_append_neu(row<char>& json_str, neuron* neu, ch_string& pth_pref, 
					row<ch_string>& all_to_move)
{
	if(neu == NULL_PT){
		canon_string_append(json_str, "\"neu_lits\": ");
		canon_string_append(json_str, "[],\n");
		canon_string_append(json_str, "\"neu_idx\": \"0\",");
		return;
	}
	proof_append_lits(json_str, neu->ne_fibres);
	canon_string_append(json_str, ",\n");
	
	canon_string_append(json_str, "\"neu_idx\": ");
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, neu->ne_index);
	canon_string_append(json_str, "\",\n");
	
	if(! neu->ne_original){
		canon_string_append(json_str, "\"neu_jsn\": ");
		canon_string_append(json_str, "\"");
		ch_string neu_full_dir_pth = proof_get_tk_dir_path(pth_pref, neu->ne_proof_tk); 
		ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
					proof_get_tk_file_name(neu->ne_proof_tk));
		canon_string_append(json_str, neu_full_file_pth);
		canon_string_append(json_str, "\",\n");
		
		if(pth_pref == SKG_INVALID_PTH){
			all_to_move.push(neu_full_dir_pth);
		}
		
		BRAIN_DBG(brain* pt_brn = neu->get_dbg_brn());
		BRAIN_CK(pt_brn != NULL_PT);
		DBG_PRT_WITH(115, *pt_brn, os << "LEARNED neu in=\n" << neu_full_file_pth << "\n");
	}
}

void
proof_write_all_json_files_for(deduction& dct){
	BRAIN_CK(dct.ck_dbg_srcs());
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	
	long pp = INVALID_IDX;
	long aa = 0;
	for(aa = 0; aa < all_to_wrt.size(); aa++){
		neuromap& nmp = *(all_to_wrt[aa]);
		if(nmp.na_wrt_ok){
			proof_write_json_file_for(dct, aa, pp);
			pp = aa;
		}
	}
	proof_write_json_file_for(dct, INVALID_IDX, pp);
	
	//br_charge_trail.get_qu_layer(
}

void
proof_write_html_file_for(ch_string end_dir_pth, deduction& dct){
	brain& brn = dct.get_brn();
	reason& rsn = dct.dt_rsn;
	skeleton_glb& skg = brn.get_skeleton();
	ticket& pf_tk = rsn.rs_tk;
	
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

void
proof_append_uniron(row<char>& json_str, prop_signal& the_sig, bool& is_fst_ps, 
				ch_string& pth_pref, row<ch_string>& all_to_move)
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
	ticket& pf_tk = uqu->qu_proof_tk;
	
	canon_string_append(json_str, "\"neu_lits\": [\"");
	canon_long_append(json_str, uqu->qu_id);
	canon_string_append(json_str, "\"], \n");
	
	canon_string_append(json_str, "\"neu_idx\": \"0\", \n");
	
	canon_string_append(json_str, "\"neu_jsn\": ");
	canon_string_append(json_str, "\"");
	ch_string neu_full_dir_pth = proof_get_tk_dir_path(pth_pref, pf_tk); 
	ch_string neu_full_file_pth = proof_add_paths(neu_full_dir_pth, 
				proof_get_tk_file_name(pf_tk));
	canon_string_append(json_str, neu_full_file_pth);
	canon_string_append(json_str, "\",\n");
	
	canon_string_append(json_str, "\"va_r\": ");
	proof_append_qua(json_str, uqu);
	canon_string_append(json_str, "}\n");

	if(pth_pref == SKG_INVALID_PTH){
		all_to_move.push(neu_full_dir_pth);
	}
}

/*ch_string
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
proof_write_json_file_for(deduction& dct, long to_wrt_idx, long prv_wrt_idx)
{
	brain& brn = dct.get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	
	row<prop_signal>& trace = dct.dt_all_noted;
	reason& rsn = dct.dt_rsn;
	row_neuromap_t& all_to_wrt = dct.dt_all_to_wrt;
	
	DBG_PRT_WITH(115, brn, 
		os << "=================================================================\n";
		os << "JSON_FILE_FOR wrt_idx=" << to_wrt_idx << "\n";
		brn.print_trail(os);
		os << " ALL_CONF=\n";
		brn.br_all_conflicts_found.print_row_data(os, true, "\n");
		os << " ALL_NOTED=\n";
		trace.print_row_data(os, true, "\n");
		os << " REASON=\n";
		os << rsn << "\n";
		os << "learned_unirons=" << brn.br_learned_unit_neurons << "\n";
	);
	
	bool has_prv = (prv_wrt_idx != INVALID_IDX);
	
	long end_trace_idx = trace.size() - 1;
	
	if(brn.in_root_lv()){
		end_trace_idx++;
	}
	
	ch_string end_dir_pth = SKG_INVALID_PTH;
	if(all_to_wrt.is_valid_idx(to_wrt_idx)){
		end_trace_idx = proof_get_trace_idx_of(dct, to_wrt_idx);
		neuromap& the_nmp = *(all_to_wrt[to_wrt_idx]);
		BRAIN_CK(the_nmp.nmp_is_cand());
		BRAIN_CK(the_nmp.na_wrt_ok);
		end_dir_pth = proof_get_nmp_proof_path(the_nmp);
		
		DBG_PRT_WITH(115, brn,
			os << " WRT_NMP=" << &the_nmp << "\n";
			quanton* nmp_cand_qu = the_nmp.na_candidate_qua;
			os << " CAND_QUA=" << nmp_cand_qu << "\n";
			if(nmp_cand_qu != NULL_PT){
				ticket& cand_pf_tk = nmp_cand_qu->qu_proof_tk;
				ch_string cand_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, cand_pf_tk);
				os << " CAND_QUA_pf_tk=" << cand_pf_tk << "\n";
				os << " CAND_QUA_pf_pth=" << cand_pth << "\n";
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
	
	ch_string proof_final_skl_pth = proof_get_tk_dir_path(end_dir_pth, rsn.rs_tk);
	
	row<char> json_str;	
	bool is_fst_ps = true;
	
	canon_string_append(json_str, "{\n");
	canon_string_append(json_str, "\"name\": \"" + proof_final_skl_pth + "\",\n");
	canon_string_append(json_str, "\"chain\": [\n");
	
	if(! has_prv){
		canon_string_append(json_str, "{");
		if(dct.dt_confl != NULL_PT){
			ch_string inv_pth = SKG_INVALID_PTH;
			BRAIN_CK(dct.dt_confl->ne_original);
			proof_append_neu(json_str, dct.dt_confl, inv_pth, all_to_move);
			BRAIN_CK(all_to_move.is_empty());
		} else {
			BRAIN_CK(dct.dt_last_found != NULL_PT);
			proof_append_lits(json_str, dct.dt_first_causes);
			canon_string_append(json_str, ",\n");
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
			proof_append_ps(json_str, prv_lst, is_fst_ps, ini_dir_pth, all_to_move);
			BRAIN_CK(all_to_move.is_empty());
		}
	}

	if(! has_prv && (end_trace_idx == INVALID_IDX)){
		end_trace_idx = trace.size();
	}
	
	BRAIN_CK(all_to_move.is_empty());
	
	if(end_trace_idx != INVALID_IDX){
		BRAIN_CK(ini_trace_idx != INVALID_IDX);
		
		// APPEND_THE_TRACE
		ch_string pth_pref = SKG_INVALID_PTH;
		for(long aa = ini_trace_idx; aa < end_trace_idx; aa++){
			prop_signal& nxt_sig = trace[aa];
			
			bool is_uniron = nxt_sig.is_uniron();

			if(is_uniron){
				DBG_PRT_WITH(115, brn, 
					os << " UNIRON=" << nxt_sig;
					os << " q_pf_tk=" << nxt_sig.ps_quanton->qu_proof_tk << "\n";
				);
				proof_append_uniron(json_str, nxt_sig, is_fst_ps, pth_pref, all_to_move);
			} else {
				proof_append_ps(json_str, nxt_sig, is_fst_ps, pth_pref, all_to_move);
			}
		}
	}
	
	DBG_PRT_WITH(115, brn, 
		os << " all_to_mv >>>>>\n";
		all_to_move.print_row_data(os, true, "\n");
		os << " all_to_mv <<<<<\n";
	);

	canon_string_append(json_str, "]\n");
	canon_string_append(json_str, "}\n");
	
	ch_string pf_dir_pth = proof_get_tk_dir_path(skg.kg_tmp_proof_path, rsn.rs_tk);
	
	ch_string os_dir_pth = skg.as_full_path(pf_dir_pth);
	path_create(os_dir_pth); //OS_OPER
	DBG_PRT_WITH(115, brn, os << "Created path=\n" << os_dir_pth);
	
	for(long aa = 0; aa < all_to_move.size(); aa++){
		ch_string pth_mv = all_to_move[aa];
		ch_string src_pth_mv = proof_add_paths(skg.kg_tmp_proof_path, pth_mv);
		ch_string dst_pth_mv = proof_add_paths(pf_dir_pth, pth_mv);
		ch_string os_src_pth_mv = skg.as_full_path(src_pth_mv);
		ch_string os_dst_pth_mv = skg.as_full_path(dst_pth_mv);
		
		BRAIN_CK(file_exists(skg.as_full_path(skg.kg_tmp_proof_path)));
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
		);
		DBG_PRT_WITH(115, brn, os << "MOVED1\n";
			os << " src_pth=\n" << os_src_pth_mv << "\n";
			os << " dst_pth=\n" << os_dst_pth_mv << "\n";
		);
	}
	
	ch_string os_file_pth = proof_add_paths(os_dir_pth, proof_get_tk_file_name(rsn.rs_tk));
	write_file(os_file_pth, json_str, true); //OS_OPER
	DBG_PRT_WITH(115, brn, os << "Wrote file=\n" << os_file_pth);
	
	if(end_dir_pth != SKG_INVALID_PTH){
		ch_string os_end_dir_pth = skg.as_full_path(end_dir_pth);
		path_create(os_end_dir_pth);
		DBG_PRT_WITH(115, brn, os << "Created_END_path=\n" << end_dir_pth);
		
		proof_write_html_file_for(end_dir_pth, dct);
		
		ch_string src_pth_mv = pf_dir_pth;
		ch_string dst_pth_mv = proof_get_tk_dir_path(end_dir_pth, rsn.rs_tk);
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
	}
}


