

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

str_set.h

string set wrapper.

--------------------------------------------------------------*/

#ifndef STR_SET_H
#define STR_SET_H

#include <set>
#include <map>

//=================================================================
// strset

typedef std::set<ch_string> 	string_set_t;
//typedef mem_redblack<ch_string>						string_set_t;
typedef std::map<ch_string, long> 	string_long_map_t;
typedef std::map<ch_string, ch_string> 	str_str_map_t;

inline
void strset_clear(string_set_t& ss){
	//ss.clear_redblack();
	ss.clear();
}

inline
bool strset_is_empty(string_set_t& ss){
	//bool ee = ss.is_empty();
	bool ee = ss.empty();
	return ee;
}

inline
long strset_size(string_set_t& ss){
	//long nn = ss.size();
	long nn = ss.size();
	return nn;
}

inline
bool strset_find_path(string_set_t& ss, ch_string pth){
	//bool ff = ss.search(pth);
	bool ff = (ss.find(pth) != ss.end());
	return ff;
}

inline
void strset_add_path(string_set_t& ss, ch_string pth){
	if(pth.empty()){ return; }
	if(pth == ""){ return; }
	//ss.push(pth);
	ss.insert(pth);
}

inline
void strset_print(bj_ostream& os, string_set_t& pmp){
	for(string_set_t::iterator it = pmp.begin(); it != pmp.end(); ++it){
		os << *it << '\n';
	}
}


#endif		// STR_SET_H


