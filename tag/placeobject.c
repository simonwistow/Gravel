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

swf_placeobject *
swf_parse_placeobject (swf_parser * context, int * error)
{
    swf_placeobject * place;

    if ((place = (swf_placeobject *) calloc (1, sizeof (swf_placeobject))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    place->tagid = (SWF_U32) swf_parse_get_word(context);
    place->depth = (SWF_U32) swf_parse_get_word(context);

    place->cxform = NULL;
    place->matrix = NULL;

    if ((place->matrix = swf_parse_get_matrix (context, error)) == NULL) {
        goto FAIL;
    }

    if (swf_parse_tell(context) < context->tagend) {
		if ((place->cxform = swf_parse_get_cxform (context, error, 0)) == NULL) {
            goto FAIL;
		}
    }

    return place;

 FAIL:
    swf_destroy_placeobject (place);
    return NULL;
}


void
swf_add_placeobject (swf_movie * movie, int * error, SWF_U16 char_id) 
{
    swf_tagrecord * temp;
	SWF_U16 depth;

    temp = swf_make_tagrecord(error);

    if (*error) {
		return;
    }

    temp->next = NULL;
    temp->id = tagPlaceObject;
    temp->tag = NULL;
    temp->serialised = 0;

/* Place Object specifics */
	depth = 1;
	
	swf_movie_put_word(movie, error, char_id);
	swf_movie_put_word(movie, error, depth);
    temp->buffer->size = 4;

	
/* Footer ... */

    *(movie->lastp) = temp;
    movie->lastp = &(temp->next);

    return;
}


void
swf_destroy_placeobject (swf_placeobject * object)
{
    if (object==NULL) {
        return;
    }

    swf_destroy_matrix (object->matrix);
    swf_destroy_cxform (object->cxform);

    swf_free (object);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
