
/*************************************************************

This file is part of ben-jose.

--------------------------------------------------------------*/

/*! 
\file macro_algorithm_ben_jose.cpp
\brief This is a documentation file to help understand the innerworking of the library. It is not compiled.

\details 
It is an outline of the functions that get called
during the solving of a SAT instance when the user of the 
library uses one of the three presentations of the 
ben-jose API top solve instances:

\see bj_solve_file
\see bj_solve_data
\see bj_solve_literals

This file is not compiled it is just to help in the 
understanding of the inner-works of the library, and to help
the interested programmer to get familiar with the backbone 
functions of the library.

The following are its contents:

\snippet macro_algorithm_ben_jose.cpp macro_bj_id

*/

//! [macro_bj_id]
#define THIS_CODE_IS_NOT_COMPILED

void
brain::solve_instance(){
	// init all brain data with any of:
	//		a DIMACS file
	//		a byte array (the chars in a DIMACS file)
	//		a literals array (long ints represent literals
	//			and a 0 separates clauses)
	load_instance(); 
	think();
}

void 
brain::think(){
	while(o_info.bjo_result == bjr_unknown_satisf){
		pulsate();
	}
}

void
brain::pulsate(){
	propagate_signals();
	if(found_conflict()){
		deduce_and_reverse_trail();
	} else{
		choose_quanton();
		start_propagation();
	}
}

void
brain::deduce_and_reverse_trail(){
	deduction& dct = br_pulse_deduc;
	reason& rsn = dct.dt_rsn;
	
	dct.reset_deduction();
	
	analyse_conflicts(br_all_conflicts_found, dct);
	reverse_with(rsn);
}

void
brain::analyse_conflicts(){
	deduction& dct = br_pulse_deduc;
	deducer& ddc = br_dedcer;
	
	dct.reset_deduction();
	ddc.deduce(dct); // good old conflict analysis. 
	//it finds a reason (clause) for a conflict
	
	candidate_find_analysis(ddc, dct); // try to go further by 
	// finding unsat sub-formulas

	row_neuromap_t& to_wrt = dct.dt_all_to_wrt;
	write_all_nmps(to_wrt);
	
}

void
brain::write_all_nmps(row<neuromap*>& to_wrt){
	for(int aa = 0; aa < to_wrt.size(); aa++){
		neuromap& nmp = *(to_wrt[aa]);
		nmp.map_write();
	}
}

void
brain::candidate_find_analysis(deducer& dedcer, deduction& dct){
	neuromap* out_nmp = NULL_PT;
	for(long aa = br_candidate_nmp_lvs.last_idx(); aa >= 0; aa--){
		out_nmp = br_candidate_nmp_lvs[aa];
		if(! out_nmp->map_find()){
			break;
		}
		dct.reset_deduction();
		dedcer.deduce(dct);
	}
}

bool
neuromap::map_find(){
	return map_oper(mo_find);
}

bool
neuromap::map_write(){
	return map_oper(mo_save);
}

bool
neuromap::map_oper(mem_op_t mm){
	// prepare the neuromap: stabilize it to find it's BCFF.
	bool prep_ok = map_prepare_mem_oper(mm);
	if(! prep_ok){
		return false;
	}
	brain& brn = get_brn();
	skeleton_glb& skg = brn.get_skeleton();
	
	canon_cnf& tmp_tauto_cnf = brn.br_tmp_wrt_tauto_cnf; // the unsat BCFF 
	canon_cnf& tmp_diff_cnf = brn.br_tmp_wrt_diff_cnf; // the diff between tauto and guide.
	canon_cnf& tmp_guide_cnf = brn.br_tmp_wrt_guide_cnf; // the guide of tauto
	
	if(mm == mo_find){
		instance_info& iinfo = brn.get_my_inst();
		row_neuron_t& all_found = na_found_col.co_neus;
		
		// find this neuromap in the skeleton:
		tmp_diff_cnf.first_vnt_i_super_of(skg, all_found, &iinfo); 
	} else {
		ch_string& tau_pth = na_tauto_pth;

		// write all relevant cnfs of this neuromap in the skeleton:
		tmp_tauto_cnf.save_cnf(skg, tau_pth);
		tmp_diff_cnf.save_cnf(skg, tau_pth);
		tmp_guide_cnf.save_cnf(skg, tau_pth);
	}
}

bool
neuromap::map_prepare_mem_oper(mem_op_t mm){
	brain& brn = get_brn();
	
	brn.all_mutual_init(); // init all stab aux params
	
	// final stab of the guide
	sort_glb& guide_ne_srg = brn.br_guide_neus_srg;
	sort_glb& guide_qu_srg = brn.br_guide_quas_srg;

	// init sort_glb info and do some stab.
	if(! has_stab_guide()){
		map_set_stab_guide();
	} else {
		guide_col.load_colors_into(guide_ne_srg, guide_qu_srg, dbg_call_1, this);
	}
	// final stab guide:
	guide_ne_srg.stab_mutual(guide_qu_srg, false);
	
	coloring final_guide_col(&brn);
	final_guide_col.save_colors_from(guide_ne_srg, guide_qu_srg, tid_tee_consec, false);

	coloring& ini_tau_col = brn.br_tmp_ini_tauto_col;
	map_get_initial_tauto_coloring(final_guide_col, ini_tau_col, mm);
	
	// final tauto stab of this neuromap
	brn.all_mutual_init();

	sort_glb& tauto_ne_srg = brn.br_tauto_neus_srg;
	sort_glb& tauto_qu_srg = brn.br_tauto_quas_srg;
	
	// init all sort_glb info:
	ini_tau_col.load_colors_into(tauto_ne_srg, tauto_qu_srg, dbg_call_3, this, true);
	
	tauto_ne_srg.stab_mutual_unique(tauto_qu_srg, this);
	
	map_prepare_wrt_cnfs(mm, quick_find_ref); // this sets the temp cnfs for this nmp.
}

void
sort_glb::stab_mutual_unique(sort_glb& srg2, neuromap* dbg_nmp){
	sort_glb& srg1 = *this;
	bool all_consec = false;
	while(! all_consec){
		stab_mutual_core(srg2);
		all_consec = srg2.sg_step_all_consec;
		if(! all_consec){
			srg2.stab_mutual_choose_one(srg1);
		}
	}
	stab_mutual_end(srg2, true);
}

void
sort_glb::stab_mutual(sort_glb& srg2, bool one_ccl_per_ss){
	stab_mutual_core(srg2);
	stab_mutual_end(srg2, one_ccl_per_ss);
}

void
sort_glb::stab_mutual_core(sort_glb& srg2){
	sort_glb& srg1 = *this;
	bool has_diff = true;
	while(has_diff){
		srg1.step_neus(srg2);
		bool diff1 = srg1.sg_step_has_diff;
		srg2.step_opps(srg1);
		srg2.step_quas(srg1);
		bool diff2 = srg2.sg_step_has_diff;
		has_diff = (diff1 || diff2);
	}
}

void
sort_glb::step_neus(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_neus);
}

void
sort_glb::step_opps(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_opps);
}

void
sort_glb::step_quas(sort_glb& mates_srg){
	step_mutual_stabilize(mates_srg, sm_with_quas);
}

void
sort_glb::step_mutual_stabilize(sort_glb& srg2, step_mutual_op_t op){
	sort_glb& srg1 = *this;

	// initialize some step info
	// (see the code)
	// .....
	
	row<sorset*>& sets = sg_step_prv_sorsets;
	sets.clear();

	sg_step_sorsets.move_to(sets);
	sg_step_sorsets.set_cap(sets.size());
	
	for(long aa = 0; aa < sets.size(); aa++){
		sorset& srs = *(sets[aa]);
		sets[aa] = NULL_PT;

		srs.step_mutual_stabilize_rec(srg1, srg2);
	}
	sets.clear();

	// setup final info (sg_cnf_clauses)
}

void
sorset::step_mutual_stabilize_rec(sort_glb& srg1, sort_glb& srg2){
	while(has_subsets()){
		sorset& nsr = first_subset();
		nsr.step_mutual_stabilize_rec(srg1, srg2);
	}
}

void
sorset::step_mutual_stabilize_rec(sort_glb& srg1, sort_glb& srg2){
	while(has_subsets()){
		sorset& nsr = first_subset();
		nsr.step_mutual_stabilize_rec(srg1, srg2);
	}
	
	binder* fst = ss_items.bn_right;
	binder* lst = &(ss_items);
	binder* rgt = NULL_PT;
	for(rgt = fst; rgt != lst; rgt = rgt->bn_right){
		sortee& srt = as_sortee(rgt);
		if(oper == sm_with_neus){
			row<sortee*>& all_mates = srt.so_related->so_mates;
			srg2.sort_all_from(all_mates, curr_stab_consec, false, 0, true, tc_none,
								&srg1, &srt); // it calls sort_from for all mates
		}
		if(oper == sm_with_opps){
			sortee& opp = srt.opposite();
			sorset& vssl = opp.get_vessel();
	
			if(&vssl != this){
				opp.sort_from(srg1, curr_stab_consec);
			}
		}
		if(oper == sm_with_quas){
			long qua_id = srt.get_qua_id(srg1);
			
			row<sortee*>& all_mates = srt.so_related->so_mates;
			srg2.sort_all_from(all_mates, curr_stab_consec, true, qua_id, false, tc_mates,
								&srg1, &srt); // it calls sort_from for all mates
		}
	}
}

//! [macro_bj_id]

