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
 *
 * $Log: print_utils.h,v $
 * Revision 1.6  2001/06/22 17:16:51  muttley
 * Fixed get_textrecords and get_textrecord and associated destructors and printers
 *
 */

#include "swf_types.h"
#include "swf_parse.h"

#ifndef PRINT_UTILS_H

#define PRINT_UTILS_H

#define INDENT  printf("      ");

extern void print_cxform                (swf_cxform *, const char *);
extern void print_matrix                (swf_matrix *, const char *);
extern void print_rect                  (swf_rect   *  , const char *);
extern void print_shaperecords          (swf_shaperecord_list *, const char *);
extern void print_textrecord            (swf_textrecord *, const char *);
extern void print_textrecords           (swf_textrecord *, const char *);
extern void print_buttonrecords         (swf_buttonrecord_list *, const char *);
extern void print_doactions             (swf_doaction_list *, const char *);
extern void print_imageguts             (swf_imageguts *, const char *);
extern void print_button2actions        (swf_button2action_list *, const char *);
extern void print_shape_records         (swf_shaperecord_list *);
extern void print_mp3header_list        (swf_mp3header_list *, const char *);
extern void print_adpcm                 (swf_adpcm *, const char *);
extern void print_startsound            (swf_startsound *, const char *);
extern void print_soundstreamhead       (swf_soundstreamhead *, const char *, int);
extern void print_shapestyle            (swf_shapestyle * style, const char * str);

#endif


