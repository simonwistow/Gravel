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


#include <stdio.h>
#include "swf_types.h"
#include "swf_file.h"

/* 
 * Have never seen a movie in the wild with a frame rate > 15. 
 * So set maximum as 25 to be conservative. 
 */

#define MAXVER 5
#define MAXSPEED 25
#define TYPICALSPEED 12

#define LINESTYLEMASK 0x20

typedef struct swf_triangle swf_triangle;

/* 
 * NOTE: There's a lot of stream/file-orientated duplication here.
 * Perhaps we should refactor into a stream adaptor shared by the
 * generator and parser
 */

/* 
 * High level for testing serialisation
 * (Dancing triangles flash movies, anyone?) 
 * FIXME: This 
 */

struct swf_triangle {

};


void swf_make_header_blank (swf_movie * movie, int * error, int ver, int speed);
void swf_make_header_raw (swf_movie * movie, int * error, swf_rect * rect);
void swf_make_header (swf_movie * movie, int * error, SCOORD x1, SCOORD x2, SCOORD y1, SCOORD y2);


void swf_make_finalise(swf_movie * movie, int * error);
swf_movie * swf_make_movie (int * error);
void swf_destroy_movie (swf_movie * movie);


swf_tagrecord * swf_make_tagrecord (int * error, SWF_U16 myid);
swf_shaperecord_list * swf_make_shaperecords_for_triangle(int * error);
swf_tagrecord * swf_make_triangle_as_tag(swf_movie * movie, int * error);


/*
void swf_get_raw_shape (swf_parser * swf, int * error, swf_tagrecord * mybuffer);
void swf_get_nth_shape (swf_parser * swf, int * error, int which_shape, swf_tagrecord * mybuffer); 
*/

swf_tagrecord * swf_get_raw_shape (swf_parser * swf, int * error);
swf_tagrecord * swf_get_nth_shape (swf_parser * swf, int * error, int which_shape);


void swf_dump_shape (swf_movie * movie, int * error, swf_tagrecord * temp);



swf_linestyle * swf_make_linestyle(int * error);
swf_fillstyle * swf_make_fillstyle(int * error);

