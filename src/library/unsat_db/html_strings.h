

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

Copyright (C) 2007-2012, 2014-2016. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/joseluisquiroga/ben-jose

ben-jose is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

dbg_strings_html.h

some strings for html printing.

--------------------------------------------------------------*/

#ifndef STR_HTML_H
#define STR_HTML_H

#define HTMi_html "<html>"
#define HTMe_html "</html>"
#define HTMi_title "<title>"
#define HTMe_title "</title>"
#define HTMi_head "<head>"
#define HTMe_head "</head>"
#define HTMi_body "<body>"
#define HTMe_body "</body>"
#define HTMi_h1 "<H1>"
#define HTMe_h1 "</H1>"
#define HTMi_h2 "<H2>"
#define HTMe_h2 "</H2>"
#define HTMi_h3 "<H3>"
#define HTMe_h3 "</H3>"
#define HTMi_p "<p>"
#define HTMe_p "</p>"
#define HTMi_js "<!-- inject:js -->"
#define HTMe_js "<!-- endinject:js -->"

#define HTMi_src "<script src='"
#define HTMe_src "'></script>"
#define HTMi_script "<script>"
#define HTMe_script "</script>"
#define HTMi_pre "<pre>"
#define HTMe_pre "</pre>"
#define HTMi_style "<style>"
#define HTMe_style "</style>"

#define HTM_word_wrap_css3 "white-space: pre-wrap;       /* css-3 */"
#define HTM_word_wrap_mozilla "white-space: -moz-pre-wrap;  /* Mozilla, since 1999 */"
#define HTM_word_wrap_opera4 "white-space: -pre-wrap;      /* Opera 4-6 */"
#define HTM_word_wrap_opera7 "white-space: -o-pre-wrap;    /* Opera 7 */"
#define HTM_word_wrap_ie "word-wrap: break-word;       /* Internet Explorer 5.5+ */"

#define HTM_div(nm_dv) "<div id='" + nm_dv + "'></div>" 
	
#define HTM_cy_div(nm_dv) "<div id='" + nm_dv + \
	"' style='height:600px;width:800px;border:1px solid #777;'></div>" \
	
// end_of_define

#define HTM_cy_hidden_div(nm_dv) "<div id='" + nm_dv + \
	"' style='height:600px;width:800px;border:1px solid #777;display: none;'></div>" \
	
// end_of_define

#define JS_inner_htm(div_txt, var_htm_txt) "document.getElementById('" + \
	div_txt + "').innerHTML = " + var_htm_txt + ";" \
	
// end_of_define

#define HTM_br "<br>\n"

#define HTM_css(dir) \
	"<link rel='stylesheet' type='text/css' \n\t\t\t href='./" + dir \
		+ "/sw_proof.css' media='screen' />" \
	
// end_of_define

#define HTMi_proof_ul "<ul class='collapsibleList'>"
#define HTMe_proof_ul "</ul>"

#define HTM_proof_li(nd_id, nd_lst, jsn_file, lbl_str) \
	"\t\t <li>\n" \
	"\t\t <label for='" + nd_id + "' \n" + \
	"\t\t\t onclick=\"populate_ul('" + nd_lst + "', './"+ jsn_file + "')\">\n" + \
	"\t\t\t " + lbl_str + "\n" + \
	"\t\t </label> \n" + \
	"\t\t <input type='checkbox' id='" + nd_id + "' />\n" + \
	"\t\t <ul id='" + nd_lst + "'>\n" + \
	"\t\t </ul>\n" \
	"\t\t </li>\n" \
	
// end_of_define
	

#endif		// STR_HTML_H


