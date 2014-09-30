

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

kry.h

Base classes and abstract data types to code the system.

--------------------------------------------------------------*/

#ifndef KRY_H
#define KRY_H

#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include "bit_row.h"
#include "tools.h"
#include "secure_gets.h"
#include "tak_mak.h"

#define KRY_CK(prm)	   DBG_CK(prm)

#ifndef CARRIAGE_RETURN
#define CARRIAGE_RETURN		((char)13)
#endif

#define MIN_KEY_INIT_CHANGES 1000
#define MAX_KEY_INIT_CHANGES 2000

#define NUM_BYTES_SHA2	32	// 256 bits

#define fst_long(pt_dat) (*((long*)pt_dat))

#define pt_as(pt_dat, tp_dat) (*((tp_dat*)pt_dat))

#define wrt_val(stm, val) stm.write((char*)(&val), sizeof(val))
#define rd_val(stm, val) stm.read((char*)(&val), sizeof(val))

extern char* version_msg;


// inline text funcs

inline
bool
is_format_text(char cc){
	// \t \n \v \f \r
	//return ((cc >= 9) && (cc <= 13));
	return ((cc == 9) || (cc == 10) || (cc == 13));
}

inline
bool
is_print_text(char cc){
	// desde ' ' (32) hasta '~' (126)
	return ((cc >= 32) && (cc <= 126));
}

inline
bool
is_text(char cc){
	return (is_format_text(cc) || is_print_text(cc));
}

inline
t_1byte
rotate_byte(t_1byte bb, long val){
	return (bb + (t_1byte)val);
}

// kry_encryptor class

enum	kr_stage {
	st_invalid,
	st_swap,
	st_rot
};

class kry_encryptor {
private:
	std::ifstream			input_fstm;
	std::ofstream			output_fstm;

	kr_stage			stage;

	tak_mak				for_key;
	tak_mak				for_byte_swap;
	tak_mak				for_byte_rot;

	s_bit_row			key_bits;
	s_row<unsigned long>		key_longs;

	secure_row<long>		opers;

	t_1byte*			pt_file_data;
	long				file_data_sz;

	s_row<t_1byte>			target_bytes;

	void	add_sha(char* kry_data, unsigned char* sha_arr);
	bool	open_ifile(const char* in_nm, std::ifstream& in_stm);
	char*	read_file(std::istream& in_stm, long& data_sz,
			long head_sz = 0, long tail_sz = 0);

public:
	// for general use

	static
	std::string			kry_0_1_header;

	static
	std::string			kry_end_of_header;

	static
	std::string			kry_help;

	bool				with_sha;
	bool				encry;

	std::string			input_file_nm;
	secure_row<t_1byte>		key;

	std::string			output_file_nm;

	bool				prt_help;
	bool				prt_version;

	// for use as a lib

	bool				use_isstm;
	std::stringstream		data_isstm;

	bool				use_osstm;
	std::ostringstream		data_osstm;

	// all funcs

	static
	bool	is_all_text(char* chrs, long the_sz);

	static
	void	two_chars_to_byte(char* two_chrs, char* pt_byte);

	static
	long	hex_str_to_chars(char* chrs, long the_sz);

	kry_encryptor(){
		use_isstm = false;
		use_osstm = false;

		input_file_nm = "";
		output_file_nm = "";

		with_sha = true;
		encry = true;

		stage = st_invalid;

		pt_file_data = NULL_PT;
		file_data_sz = 0;

		prt_help = false;
		prt_version = false;
	}

	~kry_encryptor(){
		end_target();
	}

	void	end_target(){
		if(pt_file_data != NULL_PT){
			memset(pt_file_data, 0, file_data_sz);
			free(pt_file_data);
			pt_file_data = NULL_PT;
			file_data_sz = 0;

			target_bytes.clear();
		}
	}

	bool	has_target(){
		return (! target_bytes.is_empty());
	}

	bool	has_key(){
		return (! key_longs.is_empty());
	}

	void	shuffle_key();

	tak_mak&	get_tm(){
		tak_mak* tm = NULL_PT;
		switch(stage){
		case st_invalid:
			break;
		case st_swap:
			tm = &for_byte_swap;
			break;
		case st_rot:
			tm = &for_byte_rot;
			break;
		}
		KRY_CK(tm != NULL_PT);
		return *tm;
	}

	long	get_max_op(){
		long max_o = 0;
		switch(stage){
		case st_invalid:
			break;
		case st_swap:
			max_o = target_bytes.size();
			break;
		case st_rot:
			max_o = target_bytes.size();
			break;
		}
		return max_o;
	}

	long	gen_oper(tak_mak& tm_gen, long max_op);

	long	chr_row_to_long(row<char>& rr);

	void	fill_limits(secure_row<char>& tmp_str,
				long& num_1, long& num_2);

	void	add_key_section(s_row<char>& file_bytes, 
				secure_row<char>& tmp_key, 
				long num1, long num2);

	void	ask_key(secure_row<t_1byte>& the_key);
	void	get_key(secure_row<t_1byte>& the_key);

	void	init_key();

	void	init_tak_mak(tak_mak& tmak);
	void	init_tak_maks();
	void	init_target_encry();
	void	init_target_decry();

	void	init_opers(){
		tak_mak& tm_gen = get_tm();
		long max_op = get_max_op();

		opers.set_size(target_bytes.size());
		for(long aa = 0; aa < target_bytes.size(); aa++){
			opers[aa] = gen_oper(tm_gen, max_op);
		}
	}

	void	byte_swap(long oper){
		long v_op = opers[oper];
		target_bytes.swap(v_op, oper);
	}

	void	byte_rot(long oper){
		long v_op = opers[oper];
		if(! encry){
			v_op = -v_op;
		}
		target_bytes[oper] = rotate_byte(target_bytes[oper], v_op);
	}

	void	encry_swap(){
		KRY_CK(encry);
		KRY_CK(! target_bytes.is_empty());
		stage = st_swap;
		init_opers();
		for(long aa = 0; aa < opers.size(); aa++){
			byte_swap(aa);
		}
	}

	void	encry_rot(){
		KRY_CK(encry);
		KRY_CK(! target_bytes.is_empty());
		stage = st_rot;
		init_opers();
		for(long aa = 0; aa < opers.size(); aa++){
			byte_rot(aa);
		}
	}

	void	decry_swap(){
		KRY_CK(! encry);
		stage = st_swap;
		init_opers();
		for(long aa = (opers.size() - 1); aa >= 0; aa--){
			byte_swap(aa);
		}
	}

	void	decry_rot(){
		KRY_CK(! encry);
		stage = st_rot;
		init_opers();
		for(long aa = (opers.size() - 1); aa >= 0; aa--){
			byte_rot(aa);
		}
	}

	void	process_target(){
		if(! has_key() || ! has_target()){
			return;
		}
		if(encry){
			encry_swap();
			encry_rot();
		} else {
			decry_rot();
			decry_swap();
		}
	}

	void	init_output(){
		std::ostream& os = std::cout;

		if(! has_key() || ! has_target()){
			return;
		}
		if(use_osstm){
			return;
		}

		std::string& o_nm = output_file_nm;

		if(o_nm.size() == 0){
			std::string encry_ext = ".enkry";
			if(encry){
				o_nm = input_file_nm + encry_ext;
			} else {
				long i_sz = input_file_nm.size();
				//long pos_ext = i_sz - strlen(encry_ext);
				long pos_ext = i_sz - encry_ext.length();
				if(pos_ext > 0){
					o_nm = input_file_nm.substr(0, (i_sz - pos_ext));
				}
				o_nm = input_file_nm + ".dekry";
			}
		}

		output_fstm.open(o_nm.c_str(), std::ios::binary);
		if(! output_fstm.good()){
			os << "Archivo de salida " << o_nm <<
				" invalido." << std::endl;
		}
	}

	std::ostream&	get_output_stm(){
		if(use_osstm){
			return data_osstm;
		}
		return output_fstm;
	}

	void	write_output(){
		if(! has_key() || ! has_target()){
			return;
		}

		init_output();
		if(! get_output_stm().good()){
			return;
		}

		if(encry){
			write_encry_file();
		} else {
			write_decry_file();
		}
	}

	void	write_encry_file();
	void	write_decry_file();

	void	init_input(){
		std::ostream& os = std::cout;

		if(use_isstm || (input_file_nm.size() == 0)){
			return;
		}

		const char* f_nm = input_file_nm.c_str();  
		if(! open_ifile(f_nm, input_fstm)){
			os << "Could not open input file '" 
				<< input_file_nm << "'" << std::endl;
		}
	}

	std::istream&	get_input_stm(){
		if(use_isstm){
			return data_isstm;
		}
		return input_fstm;
	}

	bool	has_input(){
		return get_input_stm().good();
	}

	void	process_stm(){
		KRY_CK(sizeof(t_1byte) == sizeof(unsigned char));
		KRY_CK(sizeof(t_1byte) == sizeof(char));

		init_input();
		if(! encry){
			init_target_decry();
		}
		init_key();
		init_tak_maks();
		if(encry){
			init_target_encry();
		}
		process_target();
		write_output();
	}

	void	get_args(int argc, char** argv);
};

void	kry_encryptor_main(int argc, char** argv);


#endif  // KRY_H
