

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


//=================================================================
// debug defs

#define DBG_ALL_LVS -1

#define	DBG_BR_COND(pt_slv, lev, cond)	(dbg_check_lev(pt_slv, lev) && (cond))

#define	DBG_COND(lev, cond)	DBG_BR_COND(get_dbg_slv(), lev, cond)

#define INVALID_DBG_LV 		-123

bool	dbg_print_cond_func(
	solver* pt_slv,
	bool prm,
	bool is_ck = false,
	const ch_string fnam = "NO_NAME",
	int lnum = 0,
	const ch_string prm_str = "NO_PRM",
	long dbg_lv = INVALID_DBG_LV);

#define	DBG_BR_PRT_COND(pt_brn, lev, cond, comm) \
	DBG_COND_COMM(DBG_BR_COND(pt_brn, lev, cond), \
		dbg_print_cond_func(pt_brn, DBG_BR_COND(pt_brn, lev, cond), \
			false, "NO_NAME", 0, #cond, lev); \
		comm; \
	) \

//--end_of_def
	
#define	DBG_PRT_COND(lev, cond, comm)	DBG_BR_PRT_COND(get_dbg_slv(), lev, cond, comm)

#define	DBG_PRT_COND_WITH(lev, obj, cond, comm)	\
	DBG_BR_PRT_COND((obj).get_dbg_slv(), lev, cond, comm)

#define	DBG_PRT_WITH(lev, obj, comm)	DBG_BR_PRT_COND((obj).get_dbg_slv(), lev, true, comm)

#define	DBG_PRT(lev, comm)	DBG_PRT_COND(lev, true, comm)

#define	DBG_COMMAND(lev, comm) \
		if(DBG_COND(lev, true)){ \
			bj_ostream& os = bj_dbg; \
			DBG(comm); \
			os.flush(); \
		} \

//--end_of_def

#define DBG_BJ_LIB_CK(prm) DBG_CK(prm) 

#define DBG_SLOW(prm)
//define DBG_SLOW(prm)	DBG(prm)

#ifdef FULL_DEBUG
	ch_string get_cy_dir(brain& brn);
#endif	

void dbg_prt_open(ch_string& path, bj_ofstream& stm, bool append = false);


#endif		// DBG_PRINT_H


