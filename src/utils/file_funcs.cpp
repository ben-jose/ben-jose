

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

file_funcs.cpp  

file funcs.

--------------------------------------------------------------*/

#include <errno.h>
#include <fcntl.h>
#include <fstream>

#include "file_tree.h"

#include "tools.h"
#include "file_funcs.h"
#include "util_funcs.h"

void
write_in_str(average& the_avg, ch_string& av_str){
	av_str = "";
	std::stringstream ss1;
	ss1 << the_avg.avg << bj_eol;
	ss1 << the_avg.sz << bj_eol;
	ss1.flush();
	av_str = ss1.str();
}

void
read_from_str(average& the_avg, ch_string& av_str){
	std::stringstream ss2(av_str);
	ss2 >> the_avg.avg;
	ss2 >> the_avg.sz;
}

void
read_file(ch_string f_nam, row<char>& f_data){
	const char* ff_nn = f_nam.c_str();
	std::ifstream istm;
	istm.open(ff_nn, std::ios::binary);
	if(! istm.good() || ! istm.is_open()){
		//bj_ostr_stream& msg = file_err_msg(0, -1, "Could not open file ");
		//msg << "'" << f_nam << "'.";

		//DBG_THROW(bj_out << msg.str() << bj_eol);

		char* file_cannot_open = as_pt_char("Cannot open file exception");
		DBG_THROW_CK(file_cannot_open != file_cannot_open);
		throw file_exception(file_cannot_open);
		abort_func(1, file_cannot_open);
	}

	// get size of file:
	istm.seekg (0, std::ios::end);
	long file_sz = istm.tellg();
	istm.seekg (0, std::ios::beg);

	if(file_sz < 0){
		//bj_ostr_stream& msg = file_err_msg(0, -1,
		//	"Could not compute file size. ");
		//msg << "'" << f_nam << "'.";

		char* file_cannot_size = as_pt_char("Cannot compute file size exception");
		DBG_THROW_CK(file_cannot_size != file_cannot_size);
		throw file_exception(file_cannot_size);
		abort_func(1, file_cannot_size);
	}

	DBG_CK(sizeof(char) == 1);

	char* file_content = tpl_malloc<char>(file_sz + 1);
	istm.read(file_content, file_sz);
	long num_read = istm.gcount();
	if(num_read != file_sz){
		tpl_free<char>(file_content, file_sz + 1);

		//bj_ostr_stream& msg = file_err_msg(0, -1,
		//	"Could not read full file into memory. ");
		//msg << "'" << f_nam << "'.";

		char* file_cannot_fit = as_pt_char("Cannot fit file in memory exception");
		DBG_THROW_CK(file_cannot_fit != file_cannot_fit);
		throw file_exception(file_cannot_fit);
		abort_func(1, file_cannot_fit);
	}
	file_content[file_sz] = END_OF_SEC;

	s_row<char> tmp_rr;
	tmp_rr.init_data(file_content, file_sz + 1);

	f_data.clear();
	tmp_rr.move_to(f_data);

	DBG_CK(f_data.last() == END_OF_SEC);
}

bool
write_file(ch_string& the_pth, row<char>& dat, bool write_once){
	int fd = 0;
	
	if(write_once){
		if((fd = open(the_pth.c_str(), O_RDWR|O_CREAT|O_EXCL, 0444)) == -1){
			return false;
		}
	} else {
		if((fd = creat(the_pth.c_str(), 0777)) == -1){
			return false;
		}
	}

	const char* the_data = dat.get_c_array();
	long the_sz = dat.size();

	write(fd, the_data, the_sz);
	close(fd);

	return true;
}

bj_big_int_t
inc_fnum(ch_string f_nam){
	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDWR|O_CREAT, 0744);
	if(fd == -1){
		return -1;
	}

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return -2;
		} else {
			close(fd);
			return -3;
		}
	}

	bj_big_int_t the_num = 0;

	off_t pos1 = lseek(fd, 0, SEEK_END);
	if(pos1 == 0){
		the_num = 1;
		write(fd, "1", 1);
	}
	else if(pos1 != -1){
		off_t pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			close(fd);
			return -4;
		}

		ch_string num_str;
		char* pt_str = (char*)malloc(pos1 + 1);
		ssize_t nr = read(fd, pt_str, pos1);
		if(nr != pos1){
			close(fd);
			return -5;
		}
		pt_str[pos1] = 0;
		num_str = pt_str;
		free(pt_str);

		the_num = num_str;

		pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			close(fd);
			return -6;
		}

		pos0 = ftruncate(fd, 0);
		if(pos0 != 0){
			close(fd);
			return -7;
		}

		the_num++;
		ch_string num_str2 = the_num.get_str();
		off_t pos2 = num_str2.size();
		ssize_t nw = write(fd, num_str2.c_str(), pos2);
		if(nw != pos2){
			close(fd);
			return -8;
		}
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return -9;
	}

	close(fd);
	return the_num;
}

ch_string
get_fstr(ch_string f_nam){
	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDONLY, 0744);
	if(fd == -1){
		return "-1";
	}

	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return "-2";
		} else {
			close(fd);
			return "-3";
		}
	}

	ch_string the_val_str = "";

	off_t pos0 = 0;
	off_t pos1 = lseek(fd, 0, SEEK_END);
	if((pos1 != -1) && (pos1 != 0)){
		pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			close(fd);
			return "-4";
		}

		char* pt_str = (char*)malloc(pos1 + 1);
		ssize_t nr = read(fd, pt_str, pos1);
		if(nr != pos1){
			close(fd);
			return "-5";
		}
		pt_str[pos1] = 0;

		the_val_str = pt_str;
		free(pt_str);
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return "-6";
	}

	close(fd);
	return the_val_str;
}

long
set_fstr(ch_string f_nam, ch_string the_val_str){
	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDWR|O_CREAT, 0744);
	if(fd == -1){
		return -1;
	}

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return -2;
		} else {
			close(fd);
			return -3;
		}
	}

	off_t pos0 = lseek(fd, 0, SEEK_SET);
	if(pos0 != 0){
		close(fd);
		return -4;
	}

	pos0 = ftruncate(fd, 0);
	if(pos0 != 0){
		close(fd);
		return -5;
	}

	off_t pos2 = the_val_str.size();
	ssize_t nw = write(fd, the_val_str.c_str(), pos2);
	if(nw != pos2){
		close(fd);
		return -6;
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return -7;
	}

	close(fd);
	return 0;
}

//============================================================
// elapsed funcs

bj_big_float_t
update_elapsed(ch_string f_nam){
	struct stat sf1;
	time_t last_mtime = 0;

	int resp1 = stat(f_nam.c_str(), &sf1);
	if(resp1 == 0){
		last_mtime = sf1.st_mtime;
	}

	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDWR|O_CREAT|O_SYNC, 0744);
	if(fd == -1){
		return -1;
	}

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			fsync(fd); close(fd);
			return -2;
		} else {
			fsync(fd); close(fd);
			return -3;
		}
	}

	average	the_avg;

	off_t pos0 = 0;
	off_t pos1 = lseek(fd, 0, SEEK_END);
	if((pos1 != -1) && (pos1 != 0)){
		pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			fsync(fd); close(fd);
			return -4;
		}

		char* pt_str = (char*)malloc(pos1 + 1);
		ssize_t nr = read(fd, pt_str, pos1);
		if(nr != pos1){
			fsync(fd); close(fd);
			return -5;
		}
		pt_str[pos1] = 0;

		ch_string av_str = pt_str;

		free(pt_str);

		read_from_str(the_avg, av_str);

		time_t now_time = time(0);
		double dtm = difftime(now_time, last_mtime);
		bj_big_float_t nxt_elap = dtm;

		the_avg.add_val(nxt_elap);
	}

	ch_string out_str;
	write_in_str(the_avg, out_str);

	//bj_out << "out_str='" << out_str << "'" << bj_eol;
	//bj_out.flush();

	pos0 = lseek(fd, 0, SEEK_SET);
	if(pos0 != 0){
		fsync(fd); close(fd);
		return -6;
	}

	pos0 = ftruncate(fd, 0);
	if(pos0 != 0){
		fsync(fd); close(fd);
		return -7;
	}

	off_t endpos = out_str.size();
	ssize_t nw = write(fd, out_str.c_str(), endpos);
	if(nw != endpos){
		fsync(fd); close(fd);
		return -8;
	}
	fsync(fd);

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		fsync(fd); close(fd);
		return -9;
	}

	fsync(fd); close(fd);
	return the_avg.avg;
}

long
read_elapsed(ch_string f_nam, average& the_avg){
	the_avg.init_average();

	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDONLY, 0744);
	if(fd == -1){
		return -1;
	}

	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return -2;
		} else {
			close(fd);
			return -3;
		}
	}

	off_t pos0 = 0;
	off_t pos1 = lseek(fd, 0, SEEK_END);
	if((pos1 != -1) && (pos1 != 0)){
		pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			close(fd);
			return -4;
		}

		char* pt_str = (char*)malloc(pos1 + 1);
		ssize_t nr = read(fd, pt_str, pos1);
		if(nr != pos1){
			close(fd);
			return -5;
		}
		pt_str[pos1] = 0;

		ch_string av_str = pt_str;

		free(pt_str);

		read_from_str(the_avg, av_str);
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return -6;
	}

	close(fd);
	return 0;
}


ch_string
get_nftw_flag_str(long ff){
	ch_string out_str = "?ERROR?";
	switch(ff){
	case FTW_F:		out_str = "FTW_F";		break;
	case FTW_D:		out_str = "FTW_D";		break;
	case FTW_DNR:		out_str = "FTW_DNR";		break;
	case FTW_NS:		out_str = "FTW_NS";		break;
	case FTW_SL:		out_str = "FTW_SL";		break;
	case FTW_SLN:		out_str = "FTW_SLN";		break;
	case FTW_DP:		out_str = "FTW_DP";		break;
	};
	//DBG_PRT(83, os << out_str);
	return out_str;
}

ch_string
get_errno_str(long val_errno){
	ch_string out_str = "?ERROR?";
	switch(val_errno){
	case EACCES:		out_str = "EACCES";		break;
	case EBUSY:		out_str = "EBUSY";		break;
	case EFAULT:		out_str = "EFAULT";		break;
	case EINVAL:		out_str = "EINVAL";		break;
	case EISDIR:		out_str = "EISDIR";		break;
	case ELOOP:		out_str = "ELOOP";		break;
	case EMLINK:		out_str = "EMLINK";		break;
	case ENAMETOOLONG:	out_str = "ENAMETOOLONG";	break;
	case ENOENT:		out_str = "ENOENT";		break;
	case ENOMEM:		out_str = "ENOMEM";		break;
	case ENOSPC:		out_str = "ENOSPC";		break;
	case ENOTDIR:		out_str = "ENOTDIR";		break;
	case ENOTEMPTY:		out_str = "ENOTEMPTY";		break;
	case EEXIST:		out_str = "EEXIST";		break;
	case EPERM:		out_str = "EPERM";		break;
	case EROFS:		out_str = "EROFS";		break;
	case EXDEV:		out_str = "EXDEV";		break;
	};
	//DBG_PRT(83, os << out_str);
	return out_str;
}

int
delete_dir_entry(const char *fpath, const struct stat *sb,
			int tflag, struct FTW *ftwbuf)
{
	MARK_USED(sb);
	MARK_USED(ftwbuf);
	//DBG_PRT(95, os << "deleting entry " << fpath);

	switch (tflag) {
	case FTW_D:
	case FTW_DNR:
	case FTW_DP:
		{
			int rr1 = rmdir(fpath);
			MARK_USED(rr1);
			DBG_COND_COMM((rr1 != 0), os << "failed deleting dir " << fpath 
				<< " errno=" << errno << " " << get_errno_str(errno)
				<< " tflag=" << tflag << " " << get_nftw_flag_str(tflag));
			FILE_FN_CK((rr1 == 0) || (errno == ENOTEMPTY));
		}
		break;
	default:
		{
			int rr2 = unlink(fpath);
			MARK_USED(rr2);
			DBG_COND_COMM((rr2 != 0), os << "failed deleting file " << fpath 
				<< " errno=" << errno << " " << get_errno_str(errno)
				<< " tflag=" << tflag << " " << get_nftw_flag_str(tflag));
			FILE_FN_CK((rr2 == 0) || (errno == ENOTEMPTY));
		}
		break;
	}
	return (0);
}

void
delete_directory(ch_string& dir_nm){
	long max_depth = BJ_FILE_MAX_PATH_DEPTH;

	str_pos_t lst_pos = dir_nm.size() - 1;
	if(dir_nm[lst_pos] == '/'){
		dir_nm = dir_nm.substr(0, lst_pos);
	}

	//DBG_PRT(74, os << "deleting directory " << dir_nm);

	nftw(dir_nm.c_str(), delete_dir_entry, max_depth, FTW_DEPTH | FTW_PHYS);
}

bool 
file_touch(ch_string the_pth){
	//SKELETON_CK(not_skl_path(the_pth));
	int ok1 = utimensat(AT_FDCWD, the_pth.c_str(), NULL_PT, 0);
	return (ok1 == 0);
}

bool
path_is_dead_lock(ch_string the_pth){
	//SKELETON_CK(not_skl_path(the_pth));
	ch_string posfix = BJ_LOCK_NAME;
	long idx = the_pth.size() - posfix.size();
	if(idx < 0){ return false; }
	
	ch_string endstr = the_pth.substr(idx);
	if(endstr != posfix){ return false; }

	time_t tm1 = time(NULL_PT);
	if(tm1 == -1){ return false; }

	struct stat sf1;

	int resp1 = stat(the_pth.c_str(), &sf1);
	bool ok1 = (resp1 == 0);

	double dtm = difftime(tm1, sf1.st_mtime);

	bool is_dead = ok1 && (dtm >= BJ_SECS_DEAD_LOCK);
	return is_dead;
}

int
get_file_write_lock(ch_string full_nm){
	int fd_lock = open(full_nm.c_str(), O_RDWR|O_CREAT|O_EXCL, 0444);
	return fd_lock;
}

void
drop_file_write_lock(int fd_lock){
	close(fd_lock);
}

bool
file_exists(ch_string th_pth){
	std::ifstream istm;
	//SKELETON_CK(not_skl_path(th_pth));
	istm.open(th_pth.c_str(), std::ios_base::in);
	return istm.good();
}

bool
file_newer_than(ch_string the_pth, time_t tm1){
	//SKELETON_CK(not_skl_path(the_pth));
	struct stat sf1;

	int resp1 = stat(the_pth.c_str(), &sf1);
	bool ok1 = (resp1 == 0);

	double dtm = difftime(sf1.st_mtime, tm1);

	bool nwr_than = ok1 && (dtm > 0);
	return nwr_than;
}

time_t
path_verify(ch_string the_pth){
	time_t vrf_tm = -1;
	struct stat sf1;
	int ok1 = stat(the_pth.c_str(), &sf1);
	if(ok1 == 0){
		vrf_tm = sf1.st_mtime;
	} 
	return vrf_tm;
}

bool
make_dir(ch_string the_pth, mode_t mod){
	int resp = mkdir(the_pth.c_str(), mod);
	return (resp == 0);
}
