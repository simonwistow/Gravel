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

swf_definesound *
swf_parse_definesound (swf_parser * context, int * error)
{
    swf_definesound * sound;
    int n_samples_adpcm;


    if ((sound = (swf_definesound *) calloc (1, sizeof (swf_definesound))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    sound->tagid = swf_parse_get_word (context);

    sound->compression    = swf_parse_get_bits (context, 4);      /* uncompressed, ADPCM or MP3 */
    sound->sample_rate    = swf_parse_get_bits (context, 2);
    sound->sample_size    = swf_parse_get_bits (context, 1);
    sound->stereo_mono    = swf_parse_get_bits (context, 1);
    sound->sample_count   = swf_parse_get_dword (context);
    sound->adpcm          = NULL;
    sound->mp3header_list = NULL;



    switch (sound->compression)
    {
        case 1:
            n_samples_adpcm = 0;
            /*
            if ((sound->adpcm = swf_parse_adpcm_decompress(context, error, sound->sample_count, sound->stereo_mono, sound->sample_size, n_samples_adpcm)) == NULL)
            {
                goto FAIL;
            }
            */
            break;

        case 2:
            sound->delay = swf_parse_get_word(context);
            if ((sound->mp3header_list = swf_parse_get_mp3headers (context, error, sound->sample_count)) == NULL)
            {
                goto FAIL;
            }
            break;
    }

    return sound;

 FAIL:
    swf_destroy_definesound (sound);
    return NULL;
}


void
swf_destroy_definesound (swf_definesound * sound)
{

    if (sound==NULL) {
        return;
    }
    swf_destroy_mp3header_list (sound->mp3header_list);
    swf_destroy_adpcm (sound->adpcm);
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
