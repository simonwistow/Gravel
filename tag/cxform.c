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
 *
 */

swf_cxform *
swf_parse_get_cxform (swf_parser * context, int * error, int has_alpha)
{
    swf_cxform * cxform;

    int need_add, need_mul, n_bits;

    if ((cxform = (swf_cxform *) calloc (1, sizeof (swf_cxform))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    swf_parse_initbits (context);

    /* !!! The spec has these bits reversed !!! */
    need_add = swf_parse_get_bits (context, 1);
    need_mul = swf_parse_get_bits (context, 1);
    /* !!! The spec has these bits reversed !!! */


    n_bits = (int) swf_parse_get_bits(context, 4);

    cxform->aa = 256;
    cxform->ab = 0;

    if (need_mul) {
		cxform->ra = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		cxform->ga = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		cxform->ba = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		if (has_alpha) {
			cxform->aa = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		}
    } else {
		cxform->ra = cxform->ga = cxform->ba = 256;
    }

    if (need_add) {
		cxform->rb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		cxform->gb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		cxform->bb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
		if (has_alpha) {
			cxform->ab = (SWF_S16)swf_parse_get_sbits (context, n_bits);
		}
    } else {
		cxform->rb = cxform->gb = cxform->bb = 0;
    }

    return cxform;
}


void
swf_destroy_cxform (swf_cxform * cxform)
{
    if (cxform==NULL) {
        return;
    }

    swf_free (cxform);

    return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
