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

swf_namecharacter *
swf_parse_namecharacter (swf_parser * context, int * error)
{
    swf_namecharacter * name;

    if ((name = (swf_namecharacter *) calloc (1, sizeof(swf_namecharacter))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    name->tagid = swf_parse_get_word (context);
    name->label = swf_parse_get_string (context, error);

    return name;
}

void
swf_destroy_namecharacter (swf_namecharacter * object)
{
    if (object==NULL) {
        return;
    }

    swf_free (object->label);
    swf_free (object);
	return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
