
nod_w = 10;
dip_h = 10;
neu_h = 3;

col_pos = '#00CCFF';
col_neg = 'red';

ALL_CNF_GPHS = [];

ID_0 = null;

// orange | green | yellow | blue
// SeaShell
// ellipse | triangle | pentagon | hexagon | heptagon | octagon | star

white_nodes_style = [
	{
		selector: 'node',
		css: {
			'content': 'data(lbl)',
			//'color': 'aqua',
			'width': nod_w
		}
	}
	,{
		selector: 'edge',
		css: {
			'curve-style': 'haystack', // fast edges
			//'curve-style': 'bezier',
			'width': 2
		}
	}
	,{
		selector: 'node:selected',
		css: {
			'border-color': col_neg,
			'border-width': 2
		}
	}
	,{
		selector: 'edge:selected',
		css: {
			'line-color': 'yellow',
			'width': 5
		}
	}
	,{
		selector: 'node.neuron',
		css: {
			'shape':'rectangle',
			'height': neu_h,
			'background-color': 'white'
		}
	}
	,{
		selector: 'node.dipole',
		css: {
			'height': dip_h,
			'background-color': 'white'
		}
	}
	,{
		selector: 'node.clb1',
		css: { 
			'color': 'yellow'
		}
	}
	,{
		selector: 'node.clb2',
		css: { 
			'color': 'aqua'
		}
	}
	,{
		selector: 'node.cho',
		css: { 
			'background-color': 'aqua',
			'shape':'triangle'
		}
	}
	,{
		selector: 'node.lrn',
		css: {
			//'background-color': '#66FF66',
			'background-color': 'aqua',
			'shape':'star'
		}
	}
	,{
		selector: 'edge.in_pos',
		css: {
			// circle | tee | square | triangle
			'line-color': col_pos
		}
	}
	,{
		selector: 'edge.in_neg',
		css: {
			'line-style': 'dashed',
			'line-color': col_neg
		}
	}
];

black_nodes_style = [
	{
		//prt
		selector: 'node',
		css: {
			'content': 'data(lbl)',
			//'color': 'ForestGreen',
			'width': nod_w
		}
	}
	,{
		//prt
		selector: 'edge',
		css: {
			'curve-style': 'haystack', // fast edges
			//'curve-style': 'bezier',
			'width': 2
		}
	}
	,{
		//prt
		selector: 'node:selected',
		css: {
			'border-color': col_neg,
			'border-width': 2
		}
	}
	,{
		//prt
		selector: 'edge:selected',
		css: {
			'line-color': 'DarkBlue',
			'width': 5
		}
	}
	,{
		//prt
		selector: 'node.neuron',
		css: {
			'shape':'rectangle',
			'height': neu_h,
			'background-color': 'black'
		}
	}
	,{
		//prt
		selector: 'node.dipole',
		css: {
			'height': dip_h,
			'background-color': 'black'
		}
	}
	,{
		//prt
		selector: 'node.clb1',
		css: { 
			'color': 'DarkBlue'
		}
	}
	,{
		//prt
		selector: 'node.clb2',
		css: { 
			'color': 'ForestGreen'
		}
	}
	,{
		//prt
		selector: 'node.cho',
		css: { 
			'background-color': 'ForestGreen',
			'shape':'triangle'
		}
	}
	,{
		//prt
		selector: 'node.lrn',
		css: {
			//'background-color': '#66FF66',
			'background-color': 'ForestGreen',
			'shape':'star'
		}
	}
	,{
		//prt
		selector: 'edge.in_pos',
		css: {
			// circle | tee | square | triangle
			'line-color': col_pos
		}
	}
	,{
		//prt
		selector: 'edge.in_neg',
		css: {
			'line-style': 'dashed',
			'line-color': col_neg
		}
	}
];

function set_node_tier(gph, node, nd_tier) {
	if(nd_tier === undefined){
		nd_tier = null;
	}
	if(nd_tier != null){
		the_nd = gph.filter('node[id = "' + node + '"]');
		the_nd.data('lbl', nd_tier);
	}
}

function play_node(gph, node){

	q_tar = gph.elements('edge[target = "' + node + '"]');

	q_pos = q_tar.filter('.in_pos');
	q_neg = q_tar.filter('.in_neg');
	
	q_pos.removeClass('in_pos');
	q_pos.addClass('in_neg');
	q_neg.removeClass('in_neg');
	q_neg.addClass('in_pos');
}

function fn_handle_tap(event){
	if(event.cyTarget.isNode()){
		var nd_id = event.cyTarget.data("id");
		play_node(event.cy, nd_id);
	}
}

function get_data(gph, the_nd, dat_nm){
	var tmp1 = the_nd.data(dat_nm);
	if(tmp1 === undefined){
		tmp1 = '';
		the_nd.data(dat_nm, tmp1);
	}
	return tmp1;
}

function set_lbl2(gph, node_id){
	the_nd = gph.filter('node[id = "' + node_id + '"]');
	var tmp1 = get_data(gph, the_nd, 'lbl');
	var tmp2 = get_data(gph, the_nd, 'lbl2');
	
	the_nd.data('lbl', tmp2);
	the_nd.data('lbl2', tmp1);

	if(the_nd.hasClass('clb1')){
		the_nd.removeClass('clb1');
		the_nd.addClass('clb2');
	} else
	if(the_nd.hasClass('clb2')){
		the_nd.removeClass('clb2');
		the_nd.addClass('clb1');
	} else {
		the_nd.removeClass('clb1');
		the_nd.addClass('clb2');
	}
}

/*
function set_lbl2(gph, node_id){
	the_nd = gph.filter('node[id = "' + node_id + '"]');
	var tmp1 = the_nd.data('lbl');
	var tmp2 = the_nd.data('lbl2'); 
	if(tmp2 === undefined){
		the_nd.data('lbl', '');
	} else {
		the_nd.data('lbl', tmp2);
	}
	if(tmp1 === undefined){
		the_nd.data('lbl2', '');
	} else {
		the_nd.data('lbl2', tmp1);
	}
	
	if(the_nd.hasClass('clb1')){
		the_nd.removeClass('clb1');
		the_nd.addClass('clb2');
	} else
	if(the_nd.hasClass('clb2')){
		the_nd.removeClass('clb2');
		the_nd.addClass('clb1');
	} else {
		the_nd.removeClass('clb1');
		the_nd.addClass('clb2');
	}
}
*/

function fn_handle_mouseover(event){
	if(event.cyTarget.isNode()){
		var nd_id = event.cyTarget.data("id");
		set_lbl2(event.cy, nd_id);
	}
}

var tappedBefore = null;

function fn_handle_click(event){
	var tappedNow = event.cyTarget;
	setTimeout(function(){ tappedBefore = null; }, 300);
	if(tappedBefore === tappedNow) {
		tappedNow.trigger('doubleTap');
		tappedBefore = null;
	} else {
		tappedBefore = tappedNow;
	}
}

function fn_handle_double_click(event){
	event.cy.fit();
}

function show_cnf(grph_div_id, grph_elems, layout_nm, all_to_play, all_tiers){
	
	cnf_gph = cytoscape({
		container: document.getElementById(grph_div_id),

		style: white_nodes_style,

		elements: grph_elems,
		
		layout: {
			//name: 'preset'
			directed: true,
			name: layout_nm
		},
  
		ready: function(){
			//window.cy = this;
		}
	});
	
	//all_dipoles = cnf_gph.filter('node[name = "d*"]');
	//all_dipoles = cnf_gph.dipole;
	all_dipoles = cnf_gph.filter('node.dipole');
	all_dipoles.addClass('clb1');
	if((ID_0 == null) && (all_dipoles.length > 0)){
		ID_0 = all_dipoles[0].id();
		//alert('ID_0=' + ID_0);
		//DIPOLE_0 = all_dipoles[0];
	}
	
	cnf_gph.on('tap', fn_handle_click);
	cnf_gph.on('doubleTap', fn_handle_double_click);
	
	cnf_gph.on('tap', 'node', fn_handle_tap);
	cnf_gph.on('mouseover', 'node', fn_handle_mouseover);
	
	if(all_to_play === undefined){
		all_to_play = null;
	}
	if(all_to_play != null){
		for (aa = 0; aa < all_to_play.length; aa++) {
			play_node(cnf_gph, all_to_play[aa]);
		}
	}
	if(all_tiers === undefined){
		all_tiers = null;
	}
	if(all_tiers != null){
		for (aa = 0; aa < all_tiers.length; aa++) {
			var nd_id = 'd' + aa;
			set_node_tier(cnf_gph, nd_id, all_tiers[aa]);
		}
	}
	//var nd_id = 'd' + 1;
	//set_node_tier(cnf_gph, nd_id, 20);
	//play_node(cnf_gph, 'd2');
	
	var gph_pair = [cnf_gph, grph_div_id];
	ALL_CNF_GPHS.push(gph_pair);
}

function get_display(is_on){
	if(is_on === undefined){
		return 'block';
	}
	if(is_on){
		return 'block';
	}
	return 'none';
}

function set_png_for(num_gph, gph_on, img_on){
	var gph_pair = ALL_CNF_GPHS[num_gph];
	var cnf_gph = gph_pair[0];
	var grph_div_id = gph_pair[1];
	
	var grph_png_div_id = 'png_' + grph_div_id;
	
	var gph_div = document.getElementById(grph_div_id);
	var png_div = document.getElementById(grph_png_div_id);

	gph_div.style.display = get_display(gph_on);
	png_div.style.display = get_display(img_on);
	//gph_div.style.display = 'block';
	//png_div.style.display = 'block';
	
	var png64 = cnf_gph.png();

	var s_img = "style='border: 1px solid #ddd; min-width: 3em; min-height: 3em'";
	var full_img = 'Embedded PNG Image\n <img src="' + png64 + '" ' + s_img + '>';
	
	png_div.innerHTML = full_img;
	
}

function export_all_gphs(){
	for (aa = 0; aa < ALL_CNF_GPHS.length; aa++) {
		set_png_for(aa, false, true);
	}
}

function change_theme(){
	var theme_sel = document.getElementById('gph_theme').value;
	var is_blk = (theme_sel == 'black_nodes');
	for (aa = 0; aa < ALL_CNF_GPHS.length; aa++) {
		var gph_pair = ALL_CNF_GPHS[aa];
		var cnf_gph = gph_pair[0];
		if(is_blk){
			cnf_gph.style(black_nodes_style).update();
		} else {
			cnf_gph.style(white_nodes_style).update();
		}
	}
}

function no_labels(num_gph){
	var gph_idx = num_gph - 1;
	var gphs_sz = ALL_CNF_GPHS.length;
	if(isNaN(gph_idx) || (gph_idx < 0) || (gph_idx >= gphs_sz)){
		alert('ERROR. no_labels-invalid-param' + ' gph_idx=' + gph_idx + 
			' gphs_sz=' + gphs_sz);
		return;
	}
	var gph_pair = ALL_CNF_GPHS[gph_idx];
	var cnf_gph = gph_pair[0];
	var grph_div_id = gph_pair[1];
	//alert('GRAPH_ID=' + grph_div_id + ' gph_idx=' + gph_idx);

	all_cho = cnf_gph.filter('node.cho');
	all_cho.removeClass('cho');
	
	all_dipoles = cnf_gph.filter('node.dipole');
	all_dipoles.removeData('lbl lbl2');
	all_dipoles.removeClass('clb1');
	all_dipoles.removeClass('clb2');
}

function update_png(num_gph){
	var gph_idx = num_gph - 1;
	var gphs_sz = ALL_CNF_GPHS.length;
	if(isNaN(gph_idx) || (gph_idx < 0) || (gph_idx >= gphs_sz)){
		alert('ERROR. no_labels-invalid-param' + ' gph_idx=' + gph_idx + 
			' gphs_sz=' + gphs_sz);
		return;
	}
	set_png_for(gph_idx, true, true);
}

function is_black_theme(cnf_gph){
	if(ID_0 == null){
		alert('ERROR. Internal-no-ID_0');
		return;
	}
	var ele_0 = cnf_gph.getElementById(ID_0);
	//alert('bg=' + ele_0.style('background-color'));
	
	var bg_col = ele_0.style('background-color');
	var is_blk = ((bg_col != 'aqua') && (bg_col != 'white'));
	return is_blk;
}

function switch_theme(num_gph){
	
	var gph_idx = num_gph - 1;
	var gphs_sz = ALL_CNF_GPHS.length;
	if(isNaN(gph_idx) || (gph_idx < 0) || (gph_idx >= gphs_sz)){
		alert('ERROR. no_labels-invalid-param' + ' gph_idx=' + gph_idx + 
			' gphs_sz=' + gphs_sz);
		return;
	}
	var gph_pair = ALL_CNF_GPHS[gph_idx];
	var cnf_gph = gph_pair[0];
	var grph_div_id = gph_pair[1];
	
	var is_blk = is_black_theme(cnf_gph);
	//alert('is_blk=' + is_blk);
	
	if(is_blk){
		cnf_gph.style(white_nodes_style).update();
	} else {
		cnf_gph.style(black_nodes_style).update();
	}
}

