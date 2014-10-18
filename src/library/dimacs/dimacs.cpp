

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

dimacs.cpp  

Functions to read and parse dimacs files.

--------------------------------------------------------------*/

#include "tools.h"
#include "dimacs.h"
#include "config.h"
#include "file_funcs.h"

#include "support.h"

#define DIMACS_CK(prm) 	DBG_CK(prm)

// '\0'


ch_string	k_dimacs_header_str =
		"c (C) 2010. QUIROGA BELTRAN, Jose Luis. Bogota - Colombia.\n"
		"c Date of birth: December 28 of 1970.\n"
		"c Place of birth: Bogota - Colombia - Southamerica.\n"
		"c Id (cedula): 79523732 de Bogota.\n";


bj_ostr_stream& dimacs_err_msg(long num_line, char ch_err, ch_string msg){
	return parse_err_msg("DIMACS ERROR. ", num_line, ch_err, msg);
}

void skip_cnf_decl(const char*& pt_in, long line){
	if(*pt_in == 0){
		bj_ostr_stream& msg =
			dimacs_err_msg(line, -1, "Expecting 'cnf' declaration line.");
		MARK_USED(msg);

		char* dimacs_cnf_decl = as_pt_char("Expecting 'cnf' declaration dimacs exception");
		DBG_THROW_CK(dimacs_cnf_decl != dimacs_cnf_decl);
		throw dimacs_exception(dimacs_cnf_decl);
		abort_func(1, dimacs_cnf_decl);
	}

	const char* cnf_str = "cnf";
	const int cnf_str_sz = 3;
	if(memcmp(pt_in, cnf_str, cnf_str_sz) != 0){
		bj_ostr_stream& msg =
			dimacs_err_msg(line, -1, "Expecting 'cnf' declaration line.");
		MARK_USED(msg);

		char* dimacs_cnf_decl_2 = as_pt_char("Expecting cnf decl (case 2) dimacs exception");
		DBG_THROW_CK(dimacs_cnf_decl_2 != dimacs_cnf_decl_2);
		throw dimacs_exception(dimacs_cnf_decl_2);
		abort_func(1, dimacs_cnf_decl_2);
	}
	pt_in += cnf_str_sz;
}

void read_problem_decl(const char*& pt_in, long& num_var, long& num_ccl, long& line){
	num_var = 0;
	num_ccl = 0;

	pt_in++;
	skip_whitespace(pt_in, line);
	skip_cnf_decl(pt_in, line);
	skip_whitespace(pt_in, line);
	num_var = parse_int(pt_in, line);
	skip_whitespace(pt_in, line);
	num_ccl = parse_int(pt_in, line);
	skip_line(pt_in, line);
}

bool
dimacs_loader::fix_lits(row<long>& lits, bool& add_it)
{
	DIMACS_CK(ld_dots.nb_set_count == 0);
	DIMACS_CK(! lits.is_empty());
	bool fixed = false;
	add_it = false;

	row<long> tmp_dens(lits.size());
	tmp_dens.clear();

	bool satisfied = false;
	for(long ii = 0; ii < lits.size(); ii++){
		long nid = lits[ii];

		if(! ld_dots.any_true(nid)){
			ld_dots.set(nid);
			tmp_dens.push(nid);
			continue;
		}

		if(ld_dots.is_true(nid)){
			DBG_PRT(7, os << "lit_already_in_clause." <<
				" lits=" << lits <<
				" nid=" << nid <<
				" tmp_nids=" << tmp_dens << bj_eol
			);
			ld_fixes[fx_lit_already_in_clause]++;
			continue;
		}

		DIMACS_CK(ld_dots.is_true(-nid));

		DBG_PRT(7, os << "clause_has_both_lit." <<
			" lits=" << lits <<
			" nid=" << nid <<
			" tmp_nids=" << tmp_dens << bj_eol
		);
		ld_fixes[fx_clause_has_both_lit]++;
		satisfied = true;
		continue;
	}

	lits.clear();

	for(long kk = 0; kk < tmp_dens.size(); kk++){
		long n_id = tmp_dens[kk];
		ld_dots.reset(n_id);
		lits.push(n_id);
	}

	if(tmp_dens.is_empty()){
		DBG_PRT(7, os << "empty_clause." <<
			" lits=" << lits <<
			" tmp_nids=" << tmp_dens
		);
		ld_fixes[fx_empty_clause]++;
	}

	if((tmp_dens.size() == 1) && !satisfied){
		DBG_PRT(7, os << "clause_has_one_lit." <<
			" lits=" << lits <<
			" tmp_lits=" << tmp_dens << bj_eol
		);
		ld_fixes[fx_clause_has_one_lit]++;

		fixed = true;
		add_it = true;
	}

	if((tmp_dens.size() > 1) && !satisfied){
		fixed = true;
		add_it = true;
	}

	if(satisfied){
		fixed = true;
		add_it = false;
	}

	DIMACS_CK(ld_dots.nb_set_count == 0);
	DIMACS_CK(fixed);	// BECAUSE WE ARE NOT DOING SOLIDS
	return fixed;
}

bool
dimacs_loader::fixappend_lits(row<long>& lits, row<long>& prepared){
	bool add_it = false;
	bool fixed = fix_lits(lits, add_it);

	DIMACS_CK(fixed);
	if(! fixed){
		return false;
	}
	if(! add_it){
		lits.clear();
		return true;
	}

	if(lits.size() > ld_max_in_ccl_sz){
		ld_max_in_ccl_sz = lits.size();
	}

	//lits.mix_sort(cmp_abs_long);
	if(ld_as_3cnf){
		three_lits(lits, prepared);
	}

	add_lits(lits, prepared, true);

	return true;
}

void
dimacs_loader::add_lits(row<long>& lits, row<long>& prepared, bool is_orig){
	if(! is_orig){
		ld_nud_added_ccls++;
		ld_nud_added_lits += lits.size();

		if(lits.size() == 2){ ld_nud_added_twolits++; }
	}

	lits.append_to(prepared);
	prepared.push(0);

	lits.clear();
}

void
dimacs_loader::append_rr_lits(row_row_long_t& rr_lits, row<long>& prepared){
	while(! rr_lits.is_empty()){
		row<long>& ff = rr_lits.last();

		add_lits(ff, prepared, false);

		rr_lits.dec_sz();
	}
	DIMACS_CK(rr_lits.is_empty());
}

void
dimacs_loader::three_lits(row<long>& lits, row<long>& prepared){
	DIMACS_CK(ld_decl_vars > 0);

	DBG_PRT(8, os << "THREE_LITS " << lits);

	long max_ccl_sz = 3;
	long def_sz = 2;
	MARK_USED(def_sz);

	while(lits.size() > max_ccl_sz){
		long lit_1 = lits.pop();
		long lit_0 = lits.pop();

		DIMACS_CK(ld_lits2.is_empty());
		ld_lits2.push(lit_0);
		ld_lits2.push(lit_1);

		ld_nud_added_vars++;
		long curr_max_var = ld_decl_vars + ld_nud_added_vars;
		DIMACS_CK(curr_max_var > 0);

		lits.push(curr_max_var);

		DIMACS_CK(ld_rr_lits1.is_empty());
		DIMACS_CK(ld_lits2.size() == def_sz);

		calc_f_lit_equal_or(curr_max_var, ld_lits2, ld_rr_lits1);
		DIMACS_CK(ld_lits2.is_empty());
		//ld_lits2.clear();

		append_rr_lits(ld_rr_lits1, prepared);
	}
}

void
dimacs_loader::calc_f_lit_equal_or(long d_lit, row<long>& or_lits,
			row_row_long_t& rr_lits)
{
	DBG_PRT(41, os << "EQ_OR. d_lit=" << d_lit << " or_lits=" << or_lits);

	DIMACS_CK(d_lit > 0);
	DIMACS_CK(! or_lits.is_empty());

	row<long>& f1 = rr_lits.inc_sz();

	or_lits.copy_to(f1);
	f1.push(-d_lit);

	DBG_PRT(41, os << "EQ_OR. ccl=" << f1);

	long ii = 0;
	for(ii = 0; ii < or_lits.size(); ii++){
		row<long>& f2 = rr_lits.inc_sz();
		long o_lit = or_lits[ii];

		f2.push(-o_lit);
		f2.push(d_lit);

		DBG_PRT(41, os << "EQ_OR. ccl(" << ii << ")=" << f2);
	}

	or_lits.clear();
}

void
dimacs_loader::init_parse(){
	ld_max_in_ccl_sz = 0;

	ld_num_line = 0;
	ld_cursor = NULL_PT;

	ld_decl_ccls = INVALID_NATURAL;
	ld_decl_vars = INVALID_NATURAL;
	//ld_decl_lits = INVALID_NATURAL;


	ld_parsed_ccls = 0;
	ld_parsed_lits = 0;
	ld_parsed_twolits = 0;

	ld_dots.clear();

	ld_fixes.fill(0, k_last_fix);

	ld_lits1.clear();
	ld_lits2.clear();

	ld_rr_lits1.clear();
	ld_rr_lits2.clear();

	ld_last_or_lit = 0;

	ld_nud_added_ccls = 0;
	ld_nud_added_vars = 0;
	ld_nud_added_lits = 0;
	ld_nud_added_twolits = 0;

	ld_num_ccls = INVALID_NATURAL;
	ld_num_vars = INVALID_NATURAL;
	ld_tot_lits = INVALID_NATURAL;
	ld_tot_twolits = INVALID_NATURAL;
}

void
dimacs_loader::init_dimacs_loader(){

	ld_file_name = "";

	ld_as_3cnf = false;

	init_parse();
}

void
dimacs_loader::verif_num_ccls(ch_string& f_nam, long num_decl_ccls, long num_read_ccls){
	if(num_read_ccls != num_decl_ccls){
		bj_ostr_stream& msg_err = dimacs_err_msg(-1, -1,
			"Wrong number of clauses. ");
		msg_err << "Declared " << num_decl_ccls << " clauses and found ";
		msg_err << num_read_ccls << " in file " << f_nam;

		char* dimacs_bad_num_cls = as_pt_char("Bad num clauses dimacs exception");
		DBG_THROW_CK(dimacs_bad_num_cls != dimacs_bad_num_cls);
		throw dimacs_exception(dimacs_bad_num_cls);
		abort_func(1, dimacs_bad_num_cls);
	}
}

void
dimacs_loader::load_file(ch_string& f_nam){
	ld_file_name = f_nam;
	// loads the full file into ld_content
	read_file(ld_file_name, ld_content);

	DBG_PRT(11, os << " ld_content=" << ld_content.print_row_data(os, true, ""));
}

void
dimacs_loader::parse_header(){
	init_parse();

	long& num_ccl = ld_decl_ccls;
	long& num_var = ld_decl_vars;

	ch_string& f_nam = ld_file_name;

	ld_num_line = 1;
	const char*& pt_in = ld_cursor;
	pt_in = ld_content.get_c_array();
	if(pt_in == NULL_PT){
		return;
	}

	for(;;){
		skip_whitespace(pt_in, ld_num_line);
		if(*pt_in == 0){
			bj_ostr_stream& msg = dimacs_err_msg(ld_num_line, -1,
				"File without 'cnf' declaration. ");
			msg << "'" << f_nam << "'.";

			DBG_THROW(bj_out << msg);

			char* dimacs_no_cnf_decl = as_pt_char("No cnf declr dimacs exception");
			DBG_THROW_CK(dimacs_no_cnf_decl != dimacs_no_cnf_decl);
			throw dimacs_exception(dimacs_no_cnf_decl);
			abort_func(1, dimacs_no_cnf_decl);
			break;
		} else if(*pt_in == 'c'){
			skip_line(pt_in, ld_num_line);
		} else if(*pt_in == 'p'){
			read_problem_decl(pt_in, num_var, num_ccl, ld_num_line);
			break;
		} else {
			bj_ostr_stream& msg = dimacs_err_msg(ld_num_line, -1,
				"Invalid file format. It not a DIMACS file. ");
			msg << "'" << f_nam << "'.";

			char* dimacs_bad_format = as_pt_char("Bad format dimacs exception");
			DBG_THROW_CK(dimacs_bad_format != dimacs_bad_format);
			throw dimacs_exception(dimacs_bad_format);
			abort_func(1, dimacs_bad_format);
			break;
		}
	}

	if(num_var == 0){
		bj_ostr_stream& msg = dimacs_err_msg(ld_num_line, -1,
			"Invalid 'cnf' declaration. Declared 0 variables. ");
		msg << "'" << f_nam << "'.";

		char* dimacs_zero_vars = as_pt_char("Zero vars dimacs exception");
		DBG_THROW_CK(dimacs_zero_vars != dimacs_zero_vars);
		throw dimacs_exception(dimacs_zero_vars);
		abort_func(1, dimacs_zero_vars);
	}

	if(num_ccl == 0){
		bj_ostr_stream& msg = dimacs_err_msg(ld_num_line, -1,
			"Invalid 'cnf' declaration. Declared 0 clauses. ");
		msg << "'" << f_nam << "'.";

		char* dimacs_zero_cls = as_pt_char("Zero clauses dimacs exception");
		DBG_THROW_CK(dimacs_zero_cls != dimacs_zero_cls);
		throw dimacs_exception(dimacs_zero_cls);
		abort_func(1, dimacs_zero_cls);
	}
}

bool
dimacs_loader::parse_clause(row<integer>& lits){
	lits.clear();

	const char*& pt_in = ld_cursor;

	if(pt_in == NULL_PT){
		return false;
	}

	if(*pt_in == END_OF_SEC){
		return false;
	}

	skip_whitespace(pt_in, ld_num_line);

	DIMACS_DBG(row<integer> not_mapped);

	integer	parsed_lit;
	while(*pt_in != END_OF_SEC){
		skip_whitespace(pt_in, ld_num_line);
		parsed_lit = parse_int(pt_in, ld_num_line);
		if(parsed_lit == 0){ break; }
		if(get_var(parsed_lit) > ld_decl_vars){
			bj_ostr_stream& msg =
				dimacs_err_msg(ld_num_line, -1, "Invalid literal ");
			msg << "'" << parsed_lit << "'.";
			msg << "Declared " << ld_decl_vars << " literals.";

			char* dimacs_bad_lit = as_pt_char("Bad literal dimacs exception");
			DBG_THROW_CK(dimacs_bad_lit != dimacs_bad_lit);
			throw dimacs_exception(dimacs_bad_lit);
			abort_func(1, dimacs_bad_lit);
		}

		lits.push(parsed_lit);
		/*
		if(lits.size() > MAX_CLAUSE_SZ){
			bj_ostr_stream& msg =
				dimacs_err_msg(ld_num_line, -1, "Clause too long. ");
			msg << "Max allowed size is " << MAX_CLAUSE_SZ << " literals.";

			char* dimacs_cls_too_long = as_pt_char("Clause too long dimacs exception");
			DBG_THROW_CK(dimacs_cls_too_long != dimacs_cls_too_long);
			throw dimacs_exception(dimacs_cls_too_long);
			abort_func(1, dimacs_cls_too_long);
		}
		*/
	}

	if(lits.is_empty()){
		return false;
	}

	ld_parsed_ccls++;
	ld_parsed_lits += lits.size();

	if(lits.size() == 2){ ld_parsed_twolits++; }
	
	return true;
}

void
dimacs_loader::parse_all_ccls(row<long>& inst_ccls)
{
	parse_header();

	DBG_PRT(11, os << "ld_cursor=" << ld_cursor);

	DIMACS_CK(ld_nud_added_ccls == 0);
	DIMACS_CK(ld_nud_added_vars == 0);
	DIMACS_CK(ld_nud_added_lits == 0);
	DIMACS_CK(ld_nud_added_twolits == 0);

	ld_dots.init_nid_bits(ld_decl_vars + 1);

	inst_ccls.clear();

	DIMACS_CK(ld_lits1.is_empty());
	while(parse_clause(ld_lits1)){
		bool go_on = fixappend_lits(ld_lits1, inst_ccls);
		MARK_USED(go_on);
		DIMACS_CK(go_on);
		DIMACS_CK(ld_lits1.is_empty());
	}
}

void
dimacs_loader::finish_parse(row<long>& inst_ccls){
	verif_num_ccls(ld_file_name, ld_decl_ccls, ld_parsed_ccls);

	DIMACS_CK(ld_as_3cnf || (ld_nud_added_ccls == 0));
	DIMACS_CK(ld_as_3cnf || (ld_nud_added_vars == 0));
	DIMACS_CK(ld_as_3cnf || (ld_nud_added_lits == 0));
	DIMACS_CK(ld_as_3cnf || (ld_nud_added_twolits == 0));

	ld_num_ccls = ld_decl_ccls + ld_nud_added_ccls;
	ld_num_vars = ld_decl_vars + ld_nud_added_vars;
	ld_tot_lits = ld_parsed_lits + ld_nud_added_lits;
	ld_tot_twolits = ld_parsed_twolits + ld_nud_added_twolits;
}

void
dimacs_loader::parse_content(row<long>& inst_ccls){
	parse_all_ccls(inst_ccls);
	finish_parse(inst_ccls);
}

void
dimacs_loader::parse_file(ch_string& f_nam, row<long>& inst_ccls){
	load_file(f_nam);
	parse_content(inst_ccls);
}

void
dimacs_loader::lits_opps(row<long>& r_lits){
	for(long jj = 0; jj < r_lits.size(); jj++){
		long lit = r_lits[jj];
		r_lits[jj] = -lit;
	}
}

void
dimacs_loader::calc_f_lit_equal_and(long d_lit, row<long>& and_lits,
			row_row_long_t& rr_lits)
{
	DBG_PRT(42, os << "EQ_AND. d_lit=" << d_lit << " and_lits=" << and_lits);

	DIMACS_CK(! and_lits.is_empty());
	row<long>& f1 = rr_lits.inc_sz();

	and_lits.copy_to(f1);
	lits_opps(f1);
	f1.push(d_lit);

	DBG_PRT(42, os << "EQ_AND. ccl=" << f1);

	long ii = 0;
	for(ii = 0; ii < and_lits.size(); ii++){
		row<long>& f2 = rr_lits.inc_sz();
		long a_lit = and_lits[ii];

		f2.push(a_lit);
		f2.push(-d_lit);

		DBG_PRT(42, os << "EQ_AND. ccl(" << ii << ")=" << f2);
	}

	and_lits.clear();
}

