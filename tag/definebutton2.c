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

swf_definebutton2 *
swf_parse_definebutton2 (swf_parser *  context, int * error)
{

    swf_definebutton2 * button;
    SWF_U32 track_as_menu, offset, next_action;

    if ((button = (swf_definebutton2 *) calloc (1, sizeof (swf_definebutton2))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid = swf_parse_get_word (context);

    track_as_menu = swf_parse_get_byte(context);

    /* Get offset to first "Button2ActionCondition"
     * This offset is not in the spec!
     */
    offset = swf_parse_get_word(context);
    next_action = swf_parse_tell(context) + offset - 2;

    /* Parse Button Records */
    button->records = swf_parse_get_buttonrecords (context, error, TRUE);

    /*
     * Parse Button2ActionConditions
     */

    swf_parse_seek(context, next_action);

    #ifdef DEBUG
    fprintf (stderr, "[parse_definebutton2 : getting button2actions]\n");
    #endif

    if ((button->actions = swf_parse_get_button2actions (context, error)) == NULL)
    {
	    goto FAIL;
	}
    #ifdef DEBUG
    fprintf (stderr, "[parse_definebutton2 : button2actions are not NULL]\n");
    #endif
    return button;


 FAIL:
	swf_destroy_definebutton2 (button);
	return NULL;
}

void
swf_destroy_definebutton2 (swf_definebutton2 * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_buttonrecord_list(button->records);
    swf_destroy_button2action_list (button->actions);
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
