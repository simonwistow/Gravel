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

swf_setbackgroundcolour *
swf_parse_setbackgroundcolour (swf_parser * context, int * error)
{
    swf_setbackgroundcolour * back;

    if ((back = (swf_setbackgroundcolour *) calloc (1, sizeof (swf_setbackgroundcolour))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    if ((back->colour = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
		*error = SWF_EMallocFailure;
	    goto FAIL;
    }

    back->colour->r = swf_parse_get_byte(context);
    back->colour->g = swf_parse_get_byte(context);
    back->colour->b = swf_parse_get_byte(context);

    return back;

 FAIL:
    swf_destroy_setbackgroundcolour (back);
    return NULL;
}


void 
swf_add_setbackgroundcolour(swf_movie * movie, int * error, SWF_U8 red, SWF_U8 green, SWF_U8 blue, SWF_U8 alpha) 
{
    swf_tagrecord * temp;
    swf_setbackgroundcolour * col;

    temp = swf_make_tagrecord(error, tagSetBackgroundColour);

    if (*error) {
		return;
    }

    if ((col = (swf_setbackgroundcolour *) calloc ( 1, sizeof (swf_setbackgroundcolour))) == NULL) {
      fprintf(stderr, "alloc fuckup\n");
      return;
    }

    if ((col->colour = (swf_colour *) calloc ( 1, sizeof (swf_colour))) == NULL) {
      fprintf(stderr, "alloc fuckup\n");
      return;
    }

    if ((temp->buffer->raw = (SWF_U8 *) calloc ( 4, sizeof (SWF_U8))) == NULL) {
      fprintf(stderr, "alloc fuckup\n");
      return;
    }

    col->colour->r = red;
    col->colour->g = green;
    col->colour->b = blue;
    col->colour->a = alpha;

    temp->buffer->raw[0] = col->colour->r;
	temp->buffer->raw[1] = col->colour->g;
	temp->buffer->raw[2] = col->colour->b;
	temp->buffer->raw[3] = col->colour->a;

    temp->tag = col;
    temp->serialised = 1;
    temp->buffer->size = 4;

    swf_dump_shape(movie, error, temp);

    return;
}


void
swf_destroy_setbackgroundcolour (swf_setbackgroundcolour * tag)
{
    if (tag==NULL) {
        return;
    }
    swf_destroy_colour (tag->colour);
    swf_free (tag);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
