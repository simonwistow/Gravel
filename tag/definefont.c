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

swf_definefont *
swf_parse_definefont (swf_parser * context, int * error)
{
    swf_definefont * font;
    int n, start, xlast, ylast;
    int * offset_table;
    SWF_U16 fillbits, linebits;
	swf_font_extra *extra;

    if ((font = (swf_definefont *) calloc (1, sizeof (swf_definefont))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    font->shape_records = NULL;
    font->fontid = (SWF_U32) swf_parse_get_word(context);


    start = swf_parse_tell(context);
    font->offset = swf_parse_get_word (context);

    font->glyph_count = font->offset/2;

    if (!(extra = swf_fetch_font_extra(context, font->fontid, font->glyph_count))) {
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    if  ((offset_table =  (int *) calloc (font->glyph_count, sizeof (int))) == NULL) {
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    offset_table[0] = font->offset;

    for(n=1; n<font->glyph_count; n++) {
		offset_table[n] = swf_parse_get_word(context);
    }

    if  ((font->shape_records =  (swf_shaperecord_list **) calloc (font->glyph_count, sizeof (swf_shaperecord_list *))) == NULL) {
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    for(n=0; n<font->glyph_count; n++) {
        font->shape_records[n] = NULL;

        swf_parse_seek(context, offset_table[n] + start);

        swf_parse_initbits(context); /* reset bit counter */

        fillbits = (SWF_U16) swf_parse_get_bits(context, 4);
        linebits = (SWF_U16) swf_parse_get_bits(context, 4);

        xlast = 0;
        ylast = 0;

        font->shape_records[n] = NULL;
/* TODO swf_parse_get_shaperecords(context, error);*/
    }

    swf_free (offset_table);

    return font;

 FAIL:
    swf_destroy_definefont (font);
    return NULL;
}


void
swf_destroy_definefont (swf_definefont * font)
{
    int i = 0;

    if (font==NULL) {
        return;
    }

    for (i=0; i<font->glyph_count && (font->shape_records != NULL); i++) {
        swf_destroy_shaperecord_list (font->shape_records[i]);
    }

    swf_free (font->shape_records);
    swf_free (font);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
