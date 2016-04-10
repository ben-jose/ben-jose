

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

#ifdef FULL_DEBUG
#define DBG(prm) prm
#define NOT_DBG(prm) /**/ 
#else
#define DBG(prm) /**/ 
#define NOT_DBG(prm) prm
#endif

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


