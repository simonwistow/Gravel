/*
 * Copyright (C) 2002  Ben Evans <kitty@cpan.org>
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
 *
 */

swf_colour *
swf_parse_get_col (swf_parser * context, int * error, int with_alpha)
{
	swf_colour * mycol;

	if ((mycol = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
         }

    mycol->r = swf_parse_get_byte(context);
    mycol->g = swf_parse_get_byte(context);
    mycol->b = swf_parse_get_byte(context);
    mycol->a = 0xff;

    if (with_alpha) {
        mycol->a = swf_parse_get_byte(context);
    }

	return mycol;
}

void 
swf_buffer_colour(swf_buffer * buffer, int * error, swf_colour * col, int with_alpha) 
{
	swf_buffer_put_byte(buffer, error, col->r);
	swf_buffer_put_byte(buffer, error, col->g);
	swf_buffer_put_byte(buffer, error, col->b);


    if ( ( (col->a != 0xff) && (with_alpha == WITH_ALPHA) ) 
		 || (with_alpha == FORCE_ALPHA) ) {
		swf_buffer_put_byte(buffer, error, col->b);
    }
}






/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
