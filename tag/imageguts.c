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

swf_imageguts *
swf_parse_get_imageguts (swf_parser * context, int * error)
{
    int count=0;

    int size = 0;
    #define SWF_IMAGE_GUTS_BLOCK_SIZE (256)

    swf_imageguts * guts = NULL;

    //return NULL;

    if ((guts = (swf_imageguts *) calloc (1, sizeof (swf_imageguts))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }


    while (swf_parse_tell (context) < context->next_tag_pos) {
        if ((count % SWF_IMAGE_GUTS_BLOCK_SIZE) == 0) {
            size+= SWF_IMAGE_GUTS_BLOCK_SIZE;
	    if (guts->data != NULL) {
		    guts->data = (SWF_U8 *) realloc (guts->data, sizeof(SWF_U8) * size);
	    }
	    else {
		    guts->data = (SWF_U8 *) calloc (size, sizeof(SWF_U8));
	    }
	    if (guts->data == NULL) {
                *error = SWF_EReallocFailure;
                goto FAIL;
            }
        }

        guts->data[count++] = swf_parse_get_byte (context);
    }

    guts->nbytes = count;

    while ((count % SWF_IMAGE_GUTS_BLOCK_SIZE) != 0) {
        guts->data [count++] = (SWF_U8) NULL;
    }

    return guts;

 FAIL:
    swf_destroy_imageguts (guts);
    return NULL;

}


void
swf_destroy_imageguts (swf_imageguts * guts)
{
    if (guts==NULL) {
        return;
    }

    swf_free (guts->data);
    swf_free (guts);
	return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
