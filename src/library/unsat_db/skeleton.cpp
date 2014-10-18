


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

skeleton.cpp  

Classes for skeleton and directory management in canon_cnf DIMACS format.

--------------------------------------------------------------*/

#include "stack_trace.h"
#include "file_funcs.h"
#include "util_funcs.h"
#include "instance_info.h"
#include "support.h"
#include "skeleton.h"
#include "dimacs.h"

#define NUM_BYTES_SHA2	32	// 256 bits

//long canon_cnf::CF_NUM_CNFS = 0;
bj_big_int_t	skg_dbg_canon_find_id = 0;
bj_big_int_t	skg_dbg_canon_save_id = 0;

#define SKELETON_CK(prm) 	DBG_BJ_LIB_CK(prm)


bool
not_skl_path(ch_string the_pth){
	return ! path_begins_with(the_pth, SKG_SKELETON_DIR);
}

ch_string
print_hex_as_txt(row<uchar_t>& sha_rr){
	SKELETON_CK(sha_rr.size() == NUM_BYTES_SHA2);
	
	row<char> hex_str;
	sha_rr.as_hex_txt(hex_str);
	hex_str.push(0);
	
	ch_string str2(hex_str.get_c_array());
	return str2;
}

/*
ch_string
old_print_hex_as_txt(row<uchar_t>& sha_rr){
	SKELETON_CK(sha_rr.size() == NUM_BYTES_SHA2);
	bj_ostr_stream os;
	
	std::ios_base::fmtflags old_fls = os.flags();
	os.flags(std::ios::hex);
	for(int ii = 0; ii < sha_rr.size(); ii++){
		os.width(2);
		os.fill('0');
		os << (int)(sha_rr[ii]);
	}
	os.flags(old_fls);
	
	ch_string sha_txt = os.str();
	return sha_txt;
}
*/

ch_string
sha_txt_of_arr(uchar_t* to_sha, long to_sha_sz){
	row<uchar_t>	sha_rr;
	sha_rr.fill(0, NUM_BYTES_SHA2);

	uchar_t* sha_arr = (uchar_t*)(sha_rr.get_c_array());

	uchar_t* ck_arr1 = to_sha;
	MARK_USED(ck_arr1);

	sha2(to_sha, to_sha_sz, sha_arr, 0);
	SKELETON_CK(ck_arr1 == to_sha);

	ch_string sha_txt = print_hex_as_txt(sha_rr);

	SKELETON_CK((uchar_t*)(sha_rr.get_c_array()) == sha_arr);
	SKELETON_CK(sha_txt == sha_rr.as_hex_str());
	return sha_txt;
}

//============================================================
// path funcs

bool		
path_begins_with(ch_string the_pth, ch_string the_beg){
	if(the_pth.size() < the_beg.size()){ 
		return false; 
	}

	ch_string pref_str = the_pth.substr(0, the_beg.size());
	if(pref_str != the_beg){ 
		return false; 
	}
	return true;
}

bool
path_ends_with(ch_string& the_str, ch_string& the_suf){
	if(the_str.size() < the_suf.size()){
		return false;
	}
	str_pos_t pos1 = the_str.size() - the_suf.size();
	ch_string sub_str = the_str.substr(pos1);
	if(sub_str == the_suf){
		return true;
	}
	return false;
}

ch_string
path_to_absolute_path(ch_string pth){
	char rpath[BJ_PATH_MAX];

	char* rr = realpath(pth.c_str(), rpath);
	if(rr == rpath){ 
		ch_string real_pth = rpath;
		return real_pth;
	}
	return pth;
}

ch_string
nam_subset_resp(cmp_is_sub rr){
	ch_string rr_str = "?????";
	switch(rr){
		case k_lft_is_sub:
			rr_str = "k_lft_is_sub";
		break;
		case k_no_is_sub:
			rr_str = "k_no_is_sub";
		break;
		case k_rgt_is_sub:
			rr_str = "k_rgt_is_sub";
		break;
	}
	return rr_str;
}

ch_string
path_get_running_path(){
	char exepath[BJ_PATH_MAX] = {0};
	readlink("/proc/self/exe", exepath, sizeof(exepath) - 1);
	ch_string the_pth = exepath;
	return the_pth;
}

ch_string
path_get_directory(ch_string the_pth){
	long pos = (long)the_pth.rfind('/');
	ch_string the_dir = the_pth.substr(0, pos);
	return the_dir;
}

bool
dims_path_exists(ch_string base_pth, const dima_dims& dims){
	SKELETON_CK(! base_pth.empty());

	ch_string dim_pth = cnf_dims_to_path(dims);

	SKELETON_CK(! dim_pth.empty());
	SKELETON_CK(dim_pth[0] == '/');

	ch_string full_dim_pth = base_pth + dim_pth;

	bool pth_ok = file_exists(full_dim_pth);
	SKELETON_CK(! pth_ok || (full_dim_pth == path_to_absolute_path(full_dim_pth)));
	return pth_ok;
}

void
path_delete(ch_string full_pth, ch_string up_to){
	SKELETON_CK(not_skl_path(full_pth));
	ch_string sub_pth = full_pth;

	if(! path_begins_with(full_pth, up_to)){
		SKELETON_CK(false);
		return; 
	}

	delete_directory(sub_pth);
	DBG_PRT(95, os << "DEL DIR=" << sub_pth);

	str_pos_t pos_sep = 0;
	for(	pos_sep = sub_pth.rfind('/');
		(pos_sep < sub_pth.size());
		pos_sep = sub_pth.rfind('/'))
	{
		sub_pth = sub_pth.substr(0, pos_sep);
		DBG_PRT(95, os << "REM DIR=" << sub_pth);

		if(sub_pth == up_to){ break; }
		int resp = rmdir(sub_pth.c_str());
		if(resp != 0){ break; }
	}

	//ch_string the_dir = full_pth.substr(0, pos);
}

bool
path_create(ch_string n_pth){
	SKELETON_CK(not_skl_path(n_pth));
	DBG_PRT(77, os << "CREATING " << n_pth);
	DBG_PRT(77, os << "HIT RETURN TO CONTINUE...");
	DBG_COMMAND(16, getchar());

	//long resp = true;
	int eos = (int)ch_string::npos;
	int pos1 = n_pth.find('/');
	bool path_ok = true;
	while((pos1 == eos) || (pos1 < (int)n_pth.size())){
		if(pos1 == eos){
			pos1 = (int)n_pth.size();
		}

		ch_string nm_dir = n_pth.substr(0, pos1);
		if(nm_dir.size() > 0){
			path_ok = make_dir(nm_dir, 0700);
			/*
			resp = mkdir(nm_dir.c_str(), 0700);
			//path_ok = ((resp == 0) || (errno == EEXIST));
			path_ok = (resp == 0);
			*/
		}

		if((pos1 + 1) < (int)n_pth.size()){
			pos1 = n_pth.find('/', pos1 + 1);
		} else {
			pos1 = (int)n_pth.size();
		}
	}

	return path_ok;
}

void
canon_print(bj_ostream& os, row<char>& cnn){
	// this function should never be modified because the 
	// whole skeleton is based on shas of this function

	//SKELETON_CK(cnn.last() == 0);
	const char* the_str = cnn.get_c_array();
	os.write(the_str, cnn.size());
	os.flush();
}

void
canon_sha(row<char>& cnn, ch_string& sha_txt){
	long to_sha_sz = cnn.size();
	uchar_t* to_sha = (uchar_t*)(cnn.get_c_array());

	sha_txt = sha_txt_of_arr(to_sha, to_sha_sz);

	SKELETON_CK((uchar_t*)(cnn.get_c_array()) == to_sha);

	DBG_PRT(94, os << "canon sha CNN=" << bj_eol;
		os << ">>>>>>" << bj_eol;
		canon_print(os, cnn);
		os << "<<<<<<" << bj_eol;
		os << "SZ_to_SHA=" << to_sha_sz << bj_eol;
		os << "SHA=" << sha_txt;
	);
}

bool
canon_save(ch_string& the_pth, row<char>& cnn, bool write_once){
	// this function should never be modified because the 
	// whole skeleton is based on shas of this function

	DBG_PRT(78, os << "saving " << the_pth);
	DBG_PRT(78, os << "HIT RETURN TO CONTINUE..." << " dbg_id=" << skg_dbg_canon_save_id);
	DBG_COMMAND(78, getchar());

	SKELETON_CK(not_skl_path(the_pth));
	
	bool ok = write_file(the_pth, cnn, write_once);
	
	DBG_PRT(78, os << "SAVED " << the_pth << " dbg_id=" << skg_dbg_canon_save_id);
	return ok;
}

bool
canon_load(ch_string& the_pth, row<char>& cnn){
	SKELETON_CK(not_skl_path(the_pth));
	bool load_ok = false;
	try{
		read_file(the_pth, cnn);
		load_ok = true;
	} catch (const top_exception& ex1){
		load_ok = false;
	}
	return load_ok;
}

bool
canon_equal(ch_string& the_pth, row<char>& cnn){
	row<char> cnn2;
	row<char> diff1;
	bool cnn_eq = false;
	bool ld_ok = canon_load(the_pth, cnn2);
	if(ld_ok){
		cnn_eq = cnn2.equal_to_diff(cnn, diff1);
		//cnn_eq = cnn2.equal_to(cnn);
	}
	DBG_PRT_COND(92, ! cnn_eq, os << "CNN_TEST=" << bj_eol;
		os << ">>>>>" << bj_eol;	
		canon_print(os, cnn);
		os << "<<<<<" << bj_eol;	
		os << "CNN_IN_SKL=" << bj_eol;
		os << ">>>>>" << bj_eol;	
		canon_print(os, cnn2);
		os << "<<<<<" << bj_eol;	
		os << "DIFF=" << bj_eol;
		os << ">>>>>" << bj_eol;	
		canon_print(os, diff1);
		os << "<<<<<" << bj_eol;	
		os << "ld_ok=" << ld_ok << bj_eol;
		os << "cnn_eq=" << cnn_eq << bj_eol;
		os << "cnn_sz=" << cnn.size() << bj_eol;
		os << "cnn2_sz=" << cnn2.size() << bj_eol;
	);
	return cnn_eq;
}

ch_string
canon_header(ch_string& hd_str, long ccls, long vars){
	bj_ostr_stream ss_header;
	ss_header << hd_str;
	if((vars == 0) || (ccls == 0)){
		ss_header << EMPTY_CNF_COMMENT << bj_eol;
	}
	ss_header << "p cnf " << vars << " " << ccls << " " << bj_eol;
	return ss_header.str();
}

void
canon_long_append(row<char>& cnn, long nn){
	ch_string num_str = long_to_str(nn);
	canon_string_append(cnn, num_str);
}

void
canon_string_append(row<char>& cnn, ch_string ss){
	ch_string::size_type ii = 0;
	for(ii = 0; ii < ss.size(); ii++){
		cnn.push(ss[ii]);
	}
}

//============================================================
// skeleton path funcs

ch_string
canon_hash_path(const dima_dims& dims, ch_string sha_str){
	SKELETON_CK(dims.dd_tot_lits >= 0);

	ch_string sha_pth = sha_str;
	slice_str_to_path(sha_pth);

	SKELETON_CK(! sha_pth.empty());
	SKELETON_CK(sha_pth[0] == '/');
	SKELETON_CK(*(sha_pth.rbegin()) == '/');

	ch_string dim_cnf_pth = cnf_dims_to_path(dims);
	SKELETON_CK(! dim_cnf_pth.empty());
	SKELETON_CK(dim_cnf_pth[0] == '/');

	ch_string unique_str = dim_cnf_pth + sha_pth + "hh/";

	SKELETON_CK(! unique_str.empty());
	SKELETON_CK(*(unique_str.begin()) == '/');
	SKELETON_CK(*(unique_str.rbegin()) == '/');
	return unique_str;
}

/*
ch_string
canon_full_path(ch_string base_pth, ch_string upth, ch_string id_str){
	SKELETON_CK(! base_pth.empty());

	ch_string pth_ending = SKG_CANON_PATH_ENDING;
	ch_string full_pth = base_pth + upth + id_str + '/' + pth_ending + '/';

	SKELETON_CK(! full_pth.empty());
	DBG(bool ck1 = (! file_exists(full_pth) || (full_pth == (path_to_absolute_path(full_pth) + '/'))));
	SKELETON_CK(ck1);
	return full_pth;
}
*/

ch_string&
slice_str_to_path(ch_string& sha_txt){
	bj_ostr_stream ss_path;

	ss_path << '/';

	int pth_pos = 0;
	int pth_sz = 2;
	for(long ii = 0; ii < 4; ii++){
		ss_path << sha_txt.substr(pth_pos, pth_sz) << "/";
		pth_pos += pth_sz;
	}

	pth_sz = 8;
	while(pth_pos < (int)sha_txt.size()){
		ss_path << sha_txt.substr(pth_pos, pth_sz) << "/";
		pth_pos += pth_sz;
	}

	sha_txt = ss_path.str();
	return sha_txt;
}

ch_string
long_to_path(long nn, long d_per_dir){
	bj_ostr_stream ss_path;

	ch_string snum = long_to_str(nn);

	long ll = snum.length();
	ss_path << "/" << ll;

	for(long ii = 0; ii < ll; ii++){
		if((ii % d_per_dir) == 0){
			ss_path << '/';
		}
		ss_path << snum[ii];
	}

	SKELETON_CK(! ss_path.str().empty());
	SKELETON_CK((ss_path.str())[0] == '/');
	return ss_path.str();
}

bool
all_digits(ch_string& the_str){
	for(str_pos_t pp = 0; pp < the_str.size(); pp++){
		if(! isdigit(the_str[pp])){
			return false;
		}
	}
	return true;
}

ch_string
cnf_dims_to_path(const dima_dims& dims){
	long n_digs = SKG_NUM_DIGITS_IN_DIRNAM_OF_NUMBER_PATH;

	long tot_lits = dims.dd_tot_lits;
	long num_ccl = dims.dd_tot_ccls;
	long num_var = dims.dd_tot_vars;
	long num_twolits = dims.dd_tot_twolits;

	SKELETON_CK(n_digs > 0);

	SKELETON_CK(tot_lits >= 0);
	SKELETON_CK(num_ccl >= 0);
	SKELETON_CK(num_var >= 0);
	SKELETON_CK(num_twolits >= 0);

	ch_string tl_str = long_to_str(tot_lits);
	ch_string tc_str = long_to_str(num_ccl);
	ch_string tv_str = long_to_str(num_var);
	ch_string tt_str = long_to_str(num_twolits);

	ch_string pvar = long_to_path(num_var, n_digs);
	ch_string plits = long_to_path(tot_lits, n_digs);
	ch_string ptwolits = long_to_path(num_twolits, n_digs);

	ch_string pth = "/nv" + pvar + "/tt" + ptwolits + "/nc_" + tc_str + "/tl_" + tl_str + "/zz";  // 4
	return pth;
}

/*
ch_string
canon_lock_name(const dima_dims& dims, ch_string sha_str){
	long tot_lits = dims.dd_tot_lits;
	long num_ccl = dims.dd_tot_ccls;
	long num_var = dims.dd_tot_vars;
	long num_twolits = dims.dd_tot_twolits;

	SKELETON_CK(tot_lits >= 0);
	SKELETON_CK(num_ccl >= 0);
	SKELETON_CK(num_var >= 0);
	SKELETON_CK(num_twolits >= 0);

	ch_string tl_str = long_to_str(tot_lits);
	ch_string tc_str = long_to_str(num_ccl);
	ch_string tv_str = long_to_str(num_var);
	ch_string tt_str = long_to_str(num_twolits);

	ch_string XX = SKG_LOCK_SEP_STR;
	// SKG_LOCK_NAME

	ch_string the_nm = XX + tv_str + XX + tt_str + XX + tc_str + XX + tl_str + XX + sha_str + XX;
	return the_nm;
}
*/

//============================================================
// comparison funcs

comparison
cmp_canon_ids(const long& id1, const long& id2){
	long id_p1 = id1;
	long id_p2 = id2;

	SKELETON_CK(id_p1 != INVALID_QUANTON_ID);
	SKELETON_CK(id_p2 != INVALID_QUANTON_ID);

	bool in_neg1 = (id_p1 < 0);
	bool in_neg2 = (id_p2 < 0);

	long p1 = abs_long(id_p1);
	long p2 = abs_long(id_p2);

	// trl1 shorter
	if(! in_neg1 && in_neg2){ return 1; }

	// trl2 shorter
	if(in_neg1 && ! in_neg2){ return -1; }

	if(p1 < p2){ return -1; }
	if(p1 > p2){ return 1; }

	SKELETON_CK(id_p1 == id_p2);
	return 0;
}

comparison
cmp_lit_rows(row_long_t& trl1, row_long_t& trl2){
	DBG_COND_COMM(! (trl1.is_sorted(cmp_canon_ids)),
		os << "ABORTING_DATA " << bj_eol;
		os << " trl1=" << trl1 << bj_eol;
		os << " trl2=" << trl2 << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(trl1.is_sorted(cmp_canon_ids));
	DBG_COND_COMM(! (trl2.is_sorted(cmp_canon_ids)),
		os << "ABORTING_DATA " << bj_eol;
		os << " trl1=" << trl1 << bj_eol;
		os << " trl2=" << trl2 << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(trl2.is_sorted(cmp_canon_ids));

	long ii = 0;
	for(ii = 0; (ii < trl1.size()) && (ii < trl2.size()); ii++){
		long id_p1 = trl1[ii];
		long id_p2 = trl2[ii];

		comparison cmp_ids = cmp_canon_ids(id_p1, id_p2);
		if(cmp_ids != 0){
			return cmp_ids;
		}
	}

	// trl1 shorter
	if(ii < trl2.size()){ return 1; }

	// trl2 shorter
	if(ii < trl1.size()){ return -1; }

	SKELETON_CK(trl1.size() == trl2.size());
	return 0;
}

comparison
cmp_trails(row_long_t& trl1, row_long_t& trl2){
	long ii = 0;
	DBG(long lp1 = -1);
	DBG(long lp2 = -1);
	for(ii = 0; (ii < trl1.size()) && (ii < trl2.size()); ii++){
		long p1 = trl1[ii];
		long p2 = trl2[ii];

		SKELETON_CK(p1 >= 0);
		SKELETON_CK(p2 >= 0);
		SKELETON_CK((p1 == 0) || (lp1 <= p1));
		SKELETON_CK((p2 == 0) || (lp2 <= p2));
		DBG(lp1 = p1);
		DBG(lp2 = p2);

		// trl1 shorter
		if((p1 == 0) && (p2 != 0)){ return 1; }

		// trl2 shorter
		if((p1 != 0) && (p2 == 0)){ return -1; }

		if(p1 < p2){ return -1; }
		if(p1 > p2){ return 1; }
	}

	// trl1 shorter
	if(ii < trl2.size()){ return 1; }

	// trl2 shorter
	if(ii < trl1.size()){ return -1; }

	SKELETON_CK(trl1.size() == trl2.size());
	return 0;
}

comparison
cmp_clauses(canon_clause* const& ccl1, canon_clause* const& ccl2){
	SKELETON_CK(ccl1 != NULL_PT);
	SKELETON_CK(ccl2 != NULL_PT);

	row_long_t* pt_trl1 = (row_long_t*)ccl1;
	row_long_t* pt_trl2 = (row_long_t*)ccl2;

	row_long_t& trl1 = *pt_trl1;
	row_long_t& trl2 = *pt_trl2;
	SKELETON_CK(! trl1.is_empty());
	SKELETON_CK(! trl2.is_empty());

	comparison v_cc = 0;
	v_cc = cmp_lit_rows(trl1, trl2);
	DBG_PRT_COND(71, (v_cc > 0), os << "cmp ccl trails" << bj_eol
		<< "trl1" << trl1 << bj_eol
		<< "trl2" << trl2 << bj_eol
		<< "cmp_1=" << (int)v_cc;
	);

	if(v_cc == 0){
		ccl1->cc_spot = true;
		ccl2->cc_spot = true;
	}

	return v_cc;
}

comparison
cmp_variant(variant const & vnt1, variant const & vnt2){
	const bj_big_float_t& elp1 = vnt1.vn_elap.avg;
	const bj_big_float_t& elp2 = vnt2.vn_elap.avg;
	return cmp_big_floating(elp1, elp2);
}

//============================================================
// skeleton_glb

void
skeleton_glb::init_skeleton_glb(){
	kg_clauses.clear(true, true);
	kg_free_clauses.clear(true, true);;

	SKELETON_CK(kg_clauses.is_empty());
	SKELETON_CK(kg_free_clauses.is_empty());

	kg_only_save = false;
	kg_verifying = false;
	kg_local_verifying = false;

	kg_save_canon = true;
	kg_keep_skeleton = false;
	kg_find_cnn_pth = true;

	if(! kg_save_canon){
		kg_only_save = false;
		kg_verifying = false;
		kg_local_verifying = false;
	}

	kg_root_path = "";
	kg_running_path = "";	// init paths

	kg_instance_file_nam = "";

	kg_verify_path = "";
	kg_verify_mtime = time(NULL_PT);

	//init_paths(); // call it when starting
}

bj_ostream&
skeleton_glb::print_paths(bj_ostream& os){
	os << "running='" << kg_running_path << "'" << bj_eol;
	os << "root='" << kg_root_path << "'" << bj_eol;
	os << "verify='" << kg_verify_path << "'" << bj_eol;
	os << "collisions='" << kg_collisions_path << "'" << bj_eol;
	os << "missing='" << kg_missing_path << "'" << bj_eol;
	os << "corrupted='" << kg_corrupted_path << "'" << bj_eol;
	os << "overlaped='" << kg_overlaped_path << "'" << bj_eol;
	os << "dead='" << kg_dead_path << "'" << bj_eol;
	os << "broken='" << kg_broken_path << "'" << bj_eol;
	os.flush();
	return os;
}

void
skeleton_glb::report_err(ch_string pth, ch_string err_pth){
	DBG_PRT(91, os << "REPORTING " << err_pth << " " << pth);

	if(err_pth.empty() || pth.empty()){
		DBG_PRT(DBG_ALL_LVS, 
			os << "ABORTING_DATA " << bj_eol;
			os << "REPORTING " << err_pth << " " << pth << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		SKELETON_CK(false);
		return;
	}
	SKELETON_CK(ref_exists(err_pth));

	ch_string err_count_file = err_pth + '/' + SKG_ERR_COUNT_NAME;
	bj_big_int_t num_err = inc_fnum(as_full_path(err_count_file));
	if(num_err < 0){
		DBG_PRT(DBG_ALL_LVS, 
			os << "ABORTING_DATA " << bj_eol;
			os << "REPORTING " << err_pth << " " << pth << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		SKELETON_CK(false);
		return;
	}
	ch_string err_lk_nam = err_pth + "/err" + num_err.get_str() + ".skl";

	ref_write(pth, err_lk_nam);

	DBG_PRT(DBG_ALL_LVS, 
		os << "ABORTING_DATA " << bj_eol;
		os << "REPORTING " << err_pth << " " << pth << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(false);
}

void
skeleton_glb::init_paths(){
	ch_string r_pth = path_get_running_path();
	kg_running_path = path_get_directory(r_pth);
	SKELETON_CK(file_exists(kg_running_path));

	DBG_PRT(93, os << "Initing paths");

	if(kg_root_path.size() == 0){
		kg_root_path = kg_running_path;
		if(! kg_keep_skeleton){
			ch_string skl_pth = kg_root_path + SKG_SKELETON_DIR;
			delete_directory(skl_pth);
			SKELETON_CK((bj_out << "DELETING SKELETON" << bj_eol) && true);
			DBG_PRT(93, os << "DELETING SKELETON. Type return ...");
			DBG_COMMAND(93, getchar());
		}
	} else {
		if(*(kg_root_path.rbegin()) == '/'){
			long n_sz = kg_root_path.size() - 1;
			kg_root_path = kg_root_path.substr(0, n_sz);
		}
	}

	SKELETON_CK(! kg_root_path.empty());

	kg_verify_path = as_full_path(SKG_VERIFY_NAME);

	kg_collisions_path = SKG_COLLISIONS_DIR;
	kg_missing_path = SKG_MISSING_DIR;
	kg_corrupted_path = SKG_CORRUPTED_DIR;
	kg_overlaped_path = SKG_OVERLAPED_DIR;
	kg_dead_path = SKG_DEAD_DIR;
	kg_broken_path = SKG_BROKEN_DIR;

	path_create(as_full_path(kg_collisions_path));
	path_create(as_full_path(kg_missing_path));
	path_create(as_full_path(kg_corrupted_path));
	path_create(as_full_path(kg_overlaped_path));
	path_create(as_full_path(kg_dead_path));
	path_create(as_full_path(kg_broken_path));

	if(file_exists(kg_verify_path)){
		kg_verifying = true;
		kg_local_verifying = false;

		time_t tt = path_verify(kg_verify_path);
		if(tt != -1){
			kg_verify_mtime = tt;
		} else {
			kg_verifying = false;
			kg_local_verifying = false;
		}
		/*
		struct stat sf1;
		int ok1 = stat(kg_verify_path.c_str(), &sf1);
		if(ok1 == 0){
			kg_verify_mtime = sf1.st_mtime;
		} else {
			kg_verifying = false;
			kg_local_verifying = false;
		}*/
	}

	if(kg_only_save){
		kg_verifying = false;
		kg_local_verifying = false;
	}
}

//=================================================================
// canon_cnf

void
canon_clause::add_chars_to(row<char>& cnn){
	SKELETON_CK(! is_empty());
	for(long ii = 0; ii < size(); ii++){
		long var_id = pos(ii);
		SKELETON_CK(var_id != INVALID_QUANTON_ID);
		canon_long_append(cnn, var_id);
		cnn.push(' ');
	}
	cnn.push('0');
	cnn.push('\n');

	DBG_PRT(86, os << "ccl=" << *this);
}

bj_ostream&
canon_clause::print_canon_clause(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);

	return print_row_data(os, true, " ");
}

canon_clause::canon_clause(){
	DBG(cc_can_release = false);
	init_canon_clause(true);
	DBG_PRT(80, os << "CREATING ccl=" << (void*)this);
}

canon_clause::~canon_clause(){
	SKELETON_CK(cc_in_free);
	DBG_PRT(81, os << "DESTROING ccl=" << (void*)this);
}

canon_clause&
skeleton_glb::get_new_clause(){
	canon_clause* pt_ccl = NULL_PT;
	DBG(long cond_side = 0);
	//DBG_MARK_USED(cond_side);
	
	if(! kg_free_clauses.is_empty()){
		DBG(cond_side = 1);
		pt_ccl = kg_free_clauses.pop();
		DBG_PRT(82, os << "POPPED ccl=" << (void*)pt_ccl);
		SKELETON_CK(pt_ccl != NULL_PT);
		SKELETON_CK(pt_ccl->cc_in_free);
		DBG(pt_ccl->cc_in_free = false);
	} else {
		DBG(cond_side = 2);
		pt_ccl = &(kg_clauses.inc_sz());
	}
	canon_clause& ccl = *pt_ccl;

	SKELETON_CK(! ccl.cc_can_release);
	DBG(ccl.cc_can_release = true);

	DBG_COND_COMM(! (ccl.is_cc_virgin()),
		os << "ABORTING_DATA " << bj_eol;
		os << " side=" << cond_side << bj_eol;
		os << " ccl=";
		//ccl.print_row_data(os);
		os << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(ccl.is_cc_virgin());
	SKELETON_CK(! ccl.cc_in_free);

	return ccl;
}

void
skeleton_glb::release_clause(canon_clause& ccl, bool free_mem){
	SKELETON_CK(ccl.cc_can_release);
	SKELETON_CK(ccl.is_cc_virgin());
	SKELETON_CK(! ccl.cc_in_free);
	kg_free_clauses.push(&ccl);
	DBG_PRT(83, os << "pushed ccl=" << (void*)(&ccl));
	ccl.init_canon_clause(free_mem);
	DBG(ccl.cc_in_free = true);
	DBG(ccl.cc_can_release = false);
	SKELETON_CK(! ccl.cc_can_release);
}

canon_clause&
canon_cnf::add_clause(skeleton_glb& skg){
	canon_clause& ccl_1 = skg.get_new_clause();
	cf_clauses.push(&ccl_1);
	return ccl_1;
}

void
canon_clause::cc_clear(bool free_mem){
	clear(free_mem, free_mem);
}

void
canon_cnf::release_all_clauses(skeleton_glb& skg, bool free_mem){
	//SKELETON_CK(cf_filled_clauses.is_empty());
	//SKELETON_CK(cf_sample_clauses.is_empty());

	row<canon_clause*>& all_ccl = cf_clauses;
	while(! all_ccl.is_empty()){
		canon_clause& ccl = *(all_ccl.pop());

		ccl.cc_clear(free_mem);
		skg.release_clause(ccl, free_mem);
	}
	all_ccl.clear(free_mem, free_mem);
}

void
canon_cnf::add_comment_chars_to(skeleton_glb& skg, row<char>& cnn, ch_string sv_ref_pth){
	cnn.clear();

	ch_string l2 = sv_ref_pth + "\n";

	ch_string pth1 = cf_phdat.pd_ref1_nam + "\n";
	ch_string pth2 = cf_phdat.pd_ref2_nam + "\n";

	ch_string l3 = skg.kg_instance_file_nam + "\n";

	canon_string_append(cnn, "PHASE=\n");
	canon_string_append(cnn, cf_phase_str);
	canon_string_append(cnn, "ORIG_REF_PATH=\n");
	canon_string_append(cnn, l2);
	canon_string_append(cnn, "REF1=\n");
	canon_string_append(cnn, pth1);
	canon_string_append(cnn, "REF2=\n");
	canon_string_append(cnn, pth2);
	canon_string_append(cnn, "FIRST_INSTANCE=\n");
	canon_string_append(cnn, l3);
}

void
canon_cnf::add_clauses_as_chars_to(row<canon_clause*>& all_ccl, row<char>& cnn){
	cnn.clear();

	if(all_ccl.is_empty()){
		return;
	}

	ch_string cn_hd_str = SKG_CANON_HEADER_STR;

	ch_string hh_str = canon_header(cn_hd_str, all_ccl.size(), cf_dims.dd_tot_vars);
	canon_string_append(cnn, hh_str);

	DBG_PRT(70, os << "add_clauses_as_chars=" << bj_eol; all_ccl.print_row_data(os, true, "\n"));
	SKELETON_CK(! cf_sorted || all_ccl.is_sorted(cmp_clauses));

	for(long ii = 0; ii < all_ccl.size(); ii++){
		canon_clause& ccl1 = *(all_ccl[ii]);

		DBG_PRT(85, ccl1.print_canon_clause(os));
		ccl1.add_chars_to(cnn);
	}
	//cnn.push(0);
}

void
string_replace_char(ch_string& src_str, char orig, char repl){
	if(orig == repl){
		return;
	}
	for(unsigned int aa = 0; aa < src_str.size(); aa++){
		SKELETON_CK(src_str[aa] != repl);
		if(src_str[aa] == orig){
			src_str[aa] = repl;
		}
	}
}

void
canon_cnf::init_skl_paths(skeleton_glb& skg){
	row<canon_clause*>& all_ccl = cf_clauses;
	MARK_USED(all_ccl);

	SKELETON_CK(cf_dims.dd_tot_vars >= 0);
	SKELETON_CK(cf_dims.dd_tot_lits >= 0);
	SKELETON_CK(cf_dims.dd_tot_twolits >= 0);
	SKELETON_CK(cf_dims.dd_tot_ccls == all_ccl.size());
	SKELETON_CK(! cf_sha_str.empty());

	dima_dims dims2 = cf_dims;
	dima_dims dims3 = cf_dims;

	//cf_lock_nm = canon_lock_name(dims2, cf_sha_str);

	ch_string upth = canon_hash_path(dims3, cf_sha_str);
	ch_string id_str = get_id_str();

	cf_unique_path = upth + id_str + '/' + SKG_CANON_PATH_ENDING + '/';

	DBG_PRT(75, print_attrs_canon_cnf(os));
}

ch_string
canon_cnf::get_id_str(){
	row<long> lits;
	get_extreme_lits(lits);
	return lits.as_hex_str();
}

void
canon_cnf::get_extreme_lits(row<long>& lits){
	lits.clear();
	lits.fill(0, 20);

	row<canon_clause*>& all_ccl = cf_clauses;
	long aa = 0;
	long bb = 0;

	long idx_lits = 0;
	for(aa = 0; aa < all_ccl.size(); aa++){
		canon_clause& ccl = *(all_ccl[aa]);
		for(bb = 0; bb < ccl.cc_size(); bb++){
			long lit = ccl.cc_pos(bb);

			if(idx_lits == 10){ break; }
			lits[idx_lits] = lit;
			idx_lits++;
		}

		if(idx_lits == 10){ break; }
		lits[idx_lits] = 0;
		idx_lits++;
	}

	idx_lits = 19;
	for(aa = all_ccl.last_idx(); aa >= 0; aa--){
		canon_clause& ccl = *(all_ccl[aa]);
		for(bb = ccl.cc_last_idx(); bb >= 0; bb--){
			long lit = ccl.cc_pos(bb);

			if(idx_lits == 9){ break; }
			lits[idx_lits] = lit;
			idx_lits--;
		}

		if(idx_lits == 9){ break; }
		lits[idx_lits] = 0;
		idx_lits--;
	}
	SKELETON_CK(lits.size() == 20);
}

ch_string
canon_cnf::get_all_variant_dir_name(){
	ch_string core_dir = cf_phdat.vnt_nam();

	SKELETON_CK(! core_dir.empty());
	SKELETON_CK(*(core_dir.begin()) == '/');
	SKELETON_CK(*(core_dir.rbegin()) == '/');

	ch_string vnt_dir = core_dir;
	SKELETON_CK(*(vnt_dir.rbegin()) == '/');

	return vnt_dir;
}

ch_string
canon_cnf::get_variant_dir_name(long num_vnt){
	SKELETON_CK(num_vnt >= 0);
	ch_string num_str = long_to_str(num_vnt);
	ch_string vnt_dir = get_all_variant_dir_name() + num_str;
	return vnt_dir;
}

ch_string
canon_cnf::get_num_variants_name(){
	ch_string num_vnt_name = get_all_variant_dir_name() + SKG_NUM_VNT_NAME;
	return num_vnt_name;
}

bj_ostream&
canon_cnf::print_canon_cnf(bj_ostream& os, bool from_pt){
	MARK_USED(from_pt);

	row<char>& cnn = cf_chars;
	if(cnn.is_empty()){
		SKELETON_CK(cf_dims.dd_tot_ccls == cf_clauses.size());
		add_clauses_as_chars_to(cf_clauses, cnn);
	}
	canon_print(os, cnn);

	os.flush();
	return os;
}

void
canon_cnf::calc_sha_in(ch_string& sha_str){
	SKELETON_CK(sha_str.empty());

	row<char>& cnn = cf_chars;
	if(cnn.is_empty()){
		SKELETON_CK(cf_dims.dd_tot_ccls == cf_clauses.size());
		add_clauses_as_chars_to(cf_clauses, cnn);
	}

	canon_sha(cnn, sha_str);

	DBG_PRT(94, os << "calc_sha in cnf=" << (void*)this << bj_eol
		<< ">>>>>>>>" << bj_eol;
		canon_print(os, cnn);
		os << ">>>>>>>>" << bj_eol;
		os << "SHA='" << sha_str << "'" << bj_eol;
	);
}

//=================================================================
// canon search funcs

long
canon_purge_clauses(skeleton_glb& skg, row<canon_clause*>& all_ccl, long& tot_lits, long& tot_twolits){
	long num_pp = 0;

	long idx1 = 0;
	for(long ii = 0; ii < all_ccl.size(); ii++){
		if(ii > 0){
			canon_clause* ccl1 = all_ccl[idx1];
			canon_clause* ccl2 = all_ccl[ii];
			SKELETON_CK(ccl2 != NULL_PT);

			long cmp_ccl = cmp_clauses(ccl1, ccl2);
			if(cmp_ccl == 0){
				num_pp++;
				all_ccl[ii] = NULL_PT;

				SKELETON_CK(ccl2 != NULL_PT);
				bool free_cc_mem = true;
				ccl2->cc_clear(free_cc_mem);
				skg.release_clause(*ccl2, free_cc_mem);
			} else {
				idx1 = ii;
			}
		}
	}
	if(num_pp > 0){
		tot_lits = 0;
		tot_twolits = 0;
		row<canon_clause*> non_reps;
		for(long ii = 0; ii < all_ccl.size(); ii++){
			canon_clause* pt_ccl = all_ccl[ii];
			if(pt_ccl != NULL_PT){
				non_reps.push(pt_ccl);
				tot_lits += pt_ccl->cc_size();
				if(pt_ccl->cc_size() == 2){ tot_twolits++; }
			}
		}
		non_reps.move_to(all_ccl);
	}
	return num_pp;
}

long
canon_cnf::purge_clauses(skeleton_glb& skg){
	row<canon_clause*>& all_ccl = cf_clauses;
	cf_num_purged = canon_purge_clauses(skg, all_ccl, cf_dims.dd_tot_lits, cf_dims.dd_tot_twolits);
	return cf_num_purged;
}

void
skeleton_glb::start_local(){
	kg_cnf_paths_found.clear_redblack();
}

void
skeleton_glb::clear_all(){
	skeleton_glb& skg = *this;

	kg_tmp_lits.clear(true, true);
	kg_tmp_cnf.release_and_init(skg, true);	// before clear of all clauses
	kg_tmp_all_nxt_vnts.clear(true, true);
	kg_tmp_all_del_paths.clear(true, true);

	kg_clauses.clear(true, true);
	kg_free_clauses.clear(true, true);
	
	kg_cnf_paths_found.clear_redblack();

	DBG_PRT(89, os << "CLEARED free_clauses" << " stack=" << STACK_STR);

	SKELETON_CK(kg_clauses.is_empty());
	SKELETON_CK(kg_free_clauses.is_empty());
	SKELETON_CK(kg_cnf_paths_found.is_empty());
}

void
canon_cnf::fill_with(skeleton_glb& skg, row<long>& all_lits, long num_cla, long num_var){
	MARK_USED(num_cla);
	release_and_init(skg);

	DBG_PRT(90, os << "fill_with. all_lits=" << all_lits);

	long tot_lits = 0;
	long tot_twolits = 0;
	load_lits(skg, all_lits, tot_lits, tot_twolits);

	cf_dims.dd_tot_vars = num_var;
	cf_dims.dd_tot_lits = tot_lits;
	cf_dims.dd_tot_twolits = tot_twolits;

	calc_sha();
	init_skl_paths(skg);
}

ch_string
canon_cnf::calc_loader_sha_str(dimacs_loader& the_loader){
	uchar_t* arr_to_sha = (uchar_t*)(the_loader.ld_content.get_c_array());
	long arr_to_sha_sz = the_loader.ld_content.get_c_array_sz() - 1;
	
	ch_string the_sha = sha_txt_of_arr(arr_to_sha, arr_to_sha_sz);

	DBG_PRT(94, os << "calc_loader_sha str " << bj_eol << the_loader.ld_file_name << bj_eol;
		os << " CONTENT=" << bj_eol;
		os << ">>>>>>" << bj_eol;
		os << arr_to_sha;
		os << "<<<<<<" << bj_eol;
		os << "SZ_to_SHA=" << arr_to_sha_sz << bj_eol;
		os << "SHA=" << the_sha << bj_eol;
		os << "sizeof(std::istream::char_type)=" << sizeof(std::istream::char_type);
	);

	return the_sha;
}

bool
canon_cnf::load_from(skeleton_glb& skg, ch_string& f_nam){
	SKELETON_CK(not_skl_path(f_nam));
	row<long>& all_lits = skg.kg_tmp_lits;

	release_and_init(skg);
	all_lits.clear();

	bool load_ok = true;
	dimacs_loader	the_loader;
	try{
		the_loader.parse_file(f_nam, all_lits);
	} catch (const top_exception& ex1){
		load_ok = false;
	}

	if(! load_ok){
		DBG_PRT(79, os << "1. LOAD CNN FAILED f_nam=" << f_nam);
		SKELETON_CK(false);
		return false;
	}

	long tot_lits = 0;
	long tot_twolits = 0;
	load_lits(skg, all_lits, tot_lits, tot_twolits);

	SKELETON_CK(purge_clauses(skg) == 0);
	SKELETON_CK(the_loader.ld_num_ccls == cf_dims.dd_tot_ccls);
	SKELETON_CK(cf_clauses.size() == cf_dims.dd_tot_ccls);

	if(! cf_clauses.is_sorted(cmp_clauses)){
		DBG_PRT(79, os << "2. LOAD CNN FAILED f_nam=" << f_nam);
		SKELETON_CK(false);
		return false;
	}

	cf_dims.dd_tot_vars = the_loader.ld_num_vars;
	cf_dims.dd_tot_lits = the_loader.ld_tot_lits;
	cf_dims.dd_tot_twolits = the_loader.ld_tot_twolits;

	if(cf_dims.dd_tot_lits != tot_lits){
		DBG_PRT(79, os << "3. LOAD CNN FAILED f_nam=" << f_nam);
		SKELETON_CK(false);
		return false;
	}

	if(cf_dims.dd_tot_twolits != tot_twolits){
		DBG_PRT(79, os << "4. LOAD CNN FAILED f_nam=" << f_nam);
		SKELETON_CK(false);
		return false;
	}

	if(cf_dims.dd_tot_lits <= 0){
		DBG_PRT(79, os << "5. LOAD CNN FAILED f_nam=" << f_nam);
		SKELETON_CK(false);
		return false;
	}

	cf_sha_str = calc_loader_sha_str(the_loader);
	//cf_sha_str = the_loader.ld_sha_str;

	//SKELETON_CK(false);

	DBG_PRT(94, os << "BEFORE_SHA=" << bj_eol; canon_print(os, cf_chars); os << "EMPTY=" << cf_chars.is_empty());

	DBG(ch_string ck_sha_str);
	DBG(calc_sha_in(ck_sha_str));
	DBG_COND_COMM(! (cf_sha_str == ck_sha_str),
		os << "ABORTING_DATA " << bj_eol;
		os << "CLAUSES" << bj_eol;
		cf_clauses.print_row_data(os, true, "\n");
		//os << "FILLED" << bj_eol;
		//cf_filled_clauses.print_row_data(os, true, "\n");
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(cf_sha_str == ck_sha_str);

	init_skl_paths(skg);

	return true;
}

void
canon_cnf::load_lits(skeleton_glb& skg, row_long_t& all_lits, long& tot_lits, long& tot_twolits){
	tot_lits = 0;
	tot_twolits = 0;

	row<canon_clause*>& all_ccl = cf_clauses;
	SKELETON_CK(all_ccl.is_empty());

	long ii = 0;
	long first = 0;
	for(ii = 0; ii < all_lits.size(); ii++){
		long v_id = all_lits[ii];
		if(v_id == 0){
			canon_clause& ccl_1 = add_clause(skg);

			long last = ii;
			for(long jj = first; jj < last; jj++){
				SKELETON_CK(all_lits.is_valid_idx(jj));
				long n_var_id = all_lits[jj];
				SKELETON_CK(n_var_id != INVALID_QUANTON_ID);
				ccl_1.cc_push(n_var_id);
				tot_lits++;

				long n_sz = ccl_1.cc_size();
				if(n_sz == 2){ tot_twolits++; }
				if(n_sz == 3){ tot_twolits--; }
			}

			first = ii + 1;
		}
	}

	cf_dims.dd_tot_ccls = all_ccl.size();
}

// sub & super

void
canon_cnf::clear_all_spots(){
	row<canon_clause*>& the_ccls = cf_clauses;
	for(long aa = 0; aa < the_ccls.size(); aa++){
		SKELETON_CK(the_ccls[aa] != NULL_PT);
		canon_clause& ccl_1 = *(the_ccls[aa]);
		ccl_1.cc_spot = false;
	}
}

bool
canon_cnf::i_am_super_of(canon_cnf& the_cnf, bool& are_eq){
	clear_all_spots();

	row<canon_clause*>& all_my_ccls = cf_clauses;
	row<canon_clause*>& all_cnn_ccls = the_cnf.cf_clauses;

	are_eq = false;
	cmp_is_sub cmp_resp =
		all_my_ccls.sorted_set_is_subset(all_cnn_ccls, cmp_clauses, are_eq);

	bool is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));

	DBG_PRT_COND(92, (cmp_resp == k_rgt_is_sub), os << "IN " << bj_eol;
		os << " MY_CCLS=" << bj_eol;
		all_my_ccls.print_row_data(os, true, "\n");
		os << " HAS_SUB=" << bj_eol;
		all_cnn_ccls.print_row_data(os, true, "\n");
	);

	DBG_PRT_COND(96, are_eq, os << "CCLS ARE EQUAL (super_of)" << this << bj_eol);

	return is_sub;
}

bool
canon_cnf::i_am_sub_of(canon_cnf& the_cnf, bool& are_eq){
	row<canon_clause*>& all_my_ccls = cf_clauses;
	row<canon_clause*>& all_cnn_ccls = the_cnf.cf_clauses;

	are_eq = false;
	cmp_is_sub cmp_resp =
		all_cnn_ccls.sorted_set_is_subset(all_my_ccls, cmp_clauses, are_eq);

	bool is_sub = (are_eq || (cmp_resp == k_rgt_is_sub));

	DBG_PRT_COND(92, (cmp_resp == k_rgt_is_sub), os << "IN " << bj_eol;
		os << " CNF_CCLS=" << bj_eol;
		all_cnn_ccls.print_row_data(os, true, "\n");
		os << " HAS_SUB=" << bj_eol;
		all_my_ccls.print_row_data(os, true, "\n");
	);

	DBG_PRT_COND(97, are_eq, os << "CCLS ARE EQUAL (sub of)");

	return is_sub;
}

bool
canon_cnf::i_super_of_vnt(skeleton_glb& skg, ch_string& vpth){
	SKELETON_CK(cf_clauses.is_sorted(cmp_clauses));

	canon_cnf& the_cnf = skg.kg_tmp_cnf;
	SKELETON_CK(this != &(the_cnf));

	bool all_ok = false;
	ch_string the_ref = skg.ref_vnt_name(vpth, SKG_DIFF_NAME);

	if(! skg.ref_exists(the_ref)){
		skg.report_err(the_ref, skg.kg_missing_path);
		return false;
	}

	ch_string the_pth = skg.as_full_path(the_ref);
	all_ok = the_cnf.load_from(skg, the_pth);
	if(! all_ok){
		skg.report_err(the_ref, skg.kg_corrupted_path);
		return false;
	}

	bool are_eq = false;
	bool is_sub = i_am_super_of(the_cnf, are_eq);

	return is_sub;
}

bool
canon_cnf::i_sub_of_vnt(skeleton_glb& skg, ch_string& vpth, bool& are_eq){
	SKELETON_CK(cf_clauses.is_sorted(cmp_clauses));
	//SKELETON_CK(cf_filled_clauses.is_sorted(cmp_clauses));

	canon_cnf& the_cnf = skg.kg_tmp_cnf;
	SKELETON_CK(this != &(the_cnf));

	ch_string the_ref = skg.ref_vnt_name(vpth, SKG_DIFF_NAME);

	if(! skg.ref_exists(the_ref)){
		skg.report_err(the_ref, skg.kg_missing_path);
		return false;
	}

	ch_string the_pth = skg.as_full_path(the_ref);
	bool all_ok = the_cnf.load_from(skg, the_pth);
	if(! all_ok){
		skg.report_err(the_ref, skg.kg_corrupted_path);
		return false;
	}

	bool is_sub = i_am_sub_of(the_cnf, are_eq);

	return is_sub;
}

bool
skeleton_glb::find_path(ch_string pth_to_find, inst_out_info* out_info){
	if(! kg_find_cnn_pth){
		DBG_PRT(109, os << "NOT finding 1 pth=" << pth_to_find);
		return false;
	}

	if(! in_skl(pth_to_find)){
		DBG_PRT(109, os << "NOT finding 2 pth=" << pth_to_find);
		SKELETON_CK(false);
		return false; 
	}

	string_set_t& all_found = kg_cnf_paths_found;

	SKELETON_CK(! pth_to_find.empty());

	//bool found_it = (all_found.find(pth_to_find) != all_found.end());
	bool found_it = all_found.search(pth_to_find);
	if(found_it && (out_info != NULL)){
		out_info->iot_new_hits++;
	}

	if(kg_local_verifying || kg_only_save){
		DBG_PRT(109, os << "NOT finding 3 pth=" << pth_to_find);
		return found_it;
	}

	bool to_inser = false;
	if(! found_it){
		to_inser = true;
		if(! kg_verifying){
			found_it = file_exists(pth_to_find);
		} else {
			found_it = file_newer_than(pth_to_find, kg_verify_mtime);
		}
		DBG_PRT(109, os << "found_it=" << found_it << " for path " << pth_to_find);
	}
	if(found_it && to_inser && ! pth_to_find.empty()){
		DBG_PRT(98, os << "ADDING FOUND '" << pth_to_find << "'");
		all_found.push(pth_to_find);
	}

	return found_it;
}

bj_big_int_t
canon_cnf::get_num_variants(skeleton_glb& skg){
	ch_string f_nm = skg.as_full_path(get_num_variants_name());
	ch_string num_str1 = get_fstr(f_nm);
	bj_big_int_t num_vnts;
	num_vnts = num_str1;
	if(num_vnts < 0){
		num_vnts = 0;
	}
	if(num_vnts > SKG_MAX_NUM_VARIANT){
		num_vnts = 0;
	}
	return num_vnts;
}

void
canon_cnf::set_num_variants(skeleton_glb& skg, bj_big_int_t num_vnts){
	if(num_vnts < 0){
		num_vnts = 0;
	}
	if(num_vnts > SKG_MAX_NUM_VARIANT){
		num_vnts = 0;
	}
	ch_string f_nm = skg.as_full_path(get_num_variants_name());
	ch_string val_str = num_vnts.get_str();
	long wr_ok = set_fstr(f_nm, val_str);
	if(wr_ok != 0){
		unlink(f_nm.c_str());
	}
}

bool
canon_cnf::all_nxt_vnt(skeleton_glb& skg, row<variant>& all_next, row<ch_string>& all_del){
	bool num_i_sub = 0;
	long num_eq = 0;
	average avg_szs;

	DBG_PRT(107, os << "getting all_vnts in=" << skg.as_full_path(get_all_variant_dir_name()));

	all_next.clear();
	all_del.clear();

	bj_big_int_t num_vnts = get_num_variants(skg);
	for(long aa = 0; aa < num_vnts; aa++){
		ch_string vpth = get_variant_path(skg, aa);
		if(vpth == ""){
			continue;
		}

		bool are_eq = false;
		bool i_sub_it = i_sub_of_vnt(skg, vpth, are_eq);
		bool del_it = false;
		average val_elap;

		if(i_sub_it){ 
			num_i_sub++; 
			if(are_eq){ num_eq++; }
			else { del_it = true; }
		}

		if(num_i_sub == 0){
			SKELETON_CK(num_eq == 0);
			
			ch_string elp_nm = skg.ref_vnt_name(vpth, SKG_ELAPSED_NAME);
			long rd_ok = read_elapsed(skg.as_full_path(elp_nm), val_elap);
			if(rd_ok == 0){
				avg_szs.add_val(val_elap.sz);
			} else {
				del_it = true;
			}
		}

		if(! del_it){
			variant& the_vnt = all_next.inc_sz();
			the_vnt.vn_real_path = vpth;
			the_vnt.vn_elap = val_elap;
		} else {
			ch_string vdir = get_variant_dir_name(aa);
			skg.ref_remove(vdir);
			DBG_PRT(107, os << "UNLINKING=" << vdir << " num_vnts=" << num_vnts);

			ch_string& the_del_pth = all_del.inc_sz();
			the_del_pth = vpth;
		}
	}

	if(all_next.size() == SKG_MAX_NUM_VARIANT){
		long del_idx = choose_variant(all_next, avg_szs.avg);
		if(del_idx == INVALID_IDX){
			del_idx = 1;
			/*
			DBG(choose_variant(all_next, avg_szs.avg, true));
			DBG_PRT(DBG_ALL_LVS, 
				os << "ABORTING_DATA " << bj_eol;
				os << " all_next=" << all_next << bj_eol;
				os << " avg_szs=" << avg_szs << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			SKELETON_CK(false);
			*/
		}
		SKELETON_CK(all_next.is_valid_idx(del_idx));
		variant& the_vnt = all_next[del_idx];

		DBG(ch_string vpth = get_variant_path(skg, del_idx));
		SKELETON_CK(vpth == the_vnt.vn_real_path);

		ch_string vdir = get_variant_dir_name(del_idx);
		skg.ref_remove(vdir);
		DBG_PRT(107, os << "UNLINKING CHOSEN=" << vdir << " num_vnts=" << num_vnts);

		ch_string& the_del_pth = all_del.inc_sz();
		the_del_pth = the_vnt.vn_real_path;

		all_next.swapop(del_idx);
	}

	bool has_eq = (num_eq > 0);

	long nxt_sz = all_next.size();
	if(! has_eq){ nxt_sz++; }

	if(nxt_sz < num_vnts){
		for(long bb = nxt_sz; bb < num_vnts; bb++){
			ch_string vdir = get_variant_dir_name(bb);
			skg.ref_remove(vdir);
		}
	}

	all_next.mix_sort(cmp_variant);

	SKELETON_CK(all_next.size() < SKG_MAX_NUM_VARIANT);

	return has_eq;
}

long
canon_cnf::first_vnt_i_super_of(skeleton_glb& skg){
	long fst_vnt = INVALID_NATURAL;

	//for(long aa = 0; aa < SKG_MAX_NUM_VARIANT; aa++){

	bj_big_int_t num_vnts = get_num_variants(skg);
	for(long aa = 0; aa < num_vnts; aa++){
		ch_string vpth = get_variant_path(skg, aa, skg.in_verif());
		if(vpth == ""){
			break;
		}
		bool supe1 = i_super_of_vnt(skg, vpth);
		if(supe1){
			fst_vnt = aa;
			ch_string elp_nm = skg.ref_vnt_name(vpth, SKG_ELAPSED_NAME);
			update_elapsed(skg.as_full_path(elp_nm));
			break;
		}
	}
	return fst_vnt;
}

long
choose_variant(row<variant>& all_vnt, bj_big_float_t& avg_szs, bool in_dbg){
	//bj_big_int_t avg_szs = r_avg_szs;
	long ch_idx = INVALID_IDX;
	bj_big_float_t max_val_elap = INVALID_NATURAL;
	
	for(long aa = 0; aa < all_vnt.size(); aa++){
		average& nxt_avg = all_vnt[aa].vn_elap;
		bj_big_int_t& n_avg_sz = nxt_avg.sz;

		if(n_avg_sz < 0){
			DBG_PRT_COND(73, in_dbg, os << "n_avg_sz=" << n_avg_sz);
			continue;
		}
		if(n_avg_sz > avg_szs){
			DBG_PRT_COND(73, in_dbg, os << "n_avg_sz=" << n_avg_sz << " avg_szs=" << avg_szs);
			continue;
		}

		bj_big_float_t nxt_elap = nxt_avg.avg;
		
		if((ch_idx == INVALID_IDX) || (nxt_elap > max_val_elap)){
			ch_idx = aa;
			max_val_elap = nxt_elap;
		}
	}
	return ch_idx;
}

ch_string
canon_cnf::get_variant_path(skeleton_glb& skg, long num_vnt, bool skip_report){
	SKELETON_CK(skg.ref_exists(get_all_variant_dir_name()));

	ch_string vdir = get_variant_dir_name(num_vnt);
	ch_string vpth = skg.ref_read(vdir);

	SKELETON_CK(! path_begins_with(vpth, get_all_variant_dir_name()));

	if((vpth != "") && ! skg.kg_only_save && ! skg.find_path(skg.as_full_path(vpth))){
		if(! skip_report){
			skg.report_err(vdir, skg.kg_broken_path);
			if(skg.ref_in_skl(vdir)){
				skg.ref_remove(vdir);
			}
			SKELETON_CK(false);
		}
		vpth = "";
	}

	DBG(
		if((vpth != "") && skg.kg_only_save){ 
			SKELETON_CK(skg.ref_in_skl(vpth));
			SKELETON_CK(skg.ref_exists(vpth));
		}
	);

	return vpth;
}

void
canon_cnf::update_parent_variants(skeleton_glb& skg, ch_string sv_dir){
	SKELETON_CK(has_phase_path());

	DBG(string_set_t all_lnks(cmp_string));
	//DBG(string_set_t all_lnks);

	row<variant>& all_next = skg.kg_tmp_all_nxt_vnts;
	row<ch_string>& all_del = skg.kg_tmp_all_del_paths;

	bool has_eq = all_nxt_vnt(skg, all_next, all_del);

	ch_string vnt_dir = get_all_variant_dir_name();
	if(! skg.ref_in_skl(vnt_dir)){
		SKELETON_CK(false);
		return; 
	}

	SKELETON_CK(skg.ref_exists(vnt_dir));

	long aa = 0;
	for(aa = 0; aa < all_next.size(); aa++){
		variant& the_vnt = all_next[aa];
		ch_string vnt_pth = get_variant_dir_name(aa);
		ch_string lnk_pth = the_vnt.vn_real_path;

		SKELETON_CK(skg.ref_in_skl(vnt_pth));
		SKELETON_CK(skg.ref_in_skl(lnk_pth));
		SKELETON_CK(skg.ref_exists(lnk_pth));
		
		//DBG(bool in_lnks = (all_lnks.find(lnk_pth) != all_lnks.end()));
		DBG(bool in_lnks = all_lnks.search(lnk_pth));
		SKELETON_CK(! in_lnks);
		DBG(all_lnks.push(lnk_pth));

		skg.ref_remove(vnt_pth);
		skg.ref_write(lnk_pth, vnt_pth);
		DBG_PRT(107, os << "vnt_pth=" << vnt_pth << " to " << bj_eol << " lnk_pth=" << lnk_pth);
	}

	SKELETON_CK(aa == all_next.size());

	ch_string lst_pth = get_variant_dir_name(aa);

	SKELETON_CK(skg.ref_in_skl(lst_pth));
	SKELETON_CK(skg.ref_in_skl(sv_dir));
	SKELETON_CK(skg.ref_exists(sv_dir));
	SKELETON_CK(skg.ref_exists(vnt_dir));

	long nm_vnts = all_next.size();

	if(! has_eq){
		//DBG(bool in_lnks2 = (all_lnks.find(sv_dir) != all_lnks.end()));
		DBG(bool in_lnks2 = all_lnks.search(sv_dir));
		SKELETON_CK(! in_lnks2);
		DBG(all_lnks.push(sv_dir));

		skg.ref_remove(lst_pth);
		skg.ref_write(sv_dir, lst_pth);
		DBG_PRT(107, os << "lst_pth=" << lst_pth << " to " << bj_eol << " full_pth=" << sv_dir);

		SKELETON_CK(skg.ref_exists(lst_pth));

		nm_vnts++;
	}

	set_num_variants(skg, nm_vnts);

	for(aa = 0; aa < all_del.size(); aa++){
		ch_string& the_del_pth = all_del[aa];
		path_delete(skg.as_full_path(the_del_pth), skg.kg_root_path);
		DBG_PRT(107, os << "deleted=" << the_del_pth);
	}

	SKELETON_CK(ck_vnts(skg));
}

bool
canon_cnf::ck_vnts(skeleton_glb& skg){
	DBG_PRT(107, os << "checking=" << skg.as_full_path(get_all_variant_dir_name()));

	bj_big_int_t num_vnts = get_num_variants(skg);
	long aa = 0;
	for(aa = 0; aa < num_vnts; aa++){
		ch_string vpth = get_variant_path(skg, aa);
		DBG_COND_COMM(! (vpth != ""),
			os << "ABORTING_DATA case1" << bj_eol;
			os << " num_vnts=" << num_vnts << bj_eol;
			os << " aa=" << aa << bj_eol;
			os << " vpth=" << vpth << bj_eol;
			os << "END_OF_aborting_data" << bj_eol;
		);
		SKELETON_CK(vpth != "");
	}
	if(aa == num_vnts){
		for(; aa < SKG_MAX_NUM_VARIANT; aa++){
			ch_string vpth = get_variant_path(skg, aa, true);
			DBG_COND_COMM(! (vpth == ""),
				os << "ABORTING_DATA case2" << bj_eol;
				os << " num_vnts=" << num_vnts << bj_eol;
				os << " aa=" << aa << bj_eol;
				os << " vpth=" << vpth << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			SKELETON_CK(vpth == "");
		}
	}
	
	return true;
}

void
canon_count_tots(row<canon_clause*>& all_ccls, long& tot_vars, long& tot_lits, long& tot_twolits){
	tot_vars = 0; 
	tot_lits = 0;
	tot_twolits = 0;

	for(long aa = 0; aa < all_ccls.size(); aa++){
		canon_clause& the_ccl = *(all_ccls[aa]);
		long ccl_sz = the_ccl.cc_size();
		tot_lits += ccl_sz;
		if(ccl_sz == 2){ tot_twolits++; }
		for(long bb = 0; bb < the_ccl.cc_size(); bb++){
			long lit = the_ccl.cc_pos(bb);
			long vv = abs_long(lit);
			SKELETON_CK(vv != 0);
			if(vv > tot_vars){
				tot_vars = vv;
			}
		}
	}
}

bool
skeleton_glb::ref_exists(ch_string a_ref){
	ch_string f_pth = as_full_path(a_ref);
	return file_exists(f_pth);
}

bool
skeleton_glb::ref_create(ch_string a_ref){
	ch_string f_pth = as_full_path(a_ref);
	DBG_PRT(74, os << "ref_create '" << a_ref << "'");
	return path_create(f_pth);
}

bool
skeleton_glb::ref_touch(ch_string a_ref){
	ch_string f_pth = as_full_path(a_ref);
	return file_touch(f_pth);
}
	
bool
canon_cnf::save_cnf(skeleton_glb& skg, ch_string sv_pth){
	ch_string sv_dir = path_get_directory(sv_pth) + '/';
	
	if(! skg.ref_in_skl(sv_dir)){
		return false;
	}

	DBG_PRT(76, os << "before save_cnf'" << sv_pth << "'" << bj_eol 
		<< cf_sha_str << bj_eol 
		<< cf_phdat << bj_eol 
		<< cf_dbg_shas
	);

	if(! skg.kg_save_canon){
		return false;
	}

	SKELETON_CK(skg.kg_running_path != "");
	SKELETON_CK(skg.kg_root_path != "");

	// preparing

	//DBG_PRT(76, os << "'" << sv_dir << "'");

	if(cf_chars.is_empty()){
		SKELETON_CK(cf_dims.dd_tot_ccls == cf_clauses.size());
		add_clauses_as_chars_to(cf_clauses, cf_chars);
	}

	bool has_phases = has_phase_path();

	if(has_phases){
		add_comment_chars_to(skg, cf_comment_chars, sv_dir);
	}

	DBG(
		ch_string sha_verif;
		canon_sha(cf_chars, sha_verif);
	);
	SKELETON_CK(sha_verif == cf_sha_str);

	// saving

	if(! skg.ref_exists(sv_dir)){
		skg.ref_create(sv_dir);
	}

	ch_string pth1 = cf_phdat.pd_ref1_nam;
	ch_string pth2 = cf_phdat.pd_ref2_nam;

	ch_string sv_name = skg.as_full_path(sv_pth);
	DBG(bool existed = false);
	bool sv_ok = canon_save(sv_name, cf_chars);
	if(! sv_ok){
		sv_ok = canon_equal(sv_name, cf_chars);
		DBG(existed = sv_ok);
		if(! sv_ok){
			skg.report_err(sv_pth, skg.kg_collisions_path);
		}
		//DBG_PRT_COND(106, sv_ok, os << "SAVING_EQU");
		DBG(
			if(existed && has_phases){
				DBG_COND_COMM(! ((pth1 == "") || skg.ref_exists(pth1)),
					os << "ABORTING_DATA " << bj_eol;
					os << " cnf=" << this << bj_eol;
					os << " cf_phdat=" << cf_phdat << bj_eol;
					os << " sv_name=" << sv_name << bj_eol;
					os << "END_OF_aborting_data" << bj_eol;
				);
				SKELETON_CK((pth1 == "") || skg.ref_exists(pth1));
				SKELETON_CK((pth2 == "") || skg.ref_exists(pth2));

				SKELETON_CK((pth1 == "") || skg.find_path(skg.as_full_path(pth1)));
				SKELETON_CK((pth2 == "") || skg.find_path(skg.as_full_path(pth2)));
			}
		);
	}

	if(has_phases && sv_ok){
		SKELETON_CK(pth1 != sv_dir);
		SKELETON_CK(pth2 != sv_dir);

		if((pth1 != "") && ! skg.ref_exists(pth1)){
			skg.ref_create(pth1);
		}
		if((pth2 != "") && ! skg.ref_exists(pth2)){
			skg.ref_create(pth2);
		}

		SKELETON_CK((pth1 == "") || skg.ref_exists(pth1));
		SKELETON_CK((pth2 == "") || skg.ref_exists(pth2));

		SKELETON_CK((pth1 == "") || skg.find_path(skg.as_full_path(pth1)));
		SKELETON_CK((pth2 == "") || skg.find_path(skg.as_full_path(pth2)));

		ch_string comm_name = skg.as_full_path(skg.ref_vnt_name(sv_dir, SKG_COMMENT_NAME));
		canon_save(comm_name, cf_comment_chars);

		ch_string elp_nm = skg.as_full_path(skg.ref_vnt_name(sv_dir, SKG_ELAPSED_NAME));
		update_elapsed(elp_nm);

		update_parent_variants(skg, sv_dir);
	}

	if(skg.kg_local_verifying && sv_ok){
		if(pth1 != ""){ skg.kg_cnf_paths_found.push(skg.as_full_path(pth1)); }
		if(pth2 != ""){ skg.kg_cnf_paths_found.push(skg.as_full_path(pth2)); }
		//if(pth3 != ""){ skg.kg_cnf_paths_found.push(skg.as_full_path(pth3)); }
		if(sv_dir != ""){ skg.kg_cnf_paths_found.push(skg.as_full_path(sv_dir)); }
	}

	if(skg.kg_verifying && sv_ok){ 
		skg.ref_touch(pth1);
		skg.ref_touch(pth2);
		//skg.ref_touch(pth3);
		skg.ref_touch(sv_dir);
	}

	DBG_PRT(76, os << "AFTER save_cnf'" << sv_pth << "' ok=" << sv_ok << " existed=" << existed
		<< " dbg_id=" << skg_dbg_canon_save_id << bj_eol << bj_eol);
	//DBG_PRT(76, os << "HIT RETURN TO CONTINUE..." << " dbg_id=" << skg_dbg_canon_save_id);
	//DBG_COMMAND(76, getchar());

	return sv_ok;
}

int
skeleton_glb::get_write_lock(ch_string lk_dir){
	if(! ref_in_skl(lk_dir)){
		SKELETON_CK(false);		
		return -1;
	}

	if(! ref_exists(lk_dir)){
		ref_create(lk_dir);
	}

	ch_string lk_nm = lk_dir + BJ_LOCK_NAME;
	ch_string full_nm = as_full_path(lk_nm);

	DBG_PRT(72, os << "GETTING LOCK '" << full_nm << "'");

	int fd_lock = get_file_write_lock(full_nm);
	if(fd_lock == -1){
		if(path_is_dead_lock(full_nm)){
			report_err(lk_nm, kg_dead_path);
		}
		SKELETON_CK(false);
	}
	return fd_lock;
}

void
skeleton_glb::drop_write_lock(ch_string lk_dir, int fd_lock){
	drop_file_write_lock(fd_lock);

	ch_string lk_nm = lk_dir + BJ_LOCK_NAME;
	ref_remove(lk_nm);
}

void
skeleton_glb::ref_write(ch_string val_ref, ch_string nam_ref){
	//DBG_PRT(74, os << "WRITING nam_ref=" << nam_ref << "'");

	ch_string f_nm = as_full_path(nam_ref);
	long wr_ok = set_fstr(f_nm, val_ref);
	if(wr_ok != 0){
		unlink(f_nm.c_str());
	}
	
}

ch_string
skeleton_glb::ref_read(ch_string nam_ref){
	ch_string f_nm = as_full_path(nam_ref);
	ch_string val_ref = get_fstr(f_nm);
	if(! ref_in_skl(val_ref)){
		return "";
	}
	return val_ref;
}

ch_string
skeleton_glb::ref_vnt_name(ch_string vpth, ch_string sub_nm){
	SKELETON_CK(! vpth.empty());
	SKELETON_CK(*(vpth.begin()) == '/');
	SKELETON_CK(*(vpth.rbegin()) == '/');

	DBG(ch_string vfull = as_full_path(vpth));
	DBG(bool ck1 = (! ref_exists(vpth) || (vfull == (path_to_absolute_path(vfull) + '/'))));
	DBG_COND_COMM(! ck1,
		os << "ABORTING_DATA " << bj_eol;
		os << " vpth=" << vpth << bj_eol;
		os << " vfull=" << vfull << bj_eol;
		os << " val2=" << (path_to_absolute_path(vfull) + '/') << bj_eol;
		os << "END_OF_aborting_data" << bj_eol;
	);
	SKELETON_CK(ck1);

	ch_string ref_name = vpth + sub_nm;

	return ref_name;
}

void
canon_cnf::init_with(skeleton_glb& skg, row<canon_clause*>& all_ccls, 
		long tot_vars, long tot_lits, long tot_twolits, bool sorted_cnf)
{
	clear_cnf();

	cf_sorted = sorted_cnf;

	bool need_to_calc = ((tot_vars == INVALID_NATURAL) || (tot_lits == INVALID_NATURAL) || 
				(tot_twolits == INVALID_NATURAL));

	if(need_to_calc){ canon_count_tots(all_ccls, tot_vars, tot_lits, tot_twolits); }

	DBG(
		if(! need_to_calc){ 
			long dbg_tot_vars; long dbg_tot_lits; long dbg_tot_twolits;
			canon_count_tots(all_ccls, dbg_tot_vars, dbg_tot_lits, dbg_tot_twolits); 
			DBG_COND_COMM(! (tot_vars == dbg_tot_vars),
				os << "ABORTING_DATA " << bj_eol;
				os << " dbg_tot_vars=" << dbg_tot_vars << bj_eol;
				os << " tot_vars=" << tot_vars << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			SKELETON_CK(tot_vars == dbg_tot_vars);
			DBG_COND_COMM(! (tot_lits == dbg_tot_lits),
				os << "ABORTING_DATA " << bj_eol;
				os << " all_ccls=" << bj_eol;
				all_ccls.print_row_data(os, true, "\n");
				os << " dbg_tot_lits=" << dbg_tot_lits << bj_eol;
				os << " tot_lits=" << tot_lits << bj_eol;
				os << "END_OF_aborting_data" << bj_eol;
			);
			SKELETON_CK(tot_lits == dbg_tot_lits);
			SKELETON_CK(tot_twolits == dbg_tot_twolits);
		}
	);

	all_ccls.move_to(cf_clauses);

	SKELETON_CK(tot_vars != INVALID_NATURAL);
	SKELETON_CK(tot_lits != INVALID_NATURAL);
	SKELETON_CK(tot_twolits != INVALID_NATURAL);

	cf_dims.dd_tot_lits = tot_lits;	// purge_clauses change this val so leave this here.
	cf_dims.dd_tot_twolits = tot_twolits;

	//the_cnf.purge_clauses(GSKE);

	cf_dims.dd_tot_ccls = cf_clauses.size();
	cf_dims.dd_tot_vars = tot_vars;

	calc_sha();

	SKELETON_CK(! cf_sha_str.empty());

	init_skl_paths(skg);
}

/*
void
canon_cnf::get_first_full(long& idx_all_pos, long& idx_all_neg){
	DBG_PRT(76, print_canon_cnf(os));

	idx_all_pos = INVALID_IDX;
	idx_all_neg = INVALID_IDX;

	row<canon_clause*>& all_ccl = cf_clauses;
	for(long aa = 0; aa < all_ccl.size(); aa++){
		canon_clause& ccl = *(all_ccl[aa]);
		charge_t fchg = ccl.cc_is_full();

		if((idx_all_pos == INVALID_IDX) && (fchg == cg_positive)){
			idx_all_pos = aa;
		}
		if((idx_all_neg == INVALID_IDX) && (fchg == cg_negative)){
			idx_all_neg = aa;
		}
		if((idx_all_pos != INVALID_IDX) && (idx_all_neg != INVALID_IDX)){
			break;
		}
	}
}
*/

charge_t
canon_clause::cc_is_full(){
	charge_t fchg = cg_neutral;
	for(long bb = 0; bb < cc_size(); bb++){
		long lit = cc_pos(bb);
		SKELETON_CK(lit != 0);		

		if(bb == 0){
			SKELETON_CK(fchg == cg_neutral);
			if(lit > 0){
				fchg = cg_positive;
			} else {
				SKELETON_CK(lit < 0);
				fchg = cg_negative;
			}
		} else {
			SKELETON_CK(fchg != cg_neutral);
			if((lit > 0) && (fchg == cg_negative)){
				fchg = cg_neutral;
				break;
			}
			if((lit < 0) && (fchg == cg_positive)){
				fchg = cg_neutral;
				break;
			}
		}
	}
	return fchg;
}

void
mix_sort_ccls(row<canon_clause*>& the_ccls){
	for(long aa = 0; aa < the_ccls.size(); aa++){
		SKELETON_CK(the_ccls[aa] != NULL_PT);
		canon_clause& ccl = *(the_ccls[aa]);
		ccl.cc_mix_sort(cmp_canon_ids);
	}

	the_ccls.mix_sort(cmp_clauses);
}



