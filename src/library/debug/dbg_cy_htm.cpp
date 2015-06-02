

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

dbg_cy_htm.cpp  

 funcs to print htm of 'cytoscape.js' graphs.

--------------------------------------------------------------*/

#include "util_funcs.h"
#include "bj_stream.h"
#include "ch_string.h"
#include "brain.h"
#include "solver.h"
#include "dbg_prt.h"
#include "dbg_strings_html.h"


#ifdef FULL_DEBUG
ch_string
get_cy_name(brain& brn, ch_string cy_kk, long num_step){
	ch_string num_stp = long_to_str(num_step);
	ch_string pth_nm = brn.br_dbg.dbg_cy_prefix + cy_kk + num_stp + ".js";
	return pth_nm;
}

ch_string
get_cy_dir(brain& brn){
	ch_string htm_pth = brn.get_solver().slv_dbg2.dbg_html_out_path;
	ch_string ic_dir = htm_pth + "/" + brn.br_dbg.dbg_cy_prefix;
	return ic_dir;
}

ch_string
get_cy_rel_path(brain& brn, ch_string cy_kk, long num_step){
	ch_string the_nm = get_cy_name(brn, cy_kk, num_step);
	ch_string rel_pth = brn.br_dbg.dbg_cy_prefix + "/" + the_nm;
	return rel_pth;
}

ch_string
get_cy_htm_nm(brain& brn, ch_string cy_kk){
	ch_string pth = brn.br_dbg.dbg_cy_prefix + "_all" + cy_kk + "steps.htm";
	return pth;
}

ch_string
get_cy_htm_path(brain& brn, ch_string cy_kk){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_htm_nm(brn, cy_kk);
	return pth;
}

ch_string
get_cy_path(brain& brn, ch_string cy_kk, long num_step){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_rel_path(brn, cy_kk, num_step);
	return pth;
}
#endif

void
brain::dbg_start_html(ch_string cy_kk){
#ifdef FULL_DEBUG
	brain& brn = *this;
	ch_string pth = get_cy_htm_path(brn, cy_kk);

	ch_string js_tit_suf = "";
	if(cy_kk == CY_IC_KIND){
		js_tit_suf = " trail CNF graphs";
	}
	if(cy_kk == CY_NMP_KIND){
		js_tit_suf = " sub CNF graphs";
	}
	
	ch_string htm_tit = brn.br_dbg.dbg_cy_prefix + js_tit_suf;
	
	bj_ofstream of;
	dbg_prt_open(pth, of, true);

	of << HTMi_html << bj_eol;
	of << "\t" << HTMi_head << bj_eol;
	of << "\t\t" << HTMi_title << htm_tit << HTMe_title << bj_eol;
	
	of << "\t\t " << HTMi_src << CY_LIB_DIR << "/cytoscape.js" << HTMe_src << bj_eol;
	of << "\t\t " << HTMi_src << CY_LIB_DIR << "/show_cnf_fn.js" << HTMe_src << bj_eol;
	
	of << "\t" << HTMe_head << bj_eol;
	
	of << "\t" << HTMi_body << bj_eol;
	
	of.flush();
	of.close();

	DBG_PRT(147, os << "Appended to file: '" << pth << "'\n");
#endif
}

void
brain::dbg_finish_html(ch_string cy_kk){
#ifdef FULL_DEBUG
	brain& brn = *this;
	ch_string pth = get_cy_htm_path(brn, cy_kk);
	
	bj_ofstream of;
	dbg_prt_open(pth, of, true);

	of << "\t" << HTMe_body << bj_eol;
	of << HTMe_html << bj_eol;
	
	of.flush();
	of.close();

	DBG_PRT(147, os << "Appended to file: '" << pth << "'\n");
#endif
}

void
brain::dbg_append_html(ch_string cy_kk, ch_string htm_str){
#ifdef FULL_DEBUG
	brain& brn = *this;
	ch_string pth = get_cy_htm_path(brn, cy_kk);
	//ch_string pth_nm =  get_cy_htm_nm(brn, cy_kk);;
	
	bj_ofstream of;
	dbg_prt_open(pth, of, true);

	of << htm_str;
	
	of.flush();
	of.close();

	DBG_PRT(147, os << "Appended to file: '" << pth << "'\n");
#endif
}
	
void
brain::dbg_update_html_cy_graph(ch_string cy_kk, coloring* the_col, ch_string htm_str){
#ifdef FULL_DEBUG
	brain& brn = *this;
	
	ch_string sub_dir = get_cy_dir(brn) + "/" + brn.br_dbg.dbg_cy_prefix;
	
	bool dir_ok = file_exists(sub_dir);
	BRAIN_CK(dir_ok);
	if(! dir_ok){
		path_create(sub_dir);
		bj_out << "Created dir: '" << sub_dir << "'\n";
	}
	
	long n_stp = -1;
	ch_string js_grph_var_pref = "";
	ch_string js_plays_var_pref = "";
	ch_string js_fn_nm = "";
	ch_string js_tit_suf = "";
	bool is_ic = false;
	if(cy_kk == CY_IC_KIND){
		js_tit_suf = " trail CNF graphs";
		is_ic = true;
		js_grph_var_pref = "nmp_grph_";
		js_plays_var_pref = "all_plays_";
		js_fn_nm = "show_cnf";
		br_dbg.dbg_cy_ic_step++;
		n_stp = br_dbg.dbg_cy_ic_step;
	}
	if(cy_kk == CY_NMP_KIND){
		js_tit_suf = " sub CNF graphs";
		js_grph_var_pref = "nmp_grph_";
		js_fn_nm = "show_cnf";
		br_dbg.dbg_cy_nmp_step++;
		n_stp = br_dbg.dbg_cy_nmp_step;
	}
	
	ch_string htm_tit = brn.br_dbg.dbg_cy_prefix + js_tit_suf;
	
	BRAIN_CK(n_stp != -1);
	if(n_stp > MAX_CY_STEPS){
		return;
	}

	ch_string& layo_str = br_dbg.dbg_cy_layout;
	
	ch_string stp_str = long_to_str(n_stp);
	ch_string stp_pth = get_cy_path(brn, cy_kk, n_stp);
	ch_string stp_js_grph_var_nm = js_grph_var_pref + stp_str;
	ch_string stp_js_plays_var_nm = js_plays_var_pref + stp_str;
	
	if(the_col == NULL_PT){
		if(br_dbg_full_col.is_co_virgin()){
			br_dbg_full_col.init_coloring(&brn);
			br_dbg_full_col.dbg_set_brain_coloring();
		}
		the_col = &br_dbg_full_col;
	}
	BRAIN_CK(the_col != NULL_PT);
	BRAIN_CK(the_col->get_dbg_brn() != NULL_PT);
	
	bj_ofstream of_st;
	dbg_prt_open(stp_pth, of_st);
	
	of_st << stp_js_grph_var_nm << " = {" << bj_eol;
	the_col->dbg_print_col_cy_graph(of_st, is_ic);
	of_st << "};" << bj_eol;
	
	if(is_ic){
		of_st << stp_js_plays_var_nm << " = [" << bj_eol;
		dbg_print_cy_graph_node_plays(of_st);
		of_st << "];" << bj_eol;
	} 

	of_st.flush();
	of_st.close();

	DBG_PRT(147, os << "Writed file: '" << stp_pth << "'\n");
	
	ch_string pth = get_cy_htm_path(brn, cy_kk);
	ch_string pth_nm =  get_cy_htm_nm(brn, cy_kk);;
	
	bool htm_ok = file_exists(pth);
	BRAIN_CK(htm_ok);
	
	bj_ofstream of;
	dbg_prt_open(pth, of, true);
	
	long aa = n_stp;
	
	if(htm_str.size() > 0){
		of << "\t\t " << htm_str << bj_eol << bj_eol;
	}
	
	ch_string stp_aa_str = long_to_str(aa);
	ch_string step_title = "Graph #" + stp_aa_str + HTM_br;
	of << "\t\t " << step_title << bj_eol;

	ch_string div_nm = "cnf_graph_" + stp_aa_str;
	ch_string div_str = HTM_cy_div(div_nm);
	of << "\t\t " << div_str << bj_eol;
	
	ch_string rel_aa_pth = get_cy_rel_path(brn, cy_kk, aa);
	of << "\t\t " << HTMi_src << rel_aa_pth << HTMe_src << bj_eol;
	
	ch_string js_grph_var_aa_nm = js_grph_var_pref + stp_aa_str;
	ch_string js_plays_var_aa_nm = js_plays_var_pref + stp_aa_str;
	
	of << "\t\t " << HTMi_script << js_fn_nm << "('" << div_nm << "'";
	of << ", " << js_grph_var_aa_nm << ", " << layo_str;
	if(is_ic){
		of << ", " << js_plays_var_aa_nm;
	}
	of << ");" << bj_eol;
	
	of << "\t\t " << HTMe_script << bj_eol << bj_eol << bj_eol;
	
	of.flush();
	of.close();

	DBG_PRT(147, os << "Updated file: '" << pth << "'\n");
#endif
}

void
brain::dbg_print_cy_graph_node_plays(bj_ostream& os){
#ifdef FULL_DEBUG
	bool is_fst = true;
	for(long aa = 0; aa < br_positons.size(); aa++){
		quanton& qua = br_positons[aa];
		BRAIN_CK(qua.qu_id > 0);
		if(qua.is_neg()){
			os << "\t";
			if(! is_fst){ os << ","; } else { is_fst = false; }
			os << "'d" << qua.qu_id << "'" << bj_eol;
		}
	}
#endif
}

void
brain::dbg_br_init_all_cy_pos(){
#ifdef FULL_DEBUG
#endif
}

void
qulayers::dbg_ql_init_all_cy_pos(){
#ifdef FULL_DEBUG
	brain& brn = get_ql_brain();
	row_quanton_t& all_quas = brn.br_dbg_quly_cy_quas;
	get_all_ordered_quantons(all_quas);
	
	long lst_ti = INVALID_TIER;
	long num_ti = 0;
	for(long aa = 0; aa < all_quas.size(); aa++){
		BRAIN_CK(all_quas[aa] != NULL_PT);
		quanton& qua = *(all_quas[aa]);
		if(lst_ti != qua.qu_tier){
			lst_ti = qua.qu_tier;
			num_ti = 0;
		}
		num_ti++;
		
		long lv = qua.qlevel();
		ch_string pnt = "l" + lv;
		
		long x_pos = num_ti * 100;
		long y_pos = qua.qu_tier * 100;
		
		qua.qu_dbg_drw_x_pos = x_pos;
		qua.qu_dbg_drw_y_pos = y_pos;
	}
#endif
}

