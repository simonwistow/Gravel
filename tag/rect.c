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
 * Setup a rectangle (ie a structure of type swf_rect) and
 * set its dimensions by reading the correct number
 * of bits from the stream.
 */

swf_rect *
swf_parse_get_rect (swf_parser * context, int * error)
{
    swf_rect * rect;
    int nbits;
	
    if ((rect = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    swf_parse_initbits(context);
    nbits = (int) swf_parse_get_bits(context, 5);
    rect->xmin = swf_parse_get_sbits(context, nbits);
    rect->xmax = swf_parse_get_sbits(context, nbits);
    rect->ymin = swf_parse_get_sbits(context, nbits);
    rect->ymax = swf_parse_get_sbits(context, nbits);

    return rect;
}


void
swf_destroy_rect (swf_rect * rect)
{
    if (rect==NULL) {
        return;
    }

    swf_free (rect);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
