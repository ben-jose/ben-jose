

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

proof.cpp  

proof writing funcs.

--------------------------------------------------------------*/

#include "proof.h"

ch_string 
proof_get_file_path_for(brain& brn, ticket& pf_tk){
	ch_string pf_f_nm = "proof_" + pf_tk.get_str() + ".jsn";
	skeleton_glb& skg = brn.get_skeleton();
	ch_string pf_nm = skg.kg_tmp_proof_path + "/" + pf_f_nm;
	return pf_nm;
	//skg.as_full_path(skg.kg_collisions_path);
}

void
proof_write_json_file_for(deduction& deduc)
{
	brain& brn = deduc.get_brn();
	row<prop_signal>& trace = deduc.dt_all_noted;
	reason& rsn = deduc.dt_rsn;
	
	row<char> json_str;	
	ch_string proof_f_nam = proof_get_file_path_for(brn, rsn.rs_tk);
	
	canon_string_append(json_str, "{\n");
	canon_string_append(json_str, "\t\"name\": \"" + proof_f_nam + "\",\n");
	canon_string_append(json_str, "\t\"chain\": [\n");
	
	for(long aa = 0; aa < trace.size(); aa++){
		prop_signal& nxt_sig = trace[aa];
		if(aa > 0){
			canon_string_append(json_str, "\t\t,");
		}
		proof_append_ps(json_str, nxt_sig);
	}

	canon_string_append(json_str, "\t]\n");
	canon_string_append(json_str, "}\n");
	
	skeleton_glb& skg = brn.get_skeleton();
	ch_string full_pth = skg.as_full_path(proof_f_nam);

	write_file(full_pth, json_str, true);
}

void
proof_append_ps(row<char>& json_str, prop_signal& the_sig){
	canon_string_append(json_str, "\t\t{\t");
	proof_append_neu(json_str, the_sig.ps_source);
	canon_string_append(json_str, "\"va_r\": ");
	proof_append_qua(json_str, the_sig.ps_quanton);
	canon_string_append(json_str, "\t\t}\n");
}

void 
proof_append_neu(row<char>& json_str, neuron* neu){
	canon_string_append(json_str, "\"neu_lits\": ");
	if(neu == NULL_PT){
		canon_string_append(json_str, "[],\n");
		canon_string_append(json_str, "\"neu_idx\": \"0\",");
		return;
	}
	canon_string_append(json_str, "[");
	for(long aa = 0; aa < neu->fib_sz(); aa++){
		BRAIN_CK(neu->ne_fibres[aa] != NULL_PT);
		quanton& qua = *(neu->ne_fibres[aa]);
		if(aa > 0){
			canon_string_append(json_str, ",");
		}
		canon_string_append(json_str, " \"");
		canon_long_append(json_str, qua.qu_id);
		canon_string_append(json_str, "\"");
	}
	canon_string_append(json_str, "],\n");
	
	canon_string_append(json_str, "\"neu_idx\": ");
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, neu->ne_index);
	canon_string_append(json_str, "\",\n");
}

void 
proof_append_qua(row<char>& json_str, quanton* qua){
	if(qua == NULL_PT){
		canon_string_append(json_str, "\"0\"\n");
		return;
	}
	canon_string_append(json_str, "\"");
	canon_long_append(json_str, qua->qu_id);
	canon_string_append(json_str, "\"\n");
}

void
proof_write_all_json_files_for(deduction& deduc){
}

