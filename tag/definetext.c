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

swf_definetext *
swf_parse_definetext (swf_parser * context, int * error)
{

    swf_definetext * text;
    int n_glyph_bits, n_advance_bits;


    if ((text = (swf_definetext *) calloc (1, sizeof (swf_definetext))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    text->tagid  = swf_parse_get_word   (context);
    text->rect   = swf_parse_get_rect   (context, error);
    text->matrix = swf_parse_get_matrix (context, error);

    n_glyph_bits   = (int)swf_parse_get_byte (context);
    n_advance_bits = (int)swf_parse_get_byte (context);

    if ((text->records  =  swf_parse_get_textrecords (context, error, FALSE, n_glyph_bits, n_advance_bits)) == NULL) {
		goto FAIL;
    }

    return text;

    FAIL:
    swf_destroy_definetext (text);
    return NULL;
}


void
swf_destroy_definetext (swf_definetext * text)
{
    if (text==NULL) {
        return;
    }

    swf_destroy_rect (text->rect);
    swf_destroy_matrix (text->matrix);
    swf_destroy_textrecord_list (text->records);
    swf_free (text);

    return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
