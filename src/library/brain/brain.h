

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

brain.h

Declarations of classes and that implement the neural network.

--------------------------------------------------------------*/

#ifndef BRAIN_H
#define BRAIN_H

//=================================================================

#include "tools.h"
#include "ben_jose.h"
#include "instance_info.h"
#include "sortor.h"
#include "dbg_config.h"
#include "dbg_prt.h"

//=============================================================================
// defines

#define BRAIN_DBG(prm) DBG(prm)

#define BRAIN_CK(prm) DBG_BJ_LIB_CK(prm)
#define BRAIN_CK_PRT(prm, comms1)  DBG_CK_2(prm, comms1)

#define BRAIN_CK_0(prm)	DBG_BJ_LIB_CK(prm)

#define BRAIN_CK_1(prm)  /**/

#define BRAIN_CK_2(prm)  /**/

#define DEDUC_DBG(cod)	DBG(cod)


//=============================================================================
// MAIN CLASSES

#define ROOT_LEVEL 0
#define MIN_TRAIL_SZ 1

#define MIN_NUM_BLKS 100
#define MIN_NUM_MAPS 100

enum action_t {
	ac_invalid = -100,
	ac_recoil = -1,
	ac_stop = 0,
	ac_go_on = 1
};

enum mem_op_t {
	mo_invalid = 0,
	mo_save = 1,
	mo_find = 2
};

#define k_invalid_order		0
#define k_left_order		1
#define k_right_order		2
#define k_random_order		3

#define INVALID_RECOIL 0
#define INVALID_LEVEL -1
#define INVALID_TIER -1
#define INVALID_BLOCK -1

#define MAX_LAYERS 1000

//=============================================================================
// defs

#define DBG_DIR "./dbg_ic_output/"
#define DBG_PREF "brain"
#define DBG_SUFI ".myl"


//=============================================================================
// decl

class solver;
class skeleton_glb;
class instance_info;

BRAIN_DBG(class dbg_inst_info;)

class ticket;
class quanton;
class neuron;
class deduction;
class coloring;
class neuromap;
class memap;
class deducer;
class notekeeper;
class prop_signal;
class leveldat;
class brain;

typedef	row<quanton*>		row_quanton_t;
typedef	row<row_quanton_t>	row_row_quanton_t;

typedef bj_big_int_t	recoil_counter_t;

//=================================================================
// printing declarations

// in print_macros.h
/*
#define DECLARE_PRINT_FUNCS(obj_t) \
bj_ostream& operator << (bj_ostream& os, obj_t& obj1); \
bj_ostream& operator << (bj_ostream& os, obj_t* obj1); \
\

// end_of_define

#define DEFINE_PRINT_FUNCS(obj_t) \
inline \
bj_ostream& operator << (bj_ostream& os, obj_t& obj1){ \
	obj1.print_##obj_t(os); \
	os.flush(); \
	return os; \
} \
\
inline \
bj_ostream& operator << (bj_ostream& os, obj_t* obj1){ \
	if(obj1 == NULL_PT){ \
		os << "NULL_" << #obj_t; \
	} else { \
		obj1->print_##obj_t(os, true); \
	} \
	os.flush(); \
	return os; \
} \
\

// end_of_define
*/

DECLARE_PRINT_FUNCS(ticket)
DECLARE_PRINT_FUNCS(quanton)
DECLARE_PRINT_FUNCS(neuron)
DECLARE_PRINT_FUNCS(deduction)
DECLARE_PRINT_FUNCS(prop_signal)
DECLARE_PRINT_FUNCS(coloring)
DECLARE_PRINT_FUNCS(neuromap)
DECLARE_PRINT_FUNCS(memap)
DECLARE_PRINT_FUNCS(leveldat)

//=================================================================
// comparison declarations

bool 		is_ticket_eq(ticket& x, ticket& y);
comparison	cmp_long_id(quanton* const & qua1, quanton* const & qua2);
comparison	cmp_choice_idx_lt(quanton* const & qua1, quanton* const & qua2);
comparison	cmp_choice_idx_gt(quanton* const & qua1, quanton* const & qua2);
comparison	cmp_qlevel(quanton* const & qua1, quanton* const & qua2);
comparison	cmp_qtier(quanton* const & qua1, quanton* const & qua2);
bool		ck_motives(brain& brn, row_quanton_t& mots);


//=================================================================
// funcs declarations

void	dbg_prepare_used_dbg_ccl(row_quanton_t& rr_qua, canon_clause& dbg_ccl);
void	dbg_print_ccls_neus(bj_ostream& os, row<canon_clause*>& dbg_ccls);
bool	dbg_run_satex_on(brain& brn, ch_string f_nam);

long	set_spots_of(brain& brn, row<neuron*>& neus);
long	reset_spots_of(brain& brn, row<neuron*>& neus);

charge_t 	negate_trinary(charge_t val);

bool	has_neu(row<neuron*>& rr_neus, neuron* neu);

long	set_dots_of(brain& brn, row_quanton_t& quans);
long	reset_dots_of(brain& brn, row_quanton_t& quans);

void	negate_quantons(row_quanton_t& qua_row);
void	get_ids_of(row_quanton_t& quans, row_long_t& the_ids);
void	elim_until_dominated(brain& brn, quanton& qua);
void	find_max_level(row_quanton_t& tmp_mots, long& max_lev);

void	split_tees(sort_glb& srg, row<sortee*>& sorted_tees, row<sortee*>& sub_tees, 
			row<canon_clause*>& ccls_in, row<canon_clause*>& ccls_not_in);



//=============================================================================
// ticket

class ticket {
	public:
	recoil_counter_t	tk_recoil;	// recoil at update_tk time
	long			tk_level;	// level at update_tk time

	// methods

	ticket(){
		init_ticket();
	}

	void	init_ticket(){
		tk_recoil = INVALID_RECOIL;
		tk_level = INVALID_LEVEL;
	}

	brain*	get_dbg_brn(){
		return NULL;
	}
	
	bool	is_valid(){
		return ((tk_recoil != INVALID_RECOIL) && (tk_level != INVALID_LEVEL));
	}

	void	update_ticket(brain& brn);

	bj_ostream&	print_ticket(bj_ostream& os, bool from_pt = false){
		os << "[";
		os << "rc:" << tk_recoil;
		os << " lv:" << tk_level;
		os << "]";
		os.flush();
		return os;
	}
};

inline
bool
is_ticket_eq(ticket& x, ticket& y){
	if(x.tk_recoil != y.tk_recoil){ return false; }
	if(x.tk_level != y.tk_level){ return false; }
	return true;
}

//=============================================================================
// quanton

#define DECLARE_NI_FLAG_FUNCS(flag_attr, flag_nam, single) \
	bool	has_pos_##flag_nam(){ \
		return is_##flag_nam(); \
	} \
	\
	bool	has_neg_##flag_nam(){ \
		return opposite().is_##flag_nam(); \
	} \
	\
	bool		is_##flag_nam(){ \
		bool resp = get_flag(flag_attr, k_##flag_nam##_flag); \
		return resp; \
	} \
	\
	bool		has_##flag_nam(){ \
		return (is_##flag_nam() || opposite().is_##flag_nam()); \
	} \
	void	reset_its_##flag_nam(brain& brn); \
	void	reset_##flag_nam(brain& brn); \
	void	set_##flag_nam(brain& brn); \
	\
\

// end_of_define

#define DEFINE_NI_FLAG_FUNCS(flag_attr, flag_nam, glb_counter, single) \
	void	quanton::reset_its_##flag_nam(brain& brn){ \
		BRAIN_CK(has_##flag_nam()); \
		if(is_##flag_nam()){ \
			reset_##flag_nam(brn); \
		} else { \
			opposite().reset_##flag_nam(brn); \
		} \
	} \
	\
	void	quanton::reset_##flag_nam(brain& brn){ \
		BRAIN_CK(is_##flag_nam()); \
		BRAIN_CK(! single || ! opposite().is_##flag_nam()); \
		reset_flag(flag_attr, k_##flag_nam##_flag); \
		brn.glb_counter--; \
	} \
	\
	void	quanton::set_##flag_nam(brain& brn){ \
		BRAIN_CK(! is_##flag_nam()); \
		BRAIN_CK(! single || ! opposite().is_##flag_nam()); \
		set_flag(flag_attr, k_##flag_nam##_flag); \
		brn.glb_counter++; \
	} \
 	\
\

// end_of_define

#define DECLARE_NI_FLAG_ALL_FUNCS(flag_nam) \
 	long		reset_all_its_##flag_nam(brain& brn, row_quanton_t& rr_all); \
 	long		reset_all_##flag_nam(brain& brn, row_quanton_t& rr_all); \
	long		set_all_##flag_nam(brain& brn, row_quanton_t& rr_all); \
\

// end_of_define

#define DEFINE_NI_FLAG_ALL_FUNCS(flag_nam) \
 	long		reset_all_its_##flag_nam(brain& brn, row_quanton_t& rr_all){ \
		long num_qua = 0; \
		for(long aa = 0; aa < rr_all.size(); aa++){ \
			BRAIN_CK(rr_all[aa] != NULL_PT); \
			quanton& qua = *(rr_all[aa]); \
			if(qua.has_##flag_nam()){ \
				qua.reset_its_##flag_nam(brn); \
				num_qua++; \
			} \
		} \
		return num_qua; \
	} \
	\
 	long		reset_all_##flag_nam(brain& brn, row_quanton_t& rr_all){ \
		long num_qua_mod = 0; \
		for(long aa = 0; aa < rr_all.size(); aa++){ \
			BRAIN_CK(rr_all[aa] != NULL_PT); \
			quanton& qua = *(rr_all[aa]); \
			if(qua.has_##flag_nam()){ \
				qua.reset_##flag_nam(brn); \
				num_qua_mod++; \
			} \
		} \
		return num_qua_mod; \
	} \
	\
	long		set_all_##flag_nam(brain& brn, row_quanton_t& rr_all){ \
		long num_qua_mod = 0; \
		for(long aa = 0; aa < rr_all.size(); aa++){ \
			quanton& qua = *(rr_all[aa]); \
			if(! qua.has_##flag_nam()){ \
				qua.set_##flag_nam(brn); \
				num_qua_mod++; \
			} \
		} \
		return num_qua_mod; \
	} \
 	\
\

// end_of_define

#define k_note0_flag			k_flag0
#define k_note1_flag			k_flag1
#define k_note2_flag			k_flag2
#define k_note3_flag			k_flag3
#define k_note4_flag			k_flag4
#define k_note5_flag			k_flag5

DECLARE_NI_FLAG_ALL_FUNCS(note0);
DECLARE_NI_FLAG_ALL_FUNCS(note1);
DECLARE_NI_FLAG_ALL_FUNCS(note2);
DECLARE_NI_FLAG_ALL_FUNCS(note3);
DECLARE_NI_FLAG_ALL_FUNCS(note4);
DECLARE_NI_FLAG_ALL_FUNCS(note5);

class quanton {
	public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return quanton::CL_NAME;
	}
	
	BRAIN_DBG(
		brain*		qu_pt_brn;
	)

	DBG(long		qu_dbg_ic_trail_idx);	// idx in trail

	// id attributes
	long			qu_id;		// my long number id
	long			qu_index;	// idx in brain's 'positons' or 'negatons'
	charge_t		qu_spin;	// positive if positon or negative if negaton
	quanton*		qu_inverse;	// my inverse quanton

	// state attributes
	// symetric means that it must be set for positons and negatons at the same time

	t_1byte			qu_flags;

	charge_t		qu_dot;		// symetric. during learning and neuron creation
	charge_t		qu_mark;	// symetric. during learning and neuron creation
	long			qu_mark_idx;	// in dotted_trace idx when marking

	charge_t		qu_charge;	// symetric. current charge
	ticket			qu_charge_tk;	// symetric. 

	long			qu_tier;	// the tier at which it was charged

	neuron*			qu_source;	// source of signal when charged

	// tunneling attributes
	row<neuron*>		qu_tunnels;	// tunnelled neurons.
	neuron*			qu_uncharged_partner_neu;
	neuron*			qu_bak_uncharged_partner_neu;
	bool			qu_in_semi_monos_to_update;

	// choice attributes
	long			qu_choice_idx;	// idx in brain's 'choices'

	row<neuron*>		qu_full_charged;

	// maps in
	neuromap*		qu_curr_nemap;
	//memap*			qu_curr_map;
	
	prop_signal*	qu_tmp_psig;

	// finder data

	DBG(long		qu_dbg_tee_ti);
	
	sortee			qu_tee;
	sortrel			qu_reltee;

	// dbg attributes
	
	DBG(bj_big_int_t	qu_dbg_fst_lap_cho);
	DBG(bj_big_int_t	qu_dbg_num_laps_cho);

	// methods

	quanton(){
		init_quanton(NULL, cg_neutral, INVALID_IDX, NULL);
	}

	~quanton(){

		BRAIN_CK_0(qu_source == NULL);

		BRAIN_CK_0(qu_tunnels.size() == 0);

		qu_tunnels.clear(false, true);

		init_quanton(NULL, cg_neutral, 0, NULL);
	}

	DECLARE_NI_FLAG_FUNCS(qu_flags, note0, true);
	DECLARE_NI_FLAG_FUNCS(qu_flags, note1, true);
	DECLARE_NI_FLAG_FUNCS(qu_flags, note2, true);
	DECLARE_NI_FLAG_FUNCS(qu_flags, note3, true);
	DECLARE_NI_FLAG_FUNCS(qu_flags, note4, false);
	DECLARE_NI_FLAG_FUNCS(qu_flags, note5, false);

	void	qua_tunnel_signals(brain* brn);
	
	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = qu_pt_brn);
		return the_brn;
	}

	void	init_quanton(brain* brn, charge_t spn, long ii, quanton* inv){
		BRAIN_DBG(qu_pt_brn = brn);

		DBG(qu_dbg_ic_trail_idx = INVALID_IDX);

		qu_id = (spn == cg_positive)?(ii + 1):(-(ii + 1));
		if(spn == cg_neutral){ qu_id = 0; }

		qu_index = ii;
		qu_spin = spn;
		qu_inverse = inv;

		qu_uncharged_partner_neu = NULL_PT;
		qu_bak_uncharged_partner_neu = NULL_PT;
		qu_in_semi_monos_to_update = false;

		qu_choice_idx = INVALID_IDX;

		qu_flags = 0;

		qu_dot = cg_neutral;
		qu_mark = cg_neutral;	
		qu_mark_idx = INVALID_IDX;

		qu_charge = cg_neutral;
		qu_charge_tk.init_ticket();

		qu_tier = INVALID_TIER;

		qu_source = NULL;

		qu_curr_nemap = NULL_PT;
		//qu_curr_map = NULL_PT;
		
		qu_tmp_psig = NULL_PT;

		DBG(qu_dbg_tee_ti = INVALID_NATURAL);

		qu_tee.init_sortee(true);
		qu_reltee.init_sortrel(true);

		qu_tee.so_dbg_me_class = 1;
		qu_tee.so_me = this;
		qu_tee.so_related = &qu_reltee;

		qu_tee.so_dbg_extrn_id = qu_id;

		if(qu_inverse != NULL_PT){
			sortee& oppt = qu_inverse->qu_tee;
			qu_reltee.so_opposite = &(oppt);
		}

		DBG(qu_dbg_fst_lap_cho = 0);
		DBG(qu_dbg_num_laps_cho = 0);
	}

	void	init_tees_ccl_id(){
		BRAIN_CK(qu_inverse != NULL_PT);
		qu_tee.so_last_ccl_id = 0;
		qu_inverse->qu_tee.so_last_ccl_id = 0;
	}

	void	reset_and_add_tee(sort_glb& quas_srg, sort_id_t quas_consec);

	bool		is_semi_mono(){
		bool c1 = (opposite().qu_uncharged_partner_neu == NULL_PT);
		bool c2 = (qu_uncharged_partner_neu == NULL_PT);
		return (c1 || c2);
	}

	quanton*	get_semi_mono(){
		if(qu_uncharged_partner_neu != NULL_PT){
			return this;
		}
		quanton& opp = opposite();
		return &opp;
	}

	quanton&	opposite(){
		BRAIN_CK(qu_inverse != NULL_PT);
		return (*qu_inverse);
	}

	quanton*	get_positon(){
		if(qu_spin == cg_negative){
			BRAIN_CK_0(qu_inverse->qu_spin == cg_positive);
			return qu_inverse;
		}
		BRAIN_CK_0(qu_spin == cg_positive);
		return this;
	}

	long		abs_id(){
		long resp = get_positon()->qu_id;
		BRAIN_CK_0(resp > 0);
		return resp;
	}

	bool	ck_all_tunnels();
	void	tunnel_swapop(long idx_pop);

	// dots

	bool	has_dot(){
		return (qu_dot != cg_neutral);	
	}

	void	set_dot(brain& brn);
	void	reset_dot(brain& brn);

	// marks

	bool	has_mark(){	
		return (qu_mark != cg_neutral);	
	}

	bool	has_pos_mark(){	
		return (qu_mark == cg_positive);	
	}

	bool	has_neg_mark(){	
		return (qu_mark == cg_negative);	
	}

	void	set_mark(brain& brn);
	void	reset_mark(brain& brn);

	//recoil_counter_t	qrecoil(){ return qu_charge_tk.tk_recoil; }
	long			qlevel(){ return qu_charge_tk.tk_level; }

	bool		ck_charge(brain& brn);

	void		set_charge(brain& brn, neuron* src, charge_t cha, long max_tier);
	charge_t	get_charge(){ return qu_charge; }
	bool		has_charge(){ return (get_charge() != cg_neutral); }
	bool		is_pos(){ return (get_charge() == cg_positive); } 
	bool		is_neg(){ return (get_charge() == cg_negative); } 

	bool		has_tier(){ return (qu_tier != INVALID_TIER); }
	
	void		set_source(brain& brn, neuron* neu);
	neuron*		get_source();

	bool		has_source(){
		return (get_source() != NULL_PT);
	}

	void		add_source(neuron& neu);

	bool		in_qu_dominated(brain& brn);
	void		make_qu_dominated(brain& brn);

	void		set_uncharged_partner_neu(brain& brn, long uidx, 
										long dbg_call, neuron* dbg_neu);
	neuron*		get_uncharged_partner_neu(dbg_call_id dbg_call);
	long		find_uncharged_partner_neu();
	void		reset_uncharged_partner_neu(brain& brn);
	bool		ck_uncharged_partner_neu();

	bool		is_choice(){
		bool cho = (! has_source() && (qlevel() != ROOT_LEVEL));
		return cho;
	}
	
	bool	has_tmp_psig(){
		return (qu_tmp_psig != NULL_PT);
	}

	bj_ostream&		print_quanton(bj_ostream& os, bool from_pt = false);

	bj_ostream& 		print_ic_label(bj_ostream& os){
		os << "\"";
		if(is_neg()){ os << "K"; }
		if(qu_spin == cg_negative){ os << "["; }
		os << abs_id();
		if(qu_spin == cg_negative){ os << "]"; }


		os << "<";
		os << qlevel();
		DBG(os << "_" << qu_dbg_ic_trail_idx);
		os << ">";
		os << "\"";
		return os;
	}
};

inline
void
set_all_qu_nemap(row_quanton_t& all_quas, neuromap* nmp)
{
	for(long ii = 0; ii < all_quas.size(); ii++){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		quanton& qua = *(all_quas[ii]);
		quanton& opp = qua.opposite();
		BRAIN_CK(qua.qu_curr_nemap != nmp);
		BRAIN_CK(opp.qu_curr_nemap != nmp);
		qua.qu_curr_nemap = nmp;
		opp.qu_curr_nemap = nmp;
	}
}

inline
void
make_all_qu_dominated(brain& brn, row_quanton_t& all_quas, 
			long first_idx = 0, long last_idx = -1)
{
	if(last_idx < 0){ last_idx = all_quas.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(all_quas.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == all_quas.size()) || all_quas.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		BRAIN_CK(all_quas[ii] != NULL_PT);
		quanton& qua = *(all_quas[ii]);
		qua.make_qu_dominated(brn);
	}
}

//=============================================================================
// neuron

class neuron {
	public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return neuron::CL_NAME;
	}

	BRAIN_DBG(
		brain*		ne_pt_brn;
	)

	//static 
	//long			NE_TOT_SPOTS;

	long			ne_index;		//idx in br_neurons
	bool			ne_original;

	row_quanton_t		ne_fibres;		// used in forward propagation of negative signls

	long			ne_fibre_0_idx;		// this == fibres[0]->tunnels[fibre_0_idx]
	long			ne_fibre_1_idx;		// this == fibres[1]->tunnels[fibre_1_idx]

	long			ne_edge;		// last (with max_level) of negated tunneled quantons 
	ticket			ne_edge_tk;

	bool			ne_is_conflict;		// to push conflcts only once into conflct queue

	sortee			ne_tee;
	sortrel			ne_reltee;

	neuromap*		ne_curr_nemap;
	//memap*			ne_curr_map;

	ticket			ne_recoil_tk;		// srcs of the confl are updated at recoil time
	ticket			ne_deduc_tk;		// srcs of the confl are updated at deduction time

	long			ne_num_remote_tees;

	bool			ne_spot;

	DBG(
		bool			ne_dbg_used_no_orig;
		canon_clause	ne_dbg_ccl;
		bool			ne_dbg_in_used;
		ticket			ne_dbg_filled_tk;	// all filled updated a filled time
		ticket			ne_dbg_creation_tk;
	)

	// methods

	neuron(){
		ne_index = INVALID_IDX;	// not in init_neuron to be consistent with is_ne_virgin.
		init_neuron();
	}

	~neuron(){
		DBG(ne_dbg_ccl.cc_in_free = true);
		init_neuron();
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = ne_pt_brn);
		return the_brn;
	}

	void	init_neuron(){
		BRAIN_DBG(ne_pt_brn = NULL);
		
		//if(fib_sz() > 0){
		if(fib_sz() > 1){
			row_quanton_t empty;
			BRAIN_CK_0(empty.size() == 0);
			quanton* forced_qua = update_fibres(empty, false);
			MARK_USED(forced_qua);
			BRAIN_CK_0(forced_qua == NULL);
		}

		ne_original = false;

		ne_fibre_0_idx = INVALID_IDX;
		ne_fibre_1_idx = INVALID_IDX;

		ne_edge = INVALID_IDX;
		ne_edge_tk.init_ticket();

		ne_is_conflict = false;

		ne_tee.init_sortee(true);
		ne_reltee.init_sortrel(true);

		ne_tee.so_dbg_me_class = 2;
		ne_tee.so_me = this;
		ne_tee.so_related = &ne_reltee;
		ne_tee.so_ccl.cc_me = this;

		ne_curr_nemap = NULL_PT;
		//ne_curr_map = NULL_PT;

		ne_recoil_tk.init_ticket();
		ne_deduc_tk.init_ticket();

		ne_num_remote_tees = 0;
	
		ne_spot = false;

		DBG(
			ne_dbg_used_no_orig = false;
			ne_dbg_ccl.cc_me = this;
			ne_dbg_in_used = false;
			ne_dbg_filled_tk.init_ticket();
		)
	}


	bool	is_ne_virgin(){
		bool c1 = (fib_sz() == 0);
		bool c2 = (ne_original == false);
		bool c3 = (ne_fibre_0_idx == INVALID_IDX);
		bool c4 = (ne_fibre_1_idx == INVALID_IDX);
		bool c5 = (ne_edge == INVALID_IDX);
		bool c6 = (! ne_edge_tk.is_valid());
		bool c7 = (ne_is_conflict == false);
		bool c8 = true; BRAIN_DBG(c8 = (ne_dbg_used_no_orig == false);)
		bool c9 = (ne_curr_nemap == NULL_PT);
		//bool c10 = (ne_curr_map == NULL_PT);
		bool c10 = (! ne_recoil_tk.is_valid());

		return (c1 && c2 && c3 && c4 && c5 && c6 && c7 && c8 && c9 && c10);
	}

	long	fib_sz(){ return ne_fibres.size(); }

	bool	ck_tunnels(){
		BRAIN_CK_0(	(ne_fibre_0_idx == INVALID_IDX) || 
				((ne_fibres[0]->qu_tunnels)[ne_fibre_0_idx] == this));
		BRAIN_CK_0(	(ne_fibre_1_idx == INVALID_IDX) ||	
				((ne_fibres[1]->qu_tunnels)[ne_fibre_1_idx] == this));
		return true;
	}


	void	set_spot(brain& brn);
	void	reset_spot(brain& brn);
	bool	has_spot(){
		return ne_spot;
	}

	quanton* forced_quanton(){
		return ne_fibres[0];
	}

	void	set_motives(brain& brn, notekeeper& dke, bool is_first);

	void	swap_fibres_0_1(){
		BRAIN_CK(ck_tunnels());
		quanton* fb_0 = ne_fibres[0];
		ne_fibres[0] = ne_fibres[1];
		ne_fibres[1] = fb_0;

		long fb_0_idx = ne_fibre_0_idx;
		ne_fibre_0_idx = ne_fibre_1_idx;
		ne_fibre_1_idx = fb_0_idx;
		BRAIN_CK(ck_tunnels());
	}

	void	swap_fibres_1(long idx){
		BRAIN_CK(ck_tunnels());
		quanton* fb_1 = ne_fibres[1];
		long	idx_1 = ne_fibre_1_idx;

		ne_fibres[1] = ne_fibres[idx];
		ne_fibres[idx] = fb_1;



		ne_fibres[1]->qu_tunnels.push(this);
		ne_fibre_1_idx = ne_fibres[1]->qu_tunnels.size() - 1;

		fb_1->tunnel_swapop(idx_1);
		BRAIN_CK(ck_tunnels());
	}

	void	swap_fibres_0(long idx){
		BRAIN_CK(ck_tunnels());
		quanton* fb_0 = ne_fibres[0];
		long	idx_0 = ne_fibre_0_idx;

		ne_fibres[0] = ne_fibres[idx];
		ne_fibres[idx] = fb_0;

		ne_fibres[0]->qu_tunnels.push(this);
		ne_fibre_0_idx = ne_fibres[0]->qu_tunnels.size() - 1;

		fb_0->tunnel_swapop(idx_0);
		BRAIN_CK(ck_tunnels());
	}

	void		neu_swap_edge(brain& brn, long ii);
	void		neu_tunnel_signals(brain& brn, quanton& r_qua);

	quanton*	update_fibres(row_quanton_t& synps, bool orig);
	//quanton*	get_prt_fibres(row_quanton_t& tmp_fibres, bool sort_them = true);	
	// sorted by id 

	bool		is_partner_fib(quanton& qua){
		BRAIN_CK(fib_sz() > 1);
		quanton& qua0 = *(ne_fibres[0]);
		quanton& qua1 = *(ne_fibres[1]);
		return ((&qua == &qua0) || (&qua == &qua1));
	}

	quanton&	partner_fib(quanton& qua){
		BRAIN_CK(fib_sz() > 1);
		quanton& qua0 = *(ne_fibres[0]);
		quanton& qua1 = *(ne_fibres[1]);

		if(&qua == &qua0){ return qua1;	}
		BRAIN_CK(&qua == &qua1);
		return qua0;
	}

	void		update_uncharged(brain& brn, quanton* chg_qua);

	bool		ck_all_charges(brain* brn, long from);
	bool		ck_all_has_charge(long& npos);
	bool		ck_no_source_of_any();

	bool	neu_compute_binary(){
		for(long ii = 0; ii < fib_sz(); ii++){
			charge_t chg = ne_fibres[ii]->get_charge();
			if(chg == cg_positive){
				return true;
			}
		}
		return false;
	}

	bool	all_marked_after_idx(brain& brn, long trl_idx);
	bool	is_filled_of_marks(quanton& nxt_qua);
	bool	in_neuromap(long min_tier, long max_tier, bool& has_upper_pos);

	//for IS_SAT_CK
	bool	neu_compute_dots(){
		for(long ii = 0; ii < fib_sz(); ii++){
			charge_t chg = ne_fibres[ii]->qu_dot;
			if(chg == cg_positive){
				return true;
			}
		}
		return false;
	}

	bool	recoiled_in_or_after(ticket tik){
		return (ne_recoil_tk.tk_recoil >= tik.tk_recoil);
	}

	bool	deduced_in_or_after(ticket tik){
		return (ne_deduc_tk.tk_recoil >= tik.tk_recoil);
	}

	bool	in_ne_dominated(brain& brn);
	void	make_ne_dominated(brain& brn);

	void	fill_mutual_sortees(brain& brn);

	bool	has_qua(quanton& tg_qua);

	void	map_set_dbg_ccl(mem_op_t mm, brain& brn);
	void	add_dbg_ccl(brain& brn, row<canon_clause*>& the_ccls, 
						row<neuron*>& the_neus, dima_dims& dims);

	sorset*	get_sorset(){
		return ne_tee.so_vessel;
	}

	bj_ostream& 	print_neu_base(bj_ostream& os, bool detail, bool prt_src, 
								   bool sort_fib);
	bj_ostream&	print_neuron(bj_ostream& os, bool from_pt = false);
};

inline
void
set_all_ne_nemap(row<neuron*>& all_neus, neuromap* nmp)
{
	for(long ii = 0; ii < all_neus.size(); ii++){
		BRAIN_CK(all_neus[ii] != NULL_PT);
		neuron& neu = *(all_neus[ii]);
		BRAIN_CK(neu.ne_curr_nemap != nmp);
		neu.ne_curr_nemap = nmp;
	}
}

inline
void
make_all_ne_dominated(brain& brn, row<neuron*>& all_neus, 
			long first_idx = 0, long last_idx = -1)
{
	if(last_idx < 0){ last_idx = all_neus.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(all_neus.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == all_neus.size()) || all_neus.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		BRAIN_CK(all_neus[ii] != NULL_PT);
		neuron& neu = *(all_neus[ii]);
		neu.make_ne_dominated(brn);
	}
}

//=============================================================================
// prop_signal

class prop_signal {
	public:

	quanton*	ps_quanton;
	neuron*		ps_source;
	long		ps_tier;

	prop_signal(){
		init_prop_signal();
	}

	void	init_prop_signal(quanton* qua = NULL_PT, neuron* the_src = NULL_PT, 
							 long the_tier = INVALID_TIER)
	{
		ps_quanton = qua;
		ps_source = the_src;
		ps_tier = the_tier;
	}

	void	init_qua_signal(quanton& qua){
		init_prop_signal(&qua, qua.qu_source, qua.qu_tier);
	}

	~prop_signal(){
		init_prop_signal();
	}

	bool	is_ps_virgin(){
		bool c1 = (ps_quanton == NULL_PT);
		bool c2 = (ps_source == NULL_PT);
		bool c3 = (ps_tier == INVALID_TIER);

		return (c1 && c2 && c3);
	}

	void	make_ps_dominated(brain& brn){
		if(ps_quanton != NULL_PT){
			ps_quanton->make_qu_dominated(brn);
		}
		if(ps_source != NULL_PT){
			ps_source->make_ne_dominated(brn);
		}
	}
	
	void	update_ps_deduc_tk(brain& brn){
		if(ps_source != NULL_PT){
			ps_source->ne_deduc_tk.update_ticket(brn);
		}
	}
	
	void	set_mark_and_spot(brain& brn){
		if(ps_quanton != NULL_PT){
			ps_quanton->set_mark(brn);
		}
		if(ps_source != NULL_PT){
			ps_source->set_spot(brn);
		}
	}
	
	void	reset_mark_and_spot(brain& brn){
		if(ps_quanton != NULL_PT){
			ps_quanton->reset_mark(brn);
		}
		if(ps_source != NULL_PT){
			ps_source->reset_spot(brn);
		}
	}
	
	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(if(ps_quanton != NULL){ the_brn = ps_quanton->get_dbg_brn(); });
		return the_brn;
	}

	bj_ostream& 	print_prop_signal(bj_ostream& os, bool from_pt = false){
		MARK_USED(from_pt);
		os << "sg={";
		os << ps_quanton;
		if(ps_source == NULL_PT){ os << ".NULL_SRC"; }
		else { os << "." << ((void*)ps_source) << "." << ps_source->ne_index; }
		//os << ps_source << ".";
		//os << ps_tier;
		os << "}";
		os.flush();
		return os;
	}
};

inline
void
make_all_ps_dominated(brain& brn, row<prop_signal>& trace, 
			long first_idx = 0, long last_idx = -1)
{
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		q_sig.make_ps_dominated(brn);
	}
}

inline
void
set_marks_and_spots_of(brain& brn, row<prop_signal>& trace)
{
	for(long ii = 0; ii < trace.size(); ii++){
		prop_signal& q_sig = trace[ii];
		q_sig.set_mark_and_spot(brn);
	}
}

inline
void
reset_marks_and_spots_of(brain& brn, row<prop_signal>& trace)
{
	for(long ii = 0; ii < trace.size(); ii++){
		prop_signal& q_sig = trace[ii];
		q_sig.reset_mark_and_spot(brn);
	}
}

inline
void
append_all_trace_quas(row<prop_signal>& trace, row_quanton_t& all_quas)
{
	for(long ii = 0; ii < trace.size(); ii++){
		prop_signal& q_sig = trace[ii];
		BRAIN_CK(q_sig.ps_quanton != NULL_PT);
		all_quas.push(q_sig.ps_quanton);
	}
}

inline
void
update_all_ps_deduc_tk(brain& brn, row<prop_signal>& trace, 
			long first_idx = 0, long last_idx = -1)
{
	if(last_idx < 0){ last_idx = trace.size(); }

	BRAIN_CK(first_idx <= last_idx);
	BRAIN_CK(trace.is_valid_idx(first_idx));
	BRAIN_CK((last_idx == trace.size()) || trace.is_valid_idx(last_idx));

	for(long ii = first_idx; ii < last_idx; ii++){
		prop_signal& q_sig = trace[ii];
		q_sig.update_ps_deduc_tk(brn);
	}
}

void	set_marks_of(brain& brn, row<prop_signal>& trace, 
			long first_idx = 0, long last_idx = -1, bool with_related = false);
void	reset_marks_of(brain& brn, row<prop_signal>& trace, 
			long first_idx = 0, long last_idx = -1, bool with_related = false);

//=============================================================================
// deduction

class deduction {
	public:

	row_quanton_t		dt_motives;
	quanton*			dt_forced;
	long				dt_target_level;
	//long			dt_forced_level;

	deduction(){
		init_deduction();
	}

	~deduction(){
		init_deduction();
	}

	void	init_deduction(){
		dt_motives.clear();
		dt_forced = NULL_PT;
		dt_target_level = INVALID_LEVEL;
		//dt_forced_level = INVALID_LEVEL;
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(if(dt_forced != NULL){ the_brn = dt_forced->get_dbg_brn(); });
		return the_brn;
	}

	bool	is_dt_virgin(){
		bool c1 = (dt_motives.is_empty());
		bool c2 = (dt_forced == NULL_PT);
		bool c3 = (dt_target_level == INVALID_LEVEL);
		//bool c5 = (dt_forced_level == INVALID_LEVEL);

		bool is_vg = (c1 && c2 && c3);
	
		return is_vg;
	}
	
	void	move_to_dct(deduction& dct2){
		BRAIN_CK(dct2.is_dt_virgin());
		dt_motives.move_to(dct2.dt_motives);

		dct2.dt_forced = dt_forced;
		dct2.dt_target_level = dt_target_level;
		//dct2.dt_forced_level = dt_forced_level;

		init_deduction();
		BRAIN_CK(is_dt_virgin());
	}

	void	set_with(brain& brn, notekeeper& dke, quanton& nxt_qua);

	quanton&	get_forced(){
		BRAIN_CK(dt_forced != NULL_PT);
		return *dt_forced;
	}

	bool	is_dt_singleton(){
		return dt_motives.is_empty();
	}

	bj_ostream&	print_deduction(bj_ostream& os, bool from_pt = false){
		MARK_USED(from_pt);
		os << "dt={ mots=" << dt_motives;
		os << " qu:" << dt_forced;
		os << " lv:" << dt_target_level;
		//os << " fl:" << dt_forced_level;
		os << "}";
		os.flush();
		return os;
	}
};

//=============================================================================
// coloring

class coloring {
	public:
		
	brain*			co_brn;
		
	row_quanton_t	co_quas;
	row<long>		co_qua_colors;
	bool			co_all_qua_consec;

	row<neuron*>		co_neus;
	row<long>		co_neu_colors;
	bool			co_all_neu_consec;

	long			co_szs_idx;

	coloring(brain* pt_brn = NULL_PT){
		init_coloring(pt_brn);
	}

	~coloring(){
		init_coloring();
	}

	void	init_coloring(brain* pt_brn = NULL){
		co_brn = pt_brn;
		
		co_quas.clear();
		co_qua_colors.clear();
		co_all_qua_consec = false;

		co_neus.clear();
		co_neu_colors.clear();
		co_all_neu_consec = false;

		co_szs_idx = INVALID_IDX;
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = co_brn);
		return the_brn;
	}

	bool	is_co_virgin(){
		bool c1 = (co_quas.is_empty());
		bool c2 = (co_qua_colors.is_empty());
		bool c3 = (co_all_qua_consec == false);

		bool c4 = (co_neus.is_empty());
		bool c5 = (co_neu_colors.is_empty());
		bool c6 = (co_all_neu_consec == false);

		bool c7 = (co_szs_idx == INVALID_IDX);

		bool is_vg = (c1 && c2 && c3 && c4 && c5 && c6 && c7);
	
		return is_vg;
	}

	bool	has_diff_col(row<long>& the_colors, long col_idx);

	void	save_colors_from(sort_glb& neus_srg, sort_glb& quas_srg);
	void	load_colors_into(brain& brn, sort_glb& neus_srg, sort_glb& quas_srg, 
							 dima_dims& dims);
	void	add_coloring(brain& brn, coloring& clr);
	//void	calc_dims(brain& brn, dima_dims& dims);

	void	get_initial_sorting_coloring(brain& brn, coloring& clr, bool fill_neus);

	void	move_co_to(coloring& col2);
	void	copy_co_to(coloring& col2);

	bool 	ck_cols(){
		BRAIN_CK(co_quas.size() == co_qua_colors.size());
		BRAIN_CK(co_neus.size() == co_neu_colors.size());
		return true;
	}

	bj_ostream&	print_coloring(bj_ostream& os, bool from_pt = false){
		MARK_USED(from_pt);
		os << "CO(" << (void*)this <<")={ " << bj_eol;
		os << " quas=" << co_quas << bj_eol;
		os << " cols_quas=" << co_qua_colors << bj_eol;
		os << " neus=" << co_neus << bj_eol;
		os << " qu_all=" << co_all_qua_consec;
		os << " ne_all=" << co_all_neu_consec;
		os << " szs_idx=" << co_szs_idx;
		os << bj_eol;
		os << "}";
		os.flush();
		return os;
	}
};

//=============================================================================
// neuromap

// coding this replacement for memaps
class neuromap {
	public:
	long			na_index;		//idx in br_neuromaps
	
	brain*			na_brn;	
	bool			na_active;
	long			na_deact_tier;
	long			na_orig_lv;
	quanton*		na_orig_cho;
	neuromap*		na_submap;
	
	prop_signal		na_confl;

	row<neuron*>		na_all_filled_in_propag;
	row<neuron*>		na_not_selected;
	
	row<prop_signal>	na_forced;
	row<neuron*>		na_non_forced;
	
	coloring		ma_guide_col;
	
	neuromap(brain* pt_brn = NULL) {
		na_index = INVALID_IDX;
		init_neuromap(pt_brn);
	}

	~neuromap(){
		init_neuromap();
	}

	void	init_neuromap(brain* pt_brn = NULL){
		na_brn = pt_brn;		
		na_active = false;
		na_deact_tier = INVALID_TIER;
		na_orig_lv = INVALID_LEVEL;
		na_orig_cho = NULL_PT;
		na_submap = NULL_PT;
		
		na_confl.init_prop_signal();
		
		na_all_filled_in_propag.clear();
		na_not_selected.clear();
		na_forced.clear(true);
		na_non_forced.clear();
		
		ma_guide_col.init_coloring();
	}
	
	bool	is_na_virgin(){
		bool c1 = (na_brn == NULL_PT);
		bool c2 = (na_active == false);
		bool c3 = (na_deact_tier == INVALID_TIER);
		bool c4 = (na_orig_lv == INVALID_LEVEL);
		bool c5 = (na_orig_cho == NULL_PT);
		bool c6 = (na_submap == NULL_PT);

		return (c1 && c2 && c3 && c4 && c5 && c6);
	}

	void	reset_neuromap(brain& brn);
	
	brain&	get_brn(){
		BRAIN_CK(na_brn != NULL);
		return (*na_brn);
	}
	
	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = na_brn);
		return the_brn;
	}

	void	map_get_all_quas(row_quanton_t& all_quas);
	void	map_get_all_neus(row<neuron*>& all_neus);
	
	quanton*	map_choose_quanton();
	
	void	map_set_all_qu_curr_dom();
	void	map_reset_all_qu_curr_dom();

	void	map_set_all_ne_curr_dom();
	void	map_reset_all_ne_curr_dom();

	bool	map_ck_all_qu_dominated();
	bool	map_ck_all_ne_dominated();

	void	map_make_dominated();
	void	map_activate();
	void	map_deactivate();
	
	void	set_all_filled_in_propag();
	void	fill_non_forced_from(row<neuron*>& all_neus, long min_ti, long max_ti);
	void	fill_non_forced();
	
	long	get_min_tier(){
		long mti = INVALID_TIER;
		if(! na_forced.is_empty()){
			quanton* qua = na_forced.last().ps_quanton;
			if(qua != NULL_PT){
				mti = qua->qu_tier;
			}
		}
		return mti;
	}

	long	get_max_tier(){
		long mti = INVALID_TIER;
		if(! na_forced.is_empty()){
			quanton* qua = na_forced.first().ps_quanton;
			if(qua != NULL_PT){
				mti = qua->qu_tier;
			}
		}
		return mti;
	}

	bj_ostream&	print_neuromap(bj_ostream& os, bool from_pt = false);
};

//=============================================================================
// memap

class memap {
	public:
		
	brain*			ma_brn;

	ticket			ma_before_retract_tk;
	row<ticket>		ma_after_retract_tks;

	long			ma_tier;
	prop_signal		ma_confl;
	quanton*		ma_cho;

	row<prop_signal>	ma_dotted;
	row<neuron*>		ma_filled;

	row<long>		ma_szs_dotted;
	row<long>		ma_szs_filled;

	row<neuron*>		ma_fll_in_lv;
	row<neuron*>		ma_discarded;

	coloring		ma_save_guide_col;
	coloring		ma_find_guide_col;

	bool			ma_active;

	memap() {
		ma_active = false;
		init_memap();
	}

	~memap(){
		init_memap();
	}

	void	init_memap(brain* pt_brn = NULL){
		BRAIN_CK(! ma_active);
		
		ma_brn = pt_brn;

		ma_before_retract_tk.init_ticket();
		
		ma_tier = INVALID_TIER;
		ma_confl.init_prop_signal();
		ma_cho = NULL_PT;

		ma_dotted.clear();
		ma_filled.clear();

		ma_szs_dotted.clear();
		ma_szs_filled.clear();

		ma_fll_in_lv.clear();
		ma_discarded.clear();

		ma_save_guide_col.init_coloring(pt_brn);
		ma_find_guide_col.init_coloring(pt_brn);

		ma_active = false;
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = ma_brn);
		return the_brn;
	}

	void	reset_memap(brain& brn);

	bool	is_ma_virgin(){
		bool c2 = ! ma_before_retract_tk.is_valid();
		bool c3 = (ma_confl.is_ps_virgin());
		bool c4 = (ma_cho == NULL_PT);

		bool c5 = (ma_dotted.is_empty());
		bool c6 = (ma_filled.is_empty());

		bool c7 = (ma_szs_dotted.is_empty());
		bool c8 = (ma_szs_filled.is_empty());

		bool c9 = (ma_fll_in_lv.is_empty());
		bool c10 = (ma_discarded.is_empty());

		bool c11 = (ma_save_guide_col.is_co_virgin());
		bool c12 = (ma_find_guide_col.is_co_virgin());

		bool c13 = (ma_active == false);

		bool is_vg = (c2 && c3 && c4 && c5 && c6 && c7 && 
			c8 && c9 && c10 && c11 && c12 && c13);
	
		return is_vg;
	}

	quanton&	map_last_dotted(){
		BRAIN_CK(! ma_dotted.is_empty());
		quanton* pt_nxt_qua = ma_dotted.last().ps_quanton;
		BRAIN_CK(pt_nxt_qua != NULL_PT);

		quanton& nxt_qua = *pt_nxt_qua;
		return nxt_qua;
	}

	void	move_data_to(memap& mpp);

	void	map_record_szs();

	void	set_filled(brain& brn);

	void	map_replace_with(brain& brn, memap& mpp, dbg_call_id dbg_id);

	void	map_set_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls,
							row<neuron*>& the_neus, dima_dims& dims);

	bool	map_ck_simple_no_satisf(mem_op_t mm, brain& brn);
	void	dbg_prepare_used_dbg_cnf(mem_op_t mm, brain& brn, row<canon_clause*>& the_ccls);
	bool	dbg_ck_used_simple_no_satisf(mem_op_t mm, brain& brn);

	void	map_assemble_tees_related(mem_op_t mm, brain& brn);
	void	map_assemble_forced_sorter(mem_op_t mm, brain& brn, long first_idx);

	coloring&	map_guide_coloring(mem_op_t mm);

	void	map_inc_stab_guide(brain& brn, coloring& guide_col);
	bool 	map_prepare_mem_oper(mem_op_t mm, brain& brn);

	void	map_prepare_tees_related(mem_op_t mm, brain& brn);
	void	map_prepare_forced_sorter(mem_op_t mm, brain& brn, long first_idx);

	bool	map_find(brain& brn);
	bool	map_save(brain& brn);
	bool	map_oper(mem_op_t mm, brain& brn);
	bool	map_ck_contained_in(brain& brn, coloring& colr, dbg_call_id dbg_id);

	void	map_dbg_print(bj_ostream& os, mem_op_t mm, brain& brn);

	void	map_get_layer_neus(row<neuron*>& neus, long lyr_idx1, long lyr_idx2, 
							   bool ck_tks);


	void	get_initial_guide_coloring(brain& brn, coloring& clr, long idx_szs);
	void	get_initial_tauto_coloring(brain& brn, coloring& stab_guide_clr, 
									   coloring& base_final_clr, bool ck_tks);

	long	get_save_idx();
	long	get_find_idx();

	long 	get_last_trace_sz(){
		long t_sz = 0;
		if(! ma_szs_dotted.is_empty()){
			t_sz = ma_szs_dotted.last();
		}
		return t_sz;
	}

	long 	get_last_filled_sz(){
		long f_sz = 0;
		if(! ma_szs_filled.is_empty()){
			f_sz = ma_szs_filled.last();
		}
		return f_sz;
	}

	long 	get_trace_sz(mem_op_t mm);
	long 	get_filled_sz(mem_op_t mm);

	bool	in_last2(){
		return (ma_szs_dotted.size() == 2);
	}

	bool 	ck_last_szs(){
		BRAIN_CK(ma_szs_dotted.size() == ma_szs_filled.size());
		BRAIN_CK(get_last_trace_sz() == ma_dotted.size());
		BRAIN_CK(get_last_filled_sz() == ma_filled.size());
		return true;
	}

	bool 	ck_guide_idx(coloring& guide_col, dbg_call_id id);
	bool 	ck_map_guides(dbg_call_id dbg_id);

	bj_ostream&	print_memap(bj_ostream& os, bool from_pt = false);
};

//=============================================================================
// notekeeper

class notekeeper {
	public:
	typedef bool (quanton::*has_fn_pt_t)();
	typedef void (quanton::*do_fn_pt_t)(brain& brn);
	typedef long (*do_row_fn_pt_t)(brain& brn, row_quanton_t& quans);

	brain*			dk_brain;

	long*			dk_note_counter;
	has_fn_pt_t		dk_has_note_fn;
	do_fn_pt_t		dk_set_note_fn;
	do_fn_pt_t		dk_reset_note_fn;
	do_row_fn_pt_t		dk_set_all_fn;
	do_row_fn_pt_t		dk_reset_all_fn;

	long			dk_pop_layer;
	long			dk_note_layer;
	long			dk_tot_noted;
	long 			dk_num_noted_in_layer;

	row_row_quanton_t	dk_motives_by_layer;

	notekeeper(brain* brn = NULL_PT, long tg_lv = INVALID_LEVEL)
	{
		init_notekeeper(brn, tg_lv);
	}

	~notekeeper(){
		init_notekeeper();
	}

	void	init_notekeeper(brain* brn = NULL_PT, long tg_lv = INVALID_LEVEL)
	{
		dk_brain = brn;

		dk_note_counter = NULL_PT;
		dk_has_note_fn = NULL_PT;
		dk_set_note_fn = NULL_PT;
		dk_reset_note_fn = NULL_PT;
		dk_set_all_fn = NULL_PT;
		dk_reset_all_fn = NULL_PT;

		init_notes(tg_lv);
	}
	
	long 	nk_get_counter(){
		BRAIN_CK(dk_note_counter != NULL_PT);
		return *dk_note_counter;
	}
	
	bool	nk_has_note(quanton& qua){
		BRAIN_CK(dk_has_note_fn != NULL_PT);
		return (qua.*dk_has_note_fn)();
	}

	void	nk_set_note(quanton& qua){
		BRAIN_CK(dk_set_note_fn != NULL_PT);
		brain& brn = get_dk_brain();
		(qua.*dk_set_note_fn)(brn);
	}

	void	nk_reset_note(quanton& qua){
		BRAIN_CK(dk_reset_note_fn != NULL_PT);
		brain& brn = get_dk_brain();
		(qua.*dk_reset_note_fn)(brn);
	}
	
	long 	nk_set_all_notes(row_quanton_t& quans){
		BRAIN_CK(dk_set_all_fn != NULL_PT);
		brain& brn = get_dk_brain();
		return (*dk_set_all_fn)(brn, quans);
	}

	long 	nk_reset_all_notes(row_quanton_t& quans){
		BRAIN_CK(dk_reset_all_fn != NULL_PT);
		brain& brn = get_dk_brain();
		return (*dk_reset_all_fn)(brn, quans);
	}

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = dk_brain);
		return the_brn;
	}

	void	init_funcs(long* cnter, has_fn_pt_t has_note, do_fn_pt_t set_note, 
					   do_fn_pt_t reset_note, do_row_fn_pt_t set_all,	
				 do_row_fn_pt_t reset_all_its)
	{
		dk_note_counter = cnter;
		dk_has_note_fn = has_note;
		dk_set_note_fn = set_note;
		dk_reset_note_fn = reset_note;
		dk_set_all_fn = set_all;
		dk_reset_all_fn = reset_all_its;
	}

	void	init_notes(long tg_lv){
		dk_pop_layer = INVALID_IDX;
		dk_note_layer = tg_lv;
		dk_tot_noted = 0;
		dk_num_noted_in_layer = 0;

		dk_motives_by_layer.clear(true, true);
	}

	bool	ck_funcs(){
		bool c0 = (dk_note_counter != NULL_PT);
		bool c1 = (dk_has_note_fn != NULL_PT);
		bool c2 = (dk_set_note_fn != NULL_PT);
		bool c3 = (dk_reset_note_fn != NULL_PT);
		bool c4 = (dk_set_all_fn != NULL_PT);
		bool c5 = (dk_reset_all_fn != NULL_PT);
		return (c0 && c1 && c2 && c3 && c4 && c5);
	}

	brain&	get_dk_brain();

	long 	get_note_counter(){
		BRAIN_CK(dk_note_counter != NULL_PT);
		return (*dk_note_counter);
	}

	long 	get_lv_idx(long lv);

	void	clear_all_motives(long lim_lv = INVALID_LEVEL, bool reset_notes = true);
	
	row_quanton_t&	get_layer(long q_layer);

	bool	ck_notes(row_quanton_t& mots, bool val){

		BRAIN_CK(ck_funcs());
		for(long ii = 0; ii < mots.size(); ii++){

			quanton* qua = mots[ii];
			MARK_USED(qua);
			BRAIN_CK(qua != NULL_PT);

			bool c1 = (qua->*dk_has_note_fn)();
			MARK_USED(c1);
			BRAIN_CK(c1 == val);
		}
		return true;
	}

	void	get_all_ordered_motives(row_quanton_t& mots){
		mots.clear();
		for(long aa = 0; aa < dk_motives_by_layer.size(); aa++){
			row_quanton_t& lv_mots = dk_motives_by_layer[aa];
			lv_mots.append_to(mots);
		}
	}
	
	long	get_num_motives(){
		long nmm = 0;
		for(long aa = 0; aa < dk_motives_by_layer.size(); aa++){
			row_quanton_t& lv_mots = dk_motives_by_layer[aa];
			nmm += lv_mots.size();
		}
		return nmm;

	}
	
	bool	is_empty(){
		return (get_num_motives() == 0);
	}

	void	dec_notes(){
		BRAIN_CK(dk_tot_noted > 0);
		BRAIN_CK(dk_num_noted_in_layer > 0);

		dk_tot_noted--;
		BRAIN_CK(dk_tot_noted >= 0);

		dk_num_noted_in_layer--;
		if(dk_num_noted_in_layer == 0){
			row_quanton_t& mots = get_layer(dk_note_layer);
			BRAIN_CK(ck_notes(mots, false));
			mots.clear();
		}

		BRAIN_CK(dk_num_noted_in_layer >= 0);
	}

	void	update_notes_layer(long q_layer){
		BRAIN_CK(q_layer >= 0);
		BRAIN_CK(q_layer <= dk_note_layer);
		if(q_layer < dk_note_layer){
			row_quanton_t& nxt_mots = get_layer(q_layer);

			BRAIN_CK(dk_num_noted_in_layer == 0);
			BRAIN_CK(ck_empty_layers(q_layer));
			BRAIN_CK(ck_notes(nxt_mots, true));

			dk_note_layer = q_layer;
			dk_num_noted_in_layer = nxt_mots.size();
		}
		BRAIN_CK(q_layer == dk_note_layer);
	}

	bool	ck_empty_layers(long q_layer){
		BRAIN_CK(q_layer >= 0);
		BRAIN_CK(q_layer < dk_note_layer);
		BRAIN_CK(! dk_motives_by_layer.is_empty());
		
		long l_idx = dk_motives_by_layer.last_idx();
		BRAIN_CK(q_layer <= l_idx);
		for(long ii = l_idx; ii > q_layer; ii--){
			BRAIN_CK(get_layer(ii).is_empty());
		}
		return true;
	}

	void	set_motive_notes(row_quanton_t& rr_qua, long from, long until);

	void	add_motive(quanton& qua, long q_layer){
		row_quanton_t& layer_mots = get_layer(q_layer);
		layer_mots.push(&qua);
		if(q_layer > dk_pop_layer){
			dk_pop_layer = q_layer;
		}
	}

	long	get_nxt_busy_layer(long lyr, bool with_restart = false){
		row_row_quanton_t& mots_by_ly = dk_motives_by_layer;
		if(mots_by_ly.is_empty()){
			return INVALID_IDX;
		}
		long nxt_lyr = lyr;
		if(with_restart){
			nxt_lyr = mots_by_ly.last_idx();
		}
		while(mots_by_ly.is_valid_idx(nxt_lyr) && mots_by_ly[nxt_lyr].is_empty()){
			nxt_lyr--;
		}
		BRAIN_CK((nxt_lyr == INVALID_IDX) || mots_by_ly.is_valid_idx(nxt_lyr));
		return nxt_lyr;
	}

	void		update_pop_layer(bool with_restart = false){
		dk_pop_layer = get_nxt_busy_layer(dk_pop_layer, with_restart);
	}

	quanton*	pop_motive(){
		row_row_quanton_t& mots_by_ly = dk_motives_by_layer;
		BRAIN_CK(ck_pop_layer());
		if(! mots_by_ly.is_valid_idx(dk_pop_layer)){
			return NULL_PT;
		}
		row_quanton_t& mots = mots_by_ly[dk_pop_layer];
		if(mots.is_empty()){
			return NULL_PT;
		}
		quanton* qua = mots.pop();
		if(mots.is_empty()){
			update_pop_layer();
		}
		return qua;
	}

	quanton*	last_motive(){
		row_row_quanton_t& mots_by_ly = dk_motives_by_layer;
		BRAIN_CK(ck_pop_layer());
		if(! mots_by_ly.is_valid_idx(dk_pop_layer)){
			return NULL_PT;
		}
		row_quanton_t& mots = mots_by_ly[dk_pop_layer];
		if(mots.is_empty()){
			return NULL_PT;
		}
		quanton* qua = mots.last();
		return qua;
	}

	bool		ck_pop_layer(){
		row_row_quanton_t& mots_by_ly = dk_motives_by_layer;
		if(mots_by_ly.is_empty()){
			BRAIN_CK(dk_pop_layer == INVALID_IDX);
			return true;
		}
		long p_ly = mots_by_ly.last_idx();
		while(mots_by_ly.is_valid_idx(p_ly) && (p_ly > dk_pop_layer)){
			BRAIN_CK(mots_by_ly[p_ly].is_empty());
			p_ly--;
		}
		BRAIN_CK(p_ly == dk_pop_layer);
		if(mots_by_ly.is_valid_idx(dk_pop_layer)){
			BRAIN_CK(! mots_by_ly[dk_pop_layer].is_empty());

		}
		return true;
	}

	bool	has_motives(){
		BRAIN_CK(ck_pop_layer());
		if(dk_pop_layer == INVALID_IDX){
			return false;
		}
		return true;
	}

	long	last_qlevel(){
		quanton* qua = last_motive();
		if(qua == NULL_PT){
			return 0;
		}
		return qua->qlevel();
	}

	long	last_qtier(){
		quanton* qua = last_motive();
		if(qua == NULL_PT){
			return 0;
		}
		return qua->qu_tier;
	}
};

//=============================================================================
// nkref

class nkref {
	public:
	notekeeper*	nr_keeper;
	long		nr_curr_qua_layer;
	long		nr_curr_qua_idx;
	
	nkref(notekeeper* the_kpr = NULL_PT){
		init_nkref(the_kpr);
	}

	~nkref(){
		init_nkref();
	}

	void	init_nkref(notekeeper* the_kpr = NULL_PT){
		nr_keeper = the_kpr;
		nr_curr_qua_layer = INVALID_IDX;
		nr_curr_qua_idx = INVALID_IDX;
	}
	
	bool	is_nr_virgin(){
		bool c1 = (nr_keeper == NULL_PT);
		bool c2 = (nr_curr_qua_layer == INVALID_IDX);
		bool c3 = (nr_curr_qua_idx == INVALID_IDX);

		return (c1 && c2 && c3);
	}
	
	notekeeper& 	get_keeper(){
		BRAIN_CK(nr_keeper != NULL_PT);
		return *nr_keeper;
	}

	void 	update_curr_quanton(bool reset = false){
		if(nr_keeper == NULL_PT){
			return;
		}
		BRAIN_CK(nr_keeper != NULL_PT);
		nr_curr_qua_layer = nr_keeper->get_nxt_busy_layer(nr_curr_qua_layer, reset);
		nr_curr_qua_idx = INVALID_IDX;
		
		if(nr_curr_qua_layer != INVALID_IDX){
			nr_curr_qua_idx = nr_keeper->dk_motives_by_layer[nr_curr_qua_layer].last_idx();
		}
	}

	void 	reset_curr_quanton(){
		if(nr_keeper == NULL_PT){
			return;
		}
		BRAIN_CK(nr_keeper != NULL_PT);
		nr_curr_qua_layer = INVALID_IDX;
		update_curr_quanton(true);
	}
	
	void 	dec_curr_quanton(){
		if(nr_keeper == NULL_PT){
			return;
		}
		BRAIN_CK(nr_keeper != NULL_PT);
		BRAIN_CK(nr_curr_qua_idx >= INVALID_IDX);
		if(nr_curr_qua_idx == INVALID_IDX){
			return;
		}
		nr_curr_qua_idx--;
		if(nr_curr_qua_idx == INVALID_IDX){
			BRAIN_CK(nr_curr_qua_layer >= 0);
			nr_curr_qua_layer--;
			if(nr_curr_qua_layer == INVALID_IDX){
				return;
			}
			update_curr_quanton();
		}
	}
	
	quanton*	get_curr_quanton(){
		if(nr_keeper == NULL_PT){
			return NULL_PT;
		}
		if(nr_curr_qua_idx == INVALID_IDX){
			return NULL_PT;
		}
		BRAIN_CK(nr_keeper != NULL_PT);
		BRAIN_CK(nr_keeper->dk_motives_by_layer.is_valid_idx(nr_curr_qua_layer));
		row_quanton_t& lv_mots = nr_keeper->dk_motives_by_layer[nr_curr_qua_layer];
		
		BRAIN_CK(lv_mots.is_valid_idx(nr_curr_qua_idx));
		
		quanton* nxt_qua = lv_mots[nr_curr_qua_idx];
		BRAIN_CK(nxt_qua != NULL_PT);
		return nxt_qua;
	}
	
	long	get_curr_qlevel(){
		long lv = INVALID_LEVEL;
		quanton* qua = get_curr_quanton();
		if(qua != NULL_PT){
			lv = qua->qlevel();
		}
		return lv;
	}
	
	bool		has_curr_quanton(){
		return 	(get_curr_quanton() != NULL_PT);
	}
	
};

//=============================================================================
// deducer

class deducer {
	public:

	brain*			de_brain;

	notekeeper		de_dct_nkpr;
	notekeeper		de_nmp_nkpr;
	
	nkref			de_dct_ref;
	nkref			de_nmp_ref;
	
	row<prop_signal>	de_all_dct_noted;
	row<prop_signal>	de_all_nmp_noted;

	prop_signal 	de_first_bk_psig;
	prop_signal 	de_next_bk_psig;
	
	neuromap*		de_nxt_nmp;
	deduction		de_nxt_dct;

	deducer(){
		init_deducer();
	}

	~deducer(){
		init_deducer();
	}

	void	init_deducer(brain* brn = NULL_PT);

	static
	void	init_nk_with_dots(notekeeper& nkpr, brain* brn, long tg_lv);
	
	static
	void	init_nk_with_note0(notekeeper& nkpr, brain* brn, long tg_lv);

	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = de_brain);
		return the_brn;
	}

	brain&		get_de_brain();
	notekeeper& get_orig_trail();
	
	neuron* 	tg_confl(){
		BRAIN_CK(de_first_bk_psig.ps_source != NULL_PT);
		return de_first_bk_psig.ps_source;
	}
	
	neuromap&	nxt_nmp(){
		BRAIN_CK(de_nxt_nmp != NULL_PT);
		return *de_nxt_nmp;
	}
	
	bool	is_end_of_dct(){
		long num_ly_notes = de_dct_nkpr.dk_num_noted_in_layer;
		BRAIN_CK(num_ly_notes >= 0);
		return (num_ly_notes == 0);
	}
	
	bool	is_end_of_lv(){
		bool eolv = (de_next_bk_psig.ps_source == NULL_PT);
		return eolv;
	}
	
	bool	ck_end_of_nmp();
	
	bool	is_end_of_nmp(){
		bool eonm = (is_end_of_lv() || ! de_next_bk_psig.ps_source->ne_original);
		return eonm;
	}
	
	void	fill_dct(notekeeper& nkpr, nkref& nkr, deduction& dct);
	void	update_all_deduction_noted();
	//void	fill_nmp(notekeeper& nkpr, nkref& nkr);
	
	void 		deduction_analysis(prop_signal const & confl, deduction& dct);
	void 		neuromap_write_analysis(deduction& dct, row<neuromap*>& all_nmps);
	neuromap* 	neuromap_find_analysis(deduction& dct);
	void 		neuromap_setup_analysis(deduction& dct, row<neuromap*>& all_nmps);
	
	void		find_next_source(notekeeper& nkpr, nkref& ref, row<prop_signal>& all_noted,
									   bool only_origs = false);
	void		find_next_noted(notekeeper& nkpr, nkref& ref);

	neuromap*	update_neuromap(neuromap* prev_nmp);
	neuromap*	calc_neuromap(prop_signal const & confl_sg, long min_lv, 
							  neuromap* prev_nmp);
	void 		end_calc_neuromap();
};

//=============================================================================
// leveldat

class leveldat {
	public:
	
	brain*			ld_brn;
		
	long			ld_idx;
	memap			ld_map0;
	row<prop_signal>	ld_pre_sigs;
	row<neuron*>		ld_learned;
	quanton*		ld_chosen;
	row_quanton_t		ld_upper_quas;

	row_quanton_t		ld_semi_monos_to_update;

	quanton*		ld_first_learned;
	
	row<neuromap*>	ld_all_nmps_to_write;
	neuromap*		ld_nmp_of_lv;

	leveldat(brain* pt_brn = NULL) {
		init_leveldat(pt_brn);
	}

	~leveldat(){
		init_leveldat();
	}

	void	init_leveldat(brain* pt_brn = NULL){
		ld_brn = pt_brn;
		ld_idx = INVALID_IDX;
		ld_map0.init_memap(pt_brn);
		ld_pre_sigs.clear();
		ld_learned.clear();
		ld_chosen = NULL_PT;
		ld_upper_quas.clear();

		BRAIN_CK(ld_semi_monos_to_update.is_empty());
		ld_semi_monos_to_update.clear();

		ld_first_learned = NULL_PT;
		
		ld_all_nmps_to_write.clear();
		ld_nmp_of_lv = NULL_PT;
	}
	
	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = ld_brn);
		return the_brn;
	}
	
	static
	leveldat* create_leveldat(brain* pt_brn = NULL){
		leveldat* lv = tpl_malloc<leveldat>();
		new (lv) leveldat();
		lv->ld_brn = pt_brn;
		return lv;
	}

	static
	void release_leveldat(leveldat* lv){
		BRAIN_CK(lv != NULL_PT);
		BRAIN_CK(lv->ld_all_nmps_to_write.is_empty());
		BRAIN_CK(lv->ld_nmp_of_lv == NULL_PT);
		lv->~leveldat();
		tpl_free<leveldat>(lv);
	}
	
	bool 	has_learned(){
		return ! ld_learned.is_empty();
	}

	long 	num_learned(){
		return ld_learned.size();
	}
	
	void	reset_semi_monos(brain& brn);
	void	release_learned(brain& brn);

	bj_ostream&	print_leveldat(bj_ostream& os, bool from_pt = false){
		if(from_pt){
			os << "[LV=" << ld_idx << ".cho=" << ld_chosen << "]";
			return os;
		}
		os << "LVDAT(" << (void*)this <<")={" << bj_eol;
		os << " ld_map0=" << ld_map0 << bj_eol;
		os << " ld_chosen=" << ld_chosen << bj_eol;
		os << " ld_upper_quas=" << ld_upper_quas << bj_eol;
		os << " ld_semi_monos_to_update=" << ld_semi_monos_to_update << bj_eol;
		os << "}";
		os.flush();
		return os;
	}
};

//=================================================================
// dbg_inst_info

#ifdef FULL_DEBUG

class dbg_inst_info {
public:
	long	dbg_before_retract_lv;
	long	dbg_last_recoil_lv;
	
	deduction	dbg_deduc;
	
	row<neuron*>	 	dbg_simple_neus;
	row<neuron*>	 	dbg_used_neus;
	row<canon_clause*> 	dbg_ccls;
	canon_cnf		dbg_cnf;

	bool	dbg_do_finds;
	
	bj_big_int_t	dbg_find_id;
	bj_big_int_t	dbg_save_id;
	bj_big_int_t	dbg_canon_find_id;
	bj_big_int_t	dbg_canon_save_id;

	row<neuron*>	 	dbg_original_used;
	row_quanton_t		dbg_all_chosen;
	
	bool	dbg_ic_active;
	bool	dbg_ic_after;
	long	dbg_ic_max_seq;
	long	dbg_ic_seq;
	bool	dbg_ic_gen_jpg;

	bool	dbg_just_read;
	bool	dbg_clean_code;
	
	bool	dbg_periodic_prt;
	
	row<debug_entry>	dbg_start_dbg_entries;
	row<debug_entry>	dbg_stop_dbg_entries;
	long			dbg_current_start_entry;
	long			dbg_current_stop_entry;
	
	bool 		dbg_bad_cycle1;
	row<bool>	dbg_levs_arr;
	
	dbg_inst_info(){
		init_dbg_inst_info();
	}

	void	init_dbg_inst_info();

};

#endif

//=============================================================================
// brain

void		due_periodic_prt(void* pm, double curr_secs);

class brain {
private:
	brain&  operator = (brain& other){
		throw instance_exception(inx_bad_eq_op);
	}

	brain(brain& other){ 
		throw instance_exception(inx_bad_creat);
	}
	
public:

	BRAIN_DBG(
		dbg_inst_info  	br_dbg;
		brain*			br_pt_brn;
	)
	
	solver* 		br_pt_slvr;
		
	timer			br_prt_timer;

	double 			br_start_load_tm;
	double 			br_start_solve_tm;

	row<leveldat*>		br_data_levels;
	
	// temporal attributes
	row_quanton_t		br_tmp_fixing_quantons;
	row_quanton_t		br_tmp_load_quantons;
	row_quanton_t		br_tmp_assig_quantons;

	row_quanton_t		br_tmp_motives;
	row_quanton_t		br_tmp_edge;
	row_quanton_t		br_tmp_choose;
	row_quanton_t		br_tmp_qu_fill_nmp;
	row_quanton_t		br_tmp_qu_activate;
	row<neuron*>		br_tmp_ne_activate;
	row_quanton_t		br_tmp_qu_dom;
	row<neuron*>		br_tmp_ne_dom;
	row<neuron*>		br_tmp_ne_fill_nmp;

	row_quanton_t		br_tmp_rever_quas;

	// config attributes
	ch_string		br_file_name;
	ch_string		br_file_name_in_ic;

	// state attributes
	ticket			br_current_ticket;

	//row_quanton_t		satisfying;		// charges after finishing
	k_row<quanton>		br_positons;	// all quantons with positive charge
	k_row<quanton>		br_negatons;	// all quantons with negative charge

	notekeeper		br_charge_trail;
	row_quanton_t		br_tmp_trail;		// in time of charging order

	charge_t		br_choice_spin;
	long			br_choice_order;

	//long			br_choices_lim;	// last known choice idx
	row_quanton_t		br_choices;	// to find non charged quantons quickly
	row_quanton_t		br_chosen;	// the in 'root' level + chosen ones

	k_row<neuron>		br_neurons;	// all neurons
	row<neuron*>		br_free_neurons;
	long 			br_num_active_neurons;

	row<neuron*>		br_unit_neurons;

	k_row<neuromap>		br_neuromaps;	// all maps
	row<neuromap*>		br_free_neuromaps;
	long 			br_num_active_neuromaps;

	long			br_first_psignal;
	long			br_last_psignal;
	row<prop_signal>	br_psignals;	// forward propagated signals
	row<prop_signal>	br_delayed_psignals;

	deducer			br_deducer;
	
	deduction		br_retract_dct;
	//deduction		br_retract_nxt_dct;

	notekeeper		br_retract_nke0;
	memap			br_retract_map0;
	bool			br_retract_is_first_lv;

	row_quanton_t		br_semi_monos_to_update;
	
	row<sortee*> 		br_tmp_wrt_tauto_tees;
	row<sortee*> 		br_tmp_wrt_guide_tees;

	row<canon_clause*> 	br_tmp_wrt_tauto_ccls;
	row<canon_clause*> 	br_tmp_wrt_diff_ccls;
	row<canon_clause*> 	br_tmp_wrt_guide_ccls;

	canon_cnf		br_tmp_wrt_tauto_cnf;
	canon_cnf		br_tmp_wrt_diff_cnf;
	canon_cnf		br_tmp_wrt_guide_cnf;

	prop_signal		br_conflict_found;

	row<neuromap*>	br_maps_active;

	sort_glb 		br_forced_srg;
	sort_glb 		br_filled_srg;
	bool			br_quick_mem_oper;

	sort_glb 		br_guide_neus_srg;
	sort_glb 		br_guide_quas_srg;

	sort_glb 		br_tauto_neus_srg;
	sort_glb 		br_tauto_quas_srg;

	sort_glb 		br_clls_srg;

	long 			br_num_memo;

	//neuron			br_root_conflict;
	//quanton			br_conflict_quanton;
	quanton			br_top_block;

	long			br_tot_qu_dots;
	long			br_tot_qu_marks;
	long			br_tot_ne_spots;

	row<neuron*>		br_tmp_found_neus;
	row<neuron*>		br_tmp_selected;
	row<neuron*>		br_tmp_not_selected;
	row<neuron*>		br_tmp_discarded;
	row<neuron*>		br_tmp_ck_sat_neus;
	row<neuron*>		br_tmp_prt_neus;
	row<neuron*>		br_tmp_stab_neus;

	row_quanton_t		br_tmp_stab_quas;
	row_quanton_t		br_tmp_sorted_quas;

	long			br_qu_tot_note0;
	long			br_qu_tot_note1;
	long			br_qu_tot_note2;
	long			br_qu_tot_note3;
	long			br_qu_tot_note4;
	long			br_qu_tot_note5;

	// final message

	bj_ostr_stream	br_final_msg;

	// methods

	brain(solver& ss);
	~brain();

	void	init_brain(solver& ss);

	void	init_all_dbg_brn();
	
	brain*	get_dbg_brn(){
		brain* the_brn = NULL;
		BRAIN_DBG(the_brn = br_pt_brn);
		return the_brn;
	}
	
	void	all_mutual_init(){
		br_guide_neus_srg.stab_mutual_init();
		br_guide_quas_srg.stab_mutual_init();

		br_tauto_neus_srg.stab_mutual_init();
		br_tauto_quas_srg.stab_mutual_init();
	}

	void	release_brain();

	solver& 		get_solver();
	skeleton_glb& 	get_skeleton();
	instance_info&	get_my_inst();	
	bj_output_t&	get_out_info();

	// core methods

	long		brn_tunnel_signals(bool only_in_dom);
	quanton*	choose_quanton();

	bool		dbg_ck_deducs(deduction& dct1, deduction& dct2);

	long		propagate_signals();
	void		pulsate();
	void		start_propagation(quanton& qua);

	void		init_forced_sorter();

	// psignal methods

	void	put_psignal(quanton& qua, neuron* src, long max_tier){
		BRAIN_CK(br_first_psignal <= br_last_psignal);
		if(br_psignals.is_empty()){
			br_psignals.inc_sz();
		}
		br_last_psignal++;
		if(br_last_psignal == br_psignals.size()){
			br_psignals.inc_sz();
		}
		BRAIN_CK(br_psignals.is_valid_idx(br_last_psignal));
		prop_signal& sgnl = br_psignals[br_last_psignal];
		sgnl.init_prop_signal(&qua, src, max_tier);
	}

	prop_signal&	pick_psignal(){
		br_first_psignal++;
		BRAIN_CK(br_first_psignal <= br_last_psignal);
		BRAIN_CK(br_psignals.is_valid_idx(br_first_psignal));
		prop_signal& sgnl = br_psignals[br_first_psignal];
		return sgnl;
	}

	bool	has_psignals(){
		BRAIN_CK(br_first_psignal <= br_last_psignal);
		return (br_first_psignal < br_last_psignal);
	}

	long	num_psignals(){
		BRAIN_CK(br_first_psignal <= br_last_psignal);
		return (br_last_psignal - br_first_psignal);
	}

	void	reset_psignals(){
		br_first_psignal = 0;
		br_last_psignal = 0;
	}

	bool	has_reset_psignals(){
		return ((br_first_psignal == 0) && (br_last_psignal == 0));
	}

	void	send_psignal(quanton& qua, neuron* neu, long max_tier){ 
		BRAIN_CK((max_tier > 0) || (level() == ROOT_LEVEL));
		put_psignal(qua, neu, max_tier);
	}

	void	send_row_psignals(row<prop_signal>& to_send){ 
		for(long aa = 0; aa < to_send.size(); aa++){
			prop_signal& psig = to_send[aa];
			BRAIN_CK(psig.ps_quanton != NULL_PT);

			send_psignal(*psig.ps_quanton, psig.ps_source, psig.ps_tier);
		}
	}

	quanton*	receive_psignal(bool only_in_dom);

	bj_ostream& 	print_psignals(bj_ostream& os, bool just_qua = false){
		os << "[";
		for(long aa = br_first_psignal + 1; aa <= br_last_psignal; aa++){
			if(br_psignals.is_valid_idx(aa)){
				if(just_qua){
					os << br_psignals[aa].ps_quanton << ", ";
				} else {
					os << br_psignals[aa] << bj_eol;
				}
			}
		}
		os << "]";
		os.flush();
		return os;
	}

	// aux memaps

	void	deactivate_last_map();
	void	close_all_maps();

	void	print_active_maps(bj_ostream& os);

	// aux neuromaps

	neuromap&	locate_neuromap(){
		neuromap* pt_nmp = NULL_PT;
		if(! br_free_neuromaps.is_empty()){
			pt_nmp = br_free_neuromaps.pop();
			long nmp_idx = pt_nmp->na_index;
			MARK_USED(nmp_idx);
			BRAIN_CK(br_neuromaps.is_valid_idx(nmp_idx));
			BRAIN_CK(&(br_neuromaps[nmp_idx]) == pt_nmp);
		} else {
			pt_nmp = &(br_neuromaps.inc_sz());
			pt_nmp->na_index = br_neuromaps.last_idx();
		}
		BRAIN_CK(pt_nmp != NULL);

		neuromap& nmp = *pt_nmp;
		BRAIN_CK(nmp.na_index != INVALID_IDX);
		BRAIN_CK(nmp.is_na_virgin());
		
		nmp.na_brn = this;

		br_num_active_neuromaps++;
		return nmp;
	}

	void	release_neuromap(neuromap& nmp){
		BRAIN_CK(! br_neuromaps.is_empty());
		br_free_neuromaps.push(&nmp);
		nmp.init_neuromap();

		BRAIN_CK(nmp.is_na_virgin());

		br_num_active_neuromaps--;
	}

	// aux neuron

	neuron&	locate_neuron(){
		neuron* pt_neu = NULL_PT;
		if(! br_free_neurons.is_empty()){
			pt_neu = br_free_neurons.pop();
			long neu_idx = pt_neu->ne_index;
			MARK_USED(neu_idx);
			BRAIN_CK(br_neurons.is_valid_idx(neu_idx));
			BRAIN_CK(&(br_neurons[neu_idx]) == pt_neu);
		} else {
			pt_neu = &(br_neurons.inc_sz());
			pt_neu->ne_index = br_neurons.size() - 1;
		}
		BRAIN_CK(pt_neu != NULL);

		neuron& neu = *pt_neu;
		BRAIN_CK(neu.ne_index != INVALID_IDX);
		BRAIN_CK(neu.is_ne_virgin());
		
		BRAIN_DBG(neu.ne_pt_brn = this);

		br_num_active_neurons++;
		return neu;
	}

	void	release_neuron(neuron& neu){
		BRAIN_CK(! br_neurons.is_empty());
		br_free_neurons.push(&neu);
		neu.init_neuron();

		BRAIN_CK(neu.is_ne_virgin());

		br_num_active_neurons--;
	}

	// aux methods

	void	reset_conflict(){
		br_conflict_found.init_prop_signal();
	}
	
	bool 	found_conflict(){
		bool h_cfl = (br_conflict_found.ps_quanton != NULL);
		BRAIN_CK(! h_cfl || (br_conflict_found.ps_tier != INVALID_TIER));
		return h_cfl;
	}

	void	set_file_name_in_ic(ch_string f_nam = "");
	void	config_brain(ch_string f_nam = "");
	void	init_loading(long num_qua, long num_neu);
	void	init_uncharged();

	neuron*	add_neuron(row_quanton_t& quans, quanton*& forced_qua, bool orig);
	neuron*	learn_mots(row_quanton_t& the_mots, quanton& forced_qua);

	quanton*	get_quanton(long q_id);
	
	recoil_counter_t	recoil(){
		return br_current_ticket.tk_recoil;
	}

	void	inc_recoil(){
		br_current_ticket.tk_recoil++;
	}

	long		level(){
		BRAIN_CK(br_data_levels.size() == (br_current_ticket.tk_level + 1));
		return br_current_ticket.tk_level;
	}

	leveldat&	data_level(){
		BRAIN_CK(! br_data_levels.is_empty());
		leveldat* pt_lv = br_data_levels.last();
		BRAIN_CK(pt_lv != NULL);
		return *pt_lv;
	}

	leveldat&	inc_data_levels(){
		leveldat* pt_lv = leveldat::create_leveldat(this);
		BRAIN_CK(pt_lv != NULL);
		br_data_levels.push(pt_lv);
		leveldat& dat_lv = *pt_lv;
		dat_lv.ld_idx = br_data_levels.last_idx();
		return dat_lv;
	}
	
	void	inc_level(quanton& qua){
		(br_current_ticket.tk_level)++;
		
		leveldat& dat_lv = inc_data_levels();
		dat_lv.ld_chosen = &qua;
	}

	void	dec_level(){
		br_current_ticket.tk_level--;

		brain& brn = *this;
		data_level().reset_semi_monos(brn);
		data_level().release_learned(brn);

		leveldat* lv = br_data_levels.pop();
		leveldat::release_leveldat(lv);
	}

	long	trail_level(){
		return br_charge_trail.last_qlevel();
	}

	bool	in_edge_of_level();
	bool	can_write_reverse_map(deduction& dct);
	bool	in_edge_of_target_lv(deduction& dct);


	quanton*	curr_choice(){
		leveldat* lv = br_data_levels.last();
		BRAIN_CK(lv != NULL);
		return lv->ld_chosen;
	}

	bool 	lv_has_learned(){
		return data_level().has_learned();
	}

	long 	lv_num_learned(){
		return data_level().num_learned();
	}

	void	update_semi_monos();

	void	retract_choice();
	void	retract_to(long tg_lv = ROOT_LEVEL);
	void	reverse();
	void	new_reverse();

	bool	map_get_sorted_clauses_of(row<neuron*>& neus, long neus_sz, 
			row<sortee*>& qtees, row<canon_clause*>& sorted_ccls);

	bool	map_sort_filled_with(mem_op_t mm, row<neuron*>& filled, long filled_sz, 
			row<sortee*>& all_q_tees, row<canon_clause*>& sorted_ccls);


	void	fill_mutual_tees(sort_glb& neus_srg, sort_glb& quas_srg, 
							 row<neuron*>& all_neus, row_quanton_t& all_quas);

	//void	stab_it();

	long 	tier(){
		long tr = br_charge_trail.last_qtier();
		return tr;
	}

	quanton&	trail_last(){
		quanton* qua = br_charge_trail.last_motive();
		BRAIN_CK(qua != NULL_PT);
		return *qua;
	}

	void	init_mem_tmps(){
		br_tmp_wrt_tauto_ccls.clear();
		br_tmp_wrt_guide_ccls.clear();
		br_tmp_wrt_diff_ccls.clear();

		br_tmp_wrt_tauto_cnf.clear_cnf();
		br_tmp_wrt_diff_cnf.clear_cnf();
		br_tmp_wrt_guide_cnf.clear_cnf();
	}

	// aux func

	void 	init_tots_notes(){
		br_qu_tot_note0 = 0;
		br_qu_tot_note1 = 0;
		br_qu_tot_note2 = 0;
		br_qu_tot_note3 = 0;
		br_qu_tot_note4 = 0;
		br_qu_tot_note5 = 0;
	}

	bool	ck_choices(bool after = false);
	bool	ck_trail();
	void	print_trail(bj_ostream& os, bool no_src_only = false);

	bj_ostream& 	print_all_quantons(bj_ostream& os, long ln_sz, ch_string ln_fd);

	bool	brn_dbg_compute_binary(row<neuron*>& neus);
	bool	brn_dbg_compute_dots(row<neuron*>& neus);
	bool	brn_dbg_compute_dots_of(row<neuron*>& neus, row_quanton_t& assig);

	void		read_cnf(dimacs_loader& ldr);
	void		parse_cnf(dimacs_loader& ldr, row<long>& all_ccls);
	void		load_neuron(row_quanton_t& neu_quas);
	bool		load_brain(long num_neu, long num_var, row_long_t& load_ccls);
	
	bool		load_instance();
	void		aux_solve_instance();
	
	bj_satisf_val_t 	solve_instance();

	neuromap*	get_last_upper_map(){
		neuromap* up_dom = NULL_PT;
		if(! br_maps_active.is_empty()){
			up_dom = br_maps_active.last();
			BRAIN_CK(up_dom != NULL_PT);
		}
		return up_dom;
	}

	void		fill_with_origs(row<neuron*>& neus);

	void		check_timeout();
	void		dbg_check_sat_assig();

	void		set_result(bj_satisf_val_t re);
	bj_satisf_val_t	get_result();

	void		dbg_add_to_used(neuron& neu);
	
	void 	dbg_prt_lvs_have_learned(bj_ostream& os);
	void 	dbg_prt_lvs_active(bj_ostream& os);
	void 	dbg_prt_lvs_virgin(bj_ostream& os);
	void 	dbg_prt_lvs_cho(bj_ostream& os);

	void		print_active_blocks(bj_ostream& os);

	bj_ostream& 	print_brain(bj_ostream& os);

	bj_ostream&	print_all_original(bj_ostream& os);
};

//=============================================================================
// INLINES dependant on class declarations  

inline 
void
ticket::update_ticket(brain& brn){
	tk_recoil = brn.recoil();
	tk_level = brn.level();
}

inline 
void			
quanton::tunnel_swapop(long idx_pop){
	long idx_mov = qu_tunnels.size() - 1;
	neuron* neu2 = qu_tunnels.last();
	BRAIN_CK(neu2->ck_tunnels());
	qu_tunnels.swapop(idx_pop);
	if(idx_pop != idx_mov){ 
		BRAIN_CK(qu_tunnels[idx_pop] == neu2);
		if(neu2->ne_fibres[0] == this){
			BRAIN_CK(neu2->ne_fibre_0_idx == idx_mov);
			neu2->ne_fibre_0_idx = idx_pop;
		} else {
			BRAIN_CK(neu2->ne_fibres[1] == this);
			BRAIN_CK(neu2->ne_fibre_1_idx == idx_mov);
			neu2->ne_fibre_1_idx = idx_pop;
		}
		BRAIN_CK(neu2->ck_tunnels());
	}
}

inline 
void			
quanton::set_source(brain& brn, neuron* neu){
	BRAIN_CK((qu_source == NULL_PT) || (qu_inverse->qu_source == NULL_PT));
	BRAIN_CK((qu_inverse->qu_source == NULL_PT) || (neu == NULL_PT));
	BRAIN_CK((neu == NULL_PT) || neu->ne_original || ! neu->ne_dbg_used_no_orig);

	BRAIN_DBG(
		if((neu != NULL_PT) && ! neu->ne_original){
			BRAIN_CK(neu->ne_dbg_used_no_orig == false);
			neu->ne_dbg_used_no_orig = true;
		}

		if(! has_charge()){
			BRAIN_CK(neu == NULL_PT);
			BRAIN_CK(qu_inverse->qu_source == NULL_PT);
			if((qu_source != NULL_PT) && ! qu_source->ne_original){
				qu_source->ne_dbg_used_no_orig = false;
			}		
		}
	)
	
	qu_source = neu;
	if(neu == NULL_PT){ 
		qu_inverse->qu_source = NULL_PT;
		return; 
	}

	BRAIN_CK_0(qu_source != NULL_PT);
	BRAIN_CK(qu_source->ck_all_charges(&brn, 1));
}

inline
neuron*
quanton::get_source(){
	if(qu_source == NULL_PT){
		//BRAIN_CK(qu_inverse->qu_source == NULL_PT);
		return qu_inverse->qu_source;
	}

	BRAIN_CK((qu_source == NULL_PT) || ! qu_source->is_ne_virgin());
	return qu_source;
}

inline
void
quanton::set_dot(brain& brn){
	BRAIN_CK(qu_dot == cg_neutral);
	BRAIN_CK(qu_inverse->qu_dot == cg_neutral);
	qu_dot = cg_positive;
	qu_inverse->qu_dot = cg_negative;
	brn.br_tot_qu_dots++;
}

inline
void
quanton::reset_dot(brain& brn){
	BRAIN_CK(qu_dot != cg_neutral);
	BRAIN_CK(qu_inverse->qu_dot != cg_neutral);
	qu_dot = cg_neutral;
	qu_inverse->qu_dot = cg_neutral;
	brn.br_tot_qu_dots--;
}

inline
void
quanton::set_mark(brain& brn){
	BRAIN_CK(qu_inverse != NULL_PT);
	BRAIN_CK(qu_mark == cg_neutral);
	BRAIN_CK(qu_inverse->qu_mark == cg_neutral);
	qu_mark = cg_positive;
	qu_inverse->qu_mark = cg_negative;
	brn.br_tot_qu_marks++;
}

inline
void
quanton::reset_mark(brain& brn){
	BRAIN_CK(qu_inverse != NULL_PT);
	BRAIN_CK(qu_mark != cg_neutral);
	BRAIN_CK(qu_inverse->qu_mark != cg_neutral);
	qu_mark = cg_neutral;
	qu_inverse->qu_mark = cg_neutral;
	brn.br_tot_qu_marks--;
}

inline
void
neuron::set_spot(brain& brn){
	BRAIN_CK(! ne_spot);
	ne_spot = true;
	brn.br_tot_ne_spots++;
}

inline
void
neuron::reset_spot(brain& brn){
	BRAIN_CK(ne_spot);
	ne_spot = false;
	brn.br_tot_ne_spots--;
}

inline
void
memap::reset_memap(brain& brn){
	//BRAIN_CK(map_ck_all_qu_dominated(brn));
	init_memap(&brn);
}

inline
void
neuromap::reset_neuromap(brain& brn){
	BRAIN_CK(map_ck_all_qu_dominated());
	init_neuromap(&brn);
}

inline
long 
notekeeper::get_lv_idx(long lv){
	BRAIN_CK(lv >= 0);
	BRAIN_CK(lv < MAX_LAYERS); 

	long lv_idx = lv;
	return lv_idx;
}

inline
row_quanton_t&	
notekeeper::get_layer(long lv){
	BRAIN_CK(lv >= 0); 
	BRAIN_CK(lv < MAX_LAYERS); 
	while(dk_motives_by_layer.size() <= lv){
		dk_motives_by_layer.inc_sz();
	}
	long lv_idx = get_lv_idx(lv);
	BRAIN_CK(dk_motives_by_layer.is_valid_idx(lv_idx));
	row_quanton_t& mots = dk_motives_by_layer[lv_idx];
	return mots;
}

inline
brain&
notekeeper::get_dk_brain(){
	BRAIN_CK(dk_brain != NULL_PT);
	return *dk_brain;	
}

inline
brain&
deducer::get_de_brain(){
	BRAIN_CK(de_brain != NULL_PT);
	return *de_brain;	
}

inline
notekeeper& 
deducer::get_orig_trail(){
	return get_de_brain().br_charge_trail;
}

inline
comparison	cmp_long_id(quanton* const & qua1, quanton* const & qua2){
	BRAIN_CK(qua1 != NULL_PT);
	BRAIN_CK(qua2 != NULL_PT);
	return cmp_long(qua1->qu_id, qua2->qu_id);
}

inline
comparison	cmp_choice_idx_lt(quanton* const & qua1, quanton* const & qua2){
	BRAIN_CK(qua1 != NULL_PT);
	BRAIN_CK(qua2 != NULL_PT);
	return cmp_long(qua1->qu_choice_idx, qua2->qu_choice_idx);
}

inline
comparison	cmp_choice_idx_gt(quanton* const & qua1, quanton* const & qua2){
	BRAIN_CK(qua1 != NULL_PT);
	BRAIN_CK(qua2 != NULL_PT);
	return cmp_long(qua2->qu_choice_idx, qua1->qu_choice_idx);
}

inline
comparison	cmp_qlevel(quanton* const & qua1, quanton* const & qua2){
	BRAIN_CK(qua1 != NULL_PT);
	BRAIN_CK(qua2 != NULL_PT);
	long	qlev1 = qua1->qlevel();
	long	qlev2 = qua2->qlevel();
	bool inv1 = (qlev1 == INVALID_LEVEL);
	bool inv2 = (qlev2 == INVALID_LEVEL);
	// sort them in inverse order with inv as the max:
	if(inv1 && inv2){ return 0; }

	if(inv1 && !inv2){ return -1; }
	if(!inv1 && inv2){ return 1; }
	if(qlev1 == qlev2){
		return cmp_long(qua2->get_charge(), qua1->get_charge());
	}
	return cmp_long(qlev2, qlev1);
}

inline
charge_t negate_trinary(charge_t val){
	if(val == cg_negative){
		return cg_positive;
	}
	if(val == cg_positive){
		return cg_negative;
	}
	return cg_neutral;
}

inline
void	negate_quantons(row_quanton_t& qua_row){
	for(long kk = 0; kk < qua_row.size(); kk++){
		qua_row[kk] = qua_row[kk]->qu_inverse;
	}

}

inline
void	get_ids_of(row_quanton_t& quans, row_long_t& the_ids){
	the_ids.clear();
	for(long kk = 0; kk < quans.size(); kk++){
		quanton& qua = *(quans[kk]);
		the_ids.push(qua.qu_id);

	}
}

inline
void	find_max_level(row_quanton_t& tmp_mots, long& max_lev){
	//max_lev = INVALID_LEVEL;
	max_lev = ROOT_LEVEL;

	for(long aa = 0; aa < tmp_mots.size(); aa++){

		quanton& qua = *(tmp_mots[aa]);
		max_lev = max(max_lev, qua.qlevel());
	}
}

//=============================================================================
// printing funcs

DEFINE_PRINT_FUNCS(ticket)
DEFINE_PRINT_FUNCS(quanton)
DEFINE_PRINT_FUNCS(neuron)
DEFINE_PRINT_FUNCS(deduction)
DEFINE_PRINT_FUNCS(prop_signal)
DEFINE_PRINT_FUNCS(coloring)
DEFINE_PRINT_FUNCS(neuromap)
DEFINE_PRINT_FUNCS(memap)
DEFINE_PRINT_FUNCS(leveldat)

#endif		// BRAIN_H


