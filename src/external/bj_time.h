

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

bj_stream.h

Wrapper for time funcs.

--------------------------------------------------------------*/


#ifndef BJ_TIME_H
#define BJ_TIME_H

#ifdef WIN32
#include <ctime>
static inline double cpu_time(void){
	// time in seconds
	// This is a slow function use with care in performance code
	clock_t ct = clock();
	if(ct == -1){ return (double)ct; }
	return (((double)ct) / CLOCKS_PER_SEC);
}
#endif

#ifdef __linux
#include <sys/time.h>
#include <sys/resource.h>
static inline double cpu_time(void){
	// time in seconds
	// This is a slow function use with care in performance code
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; 
}
#endif

#endif // BJ_TIME_H


