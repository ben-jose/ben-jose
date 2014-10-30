

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

run_satex.h

func to run satex.

--------------------------------------------------------------*/

#ifndef RUN_SATEX_H
#define RUN_SATEX_H

#define RSATX_CK(prm) DBG_CK(prm)

bool	dbg_run_satex_is_no_sat(ch_string f_nam);
void	system_exec(ch_string& strstm);

#endif		// RUN_SATEX_H


