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

swf_definebuttonsound *
swf_parse_definebuttonsound (swf_parser * context, int * error)
{
    swf_definebuttonsound * button;
    int i;
    swf_startsound * state;

    if ((button = (swf_definebuttonsound *) calloc (1, sizeof (swf_definebuttonsound))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid      = swf_parse_get_word (context);
    button->up_state   = NULL;
    button->over_state = NULL;
    button->down_state = NULL;

    for (i=0; i<3; i++)
    {
        if ((state = swf_parse_startsound (context, error)) == NULL)
        {
            goto FAIL;
        }


        switch (i)
        {
		case 0:
			button->up_state = state;
			break;

		case 1:
			button->over_state = state;
			break;
			
		case 2:
			button->down_state = state;
			break;
        }

    }

    return button;

 FAIL:
    swf_destroy_definebuttonsound (button);
    return NULL;
}


void
swf_destroy_definebuttonsound (swf_definebuttonsound * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_startsound (button->up_state);
    swf_destroy_startsound (button->over_state);
    swf_destroy_startsound (button->down_state);
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
