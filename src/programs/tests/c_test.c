
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

c_test.c

file for test and debugging purposes.

--------------------------------------------------------------*/


#include <stdio.h>

#include "ben_jose.h"

#define MARK_USED(X)  ((void)(&(X)))

int main(int argc, char** argv)
{
	if(argc < 2){
		printf("args: <cnf_file_path>\n");
		return 1;
	}
	char* ff = argv[1];
	
	bj_solver_t ss = bj_solver_create("");
	
	bj_satisf_val_t  vv = bj_solve_file(ss, ff);
	switch(vv){
		case bjr_yes_satisf:
			printf("%s is SAT instance\n", ff);
			break;
		case bjr_no_satisf:
			printf("%s is UNS instance\n", ff);
			break;
		case bjr_error:
			printf("ERROR ! in %s\n", ff);
			break;
		default:
			printf("FATAL ERROR ! in %s\n", ff);
			break;
	}
	//bj_output_t 		bj_get_output(bj_solver_t bjs);
	//const long* 		bj_get_assig(bj_solver_t bjs);
	
	bj_solver_release(ss);
	return 0;
}

