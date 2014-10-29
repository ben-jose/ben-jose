
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

