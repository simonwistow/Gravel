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

/*
 * This is very similar to swf_parse_placeobject, except that this more
 * complicated type includes ratio, clipdepth, a name and some
 * additional flags.
 */

#include "tag_handy.h"
#define MAX_PLACE2_SIZE 512 

/* MAX_PLACE2_SIZE has to be larger than MAX_PLACE_SIZE as 
 * the name attribute alone could be 255 
 */

swf_placeobject2 *
swf_parse_placeobject2 (swf_parser * context, int * error)
{
    swf_placeobject2 * place;

    if ((place = (swf_placeobject2 *) calloc (1, sizeof (swf_placeobject2))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    place->flags = swf_parse_get_byte (context);
    place->depth = swf_parse_get_word (context);
    place->matrix = NULL;
    place->cxform = NULL;


    /* Get the tag if specified. */
    if (place->flags & splaceCharacter) {
        place->tag = swf_parse_get_word (context);
    }

    /* Get the matrix if specified. */
    if (place->flags & splaceMatrix) {
        if ((place->matrix  = (swf_matrix *) swf_parse_get_matrix (context, error)) == NULL) {
            goto FAIL;
		}
    }

    /* Get the color transform if specified. */
    if (place->flags & splaceColorTransform) {
        if ((place->cxform = (swf_cxform *) swf_parse_get_cxform(context, error, TRUE)) == NULL) {
            goto FAIL;
    	}
    }

    /* Get the ratio if specified. */
    if (place->flags & splaceRatio) {
        place->ratio = swf_parse_get_word (context);
    }

    /* Get the clipdepth if specified. */
    if (place->flags & splaceDefineClip) {
        place->clip_depth = swf_parse_get_word (context);
    }

    /* Get the instance name */

    if (place->flags & splaceName){
        if ((place->name = (char *) swf_parse_get_string(context, error)) == NULL) {
            goto FAIL;
		}
    }


    return place;

 FAIL:
    swf_destroy_placeobject2 (place);
    return NULL;
}


/* We also need 'ratio' (and 'move') attributes */
void
swf_add_placeobject2 (swf_movie * movie, int * error, swf_matrix * mym, SWF_U16 char_id, SWF_U16 depth, swf_cxform * mycx, char * myname)
{
	swf_tagrecord * temp;
	SWF_U8 hasName, hasRatio, hasColour, hasMatrix, hasChar;

	hasName = hasRatio = hasColour = hasMatrix = hasChar = 0;

    temp = swf_make_tagrecord(error, tagPlaceObject2);

    if (*error) {
		return;
    }
   
/* Place Object2 specifics */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (MAX_PLACE2_SIZE, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	hasColour = (mycx != NULL);
	hasMatrix = (mym  != NULL);
	hasName   = (myname  != NULL);
	hasChar   = (char_id != 0);

	//	printf("hasName = %i hasRatio = %i hasColour = %i hasMatrix = %i hasChar = %i\n", hasName, hasRatio, hasColour, hasMatrix, hasChar);

    swf_buffer_initbits(temp->buffer);
	swf_buffer_put_bits(temp->buffer, 2, 0); /* Undocument bits, assume 0 */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* hasName, assume 0 for now */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* hasRatio, assume 0 for now */
	swf_buffer_put_bits(temp->buffer, 1, hasColour);
	swf_buffer_put_bits(temp->buffer, 1, hasMatrix);
	swf_buffer_put_bits(temp->buffer, 1, hasChar);
	swf_buffer_put_bits(temp->buffer, 1, 0); /* hasMove, assume 0 for now */
    swf_buffer_flush_bits(temp->buffer);

	swf_buffer_put_word(temp->buffer, error, depth);
	if (hasChar) {
		swf_buffer_put_word(temp->buffer, error, char_id);
	}
	if (hasMatrix) {
		swf_serialise_matrix(temp->buffer, error, mym);
	}
	if (hasColour) {
		swf_serialise_cxform_force_alpha(temp->buffer, error, mycx);
	}
	if (hasRatio) {
		/* Not yet */
	}
	if (hasName) {
		swf_buffer_put_string(temp->buffer, error, myname);
	}

    temp->serialised = 1;
	
/* Footer ... */

    swf_dump_shape(movie, error, temp);

    return;
}


void
swf_destroy_placeobject2 (swf_placeobject2 * object)
{
    if (object==NULL) {
        return;
    }

    swf_destroy_matrix (object->matrix);
    swf_destroy_cxform (object->cxform);

    swf_free (object->name);
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
