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

swf_definebutton *
swf_parse_definebutton (swf_parser * context, int * error)
{
    swf_definebutton * button;

    if ((button = (swf_definebutton *) calloc (1, sizeof (swf_definebutton))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid   = swf_parse_get_word (context);

    if ((button->records = swf_parse_get_buttonrecords (context, error, FALSE)) == NULL)
    {
        goto FAIL;
    }

    if ((button->actions = swf_parse_get_doactions (context, error)) == NULL)
    {
        goto FAIL;
    }

    return button;

    FAIL:
    swf_destroy_definebutton (button);
    return NULL;

}

void
swf_destroy_definebutton (swf_definebutton * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_buttonrecord_list(button->records);
    swf_destroy_doaction_list (button->actions);
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
