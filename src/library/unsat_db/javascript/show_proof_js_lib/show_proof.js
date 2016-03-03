

var MAX_REPL_VAL = 10000;

var SKL_PARENT = "../..";

var CNF_JSN_DATA = null;
var CNF_CCLS_REPL_ARR = null;
var CNF_VARS_REPL_ARR = null;

var ul_templ = "<ul id={ul_id}></ul>";

var li_1_templ = "<li>{lb_txt}</li>\n";

var li_2_templ = `
	<li>
		<label title="{lb_tit}" for="{lb_id}" 
			onclick="populate_ul('{ul_id}', '{jsn_rel_pth}')">
			{lb_txt}
		</label>
		<input type="checkbox" id="{lb_id}" />
		<ul id="{ul_id}">
		</ul>
	</li>
`;

var a_lit_templ = "<a class='{vr_id}'>{lit}</a>";
var a_ref_main_cla_templ = "<li><a href='#{cla_id}'>{all_lits}</a></li>";
var a_main_cla_templ = "<a name='{cla_id}'>[{all_lits}]</a>";

function get_dir(full_pth){
	var dir_pth = full_pth.substring(0, full_pth.lastIndexOf("/")+1);
	return dir_pth;
}

function get_URL_parameter(name) {
	return decodeURIComponent(
		(new RegExp('[?|&]' + name + '=' + 
			'([^&;]+?)(&|#|;|$)').exec(location.search) ||
			[,""])[1].replace(/\+/g, '%20'))||null;
}

function get_repl_val(val_in, repl_arr){
	if(repl_arr === undefined){
		return val_in;
	}
	if(repl_arr == null){
		return val_in;
	}
	idx_repl = Math.abs(val_in);
	val_out = repl_arr[idx_repl];
	if(val_out == null){
		return 0;
	}
	if(val_out === undefined){
		alert('ERR1. val_in' + val_in + '\n idx_repl=' + idx_repl + 
				'\n repl_arr=' + repl_arr);
		return "ERROR-in-cnf-proof. Undefined-replace-for " + idx_repl;
	}
	if(isNaN(val_in)){
		alert('ERR2. val_in' + val_in + '\n idx_repl=' + idx_repl + 
				'\n repl_arr=' + repl_arr + '\n val_out=' + val_out);
		return "ERROR-in-cnf-proof. Not-a-number-replace-for " + idx_repl;
	}
	if(val_in < 0){
		val_out = -val_out;
	}
	return val_out;
}

function replace_vals(orig_arr, repl_arr, out_pairs){
	if(repl_arr === undefined){
		if(is_array(orig_arr)){
			sort_nums(orig_arr);
		}
		return orig_arr;
	}
	if(repl_arr == null){
		if(is_array(orig_arr)){
			sort_nums(orig_arr);
		}
		return orig_arr;
	}
	var out_arr = [];
	var err_str = null;
	for (aa = 0; aa < orig_arr.length; aa++) {
		val_in = orig_arr[aa];
		val_out = get_repl_val(val_in, repl_arr);
		if(isNaN(val_out)){
			err_str = val_out;
			break;
		}
		if(val_out != 0){
			out_arr.push(val_out);
			if(! (out_pairs === undefined)){
				var id_repl = Math.abs(val_in);
				var pp = [val_out, id_repl];
				out_pairs.push(pp);
			}
		}
	}
	if(err_str != null){
		if(! (out_pairs === undefined)){
			out_pairs = [];
		}
		return err_str;
	}
	if(is_array(out_arr)){
		sort_nums(out_arr);
	}
	return out_arr;
}   

function is_array(someVar){
	if( Object.prototype.toString.call( someVar ) === '[object Array]' ) {
		return true;
	}
	return false;
}

function set_elem_html(elem_id, htm_str){
	if(htm_str === undefined){
		htm_str = "undef";
	} else 
	if(htm_str == null){
		htm_str = "null";
	} 
	var fi1 = document.getElementById(elem_id);
	fi1.innerHTML = htm_str;
}

function sort_nums(arr_nums){
	arr_nums.sort(function(aa, bb){return aa-bb});
}

function sort_pairs(arr_pairs){
	arr_pairs.sort(function(pp1, pp2){return pp1[0]-pp2[0]});
}

function set_neu_str(elem_id, neu_arr, repl_arr){
	rr = replace_vals(neu_arr, repl_arr);
	var fi1 = document.getElementById(elem_id);
	fi1.innerHTML = "[" + rr + "]";
}   

function set_literal_str(elem_id, lit_val, repl_arr){
	if(repl_arr === undefined){
		out_val = lit_val;
	} else 
	if(repl_arr == null){
		out_val = lit_val;
	} else {
		out_val = get_repl_val(lit_val, repl_arr);
	}
	var fi1 = document.getElementById(elem_id);
	fi1.innerHTML = out_val;
}   

function check_url(url)
{
    var http = new XMLHttpRequest();
    http.open('HEAD', url, false);
    http.send();
    return http.status!=404;
}

function load_JSON_func(file_nm, callback, is_async) {
	var xobj = new XMLHttpRequest();
	
	var result = null;
	/*xobj.onerror = function(data) { 
		result = null;
		//alert('ERROR. Cannot_load_file ' + file_nm);
    };*/
	
	xobj.overrideMimeType("application/json");
	xobj.open('GET', file_nm, is_async); 
	xobj.onreadystatechange = function () {
		if (xobj.readyState == 4 && xobj.status == "200") {
			// Required use of an anonymous callback 
			// as .open will NOT return a value but simply 
			// returns undefined in asynchronous mode
			callback(xobj.responseText);
			//result = xobj.responseText;
			//setTimeout(function() { callback(result); }, 100);
		}
	};

	xobj.send(null);  
}

function load_json_file(file_nm) {
	var the_json_data = null;
	load_JSON_func(file_nm, function(response) {
		// Parse JSON string into object
		if(response === undefined){
			alert('ERROR. 1_Cannot_load_file ' + file_nm);
		} else if(response == null){
			alert('ERROR. 2_Cannot_load_file ' + file_nm);
		} else {
			the_json_data = JSON.parse(response);
		}
	}, false);
	if(the_json_data === undefined){
		the_json_data = null;
	}
	return the_json_data;
}

String.prototype.supplant = function (oo) {
	return this.replace(/{([^{}]*)}/g,
		function (aa, bb) {
			var rr = oo[bb];
			return typeof rr === 'string' || typeof rr === 'number' ? rr : aa;
		}
	);
};

function get_arr_from(pairs){
	var out_arr = [];
	for (var ii = 0; ii < pairs.length; ii++){
		var pp = pairs[ii];
		var pp_idx = pp[0];
		out_arr[pp_idx] = pp[1];
	}
	return out_arr;
}

function half_resolution(neu, all_chgs, va_r, out_arr){
	//alert("neu=" + neu + "\n all_chgs=" + all_chgs + "\n va_r=" + va_r);
	for (var ii = 0; ii < neu.length; ii++){
		var qua = neu[ii];
		var vv = Math.abs(qua);
		var add_it = ((all_chgs == null) || (all_chgs[vv] === undefined));
		if(add_it && (vv != va_r)){
			out_arr.push(qua);
			if(all_chgs != null){
				var chg = 1;
				if(qua < 0){ chg = -1; }
				all_chgs[vv] = chg;
			}
		}
	}
	return out_arr;
}

function calc_resolution(neu1, all_chgs, neu2, va_r){
	if(all_chgs == null){
		alert("ERROR. NULL-all_chgs");
	}
	if(all_chgs === undefined){
		alert("ERROR. UNDEF-all_chgs");
	}
	var va_r = Math.abs(va_r);
	var all_chgs1 = null;
	if(all_chgs.length == 0){ all_chgs1 = all_chgs; }
	var all_chgs2 = all_chgs;
	
	var out_arr = [];
	half_resolution(neu1, all_chgs1, va_r, out_arr);
	half_resolution(neu2, all_chgs2, va_r, out_arr);
	all_chgs[va_r] = undefined;
	
	sort_nums(out_arr);
	
	return out_arr;
}

function populate_main_ul(ul_id, jsn_pth_id){
	var jsn_txt_area = document.getElementById(jsn_pth_id);
	var jsn_pth = jsn_txt_area.value.trim();
	populate_ul(ul_id, jsn_pth);
}

function populate_ul(ul_id, jsn_pth){
	var the_ul = document.getElementById(ul_id);
	var all_lis = the_ul.getElementsByTagName('li');
	if(all_lis.length == 0){
		
		var dir_pth = get_dir(jsn_pth);
		alert('Loading-file ' + jsn_pth + '\n in dir=' + dir_pth);
		
		//document.location = dir_pth;
		
		var jsn_obj = load_json_file(jsn_pth);
		if(jsn_obj != null){
			var all_chgs = [];
			var res_neu = null;
			var the_chain = jsn_obj.chain;
			var htm_str = [];

			alert('CHAIN_LENGTH= ' + the_chain.length);
			
			for (var ii = 0; ii < the_chain.length; ii++){
				var ch_stp = the_chain[ii];
				
				var s_lits = replace_vals(ch_stp.neu_lits, CNF_VARS_REPL_ARR);
				if(! is_array(s_lits)){
					alert(s_lits);
					return;
				}
				
				var cla_id_str = '' + get_repl_val(ch_stp.neu_idx, CNF_CCLS_REPL_ARR);
	
				var the_lits_str = "[" + s_lits + "]";
				var ne_ty = ch_stp.neu_type;
				var ext_pth = null;
				if((! (ne_ty === undefined)) && (ne_ty == "full")){
					the_lits_str += " F";
					ext_pth = ch_stp.neu_full_jsn_pth;
					dir_pth = SKL_PARENT + ext_pth + '/';
				}
				
				var ne_jsn = ch_stp.neu_jsn_pth;
				if(ne_jsn === undefined){
					//var li_elem_2 = li_1_templ.supplant({lb_txt: the_lits_str});
					var li_elem_2 = a_ref_main_cla_templ.supplant(
						{all_lits: the_lits_str, cla_id: cla_id_str});
					
					htm_str.push(li_elem_2);
				} else {
					var full_pth = dir_pth + ne_jsn;
					var tit_str = ne_jsn;
					if(ext_pth != null){
						tit_str = full_pth;
					}
					
					var sub_ul_id = ul_id + '_' + ii;
					var sub_lb_id = 'lb_' + sub_ul_id;
					var sub_lb_txt = the_lits_str;
					var repl = {
						lb_tit: tit_str,
						lb_id: sub_lb_id,
						ul_id: sub_ul_id,
						jsn_rel_pth: full_pth,
						lb_txt: sub_lb_txt
					};
					var li_elem_2 = li_2_templ.supplant(repl);
					htm_str.push(li_elem_2);
				}
				
				//var res_var = ch_stp.va_r;
				var res_var = get_repl_val(ch_stp.va_r, CNF_VARS_REPL_ARR);
				
				if((res_neu != null) && (res_var != 0)){
					var va_r_str = "RES " + res_var;
					var li_elem_3 = li_1_templ.supplant({lb_txt: va_r_str});
					htm_str.push(li_elem_3);
					
					htm_str.push('<hr>\n');
					
					res_neu = calc_resolution(res_neu, all_chgs, 
											  s_lits, res_var);
					var cl_res_str = "[" + res_neu + "]";
					var li_elem_res = li_1_templ.supplant({lb_txt: cl_res_str});
					htm_str.push(li_elem_res);
				} else {
					res_neu = s_lits; 
				}
			}
			htm_str = htm_str.join("");
			//alert(htm_str);
			the_ul.innerHTML = htm_str;
		} else {
			alert('ERROR. Cannot-parse-json-file ' + jsn_pth);
		}
	}
}

function is_array_ok(arr1){
	if(arr1 === undefined){
		return false;
	}
	if(arr1 == null){
		return false;
	}
	if(! is_array(arr1)){
		return false;
	}
	return true;
}

function calc_repl_arr(subst_arr){
	if(! is_array_ok(subst_arr)){
		alert('ERROR. subst_arr-is-not-array.');
		return null;
	}
	var repl_arr = [];
	for (aa = 0; aa < subst_arr.length; aa++) {
		repl_pair = subst_arr[aa];
		if(! is_array_ok(repl_pair)){
			alert('ERROR. subst_arr-is-not-array.');
			return null;
		}
		if(repl_pair.length != 2){
			alert('ERROR. subst_arr-is-not-pair.');
			return null;
		}
		var vv0 = repl_pair[0];
		if(isNaN(vv0)){
			continue;
		}
		var vv1 = repl_pair[1];
		if(isNaN(vv1)){
			continue;
		}
		if(vv0 < 0){
			alert('ERROR. invalid-negative-value-in-replace-pair [' + repl_pair + ']');
			return null;
		}
		if(vv0 > MAX_REPL_VAL){
			alert('ERROR. replace-value-bigger-than-MAX_REPL_VAL(' + MAX_REPL_VAL + ')');
			return null;
		}
		repl_arr[vv0] = vv1;
	}
	return repl_arr;
}

function populate_main_ul_2(ul_id, jsn_pth_id){
	if(CNF_JSN_DATA != null){
		//alert('To change the proof refresh (F5) the page.');
	}
	if(CNF_JSN_DATA == null){
		var jsn_txt_area = document.getElementById(jsn_pth_id);
		//var jsn_pth = jsn_txt_area.value.trim();
		var val_txt_area = jsn_txt_area.value.trim();
		var beg = val_txt_area.substring(0, 9);
		if(beg != '/SKELETON'){
			alert('Path must start with "/SKELETON"');
			return;
		}
		
		var jsn_pth = SKL_PARENT + val_txt_area;
		
		CNF_JSN_DATA = load_json_file(jsn_pth);
		if(CNF_JSN_DATA === undefined){
			alert('ERROR. Cannot-load-file ' + jsn_pth);
			return;
		}
		if(CNF_JSN_DATA == null){
			alert('ERROR. Cannot-load-file ' + jsn_pth);
			return;
		}
		
		var bj_proof_nm = CNF_JSN_DATA.neuromap_proof;
		if(bj_proof_nm === undefined){
			alert('ERROR. Cannot-find-neuromap_proof-in-cnf-json-data');
			return;
		}
		
		//alert('FLAG_2');
		
		var dir_pth = get_dir(jsn_pth);
		/*if(! dir_pth.startswith('/SKELETON')){
			alert('PATH_DOES_NOT_START_WITH_SKELETON !!!');
		}*/
		var bj_proof_pth = dir_pth + bj_proof_nm;
		
		alert(bj_proof_pth);
		//alert('v_perm=' +  JSON.stringify(CNF_JSN_DATA.vars_permutation, null, 2));

		CNF_VARS_REPL_ARR = calc_repl_arr(CNF_JSN_DATA.vars_permutation);
		CNF_CCLS_REPL_ARR = calc_repl_arr(CNF_JSN_DATA.ccls_permutation);

		//alert('v_repl=' +  JSON.stringify(CNF_VARS_REPL_ARR, null, 2));

		var cnf_txt = document.createTextNode("DIMACS CNF");
		document.body.appendChild(cnf_txt);
		
		var cnf_tit = document.getElementById('main_cnf_title');
		cnf_tit.innerHTML = 'The CNF';
		
		var cnf_cont = document.getElementById('main_cnf_cont');
		var cnf_tb = create_cnf_table();
		cnf_cont.appendChild(cnf_tb);

		var main_lb = document.getElementById('proof_label');
		main_lb.innerHTML = 'The Proof';
		
		populate_ul(ul_id, bj_proof_pth);
	}
}

function create_cnf_table(){
	var bj_cnf = CNF_JSN_DATA.neuromap_ccls;
		
	var tbl  = document.createElement('table');
	//tbl.style.width  = '100px';
	tbl.style.border = '1px solid black';

	var num_rr = bj_cnf.length;
	var num_cl = 1;
	for(var ii = 0; ii < num_rr; ii++){
		var tr = tbl.insertRow();
		for(var jj = 0; jj < num_cl; jj++){
			var td = tr.insertCell();
			
			var the_lits = bj_cnf[ii];
			var s_lits = replace_vals(the_lits, CNF_VARS_REPL_ARR);
			if(! is_array(s_lits)){
				alert(s_lits);
				return;
			}
			var the_lits_str = a_main_cla_templ.supplant(
				{cla_id: '' + ii, all_lits: '' + s_lits}
			);
			
			td.innerHTML = the_lits_str;
			td.style.border = '1px solid black';
		}
	}
	return tbl;
}

