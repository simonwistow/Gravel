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
 * Read the id and name of the font, and read in the glyphs
 * corresponding to this font.
 */

swf_definefontinfo *
swf_parse_definefontinfo (swf_parser * context, int * error)
{
    int n, glyph_count;

    swf_definefontinfo * info;

    if ((info = (swf_definefontinfo *) calloc (1, sizeof (swf_definefontinfo))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    info->fontid = (SWF_U32) swf_parse_get_word (context);
    info->namelen = swf_parse_get_byte (context);

    /* Allocate enough space to hold the name of the font */
    if ( ( info->fontname = (char *) calloc ((1 + info->namelen), sizeof(char))) == NULL ){
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    /* Read the fontname from the stream*/
    for(n=0; n < info->namelen; n++) {
        info->fontname [n] = (char) swf_parse_get_byte (context);
    }

    info->fontname [n] = '\0';

    info->flags = swf_parse_get_byte (context);

    /* The glyph counts (ie, how many symbols are in each font) have
       presumably already been read into place by a DefineFont tag */
    glyph_count = context->glyph_counts[info->fontid];

    if ((info->code_table = (int *) calloc (glyph_count, sizeof(int) ) ) == NULL) {
        *error = SWF_EMallocFailure;
        goto FAIL;
    }

    /* Read in the glyphs present in this font, taking account of whether
       they are 8-bit or 16-bit values.
     */
    for(n=0; n < glyph_count; n++)
    {
        if (info->flags & FONT_WIDE_CODES) {
            info->code_table[n] = (int) swf_parse_get_word (context);
        } else {
	        info->code_table[n] = (int) swf_parse_get_byte (context);
        }

        context->font_chars [info->fontid][n] = (char) info->code_table[n];

    }



    return info;

 FAIL:
    swf_destroy_definefontinfo (info);
    return NULL;
}


void
swf_destroy_definefontinfo (swf_definefontinfo * info)
{
    if (info==NULL) {
        return;
    }

    swf_free (info->code_table);
    swf_free (info->fontname);
    swf_free (info);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
