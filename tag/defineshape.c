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
 */

#include "tag_handy.h"

/*
 * Thin wrapper over swf_parse_defineshape_aux.
 * The spec has defineshape, defineshape2, etc but rather than
 * aping that, I think it's better to have these thin wrappers. --BE
 */

swf_defineshape *
swf_parse_defineshape (swf_parser * context, int * error)
{
    return (swf_defineshape *) swf_parse_defineshape_aux (context, error, FALSE);
}


swf_defineshape *
swf_parse_defineshape_aux (swf_parser * context, int * error, int with_alpha)
{

    swf_defineshape * shape;
	
    if ((shape = (swf_defineshape *) calloc (1, sizeof (swf_defineshape))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    shape->tagid  = (SWF_U32) swf_parse_get_word (context);
    shape->rect   = NULL;
    shape->style  = NULL;
    shape->record = NULL;

    return shape;

    /* Get the bounding rectangle */

    if ((shape->rect =  swf_parse_get_rect (context, error)) == NULL) {
        goto FAIL;
    }

    if ((shape->style = swf_parse_get_shapestyle(context, error, with_alpha)) == NULL) {
        goto FAIL;
    }

/*
 * Bug!  this was not in the original swf_parse.cpp
 * Required to reset bit counters and read byte aligned.
 */

    swf_parse_initbits (context);

    context->fill_bits = (SWF_U16) swf_parse_get_bits (context, 4);
    context->line_bits = (SWF_U16) swf_parse_get_bits (context, 4);

/* TODO simon */
    if ((shape->record = swf_parse_get_shaperecords (context,error)) == NULL) {
        goto FAIL;
    }

    return shape;

 FAIL:
    swf_destroy_defineshape (shape);
    return NULL;
}


void
swf_serialise_defineshape (swf_buffer * buffer, int * error, swf_defineshape * shape)
{
	swf_shaperecord *node, *temp;

	swf_buffer_initbits(buffer);
	swf_buffer_put_word(buffer, error, shape->tagid);
	swf_buffer_rect(buffer, error, shape->rect);
	
	swf_buffer_shapestyle(buffer, error, shape->style);

	node = shape->record->first;

	while (node != NULL) {
		temp = node;
		node = node->next;
		// FIXME: Do stuff...
	}

}

void
swf_destroy_defineshape (swf_defineshape * shape)
{

    if (shape==NULL) {
        return;
    }
    swf_destroy_rect (shape->rect);
    swf_destroy_shapestyle (shape->style);
    swf_destroy_shaperecord_list (shape->record);
    swf_free(shape);
    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
