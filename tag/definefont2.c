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

swf_definefont2 *
swf_parse_definefont2 (swf_parser * context, int * error)
{
    swf_definefont2 * font;
    int i, n, data_pos;
    SWF_U32 code_offset;
    SWF_U32 * offset_table;

    if ((font = (swf_definefont2 *) calloc (1, sizeof (swf_definefont2))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    font->name = NULL;
    font->code_table = NULL;
    font->glyphs = NULL;
    font->kerning_pairs = NULL;
    font->bounds = NULL;

    font->fontid = swf_parse_get_word (context);
    font->flags = swf_parse_get_word (context);
    font->name_len = swf_parse_get_byte (context);

    if ((font->name = (char *) calloc (font->name_len, sizeof(char))) == NULL) {
  	    *error = SWF_EMallocFailure;
    	goto FAIL;
    }

    for (i=0; i<font->name_len; i++) {
        font->name[i] = (char) swf_parse_get_byte(context);
    }

    /* Get the number of glyphs. */
    font->glyph_count = swf_parse_get_word(context);


    context->glyph_counts[font->fontid] = font->glyph_count;

    context->number_of_fonts++;
    /* todo :
     * maybe the number of fonts and the font id will get out of sync.
     * Should we have a lookup up table?
     * Also, what happens if we define more than 255 fonts? Should
     * put checks in for that
     */

    if (font->glyph_count > 0)
    {
	    if ((context->font_chars [font->fontid] = (char *) calloc (font->glyph_count, sizeof (char))) == NULL)
	    {
		    *error = SWF_EMallocFailure;
		    goto FAIL;
	    }

		data_pos = swf_parse_tell(context);

        /* Get the FontOffsetTable */


        if ((offset_table = (SWF_U32 *) calloc (font->glyph_count, sizeof (SWF_U32))) == NULL)
        {
	        *error = SWF_EMallocFailure;
            goto FAIL;
        }

        for (n=0; n<font->glyph_count; n++) {
            if (font->flags & sfontFlagsWideOffsets) {
                offset_table[n] = swf_parse_get_dword(context);
            } else {
                offset_table[n] = swf_parse_get_word(context);
            }
    	}

        /* Get the CodeOffset */


        code_offset = 0;
        if (font->flags & sfontFlagsWideOffsets) {
            code_offset = swf_parse_get_dword(context);
        } else {
            code_offset = swf_parse_get_word(context);
		}

        /* Get the Glyphs */
		if ((font->glyphs = (swf_shaperecord_list **) calloc (font->glyph_count, sizeof(swf_shaperecord_list *))) == NULL) {
			goto FAIL;
		}


        for(n=0; n<font->glyph_count; n++) {

    	    swf_parse_seek (context, data_pos + offset_table[n]);
    	    swf_parse_initbits (context); /* reset bit counter */

    	    /* todo simon : do these really need to be set in the context? */
            context->fill_bits = (SWF_U16) swf_parse_get_bits(context, 4);
            context->line_bits = (SWF_U16) swf_parse_get_bits(context, 4);

    	    font->glyphs[n] = (swf_shaperecord_list *) swf_parse_get_shaperecords(context, error);

        }

    	swf_free (offset_table);


        if (swf_parse_tell (context) != data_pos + code_offset)
        {
	        /* todo simon : should I return NULL here? */
    	    swf_parse_seek(context, data_pos + code_offset);
        }

    	if ((font->code_table = (SWF_U32 *) calloc (font->glyph_count, sizeof (SWF_U32))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
	    }

    	/* Get the CodeTable */
        for (i=0; i<font->glyph_count; i++) {
            if (font->flags & sfontFlagsWideOffsets) {
                font->code_table [i] = swf_parse_get_word (context);
            } else {
                font->code_table [i] = swf_parse_get_byte (context);
			}
        }
    }

    if (font->flags & sfontFlagsHasLayout) {

        /* Get "layout" fields */

        font->ascent  = swf_parse_get_word (context);
        font->descent = swf_parse_get_word (context);
        font->leading = swf_parse_get_word (context);

        /* Skip Advance table */
		/* todo simon : does this need to be done ???*/
        swf_parse_skip (context, font->glyph_count * 2);
		


        /* Get BoundsTable */
		if ((font->bounds = (swf_rect **) calloc (font->glyph_count, sizeof(swf_rect*))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
	    }

        for (i=0; i<font->glyph_count; i++)
        {
           if  ((font->bounds[i] = swf_parse_get_rect (context, error)) == NULL) {
                goto FAIL;
            }
        }

        /*
         * Get Kerning Pairs
         */

    	font->nkerning_pairs = swf_parse_get_word (context);

        if (font->nkerning_pairs && (font->kerning_pairs = (swf_kerningpair **) calloc (font->nkerning_pairs, sizeof(swf_kerningpair *))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }


        for (i=0; i<font->nkerning_pairs; i++)
        {
	        if ((font->kerning_pairs[i] = (swf_kerningpair *) calloc (1, sizeof(swf_kerningpair))) == NULL) {
                *error = SWF_EMallocFailure;
	    	    goto FAIL;
	        }

            if (font->flags & sfontFlagsWideOffsets)
            {
                font->kerning_pairs[i]->code1 = swf_parse_get_word (context);
                font->kerning_pairs[i]->code2 = swf_parse_get_word (context);
            }
            else
            {
                font->kerning_pairs[i]->code1 = swf_parse_get_byte (context);
                font->kerning_pairs[i]->code2 = swf_parse_get_byte (context);
            }
            font->kerning_pairs[i]->adjust = swf_parse_get_word(context);

        }
    }


    return font;

    FAIL:
    swf_destroy_definefont2 (font);
    return NULL;

}

void
swf_destroy_definefont2 (swf_definefont2 * font)
{
    int i=0;

    if (font==NULL) {
        return;
    }

    for (i=0; i<font->glyph_count; i++) {
        swf_destroy_shaperecord_list (font->glyphs[i]);
        if (font->bounds) swf_destroy_rect (font->bounds[i]);
    }

    swf_free (font->glyphs);
    swf_free (font->name);
    swf_free (font->code_table);
    swf_free (font->bounds);

    for (i=0; i<font->nkerning_pairs; i++) {
        swf_destroy_kerningpair (font->kerning_pairs[i]);
    }

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
