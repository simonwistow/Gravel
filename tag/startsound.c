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

swf_startsound *
swf_parse_startsound (swf_parser * context, int * error)
{
    int i = 0;
    swf_startsound * sound;

    if ((sound = (swf_startsound *) calloc (1, sizeof (swf_startsound))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    sound->tagid = swf_parse_get_word (context);

    if (sound->tagid) {
        sound->code =  swf_parse_get_byte (context);
		
        if ( sound->code & soundHasInPoint ) {
            sound->inpoint = swf_parse_get_dword (context);
        }

        if ( sound->code & soundHasOutPoint ) {
            sound->outpoint = swf_parse_get_dword (context);
        }

        if ( sound->code & soundHasLoops ) {
            sound->loops = swf_parse_get_dword (context);
        }

        sound->npoints = 0;

        if (  sound->code & soundHasEnvelope ) {
            sound->npoints = swf_parse_get_byte (context);
			
			if (sound->npoints) {
				if ((sound->points = (swf_soundpoint **) calloc (sound->npoints, sizeof (swf_soundpoint *))) == NULL) {
					*error = SWF_EMallocFailure;
					goto FAIL;
				}
			}
			
            for (i = 0; i < sound->npoints; i++ ) {
				sound->points[i] = NULL;
				if ((sound->points[i] = (swf_soundpoint *) calloc (1, sizeof (swf_soundpoint))) == NULL) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }

                sound->points[i]->mark = swf_parse_get_dword (context);
                sound->points[i]->lc   = swf_parse_get_dword (context);
                sound->points[i]->rc   = swf_parse_get_dword (context);
            }
        }
    }

    return sound;

 FAIL:
    swf_destroy_startsound (sound);
    return NULL;
}


void
swf_destroy_startsound (swf_startsound * sound)
{
    int i = 0;

    if (sound==NULL) {
        return;
    }


    for (i=0; i<sound->npoints; i++) {
        swf_destroy_soundpoint(sound->points[i]);
    }
    swf_free (sound->points);
    swf_free (sound);
	return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
