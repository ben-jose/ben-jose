

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

Copyright (C) 2011, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

test_sortor.cpp  

file for test and debugging sortor class.

--------------------------------------------------------------*/

#include "dbg_sortor.h"

std::string	elem_sor::CL_NAME = "{elem_sor}";

comparison
cmp_elem_sr(elem_sor* const& e1, elem_sor* const& e2){
	SORTER_CK(e1 != NULL_PT);
	SORTER_CK(e2 != NULL_PT);

	row_sort_id_t& ids1 = e1->es_ids;
	row_sort_id_t& ids2 = e2->es_ids;

	return cmp_sorset_trails(ids1, ids2);
}

typedef comparison (*cmp_elems_func_t)(elem_sor* const & obj1, elem_sor* const & obj2);

bool	ck_sorted_elems(row<elem_sor*>& rr_ele, cmp_elems_func_t cmp_fn){
	std::ostream& os = std::cout;

	long the_sz = rr_ele.size();
	if(the_sz == 0){ return true; }
	elem_sor* lst = rr_ele[0];
	for(row_index ii = 1; ii < the_sz; ii++){
		elem_sor* ele = rr_ele[ii];
		if((*cmp_fn)(lst, ele) > 0){
			os << "ck_sorted_elems_FAILED_with" << std::endl;
			os << "ele_1=" << *lst << std::endl;
			os << "ele_2=" << *ele << std::endl;

			os << "ids_1=" << lst->es_ids << std::endl;
			os << "ids_2=" << ele->es_ids << std::endl;
			return false;
		}
		lst = ele;
	}
	return true;
}

typedef comparison (*cmp_srs_func_t)(sorset* const & obj1, sorset* const & obj2);

bool	ck_sorted_sorsets(row<sorset*>& rr_srss, cmp_srs_func_t cmp_fn){
	std::ostream& os = std::cout;

	long the_sz = rr_srss.size();
	if(the_sz == 0){ return true; }
	sorset* lst = rr_srss[0];
	for(row_index ii = 1; ii < the_sz; ii++){
		sorset* srs = rr_srss[ii];
		if((*cmp_fn)(lst, srs) > 0){
			os << "ck_sorted_sorsets_FAILED_with" << std::endl;
			os << "srs_1=" << lst << std::endl; 	// prt_sorset
			os << "srs_2=" << srs << std::endl;   // prt_sorset
			return false;
		}
		lst = srs;
	}
	return true;
}


bool	test_nw_srt_from(long test_consec, long start_rnd, avg_stat& spd_avg, 
						 bool DEBUG_SRT_FROM = false, bool SPEED_TEST = false)
{
	std::ostream& os = std::cout;
	os << "TEST " << test_consec << "(seed=" << start_rnd << ")";
	os.flush();

	sort_glb glb1;
	//glb1.init_head_ss();
	glb1.stab_mutual_init();

	//os << "START_RND=" << start_rnd << std::endl;

	tak_mak rnd_gen(start_rnd);

	k_row<elem_sor> all_elem;

	row<op_sor> all_ops;
	if(SPEED_TEST){
		all_ops.set_cap(10000);
	}

	long num_elem = rnd_gen.gen_rand_int32_ie(1, 1000);
	all_elem.set_cap(num_elem);
	for(long aa = 0; aa <= num_elem; aa++){
		elem_sor& ele = all_elem.inc_sz();
		ele.es_id = "e=";
	}


	row<elem_sor*> tmp_rr1;
	row<elem_sor*> tmp_rr2;
	bool stop_all = false;

	std::stringstream ss_tmp;
	
	long num_lvs = rnd_gen.gen_rand_int32_ie(1, 10);
	for(long bb = 1; ((bb < num_lvs) && ! stop_all); bb++){
		glb1.sg_curr_stab_consec++;
		
		DBG_CK(glb1.sg_curr_stab_consec >= bb);
		DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);
		
		std::string bb_ss = long_to_str(bb);
		long num_reps_lv = rnd_gen.gen_rand_int32_ie(1, 10);
		for(long dd = 0; ((dd < num_reps_lv) && ! stop_all); dd++){
			DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);
			
			long num_elem_rep = rnd_gen.gen_rand_int32_ie(1, num_elem);
			for(long cc = 0; ((cc < num_elem_rep) && ! stop_all); cc++){
				DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);
				
				bool all_csecs = false;
				long elem_idx = rnd_gen.gen_rand_int32_ie(1, num_elem);
				SORTER_CK(all_elem.is_valid_idx(elem_idx));
				elem_sor& ele = all_elem[elem_idx];

				if(DEBUG_SRT_FROM){
					ss_tmp.clear();
					ss_tmp.seekg(0, std::ios::beg);
					ss_tmp.str("");
					ss_tmp.flush();

					glb1.sort_to_row_and_all_consec<elem_sor>(tmp_rr1, all_csecs);
					tmp_rr1.print_row_data(ss_tmp, true, "\n");

					if(! tmp_rr1.is_sorted(cmp_elem_sr)){
						//tmp_rr1.print_row_data(os, true, "\n");
						std::string out_ss = ss_tmp.str();
						os << out_ss;
						os << "FAILED starting" << std::endl << ele << std::endl;
						os << "START_RND=" << start_rnd << std::endl;
						stop_all = true;
						break;
					}
				}

				std::string dbg_id_start = ele.es_id;

				if(SPEED_TEST){
					op_sor& the_op = all_ops.inc_sz();
					the_op.op_elem = &ele;
					the_op.op_id = bb;
				} else {
					DBG_CK(glb1.sg_curr_stab_consec >= bb);
					DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);
					ele.es_srt_bdr.sort_from(glb1, bb);
					DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);
				}

				ele.es_id += "b" + bb_ss;
				ele.add(bb);

				std::string dbg_id_end = ele.es_id;

				if(DEBUG_SRT_FROM){
					glb1.sort_to_row_and_all_consec<elem_sor>(tmp_rr2, all_csecs);
					if(! tmp_rr2.is_sorted(cmp_elem_sr)){
						os << "STARTING" << std::endl;
						std::string out_ss = ss_tmp.str();
						os << out_ss;
						//tmp_rr1.print_row_data(os, true, "\n");
						os << std::endl << std::endl << "ENDING" << std::endl;
						tmp_rr2.print_row_data(os, true, "\n");
						os << "FAILED ending" << std::endl << ele << std::endl;
						os << "curr_id=" << bb << std::endl;
						os << "dbg_id_start=" << dbg_id_start << std::endl;
						os << "dbg_id_end=" << dbg_id_end << std::endl;
						os << "START_RND=" << start_rnd << std::endl;
						stop_all = true;
						ck_sorted_elems(tmp_rr2, cmp_elem_sr);
						break;
					}
				}
			}
		}
	}
	os << "::";
	os.flush();
	//DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);

	long num_srt_from = 0;
	double run_speed = 0;
	if(SPEED_TEST){
		double start_tm = cpu_time();

		for(long aa = 0; aa < all_ops.size(); aa++){
			op_sor& the_op = all_ops[aa];
			SORTER_CK(the_op.op_elem != NULL_PT);
			SORTER_CK(the_op.op_id > 0);
			the_op.op_elem->es_srt_bdr.sort_from(glb1, the_op.op_id);
			num_srt_from++;
		}
		double finish_tm = cpu_time();

		run_speed = num_srt_from / (finish_tm - start_tm);

		if(finite(run_speed)){
			spd_avg.add_val(run_speed);
		}
	}
	//DBG_CK(glb1.sg_curr_stab_consec >= glb1.sg_dbg_last_id);

	if(! stop_all){
		row<elem_sor*> s_rr;
		bool all_in_consec = false;
		glb1.sort_to_row_and_all_consec<elem_sor>(s_rr, all_in_consec);

		os << " #elem=" << num_elem << " #ops=" << num_srt_from << " speed=" << run_speed 
			<< " finite=" << finite(run_speed);

		//os << "SIZE=" << s_rr.size() << bj_eol;
		//s_rr.print_row_data(os, true, "\n");

		//os << "NUM_ELEM=" << num_elem << std::endl;
		//os << "START_RND=" << start_rnd << std::endl;
		//os << std::endl;

		bool finish_ok = true;

		if(! s_rr.is_sorted(cmp_elem_sr)){
			s_rr.print_row_data(os, true, "\n");
			os << " START_RND=" << start_rnd << std::endl;
			SORTER_CK(ck_sorted_elems(s_rr, cmp_elem_sr));
			finish_ok = false;
		}

		glb1.sort_to_tmp_srss();
		if(! glb1.sg_tmp_srss.is_sorted(cmp_sorsets)){
			s_rr.print_row_data(os, true, "\n");
			os << " START_RND=" << start_rnd << std::endl;
			ck_sorted_sorsets(glb1.sg_tmp_srss, cmp_sorsets);
			SORTER_CK(false);
			finish_ok = false;
		}
		//os << "FINISHED OK=" << start_rnd << std::endl;
		if(finish_ok){
			os << " finished ok";
		} else {
			os << " FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!";
			os.flush();
			abort_func(0);
		}

		//os << std::endl;

		//glb1.release_all();
		//glb1.init_head_ss();
		glb1.stab_mutual_init();
	}
	//os << "START_RND=" << start_rnd << std::endl;
	
	return ! stop_all;
}

void	test_full_srt_from(){
	std::ostream& os = std::cout;

	avg_stat speed_avg;

	row_long_t rr1;
	row_long_t rr2;

	rr1.clear();
	rr2.clear();

	SORTER_CK(rr1.equal_to(rr2));

	long start_rnd = time(0);
	tak_mak g1(start_rnd);

	long NUM_TESTS = 1000;
	//long NUM_TESTS = 1;
	long ii = 0;
	for(ii = 0; ii < NUM_TESTS; ii++){
		long start_val = g1.gen_rand_int32_ie(1, LONG_MAX);
		//long start_val = 1202716915;

		//os << "seed=" << start_val << std::endl;

		bool do_debug = false;
		bool do_speed_test = false;
		bool test_ok = test_nw_srt_from(ii, start_val, speed_avg, do_debug, do_speed_test);
		os << std::endl;

		if(! test_ok){
			os << "FAILED seed=" << start_val << std::endl;
			break;
		}
	}

	os << "average_speed = " << speed_avg << std::endl;
}

int	main(int argc, char** argv){
	MARK_USED(argc);
	MARK_USED(argv);
	std::ostream& os = std::cout;

	MEM_CTRL(mem_size tt_mem_in_u = MEM_STATS.num_bytes_in_use;)
	MEM_CTRL(MARK_USED(tt_mem_in_u));

	test_full_srt_from();

	os << "ENDING TESTS___________  ";
	DBG(os << "MEM_STATS.num_bytes_in_use = " << MEM_STATS.num_bytes_in_use);
	os << std::endl;
	os.flush();

	DBG_CK(tt_mem_in_u == MEM_STATS.num_bytes_in_use);

	//MEM_CTRL(mem_size mem_u = MEM_STATS.num_bytes_in_use;)  // not nedded. tested during
	//SUPPORT_CK(mem_u == MEM_STATS.num_bytes_in_use); // mem.cpp MEM_STATS destruction

	os << std::endl;
	os << "End of tests" << std::endl;
	//getchar();

	return 0;
}

void	test_icg_sort(long test_consec, long start_rnd){
	std::ostream& os = std::cout;
	os << "TEST " << test_consec;

	//sort_glb glb2;

	tak_mak rnd_gen(start_rnd);

	k_row<elem_sor> all_elem;

	long num_elem = rnd_gen.gen_rand_int32_ie(1, 1000);
	all_elem.set_cap(num_elem);
	for(long aa = 0; aa <= num_elem; aa++){
		elem_sor& ele = all_elem.inc_sz();
		ele.es_id = "e=";
	}


	//while(
}

