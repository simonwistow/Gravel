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

swf_definebitsjpeg2 *
swf_parse_definebitsjpeg2 (swf_parser * context, int * error)
{
    swf_definebitsjpeg2 * bits;

    if ((bits = (swf_definebitsjpeg2 *) calloc (1, sizeof (swf_definebitsjpeg2))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);
    if ((bits->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        goto FAIL;

    }


    return bits;

 FAIL:
    swf_destroy_definebitsjpeg2 (bits);
    return NULL;
}


void
swf_destroy_definebitsjpeg2 (swf_definebitsjpeg2 * bits)
{
    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);

    return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
