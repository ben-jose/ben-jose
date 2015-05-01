
nod_w = 10;
dip_h = 10;
neu_h = 3;

col_pos = '#00CCFF';
col_neg = 'red';

// orange | green | yellow | blue
// SeaShell
// ellipse | triangle | pentagon | hexagon | heptagon | octagon | star

the_cnf_style = [
	{
		selector: 'node',
		css: {
			'content': 'data(lbl)',
			'color': 'yellow',
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

set_node_tier = function(gph, node, nd_tier) {
	if(nd_tier === undefined){
		nd_tier = null;
	}
	if(nd_tier != null){
		the_nd = gph.filter('node[id = "' + node + '"]');
		the_nd.data('lbl', nd_tier);
	}
};

play_node = function(gph, node, nd_tier) {

	q_tar = gph.elements('edge[target = "' + node + '"]');

	q_pos = q_tar.filter('.in_pos');
	q_neg = q_tar.filter('.in_neg');
	
	q_pos.removeClass('in_pos');
	q_pos.addClass('in_neg');
	q_neg.removeClass('in_neg');
	q_neg.addClass('in_pos');
	
	set_node_tier(gph, node, nd_tier);
};

fn_handle_event = function(event) {
	if(event.cyTarget.isNode()){
		var nd_id = event.cyTarget.data("id");
		play_node(event.cy, nd_id);
	}
};

show_cnf = function(grph_div_id, grph_elems, layout_nm, all_to_play, all_tiers){
	
	cnf_gph = cytoscape({
		container: document.getElementById(grph_div_id),

		style: the_cnf_style,

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

	cnf_gph.on('select', 'node', fn_handle_event);
	
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
	//play_node(cnf_gph, 'd2', 15);
}

