

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

support.cpp  

Global classes and functions that support and assist the system.

--------------------------------------------------------------*/

int		tests_main_(int argc, char** argv);

int	main(int argc, char** argv){
	int rr = tests_main_(argc, argv);
	return rr;
}


/*

==============================================================
method pointers

DECL example
char (the_class::*the_method_pt)(int param1);

ASSIG example
the_method_pt = &the_class::a_method_of_the_class;

CALL Example
char cc = (an_object_of_the_class.*the_method_pt)(3);


Note that "::*", ".*" and "->*" are actually 
separate C++ operators.  

	reason, reasoning, reasoned, reasoner

		inductive, induce, induced, inducing, inducible

		deductive, deduce, deduced, deducing, deducible

		learn, learned, learning, learnable, learner

		specify, specified, specifying, specifiable, 
		specifier, specifies, specification

	study, studying, studied, studiable, studies

		analytic, analyze, analysed, analysing, analyzable
		analyser, analysis

		synthetic, synthesize, synthesized, synthesizing, 
		synthesizer, synthesis

		memorize, memorized, memorizing, memorizable, memorizer

		simplify, simplified, simplifying, simplificative, 
		simplifier, simplifies, simplification


	specify induce deduce simplify learn analyze synthesize

*/

