
/*
 * Copyright (C) 2001-2  Ben Evans <kitty@cpan.org>
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
 * 	$Id: header.c,v 1.1 2002/05/29 16:40:47 kitty_goth Exp $	
 */

#define SWF_OUT_STREAM 10240

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_destroy.h"
#include "swf_serialise.h"

#define SWF_DESTROY_MAX_TAG_ID 48

#include <stdio.h>


void 
swf_make_header_blank (swf_movie * movie, int * error, int ver, int speed) 
{
    swf_header * header;

    if ((ver < 0) || (ver > MAXVER) || (speed < 0) || (speed > MAXSPEED) ) {
	*error = SWF_EMallocFailure;
	return;
    }

    if (0 == ver) {
	ver = MAXVER;
    }

    if ((header = (swf_header *) calloc (1, sizeof (swf_header))) == NULL) {
	*error = SWF_EMallocFailure;
	return;
    }

/* 
 * Hardwired, because this is an illegal size to indicate
 * this movie needs to be finalised - ie have its frames
 * counted, size checked, etc., before
 * serialisation 
 */
    header->size = 20; 



    header->version = ver; 
    header->rate = speed; 
    header->count = 1;
    header->bounds = NULL;

    movie->header = header;

    return;
}


void
swf_make_header_raw (swf_movie * movie, int * error, swf_rect * rect) 
{
    swf_header * header;

    header = movie->header;

    if (!header) {
	swf_make_header_blank(movie, error, MAXVER, TYPICALSPEED);
	header = movie->header;
    }

    /* Throw the error further up the stack */
    if (*error) {
	return;
    }

    header->bounds = rect;

    return;
}

void
swf_make_header (swf_movie * movie, int * error, SCOORD x1, SCOORD x2, SCOORD y1, SCOORD y2) 
{
    swf_rect * rect;

    if ((rect = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
	*error = SWF_EMallocFailure;
	return;
    }

    rect->xmin = x1;
    rect->xmax = x2;
    rect->ymin = y1;
    rect->ymax = y2;

    swf_make_header_raw(movie, error, rect);

    return;
}
