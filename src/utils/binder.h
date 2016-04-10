

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

binder.h

binder class and related.

--------------------------------------------------------------*/

#ifndef BINDER_H
#define BINDER_H

#include "tools.h"

#define BINDER_CK(prm) 	DBG_CK(prm)

//=================================================================
// binder

class binder {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return binder::CL_NAME;
	}

	binder*		bn_left;
	binder*		bn_right;

	binder(){
		init_binder();
	}

	~binder(){
	}

	void		init_binder(){
		bn_left = this;
		bn_right = this;
	}

	bool	is_alone(){
		return ((bn_left == this) && (bn_right == this));
	}

	virtual
	void	let_go(){
		bn_left->bn_right = bn_right;
		bn_right->bn_left = bn_left;
		bn_left = this;
		bn_right = this;
	}

	bool	ck_binder(){
		BINDER_CK(bn_right->bn_left == this);
		BINDER_CK(bn_left->bn_right == this);
		return true;
	}

	void	bind_to_my_right(binder& the_rgt){
		BINDER_CK(the_rgt.is_alone());
		BINDER_CK(ck_binder());

		the_rgt.bn_right = bn_right;
		the_rgt.bn_left = this;
		bn_right->bn_left = &the_rgt;
		bn_right = &the_rgt;

		BINDER_CK(the_rgt.ck_binder());
		BINDER_CK(ck_binder());
	}

	void	bind_to_my_left(binder& the_lft){
		BINDER_CK(the_lft.is_alone());
		BINDER_CK(ck_binder());

		the_lft.bn_left = bn_left;
		the_lft.bn_right = this;
		bn_left->bn_right = &the_lft;
		bn_left = &the_lft;

		BINDER_CK(the_lft.ck_binder());
		BINDER_CK(ck_binder());
	}

	bool	is_grip();
	bool	is_leftmost();
	bool	is_rightmost();

	bool	is_last_in_grip(){
		return (is_leftmost() && is_rightmost());
	}
};

//=================================================================
// grip

class grip : public binder {
public:
	static
	char*	CL_NAME;

	virtual
	char*	get_cls_name(){
		return grip::CL_NAME;
	}

	virtual
	void	let_go(){
		BINDER_CK(false);
	}

	bool	is_single(){
		return (! is_alone() && (bn_left == bn_right));
	}

	bool	is_multiple(){
		return (! is_alone() && ! is_single());
	}

	void	move_all_to_my_right(grip& grp){
		if(grp.is_alone()){
			return;
		}

		BINDER_CK(ck_binder());

		binder* new_rgt = grp.bn_right;
		binder* new_mid = grp.bn_left;
		binder* old_rgt = bn_right;

		grp.bn_right = &grp;
		grp.bn_left = &grp;

		bn_right = new_rgt;
		bn_right->bn_left = this;

		new_mid->bn_right = old_rgt;
		new_mid->bn_right->bn_left = new_mid;

		BINDER_CK(grp.is_alone());
		BINDER_CK(new_rgt->ck_binder());
		BINDER_CK(new_mid->ck_binder());
		BINDER_CK(old_rgt->ck_binder());
		BINDER_CK(ck_binder());
	}

	void	move_all_to_my_left(grip& grp){
		if(grp.is_alone()){
			return;
		}

		BINDER_CK(ck_binder());

		binder* new_lft = grp.bn_left;
		binder* new_mid = grp.bn_right;
		binder* old_lft = bn_left;

		grp.bn_right = &grp;
		grp.bn_left = &grp;

		bn_left = new_lft;
		bn_left->bn_right = this;

		new_mid->bn_left = old_lft;
		new_mid->bn_left->bn_right = new_mid;

		BINDER_CK(grp.is_alone());
		BINDER_CK(new_lft->ck_binder());
		BINDER_CK(new_mid->ck_binder());
		BINDER_CK(old_lft->ck_binder());
		BINDER_CK(ck_binder());
	}

	void 	let_all_go(){
		while(! is_alone()){
			bn_right->let_go();
		}
	}

	void 	forced_let_go(){
		binder::let_go();
	}

	template<class obj_t1>
	void		append_all_as(row<obj_t1*>& rr, bool clr = false);
};

inline
bool
binder::is_grip(){
	bool is_g = (get_cls_name() == grip::CL_NAME);
	return is_g;
}

inline
bool
binder::is_leftmost(){
	BINDER_CK(! is_grip());
	return bn_left->is_grip();
}

inline
bool
binder::is_rightmost(){
	BINDER_CK(! is_grip());
	return bn_right->is_grip();
}

//=================================================================
// receptor

template<class obj_t1>
class receptor : public binder {
public:
	virtual
	char*	get_cls_name(){
		return obj_t1::CL_NAME;
	}

	obj_t1*		re_me;

	receptor(){
		init_receptor();
	}

	~receptor(){
	}

	void	init_receptor(){
		re_me = NULL_PT;
	}

	receptor(obj_t1& tgt){
		re_me = &tgt;
	}

	receptor(obj_t1* tgt){
		re_me = tgt;
	}

	operator obj_t1* () {
		return re_me;
	}

	operator obj_t1& () {
		BINDER_CK(re_me != NULL_PT);
		return *re_me;
	}
};

//=================================================================
// TEMPLATE DEFS

template<class obj_t1>
obj_t1&
rcp_as(binder* bdr){
	BINDER_CK(bdr != NULL_PT);
	BINDER_CK(bdr->get_cls_name() == obj_t1::CL_NAME);

	receptor<obj_t1>& rcp = *((receptor<obj_t1>*)(bdr));
	BINDER_CK(rcp.re_me != NULL_PT);

	obj_t1& obj_1 = rcp;
	return obj_1;
}


template<class obj_t1>
void
grip::append_all_as(row<obj_t1*>& rr, bool clr){
	if(clr){
		rr.clear();
	}

	binder* fst_bdr = bn_right;
	binder* lst_bdr = this;
	for(binder* bdr_1 = fst_bdr; bdr_1 != lst_bdr; bdr_1 = bdr_1->bn_right){
		obj_t1& ob_1 = rcp_as<obj_t1>(bdr_1);
		rr.push(&ob_1);
	}
}


#endif		// BINDER_H


