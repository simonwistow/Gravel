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

swf_defineedittext *
swf_parse_defineedittext (swf_parser * context, int * error)
{

    swf_defineedittext * text;

    if ((text = (swf_defineedittext *) calloc (1, sizeof(swf_defineedittext))) == NULL)
    {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    text->tagid  = swf_parse_get_word (context);
    text->bounds = swf_parse_get_rect(context, error);
    text->flags  = swf_parse_get_word (context);

    if (text->flags & seditTextFlagsHasFont) {
        text->font_id = swf_parse_get_word (context);
        text->font_height = swf_parse_get_word (context);
    }

    if (text->flags & seditTextFlagsHasTextColor) {
        text->colour = swf_parse_get_colour (context, error,  TRUE);
    }

    if (text->flags & seditTextFlagsHasMaxLength) {
        text->max_length = swf_parse_get_word (context);
    }


    if (text->flags & seditTextFlagsHasLayout) {
        text->align        = swf_parse_get_byte (context);
       	text->left_margin  = swf_parse_get_word (context);
        text->right_margin = swf_parse_get_word (context);
        text->indent       = swf_parse_get_word (context);
        text->leading      = swf_parse_get_word (context);
    }

    if ((text->variable = swf_parse_get_string (context, error)) == NULL) {
        goto FAIL;
    }

    if (text->flags & seditTextFlagsHasText ) {
        if ((text->initial_text = swf_parse_get_string (context, error)) == NULL) {
			goto FAIL;
        }
    }


    return text;

 FAIL:
    swf_destroy_defineedittext (text);
    return NULL;
}


void
swf_destroy_defineedittext (swf_defineedittext * text)
{
    if (text==NULL) {
        return;
    }

    swf_free (text->variable);
    swf_free (text->initial_text);

    swf_destroy_rect (text->bounds);

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
