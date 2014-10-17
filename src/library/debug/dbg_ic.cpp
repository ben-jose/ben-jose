

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

dbg_ic.cpp  

funcs for inference graph printing

--------------------------------------------------------------*/

#include <fstream>

#include "support.h"
#include "brain.h"
#include "dbg_run_satex.h"
#include "config.h"

#define DBG_IC_RANKS		1
#define DBG_IC_SUBGRAPHS	2

#define DBG_PREF_IC "ic_"
#define DBG_PREF_OLD_IC "old_ic_"
//define DBG_SUFI_TLP ".tlp"
#define DBG_SUFI_DOT ".dot"
#define DBG_PREF_NEU "neus_"
#define DBG_SUFI_NEU ".myl"

// IC GRAPH PRT_FUNCS

//ch_string	dbg_name(ch_string pref, long seq, ch_string suf);

ch_string	dbg_name(ch_string pref, long seq, ch_string suf){
	bj_ostr_stream stm1;
	long ancho = 3;
	char ch_cero = '0';
	stm1 << DBG_DIR << pref;
	stm1.width(ancho);
	stm1.fill(ch_cero);
	stm1 << seq << suf;
	ch_string nm = stm1.str();
	return nm;
}

bj_ostream& 	
dbg_ic_prt_dotty_label(brain& brn, bj_ostream& os){
#ifdef FULL_DEBUG
	//stats.print_info(os, this, 0.0, true, true);
	os << "\\n \\" << bj_eol;
	//os << "i#n " << stats.num_start_neu << " ";
	//os << "#n " << stats.num_neurons << " ";
	os << "\\n \\" << bj_eol;
	os << brn.br_final_msg << " ";
	os << "recoil " << brn.recoil() << "  ";	
	//os << "excited_level " << br_excited_level << "  ";
	os << "\\n \\" << bj_eol;
	os << "Quantons: ";
	brn.print_all_quantons(os, 10, "\\n\\\n");
	os << "\\n \\" << bj_eol;

	os << "Reasons learned: ";
	os << "\\n \\" << bj_eol;
	//br_reasons.print_row_data(os, true, "\\n\\\n");
#endif
	return os;
}

bj_ostream& 	
dbg_ic_prt_dotty_file(brain& brn, bj_ostream& os, row_quanton_t& the_trail, 
							 long style){
#ifdef FULL_DEBUG
	//dbg_inst_info& dbg_info = brn.br_dbg;
	long ii;
	row_quanton_t nodes;

	os << "digraph \"ic_graph\" {" << bj_eol;
	//os << "node [style=filled,fontsize=20];" << bj_eol;
	os << "node [fontsize=20];" << bj_eol;
	os << "edge [style=bold];" << bj_eol;
	os << "graph [fontsize=20];" << bj_eol;
	os << "compound=true" << bj_eol;

	os << "subgraph cluster0 {" << bj_eol;
	os << "\"" << brn.br_file_name_in_ic << "\" [shape=box];" << bj_eol;
	os << bj_eol << bj_eol;
	os << "label = \"";
	dbg_ic_prt_dotty_label(brn, os);
	os << "\";" << bj_eol;
	os << "}" << bj_eol << bj_eol;

	for(ii = 0; ii < the_trail.size(); ii++){
		quanton* qua = the_trail[ii];
		BRAIN_CK(qua != NULL_PT);

		nodes.push(qua);

		quanton* inv = qua->qu_inverse;
		BRAIN_CK(inv != NULL_PT);
		if(inv->qu_source != NULL_PT){
			nodes.push(inv);
		}

		DBG(qua->qu_dbg_ic_trail_idx = ii);
		DBG(inv->qu_dbg_ic_trail_idx = ii);
	}
	os << bj_eol << bj_eol;

	long clus = 1;
	long q_lv_old = -1;
	long q_lv = q_lv_old;
	long lev = brn.level();

	long edg_id = 0;
	MARK_USED(edg_id);
	for(ii = 0; ii < nodes.size(); ii++){
		quanton* qua1 = nodes[ii];
		
		q_lv_old = q_lv;
		q_lv = qua1->qlevel();

		if((style == DBG_IC_SUBGRAPHS) && (q_lv_old != q_lv)){
			if(q_lv_old != -1){
				os << "} " << bj_eol;
			}
			os << "subgraph cluster" << clus << " { " << bj_eol;
			clus++;
		}

		quanton* qua = qua1;
		qua->print_ic_label(os);
		DBG_CK(qua->qlevel() <= lev);

		bool has_src = (qua->qu_source != NULL_PT);

		if(qua->qlevel() == lev){ 
			os << " [";
			if(! has_src){
				os << "shape=\"diamond\",";
			} else {
				os << "shape=\"box\",";
			}
			os << "fontcolor=\"blue\"";
			os << "]";
		} else if(! has_src){
			os << " [shape=\"diamond\"]";
		} 
		os << ";" << bj_eol;

		if(qua1->qu_charge == cg_negative){ 
			quanton* inv = qua1->qu_inverse;

			inv->print_ic_label(os);
			os << " -> ";
			qua1->print_ic_label(os);
			os << " [color=\"red\"]";
			os << ";" << bj_eol;
		}

		neuron* neu = qua1->qu_source;
		if(neu != NULL_PT){
			for(long kk = 0; kk < neu->fib_sz(); kk++){
				quanton* qua2 = (neu->ne_fibres)[kk];
				bool q2_has_src = (qua2->qu_source != NULL_PT);

				if(	(qua2->qu_charge == cg_negative) && 
					((qua1->qu_charge == cg_positive) || ! q2_has_src)	
				){ 
					qua2 = qua2->qu_inverse;
				}

				if(qua1 == qua2){ continue; }

				qua2->print_ic_label(os);
				os << " -> ";
				qua1->print_ic_label(os);

				//char* mark = "";
				ch_string mark = "";

				os << "[label=\"" << mark;
				os << neu->ne_index;
				os << mark << "\"]";
				os << ";" << bj_eol;
			}
		}		
	}
	if((style == DBG_IC_SUBGRAPHS) && (q_lv_old != -1)){
		os << "} " << bj_eol;
	}

	os << "}" << bj_eol;

#endif
	return os;
}


void	dbg_reset_ic_files(brain& brn){
#ifdef FULL_DEBUG
	dbg_inst_info& dbg_info = brn.br_dbg;
	ch_string rm_str = "rm ";
	ch_string dd_str = DBG_DIR;
	if(brn.br_dbg.dbg_ic_active){
		ch_string tg_str = "*.dot";
		ch_string o_str = rm_str + dd_str + tg_str;
		system_exec(o_str);
	}

	if(dbg_info.dbg_ic_gen_jpg){
		ch_string tg_str = "all_dot_to_jpg.bat";
		ch_string o_str = rm_str + dd_str + tg_str;
		system_exec(o_str);
	}
#endif
}

void
dbg_ic_print(brain& brn, row_quanton_t& the_trail){
#ifdef FULL_DEBUG
	dbg_inst_info& dbg_info = brn.br_dbg;
	if((dbg_info.dbg_ic_seq >= dbg_info.dbg_ic_max_seq) && (dbg_info.dbg_ic_max_seq >= 0)){
		return;
	}
	DBG_PRT(30, os << "**IC_PRT**");
	DBG_PRT(31, os << "THE BRAIN=" << bj_eol;
		brn.print_brain(os);
		os << "Type ENTER to print the IC: " << bj_eol;
		DO_GETCHAR;
	);

	std::ofstream ff;
	dbg_info.dbg_ic_seq++;

	ch_string f_suf = DBG_SUFI_DOT;
	ch_string f_nam = dbg_name(DBG_PREF_IC, dbg_info.dbg_ic_seq, f_suf);

	ff.open(f_nam.c_str());
	DBG(dbg_ic_prt_dotty_file(brn, ff, the_trail, DBG_IC_SUBGRAPHS));
	ff.close();

	if(dbg_info.dbg_ic_gen_jpg){
		//long ancho = 3;
		//char ch_cero = '0';

		bj_ostr_stream o_str;
		o_str << "echo dot -Tjpg -o ";

		ch_string jpg_suf = ".jpg";
		ch_string nm_jpg = dbg_name(DBG_PREF_IC, dbg_info.dbg_ic_seq, jpg_suf);

		o_str << nm_jpg << " " << f_nam;

		/*
		o_str << DBG_DIR << DBG_PREF_IC;
		o_str.width(ancho);
		o_str.fill(ch_cero);
		o_str << dbg_info.dbg_ic_seq << ".jpg ";

		o_str << DBG_DIR << DBG_PREF_IC;
		o_str.width(ancho);
		o_str.fill(ch_cero);
		o_str << dbg_info.dbg_ic_seq << f_suf;
		*/

		o_str << " >> " << DBG_DIR << "all_dot_to_jpg.bat";

		ch_string comm = o_str.str();
		system_exec(comm);
	}

	if(brn.recoil() == 1){
		f_suf = DBG_SUFI_NEU;
		f_nam = dbg_name(DBG_PREF_NEU, 0, f_suf);

		ff.open(f_nam.c_str());
		brn.print_all_original(ff);
		ff.close();
	}
#endif
}


