/*
 * Copyright (C) 2002  Ben Evans <kitty@cpan.org>
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
 * 	$Id: triangle.c,v 1.2 2002/06/18 22:38:27 kitty_goth Exp $	
 */

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_destroy.h"
#include "swf_serialise.h"

#include <stdio.h>

swf_shaperecord_list * 
swf_make_shaperecords_for_triangle(int * error) 
{
    swf_shaperecord_list * list;
    swf_shaperecord * record;

    if ((list = (swf_shaperecord_list *) calloc (1, sizeof (swf_shaperecord_list))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    list->record_count = 0;
    list->first = NULL;
    list->lastp = &(list->first);

    /* This list has five records in it */

/* First, we need a non-edge, change of style record */ 
    record = swf_make_shaperecord(error, 0);
    record->flags = eflagsFill1 | eflagsMoveTo;
//    record->flags = eflagsFill1 | eflagsLine;

    record->fillstyle0 = 0;
    record->fillstyle1 = 1;
    record->linestyle = 1;

    record->x = 100*20;
    record->y = 0*20;


    swf_add_shaperecord(list, error, record);
    list->record_count++;

/* Then we need some edges */

    /* */
    record = swf_make_shaperecord(error, 1);
    record->x = 75 * 20;
    swf_add_shaperecord(list, error, record);
    list->record_count++;

    /* */
    record = swf_make_shaperecord(error, 1);
    record->y = 50 * 20;
    swf_add_shaperecord(list, error, record);
    list->record_count++;

    /* */
    record = swf_make_shaperecord(error, 1);
    record->x = -75 * 20;
    record->y = -50 * 20;
    swf_add_shaperecord(list, error, record);
    list->record_count++;

    /*    record = swf_make_shaperecord(error, 1);
    record->y = -50 * 20;
    swf_add_shaperecord(list, error, record);
    list->record_count++; */


/* Then we need an end-of-shape edge */
    record = swf_make_shaperecord(error, 0);
    swf_add_shaperecord(list, error, record);
    list->record_count++;

    return list;
}



/* To make a triangle, we need
   A shaperecord, comprising...
 * Fillstyle (just one for now)
 * Linestyle (just one for now)
 * three edges (straight lines)
 */
swf_tagrecord * 
swf_make_triangle(swf_movie * movie, int * error) 
{
    swf_tagrecord * triangle;
    swf_defineshape * shape;
    swf_rect * canvas;
    swf_shapestyle * mystyle;

    triangle = swf_make_tagrecord(error, tagDefineShape);

    if (*error) {
	return NULL;
    }

    if ((shape = (swf_defineshape *) calloc (1, sizeof (swf_defineshape))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    if ((canvas = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    if ((mystyle = (swf_shapestyle *) calloc (1, sizeof (swf_shapestyle))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }
    if ((mystyle->fills = (swf_fillstyle **) calloc (1, sizeof (swf_fillstyle *))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }
    if ((mystyle->lines = (swf_linestyle **) calloc (1, sizeof (swf_linestyle *))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }



    shape->tagid = ++(movie->max_obj_id);

    canvas->xmin = -200 * 20;
    canvas->xmax = 200 * 20;
    canvas->ymin = -200 * 20;
    canvas->ymax = 200 * 20;

    shape->rect = canvas;

    mystyle->nfills = 1;
    mystyle->nlines = 1;

    *(mystyle->fills) = swf_make_fillstyle(error);
    *(mystyle->lines) = swf_make_linestyle(error);
//    *(mystyle->lines) = NULL;

    shape->style = mystyle;

    shape->record = swf_make_shaperecords_for_triangle(error);

    triangle->tag = (void *) shape;

    return triangle;

 FAIL:
    swf_destroy_tagrecord(triangle);
    *error = SWF_EMallocFailure;
    return NULL;
}

