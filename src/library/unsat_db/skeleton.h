

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

skeleton.h  

Classes for skeleton and directory management in canon_cnf DIMACS format.

--------------------------------------------------------------*/


#ifndef SKELETON_H
#define SKELETON_H

#include "bj_stream.h"
#include "tools.h"
#include "file_funcs.h"
#include "dimacs.h"
#include "sha2.h"
#include "print_macros.h"
#include "dbg_prt.h"
#include "str_set.h"

enum charge_t {
	cg_negative = -1,
	cg_neutral = 0,
	cg_positive = 1
};

class instance_info;

class solver;
class brain;
class neuron;
class canon_clause;
class variant;
class canon_cnf;
class skeleton_glb;


#define SKELETON_DBG(prm) DBG(prm)
#define SKELETON_CK(prm) 	DBG_BJ_LIB_CK(prm)
#define SKELETON_CK_PRT(prm, comms1)  DBG_CK_2(prm, comms1)


//=================================================================
// defines

#define SKG_NUM_DIGITS_IN_DIRNAM_OF_NUMBER_PATH 1
#define SKG_MAX_SAMPLE_NUM_LITS 1000
#define SKG_MAX_SAMPLE_NUM_CCLS 10

#ifdef FULL_DEBUG
#define SKG_MAX_NUM_VARIANT 10
#else
#define SKG_MAX_NUM_VARIANT 10
#endif

#define SKG_LOCK_SEP_STR "%"
#define SKG_LOCK_SEP '%'
#define SKG_VARIANT_DIR "vv/"

#define SKG_CANON_PATH_ENDING "oo"

#define SKG_CANON_HEADER_STR \
		"c Yashua Melej Hamashiaj. \n" \
		"c ben-jose dimacs unsat cnf generated file.\n" \
		"c Any modification will make it useless for ben-jose. \n" \

//--end_of_def


//=================================================================
// skeleton defines

//define SKG_APROX_SHA_PTH_SZ	70

#define SKG_INVALID_PTH		"skg_invalid_pth"
#define SKG_SKELETON_DIR	"/SKELETON"
#define SKG_CNF_DIR		"/SKELETON/CNF"
#define SKG_REF_DIR		"/SKELETON/REF"
#define SKG_LCK_DIR		"/SKELETON/LCK"
#define SKG_TMP_PROOF_DIR	"/SKELETON/TMP/PROOF"
#define SKG_COLLISIONS_DIR	"/SKELETON/ERR/COLLISIONS"
#define SKG_MISSING_DIR		"/SKELETON/ERR/MISSING"
#define SKG_CORRUPTED_DIR	"/SKELETON/ERR/CORRUPTED"
#define SKG_OVERLAPED_DIR	"/SKELETON/ERR/OVERLAPED"
#define SKG_DEAD_DIR		"/SKELETON/ERR/DEAD"
#define SKG_BROKEN_DIR		"/SKELETON/ERR/BROKEN"

#define SKG_REF_SUF 		".ref"

#define SKG_CANON_NAME		"canon.skl"
#define SKG_DIFF_NAME		"diff.skl"
#define SKG_GUIDE_NAME		"guide.skl"
#define SKG_GUIDE_SHA_NAME	"guide_sha.skl"
#define SKG_COMMENT_NAME	"comment.skl"
#define SKG_ELAPSED_NAME	"elapsed.skl"
#define SKG_NUM_VNT_NAME	"numvariants.skl"

#define SKG_ERR_COUNT_NAME	"err_count.skl"
#define SKG_READY_NAME		"ready"

#define SKG_VERIFY_NAME		"/SKELETON/verify.skl"

#define SKG_DBG_COLLI_DIFF	"/dbg_colli_diff.skl"
#define SKG_DBG_COLLI_NEW	"/dbg_colli_new.skl"

//=================================================================
// funtion declarations

DECLARE_PRINT_FUNCS(canon_clause)
DECLARE_PRINT_FUNCS(variant)
DECLARE_PRINT_FUNCS(canon_cnf)

//=================================================================
// path funcs

bool		print_str_long_map(bj_ostream& os, string_long_map_t& pmp);

bool		not_skl_path(ch_string pth);

ch_string	nam_subset_resp(cmp_is_sub rr);
void		string_replace_char(ch_string& src_str, char orig, char repl);

bool		path_is_dead_lock(ch_string the_pth);
//bool		path_is_diff_file(ch_string the_pth);

bool 		dims_path_exists(ch_string base_pth, const dima_dims& dims);

ch_string&	slice_str_to_path(ch_string& sha_txt);
ch_string	long_to_path(long nn, long d_per_dir);
bool		all_digits(ch_string& the_str);
ch_string	cnf_dims_to_path(const dima_dims& dims);

void		delete_sha_skeleton(ch_string& sha_pth);

bool		canon_save(skeleton_glb& skg, ch_string& the_pth, row<char>& cnn, 
					   bool write_once = true);
bool		canon_load(skeleton_glb& skg, ch_string& the_pth, row<char>& cnn);
bool		canon_equal(skeleton_glb& skg, ch_string& the_pth, row<char>& cnn);

ch_string	canon_hash_path(const dima_dims& dims, ch_string sha_str);
//ch_string	canon_lock_name(const dima_dims& dims, ch_string sha_str);

void		canon_print(bj_ostream& os, row<char>& cnn);
void		canon_sha(row<char>& cnn, ch_string& sha_txt, ch_string& minisha_txt);

ch_string	canon_header(ch_string& hd_str, long ccls, long vars);
void		canon_long_append(row<char>& cnn, long nn);
void		canon_string_append(row<char>& cnn, ch_string ss);
long		canon_purge_clauses(skeleton_glb& skg, row<canon_clause*>& all_ccl, long& tot_lits, long& tot_twolits);

void		canon_count_tots(row<canon_clause*>& all_ccls, long& tot_vars, long& tot_lits, long& tot_twolits);


//======================================================================
// skeleton_exception

class skeleton_exception : public top_exception {
public:
	skeleton_exception(long the_id = 0) : top_exception(the_id)
	{}
};

//=================================================================
// canon_clause

//define MAX_CLAUSE_SZ	100

class canon_clause : private row_long_t {
public:
	SKELETON_DBG(
		brain*	cc_pt_brn;
		bool	cc_can_release;
		bool	cc_in_free;
	);

	void*	cc_me;
	bool	cc_spot;

	canon_clause();
	~canon_clause();

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		SKELETON_DBG(the_brn = cc_pt_brn);
		return the_brn;
	}

	solver*	get_dbg_slv();
	
	void	set_dbg_brn(brain* pt_brn){
		SKELETON_DBG(cc_pt_brn = pt_brn);
	}
	
	bool	is_cc_virgin(){
		bool c1 = is_empty();
		return c1;
	}

	void	init_canon_clause(bool free_mem){
		SKELETON_DBG(
			cc_pt_brn = NULL;
			cc_in_free = false;
		)

		cc_me = NULL_PT;
		cc_spot = false;
		clear(free_mem, free_mem);
	}

	template<class obj_t1>
	obj_t1&
	me_as(){
		SKELETON_CK(cc_me != NULL_PT);
		obj_t1& obj = *((obj_t1*)(cc_me));
		SKELETON_CK(obj.get_cls_name() == obj_t1::CL_NAME);
		return obj;
	}

	bool	cc_is_empty(){
		SKELETON_CK(! cc_in_free);
		return is_empty();
	}

	long	cc_size(){
		SKELETON_CK(! cc_in_free);
		return size();
	}

	long	cc_last_idx(){
		SKELETON_CK(! cc_in_free);
		return last_idx();
	}

	void	cc_push(long the_lit){
		SKELETON_CK(! cc_in_free);
		push(the_lit);
	}

	void	cc_mix_sort(cmp_func_t cmp_fn){
		SKELETON_CK(! cc_in_free);
		mix_sort(cmp_fn);
	}

	bool	cc_is_sorted(cmp_func_t cmp_fn){
		SKELETON_CK(! cc_in_free);
		return is_sorted(cmp_fn);
	}

	long	cc_pos(long the_idx){
		SKELETON_CK(! cc_in_free);
		SKELETON_CK(is_valid_idx(the_idx));
		return pos(the_idx);
	}

    	void 	cc_copy_to(canon_clause& dest, 
				row_index first_ii = 0, row_index last_ii = -1,
				bool inv = false)
	{ 
		copy_to(dest, first_ii, last_ii, inv);
	}

	void	cc_clear(bool free_mem);

	charge_t	cc_is_full();

	void		add_chars_to(row<char>& cnn);

	bj_ostream&	print_canon_clause(bj_ostream& os, bool from_pt = false);
};

comparison
cmp_canon_ids(const long& id1, const long& id2);

comparison
cmp_lit_rows(row_long_t& trl1, row_long_t& trl2);

//comparison
//cmp_trails(row_long_t& trl1, row_long_t& trl2);

comparison
cmp_clauses(canon_clause* const& srt1, canon_clause* const& srt2);

template<class obj_t1>
void
ccl_row_as(row<canon_clause*>& rr1, row<obj_t1*>& rr2, bool only_spotted = false){
	rr2.clear();
	rr2.set_cap(rr1.size());
	for(long ii = 0; ii < rr1.size(); ii++){
		SKELETON_CK(rr1[ii] != NULL_PT);		
		canon_clause& the_ccl = *(rr1[ii]);

		bool add_it = true;
		if(only_spotted){ add_it = the_ccl.cc_spot; }

		if(add_it){
			obj_t1& obj_1 = the_ccl.me_as<obj_t1>();
			rr2.push(&obj_1);
		}
	}
}

//=================================================================
// variant

class variant {
public:
	ch_string	vn_real_path;
	average		vn_elap;

	variant(){
		init_variant();
	}

	~variant(){
	}

	void	init_variant(){
		vn_real_path = "";
		vn_elap.init_average();
	}

	bj_ostream&	print_variant(bj_ostream& os, bool from_pt = false){	
		os << "vn[" << vn_real_path << " " << vn_elap << "]" << bj_eol;
		return os;
	}
};

long		choose_variant(row<variant>& all_vnt, 
						   bj_big_float_t& avg_szs, bool in_dbg = false);

comparison
cmp_variant(variant const & vnt1, variant const & vnt2);

//=================================================================
// canon_cnf

enum cnf_kind_t {
	fk_invalid = 10,
	fk_guide,
	fk_diff,
	fk_canon
};

class canon_cnf {
public:
	SKELETON_DBG(brain*		cf_pt_brn;)
	
	cnf_kind_t		cf_kind;
	
	bool			cf_sorted;

	dima_dims		cf_dims;

	row<canon_clause*>	cf_clauses;

	ch_string		cf_sha_str;
	ch_string		cf_minisha_str;
	ch_string		cf_diff_minisha_str;

	row<char>		cf_chars;
	row<char>		cf_comment_chars;
	long			cf_num_cls_in_chars;

	ch_string		cf_phase_str;

	ch_string		cf_unique_path;

	long			cf_num_purged;

	canon_cnf*		cf_guide_cnf;
	canon_cnf*		cf_tauto_cnf;
	ch_string		cf_quick_find_ref;
	row_str_t		cf_dbg_shas;
	
	instance_info* 	cf_inst_inf;
	
	ch_string 		cf_tmp_sv_dir;
	ch_string 		cf_tmp_pth1;
	ch_string 		cf_tmp_pth2;

	SKELETON_DBG(
		void* 	cf_dbg_orig_nmp;
		bool	cf_dbg_file_existed;
	);
		
	canon_cnf(){
		init_canon_cnf();
	}

	~canon_cnf(){
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		SKELETON_DBG(the_brn = cf_pt_brn);
		return the_brn;
	}

	solver*	get_dbg_slv();
	
	void	set_dbg_brn(brain* pt_brn){
		SKELETON_DBG(cf_pt_brn = pt_brn);
	}
	
	void init_canon_cnf(bool free_mem = false){
		SKELETON_DBG(cf_pt_brn = NULL);
		
		cf_kind = fk_invalid;
		
		cf_sorted = true;
	
		cf_dims.init_dima_dims(INVALID_NATURAL);
		cf_dims.dd_tot_twolits = 0;

		SKELETON_CK(cf_clauses.is_empty());

		cf_sha_str = "";
		cf_minisha_str = "";
		cf_diff_minisha_str = "";

		cf_chars.clear(free_mem, free_mem);
		cf_comment_chars.clear(free_mem, free_mem);
		cf_num_cls_in_chars = -1;

		cf_phase_str = "";

		cf_unique_path = "";

		cf_num_purged = 0;

		cf_guide_cnf = NULL_PT;
		cf_tauto_cnf = NULL_PT;
		cf_quick_find_ref = "";
		cf_dbg_shas.clear(free_mem, free_mem);
		
		cf_inst_inf = NULL_PT;

		cf_tmp_sv_dir = "";
		cf_tmp_pth1 = "";
		cf_tmp_pth2 = "";
		
		SKELETON_DBG(
			cf_dbg_orig_nmp = NULL_PT;
			cf_dbg_file_existed = false;
		);
		
		SKELETON_CK(cf_unique_path.size() == 0);
	}

	void	init_with_ccls(skeleton_glb& skg, row<canon_clause*>& all_ccls, 
				long tot_vars = INVALID_NATURAL, 
				long tot_lits = INVALID_NATURAL, 
				long tot_twolits = INVALID_NATURAL, 
				bool sorted_cnf = true);

	void	release_and_init(skeleton_glb& skg, bool free_mem = false){
		release_all_clauses(skg, free_mem);
		brain* old_dbg_brn = get_dbg_brn();
		init_canon_cnf(free_mem);
		set_dbg_brn(old_dbg_brn);
	}

	void	clear_all_spots();

	bool	ck_full_dir(ch_string sv_dir){
		SKELETON_CK(! sv_dir.empty());
		SKELETON_CK(*(sv_dir.begin()) == '/');
		SKELETON_CK(*(sv_dir.rbegin()) == '/');
		return true;
	}

	bool	ck_all_can_release(row<canon_clause*>& rr, bool can_val){
		for(long aa = 0; aa < rr.size(); aa++){
			SKELETON_CK(rr[aa] != NULL_PT);
			canon_clause& ccl = *(rr[aa]);
			MARK_USED(ccl);
			SKELETON_CK(ccl.cc_can_release == can_val);
		}
		return true;
	}

	void	clear_cnf(){
		SKELETON_CK(ck_all_can_release(cf_clauses, false));
		cf_clauses.clear();
		
		brain* old_dbg_brn = get_dbg_brn();
		init_canon_cnf(false);
		set_dbg_brn(old_dbg_brn);
	}

	canon_clause&	add_clause(skeleton_glb& skg);
	
	bool	is_empty(){
		SKELETON_CK(cf_dims.dd_tot_ccls == cf_clauses.size());
		bool is_e = cf_clauses.is_empty();
		return is_e;
	}

	ch_string	get_id_str();
	void		get_extreme_lits(row<long>& lits);

	ch_string	get_num_variants_file_name(skeleton_glb& skg);

	void	release_all_clauses(skeleton_glb& skg, bool free_mem = false);

	bool	has_phase_path(){
		return has_cnfs();
	}
	
	bool	has_cnfs(){
		bool c1 = (cf_guide_cnf != NULL_PT);
		bool c2 = (cf_tauto_cnf != NULL_PT);
		bool c_ok = (c1 && c2);
		SKELETON_CK(is_diff() == c_ok);
		return c_ok;
	}

	canon_cnf&	get_guide_cnf(){
		SKELETON_CK(cf_guide_cnf != NULL_PT);
		return *cf_guide_cnf;
	}
	
	canon_cnf&	get_tauto_cnf(){
		SKELETON_CK(cf_tauto_cnf != NULL_PT);
		return *cf_tauto_cnf;
	}
	
	ch_string	get_ref1_nam(){
		SKELETON_CK(has_cnfs());
		SKELETON_CK(is_diff());
		return cf_guide_cnf->get_ref_path();
	}

	ch_string	get_ref2_nam(){
		SKELETON_CK(has_cnfs());
		SKELETON_CK(is_diff());
		return cf_guide_cnf->get_lck_path();
	}

	ch_string	get_ref3_nam(){
		SKELETON_CK(has_cnfs());
		SKELETON_CK(is_diff());
		return cf_quick_find_ref;
	}

	ch_string	get_vnt_nam(){
		return get_ref1_nam();
	}

	ch_string	get_lck_nam(){
		return get_ref2_nam();
	}

	bool	is_canon(){
		return (cf_kind == fk_canon);
	}
	
	bool	is_diff(){
		return (cf_kind == fk_diff);
	}
	
	bool	is_guide(){
		return (cf_kind == fk_guide);
	}

	void	update_diff_refs(skeleton_glb& skg, ch_string sv_vpth);
	void	update_mng_verif_sys(skeleton_glb& skg, ch_string sv_vpth);
	
	void	set_kind_from(ch_string the_pth){
		SKELETON_CK(cf_kind == fk_invalid);
		
		ch_string nm = path_get_name(the_pth);
		if(nm == SKG_GUIDE_NAME){
			cf_kind = fk_guide;
		} else
		if(nm == SKG_DIFF_NAME){
			cf_kind = fk_diff;
		} else
		if(nm == SKG_CANON_NAME){
			cf_kind = fk_canon;
		} 
		SKELETON_CK(cf_kind != fk_invalid);
	}
	
	ch_string	get_kind_name(){
		ch_string fnm = "";
		SKELETON_CK(cf_kind != fk_invalid);
		switch(cf_kind){
			case fk_guide: 
				fnm = SKG_GUIDE_NAME;
				break;
			case fk_diff: 
				fnm = SKG_DIFF_NAME;
				break;
			case fk_canon: 
				fnm = SKG_CANON_NAME;
				break;
			default: 
				break;
		}
		SKELETON_CK(fnm != "");
		return fnm;
	}

	ch_string	get_all_variant_dir_name();
	ch_string	get_variant_ref_fname(long num_vnt);
	ch_string	get_variant_path(skeleton_glb& skg, long num_vnt, bool skip_report = false);

	bool	i_am_sub_of(canon_cnf& the_cnf, bool& are_eq);
	bool	i_am_super_of(canon_cnf& the_cnf, bool& are_eq);
	bool	i_sub_of_vnt(skeleton_glb& skg, ch_string& vpth, bool& are_eq);
	bool	i_super_of_vnt(skeleton_glb& skg, ch_string& vpth);
	bool	i_equal_to_vnt(skeleton_glb& skg, ch_string& vpth);
	bool	i_exact_found(skeleton_glb& skg);

	bj_big_int_t	get_num_variants(skeleton_glb& skg);
	void		set_num_variants(skeleton_glb& skg, bj_big_int_t num_vnts);

	bool	all_nxt_vnt(skeleton_glb& skg, row<variant>& all_next, row<ch_string>& all_del);
	
	ch_string	first_vnt_i_super_of(skeleton_glb& skg, row<neuron*>& all_found, 
									 instance_info* iinfo = NULL);
	
	bool	ck_vnts(skeleton_glb& skg);

	ch_string	get_cnf_path();

	ch_string	get_ref_path(){
		SKELETON_CK(cf_unique_path.size() > 0);
		ch_string ref_pth = SKG_REF_DIR + cf_unique_path;
		return ref_pth;
	}

	ch_string	get_lck_path(){
		SKELETON_CK(cf_unique_path.size() > 0);
		ch_string ref_pth = SKG_LCK_DIR + cf_unique_path;
		return ref_pth;
	}
	
	bool	is_new(skeleton_glb& skg);
	
	ch_string prepare_cnf(skeleton_glb& skg, ch_string sv_base_pth);

	bool	save_canon_cnf(ch_string& the_pth, row<char>& cnn, bool write_once = true);
	bool	save_cnf(skeleton_glb& skg, ch_string pth);
	void	update_parent_variants(skeleton_glb& skg, ch_string sv_dir);

	void	update_chars_to_write();

	void	load_lits(skeleton_glb& skg, row_long_t& all_lits, long& tot_lits, 
					  long& tot_twolits);
	void	calc_sha_in(ch_string& sha_str, ch_string& minisha_str);

	long	purge_clauses(skeleton_glb& skg);
	bool	ck_all_sup_leaf(row_str_t& all_pth);

	void	calc_sha(){
		calc_sha_in(cf_sha_str, cf_minisha_str);
	}

	void	init_skl_paths(skeleton_glb& skg);

	//void		fill_with(skeleton_glb& skg, row<long>& all_lits, long num_cla, long num_var);
	bool		load_from(skeleton_glb& skg, ch_string& f_nam);
	
	bool 	has_instance_info(){
		return (cf_inst_inf != NULL);
	}

	instance_info&	get_info(){
		SKELETON_CK(has_instance_info());
		return (*cf_inst_inf);
	}
	
	//void		get_first_full(long& idx_all_pos, long& idx_all_neg);

	//bj_ostream&	print_canon_cnf(bj_ostream& os);
	bj_ostream&	print_canon_cnf(bj_ostream& os, bool from_pt = false);

	bj_ostream&	print_attrs_canon_cnf(bj_ostream& os){
		os << cf_dims << bj_eol;
		/*
		os << "cf_tot_vars=" << cf_tot_vars << bj_eol;
		os << "cf_tot_ccls=" << cf_tot_ccls << bj_eol;
		os << "cf_tot_lits=" << cf_tot_lits << bj_eol;
		*/
		os << "cf_sha_str=" << cf_sha_str << bj_eol;
		os << "cf_minisha_str=" << cf_minisha_str << bj_eol;
		os << "cf_num_purged=" << cf_num_purged << bj_eol;
		return os;
	}
};

//=================================================================
// skeleton_glb

class skeleton_glb {
public:
	SKELETON_DBG(brain* 		kg_pt_brn;)
	
	k_row<canon_clause>	kg_clauses;
	row<canon_clause*>	kg_free_clauses;

	bool			kg_dbg_only_save;
	bool			kg_dbg_verifying_skeleton_tree;
	bool			kg_dbg_local_verifying;

	bool			kg_dbg_save_canon;
	bool			kg_keep_skeleton;
	bool			kg_find_cnn_pth;

	SKELETON_DBG(string_long_map_t 	kg_dbg_all_wrt_paths);
	
	string_set_t	kg_cnf_new_paths;
	string_set_t	kg_cnf_paths_found;

	ch_string		kg_root_path;
	ch_string		kg_running_path;

	ch_string		kg_instance_file_nam;
	
	ch_string		kg_tmp_proof_path;

	ch_string		kg_collisions_path;
	ch_string		kg_missing_path;
	ch_string		kg_corrupted_path;
	ch_string		kg_overlaped_path;
	ch_string		kg_dead_path;
	ch_string		kg_broken_path;

	ch_string		kg_verify_path;
	time_t			kg_verify_mtime;

	bool			kg_last_cnn_eq;

	row<long>		kg_tmp_lits;
	canon_cnf		kg_tmp_cnf;
	row<variant> 		kg_tmp_all_nxt_vnts;
	row<ch_string> 	kg_tmp_all_del_paths;

	//skeleton_glb() : kg_cnf_paths_found(cmp_string) {
	skeleton_glb() {
		init_skeleton_glb();
	}

	~skeleton_glb(){
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		SKELETON_DBG(the_brn = kg_pt_brn);
		return the_brn;
	}

	solver*	get_dbg_slv();
	
	void	set_dbg_brn(brain* pt_brn){
		SKELETON_DBG(
			kg_pt_brn = pt_brn;
			kg_tmp_cnf.set_dbg_brn(pt_brn);
		);
	}
				
	ch_string	as_full_path(ch_string sklroute){
		SKELETON_CK(kg_root_path != "");
		SKELETON_CK(path_begins_with(sklroute, SKG_SKELETON_DIR));
		return kg_root_path + sklroute;
	}

	void	start_local();
	void	clear_all();

	bool 	in_dbg_verif(){ 
		return (kg_dbg_verifying_skeleton_tree || kg_dbg_local_verifying); 
	}

	canon_clause&	get_new_clause();
	void	release_clause(canon_clause& ccl, bool free_mem);

	void	init_skeleton_glb();
	void	init_paths();
	void	report_err(ch_string pth, ch_string err_pth);

	bool	find_skl_path(ch_string the_pth, instance_info* iinfo = NULL);

	bool	in_skl(ch_string a_dir){
		ch_string skl_pth = as_full_path(SKG_SKELETON_DIR);
		return path_begins_with(a_dir, skl_pth);
	}

	bool	ref_in_skl(ch_string a_ref){
		return path_begins_with(a_ref, SKG_SKELETON_DIR);
	}

	bool	ref_exists(ch_string a_ref);

	bool	ref_create(ch_string a_ref);

	bool	ref_find(ch_string a_ref){
		ch_string f_pth = as_full_path(a_ref);
		return find_skl_path(f_pth);
	}

	bool	ref_touch(ch_string a_ref);

	void	ref_remove(ch_string a_ref){
		ch_string f_pth = as_full_path(a_ref);
		unlink(f_pth.c_str());
	}

	void	ref_write(ch_string nam_ref, ch_string val_ref);
	ch_string	ref_read(ch_string nam_ref);

	ch_string	ref_vnt_name(ch_string vpth, ch_string sub_nm);
	
	long num_new_paths(){
		return strset_size(kg_cnf_new_paths);
	}

	void	add_comment_chars_to(brain& brn, canon_cnf& diff_cnf, 
								 ch_string sv_ref_pth, row<char>& cnn);
	
	int		get_write_lock(ch_string lk_dir);
	void	drop_write_lock(ch_string lk_dir, int fd_lock);

	ch_string	read_guide_sha_str(ch_string vpth);
	void		write_guide_sha_str(ch_string vpth, ch_string sha_str);
	
	bj_ostream&	print_paths(bj_ostream& os);
};


//=================================================================
// printing operators

DEFINE_PRINT_FUNCS(canon_clause);
DEFINE_PRINT_FUNCS(variant);
DEFINE_PRINT_FUNCS(canon_cnf);


#endif		// SKELETON_H



