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

swf_definebuttoncxform *
swf_parse_definebuttoncxform (swf_parser * context, int * error)
{
    int size=0;
    swf_definebuttoncxform * button;

    if ((button = (swf_definebuttoncxform *) calloc (1, sizeof (swf_definebuttoncxform))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid = swf_parse_get_word (context);


    while (swf_parse_tell (context) < context->tagend)
    {
        button->ncxforms = size++;
        if ((button->cxforms = (swf_cxform **) realloc (button->cxforms, sizeof (swf_cxform) * size)) == NULL)
        {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }

        if ((button->cxforms[size-1] = swf_parse_get_cxform (context, error, FALSE)) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }

    }

    return button;

 FAIL:
    swf_destroy_definebuttoncxform (button);
    return NULL;

}


void
swf_destroy_definebuttoncxform (swf_definebuttoncxform * button)
{
    int i=0;

    if (button==NULL) {
        return;
    }


    for (i=0; i<button->ncxforms; i++) {
        swf_destroy_cxform (button->cxforms[i]);
    }
    swf_free (button->cxforms);

    swf_free (button);

	return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
