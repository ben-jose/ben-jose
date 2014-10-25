

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

ben_jose.h

ben_jose interfase.

--------------------------------------------------------------*/

#ifndef BEN_JOSE_H
#define BEN_JOSE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	k_unknown_satisf,
	k_yes_satisf,
	k_no_satisf,
	k_timeout,
	k_memout,
	k_error
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
	char	bji_with_assig;
	
	long	bji_group_id;
	long	bji_id;
	
	char*	bji_file_path;

	long	bji_data_sz;
	char* 	bji_data;

	long	bji_num_vars;
	long	bji_num_ccls;

	long	bji_literals_sz;
	long* 	bji_literals;
	
	double	bji_timeout;
	double	bji_memout;
} bj_input_t;

typedef struct {
	bj_satisf_val_t 	bjo_result;
	
	double		bjo_solve_time;
	long		bjo_num_vars;
	long		bjo_num_ccls;
	long		bjo_num_lits;
	
	double		bjo_num_laps;
	
	double		bjo_load_tm;
	double		bjo_saved_targets;
	double		bjo_old_hits;
	double		bjo_old_sub_hits;
	double		bjo_new_hits;
	double		bjo_new_sub_hits;
	
	long		bjo_final_assig_sz;
	long*		bjo_final_assig;

	bj_error_t  bjo_error;
	char		bjo_err_char;
	long 		bjo_err_line;
	long 		bjo_err_pos;
	long 		bjo_err_num_decl_cls;
	long 		bjo_err_num_decl_vars;
	long 		bjo_err_num_read_cls;
	long 		bjo_err_bad_lit;
} bj_output_t;

typedef void*	bj_uns_db_t;

void	bj_init_input(bj_input_t* in);
void	bj_release_output_assig(bj_output_t* out);

bj_uns_db_t bj_unsat_db_open(const char* bdb_dir_path);
void 		bj_unsat_db_close(bj_uns_db_t bdb);
const char* bj_unsat_db_path(bj_uns_db_t bdb);
int 		bj_unsat_db_update(bj_uns_db_t dest, bj_uns_db_t src);

int 	bj_solve_file(bj_uns_db_t bdb, const char* f_path, bj_output_t* out);
int 	bj_solve_data(bj_uns_db_t bdb, long dat_sz, char* dat, bj_output_t* out);
int 	bj_solve_literals(bj_uns_db_t bdb, long num_vars, long num_cls, 
						  long lits_sz, long* lits, bj_output_t* out);
int 	bj_solve_input(bj_uns_db_t bdb, bj_input_t* in, bj_output_t* out);


#ifdef __cplusplus
}
#endif

#endif		// BEN_JOSE_H


