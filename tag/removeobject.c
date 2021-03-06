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

swf_removeobject *
swf_parse_removeobject(swf_parser * context, int * error)
{
    swf_removeobject * object;

    if ((object = (swf_removeobject *) calloc (1, sizeof (swf_removeobject))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    object->tagid = (SWF_U32) swf_parse_get_word (context);
    object->depth = (SWF_U32) swf_parse_get_word (context);

    return object;
}

void
swf_add_removeobject (swf_movie * movie, int * error, SWF_U16 char_id, SWF_U16 depth)
{
	swf_tagrecord * temp;

    temp = swf_make_tagrecord(error, tagRemoveObject);

    if (*error) {
		return;
    }
   
    temp->serialised = 1;

/* Remove Object specifics */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (4, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	swf_buffer_put_word(temp->buffer, error, char_id);
	swf_buffer_put_word(temp->buffer, error, depth);
    temp->buffer->size = 4;

    swf_dump_tag(movie, error, temp);

    return;
}

void
swf_destroy_removeobject (swf_removeobject * object)
{
    if (object==NULL) {
        return;
    }


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
