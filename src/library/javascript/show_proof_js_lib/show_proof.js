

var ul_templ = "<ul id={ul_id}></ul>";

var li_1_templ = "<li>{lb_txt}</li>\n";

var li_2_templ = `
	<li>
		<label for="{lb_id}" onclick="populate_ul('{ul_id}', '{jsn_rel_pth}')">
			{lb_txt}
		</label>
		<input type="checkbox" id="{lb_id}" />
		<ul id="{ul_id}">
		</ul>
	</li>
`;

function get_URL_parameter(name) {
	return decodeURIComponent(
		(new RegExp('[?|&]' + name + '=' + 
			'([^&;]+?)(&|#|;|$)').exec(location.search) ||
			[,""])[1].replace(/\+/g, '%20'))||null;
}

function get_repl_val(val_in, repl_arr){
	idx_repl = Math.abs(val_in);
	val_out = repl_arr[idx_repl];
	if(val_out === undefined){
		return "ERROR in cnf proof. Undefined replace for " + idx_repl;
	}
	if(! (typeof val_out === 'number')){
		return "ERROR in cnf proof. Not a number replace for " + idx_repl;
	}
	if(val_in < 0){
		val_out = -val_out;
	}
	return val_out;
}

function replace_vals(orig_arr, repl_arr){
	if(repl_arr === undefined){
		return orig_arr;
	}
	if(repl_arr == null){
		return orig_arr;
	}
	var out_arr = [];
	var err_str = null;
	for (aa = 0; aa < orig_arr.length; aa++) {
		val_in = orig_arr[aa];
		val_out = get_repl_val(val_in, repl_arr);
		if(! (typeof val_out === 'number')){
			err_str = val_out;
			break;
		}
		if(val_out != 0){
			out_arr.push(val_out);
		}
	}
	if(err_str != null){
		return err_str;
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

function sort_nums(neu){
	neu.sort(function(a, b){return a-b});
}

function set_neu_str(elem_id, neu_arr, repl_arr){
	rr = replace_vals(neu_arr, repl_arr);
	if(is_array(rr)){
		sort_nums(rr);
	}
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

function load_JSON_func(file_nm, callback, is_async) {
	var xobj = new XMLHttpRequest();
	xobj.overrideMimeType("application/json");
	// Replace 'my_data' with the path to your file
	xobj.open('GET', file_nm, is_async); 
	xobj.onreadystatechange = function () {
		if (xobj.readyState == 4 && xobj.status == "200") {
			// Required use of an anonymous callback 
			// as .open will NOT return a value but simply 
			// returns undefined in asynchronous mode
			callback(xobj.responseText);
		}
	};
	xobj.send(null);  
}

function load_json_file(file_nm) {
	var the_json_data = null;
	load_JSON_func(file_nm, function(response) {
		// Parse JSON string into object
		the_json_data = JSON.parse(response);
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
		alert("ERROR. NULL all_chgs");
	}
	if(all_chgs === undefined){
		alert("ERROR. UNDEF all_chgs");
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

function populate_ul(ul_id, jsn_pth){
	var the_ul = document.getElementById(ul_id);
	var all_lis = the_ul.getElementsByTagName('li');
	if(all_lis.length == 0){
		var jsn_obj = load_json_file(jsn_pth);
		if(jsn_obj != null){
			var all_chgs = [];
			var res_neu = null;
			var the_chain = jsn_obj.chain;
			var htm_str = [];
			for (var ii = 0; ii < the_chain.length; ii++){
				var ch_stp = the_chain[ii];
	
				var neu_lits_str = "[" + ch_stp.neu_lits + "]";
				var neu_jsn = ch_stp.neu_jsn;
				if(neu_jsn === undefined){
					var li_elem_2 = li_1_templ.supplant({lb_txt: neu_lits_str});
					htm_str.push(li_elem_2);
				} else {
					var sub_ul_id = ul_id + '_' + ii;
					var sub_lb_id = 'lb_' + sub_ul_id;
					var repl = {
						lb_id: sub_lb_id,
						ul_id: sub_ul_id,
						jsn_rel_pth: neu_jsn,
						lb_txt: neu_lits_str
					};
					var li_elem_2 = li_2_templ.supplant(repl);
					htm_str.push(li_elem_2);
				}
				
				if((res_neu != null) && (ch_stp.va_r != 0)){
					var va_r_str = "RES " + ch_stp.va_r;
					var li_elem_3 = li_1_templ.supplant({lb_txt: va_r_str});
					htm_str.push(li_elem_3);
					
					htm_str.push('<hr>\n');
					
					res_neu = calc_resolution(res_neu, all_chgs, 
											  ch_stp.neu_lits, ch_stp.va_r);
					var cl_res_str = "[" + res_neu + "]";
					var li_elem_res = li_1_templ.supplant({lb_txt: cl_res_str});
					htm_str.push(li_elem_res);
				} else {
					res_neu = ch_stp.neu_lits; 
				}
			}
			htm_str = htm_str.join("");
			//alert(htm_str);
			the_ul.innerHTML = htm_str;
		} else {
			alert('Cannot parse json file ' + jsn_pth);
		}
	}
	//for (var index = 0; index < uls.length; index ++){
	//alert(li_2_templ);
	//alert('populating ' + ul_id + " curr_size=" + all_lis.length);
	//alert('populating ' + ul_id);
}

