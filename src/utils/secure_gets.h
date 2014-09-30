
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

secure_gets.h

A 'gets' function that does not echo.

--------------------------------------------------------------*/


#ifndef SECURE_GETS_H
#define SECURE_GETS_H

#include "tools.h"

#ifdef WIN32
#define DEL_KEY 8
#include "conio.h"
inline
void secure_gets(secure_row<char>& s_rr){
	char ky = 0;
	while(ky != '\r'){
		ky = getch();
		if(ky == DEL_KEY){
			if(! s_rr.is_empty()){
				s_rr.pop();
			}
		} else 
		if(ky != '\r'){
			s_rr.push((char)ky);
		}
	}
}
#endif

#ifdef __linux
#include "termios.h"
#define DEL_KEY 127
inline
void secure_gets(secure_row<char>& s_rr){
	char ky = 'X'; 
	struct termios original_t, new_t; 

	tcgetattr(fileno(stdin), &original_t); 
	new_t = original_t; 

	new_t.c_lflag &= ~(ICANON | ECHO); 
	tcsetattr(fileno(stdin), TCSAFLUSH, &new_t); 
	fflush(stdin); 

	while(ky != '\n'){
		ky = getchar();
		if(ky == DEL_KEY){
			if(! s_rr.is_empty()){
				s_rr.pop();
			}
		} else 
		if(ky != '\n'){
			s_rr.push((char)ky);
		}
	}

	tcsetattr(fileno(stdin), TCSANOW, &original_t); 
}
#endif

#endif // SECURE_GETS_H


