

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

ben_jose.h

ben_jose interfase.

--------------------------------------------------------------*/

#ifndef BEN_JOSE_H
#define BEN_JOSE_H

#ifdef __cplusplus
extern "C" {
#endif

#define RES_UNKNOWN_STR "unknown"
#define RES_YES_SATISF_STR "yes_satisf"
#define RES_NO_SATISF_STR "no_satisf"
#define RES_ERROR_STR "error"

#define RESULT_FIELD_SEP		"|"
#define RESULT_FIELD_SEP_CHAR		'|'

typedef enum {
	bjr_unknown_satisf,
	bjr_yes_satisf,
	bjr_no_satisf,
	bjr_error
} bj_satisf_val_t;

typedef enum {
	bje_no_error,
	bje_internal,
	bje_internal_ex,
	bje_memout,
	bje_timeout,
	bje_instance_cannot_load,
	bje_file_cannot_open,
	bje_file_corrupted,
	bje_file_too_big,
	bje_invalid_root_directory,
	bje_parse_bad_number,
	bje_dimacs_no_cnf_declaration,
	bje_dimacs_bad_cls_num,
	bje_dimacs_format_err,
	bje_dimacs_zero_vars,
	bje_dimacs_zero_clauses,
	bje_dimacs_bad_literal,
	bje_dimacs_clause_too_long
} bj_error_t;

typedef struct {
	bj_satisf_val_t 	bjo_result;
	
	double		bjo_solve_time;
	long		bjo_num_vars;
	long		bjo_num_ccls;
	long		bjo_num_lits;
	
	double		bjo_num_laps;
	
	double		bjo_load_time;
	double		bjo_saved_targets;
	double		bjo_max_variants;
	double		bjo_avg_variants;
	double		bjo_num_finds;
	double		bjo_quick_discards;
	double		bjo_old_pth_hits;
	double		bjo_new_pth_hits;
	double		bjo_sub_cnf_hits;
	double		bjo_eq_new_hits;
	double		bjo_eq_old_hits;
	double		bjo_sb_new_hits;
	double		bjo_sb_old_hits;
	
	bj_error_t  bjo_error;
	char		bjo_err_char;
	long 		bjo_err_line;
	long 		bjo_err_pos;
	long 		bjo_err_num_decl_cls;
	long 		bjo_err_num_decl_vars;
	long 		bjo_err_num_read_cls;
	long 		bjo_err_bad_lit;
} bj_output_t;

typedef void*	bj_solver_t;

#define bj_solver_is_null(bjs) (bjs == NULL)

void 		bj_init_output(bj_output_t* the_out);

bj_solver_t bj_solver_create(const char* bjs_dir_path);
void 		bj_solver_release(bj_solver_t bjs);

const char* bj_get_path(bj_solver_t bjs);

bj_satisf_val_t 	bj_solve_file(bj_solver_t bjs, const char* f_path);
bj_satisf_val_t 	bj_solve_data(bj_solver_t bjs, long dat_sz, char* dat);
bj_satisf_val_t 	bj_solve_literals(bj_solver_t bjs, long num_vars, long num_cls, 
						  long lits_sz, long* lits);

bj_satisf_val_t 	bj_get_result(bj_solver_t bjs);
bj_output_t 		bj_get_output(bj_solver_t bjs);
const long* 		bj_get_assig(bj_solver_t bjs);

void			bj_restart(bj_solver_t bjs);

int 		bj_update(bj_solver_t dest, bj_solver_t src);

void		bj_print_paths(bj_solver_t bjs);

#ifdef __cplusplus
}
#endif

#endif		// BEN_JOSE_H


