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

swf_definebitslossless *
swf_parse_definebitslossless (swf_parser * context, int * error)
{
    swf_definebitslossless * bits;

    if ((bits = (swf_definebitslossless *) calloc (1, sizeof(swf_definebitslossless))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);

    bits->format = swf_parse_get_byte(context);
    bits->width  = swf_parse_get_word(context);
    bits->height = swf_parse_get_word(context);
    bits->colourtable_size = 0;

    if (bits->format == 3) {
        bits->colourtable_size = swf_parse_get_byte (context);
    }

    bits->colourtable_size++;

    bits->data = swf_parse_get_bytes (context, context->tagend - swf_parse_tell(context));

    return bits;
}


void
swf_destroy_definebitslossless (swf_definebitslossless * object)
{
    if (object==NULL) {
        return;
    }

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
