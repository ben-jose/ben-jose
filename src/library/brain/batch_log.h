

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

batch_log.h

some aux funcs for batch_log.

--------------------------------------------------------------*/

#ifndef BATCH_LOG_H
#define BATCH_LOG_H

#include "ch_string.h"
#include "ben_jose.h"


//=================================================================
// defs

#define LOG_NM_ERROR	"error.log"
#define LOG_NM_RESULTS	"results.log"
#define LOG_NM_STATS	"stats.log"
#define LOG_NM_ASSIGS	"assigs.log"

#define RESULT_FIELD_SEP		"|"
#define RESULT_FIELD_SEP_CHAR		'|'

//=================================================================
// aux funcs

inline
ch_string
get_log_name(ch_string f_nam, ch_string sufix){
	ch_string lg_nm = f_nam + "_" + sufix;
	return lg_nm;
}

#define RES_UNKNOWN_STR "unknown"
#define RES_YES_SATISF_STR "yes_satisf"
#define RES_NO_SATISF_STR "no_satisf"
#define RES_ERROR_STR "error"

inline
bj_satisf_val_t
as_satisf(ch_string str_ln){
	bj_satisf_val_t the_val = bjr_unknown_satisf;
	if(str_ln == RES_UNKNOWN_STR){
		the_val = bjr_unknown_satisf;
	} else if(str_ln == RES_YES_SATISF_STR){
		the_val = bjr_yes_satisf;
	} else if(str_ln == RES_NO_SATISF_STR){
		the_val = bjr_no_satisf;
	} else if(str_ln == RES_ERROR_STR){
		the_val = bjr_error;
	}
	return the_val;
}

inline
ch_string
as_satisf_str(bj_satisf_val_t vv){
	ch_string sf_str = RES_UNKNOWN_STR;
	switch(vv){
		case bjr_unknown_satisf:
			sf_str = RES_UNKNOWN_STR;
			break;
		case bjr_yes_satisf:
			sf_str = RES_YES_SATISF_STR;
			break;
		case bjr_no_satisf:
			sf_str = RES_NO_SATISF_STR;
			break;
		case bjr_error:
			sf_str = RES_ERROR_STR;
			break;
	}
	return sf_str;
}


#endif		// BATCH_LOG_H


