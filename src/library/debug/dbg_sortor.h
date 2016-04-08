

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

Copyright (C) 2011, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

dbg_sortor.h

Declaration of functions to read and parse config files.

--------------------------------------------------------------*/

#ifndef TEST_SORTOR_H
#define TEST_SORTOR_H

// test sortor

#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <cstring>
#include <set>
#include <map>
#include <iterator>

#include <iostream>
#include <gmpxx.h>

#include "math.h"

//     g++ t1.cpp -lgmpxx -lgmp -o tt.exe

#include "unistd.h"
#include "limits.h"

#include "util_funcs.h"
#include "sortor.h"
#include "tak_mak.h"


class elem_sor;
DECLARE_PRINT_FUNCS(elem_sor);

class elem_sor {
public:
	static
	std::string	CL_NAME;

	virtual
	std::string&	get_cls_name(){
		return elem_sor::CL_NAME;
	}

	std::string		es_id;
	sortee			es_srt_bdr;
	row_sort_id_t	es_ids;

	elem_sor(std::string id = "??"){
		init_elem_sor(id);
	}

	~elem_sor(){
	}

	void	init_elem_sor(std::string id = "??"){
		es_id = id;
		es_srt_bdr.so_me = this;
	}

	void	add(long id){
		es_ids.push(id);
	}

	std::ostream&	print_elem_sor(std::ostream& os, bool from_pt = false){
		MARK_USED(from_pt);
		for(long aa = 0; aa < es_ids.size(); aa++){
			sort_id_t id = es_ids[aa];
			os << id;
			os << ".";
		}
		os << "  " << es_id;
		os << es_srt_bdr;
		os.flush();
		return os;
	}
};

class op_sor {
public:
	elem_sor*	op_elem;
	long		op_id;

	op_sor(){
		init_op_sor();
	}

	~op_sor(){
	}

	void	init_op_sor(){
		op_elem = NULL_PT;
		op_id = 0;
	}
};

DEFINE_PRINT_FUNCS(elem_sor);

#endif		// TEST_SORTOR_H

