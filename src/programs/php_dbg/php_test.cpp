

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

php_dbg.cpp  

php_dbg classes and funcs impl.

--------------------------------------------------------------*/

#include <cstdio>

#include "ben_jose.h"
#include "brain.h"
#include "solver.h"
#include "util_funcs.h"
#include "dbg_prt.h"

bool
load_php_by_hole_brain(brain& brn, long holes);

solver*
php_main(long num_h){
	ch_string root_pth = ".";
	
	solver* nw_slv = solver::create_solver();
	if(nw_slv == NULL){
		bj_out << "Cannot create solver. \n";
		return NULL_PT;
	}
	solver& the_slvr = *nw_slv;
	the_slvr.slv_skl.kg_root_path = root_pth;	

	DBG(the_slvr.slv_skl.kg_keep_skeleton = false;)
	the_slvr.slv_skl.init_paths();
	
	WF_DBG(bj_out << "NOT FIND (JUST AS DEFAULT_DBG)" << bj_eol);
	DBG_CHECK_SAVED(
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
	);
	BRAIN_CK((bj_out << "doing CKs (plain BRN_CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing BRN_CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing BRN_CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing BRN_CK_2s" << bj_eol) && true);
	
	instance_info& inst = the_slvr.slv_inst;
	
	long nxt_id = inst.ist_id;
	if(nxt_id >= 0){ nxt_id++; }

	ch_string num_h_str = long_to_str(num_h);
	
	inst.init_instance_info(false, false);
	inst.ist_file_path = "php_hole_" + num_h_str;
	inst.ist_id = nxt_id;
	
	brain the_brain(the_slvr);
	
	BRAIN_DBG(the_brain.br_dbg.dbg_cy_prefix = inst.ist_file_path);
	
	bool ld_ok = load_php_by_hole_brain(the_brain, num_h);
	if(! ld_ok){
		bj_out << "Cannot load php brain !!! \n";
		return nw_slv;
	}
	bj_satisf_val_t res = the_brain.solve_instance(false);
	
	bj_out << "res=" << res;

	return nw_slv;
}

int	main(int argc, char** argv){
	MARK_USED(argc);
	MARK_USED(argv);
	std::ostream& os = std::cout;

	os << "THIS IS THE PHP TEST PROG \n";
	
	if(argc < 2){
		os << "args: <num_holes>";
		return 0;
	}
	long num_h = atol(argv[1]); 
	
	solver* nw_slv = php_main(num_h);

	if(nw_slv != NULL_PT){
		solver::release_solver(nw_slv);
	}
	
	BRAIN_CK((bj_out << "doing CKs (plain BRN_CKs)" << bj_eol) && true);
	BRAIN_CK_0((bj_out << "doing BRN_CK_0s" << bj_eol) && true);
	BRAIN_CK_1((bj_out << "doing BRN_CK_1s" << bj_eol) && true);
	BRAIN_CK_2((bj_out << "doing BRN_CK_2s" << bj_eol) && true);
	DBG_CHECK_SAVED(
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
		bj_out << "CAREFUL RUNNING SATEX !!!!!" << bj_eol;
	);
	WF_DBG(bj_out << bj_eol << "NOT FIND (JUST AS DEFAULT_DBG)" << bj_eol);
	
	return 0;
}

long
get_num_neu(long holes){
	long num_neu = 0;
#ifdef FULL_DEBUG
	long pigeons = (holes + 1);
	num_neu = ((((holes * holes) + holes) / 2) * holes) + pigeons;
#endif
	return num_neu;
}

bool
load_php_by_hole_brain(brain& brn, long holes){
#ifdef FULL_DEBUG

	long pigeons = (holes + 1);
	long num_neu = get_num_neu(holes);
	long num_var = holes * pigeons;
	long ii, jj, kk;
	
	long bse_y = 0;
	long sep_x = 50;
	long sep_y = 40;
	long hlf_x = 25;
	long h_width = (pigeons * sep_x);

	long y_pos = 0;
	long x_pos = 0;

	//instance_info& inst_info = brn.get_my_inst();

	brn.init_loading(num_var, num_neu);

	BRAIN_CK(num_var > 0);

	brn.br_neurons.set_cap(num_neu);

	long num_neu_added = 0;
	long num_den_added = 0;

	long max_neu_sz = 0;

	brn.br_all_cy_quas.clear();
	brn.br_all_cy_neus.clear();
	
	// set pos quantons draw coor

	y_pos = bse_y;
	for(ii = 0; ii < num_var; ii++){
		x_pos = ii * sep_x;
		
		long nio_id = (ii + 1);
		quanton* qua = brn.get_quanton(nio_id);
		BRAIN_CK(qua != NULL_PT);
		BRAIN_REL_CK(qua != NULL_PT);
		BRAIN_CK(qua->qu_id == nio_id);
		
		brn.br_all_cy_quas.push(qua);
		
		qua->qu_dbg_drw_x_pos = x_pos;
		qua->qu_dbg_drw_y_pos = y_pos;
	}
	
	// add neurons 
	
	row_quanton_t& neu_quas = brn.br_tmp_load_quantons;
	neu_quas.clear();
	
	long ii_neu = 0;
	long ii_qua = 0; 
	long verif_num_neu = 0;
	for(ii = 0; ii < holes; ii++){
		for(jj = 1; jj <= pigeons; jj++){
			long var1 = (ii * pigeons) + jj;
			for(kk = jj + 1; kk <= pigeons; kk++){
				long var2 = (ii * pigeons) + kk;
				neu_quas.clear();
				
				quanton* qua1 = brn.get_quanton(-var1);
				BRAIN_CK(qua1 != NULL_PT);
				neu_quas.push(qua1);
				
				quanton* qua2 = brn.get_quanton(-var2);
				BRAIN_CK(qua2 != NULL_PT);
				neu_quas.push(qua2);
		
				num_neu_added++;
				long num_dens = neu_quas.size();

				BRAIN_CK(num_dens > 0);
				num_den_added += num_dens;

				if(num_dens > max_neu_sz){
					max_neu_sz = num_dens;
				}

				neuron& b_neu = brn.load_neuron(neu_quas);
				BRAIN_CK(b_neu.ne_index == ii_neu);
				BRAIN_CK(&(brn.br_neurons[ii_neu]) == &b_neu);
				BRAIN_CK(brn.get_out_info().bjo_result == bjr_unknown_satisf);
				
				//os << "{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
				//os << "', target: 'd" << var1 << "'}, classes: 'in_neg' }";
				//os << std::endl;
				ii_qua++;
				//os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
				//os << "', target: 'd" << var2 << "'}, classes: 'in_neg' }";
				//os << std::endl;
				ii_qua++;
					
				verif_num_neu++;
				ii_neu++;
			}
		}
	}
	
	for(jj = 1; jj <= pigeons; jj++){
		neu_quas.clear();
		for(ii = 0; ii < holes; ii++){
			long var1 = (ii * pigeons) + jj;

			quanton* qua1 = brn.get_quanton(var1);
			BRAIN_CK(qua1 != NULL_PT);
			neu_quas.push(qua1);
			
			//os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
			//os << "', target: 'd" << var1 << "'}, classes: 'in_pos' }";
			//os << std::endl;
			
			ii_qua++;
		}

		num_neu_added++;
		long num_dens = neu_quas.size();

		BRAIN_CK(num_dens > 0);
		num_den_added += num_dens;

		if(num_dens > max_neu_sz){
			max_neu_sz = num_dens;
		}
		
		neuron& b_neu = brn.load_neuron(neu_quas);
		BRAIN_CK(b_neu.ne_index == ii_neu);
		BRAIN_CK(&(brn.br_neurons[ii_neu]) == &b_neu);
		BRAIN_CK(brn.get_out_info().bjo_result == bjr_unknown_satisf);
			
		verif_num_neu++;
		ii_neu++;
	}
	
	// set neurons draw coor

	ii_neu = 0; 
	long p_x_pos = 0;
	x_pos = 0;
	y_pos = 0;
	for(ii = 0; ii < holes; ii++){
		p_x_pos = ii * h_width;
		for(jj = 1; jj <= pigeons; jj++){
			long n_y = 0;
			y_pos = bse_y;
			x_pos = p_x_pos + ((jj - 1) * sep_x);
			for(kk = jj + 1; kk <= pigeons; kk++){
				x_pos += hlf_x;
				if((n_y % 2) == 0){
					y_pos -= sep_y;
				}
				
				neuron& neu = brn.br_neurons[ii_neu];

				brn.br_all_cy_neus.push(&neu);
		
				neu.ne_dbg_drw_x_pos = x_pos;
				neu.ne_dbg_drw_y_pos = y_pos;
				
				ii_neu++;
				n_y++;
			}
		}
	}
	
	long full_width = h_width * holes;
	long x_p_sep = full_width / pigeons;
	x_pos = -(h_width / 2);
	y_pos = bse_y + (holes * sep_y);
	for(jj = 1; jj <= pigeons; jj++){
		x_pos += x_p_sep;
		
		neuron& neu = brn.br_neurons[ii_neu];
		
		brn.br_all_cy_neus.push(&neu);
				
		neu.ne_dbg_drw_x_pos = x_pos;
		neu.ne_dbg_drw_y_pos = y_pos;
		
		ii_neu++;
	}
	
	DBG_CK(verif_num_neu == num_neu);
	
	brn.init_alert_neus();
	brn.init_uncharged();

	double end_load_tm = run_time();
	double ld_tm = (end_load_tm - brn.br_start_load_tm);	
	brn.get_out_info().bjo_load_time = ld_tm;

	//ch_string f_nam = inst_info.get_f_nam();

	//brn.br_dbg_full_col.dbg_set_brain_coloring();
	brn.br_dbg.dbg_cy_layout = CY_PRESET_LAYOUT;
#endif
	return true;
}

