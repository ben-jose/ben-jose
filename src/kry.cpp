

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

kry.cpp  

kry encryptor functions. no-trace style encryptor.

--------------------------------------------------------------*/


#include <ctype.h>

#include "kry.h"
#include "sha2.h"

//DEFINE_MEM_STATS;

char* kry_header = NULL_PT;
long kry_header_sz = 0;

std::string
kry_encryptor::kry_0_1_header =
"kry-encryptor v0.1\n"
"(c) 2009. QUIROGA BELTRAN, Jose Luis. Bogota - Colombia.\n"
;

std::string
kry_encryptor::kry_help =
"kry <file_name> [-e|-d|-h|-v] [-r] [-o <ouput_file_name>] \n"
"\n"
"-e : encrypt the given <file_name>. (default option).\n"
"-d : decrypt the given <file_name>.\n"
"-h : show invocation info.\n"
"-v : show version info.\n"
"-r : raw process (for either -e|-d) the given <file_name>.\n"
"-o : set the name of the output file.\n"
"\n"
"See file 'kry_use.txt' in the source directory or\n"
"visit 'http://yosolosoy.com/esp/kry/'\n"
;

std::string
kry_encryptor::kry_end_of_header =
"\n<<<<.kry_encryptor_end_of_header_and_start_or_data.>>>>\n"
;

void
kry_encryptor::add_sha(char* kry_data, unsigned char* sha_arr){
	for(int ii = 0; ii < NUM_BYTES_SHA2; ++ii){
		sprintf(kry_data, "%02x", sha_arr[ii]);
	}
}

bool
kry_encryptor::is_all_text(char* chrs, long the_sz){
	bool all_txt = true;
	for(long ii = 0; ii < the_sz; ii++){
		if(! is_text(chrs[ii])){
			all_txt = false;
			break;
		}
	}
	return all_txt;
}

void
kry_encryptor::two_chars_to_byte(char* two_chrs, char* pt_byte){
	KRY_CK(is_text(*two_chrs));
	KRY_CK(is_text(*(two_chrs + 1)));

	char szBuf[] = "00";
	char* szStop;
	szBuf[0] = two_chrs[0];
	szBuf[1] = two_chrs[1];
	long vv = strtol(szBuf, &szStop, 16);

	pt_byte[0] = (char)vv;
}

long
kry_encryptor::hex_str_to_chars(char* chrs, long the_sz){
	long ii = 0;
	long jj = 0;
	for(ii = 0, jj = 0; ii < the_sz; ii += 2, jj++){
		two_chars_to_byte(chrs + ii, chrs + jj);
	}
	return jj;
}

bool
kry_encryptor::open_ifile(const char* in_nm, std::ifstream& in_stm){
	in_stm.open(in_nm, std::ios::binary);
	if(! in_stm.good()){
		return false;
	}
	return true;
}

char*
kry_encryptor::read_file(std::istream& in_stm, long& data_sz,
	long head_sz, long tail_sz)
{

	if(head_sz < 0){ return NULL_PT; }
	if(tail_sz < 0){ return NULL_PT; }

	long file_sz = 0;

	in_stm.seekg (0, std::ios::end);
	file_sz = in_stm.tellg();
	in_stm.seekg (0, std::ios::beg);

	data_sz = (t_dword)file_sz + head_sz + tail_sz;

	long mem_sz = (data_sz + 1) * sizeof(char);
	char* file_data = (char*)malloc(mem_sz);

	if(file_data == NULL_PT){ return NULL_PT; }

	KRY_CK(file_data != NULL);
	in_stm.read(file_data + head_sz, file_sz);
	long num_read = in_stm.gcount();

	if(num_read != file_sz){ 
		memset(file_data, 0, mem_sz);
		free(file_data);
		data_sz = 0;
		return NULL_PT; 
	}
	KRY_CK(num_read == file_sz);

	//in_stm.close();
	return file_data;
}

long	
kry_encryptor::gen_oper(tak_mak& tm_gen, long max_op){
	long op = 0;
	s_bit_row op_bits(&op);

	long idx1 = 0;
	long idx2 = 0;

	op = tm_gen.gen_rand_int32();
	idx1 = tm_gen.gen_rand_int32_ie(0, key_bits.size());
	idx2 = tm_gen.gen_rand_int32_ie(0, op_bits.size());

	op_bits[idx2] = key_bits[idx1];

	op = to_interval(op, 0, max_op);

	KRY_CK((op >= 0) && (op < max_op));
	return op;
}

long
kry_encryptor::chr_row_to_long(row<char>& rr){
	KRY_CK(! rr.is_empty());
	s_bit_row rr_bits;
	rr.init_s_bit_row(rr_bits);

	long ll = 0;
	s_bit_row ll_bits(&ll);
	for(long aa = 0; aa < ll_bits.size(); aa++){
		long pos_rr = for_key.gen_rand_int32_ie(0, rr_bits.size());
		ll_bits[aa] = rr_bits[pos_rr];
	}
	return ll;
}

void
kry_encryptor::fill_limits(secure_row<char>& tmp_str,
			long& num_1, long& num_2)
{
	long val_str = 0;

	tmp_str.push(0);
	val_str = atol(tmp_str.get_data());
	tmp_str.pop();

	if((val_str != 0) && (num_1 < 0)){
		num_1 = val_str;
	} else 
	if((val_str != 0) && (num_2 < 0)){
		num_2 = val_str;
	} else 
	if(	(tmp_str.size() == 1) && 
		(tmp_str[0] == '0'))
	{
		if(num_1 < 0){
			num_1 = 0;
		} else if(num_2 < 0){
			num_2 = 0;
		}
	} else 
	if((val_str == 0) && (num_1 < 0)){
		num_1 = chr_row_to_long(tmp_str);
	} else 
	if((val_str == 0) && (num_2 < 0)){
		num_2 = chr_row_to_long(tmp_str);
	}
}

void
kry_encryptor::add_key_section(s_row<char>& file_bytes,
			secure_row<char>& tmp_key, long num_1, long num_2)
{
	std::ostream& os = std::cout;

	long max_idx = file_bytes.size() + 1;
	long idx_1 = to_interval(num_1, 0, max_idx);
	long idx_2 = to_interval(num_2, 0, max_idx);

	if(idx_1 == idx_2){
		os << "kry cound not generate a valid section with"
			<< "those values. section ignored." << std::endl;
		return;
	}

	if(idx_1 > idx_2){
		file_bytes.append_to(tmp_key, idx_2, idx_1, true);
	} else {
		KRY_CK(idx_1 < idx_2);
		file_bytes.append_to(tmp_key, idx_1, idx_2); 
	}

	//os << tmp_key << std::endl;
}

void
kry_encryptor::ask_key(secure_row<t_1byte>& the_key){
	std::ostream& os = std::cout;

	secure_row<char>	tmp_key;
	secure_row<char>	tmp_str;
	s_row<char>		file_bytes;

	secure_gets(tmp_str);
	os << " * " << std::endl;

	long key_sz = 0;
	char* key_data = NULL_PT;
	std::ifstream in_stm;

	tmp_str.push(0);
	const char* key_f_nm = tmp_str.get_data();
	if(strcmp(key_f_nm, input_file_nm.c_str()) != 0){
		if(open_ifile(tmp_str.get_data(), in_stm)){
			key_data = read_file(in_stm, key_sz); 
			file_bytes.init_data(key_data, key_sz);
			os << "USING KEY BASE FILE" << std::endl;
		} 
	} else {
		os << "key base file cannot be the target file." << std::endl;
		os << "file ignored. not used as key base file." << std::endl;
	}
	tmp_str.pop();

	if(! tmp_str.is_empty()){
		unsigned long* ini_arr = (unsigned long*)(tmp_str.get_data());
		long ini_arr_sz = (tmp_str.get_data_sz() / sizeof(long));

		for_key.init_with_array(ini_arr, ini_arr_sz);

		if(file_bytes.is_empty()){
			tmp_str.append_to(tmp_key);
		}
	}

	long num_1 = -1;
	long num_2 = -1;
	while(! tmp_str.is_empty()){
		tmp_str.clear();
		os << ">";
		secure_gets(tmp_str);
		os << " * " << std::endl;

		if(! tmp_str.is_empty()){
			if(file_bytes.is_empty()){
				tmp_str.append_to(tmp_key);
			} else {
				fill_limits(tmp_str, num_1, num_2);
				if((num_1 >= 0) && (num_2 >= 0)){
					add_key_section(file_bytes, tmp_key,
							num_1, num_2);
					num_1 = -1; 
					num_2 = -1;
				}
			}
		}
	}

	if(! file_bytes.is_empty()){
		file_bytes.erase_data();
		t_1byte* dat = (t_1byte*)(file_bytes.get_data());
		free(dat);
		file_bytes.clear();
	}

	tmp_key.move_to(the_key);

	os << std::endl;
}

void
kry_encryptor::get_key(secure_row<t_1byte>& the_key){
	secure_row<t_1byte> tmp_key;

	std::ostream& os = std::cout;
	os << "key:";
	ask_key(the_key);
	if(encry){
		os << "confirm key:";
		ask_key(tmp_key);

		if(! the_key.equal_to(tmp_key)){
			os << "key confirmation failed." << std::endl;

			the_key.clear(true, true);
			tmp_key.clear(true, true);
			return;
		}
	}
}

void
kry_encryptor::init_key(){
	std::ostream& os = std::cout;

	if(encry && ! has_input()){
		return;
	}
	if(! encry && ! has_target()){
		return;
	}

	if(key.is_empty()){
		get_key(key);
	}

	if(key.is_empty()){
		return;
	}
	KRY_CK(! key.is_empty());

	int min_sz = sizeof(unsigned long);
	if(key.size() < min_sz){
		os << "Minimum key size is " << min_sz << "." << std::endl;
		key.clear(true, true);
	}

	key.init_s_bit_row(key_bits);
	key_longs.init_data_with_s_bit_row(key_bits);
}

void
kry_encryptor::shuffle_key(){
	//for_byte_swap must have been inited
	long nn = for_byte_swap.gen_rand_int32_ie(
			MIN_KEY_INIT_CHANGES, MAX_KEY_INIT_CHANGES);

	for(long aa = 0; aa < nn; aa++){
		long idx1 = for_byte_swap.gen_rand_int32_ie(0, key_bits.size());
		long idx2 = for_byte_swap.gen_rand_int32_ie(0, key_bits.size());

		key_bits.swap(idx1, idx2);
	}
}

void
kry_encryptor::init_tak_mak(tak_mak& tmak){
	if(key_longs.size() == 1){
		tmak.init_with_long(key_longs[0]);
	} else {
		tmak.init_with_array(key_longs.get_c_array(), key_longs.get_c_array_sz());
	}
}

void
kry_encryptor::init_tak_maks(){
	std::ostream& os = std::cout;
	MARK_USED(os);
	if(! has_key()){
		return;
	}

	init_tak_mak(for_byte_swap);
	shuffle_key();
	init_tak_mak(for_byte_rot);
}

void
kry_encryptor::init_target_encry(){
	if(! has_input()){
		return;
	}

	if(! has_key()){
		return;
	}

	//KRY_CK(has_input());
	std::ostream& os = std::cout;

	long hd_sz = 0;
	long tl_sz = 0;

	if(with_sha){
		hd_sz = sizeof(long);
		tl_sz = NUM_BYTES_SHA2;
	}

	long kry_data_sz = 0;
	char* kry_data = NULL_PT;

	std::istream& in_stm = get_input_stm();

	kry_data = read_file(in_stm, kry_data_sz, hd_sz, tl_sz);
	pt_file_data = kry_data;
	file_data_sz = kry_data_sz;

	if(kry_data == NULL_PT){
		os << "Could not read file " << input_file_nm << std::endl;
		return;
	}

	if(with_sha){
		unsigned char* pt_dat = NULL_PT;
		unsigned char* pt_sha = NULL_PT;

		pt_dat = (unsigned char*)(kry_data + hd_sz);
		pt_sha = (unsigned char*)(kry_data + kry_data_sz - tl_sz);

		long orig_file_sz = kry_data_sz - hd_sz - tl_sz;

		pt_as(kry_data, long) = orig_file_sz;
		sha2(pt_dat, orig_file_sz, pt_sha, 0);
	}

	target_bytes.init_data((t_1byte*)kry_data, kry_data_sz);
}

void
kry_encryptor::init_target_decry(){
	if(! has_input()){
		return;
	}

	std::ostream& os = std::cout;

	unsigned char sha_arr[NUM_BYTES_SHA2];
	memset(sha_arr, 0, NUM_BYTES_SHA2);

	KRY_CK(pt_file_data == NULL_PT);
	KRY_CK(file_data_sz == 0);

	std::istream& in_stm = get_input_stm();

	pt_file_data = read_file(in_stm, file_data_sz); 

	if(pt_file_data == NULL_PT){
		os << "Could not read file " << input_file_nm << std::endl;
		return;
	}

	KRY_CK(kry_header != NULL_PT);
	KRY_CK(kry_header_sz != 0);

	if(file_data_sz > kry_header_sz){
		int cmp_hd = memcmp(pt_file_data, kry_header, kry_header_sz);
		if(with_sha && (cmp_hd != 0)){
			os << "File " << input_file_nm << " does not seem to be a " << std::endl;
			os << "-----------------------" << std::endl;
			os << kry_header << std::endl;
			os << "-----------------------" << std::endl;
			os << "file." << std::endl;
			os << std::endl;
			os << "If it is a raw encrypted file " 
				<< "(using the -r option), "
				<< " the -r option must be selected for " 
				<< "decryption too." << std::endl;
			os << std::endl;
			os << kry_help;
			end_target();
			return;
		}
		if(! with_sha && (cmp_hd == 0)){
			os << "File " << input_file_nm << " seems to be encrypted ";
			os << "without the -r (row) option." << std::endl;
			os << std::endl;
			os << "If you get a messy file try without it." 
				<< std::endl;
		}
	}

	long encry_hd_sz = 0; 
		
	if(with_sha){
		encry_hd_sz = kry_header_sz + sizeof(long);
	}

	if(file_data_sz < encry_hd_sz){
		os << "Not a kry formated file " << input_file_nm << std::endl;
		end_target();
		return;
	}

	long kry_data_sz = file_data_sz;
	unsigned char* kry_data =
		(unsigned char*)(pt_file_data + encry_hd_sz);

	if(with_sha){
		kry_data_sz = pt_as((pt_file_data + kry_header_sz), long);

		long orig_kry_data_sz = file_data_sz - encry_hd_sz - NUM_BYTES_SHA2;
		if((orig_kry_data_sz < 0) || (kry_data_sz != orig_kry_data_sz)){
			os << "Corrupt kry encrypted file " << input_file_nm << std::endl;
			end_target();
			return;
		}

		unsigned char* kry_sha = sha_arr;
		sha2(kry_data, kry_data_sz, kry_sha, 0);

		void* orig_kry_sha = kry_data + kry_data_sz;
		int cmp_val = memcmp(kry_sha, orig_kry_sha, NUM_BYTES_SHA2);
		if(cmp_val != 0){
			os << "Verification BEFORE decry failed with " 
				<< "kry encrypted file" << input_file_nm
				<< std::endl;
			os << "File is corrupted." << std::endl;
			end_target();
			return;
		}
	}

	target_bytes.init_data((t_1byte*)kry_data, kry_data_sz);
}

void	
kry_encryptor::write_encry_file()
{
	//std::ostream& os = std::cout;

	if(target_bytes.is_empty()){
		return;
	}

	std::ostream& out_stm = get_output_stm();
	if(! out_stm.good()){
		return;
	}

	t_dword pos = out_stm.tellp();
	MARK_USED(pos);
	KRY_CK(pos == 0);

	unsigned char* kry_data = (unsigned char*)target_bytes.get_data();
	long kry_data_sz = target_bytes.size();

	unsigned char sha_arr[NUM_BYTES_SHA2];
	unsigned char* kry_sha = sha_arr;
	if(with_sha){
		memset(sha_arr, 0, NUM_BYTES_SHA2);

		sha2((unsigned char*)kry_data, kry_data_sz, kry_sha, 0);

		out_stm.write(kry_header, kry_header_sz);
		wrt_val(out_stm, kry_data_sz);
	}

	out_stm.write((const char*)kry_data, kry_data_sz);

	if(with_sha){
		out_stm.write((const char*)kry_sha, NUM_BYTES_SHA2);
	}
	out_stm.flush();
	//out_stm.close();
}

void	
kry_encryptor::write_decry_file()
{
	std::ostream& os = std::cout;

	if(target_bytes.is_empty()){
		return;
	}

	std::ostream& out_stm = get_output_stm();
	if(! out_stm.good()){
		return;
	}

	t_dword pos = out_stm.tellp();
	MARK_USED(pos);
	KRY_CK(pos == 0);

	unsigned char sha_arr[NUM_BYTES_SHA2];
	memset(sha_arr, 0, NUM_BYTES_SHA2);

	long hd_sz = 0;
	long tl_sz = 0;
	long kry_data_sz = target_bytes.size();

	if(with_sha){
		hd_sz = sizeof(long);
		tl_sz = NUM_BYTES_SHA2;
		kry_data_sz = target_bytes.size() - hd_sz - tl_sz;

		if(kry_data_sz < 0){
			os << "File " << input_file_nm << 
				" found corrupted AFTER decry" << std::endl;
			return;
		}
	}

	unsigned char* kry_data = (unsigned char*)(target_bytes.get_data() + hd_sz);

	if(with_sha){
		unsigned char* kry_sha = sha_arr;
		sha2((unsigned char*)kry_data, kry_data_sz, kry_sha, 0);

		void* orig_kry_sha = kry_data + kry_data_sz;
		int cmp_val = memcmp(kry_sha, orig_kry_sha, NUM_BYTES_SHA2);
		if(cmp_val != 0){
			os << "Verification failed AFTER decry of "
				<< input_file_nm << std::endl;
			os << "Wrong key or corrupted file." << std::endl;
			return;
		}
	}

	out_stm.write((const char*)kry_data, kry_data_sz);
	out_stm.flush();
	//out_stm.close();
}

void	
kry_encryptor::get_args(int argc, char** argv)
{
	std::ostream& os = std::cout;
	MARK_USED(os);

	KRY_CK(kry_header == NULL_PT);
	KRY_CK(kry_header_sz == 0);

	kry_header = (char*)(kry_0_1_header.c_str());
	kry_header_sz = kry_0_1_header.size();

	for(long ii = 1; ii < argc; ii++){
		std::string the_arg = argv[ii];
		if(strcmp(argv[ii], "-h") == 0){
			prt_help = true;
		} else if(strcmp(argv[ii], "-v") == 0){
			prt_version = true;
		} else if(strcmp(argv[ii], "-r") == 0){
			with_sha = false;
		} else if(strcmp(argv[ii], "-d") == 0){
			encry = false;
		} else if((strcmp(argv[ii], "-k") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1; 
			ii++;

			int arg_sz = strlen(argv[kk_idx]);
			s_row<char> tmp_str(argv[kk_idx], arg_sz);

			tmp_str.copy_to(key);
			tmp_str.clear();
			//os << "key=" << key << std::endl;

		} else if((strcmp(argv[ii], "-o") == 0) && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			output_file_nm = argv[kk_idx];

		} else if(input_file_nm.size() == 0){
			input_file_nm = argv[ii]; 
		}
	}
}

void
kry_encryptor_main(int argc, char** argv){
	KRY_CK(sizeof(char) == 1);

	std::ostream& os = std::cout;
	kry_encryptor kry_engine;

	kry_engine.get_args(argc, argv);

	if(kry_engine.prt_help){
		os << kry_engine.kry_help << std::endl;
		return;
	}
	if(kry_engine.prt_version){
		os << kry_header << std::endl;
		return;
	}
	if(kry_engine.input_file_nm.size() == 0){
		os << kry_engine.kry_help << std::endl;
		os << kry_header << std::endl;
		return;
	}

	kry_engine.process_stm();
}

