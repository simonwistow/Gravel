/*
 * Copyright (C) 2001  Simon Wistow <simon@twoshortplanks.com>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#include "swf_types.h"

#ifndef SWF_DESTROY_H
#define SWF_DESTROY_H

extern void swf_destroy_parser (swf_parser *);
extern void swf_destroy_tag (swf_tag *);
extern void swf_destroy_rect (swf_rect *);
extern void swf_destroy_header (swf_header *);
extern void swf_destroy_colour (swf_colour *);
extern void swf_destroy_gradcolour (swf_gradcolour *);
extern void swf_destroy_cxform (swf_cxform *);
extern void swf_destroy_matrix (swf_matrix *);
extern void swf_destroy_shapestyle (swf_shapestyle *);
extern void swf_destroy_linestyle (swf_linestyle *);
extern void swf_destroy_linestyle2 (swf_linestyle2 *);
extern void swf_destroy_fillstyle (swf_fillstyle *);
extern void swf_destroy_fillstyle2 (swf_fillstyle2 *);
extern void swf_destroy_rgba_pos (swf_rgba_pos *);
extern void swf_destroy_soundpoint (swf_soundpoint *);
extern void swf_destroy_imageguts (swf_imageguts *);
extern void swf_destroy_kerningpair (swf_kerningpair *);
extern void swf_destroy_adpcm (swf_adpcm *);
extern void swf_destroy_mp3header (swf_mp3header *);
extern void swf_destroy_mp3header_list (swf_mp3header_list *);
extern void swf_destroy_textrecord (swf_textrecord *);
extern void swf_destroy_textrecord_list (swf_textrecord_list *);
extern void swf_destroy_buttonrecord (swf_buttonrecord *);
extern void swf_destroy_buttonrecord_list (swf_buttonrecord_list *);
extern void swf_destroy_doaction (swf_doaction *);
extern void swf_destroy_doaction_list (swf_doaction_list *);
extern void swf_destroy_shaperecord (swf_shaperecord *);
extern void swf_destroy_shaperecord_list (swf_shaperecord_list *);
extern void swf_destroy_button2action (swf_button2action *);
extern void swf_destroy_button2action_list (swf_button2action_list *);
extern void swf_destroy_setbackgroundcolour (swf_setbackgroundcolour *);
extern void swf_destroy_definesound (swf_definesound *);
extern void swf_destroy_definefont (swf_definefont *);
extern void swf_destroy_definefont2 (swf_definefont2 *);
extern void swf_destroy_definefontinfo (swf_definefontinfo *);
extern void swf_destroy_placeobject (swf_placeobject *);
extern void swf_destroy_placeobject2 (swf_placeobject2 *);
extern void swf_destroy_defineshape (swf_defineshape *);
extern void swf_destroy_defineshape2 (swf_defineshape2 *);
extern void swf_destroy_definemorphshape (swf_definemorphshape *);
extern void swf_destroy_freecharacter (swf_freecharacter *);
extern void swf_destroy_namecharacter (swf_namecharacter *);
extern void swf_destroy_removeobject (swf_removeobject *);
extern void swf_destroy_removeobject2 (swf_removeobject2 *);
extern void swf_destroy_startsound (swf_startsound *);
extern void swf_destroy_definebits (swf_definebits *);
extern void swf_destroy_jpegtables (swf_jpegtables *);
extern void swf_destroy_definebitsjpeg2 (swf_definebitsjpeg2 *);
extern void swf_destroy_definebitsjpeg3 (swf_definebitsjpeg3 *);
extern void swf_destroy_definebitslossless (swf_definebitslossless *);
extern void swf_destroy_definetext (swf_definetext *);
extern void swf_destroy_definetext2 (swf_definetext2 *);
extern void swf_destroy_definebutton (swf_definebutton *);
extern void swf_destroy_definebutton2 (swf_definebutton2 *);
extern void swf_destroy_defineedittext (swf_defineedittext *);
extern void swf_destroy_framelabel (swf_framelabel *);
extern void swf_destroy_definebuttoncxform (swf_definebuttoncxform *);
extern void swf_destroy_definebuttonsound (swf_definebuttonsound *);
extern void swf_destroy_soundstreamblock (swf_soundstreamblock *);
extern void swf_destroy_soundstreamhead (swf_soundstreamhead *);
extern void swf_destroy_shapestyle (swf_shapestyle * style);
#endif