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

swf_textrecord *
swf_parse_get_textrecord (swf_parser * context, int * error, int has_alpha, int glyph_bits, int advance_bits)
{
    int g;
    swf_textrecord * record;
    SWF_U8 flags = swf_parse_get_byte(context);

    if (flags == 0) { return NULL; }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecord : mallocing]\n");
    #endif

    if ((record = (swf_textrecord *) calloc (1, sizeof(swf_textrecord))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    record->flags = flags;
    record->next = NULL;


    if (flags & isTextControl)
    {
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : is text control ");
        #endif
        if (flags & textHasFont)
        {

            record->font_id = swf_parse_get_word(context);
            #ifdef SWF_PARSE_DEBUG
            fprintf(stderr,"font id is %ld", record->font_id);
            #endif
        }
        if (flags & textHasColour)
        {
            record->colour = swf_parse_get_colour (context, error,  has_alpha);
        }
        if (flags & textHasXOffset)
        {
            record->xoffset = swf_parse_get_word(context);
        }
        if (flags & textHasYOffset)
        {
            record->yoffset = swf_parse_get_word(context);
        }
        if (flags & textHasFont)
        {
            record->font_height = swf_parse_get_word(context);
        }
        #ifdef SWF_PARSE_DEBUG
        fprintf(stderr,"]\n");
        #endif
    }
    else
    {
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : is glyphs]\n");
        #endif

        record->glyph_count = flags;
        if ((record->glyphs = (int **) calloc (record->glyph_count, sizeof (int *))) == NULL)
        {
           *error = SWF_EMallocFailure;
            goto FAIL;
        }

        swf_parse_initbits(context);     // reset bit counter

        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : getting %d glyphs]\n", record->glyph_count);
        #endif

        for ( g = 0; g < record->glyph_count; g++)
        {
            record->glyphs[g] = NULL;
            if ((record->glyphs[g] = (int *) calloc (2, sizeof(int))) == NULL)
            {
               *error = SWF_EMallocFailure;
                goto FAIL;
            }

            record->glyphs[g][0] = swf_parse_get_bits (context, glyph_bits);    // index
            record->glyphs[g][1] = swf_parse_get_bits (context, advance_bits);  // advance
        }


    }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecord : returning]\n");
    #endif

    return record;

    FAIL:
    swf_destroy_textrecord (record);
    return NULL;

}


void
swf_destroy_textrecord (swf_textrecord * record)
{
    int i=0;

    if (record==NULL) {
        return;
    }

    for (i=0; i<record->glyph_count; i++) {
        swf_free (record->glyphs[i]);
    }
    swf_free (record->glyphs);
    swf_free (record);

    return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
