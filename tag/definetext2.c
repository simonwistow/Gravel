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
 * Yes, I know this is exactly the same as definetext
 * but I want them as two seperate functions 'cos I'm sure
 * that the specs must be lying about this. Or maybe not.
 * Whatever.
 */
swf_definetext2 *
swf_parse_definetext2 (swf_parser * context, int * error)
{
    swf_definetext2 * text;
    int n_glyph_bits, n_advance_bits;

    if ((text = (swf_definetext2 *) calloc (1, sizeof (swf_definetext2))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    text->tagid   = swf_parse_get_word   (context);
    text->rect    = swf_parse_get_rect   (context, error);
    text->matrix  = NULL;
    text->records = NULL;
    if ((text->matrix  = swf_parse_get_matrix (context, error)) == NULL) {
        goto FAIL;
    }

    n_glyph_bits   = (int)swf_parse_get_byte (context);
    n_advance_bits = (int)swf_parse_get_byte (context);

    if ((text->records  =  swf_parse_get_textrecords (context, error, TRUE, n_glyph_bits, n_advance_bits)) == NULL) {
        goto FAIL;
    }

    return text;

    FAIL:
    swf_destroy_definetext2 (text);
    return NULL;
}

void
swf_destroy_definetext2 (swf_definetext2 * text)
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
