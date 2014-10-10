

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

bit_row.cpp

binary rows of bits funcs

--------------------------------------------------------------*/


#include "bit_row.h"

bj_ostream&	
bit_row::print_bit_row(
	bj_ostream& os, 
	bool with_lims,

	char* sep, 
	bit_row_index low, 
	bit_row_index hi,
	bit_row_index grp_sz,
	char* grp_sep
	)
{
	bit_row_index num_elem = 1;
	if(with_lims){ os << "["; }
	for(bit_row_index ii = 0; ii < sz; ii++){
		if(ii == low){ os << ">"; }
		os << pos(ii);
		if(ii == hi){ os << "<"; }
		os << sep;
		if((grp_sz > 1) && ((num_elem % grp_sz) == 0)){
			os << grp_sep;
		}
		num_elem++;
	}
	if(with_lims){ os << "] "; }
	os.flush();
	return os;
}

