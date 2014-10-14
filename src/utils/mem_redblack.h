

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

mem_redblack.h

Red-black trees with mem templates.

--------------------------------------------------------------*/


#ifndef MEM_REDBLACK_H
#define MEM_REDBLACK_H

#include "tools.h"
#include "redblack.h"

template<class obj_t>
class rbt_mem_node {
public:
	obj_t			obj;
	rbt_mem_node*	left;
	rbt_mem_node*	right;
	rbt_mem_node*	parent;
	rbt_color_t		color;

	rbt_mem_node(){
		init_rbt_mem_node();
	}
	~rbt_mem_node(){
		init_rbt_mem_node();
		//obj.~obj_t();
	}
	void	init_rbt_mem_node(){
		left = NULL_PT;
		right = NULL_PT;
		parent = NULL_PT;
		color = k_invalid_color;
	}

	bj_ostream&	print(bj_ostream& os){
		os << obj;
		return os;
	}

	friend bj_ostream& operator << (bj_ostream& os, rbt_mem_node& nod){
		nod.print(os);
		return os;
	}
};

template<class obj_t>
class rbt_mem_node_handler {
public:
	typedef obj_t					rbt_obj_t;
	typedef rbt_mem_node<rbt_obj_t>	rbt_node_t;
	typedef rbt_node_t*				rbt_nod_ref_t;
  
	typedef comparison (*cmp_func_t)(obj_t const & obj1, obj_t const & obj2);

	cmp_func_t	cmp_func;

	rbt_mem_node_handler(cmp_func_t cmp_fn) : cmp_func(cmp_fn){
		REDBLACK_CK(cmp_fn != NULL_PT);
	}

	~rbt_mem_node_handler(){
		cmp_func = NULL_PT;
		REDBLACK_CK(cmp_func == NULL_PT);
	}

	// Don't allow copying (error prone):
	// force use of referenced objs

	rbt_mem_node_handler&  operator = (
		rbt_mem_node_handler& other) 
	{ 
		RBT_OBJECT_COPY_ERROR; 
	}

	rbt_mem_node_handler(rbt_mem_node_handler& other){ 
		RBT_OBJECT_COPY_ERROR; 
	}

	rbt_nod_ref_t	create_node(rbt_obj_t const & obj1){
		rbt_nod_ref_t nd = tpl_malloc<rbt_node_t>();
		new (nd) rbt_node_t();
		nd->obj = obj1;
		return nd;
	}

	void	destroy_all_nodes(redblack<rbt_mem_node_handler<obj_t> >& rbt);

	comparison	compare_node_objects(rbt_obj_t const & obj1, rbt_obj_t const & obj2){
		REDBLACK_CK(cmp_func != NULL_PT);
		comparison cc_val = (*cmp_func)(obj1, obj2);
		return cc_val;
	}
	rbt_nod_ref_t	get_null_node_reference(){
		return NULL_PT;
	}
	rbt_nod_ref_t&	get_right_node_reference_of(rbt_nod_ref_t const & nd){
		REDBLACK_CK(nd != NULL_PT);
		return nd->right;
	}
	rbt_nod_ref_t&	get_left_node_reference_of(rbt_nod_ref_t const & nd){
		REDBLACK_CK(nd != NULL_PT);
		return nd->left;
	}
	rbt_nod_ref_t&	get_parent_node_reference_of(rbt_nod_ref_t const & nd){
		REDBLACK_CK(nd != NULL_PT);
		return nd->parent;
	}
	rbt_obj_t&	get_object_of(rbt_nod_ref_t const & nd){
		REDBLACK_CK(nd != NULL_PT);
		return nd->obj;
	}
	rbt_color_t	get_color_of(rbt_nod_ref_t const & nd){
		REDBLACK_CK(nd != NULL_PT);
		return nd->color;
	}
	void		set_color_of(rbt_nod_ref_t const & nd, rbt_color_t col){
		REDBLACK_CK(nd != NULL_PT);
		nd->color = col;
	}

	void		free_node(redblack<rbt_mem_node_handler<obj_t> >& rbt, 
				rbt_nod_ref_t nd)
	{
		REDBLACK_CK(nd != NULL_PT);
		REDBLACK_CK(rbt.is_alone(nd));
		nd->~rbt_node_t();
		tpl_free<rbt_node_t>(nd);
	}
};

template<class obj_t>
void
rbt_mem_node_handler<obj_t>::destroy_all_nodes(redblack<rbt_mem_node_handler<obj_t> >& rbt){
	while(! rbt.is_empty()){
		rbt_nod_ref_t mm = rbt.get_min();
		REDBLACK_CK(mm != NULL_PT);		
		rbt_nod_ref_t nd = rbt.rbt_remove(mm);
		free_node(rbt, nd);
	}
}

template<class obj_t>
class	mem_redblack : public redblack<rbt_mem_node_handler<obj_t> > {
public:
	typedef typename rbt_mem_node_handler<obj_t>::rbt_nod_ref_t	rbt_nod_ref_t;
	typedef typename rbt_mem_node_handler<obj_t>::rbt_node_t	rbt_node_t;
	//typedef typename rbt_mem_node_handler<obj_t>::rbt_obj_t 	rbt_obj_t;

	rbt_mem_node_handler<obj_t>			node_hdlr;

	typedef redblack<rbt_mem_node_handler<obj_t> > rbt_sup_t; 
	typedef redblack_iter<rbt_mem_node_handler<obj_t> > rbt_iter_t; 
	typedef comparison (*cmp_func_t)(obj_t const & obj1, obj_t const & obj2);

	mem_redblack(cmp_func_t cmp_fn) : redblack<rbt_mem_node_handler<obj_t> >(node_hdlr),
		node_hdlr(cmp_fn)
	{}

	~mem_redblack(){
		// MUST DO THIS so order destruction does not leave cmp in mgr in NULL
		rbt_sup_t::clear_redblack();
	}

	// Don't allow copying (error prone):
	// force use of referenced objs

	mem_redblack&  operator = (mem_redblack& other) 
	{ 
		RBT_OBJECT_COPY_ERROR; 
	}

	mem_redblack(mem_redblack& other){ 
		RBT_OBJECT_COPY_ERROR; 
	}

	bool	push(obj_t obj){
		bool p_ok = (rbt_insert(obj) != rbt_sup_t::get_null());
		return p_ok;
	}

	bool	pop(obj_t obj){
		rbt_nod_ref_t	ref_obj = search(obj);
		if(ref_obj == rbt_sup_t::get_null()){
			return false;
		}
		cmp_func_t cmp_fn = node_hdlr.cmp_func;
		MARK_USED(cmp_fn);

		REDBLACK_CK(cmp_fn(get_obj(ref_obj), obj) == 0);
		rbt_nod_ref_t	ref_pop = rbt_remove(ref_obj);

		REDBLACK_CK(ref_pop != rbt_sup_t::get_null());
		node_hdlr.free_node(*this, ref_pop);

		SLOW_REDBLACK_CK(check_min_max());
		return true;
	}

	bj_ostream&	print_row(bj_ostream& os){
		rbt_iter_t	iter1(*this);
		iter1.go_first_ref();
		os << "[";
		while(! iter1.in_null()){
			os << iter1.get_obj() << " ";
			iter1++;
		}	 
		os << "]";
		return os;
	}

	bj_ostream&	print_tree(bj_ostream& os){
		return rbt_sup_t::print(os);
	}

	friend 
	bj_ostream&	operator << (bj_ostream& os, mem_redblack& rbt){
		rbt.print_row(os);
		return os;
	}

	friend
	cmp_is_sub	cmp_mem_redblack(mem_redblack<obj_t>& rbt1, mem_redblack<obj_t>& rbt2,
				bool& are_eq)
	{
		rbt_iter_t	iter1(rbt1);
		rbt_iter_t	iter2(rbt2);
		cmp_func_t cmp_fn = rbt1.node_hdlr.cmp_func;
		REDBLACK_CK(cmp_fn == rbt2.node_hdlr.cmp_func);

		return is_subset_cmp<obj_t, rbt_iter_t>(iter1, 
			iter2, cmp_fn, are_eq); 
	}

};


#endif	// MEM_REDBLACK_H

