

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

redblack.h

Red-black trees template.

--------------------------------------------------------------*/


#ifndef REDBLACK_H
#define REDBLACK_H

#include "bj_mem.h"
#include "bj_stream.h"
#include "stack_trace.h"

#define DBG_RBT(prm) DBG(prm)
#define REDBLACK_CK(prm)	DBG_CK(prm)
//define REDBLACK_CK(prm)	/**/

//define SLOW_REDBLACK_CK(prm)	DBG_CK(prm)
#define SLOW_REDBLACK_CK(prm)	/**/

/*
The class passed to the template ('node_manager_t') must implement the following methods

	rbt_nod_ref_t	create_node(rbt_obj_t const & obj1);
	void			destroy_all_nodes(redblack<rbt_row_node_handler<obj_t> >& rbt);
	comparison		compare_node_objects(rbt_obj_t const & obj1, rbt_obj_t const & obj2);
	rbt_nod_ref_t	get_null_node_reference();
	rbt_nod_ref_t&	get_right_node_reference_of(rbt_nod_ref_t const & nd);
	rbt_nod_ref_t&	get_left_node_reference_of(rbt_nod_ref_t const & nd);
	rbt_nod_ref_t&	get_parent_node_reference_of(rbt_nod_ref_t const & nd);
	rbt_obj_t&	get_object_of(rbt_nod_ref_t const & nd);
	rbt_color_t		get_color_of(rbt_nod_ref_t const & nd);
	void			set_color_of(rbt_nod_ref_t const & nd, rbt_color_t col);

And have public typedefs for:

	rbt_nod_ref_t;		(type of node references ej: void*, long
	rbt_obj_t;		(type of objects ej: long, my_own_class
*/

typedef char	comparison;

enum rbt_color_t{
	k_invalid_color,
	k_red_color,
	k_black_color
};

//================================================================
// redblack

template <bool> struct RBT_ILLEGAL_USE_OF_OBJECT;
template <> struct RBT_ILLEGAL_USE_OF_OBJECT<true>{};
#define RBT_OBJECT_COPY_ERROR RBT_ILLEGAL_USE_OF_OBJECT<false>()

template<class node_manager_t>
class redblack {
public:
	typedef typename node_manager_t::rbt_nod_ref_t	rbt_nod_ref_t;
	typedef typename node_manager_t::rbt_obj_t 	rbt_obj_t;
	
protected:

	node_manager_t&		the_mgr;
	
	rbt_nod_ref_t	root;
	rbt_nod_ref_t	last_found;
	rbt_nod_ref_t	min_nod;
	rbt_nod_ref_t	max_nod;
	long			rbt_sz;

public:
	redblack(node_manager_t& mgr) : the_mgr(mgr){ 
		init_red_black();
	}

	~redblack(){
		clear_redblack();
	}

	// Don't allow copying (error prone):
	// force use of referenced objs

	redblack&  operator = (redblack& other) { 
		RBT_OBJECT_COPY_ERROR; 
	}

	redblack(redblack& other){ 
		RBT_OBJECT_COPY_ERROR; 
	}

	void 		clear_redblack(){
		destroy_nodes();
		init_red_black();
	}

	long		size(){
		return rbt_sz;
	}

	rbt_nod_ref_t	get_min(){
		return min_nod;
	}

	rbt_nod_ref_t	get_max(){
		return max_nod;
	}

	rbt_nod_ref_t	search(rbt_obj_t const & obj){
		rbt_nod_ref_t	the_parent;
		return search_node(obj, the_parent);
	}

	rbt_nod_ref_t	rbt_insert(rbt_obj_t const & obj);
	rbt_nod_ref_t	predecessor(rbt_nod_ref_t xx);
	rbt_nod_ref_t	successor(rbt_nod_ref_t xx);
	rbt_nod_ref_t	rbt_remove(rbt_nod_ref_t zz);
	bool			is_empty(){ 
		bool ee = (root == get_null()); 
		REDBLACK_CK(! ee || (rbt_sz == 0));
		return ee;
	}

	bool			check_tree(){
		print(bj_err, true);
		return true;
	}

	bool	check_order();
	
	bj_ostream&	print(bj_ostream& os, 
		bool just_chks = false, bool htm = false);

	friend 
	bj_ostream&	operator << (bj_ostream& os, redblack<node_manager_t>& rbt){
		rbt.print(os);
		return os;
	}

	/*
	friend 
	redblack<node_manager_t>&	operator << (redblack<node_manager_t>& rbt, 
						rbt_obj_t const & elem)
	{
		rbt.rbt_insert(elem);
		return rbt;
	}*/

	rbt_nod_ref_t	get_null(){
		return the_mgr.get_null_node_reference();
	}

	rbt_obj_t&	get_obj(rbt_nod_ref_t const & nd){
		return the_mgr.get_object_of(nd);
	}

	bool		is_alone(rbt_nod_ref_t const & nd);
	bool		check_refs(rbt_nod_ref_t const & nd, int from = 0);
	void		move_node(rbt_nod_ref_t const & nd, rbt_nod_ref_t const & nd_empty);

protected:
	
	rbt_nod_ref_t	create_node(rbt_obj_t const & obj1){
		return the_mgr.create_node(obj1);
	}

	void	destroy_nodes(){
		the_mgr.destroy_all_nodes(*this);
	}

	comparison	cmp(rbt_nod_ref_t nod1, rbt_nod_ref_t nod2){
		return the_mgr.compare_node_objects(get_obj(nod1), get_obj(nod2));
	}

	comparison	cmp_obj(rbt_nod_ref_t nod1, rbt_obj_t const & obj){
		return the_mgr.compare_node_objects(get_obj(nod1), obj);
	}

	comparison	cmp_objs(rbt_obj_t const & obj1, rbt_obj_t const & obj2){
		return the_mgr.compare_node_objects(obj1, obj2);
	}

	rbt_nod_ref_t&	get_right(rbt_nod_ref_t const & nd){
		return the_mgr.get_right_node_reference_of(nd);
	}

	rbt_nod_ref_t&	get_left(rbt_nod_ref_t const & nd){
		return the_mgr.get_left_node_reference_of(nd);
	}

	rbt_nod_ref_t&	get_parent(rbt_nod_ref_t const & nd){
		return the_mgr.get_parent_node_reference_of(nd);
	}

	rbt_color_t	get_color(rbt_nod_ref_t const & nd){
		return the_mgr.get_color_of(nd);
	}

	void		set_color(rbt_nod_ref_t const & nd, rbt_color_t col){
		the_mgr.set_color_of(nd, col);
	}

	rbt_nod_ref_t&	get_grand(rbt_nod_ref_t const & nd){
		return get_parent(get_parent(nd));
	}

	void 		init_red_black(){
		root = get_null();
		last_found = get_null();
		min_nod = get_null();
		max_nod = get_null();
		rbt_sz = 0;
	}

	bool		check_min_max(){
		rbt_nod_ref_t min_found = search_min(root);
		rbt_nod_ref_t max_found = search_max(root);
		REDBLACK_CK(min_nod == min_found);
		REDBLACK_CK(max_nod == max_found);
		REDBLACK_CK(check_refs(root, 1));
		REDBLACK_CK(check_refs(last_found, 2));
		REDBLACK_CK(check_refs(min_nod, 3));
		REDBLACK_CK(check_refs(max_nod, 4));
		return true;
	}

	void		insert_update_min_max(rbt_nod_ref_t nod){
		rbt_sz++;
		REDBLACK_CK(nod != get_null());
		if((min_nod == get_null()) || (cmp(min_nod, nod) > 0)){
			min_nod = nod;
		}
		if((max_nod == get_null()) || (cmp(max_nod, nod) < 0)){
			max_nod = nod;
		}
	}

	void		remove_update_min_max(rbt_nod_ref_t nod){
		rbt_sz--;
		REDBLACK_CK(nod != get_null());
		REDBLACK_CK(min_nod != get_null());
		REDBLACK_CK(max_nod != get_null());
		if(cmp(min_nod, nod) == 0){
			REDBLACK_CK(get_left(min_nod) == get_null());
			REDBLACK_CK(min_nod == nod);
			min_nod = successor(nod);
		}
		if(cmp(max_nod, nod) == 0){
			REDBLACK_CK(get_right(max_nod) == get_null());
			REDBLACK_CK(max_nod == nod);
			max_nod = predecessor(nod);
		}
	}

	void		move_update_min_max(rbt_nod_ref_t src,
					rbt_nod_ref_t dst)
	{
		if(last_found == src){
			last_found = dst;
		}
		if(min_nod == src){
			min_nod = dst;
		}
		if(max_nod == src){
			max_nod = dst;
		}
		if(root == src){
			root = dst;
		}
	}

	rbt_nod_ref_t	search_node(rbt_obj_t const & obj, rbt_nod_ref_t& the_parent);

	rbt_nod_ref_t	search_min(rbt_nod_ref_t xx);
	rbt_nod_ref_t	search_max(rbt_nod_ref_t xx);
	void		left_rotate(rbt_nod_ref_t xx);
	void		right_rotate(rbt_nod_ref_t yy);
	void		fix_remove(rbt_nod_ref_t xx, rbt_nod_ref_t parent_yy);

	bool	is_black(rbt_nod_ref_t nod){
		if(nod == get_null()){ return true; }
		return (get_color(nod) == k_black_color);
	}

	bool	is_red(rbt_nod_ref_t nod){
		if(nod == get_null()){ return false; }
		return (get_color(nod) == k_red_color);
	}

	void	set_black(rbt_nod_ref_t nod){
		if(nod == get_null()){ return; }
		set_color(nod, k_black_color);
	}

	void	set_red(rbt_nod_ref_t nod){
		if(nod == get_null()){ return; }
		set_color(nod, k_red_color);
	}

	bj_ostream&	print_rec(bj_ostream& os, rbt_nod_ref_t xx, int tb,
						bool& prev, rbt_nod_ref_t nod, int& tot,
						int ctr, bool just_cks);

};

//================================================================
// redblack_iter

template<class node_manager_t>
class redblack_iter {
public:
	typedef typename node_manager_t::rbt_nod_ref_t	rbt_nod_ref_t;
	typedef typename node_manager_t::rbt_obj_t 	rbt_obj_t;

	redblack<node_manager_t>& 	the_tree;
	rbt_nod_ref_t			the_ref;

	redblack_iter(redblack<node_manager_t>& rbt) : the_tree(rbt){
		the_ref = the_tree.get_null();
	}
	redblack_iter(redblack_iter& rbt_it): the_tree(rbt_it.the_tree){
		the_ref = the_tree.get_null();
	}
	~redblack_iter(){
	}

	long	size(){
		return the_tree.size();
	}

	void	go_first_ref(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 5));
		the_ref = the_tree.get_min();
	}

	void	go_last_ref(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 6));
		the_ref = the_tree.get_max();
	}

	rbt_obj_t&	get_obj(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 7));
		return the_tree.get_obj(the_ref); 
	}
	
	bool 	in_null(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 8));
		return (the_ref == the_tree.get_null());
	}

	void	go_next(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 9));
		the_ref = the_tree.successor(the_ref);
	}

	void	go_prev(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 10));
		the_ref = the_tree.predecessor(the_ref);
	}

	rbt_nod_ref_t 	get_ref(){
		REDBLACK_CK(the_tree.check_refs(the_ref, 11));
		return the_ref;
	}

	void 	operator ++ (){
		go_next();
	}

	void 	operator -- (){
		go_prev();
	}

	void 	operator ++ (int) { ++(*this); }

	void 	operator -- (int) { --(*this); }
};

//================================================================
// FUNCS

template<class node_manager_t>
bool
redblack<node_manager_t>::is_alone(rbt_nod_ref_t const & nd){
	bool is_e = (
		(nd != get_null()) &&
		(get_parent(nd) == get_null()) &&
		(get_right(nd) == get_null()) &&
		(get_left(nd) == get_null())
	);
	return is_e;
}

template<class node_manager_t>
bool
redblack<node_manager_t>::check_refs(rbt_nod_ref_t const & nd, int from){
	if(nd == get_null()){
		return true;
	}
			
	rbt_nod_ref_t pre = get_parent(nd);
	rbt_nod_ref_t lft = get_left(nd);
	rbt_nod_ref_t rgt = get_right(nd);

	if(pre != get_null()){
		rbt_nod_ref_t pre_lft = get_left(pre);
		rbt_nod_ref_t pre_rgt = get_right(pre);
		REDBLACK_CK((pre_lft == nd) || (pre_rgt == nd));
	}
	if(lft != get_null()){
		rbt_nod_ref_t lft_pre = get_parent(lft);
		REDBLACK_CK(lft_pre == nd);
	}
	if(rgt != get_null()){
		rbt_nod_ref_t rgt_pre = get_parent(rgt);
		REDBLACK_CK(rgt_pre == nd);
	}
	return true;
}

template<class node_manager_t>
void		
redblack<node_manager_t>::move_node(rbt_nod_ref_t const & nd, rbt_nod_ref_t const & alone){
	REDBLACK_CK(is_alone(alone));
	REDBLACK_CK(alone != nd);
	REDBLACK_CK(check_refs(nd, 12));
	REDBLACK_CK(check_refs(alone, 13));

	rbt_nod_ref_t pre = get_parent(nd);
	if(pre != get_null()){
		if(get_right(pre) == nd){
			get_right(pre) = alone;
		} else if(get_left(pre) == nd){
			get_left(pre) = alone;
		}
	}
	rbt_nod_ref_t lft_nod = get_left(nd);
	if(lft_nod != get_null()){
		get_parent(lft_nod) = alone;
	}
	rbt_nod_ref_t rgt_nod = get_right(nd);
	if(rgt_nod != get_null()){
		get_parent(rgt_nod) = alone;
	}

	get_obj(alone) = get_obj(nd);
	get_parent(alone) = get_parent(nd);
	get_left(alone) = get_left(nd);
	get_right(alone) = get_right(nd);
	set_color(alone, get_color(nd));

	get_parent(nd) = get_null();;
	get_left(nd) = get_null();;
	get_right(nd) = get_null();;
	REDBLACK_CK(is_alone(nd));

	move_update_min_max(nd, alone);

	REDBLACK_CK(check_refs(nd, 14));
	REDBLACK_CK(check_refs(alone, 15));
	SLOW_REDBLACK_CK(check_min_max());
}


template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::search_node(rbt_obj_t const & obj, rbt_nod_ref_t& the_parent)
{
	if((last_found != get_null()) && (cmp_obj(last_found, obj) == 0)){
		the_parent = get_parent(last_found);
		return last_found;
	}
	the_parent = get_null();
	rbt_nod_ref_t child = root;
	REDBLACK_CK(is_black(child));
	
	while((child != get_null()) && (cmp_obj(child, obj) != 0)){
		rbt_nod_ref_t lft = get_left(child);
		rbt_nod_ref_t rgt = get_right(child);
		REDBLACK_CK(!is_red(child) || (!is_red(lft) && !is_red(rgt)));

		the_parent = child;
		if(cmp_obj(child, obj) > 0){
			child = lft;
		} else {
			REDBLACK_CK(cmp_obj(child, obj) < 0);
			child = rgt;
		}
	}
	if(child != get_null()){ last_found = child; }
	return child;
}

template<class node_manager_t>
void
redblack<node_manager_t>::left_rotate(rbt_nod_ref_t xx){
	rbt_nod_ref_t yy = get_right(xx);
	rbt_nod_ref_t yy_lft = get_left(yy);

	REDBLACK_CK(check_refs(xx, 16));
	REDBLACK_CK(check_refs(yy, 17));
	
	get_right(xx) = yy_lft;
	if(yy_lft != get_null()){
		get_parent(yy_lft) = xx;
	}

	rbt_nod_ref_t parent_xx = get_parent(xx);

	get_parent(yy) = parent_xx;
	if(parent_xx == get_null()){
		root = yy;
	} else if(xx == get_left(parent_xx)){
		get_left(parent_xx) = yy;
	} else {
		REDBLACK_CK(xx == get_right(parent_xx));
		get_right(parent_xx) = yy;
	}

	get_left(yy) = xx;
	get_parent(xx) = yy;

	REDBLACK_CK(check_refs(parent_xx, 18));
	REDBLACK_CK(check_refs(yy_lft, 19));
	REDBLACK_CK(check_refs(xx, 20));
	REDBLACK_CK(check_refs(yy, 21));	
}


template<class node_manager_t>
void
redblack<node_manager_t>::right_rotate(rbt_nod_ref_t yy){
	rbt_nod_ref_t xx = get_left(yy);
	rbt_nod_ref_t xx_rgt = get_right(xx);

	REDBLACK_CK(check_refs(xx, 22));
	REDBLACK_CK(check_refs(yy, 23));
	
	get_left(yy) = xx_rgt;
	if(xx_rgt != get_null()){
		get_parent(xx_rgt) = yy;
	}

	rbt_nod_ref_t parent_yy = get_parent(yy);

	get_parent(xx) = parent_yy;
	if(parent_yy == get_null()){
		root = xx;
	} else if(yy == get_right(parent_yy)){
		get_right(parent_yy) = xx;
	} else {
		REDBLACK_CK(yy == get_left(parent_yy));
		get_left(parent_yy) = xx;
	}

	get_right(xx) = yy;
	get_parent(yy) = xx;

	REDBLACK_CK(check_refs(parent_yy, 24));
	REDBLACK_CK(check_refs(xx_rgt, 25));
	REDBLACK_CK(check_refs(xx, 26));
	REDBLACK_CK(check_refs(yy, 27));
}

template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::rbt_insert(rbt_obj_t const & obj){
	rbt_nod_ref_t parent = get_null();
	rbt_nod_ref_t child = search_node(obj, parent);
	if(child != get_null()){ 
		return get_null();
	}

	rbt_nod_ref_t zz = create_node(obj);
	REDBLACK_CK(zz != get_null());	 
	REDBLACK_CK(check_refs(zz, 28));

	get_parent(zz) = parent;
	if(parent == get_null()){
		root = zz;
	} else if(cmp_obj(parent, obj) > 0){
		REDBLACK_CK(get_left(parent) == get_null());
		get_left(parent) = zz;
	} else {
		REDBLACK_CK(cmp_obj(parent, obj) < 0);
		REDBLACK_CK(get_right(parent) == get_null());
		get_right(parent) = zz;
	}
	rbt_nod_ref_t created = zz;
	REDBLACK_CK(check_refs(created, 29));

	// fix tree (colors and structure)
	DBG_RBT(int rotations = 0);
	rbt_nod_ref_t xx = zz;
	set_red(xx);
	while((xx != root) && is_red(get_parent(xx))){
		if(get_parent(xx) == get_left(get_grand(xx))){
			rbt_nod_ref_t yy = get_right(get_grand(xx));
			if(is_red(yy)){
				set_black(get_parent(xx));
				set_black(yy);
				set_red(get_grand(xx));
				xx = get_grand(xx);
			} else {
				if(xx == get_right(get_parent(xx))){
					xx = get_parent(xx);
					left_rotate(xx);
					DBG_RBT(rotations++);
				} 
				REDBLACK_CK(xx == get_left(get_parent(xx)));
				set_black(get_parent(xx));
				set_red(get_grand(xx));
				right_rotate(get_grand(xx));
				REDBLACK_CK(is_black(get_parent(xx)));
				DBG_RBT(rotations++);
			}
		} else {
			REDBLACK_CK(get_parent(xx) == get_right(get_grand(xx)));
			rbt_nod_ref_t yy = get_left(get_grand(xx));
			if(is_red(yy)){
				set_black(get_parent(xx));
				set_black(yy);
				set_red(get_grand(xx));
				xx = get_grand(xx);
			} else {
				if(xx == get_left(get_parent(xx))){
					xx = get_parent(xx);
					right_rotate(xx);
					DBG_RBT(rotations++);
				}
				REDBLACK_CK(xx == get_right(get_parent(xx)));
				set_black(get_parent(xx));
				set_red(get_grand(xx));
				left_rotate(get_grand(xx));
				REDBLACK_CK(is_black(get_parent(xx)));
				DBG_RBT(rotations++);
			}
		}
	}
	set_black(root);
	REDBLACK_CK(rotations <= 2);
	insert_update_min_max(created);
	REDBLACK_CK(check_refs(created, 30));
	SLOW_REDBLACK_CK(check_min_max());
	return created;
}

template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::search_min(rbt_nod_ref_t xx){
	if(xx == get_null()){ return xx; }
	while(get_left(xx) != get_null()){
		xx = get_left(xx);
	}
	REDBLACK_CK(xx != get_null());
	return xx;
}

// added after adapt
template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::search_max(rbt_nod_ref_t xx){
	if(xx == get_null()){ return xx; }
	while(get_right(xx) != get_null()){
		xx = get_right(xx);
	}
	REDBLACK_CK(xx != get_null());
	return xx;
}

// added after adapt
template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::predecessor(rbt_nod_ref_t xx){
	REDBLACK_CK(xx != get_null());
	if(get_left(xx) != get_null()){
		return search_max(get_left(xx));
	}
	rbt_nod_ref_t yy = get_parent(xx);
	while((yy != get_null()) && (xx == get_left(yy))){
		xx = yy;
		yy = get_parent(yy);
	}
	return yy;
}

template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::successor(rbt_nod_ref_t xx){
	REDBLACK_CK(xx != get_null());
	if(get_right(xx) != get_null()){
		return search_min(get_right(xx));
	}
	rbt_nod_ref_t yy = get_parent(xx);
	while((yy != get_null()) && (xx == get_right(yy))){
		xx = yy;
		yy = get_parent(yy);
	}
	return yy;
}

template<class node_manager_t>
void
redblack<node_manager_t>::fix_remove(rbt_nod_ref_t xx, rbt_nod_ref_t parent){
	DBG_RBT(int rotations = 0);
	while((xx != root) && is_black(xx)){
		REDBLACK_CK(parent != get_null());
		REDBLACK_CK((xx == get_null()) || (parent == get_parent(xx)));
		if(xx == get_left(parent)){
			DBG_RBT(bool first_if = false);
			DBG(MARK_USED(first_if));
			rbt_nod_ref_t ww = get_right(parent);
			if(is_red(ww)){
				set_black(ww);
				set_red(parent);
				left_rotate(parent);
				ww = get_right(parent);
				DBG_RBT(first_if = true);
				DBG_RBT(rotations++);
			}
			if(	is_black(get_left(ww)) &&
				is_black(get_right(ww))	)
			{
				set_red(ww);
				xx = parent;
				if(xx != root){ parent = get_parent(xx); }
				else { parent = get_null(); }
				REDBLACK_CK(!first_if || is_red(xx));
			} else {
				if(is_black(get_right(ww))){
					rbt_nod_ref_t oldWw = ww;
					MARK_USED(oldWw);
					REDBLACK_CK(is_red(get_left(ww)));
					set_black(get_left(ww));
					set_red(ww);
					right_rotate(ww);
					ww = get_right(parent);
					REDBLACK_CK(get_right(ww) == oldWw);
					DBG_RBT(rotations++);
				}
				REDBLACK_CK(is_red(get_right(ww)));
				if(is_black(parent)){ set_black(ww); }
				else { set_red(ww); }
				set_black(parent);
				set_black(get_right(ww));
				left_rotate(parent);
				xx = root;
				parent = get_null();
				DBG_RBT(rotations++);
			}
		} else {
			DBG_RBT(int first_if = false);
			DBG(MARK_USED(first_if));
			REDBLACK_CK(xx == get_right(parent));
			rbt_nod_ref_t ww = get_left(parent);
			if(is_red(ww)){
				set_black(ww);
				set_red(parent);
				right_rotate(parent);
				ww = get_left(parent);
				DBG_RBT(first_if = true);
				DBG_RBT(rotations++);
			}
			if(	is_black(get_right(ww)) &&
				is_black(get_left(ww))	)
			{
				set_red(ww);
				xx = parent;
				if(xx != root){ parent = get_parent(xx); }
				else { parent = get_null(); }
				REDBLACK_CK(!first_if || is_red(xx));
			} else {
				if(is_black(get_left(ww))){
					rbt_nod_ref_t oldWw = ww;
					MARK_USED(oldWw);
					REDBLACK_CK(is_red(get_right(ww)));
					set_black(get_right(ww));
					set_red(ww);
					left_rotate(ww);
					ww = get_left(parent);
					REDBLACK_CK(get_left(ww) == oldWw);
					DBG_RBT(rotations++);
				}
				REDBLACK_CK(is_red(get_left(ww)));
				if(is_black(parent)){ set_black(ww); }
				else { set_red(ww); }
				set_black(parent);
				set_black(get_left(ww));
				right_rotate(parent);
				xx = root;
				parent = get_null();
				DBG_RBT(rotations++);
			}
		}
	}
	set_black(xx);
	REDBLACK_CK(rotations <= 3);
}

template<class node_manager_t>
typename redblack<node_manager_t>::rbt_nod_ref_t
redblack<node_manager_t>::rbt_remove(rbt_nod_ref_t zz){
	if(zz == get_null()){ return get_null(); }
	REDBLACK_CK(zz != get_null());
	REDBLACK_CK(check_refs(zz, 31));
	remove_update_min_max(zz);
	if(last_found == zz){
		last_found = get_null();
	}
 
	rbt_nod_ref_t yy = get_null();
	if(	(get_left(zz) == get_null()) || 
		(get_right(zz) == get_null())	)
	{
		yy = zz;
	} else {
		yy = successor(zz);
	}
	// yy is the node to remove

	rbt_nod_ref_t xx = get_null();
	if(get_left(yy) != get_null()){
		xx = get_left(yy);
	} else if(get_right(yy) != get_null()){
		xx = get_right(yy);
	}

	rbt_nod_ref_t parent_yy = get_parent(yy);

	if(xx != get_null()){ get_parent(xx) = parent_yy; }
	if(parent_yy == get_null()){
		root = xx;
	} else if(yy == get_left(parent_yy)){
		get_left(parent_yy) = xx;
	} else {
		REDBLACK_CK(yy == get_right(parent_yy));
		get_right(parent_yy) = xx;
	}
	if((yy != zz) && (yy != get_null())){
		move_update_min_max(yy, zz);
		get_obj(zz) = get_obj(yy);
	}
	if(is_black(yy)){ fix_remove(xx, parent_yy); }
	get_right(yy) = get_null();
	get_left(yy) = get_null();
	get_parent(yy) = get_null();

	SLOW_REDBLACK_CK(check_min_max());
	return yy;
}

template<class node_manager_t>
bj_ostream&
redblack<node_manager_t>::print_rec(bj_ostream& os, rbt_nod_ref_t xx, int tb,
						bool& prev, rbt_nod_ref_t lst_nod, int& tot,
						int ctr, bool just_cks)
{
	int ii = 0;
	if(xx == get_null()){ 
		ctr++;
		if(tot == 0){ tot = ctr; } 
		REDBLACK_CK((tot == 0) || (ctr == tot));

		if(! just_cks){
			for(ii = 0; ii < tb; ii++){ os << "\t"; }
			os << "#nil" << bj_eol;
		}
		return os;
	}

	char* cl = as_pt_char("");
	if(is_black(xx)){ ctr++; cl = as_pt_char("#"); }

	// right subtree
	rbt_nod_ref_t rgt = get_right(xx);
	REDBLACK_CK((rgt == get_null()) || (cmp(rgt, xx) > 0));
	REDBLACK_CK((rgt == get_null()) || (get_parent(rgt) == xx));
	REDBLACK_CK(!(is_red(xx) && is_red(rgt)));
	print_rec(os, rgt, tb+1, prev, lst_nod, tot, ctr, just_cks);

	// node
	if(! just_cks){
		for(ii = 0; ii < tb; ii++){ os << "\t"; }
		os.flush();
	}
	REDBLACK_CK(check_refs(xx, 32));
	char* min = as_pt_char("");
	if(xx == get_min()){
		min = as_pt_char("<");
	}
	char* max = as_pt_char("");
	if(xx == get_max()){
		max = as_pt_char(">");
	}
	if(! just_cks){
		os << cl << min << get_obj(xx) << max << bj_eol;
		os.flush();
	}
	if((lst_nod != get_null()) && (xx == lst_nod)){ 
		prev = true; 
	}

	// left subtree
	rbt_nod_ref_t lft = get_left(xx);
	REDBLACK_CK((lft == get_null()) || (cmp(lft, xx) < 0));
	REDBLACK_CK((lft == get_null()) || (get_parent(lft) == xx));
	REDBLACK_CK(!(is_red(xx) && is_red(lft)));
	print_rec(os, lft, tb+1, prev, lst_nod, tot, ctr, just_cks);
	return os;
}

template<class node_manager_t>
bj_ostream&
redblack<node_manager_t>::print(bj_ostream& os, bool just_chks, 
								bool htm)
{
	if(just_chks){
		htm = false;
	}
	if(htm){
		os << "<HEAD>" << bj_eol;
		os << "<TITLE>Sermo file</TITLE>" << bj_eol;
		os << "</HEAD>" << bj_eol;
		os << "<BODY>" << bj_eol;
		os << "<PRE>" << bj_eol;
	}

	int total = 0;
	bool prev = false;
	print_rec(os, root, 0, prev, last_found, total, 0, just_chks);
	REDBLACK_CK((last_found == get_null()) || prev);
	REDBLACK_CK(check_min_max());

	if(htm){
		os << "</PRE>" << bj_eol;
		os << "</BODY>" << bj_eol;
	}
	return os;
}

template<class node_manager_t>
bool
redblack<node_manager_t>::check_order(){
	redblack_iter<node_manager_t> iter1(*this);

	redblack<node_manager_t>::rbt_obj_t obj1;
	redblack<node_manager_t>::rbt_obj_t obj2;
	bool is_1 = true;
	
	iter1.go_first_ref();
	while(! iter1.in_null()){
		if(is_1){
			obj2 = iter1.get_obj();
		} else {
			obj1 = obj2;
			obj2 = iter1.get_obj();
			comparison cc = cmp_objs(obj1, obj2);
			if(cc >= 0){
				abort_func(0, as_pt_char("check_failed !!!"));
			}
		}
		iter1++;
	}	 
	return true;
}

#endif	// REDBLACK_H

