

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

row_redblack.h

Red-black trees in rows template.

--------------------------------------------------------------*/


#ifndef ROW_REDBLACK_H
#define ROW_REDBLACK_H

#include "tools.h"
#include "redblack.h"

template<class obj_t, class ref_t>
class rbt_row_node {
public:
	obj_t		obj;
	ref_t		left;
	ref_t		right;
	ref_t		parent;
	rbt_color_t	color;

	rbt_row_node(){
		init_rbt_node();
	}
	~rbt_row_node(){
		init_rbt_node();
	}
	void	init_rbt_node(){
		left = INVALID_IDX;
		right = INVALID_IDX;
		parent = INVALID_IDX;
		color = k_invalid_color;
	}

	std::ostream&	print(std::ostream& os){
		os << obj;
		return os;
	}

	friend std::ostream &operator<<(std::ostream& os, rbt_row_node& nod){
		nod.print(os);
		return os;
	}
};

template<class obj_t>
class rbt_row_node_handler {
public:
	typedef row_index				rbt_nod_ref_t;
	typedef obj_t					rbt_obj_t;
	typedef rbt_row_node<rbt_obj_t, rbt_nod_ref_t>	rbt_node_t;
  
	typedef comparison (*cmp_func_t)(obj_t const & obj1, obj_t const & obj2);

	row<rbt_node_t>	the_nodes;
	cmp_func_t	cmp_func;

	rbt_row_node_handler(cmp_func_t cmp_fn) : cmp_func(cmp_fn){
		REDBLACK_CK(cmp_fn != NULL_PT);
	}

	~rbt_row_node_handler(){
		cmp_func = NULL_PT;
	}

	// Don't allow copying (error prone):
	// force use of referenced objs

	rbt_row_node_handler&  operator = (
		rbt_row_node_handler& other) 
	{ 
		RBT_OBJECT_COPY_ERROR; 
	}

	rbt_row_node_handler(rbt_row_node_handler& other){ 
		RBT_OBJECT_COPY_ERROR; 
	}

	long	size(){
		return the_nodes.size();
	}

	rbt_nod_ref_t	create_node(rbt_obj_t const & obj1){
		rbt_node_t& nd = the_nodes.inc_sz();
		nd.obj = obj1;
		return the_nodes.last_idx();
	}

	void		destroy_subtree(rbt_nod_ref_t const & nd){
		the_nodes.clear(true, true);
	}

	comparison	compare_node_objects(rbt_obj_t const & obj1, rbt_obj_t const & obj2){
		return (*cmp_func)(obj1, obj2);
	}
	rbt_nod_ref_t	get_null_node_reference(){
		return INVALID_IDX;
	}
	rbt_nod_ref_t&	get_right_node_reference_of(rbt_nod_ref_t const & nd){
		return the_nodes[nd].right;
	}
	rbt_nod_ref_t&	get_left_node_reference_of(rbt_nod_ref_t const & nd){
		return the_nodes[nd].left;
	}
	rbt_nod_ref_t&	get_parent_node_reference_of(rbt_nod_ref_t const & nd){
		return the_nodes[nd].parent;
	}
	rbt_obj_t&	get_object_of(rbt_nod_ref_t const & nd){
		return the_nodes[nd].obj;
	}
	rbt_color_t	get_color_of(rbt_nod_ref_t const & nd){
		return the_nodes[nd].color;
	}
	void		set_color_of(rbt_nod_ref_t const & nd, rbt_color_t col){
		the_nodes[nd].color = col;
	}

	void		free_node(redblack<rbt_row_node_handler<obj_t> >& rbt, 
				rbt_nod_ref_t const & nd)
	{
		REDBLACK_CK(rbt.is_alone(nd));
		REDBLACK_CK(! the_nodes.is_empty());
		rbt_nod_ref_t lst_idx = the_nodes.last_idx();
		if(nd == lst_idx){
			the_nodes.dec_sz();
			return;
		}

		rbt.move_node(lst_idx, nd);
		REDBLACK_CK(rbt.is_alone(lst_idx));
		the_nodes.dec_sz();
		REDBLACK_CK(the_nodes.size() == rbt.size());
	}
};

template<class obj_t>
class	row_redblack : public redblack<rbt_row_node_handler<obj_t> > {
public:
	typedef typename rbt_row_node_handler<obj_t>::rbt_nod_ref_t	rbt_nod_ref_t;
	typedef typename rbt_row_node_handler<obj_t>::rbt_node_t	rbt_node_t;
	//typedef typename rbt_row_node_handler<obj_t>::rbt_obj_t 	rbt_obj_t;

	rbt_row_node_handler<obj_t>			node_hdlr;

	typedef redblack<rbt_row_node_handler<obj_t> > rbt_sup_t; 
	typedef redblack_iter<rbt_row_node_handler<obj_t> > rbt_iter_t; 
	typedef comparison (*cmp_func_t)(obj_t const & obj1, obj_t const & obj2);

	row_redblack(cmp_func_t cmp_fn) : redblack<rbt_row_node_handler<obj_t> >(node_hdlr),
		node_hdlr(cmp_fn)
	{}

	~row_redblack(){
	}

	// Don't allow copying (error prone):
	// force use of referenced objs

	row_redblack&  operator = (row_redblack& other) 
	{ 
		RBT_OBJECT_COPY_ERROR; 
	}

	row_redblack(row_redblack& other){ 
		RBT_OBJECT_COPY_ERROR; 
	}

	void	set_cap(row_index min_cap){
		node_hdlr.the_nodes.set_cap(min_cap);
	}

	bool	push(obj_t const & obj){
		bool p_ok = (insert(obj) != rbt_sup_t::get_null());
		return p_ok;
	}

	bool	pop(obj_t const & obj){
		rbt_nod_ref_t	ref_obj = search(obj);
		if(ref_obj == rbt_sup_t::get_null()){
			return false;
		}
		cmp_func_t cmp_fn = node_hdlr.cmp_func;

		REDBLACK_CK(cmp_fn(get_obj(ref_obj), obj) == 0);
		REDBLACK_CK(rbt_sup_t::size() == node_hdlr.size());
		rbt_nod_ref_t	ref_pop = remove(ref_obj);

		REDBLACK_CK(ref_pop != rbt_sup_t::get_null());
		node_hdlr.free_node(*this, ref_pop);
		REDBLACK_CK(rbt_sup_t::size() == node_hdlr.size());

		SLOW_REDBLACK_CK(check_min_max());
		return true;
	}

	std::ostream&	print_row(std::ostream& os){
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

	std::ostream&	print_tree(std::ostream& os){
		return rbt_sup_t::print(os);
	}

	friend 
	std::ostream&	operator << (std::ostream& os, row_redblack& rbt){
		rbt.print_row(os);
		return os;
	}

	friend
	cmp_is_sub	cmp_row_redblack(row_redblack<obj_t>& rbt1, row_redblack<obj_t>& rbt2,
				bool& are_eq)
	{
		rbt_iter_t	iter1(rbt1);
		rbt_iter_t	iter2(rbt2);
		cmp_func_t cmp_fn = rbt1.node_hdlr.cmp_func;
		REDBLACK_CK(cmp_fn == rbt2.node_hdlr.cmp_func);

		return is_subset_cmp<obj_t, rbt_iter_t>(iter1, 
			iter2, cmp_fn, are_eq); 
	}

	void	mem_copy_to(row_redblack& other){ 
		REDBLACK_CK(node_hdlr.cmp_func == 
			other.node_hdlr.cmp_func);

		other.clear_redblack();

		node_hdlr.the_nodes.mem_copy_to(
			other.node_hdlr.the_nodes);

		other.root = rbt_sup_t::root;
		other.last_found = rbt_sup_t::last_found;
		other.min_nod = rbt_sup_t::min_nod;
		other.max_nod = rbt_sup_t::max_nod;
		other.rbt_sz = rbt_sup_t::rbt_sz;
	}

	void	move_to(row_redblack& other){ 
		REDBLACK_CK(node_hdlr.cmp_func == 
			other.node_hdlr.cmp_func);

		other.clear_redblack();

		node_hdlr.the_nodes.move_to(
			other.node_hdlr.the_nodes);

		REDBLACK_CK(node_hdlr.the_nodes.is_empty());

		other.root = rbt_sup_t::root;
		other.last_found = rbt_sup_t::last_found;
		other.min_nod = rbt_sup_t::min_nod;
		other.max_nod = rbt_sup_t::max_nod;
		other.rbt_sz = rbt_sup_t::rbt_sz;

		rbt_sup_t::init_red_black();
	}

	friend 
	row_redblack&	operator << (row_redblack& rr1, row_data<obj_t>& rr2){
		for(row_index ii = 0; ii < rr2.size(); ii++){
			rr1 << rr2[ii];
		}
		return rr1;
	}

	friend 
	row_redblack&	operator << (row_redblack& rr1, row_redblack& rr2){
		rbt_iter_t	iter1(rr2);
		iter1.go_last_ref();
		while(! iter1.in_null()){
			rr1 << iter1.get_obj();
			iter1--;
		}	 
		return rr1;
	}

};


#endif	// ROW_REDBLACK_H

