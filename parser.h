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

#ifndef PARSER_H

#define PARSER_H

extern void parse_frame                 (swf_parser *, const char *);
extern void parse_end                   (swf_parser *, const char *);
extern void parse_setbackgroundcolour   (swf_parser *, const char *);
extern void parse_definefont            (swf_parser *, const char *);
extern void parse_definefont2           (swf_parser *, const char *);
extern void parse_definefontinfo        (swf_parser *, const char *);
extern void parse_placeobject           (swf_parser *, const char *);
extern void parse_placeobject2          (swf_parser *, const char *);
extern void parse_removeobject          (swf_parser *, const char *);
extern void parse_removeobject2         (swf_parser *, const char *);

extern void parse_defineshape_aux       (swf_parser *, int, const char *); //todo make more consisten
extern void parse_defineshape           (swf_parser *, const char *);
extern void parse_defineshape2          (swf_parser *, const char *);
extern void parse_defineshape3          (swf_parser *, const char *);
extern void parse_definemorphshape      (swf_parser *, const char *);
extern void parse_definebits            (swf_parser *, const char *);
extern void parse_jpegtables            (swf_parser *, const char *);
extern void parse_definebitsjpeg2       (swf_parser *, const char *);
extern void parse_definebitsjpeg3       (swf_parser *, const char *);
extern void parse_definetext            (swf_parser *, const char *);
extern void parse_definetext2           (swf_parser *, const char *);
extern void parse_defineedittext        (swf_parser *, const char *);
extern void parse_definebutton          (swf_parser *, const char *);
extern void parse_definebutton2         (swf_parser *, const char *);
extern void parse_definebuttoncxform    (swf_parser *, const char *);
extern void parse_definebuttonsound     (swf_parser *, const char *);
extern void parse_freecharacter         (swf_parser *, const char *);
extern void parse_namecharacter         (swf_parser *, const char *);
extern void parse_startsound            (swf_parser *, const char *);
extern void parse_stopsound             (swf_parser *, const char *);
extern void parse_definesound           (swf_parser *, const char *);
extern void parse_framelabel            (swf_parser *, const char *);
extern void parse_soundstreamblock      (swf_parser *, const char *);
extern void parse_soundstreamhead       (swf_parser *, const char *);
extern void parse_soundstreamhead2      (swf_parser *, const char *);

extern void usage                       (void);

const char ** init_tags                 (void);

#endif