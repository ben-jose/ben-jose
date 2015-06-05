

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
get_cy_htm_nm(brain& brn){
	ch_string pth = brn.br_dbg.dbg_cy_prefix + "_all_steps.htm";
	return pth;
}

ch_string
get_cy_htm_path(brain& brn){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_htm_nm(brn);
	return pth;
}

ch_string
get_cy_path(brain& brn, ch_string cy_kk, long num_step){
	ch_string pth = get_cy_dir(brn) + "/" + get_cy_rel_path(brn, cy_kk, num_step);
	return pth;
}
#endif

void
brain::dbg_start_html(){
#ifdef FULL_DEBUG
	brain& brn = *this;
	ch_string pth = get_cy_htm_path(brn);

	ch_string htm_tit = brn.br_dbg.dbg_cy_prefix + " debug output";
	
	bj_ofstream& of = br_dbg_htm_os;
	dbg_prt_open(pth, of, true);
	BRAIN_CK(of.good() && of.is_open());

	of << HTMi_html << bj_eol;
	of << "\t" << HTMi_head << bj_eol;
	of << "\t\t" << HTMi_title << htm_tit << HTMe_title << bj_eol;
	
	of << "\t\t " << HTMi_src << CY_LIB_DIR << "/cytoscape.js" << HTMe_src << bj_eol;
	of << "\t\t " << HTMi_src << CY_LIB_DIR << "/show_cnf_fn.js" << HTMe_src << bj_eol;
	
	of << "\t" << HTMe_head << bj_eol;
	
	of << "\t" << HTMi_body << bj_eol;
	
	of.flush();

	DBG_PRT(147, os << "Appended to file: '" << pth << "'\n");
#endif
}

void
brain::dbg_finish_html(){
#ifdef FULL_DEBUG
	bj_ofstream& of = br_dbg_htm_os;
	BRAIN_CK(of.good() && of.is_open());

	of << "\t" << HTMe_body << bj_eol;
	of << HTMe_html << bj_eol;
	
	of.flush();
	of.close();
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
	
	ch_string js_grph_var_pref = "nmp_grph_";
	ch_string js_plays_var_pref = "all_plays_";
	ch_string js_fn_nm = "show_cnf";

	br_dbg.dbg_cy_step++;
	
	long n_stp = br_dbg.dbg_cy_step;
	bool is_ic = (cy_kk == CY_IC_KIND);
	
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
	if(! is_ic){
		of_st << stp_js_plays_var_nm << " = [" << bj_eol;
		dbg_print_cy_nmp_node_plays(of_st);
		of_st << "];" << bj_eol;
	}

	of_st.flush();
	of_st.close();

	DBG_PRT(147, os << "Writed file: '" << stp_pth << "'\n");
	
	bj_ofstream& of = br_dbg_htm_os;
	BRAIN_CK(of.good() && of.is_open());
	
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
	of << ", " << js_plays_var_aa_nm;
	of << ");" << bj_eol;
	
	of << "\t\t " << HTMe_script << bj_eol << bj_eol << bj_eol;
	
	of.flush();
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

bj_ostream&
neuron::dbg_ne_print_col_cy_node(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "\t\t ,{ data: { id: 'n" << ne_index << "'}, ";
	os << "position:{x:" << ne_dbg_drw_x_pos << ", y:" << ne_dbg_drw_y_pos << "}, ";
	os << "classes:'neuron' }";
	os << std::endl;
#endif
	return os;
}

bj_ostream&
brain::dbg_br_print_col_cy_nodes(bj_ostream& os, bool is_ic){
#ifdef FULL_DEBUG

	os << "\t nodes: [" << bj_eol;
	
	row_quanton_t& all_cy_quas = br_all_cy_quas;
	for(long aa = 0 ; aa < all_cy_quas.size(); aa++){
		BRAIN_CK(all_cy_quas[aa] != NULL_PT);
		quanton& qua = *(all_cy_quas[aa]);
		quanton& opp = qua.opposite();
		
		long qid = qua.qu_id;
		BRAIN_CK(qid > 0);

		bool has_cy_sig = (! is_ic && (qua.qu_dbg_cy_sig != NULL_PT));
		bool has_cy_nmp = (! is_ic && (qua.qu_dbg_cy_nmp != NULL_PT));
		long qti = qua.qu_tier;
		bool is_cho = qua.is_choice();
		bool is_lrn = qua.is_learned_choice();
		bool is_mon = qua.opposite().is_mono();
		
		if(has_cy_nmp){
			is_cho =  false; 
			is_lrn =  false; 
			is_mon = false;
			
			BRAIN_CK(qua.qu_dbg_cy_nmp != NULL_PT);
			neuromap* nmp = qua.qu_dbg_cy_nmp;
			quanton* n_cho = nmp->na_orig_cho;
			BRAIN_CK((n_cho == &qua) || (n_cho == &opp));
			
			qti = nmp->na_orig_ti;
			cy_quk_t qki = nmp->na_orig_ki;
			BRAIN_CK(qki != cq_invalid);

			if(qki == cq_cho){ is_cho =  true; }
			if(qki == cq_for){ is_lrn =  true; }
			if(qki == cq_mono){ is_mon =  true; }
			
		}
		if(has_cy_sig){
			BRAIN_CK(qua.qu_dbg_cy_sig != NULL_PT);
			prop_signal* sg = qua.qu_dbg_cy_sig;
			quanton* n_cho = sg->ps_quanton;
			BRAIN_CK((n_cho == &qua) || (n_cho == &opp));
			
			qti = sg->ps_tier;
		}

		os << "\t\t ";
		if(aa != 0){ os << ","; }
		os << "{ data: { id: 'd" << qid << "'";
		if(is_ic && (qti > 0)){
			os << ", lbl: " << qti;
			os << ", lbl2: " << qid;
		}
		if(! is_ic){
			if(has_cy_sig){
				os << ", lbl: " << qti;
			}
			os << ", lbl2: " << qid;
		}
		os << "}, ";
		os << "position:{x:" << qua.qu_dbg_drw_x_pos;
		os << ", y:" << qua.qu_dbg_drw_y_pos << "}";
		os << ", classes:'dipole";
		if(is_ic || has_cy_nmp){
			if(is_cho){ os << " cho"; }
			if(is_lrn){ os << " lrn"; }
		}
		os << "' }";
		os << std::endl;
		//qua.dbg_qu_print_col_cy_node(os, (aa != 0));
	}
	row_neuron_t& all_cy_neus = br_all_cy_neus;
	for(long aa = 0 ; aa < all_cy_neus.size(); aa++){
		BRAIN_CK(all_cy_neus[aa] != NULL_PT);
		neuron& neu = *(all_cy_neus[aa]);
		neu.dbg_ne_print_col_cy_node(os);
	}

	os << "\t ]," << bj_eol;
#endif
	return os;
}

bj_ostream&	
coloring::dbg_print_col_cy_graph(bj_ostream& os, bool is_ic){
#ifdef FULL_DEBUG

	brain* pt_br = get_dbg_brn();
	if(pt_br == NULL){
		bj_out << "Cannot print col EMPTY BRN !!!\n";
		return os;
	}
	DBG_PRT(46, os << "Printing col=" << (void*)this);
	
	brain& brn = *pt_br;
	//long num_step = 0;
	//ch_string prefix = "brn_cnf_";
	
	brn.dbg_br_print_col_cy_nodes(os, is_ic);
	
	row_quanton_t& all_quas = co_quas;
	
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	set_all_note1(brn, all_quas);
	
	os << "\t edges: [" << bj_eol;
	long q_consec = 0;
	row_neuron_t& all_neus = co_neus;
	for(long aa = 0; aa < all_neus.size(); aa++){
		BRAIN_CK(all_neus[aa] != NULL_PT);
		neuron& neu = *(all_neus[aa]);
		neu.dbg_ne_print_col_cy_edge(os, q_consec);
	}
	os << "\t ]," << bj_eol;
	
	reset_all_note1(brn, all_quas);
	BRAIN_CK(brn.br_qu_tot_note1 == 0);
	
#endif
	return os;
}

bj_ostream&
quanton::dbg_qu_print_col_cy_edge(bj_ostream& os, long& consec, long neu_idx){
#ifdef FULL_DEBUG
	long var_id = qu_id;
	ch_string pole_str = "in_pos";
	if(qu_id < 0){
		var_id = -qu_id;
		pole_str = "in_neg";
	}
	BRAIN_CK(var_id > 0);
	os << "\t\t ";
	if(consec != 0){ os << ","; }
	os << "{ data: { id: 'q" << consec << "', source: 'n" << neu_idx;
	os << "', target: 'd" << var_id << "'}, classes: '" << pole_str << "' }";
	os << std::endl;
	
	consec++;
#endif
	return os;
}

bj_ostream&
neuron::dbg_ne_print_col_cy_edge(bj_ostream& os, long& consec){
#ifdef FULL_DEBUG
	for(long aa = 0; aa < ne_fibres.size(); aa++){
		BRAIN_CK(ne_fibres[aa] != NULL_PT);
		quanton& qua = *(ne_fibres[aa]);
		if(qua.has_note1()){
			qua.dbg_qu_print_col_cy_edge(os, consec, ne_index);
		}
	}
#endif
	return os;
}

void
neuromap::map_dbg_get_cy_coloring(coloring& clr){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	clr.init_coloring(&brn);

	row_quanton_t& all_quas = clr.co_quas;
	all_quas.clear();
	map_get_all_quas(all_quas);
	
	row_neuron_t& all_neus = clr.co_neus;
	all_neus.clear();
	map_get_all_neus(all_neus);
	
	clr.co_qua_colors.fill(1, all_quas.size());
	clr.co_neu_colors.fill(1, all_neus.size());
#endif
}

bj_ostream&
brain::dbg_print_htm_all_monos(bj_ostream& os){
#ifdef FULL_DEBUG
	os << "[";
	for(long aa = 0; aa < br_monos.size(); aa++){
		BRAIN_CK(br_monos[aa] != NULL_PT);
		quanton& qua = *(br_monos[aa]);
		if(aa != 0){ os << ","; };
		os << " (" << qua.qu_id << "." << qua.qu_lv_mono << ")";
	}
	os << "]";
#endif
return os;
}

void
neuromap::map_dbg_set_cy_maps(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	BRAIN_CK(brn.br_tot_cy_sigs == 0);
	BRAIN_CK(brn.br_tot_cy_nmps == 0);
	
	neuromap* nxt_nmp = this;
	while(nxt_nmp != NULL_PT){
		BRAIN_CK(nxt_nmp->na_orig_cho != NULL_PT);
		quanton* qua = nxt_nmp->na_orig_cho->get_positon();
		BRAIN_CK(qua->qu_id > 0);
		
		BRAIN_CK(qua->qu_dbg_cy_nmp == NULL_PT);
		qua->qu_dbg_cy_nmp = nxt_nmp;
		brn.br_tot_cy_nmps++;
		
		dbg_set_cy_sigs(brn, nxt_nmp->na_trail_propag);
		
		nxt_nmp = nxt_nmp->na_submap;
	}
#endif
}

// qu_dbg_cy_sig

void
neuromap::map_dbg_reset_cy_maps(){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	
	neuromap* nxt_nmp = this;
	while(nxt_nmp != NULL_PT){
		BRAIN_CK(nxt_nmp->na_orig_cho != NULL_PT);
		quanton* qua = nxt_nmp->na_orig_cho->get_positon();
		BRAIN_CK(qua->qu_id > 0);
		
		BRAIN_CK(qua->qu_dbg_cy_nmp == nxt_nmp);
		qua->qu_dbg_cy_nmp = NULL_PT;
		brn.br_tot_cy_nmps--;
		
		dbg_reset_cy_sigs(brn, nxt_nmp->na_trail_propag);
		
		nxt_nmp = nxt_nmp->na_submap;
	}
	
	BRAIN_CK(brn.br_tot_cy_nmps == 0);
	BRAIN_CK(brn.br_tot_cy_sigs == 0);
#endif
}

void
neuromap::map_dbg_update_html_file(ch_string msg){
#ifdef FULL_DEBUG
	brain& brn = get_brn();
	
	ch_string rc_str = brn.recoil().get_str();
	
	bj_ostr_stream ss_msg;
	ss_msg << HTMi_h2 << msg << HTMe_h2;
	ss_msg << "nmp=" << (void*)this << HTM_br;
	ss_msg << "#sub=" << na_dbg_num_submap << HTM_br;
	ss_msg << "all_sub=[";
	print_all_subnmp(ss_msg, true);
	ss_msg << "]";
	ss_msg << HTM_br;
	ss_msg << "BRN_recoil=" << rc_str << HTM_br;
	ss_msg << "ALL_MONOS=";
	brn.dbg_print_htm_all_monos(ss_msg);
	ss_msg << HTM_br;
	
	ch_string htm_msg = ss_msg.str();
	
	coloring full_col;
	map_dbg_get_cy_coloring(full_col);
	
	map_dbg_set_cy_maps();
	brn.dbg_update_html_cy_graph(CY_NMP_KIND, &(full_col), htm_msg);
	map_dbg_reset_cy_maps();
#endif
}

void
neuromap::map_dbg_all_sub_update_html_file(ch_string msg){
#ifdef FULL_DEBUG
	ch_string w_sub = (na_submap != NULL_PT)?(" SUB"):(" null_sub");
	ch_string msg1 = msg + w_sub;
	
	map_dbg_update_html_file(msg1);
	
	neuromap* nxt_nmp = na_submap;
	long sub_nn = 0;
	while(nxt_nmp != NULL_PT){
		sub_nn++;
		ch_string msg2 = msg + " [" + long_to_str(sub_nn) + "]";
		nxt_nmp->map_dbg_update_html_file(msg2);
		nxt_nmp = nxt_nmp->na_submap;
	}
#endif
}

void
coloring::dbg_set_brain_coloring(){
#ifdef FULL_DEBUG
	brain* pt_br = get_dbg_brn();
	if(pt_br == NULL){
		return;
	}
	brain& brn = *pt_br;
	
	k_row<quanton>& all_br_quas = brn.br_positons;
	k_row<neuron>& all_br_neus = brn.br_neurons;
	
	row_quanton_t&	all_quas = co_quas;
	row<long>&	qua_colors = co_qua_colors;

	row_neuron_t&	all_neus = co_neus;
	row<long>&	neu_colors = co_neu_colors;
	
	init_coloring(&brn);
	
	for(long aa = 0; aa < all_br_quas.size(); aa++){
		quanton& qua = all_br_quas[aa];
		quanton& opp = qua.opposite();
		
		all_quas.push(&qua);
		qua_colors.push(1);

		all_quas.push(&opp);
		qua_colors.push(1);
	}

	for(long bb = 0; bb < all_br_neus.size(); bb++){
		neuron& neu = all_br_neus[bb];
		if(! neu.is_ne_virgin()){
			all_neus.push(&neu);
			neu_colors.push(1);
		}
	}
#endif
}

void
dbg_set_cy_sigs(brain& brn, row<prop_signal>& trace){
	
	for(long ii = 0; ii < trace.size(); ii++){
		prop_signal& q_sig = trace[ii];
		BRAIN_CK(q_sig.ps_quanton != NULL_PT);
		quanton* qua = q_sig.ps_quanton->get_positon();
		BRAIN_CK(qua->qu_id > 0);
		
		BRAIN_CK(qua->qu_dbg_cy_sig == NULL_PT);
		qua->qu_dbg_cy_sig = &q_sig;
		brn.br_tot_cy_sigs++;
	}
}

void
dbg_reset_cy_sigs(brain& brn, row<prop_signal>& trace){
	
	for(long ii = 0; ii < trace.size(); ii++){
		prop_signal& q_sig = trace[ii];
		BRAIN_CK(q_sig.ps_quanton != NULL_PT);
		quanton* qua = q_sig.ps_quanton->get_positon();
		BRAIN_CK(qua->qu_id > 0);
		
		BRAIN_CK(qua->qu_dbg_cy_sig == &q_sig);
		qua->qu_dbg_cy_sig = NULL_PT;
		brn.br_tot_cy_sigs--;
	}
	
}

void
brain::dbg_print_cy_nmp_node_plays(bj_ostream& os){
#ifdef FULL_DEBUG
	bool is_fst = true;
	row_quanton_t& all_cy_quas = br_all_cy_quas;
	for(long aa = 0 ; aa < all_cy_quas.size(); aa++){
		BRAIN_CK(all_cy_quas[aa] != NULL_PT);
		quanton& qua = *(all_cy_quas[aa]);
		quanton& opp = qua.opposite();
		
		long qid = qua.qu_id;
		BRAIN_CK(qid > 0);

		bool has_cy_sig = (qua.qu_dbg_cy_sig != NULL_PT);
		if(has_cy_sig){
			prop_signal* sg = qua.qu_dbg_cy_sig;
			quanton* n_cho = sg->ps_quanton;
			BRAIN_CK((n_cho == &qua) || (n_cho == &opp));
			
			DBG_PRT(101, os << "PLAY qua=" << &qua << " sig=" << sg);
			
			bool was_neg = (n_cho == &opp);
			if(was_neg){
				os << "\t";
				if(! is_fst){ os << ","; } else { is_fst = false; }
				os << "'d" << qua.qu_id << "'" << bj_eol;
			}
		}
	}
#endif
}

