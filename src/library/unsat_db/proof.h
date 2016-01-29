

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

proof.h

proof writing function declarations.

--------------------------------------------------------------*/

#ifndef PROOF_FUNCS_H
#define PROOF_FUNCS_H

#include "brain.h"

void proof_write_all_json_files_for(deduction& deduc);
void proof_write_json_file_for(deduction& deduc);
void proof_append_ps(row<char>& json_str, prop_signal& the_sig);
void proof_append_neu(row<char>& json_str, neuron* neu);
void proof_append_qua(row<char>& json_str, quanton* qua);

ch_string 	proof_get_file_path_for(brain& brn, ticket& pf_tk);

#endif		// PROOF_FUNCS_H


