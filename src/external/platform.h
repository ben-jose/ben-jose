

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

platform.h

Some macros that allow to indent better platform 
dependant code.

Insted of

#ifdef MY_PLATFORM
	<some_code>
#endif

that can be messy when code has several platforms 
dependant code. Specially inside functions.

Write:
		
	MY_PLAT_COD(
		<some_code>
	);

to make the code more clear.

--------------------------------------------------------------*/


#ifndef PLATFORM_H
#define PLATFORM_H

#define MARK_USED(X)  ((void)(&(X)))

#ifdef WIN32
#define WIN32_COD(prm)	prm
#else
#define WIN32_COD(prm)	/**/
#endif


#ifdef __linux
#define LINUX_COD(prm)	prm
#else
#define LINUX_COD(prm)	/**/
#endif


#endif // PLATFORM_H


