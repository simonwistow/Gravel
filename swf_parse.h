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


#include <stdlib.h>
#include <string.h>
#include "swf_types.h"

#include "swf_error.h"


#ifndef SWF_PARSE_H
#define SWF_PARSE_H

extern const char * swf_MPEG_Ver[4];

extern swf_parser * swf_parse_create     (char *, int *);
extern swf_header * swf_parse_header     (swf_parser * , int *);




extern void         swf_parse_initbits   (swf_parser *);

extern U8           swf_parse_get_byte   (swf_parser *);
extern U8         * swf_parse_get_bytes  (swf_parser * , int);
extern U16          swf_parse_get_word   (swf_parser *);
extern U32          swf_parse_get_dword  (swf_parser *);

extern U32          swf_parse_get_sword  (swf_parser *);
extern U32          swf_parse_get_bits   (swf_parser *, S32);
extern S32          swf_parse_get_sbits  (swf_parser *, S32);

extern U32          swf_parse_nextid     (swf_parser *, int *);
extern void         swf_parse_skip       (swf_parser *, int);
extern void         swf_parse_seek       (swf_parser *, int);


extern int          swf_parse_tell       (swf_parser *);






extern swf_rect                * swf_parse_get_rect            (swf_parser * , int *);
extern swf_matrix              * swf_parse_get_matrix          (swf_parser * , int *);
extern swf_imageguts           * swf_parse_get_imageguts       (swf_parser * , int *);
extern swf_cxform              * swf_parse_get_cxform          (swf_parser * , int *, int);
extern swf_shapestyle          * swf_parse_get_shapestyle      (swf_parser * , int *, int);
extern swf_defineshape         * swf_parse_defineshape_aux     (swf_parser * , int *, int);

extern swf_adpcm               * swf_parse_adpcm_decompress    (swf_parser * , int *, int, int, int, int);
extern U32                       swf_parse_get_colour          (swf_parser * , int *, int);
extern char                    * swf_parse_get_string          (swf_parser * , int *);

extern swf_textrecord          * swf_parse_get_textrecord      (swf_parser * , int *, int, int, int);
extern swf_textrecord_list     * swf_parse_get_textrecords     (swf_parser * , int *, int, int, int);

extern swf_mp3header_list      * swf_parse_get_mp3headers      (swf_parser * , int *, int);

extern swf_buttonrecord        * swf_parse_get_buttonrecord    (swf_parser * , int *, int, int);
extern swf_buttonrecord_list   * swf_parse_get_buttonrecords   (swf_parser * , int *, int);

extern swf_shaperecord_list    * swf_parse_get_shaperecords    (swf_parser * , int *);
extern swf_shaperecord         * swf_parse_get_shaperecord     (swf_parser * , int *, int, int, int, int);

extern swf_doaction_list       * swf_parse_get_doactions       (swf_parser * , int *);
extern swf_button2action_list  * swf_parse_get_button2actions  (swf_parser * , int *);

extern swf_definebitslossless  * swf_parse_definebitslossless  (swf_parser * , int *);
extern swf_definebitslossless  * swf_parse_definebitslossless2 (swf_parser * , int *);
extern swf_definefont          * swf_parse_definefont          (swf_parser * , int *);
extern swf_definefont2         * swf_parse_definefont2         (swf_parser * , int *);
extern swf_definefontinfo      * swf_parse_definefontinfo      (swf_parser * , int *);
extern swf_definesound         * swf_parse_definesound         (swf_parser * , int *);
extern swf_placeobject         * swf_parse_placeobject         (swf_parser * , int *);
extern swf_placeobject2        * swf_parse_placeobject2        (swf_parser * , int *);
extern swf_removeobject        * swf_parse_removeobject        (swf_parser * , int *);
extern swf_removeobject2       * swf_parse_removeobject2       (swf_parser * , int *);
extern swf_startsound          * swf_parse_startsound          (swf_parser * , int *);
extern swf_definebits          * swf_parse_definebits          (swf_parser * , int *);
extern swf_jpegtables          * swf_parse_jpegtables          (swf_parser * , int *);
extern swf_definebitsjpeg2     * swf_parse_definebitsjpeg2     (swf_parser * , int *);
extern swf_definebitsjpeg3     * swf_parse_definebitsjpeg3     (swf_parser * , int *);
extern swf_definetext          * swf_parse_definetext          (swf_parser * , int *);
extern swf_definetext2         * swf_parse_definetext2         (swf_parser * , int *);
extern swf_defineedittext      * swf_parse_defineedittext      (swf_parser * , int *);
extern swf_definebutton        * swf_parse_definebutton        (swf_parser * , int *);
extern swf_definebutton2       * swf_parse_definebutton2       (swf_parser * , int *);
extern swf_framelabel          * swf_parse_framelabel          (swf_parser * , int *);
extern swf_definebuttoncxform  * swf_parse_definebuttoncxform  (swf_parser * , int *);
extern swf_definebuttonsound   * swf_parse_definebuttonsound   (swf_parser * , int *);


extern swf_soundstreamhead     * swf_parse_soundstreamhead     (swf_parser * , int *);
extern swf_soundstreamhead     * swf_parse_soundstreamhead2    (swf_parser * , int *);
extern swf_soundstreamblock    * swf_parse_soundstreamblock    (swf_parser * , int *);

extern swf_setbackgroundcolour * swf_parse_setbackgroundcolour (swf_parser * , int *);
extern swf_defineshape         * swf_parse_defineshape         (swf_parser * , int *);
extern swf_definemorphshape    * swf_parse_definemorphshape    (swf_parser * , int *);
//swf_defineshape2             * swf_parse_defineshape2        (swf_parser * , int *);
extern swf_freecharacter       * swf_parse_freecharacter       (swf_parser * , int *);
extern swf_namecharacter       * swf_parse_namecharacter       (swf_parser * , int *);

#endif



