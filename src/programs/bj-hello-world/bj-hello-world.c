
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

Copyright (C) 2007-2012, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

hello_ben_jose.c

hello world for this library.

--------------------------------------------------------------*/


#include <stdio.h>
#include "ben_jose.h"

int main(int argc, char** argv)
{
	if(argc < 2){
		printf("args: <cnf_file_path> [<db_dir>]\n");
		return 1;
	}
	char* dd = ".";
	char* ff = argv[1];
	if(argc > 2){
		dd = argv[2];
	}
	
	bj_solver_t ss = bj_solver_create(dd);
	
	bj_satisf_val_t  vv = bj_solve_file(ss, ff);
	switch(vv){
		case bjr_yes_satisf:
			printf("%s is SAT instance\n", ff);
			break;
		case bjr_no_satisf:
			printf("%s is UNS instance\n", ff);
			break;
		case bjr_error:
			{
				printf("ERROR ! in %s\n", ff);
				bj_output_t oo = bj_get_output(ss);
				const char* e_str = bj_error_str(oo.bjo_error);
				printf("%s\n", e_str);
			}
			break;
		default:
			printf("FATAL ERROR ! in %s\n", ff);
			break;
	}
	
	// more info with this functions
	//bj_output_t oo = bj_get_output(ss);
	//const long* aa = bj_get_assig(ss);
	
	bj_solver_release(ss);
	return 0;
}

