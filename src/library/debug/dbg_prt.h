

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

dbg_print.h

funcs to print debug info.

--------------------------------------------------------------*/

#ifndef DBG_PRINT_H
#define DBG_PRINT_H

#include "tools.h"

class brain;

void	dbg_prt_all_cho(brain& brn);

bool	dbg_check_lev(solver* slv, long lev);
bj_ostream&	dbg_get_out_stm(solver* slv);


//=================================================================
// release dbg defs

//define REL_PRT(dummy, comms1) {bj_eol << "change_all_REL_PRT_for_DBG_PRT_and_recomplie !";}

#define REL_PRT(dummy, comms1) /**/

/*define REL_PRT(pt_brn, comms1) \
	{ \
		bj_ostream& os = bj_dbg; \
		if(pt_brn != NULL_PT){ pt_brn->dbg_prt_margin(os); } \
		comms1; \
		os << bj_eol; \
		os.flush(); \
	} \
	
//--end_of_def
*/


//=================================================================
// debug defs

#define DBG_ALL_LVS -1
#define INVALID_DBG_LV 		-123

#define	DBG_BR_COND(pt_slv, lev, cond)	(dbg_check_lev(pt_slv, lev) && (cond))

#define	DBG_COND(lev, cond)	DBG_BR_COND(get_dbg_slv(), lev, cond)

#define	DBG_COMMAND(lev, comm) \
	DBG( \
		if(DBG_COND(lev, true)){ \
			comm; \
		} \
	) \

//--end_of_def

#define	DBG_COMM_WITH(lev, obj, comm) \
	DBG( \
		if(DBG_BR_COND((obj).get_dbg_slv(), lev, true)){ \
			comm; \
		} \
	) \

//--end_of_def

void	dbg_print_left_margin(solver* pt_slv, bj_ostream& os, long dbg_lv);

//			bool is_htm_1 = (&bj_dbg != &os);


#define	DBG_PRT_SLV(pt_slv, o_stm, lev, cond, comm) \
	DBG( \
		if(DBG_BR_COND(pt_slv, lev, cond)){ \
			bj_ostream& os = dbg_get_out_stm(pt_slv); \
			bool is_htm_1 = false; \
			if(is_htm_1){ os << bj_eol << "<pre>"; } \
			dbg_print_left_margin(pt_slv, os, lev); \
			comm; \
			if(is_htm_1){ os << "</pre>" << bj_eol; } \
			os << bj_eol; \
			os.flush(); \
		} \
	) \
	
//--end_of_def

#define	DBG_PRT_COND(lev, cond, comm) \
	DBG_PRT_SLV(get_dbg_slv(), bj_dbg, lev, cond, comm)

#define	DBG_PRT_COND_WITH(lev, obj, cond, comm) \
	DBG_PRT_SLV((obj).get_dbg_slv(), bj_dbg, lev, cond, comm)

#define	DBG_PRT_WITH(lev, obj, comm) \
	DBG_PRT_SLV((obj).get_dbg_slv(), bj_dbg, lev, true, comm)

#define	DBG_PRT(lev, comm)	DBG_PRT_COND(lev, true, comm)

#define DBG_BJ_LIB_CK(prm) DBG_CK(prm) 

#define DBG_SLOW(prm)
//define DBG_SLOW(prm)	DBG(prm)

#define DBG_PRT_ABORT(brn) \
		os << "\n________________\n ABORT_DATA\n"; \
		brn.dbg_prt_margin(os); \
	
//--end_of_def

#ifdef FULL_DEBUG
	ch_string get_cy_dir(brain& brn);
#endif	

void dbg_prt_open(ch_string& path, bj_ofstream& stm, bool append = false);


#endif		// DBG_PRINT_H


