

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

file_funcs.h

file funcs.

--------------------------------------------------------------*/

#ifndef FILE_FUNCS_H
#define FILE_FUNCS_H

#include "top_exception.h"
#include "tools.h"
//include <limits.h>

#define FILE_FN_CK(prm)		DBG_CK(prm)
#define FILE_FN_CK_2(prm, comm)		DBG_CK_2(prm, comm)

#define BJ_FILE_MAX_PATH_DEPTH 40
#define BJ_PATH_MAX 500  // should be PATH_MAX as in limits.h

#define BJ_LOCK_NAME	"lock.skl"
#define BJ_SECS_DEAD_LOCK	10000

#define END_OF_SEC	0

typedef enum {
	flx_cannot_open,
	flx_cannot_calc_size,
	flx_cannot_fit_in_mem
} fl_ex_cod_t;

class file_exception : public top_exception {
public:
	ch_string f_nm;
	file_exception(long the_id = 0, ch_string ff = "unknow_file") : top_exception(the_id)
	{
		f_nm = ff;
	}
};

void
read_file(ch_string f_nam, row<char>& f_data);

bool
write_file(ch_string& the_pth, row<char>& cnn, bool write_once);

// fnum = concurrent safe consec counter kept in file f_nam

bj_big_int_t
inc_fnum(ch_string f_nam);

ch_string
get_fstr(ch_string f_nam);

long
set_fstr(ch_string f_nam, ch_string the_val_str);

bj_big_float_t
update_elapsed(ch_string f_nam);
	
long
read_elapsed(ch_string f_nam, average& the_avg);

ch_string
get_nftw_flag_str(long ff);

ch_string
get_errno_str(long val_errno);

int
delete_dir_entry(const char *fpath, const struct stat *sb,
			int tflag, struct FTW *ftwbuf);

void
delete_directory(ch_string& dir_nm);

bool 
file_touch(ch_string the_pth);

int
get_file_write_lock(ch_string lk_dir);
	
void
drop_file_write_lock(int fd_lock);

bool
file_exists(ch_string th_pth);

bool
file_newer_than(ch_string the_pth, time_t tm1);

time_t
path_verify(ch_string the_pth);

bool
make_dir(ch_string the_pth, mode_t mod);

ch_string
path_get_running_path();

#endif		// FILE_FUNCS_H


