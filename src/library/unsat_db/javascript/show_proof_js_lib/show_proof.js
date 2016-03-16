

var MAX_REPL_VAL = 10000;

var SKL_PARENT = "../..";
var SHOW_PROOF_HTML_PAGE = SKL_PARENT + '/SKELETON/CNF/Show_Proof.htm';

var CNF_JSN_DATA = null;
var CNF_CCLS_REPL_ARR = null;
var CNF_VARS_REPL_ARR = null;

var ul_templ = "<ul id={ul_id}></ul>";

var li_1_templ = "<li title=\"{lb_tit}\">{lb_txt}</li>\n";

var li_2_templ = `
	<li>
		<label title="{lb_tit}" for="{lb_id}" 
			oncontextmenu="got_rclick('{jsn_rel_pth}')" 
			onclick="{func_to_call}('{ul_id}', '{jsn_rel_pth}')">
			{lb_txt}
		</label>
		<input type="checkbox" id="{lb_id}" />
		<ul id="{ul_id}">
		</ul>
	</li>
`;

//var li_3_templ = "<li><a href='#{cla_id}'>{all_lits}</a></li>";
var li_3_templ = "<li><i onclick=\"location.href='#{cla_id}'\">{all_lits}</i></li>";

var li_4_templ = `
	<li>
		<label title="unsat-cnf-permutation-resulting-neuron" for="{lb_id}">
			{lb_txt}
		</label>
		<input type="checkbox" id="{lb_id}" />
		<ul>
			<li>
				{subst_table_html}
			</li>
			<li>
				<label title="{lb_tit}" 
					oncontextmenu="got_rclick('{jsn_rel_pth}')" 
					onclick="{func_to_call}('{ul_id}_proof', '{jsn_rel_pth}')">
					"Show Proof in new window"
				</label>
			</li>
		</ul>
	</li>
`;

var a_main_cla_templ = "<a title='{lb_tit}' name='{cla_id}'>[{all_lits}]</a>";

//var a_main_cla_ref = "<a href='#{cla_id}'>{all_lits}</a>";
var a_main_cla_ref = "<i onclick=\"location.href='#{cla_id}'\">{all_lits}</i>";

var a_lit_templ = "<i title='{lit_tit}'>{lit_val}</i>";
var a_big_lit_templ = "<i title='ZERO for subst'>({lit_val})</i>";

function got_rclick(the_pth){
	window.prompt("To copy to clipboard do: Ctrl+C, Enter", the_pth);
	return false;
}

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

function is_map(someVar){
	if((typeof someVar === "object") && (someVar !== null)){
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

function load_http_url(file_nm, callback, is_async) {
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
	//alert('Loading-file ' + file_nm);
	window.prompt("Loading-file", file_nm);
	
	var fl_data = null;
	load_http_url(file_nm, function(response) {
		// Parse JSON string into object
		if(response === undefined){
			alert('ERROR. 1_Cannot_load_file ' + file_nm);
		} else if(response == null){
			alert('ERROR. 2_Cannot_load_file ' + file_nm);
		} else {
			fl_data = JSON.parse(response);
		}
	}, false);
	if(fl_data === undefined){
		fl_data = null;
	}
	return fl_data;
}

/*function load_json_file(file_nm) {
	var f_data = load_sync_url(file_nm);
	if(the_json_data != null){
		the_json_data = JSON.parse(f_data);
	}
	return the_json_data;
}*/

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

function is_map_ok(map1){
	if(map1 === undefined){
		return false;
	}
	if(map1 == null){
		return false;
	}
	if(! is_map(map1)){
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

function is_skl_path(pth){
	if(pth === undefined){
		return false;
	}
	if(pth == null){
		return false;
	}
	var beg = pth.substring(0, 9);
	if(beg != '/SKELETON'){
		return false;
	}
	return true;
}

function populate_main_ul_with_txt_area(){
	if(CNF_JSN_DATA == null){
		var jsn_file_to_load = null;
		
		var jsn_txt_area = document.getElementById('main_jsn_file');
		//var jsn_pth = jsn_txt_area.value.trim();
		jsn_file_to_load = jsn_txt_area.value.trim();
		var beg = jsn_file_to_load.substring(0, 9);
		if(! is_skl_path(jsn_file_to_load)){
			alert('Path must start with "/SKELETON"');
			return;
		}
		populate_main_ul(jsn_file_to_load);
	}
}

function populate_main_ul(jsn_file_to_load){
	if(CNF_JSN_DATA == null){
		var jsn_pth = SKL_PARENT + jsn_file_to_load;
		
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
		
		var dir_pth = get_dir(jsn_pth);
		var bj_proof_pth = dir_pth + bj_proof_nm;
		
		//alert(bj_proof_pth);
		CNF_VARS_REPL_ARR = calc_repl_arr(CNF_JSN_DATA.vars_permutation);
		CNF_CCLS_REPL_ARR = calc_repl_arr(CNF_JSN_DATA.ccls_permutation);

		//alert('v_repl=' +  JSON.stringify(CNF_VARS_REPL_ARR, null, 2));

		var cnf_txt = document.createTextNode("DIMACS CNF");
		document.body.appendChild(cnf_txt);
		
		var cnf_tit = document.getElementById('main_cnf_title');
		cnf_tit.innerHTML = 'The CNF';
		
		var cnf_cont = document.getElementById('main_cnf_cont');
		var cnf_tb = create_main_cnf_table();
		cnf_cont.appendChild(cnf_tb);

		var main_lb = document.getElementById('proof_label');
		main_lb.innerHTML = 'The Proof';

		var ul_elem_id = 'main_proof_ul';
		populate_ul(ul_elem_id, bj_proof_pth);
	}
}

function create_main_cnf_table(){
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
			var the_lits_str = '[' + the_lits + ']';
			var s_lits = replace_vals(the_lits, CNF_VARS_REPL_ARR);
			if(! is_array(s_lits)){
				alert(s_lits);
				return;
			}
			var the_lits_str = a_main_cla_templ.supplant(
				{	cla_id: '' + ii, 
					all_lits: '' + s_lits, 
					lb_tit: ' orig_lits=' + the_lits_str
				}
			);
			
			td.innerHTML = the_lits_str;
			td.style.border = '1px solid black';
		}
	}
	return tbl;
}

function is_last_jsn(jsn_obj){
	var jsn_pth = jsn_obj.loaded_pth;
	if(jsn_pth === undefined){
		alert('ERROR. Undefined-jsn_obj.loaded_pth.');
		return true;
	}
	
	var the_chain = jsn_obj.chain;
	if(the_chain.length <= 0){
		alert('ERROR. Empty-chain-in-json-file ' + jsn_pth);
		return true;
	}
	
	var ch_stp = the_chain[0];
	var ne_ty = ch_stp.neu_type;
	
	if(ne_ty === undefined){
		alert('ERROR. neu_type-in-json-file ' + jsn_pth);
		return true;
	}
	
	var is_lst = (ne_ty != "full");
	return is_lst;
}

function load_all_json(jsn_pth, jsn_obj_arr){
	if(! is_array_ok(jsn_obj_arr)){
		alert('ERROR. Bad-array-in-load_all_json');
		return;
	}
	while(true){
		var jsn_obj = load_json_file(jsn_pth);
		if(jsn_obj == null){
			alert('ERROR. Cannot-parse-json-file ' + jsn_pth);
			return;
		}
		jsn_obj.loaded_pth = jsn_pth;
		jsn_obj_arr.push(jsn_obj);
		
		var the_chain = jsn_obj.chain;
		
		if(the_chain.length <= 0){
			alert('ERROR. Empty-chain-in-json-file ' + jsn_pth);
			return;
		}
		
		var ch_stp = the_chain[0];
		
		var ne_ty = ch_stp.neu_type;
		var is_trace_top = ((ne_ty === undefined) || (ne_ty != "full"));
		if(is_trace_top){
			break;
		}
		
		var ext_pth = ch_stp.neu_full_jsn_pth;
		if(ext_pth === undefined){
			alert('ERROR. Undefined-previous-path-dir-in-json-file ' + jsn_pth);
			return;
		}
		var dir_pth = SKL_PARENT + ext_pth + '/';

		var ne_jsn = ch_stp.neu_jsn_pth;
		if(ne_jsn === undefined){
			alert('ERROR. Undefined-previous-path-name-in-json-file ' + jsn_pth);
			return;
		}
		jsn_pth = dir_pth + ne_jsn;
	}
}

function populate_ul(ul_elem_id, jsn_pth){
	var the_ul = document.getElementById(ul_elem_id);
	var all_lis = the_ul.getElementsByTagName('li');
	if(all_lis.length != 0){
		return;
	}
	var all_chgs = [];
	var res_neu = null;
	var htm_str = [];
	
	var all_jsn = [];
	load_all_json(jsn_pth, all_jsn);
	
	var tot_sec = all_jsn.length;
	for (var jj = all_jsn.length - 1; jj >= 0; jj--){
		//var jsn_obj = load_json_file(jsn_pth);
		var jsn_obj = all_jsn[jj];
		if(jsn_obj === undefined){
			alert('ERROR. Undefined-json-object-in ' + jsn_pth);
			return;
		}
		if(jsn_obj == null){
			alert('ERROR. Null-json-object-in ' + jsn_pth);
			return;
		}
		
		var lded_pth = get_dir(jsn_obj.loaded_pth);
		var the_chain = jsn_obj.chain;

		var chn_sz = the_chain.length - 1;
		var num_sec = jj + 1;
		var sec_nm = 'Section ' + num_sec + ' size=' + chn_sz;
		var li_pth_elem = li_1_templ.supplant({lb_tit: lded_pth, lb_txt: sec_nm});
		htm_str.push('<hr>\n');
		htm_str.push(li_pth_elem);
		htm_str.push('<hr>\n');

		alert('3.CHAIN_LENGTH= ' + the_chain.length);
		
		for (var ii = 0; ii < the_chain.length; ii++){
			var fnm_to_call = 'populate_ul';
			var sub_htm_templ = li_2_templ;
			var subst_htm = 'THE_SUBST_HTML';
			var dir_pth = lded_pth;
			
			var ch_stp = the_chain[ii];
			var stp_str = JSON.stringify(ch_stp, null, 2);
			
			var ne_ty = ch_stp.neu_type;
			if(ne_ty === undefined){
				alert('ERROR. no-neuron-type-found-in ' + stp_str);
				return;
			}
			
			var ext_pth = null;
			if(ne_ty == "full"){
				//ext_pth = ch_stp.neu_full_jsn_pth;
				//dir_pth = SKL_PARENT + ext_pth + '/';
				//alert('DEBUG. changed-1-dir_pth-to \n' + dir_pth);
				continue;
			}
			
			var orig_lits = ch_stp.neu_lits;
			var s_lits = replace_vals(orig_lits, CNF_VARS_REPL_ARR);
			if(! is_array(s_lits)){
				alert(s_lits);
				return;
			}
			
			if(ne_ty == "subst"){
				alert('GOT-subst-neuron ' + stp_str);
				ext_pth = ch_stp.neu_full_jsn_pth;
				//dir_pth = SKL_PARENT + ext_pth + '/';
				dir_pth = ext_pth + '/';
				//alert('DEBUG. changed-2-dir_pth-to \n' + dir_pth);
				fnm_to_call = 'open_new_proof';
				sub_htm_templ = li_4_templ;
				subst_htm = get_html_for_subst(ch_stp, s_lits);
			}

			var cla_id_str = '' + get_repl_val(ch_stp.neu_idx, CNF_CCLS_REPL_ARR);

			var the_lits_str = "[" + s_lits + "]";
			
			var ne_jsn = ch_stp.neu_jsn_pth;
			if(ne_jsn === undefined){
				var li_elem_2 = li_3_templ.supplant(
					{all_lits: the_lits_str, cla_id: cla_id_str});
				
				htm_str.push(li_elem_2);
			} else {
				var full_pth = dir_pth + ne_jsn;
				var tit_str = ne_jsn;
				if(ext_pth != null){
					tit_str = full_pth;
				}
				
				var sub_ul_id = ul_elem_id + '_r' + jj + '_' + ii;
				var sub_lb_id = 'lb_' + sub_ul_id;
				var sub_lb_txt = the_lits_str;
				
				tit_str = '(' + sub_ul_id + ') ' + tit_str;
				
				var repl = {
					func_to_call: fnm_to_call,
					subst_table_html: subst_htm,
					lb_tit: tit_str,
					lb_id: sub_lb_id,
					ul_id: sub_ul_id,
					jsn_rel_pth: full_pth,
					lb_txt: sub_lb_txt
				};
				//var li_elem_2 = li_2_templ.supplant(repl);
				var li_elem_2 = sub_htm_templ.supplant(repl);
				htm_str.push(li_elem_2);
				
				//alert('DEBUG. added-item \n' + li_elem_2);
				
			}
			
			//var res_var = ch_stp.va_r;
			var res_var = get_repl_val(ch_stp.va_r, CNF_VARS_REPL_ARR);
			
			if((res_neu != null) && (res_var != 0)){
				var stp_tit = 'Step ' + ii;
				var va_r_str = "RES " + res_var;
				var li_elem_3 = li_1_templ.supplant({lb_tit: stp_tit, lb_txt: va_r_str});
				htm_str.push(li_elem_3);
				
				htm_str.push('<hr>\n');
				
				res_neu = calc_resolution(res_neu, all_chgs, 
											s_lits, res_var);
				var cl_res_str = "[" + res_neu + "]";
				var li_elem_res = li_1_templ.supplant({lb_tit: stp_tit, lb_txt: cl_res_str});
				htm_str.push(li_elem_res);
			} else {
				res_neu = s_lits; 
			}
		}
	}
		
	var htm_full_str = htm_str.join("");
	//alert('Loaded_ok');
	the_ul.innerHTML = htm_full_str;
}

function open_new_proof(ul_elem_id, jsn_pth){
	localStorage.setItem(ul_elem_id, jsn_pth);
	var ulr = SHOW_PROOF_HTML_PAGE + '?pth_var=' + ul_elem_id;
	// window.location.href
	window.open(ulr);
}

function populate_main_ul_with_param(){
	var jsn_file_to_load = null;
	
	var pth_param = get_URL_parameter('pth_var');
	if(pth_param != null){
		jsn_file_to_load = localStorage.getItem(pth_param);
		if(! is_skl_path(jsn_file_to_load)){
			alert('BAD-path-in-parameter ' + jsn_file_to_load);
			jsn_file_to_load = null;
		}
		if(jsn_file_to_load != null){
			alert("GOT-path-to-load=\n'" + jsn_file_to_load + "'");
			document.getElementById('main_jsn_file').value = jsn_file_to_load;
		}
		populate_main_ul(jsn_file_to_load);
	}
}

//var uri_enc = encodeURIComponent(uri);
//var uri_dec = decodeURIComponent(uri_enc);

function create_tab_tables(tab1, tab2){
	var s_stl = '1px solid black';
	var tbl  = document.createElement('table');
	tbl.style.border = s_stl;
	var tr0 = tbl.insertRow();
	var td0_1 = tr0.insertCell();
	td0_1.innerHTML = 'LOCAL_SUB_CNF';
	var td0_2 = tr0.insertCell();
	td0_2.innerHTML = 'PROOVED_CNF';
	
	var tr = tbl.insertRow();
	var td1 = tr.insertCell();
	td1.appendChild(tab1);
	td1.style.border = s_stl;
	var td2 = tr.insertCell();
	td2.appendChild(tab2);
	td2.style.border = s_stl;
	
	return tbl;
}

function create_subst_tab(subst_ch_stp, vars_repl_arr, bi_repl_arr, subst_zeros){
	var all_ccls = subst_ch_stp.neuromap_ccls;
	var all_ccls_idx = subst_ch_stp.ccls_permutation;
	
	if(all_ccls.length != all_ccls_idx.length){
		alert('ERROR. Internal-error-diferent-sizes-in-subst-ccl-data');
		return;
	}
	
	var tbl  = document.createElement('table');
	//tbl.style.width  = '100px';
	tbl.style.border = '1px solid black';

	var num_rr = all_ccls.length;
	var num_cl = 1;
	for(var ii = 0; ii < num_rr; ii++){
		var tr = tbl.insertRow();
		for(var jj = 0; jj < num_cl; jj++){
			var td = tr.insertCell();
			
			var the_lits = all_ccls[ii];
			var s_lits = replace_vals(the_lits, vars_repl_arr);
			if(! is_array(s_lits)){
				alert(s_lits);
				return;
			}
			
			var the_idx_rel = all_ccls_idx[ii];
			if(the_idx_rel.length != 2){
				alert('ERROR. Internal-error-bad-idx-rel.');
				return;
			}
			var ccl_idx = the_idx_rel[0];
			
			//var s_lits_str = '[' + s_lits + ']';
			var s_lits_str = get_titled_lits_str(s_lits, bi_repl_arr, subst_zeros);
			
			var cla_id_str = '' + get_repl_val(ccl_idx, CNF_CCLS_REPL_ARR);
			var the_lits_str = a_main_cla_ref.supplant(
				{all_lits: s_lits_str, cla_id: cla_id_str});
			
			td.innerHTML = the_lits_str;
			td.style.border = '1px solid black';
		}
	}
	return tbl;
}

function calc_subst_perm_arr(perm_arr, in_repl_arr, bi_lft_repl_arr, bi_rgt_repl_arr){
	if(! is_array_ok(perm_arr)){
		alert('ERROR. calc-subst_perm_arr-perm_arr-is-not-array.');
		return null;
	}
	if(! is_array_ok(in_repl_arr)){
		alert('ERROR. calc-subst_perm_arr-in_repl_arr-is-not-array.');
		return null;
	}
	var s_pm__arr = [];
	for (aa = 0; aa < perm_arr.length; aa++) {
		perm_pair = perm_arr[aa];
		if(! is_array_ok(perm_pair)){
			alert('ERROR. calc-subst_perm_arr-perm_pair-is-not-array.');
			return null;
		}
		if(perm_pair.length != 2){
			alert('ERROR. calc-subst_perm_arr-perm_pair-is-not-pair.');
			return null;
		}
		var vv0 = perm_pair[0];
		if(isNaN(vv0)){
			continue;
		}
		var vv1 = perm_pair[1];
		if(isNaN(vv1)){
			continue;
		}
		var pm0 = get_repl_val(vv0, in_repl_arr);
		var pm1 = vv1;
		
		var pp0 = pm0;
		var pp1 = pm1;
		if(pp0 < 0){
			pp0 = -pp0;
			pp1 = -pp1;
		}
		if(pp0 > MAX_REPL_VAL){
			alert('ERROR. replace-value-bigger-than-MAX_REPL_VAL(' + MAX_REPL_VAL + ')');
			return null;
		}
		var s_pm_pair = [pp0, pp1];
		s_pm__arr.push(s_pm_pair);
		
		if(pm1 < 0){
			pm0 = -pm0;
			pm1 = -pm1;
		}
		if(pm1 > MAX_REPL_VAL){
			alert('ERROR. replace-value-bigger-than-MAX_REPL_VAL(' + MAX_REPL_VAL + ')');
			return null;
		}
		
		if(is_array_ok(bi_lft_repl_arr)){
			bi_lft_repl_arr[pp0] = pp1;
		}
		if(is_array_ok(bi_rgt_repl_arr)){
			bi_rgt_repl_arr[pm1] = pm0;
		}
		
	}
	return s_pm__arr;
}

function get_html_for_subst(subst_ch_stp, zeros_s_lits){
	//return 'CALCULATED_HTML_FOR_SUBST';
	
	var vars_perm = subst_ch_stp.vars_permutation;
	var ccls_perm = subst_ch_stp.ccls_permutation;
	
	var all_zeros = zeros_s_lits.slice();
	var all_subst_zeros = all_zeros.map(Math.abs);
	
	var subst_zeros = calc_ccl_map(zeros_s_lits);

	var bi_lft_repl_arr = [];
	var bi_rgt_repl_arr = [];
	var s_pm_arr = calc_subst_perm_arr(vars_perm, CNF_VARS_REPL_ARR, 
									   bi_lft_repl_arr, bi_rgt_repl_arr);
	
	var subst_vars_repl_arr = calc_repl_arr(vars_perm);
	var subst_ccls_repl_arr = calc_repl_arr(ccls_perm);
	
	var htm_str = []
	
	var zeros_str = get_titled_lits_str(all_subst_zeros, null, subst_zeros);
	htm_str.push('SUBST_ZEROS<br>');
	htm_str.push(zeros_str);
	htm_str.push('<hr>\n');
	
	
	var subst_tbl_1 = create_subst_tab(subst_ch_stp, CNF_VARS_REPL_ARR, 
									   bi_lft_repl_arr, subst_zeros);
	var subst_tbl_2 = create_subst_tab(subst_ch_stp, subst_vars_repl_arr, 
									   bi_rgt_repl_arr);
	
	var tbls = create_tab_tables(subst_tbl_1, subst_tbl_2);
	var tbls_htm = tbls.outerHTML;
	htm_str.push(tbls_htm);
	htm_str.push('<hr>\n');

	sort_pairs(s_pm_arr);
	
	var ccls_str = JSON.stringify(s_pm_arr, null, 2);
	var c_per_htm_str = '<code>' + ccls_str + '</code>';
	
	htm_str.push('VARS_PERMUTATION<br>');
	htm_str.push(c_per_htm_str);
	htm_str.push('<hr>\n');
	
	//var ccls_str_2 = JSON.stringify(CNF_VARS_REPL_ARR, null, 2);
	//var c_per_htm_str_2 = '<code>' + ccls_str_2 + '</code>';
	
	//htm_str.push(c_per_htm_str_2);
	
	var htm_full_str = htm_str.join("");
	
	return htm_full_str;
}

function get_titled_lits_str(s_lits, bi_repl_arr, subst_zeros){
	var with_tt = is_array_ok(bi_repl_arr);
	var with_zz = is_map_ok(subst_zeros);
	var all_lits_str = [];
	all_lits_str.push('[');
	
	var l_sz = s_lits.length;
	for (aa = 0; aa < l_sz; aa++) {
		if(aa > 0){
			all_lits_str.push(', ');
		}
		var ll = s_lits[aa];
		var tt = 'click to go to orig clause';
		if(with_tt){
			tt = get_repl_val(ll, bi_repl_arr);
		}
		var is_zz = false;
		if(with_zz){
			var vv = Math.abs(ll);
			var vv_str = ''+vv;
			if(vv_str in subst_zeros){
				is_zz = true;
			}
		}
		var the_lit_str = 'error-with-lit';
		if(is_zz){
			the_lit_str = a_big_lit_templ.supplant({lit_val: '' + ll});
		} else {
			the_lit_str = a_lit_templ.supplant(
				{	lit_val: '' + ll, 
					lit_tit: ' corresponds to lit "' + tt + '"'
				}
			);
		}
		
		all_lits_str.push(the_lit_str);
	}
	all_lits_str.push(']');
	
	var full_lits_str = all_lits_str.join("");
	return full_lits_str;
}

function calc_ccl_map(all_lits){
	if(! is_array_ok(all_lits)){
		alert('ERROR. all_lits-is-not-array.');
		return null;
	}
	var ccl_map = {};
	for (aa = 0; aa < all_lits.length; aa++) {
		the_lit = all_lits[aa];
		the_var = Math.abs(the_lit);
		ccl_map[''+the_var] = "in";
	}
	return ccl_map;
}

