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

swf_jpegtables *
swf_parse_jpegtables (swf_parser * context, int * error)
{
    swf_jpegtables * tables;

    if ((tables = (swf_jpegtables *) calloc (1, sizeof (swf_jpegtables))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    if ((tables->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        swf_destroy_jpegtables (tables);
        return NULL;
    }

    return tables;
}


void
swf_destroy_jpegtables (swf_jpegtables * bits)
{
    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);


    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
