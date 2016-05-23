

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

-------------------------------------------------------------- */
/*! 
\file ben_jose.h
\brief ben_jose API declaration.

DO NOT free any pointer returned by these functions. They are pointers to the internal structure of a solver. A solver is an opaque structure to the user represented by objects of type obj_solver_t. 

DO NOT release the memory of any returned pointer.
--

*/ 
//--------------------------------------------------------------

#ifndef BEN_JOSE_H
#define BEN_JOSE_H

#ifdef __cplusplus
extern "C" {
#endif
	
//============================================================
/*! \defgroup docgrp_API User's API

The basic interaction is best described by the bj-hello-world.c program

\snippet bj-hello-world.c hello_id

*/


//! String to print a \ref bjr_unknown_satisf result
#define RES_UNKNOWN_STR "unknown"
#define RES_YES_SATISF_STR "yes_satisf"
#define RES_NO_SATISF_STR "no_satisf"
#define RES_ERROR_STR "error"

#define RESULT_FIELD_SEP		"|"
#define RESULT_FIELD_SEP_CHAR		'|'

//=============================================================================
/*! \enum bj_satisf_val_t
\brief Posible final results of the solving process.
\ingroup docgrp_API

*/


typedef enum {
	bjr_unknown_satisf,		//!< Unknown result
	bjr_yes_satisf,			//!< Satisfiable CNF
	bjr_no_satisf,			//!< Unsatisfiable CNF
	bjr_error				//!< Error result
} bj_satisf_val_t;

//=============================================================================
/*! \enum bj_error_t
\brief Posible errors of the solving process.
\ingroup docgrp_API

*/
typedef enum {
	bje_no_error,	//!< No error
	bje_internal,	//!< Internal 
	bje_internal_ex,	//!< Internal with explanation 
	bje_memout,	//!< Out of memory 
	bje_timeout,	//!< Timeout 
	bje_instance_cannot_load,	//!< Can not load instance
	bje_file_cannot_open,	//!< Can not open file
	bje_file_corrupted,	//!<  Corrupted file
	bje_file_too_big,	//!< Too big file (must fit in memory)
	bje_path_too_long,	//!< Path too long
	bje_invalid_root_directory,	//!< Invalid root (database) directory
	bje_parse_bad_number,	//!< Found a non number where a number is expected
	bje_dimacs_no_cnf_declaration,	//!< Bad DIMACS format. Non existant cnf declaration line. 
	bje_dimacs_bad_cls_num,	//!< Bad DIMACS format. Inconsistent clause count with cnf declaration. 
	bje_dimacs_format_err,	//!< Bad DIMACS format. General format error. 
	bje_dimacs_zero_vars,	//!< Bad DIMACS format. No variables found.
	bje_dimacs_zero_clauses,	//!< Bad DIMACS format. No clauses found.
	bje_dimacs_bad_literal,	//!< Bad DIMACS format. Invalid clause literal.
	bje_dimacs_clause_too_long	//!< Bad DIMACS format. Clause too long.
} bj_error_t;

//=============================================================================
/*! \enum bj_param_t
\brief Posible parameters for the solving process.
\ingroup docgrp_API

*/
typedef enum {
	bjp_invalid,	//!< Invalid parameter
	bjp_as_release,	//!< Run debug version of library as release version. Assumes linking with the debug version.
	bjp_only_deduc,	//!< Run only deduction (do not use training). For debugging purposes only.
	bjp_write_proofs,	//!< Write a JSON proof if the CNF is unsatisfiable.
	bjp_test_result	//!< Compare results to an existing .test file or write one if not found.
} bj_param_t;

//=============================================================================
/*! \enum bj_output_t
\brief All output information of the solving process.
\ingroup docgrp_API

*/
typedef struct {
	bj_satisf_val_t 	bjo_result;	//!< Final result of solving process.
	
	double		bjo_solve_time;	//!< Final solving time in seconds.
	long		bjo_num_vars;	//!< Number of CNF variables.
	long		bjo_num_ccls;	//!< Number of CNF clauses.
	long		bjo_num_lits;	//!< Number of CNF literals.
	
	double		bjo_num_laps;	//!< Final number of laps (backtracs) during solving.
	double		bjo_num_recoils;	//!< Final number of recoils (backtracs+retracts) during solving.
	
	double		bjo_load_time;	//!< Final CNF loading into memory time in seconds.
	double		bjo_num_cnf_saved;	//!< Final number of CNF writed during solving.
	double		bjo_num_cnf_finds;	//!< Final number of CNF found during solving.
	double		bjo_quick_discards;	//!< Final number of CNF quickly discarded during solving.
	
	bj_error_t  bjo_error;	//!< Final error returned after solving.
	char		bjo_err_char;	//!< For a DIMACS error. The character at wich the error was found.
	long 		bjo_err_line;	//!< For a DIMACS error. The line at which the error was found.
	long 		bjo_err_pos;	//!< For a DIMACS error. The column at which the error was found.
	long 		bjo_err_num_decl_cls;	//!< For a DIMACS error. The number of declared clauses.
	long 		bjo_err_num_decl_vars;	//!< For a DIMACS error. The number of declared variables.
	long 		bjo_err_num_read_cls;	//!< For a DIMACS error. The number of read clauses.
	long 		bjo_err_bad_lit;	//!< For a DIMACS error. The error literal.
	
	char		bjo_dbg_enabled;	//!< True if program is linked to debug version of the library
	char		bjo_dbg_never_write;	//!< True if debug linked program is configured to never write CNFs
	char		bjo_dbg_never_find;	//!< True if debug linked program is configured to never find CNFs
	char		bjo_dbg_min_trainable;	//!< True if debug linked program is configured to find a CNF with minimum size
	char		bjo_dbg_as_release;	//!< True if debug linked program is configured work as release version
} bj_output_t;

//! A ben-jose solver object is of this tyoe
typedef void*	bj_solver_t;

//! Checks if a ben-jose solver object is NULL
#define bj_solver_is_null(bjs) (bjs == NULL)

// TO USER: DO NOT FREE OUTPUT POINTERS when calling these functions



/*! Init a bj_output_t structure
\param the_out A pointer to the structure to be initialized
\callgraph
*/
void 		bj_init_output(bj_output_t* the_out);



/*! Create a solver object to solve one or more CNFs
\param bjs_dir_path A C string that represents the path of the database of CNFs.
\return A bj_solver_t object representing a solver.
*/
bj_solver_t bj_solver_create(const char* bjs_dir_path);



/*! Release a solver object created with bj_solver_create
\param bjs The solver object to be released.
*/
void 		bj_solver_release(bj_solver_t bjs);



/*! Set an input parameter in the solver
\param bjs The solver object to be used.
\param prm The parameter.
\param val The value. For boolen values it is 0 (false) or not 0 (true)
*/
void		bj_set_param_char(bj_solver_t bjs, bj_param_t prm, char val);



/*! Get the value of a previusly set (with bj_set_param_char) input parameter.
\param bjs The solver object to be used.
\param prm The parameter.
\return The value. For boolen values it is 0 (false) or 1 (true)
*/
char		bj_get_param_char(bj_solver_t bjs, bj_param_t prm);
//void		bj_set_param_str(bj_solver_t bjs, bj_param_t prm, char* val);



/*! Get the value of a previusly set database path (with bj_solver_create).
\param bjs The solver object to be used.
\return The database directory path. Do not free the returned pointer.
*/
const char* bj_get_database_path(bj_solver_t bjs);



/*! Solve a CNF given the DIMACS formated file path of the CNF.
\param bjs The solver object to be used.
\param f_path The CNF file path. It must be in DIMACS format.
\return The solving result.
\callgraph
\callergraph
*/
bj_satisf_val_t 	bj_solve_file(bj_solver_t bjs, const char* f_path);



/*! Solve a CNF given a DIMACS formated C ASCII characters array.
\param bjs The solver object to be used.
\param dat_sz The number of ASCII characters in dat.
\param dat The DIMACS formated C ASCII characters array. It can be the exact contents read of a valid DIMACS file.
\return The solving result.
\callgraph
\callergraph
*/
bj_satisf_val_t 	bj_solve_data(bj_solver_t bjs, long dat_sz, const char* dat);



/*! Solve a CNF given the DIMACS values of a CNF.
\param bjs The solver object to be used.
\param num_vars The number of variables in used in lits. It is the value as read in a cnf declaration of a DIMACS file.
\param num_cls The number of clauses found in lits. It is the value as read in a cnf declaration of a DIMACS file.
\param lits_sz The size of the lits parameter.
\param lits THe array containing a set of clauses separated by zeros. Ej: [1 2 0 3 4 5 0] representes two clauses: [1 2] and [3 4 5].
\return The solving result.
\callgraph
\callergraph
*/
bj_satisf_val_t 	bj_solve_literals(bj_solver_t bjs, long num_vars, long num_cls, 
						  long lits_sz, long* lits);



/*! Get the result of a solving process. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The solving result. It has the same value returned by the previusly called bj_solve_ function.
*/
bj_satisf_val_t 	bj_get_result(bj_solver_t bjs);



/*! Get all the output a solving process. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The solving output.
\see bj_output_t
*/
bj_output_t 		bj_get_output(bj_solver_t bjs);



/*! Returns the value passed to bj_solve_file. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The value passed to bj_solve_file or null otherwise (other bj_solve_ function was called).
*/
const char* 		bj_get_solve_file_path(bj_solver_t bjs);



/*! Get the assignation to variables that satisfies the CNF. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return An array (here called 'assig') with the assignation to variables that satisfies the CNF. 

'assig' always ends with a 0. 

No memory after that 0 should be accessed. 

Let vv = assig[ii]. Then:

- If vv is positive it means the the variable represented by abs(vv) is assigned true 
- If vv is negative it means the the variable represented by abs(vv) is assigned false

Example:
[-20 4 -12 6 9 0]

Means:
- set var 20 to false
- set var 4 to true
- set var 12 to false
- set var 6 to true
- set var 9 to true

And that assignation satisfies the whole CNF (it might have more than 20 variables).
*/
const long* 		bj_get_assig(bj_solver_t bjs);



/*! Returns the last written proof path. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The path to the last written GSON proof file. 

NULL is returned if the parameter bjp_write_proofs was not used before calling a bj_solve_ function.
*/
const char* 		bj_get_last_proof_path(bj_solver_t bjs);



/*! Get the stack when the error ocurred. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The stack when the error ocurred. 

Funtion for debugging purposes. Only returns a valid value if an error has ocurred during solving.
*/
const char* 		bj_get_error_stack_str(bj_solver_t bjs);



/*! Get the assert that failed. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return The assert that failed. 

Funtion for debugging purposes. Only returns a valid value if an assert has failed.
*/
const char* 		bj_get_error_assert_str(bj_solver_t bjs);



/*! Get a string with the titles of a result string fields. 
\param bjs The solver object to be used.
\return A string with the titles of a result string fields. 

Funtion for testing purposes.
*/
const char*			bj_get_result_titles_string(bj_solver_t bjs);



/*! Get a string with the result string. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return A string with the result string. 

Funtion for testing purposes. It contains a concatenation of the string representation of some fields obtained otherwise with the bj_get_output funtion.
*/
const char*			bj_get_result_string(bj_solver_t bjs);



/*! Sets internal values in the solver with a result string previusly returned by bj_get_result_string.
\param bjs The solver object to be used.
\param rslt_str A string with the result string. 

Funtion for testing purposes. It sets internal values in the solver so that a call to bj_get_output will return the parsed values.
*/
void				bj_parse_result_string(bj_solver_t bjs, const char* rslt_str);



/*! Resets all internal values in the solver.
\param bjs The solver object to be used.

*/
void			bj_restart(bj_solver_t bjs);



/*! Prints all paths used by the solver.
\param bjs The solver object to be used.

*/
void		bj_print_paths(bj_solver_t bjs);



/*! Gets a string representing the error. A bj_solve_ function must have been previusly called.
\param bjs The solver object to be used.
\return A string representing the error. 

*/
const char* bj_error_str(bj_error_t bje);

// TO USER: DO NOT FREE OUTPUT POINTERS when calling these functions

#ifdef __cplusplus
}
#endif

#endif		// BEN_JOSE_H


