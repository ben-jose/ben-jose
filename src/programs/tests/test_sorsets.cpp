

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

test_sorsets.cpp  

functions to test sorsets.

--------------------------------------------------------------*/

#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <cstring>
#include <set>
#include <map>
#include <iterator>

#include <iostream>
#include <gmpxx.h>

//     g++ t1.cpp -lgmpxx -lgmp -o tt.exe

#include "unistd.h"
#include "limits.h"

#include "support.h"

#include "brain.h"
#include "tak_mak.h"


class elem_sor;
DECLARE_PRINT_FUNCS(elem_sor);

class elem_sor {
public:
	static
	std::string	CL_NAME;

	virtual
	std::string&	get_cls_name(){
		return elem_sor::CL_NAME;
	}

	std::string	es_id;
	sortee		es_srt_bdr;
	row_long_t	es_ids;
	row_long_t	es_sids;

	elem_sor(std::string id = "??"){
		init_elem_sor(id);
	}

	~elem_sor(){
	}

	void	init_elem_sor(std::string id = "??"){
		es_id = id;
		es_srt_bdr.so_me = this;
	}

	void	add(long id, srs_side_t sid){
		BRAIN_CK((sid == sd_before) || (sid == sd_after));
		long val_sid = -1;
		if(sid == sd_after){
			val_sid = 1;
		}
		if(es_ids.is_empty()){
			es_ids.push(id);
			es_sids.push(val_sid);
		} else {
			if(es_ids.last() != id){
				es_ids.push(id);
				es_sids.push(val_sid);
			} else {
				BRAIN_CK(es_ids.last() == id);
				es_sids.last() += val_sid;
			}
		}
	}

	std::ostream&	print_elem_sor(std::ostream& os, bool from_pt = false){
		MARK_USED(from_pt);
		BRAIN_CK(es_ids.size() == es_sids.size());
		for(long aa = 0; aa < es_ids.size(); aa++){
			long id = es_ids[aa];
			long ss = es_sids[aa];
			os << "[" << ss << "]";
			os << id;
			os << ".";
		}
		os << "  " << es_id;
		os << es_srt_bdr;
		os.flush();
		return os;
	}
};

DEFINE_PRINT_FUNCS(elem_sor);

std::string	elem_sor::CL_NAME = "{elem_sor}";

comparison
cmp_elem_sr(elem_sor* const& e1, elem_sor* const& e2){
	BRAIN_CK(e1 != NULL_PT);
	BRAIN_CK(e2 != NULL_PT);

	row_long_t& ids1 = e1->es_ids;
	row_long_t& ids2 = e2->es_ids;
	row_long_t& sids1 = e1->es_sids;
	row_long_t& sids2 = e2->es_sids;

	return cmp_sorset_trails(ids1, sids1, ids2, sids2);
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
			os << "ck_sort FAILED with" << std::endl;
			os << "ele_1=" << *lst << std::endl;
			os << "ele_2=" << *ele << std::endl;
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
			os << "ck_sort FAILED with" << std::endl;
			os << "srs_1=" << *lst << std::endl;
			os << "srs_2=" << *srs << std::endl;
			return false;
		}
		lst = srs;
	}
	return true;
}


long	test_nw_sort_from(long start_rnd, bool DEBUG_SORT_FROM = false){
	std::ostream& os = std::cout;

	sort_glb glb1;

	//os << "START_RND=" << start_rnd << std::endl;

	tak_mak rnd_gen(start_rnd);

	k_row<elem_sor> all_elem;

	long num_elem = rnd_gen.gen_rand_int32_ie(1, 100);
	all_elem.set_cap(num_elem);
	for(long aa = 0; aa <= num_elem; aa++){
		elem_sor& ele = all_elem.inc_sz();
		ele.es_id = "e=";
	}

	row<elem_sor*> tmp_rr1;
	row<elem_sor*> tmp_rr2;
	bool stop_all = false;

	std::stringstream ss_tmp;

	long num_lvs = rnd_gen.gen_rand_int32_ie(1, 30);
	for(long bb = 1; ((bb < num_lvs) && ! stop_all); bb++){
		std::string bb_ss = long_to_str(bb);
		long num_reps_lv = rnd_gen.gen_rand_int32_ie(1, 30);
		for(long dd = 0; ((dd < num_reps_lv) && ! stop_all); dd++){
			long num_elem_rep = rnd_gen.gen_rand_int32_ie(1, num_elem);
			for(long cc = 0; ((cc < num_elem_rep) && ! stop_all); cc++){
				bool all_csecs = false;
				long elem_idx = rnd_gen.gen_rand_int32_ie(1, num_elem);
				BRAIN_CK(all_elem.is_valid_idx(elem_idx));
				elem_sor& ele = all_elem[elem_idx];

				if(DEBUG_SORT_FROM){
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
				long the_sid = rnd_gen.gen_rand_int32_ie(0, 2);
				if(the_sid > 0){
					ele.es_srt_bdr.sort_from(glb1, sd_after, bb);
					ele.es_id += "a" + bb_ss;
					ele.add(bb, sd_after);
				} else {
					ele.es_srt_bdr.sort_from(glb1, sd_before, bb);
					ele.es_id += "b" + bb_ss;
					ele.add(bb, sd_before);
				}
				std::string dbg_id_end = ele.es_id;

				if(DEBUG_SORT_FROM){
					glb1.sort_to_row_and_all_consec<elem_sor>(tmp_rr2, all_csecs);
					if(! tmp_rr2.is_sorted(cmp_elem_sr)){
						os << "STARTING" << std::endl;
						std::string out_ss = ss_tmp.str();
						os << out_ss;
						//tmp_rr1.print_row_data(os, true, "\n");
						os << std::endl << std::endl << "ENDING" << std::endl;
						tmp_rr2.print_row_data(os, true, "\n");
						os << "FAILED ending" << std::endl << ele << std::endl;
						os << "the_sid=" << the_sid << std::endl;
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

	if(! stop_all){
		row<elem_sor*> s_rr;
		bool all_in_consec = false;
		glb1.sort_to_row_and_all_consec<elem_sor>(s_rr, all_in_consec);

		//s_rr.print_row_data(os, true, "\n");

		//os << "NUM_ELEM=" << num_elem << std::endl;
		//os << "START_RND=" << start_rnd << std::endl;
		os << std::endl;
		if(! s_rr.is_sorted(cmp_elem_sr)){
			s_rr.print_row_data(os, true, "\n");
			os << "START_RND=" << start_rnd << std::endl;
			BRAIN_CK(ck_sorted_elems(s_rr, cmp_elem_sr));
		}

		glb1.sort_to_tmp_srss();
		if(! glb1.sg_tmp_srss.is_sorted(cmp_sorsets)){
			s_rr.print_row_data(os, true, "\n");
			os << "START_RND=" << start_rnd << std::endl;
			ck_sorted_sorsets(glb1.sg_tmp_srss, cmp_sorsets);
			BRAIN_CK(false);
		}
		//os << "FINISHED OK=" << start_rnd << std::endl;
	}
	//os << "START_RND=" << start_rnd << std::endl;

	glb1.walk_first_separate();
	glb1.walk_first_unsort();

	return num_elem;
}

void	test_full_sort_from(){
	std::ostream& os = std::cout;

	row_long_t rr1;
	row_long_t rr2;

	rr1.clear();
	rr2.clear();

	BRAIN_CK(rr1.equal_to(rr2));

	long start_rnd = time(0);
	tak_mak g1(start_rnd);

	long ii = 0;
	for(ii = 0; ii < 100; ii++){
		long start_val = g1.gen_rand_int32_ie(1, LONG_MAX);
		//long start_val = 1202716915;
		os << "TEST " << ii << " " << start_val;
		long num_elem = test_nw_sort_from(start_val);
		os << " " << num_elem << std::endl;
	}
}

