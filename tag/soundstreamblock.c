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

swf_soundstreamblock *
swf_parse_soundstreamblock (swf_parser * context, int * error)
{
    swf_soundstreamblock * block;
	
    if ((block = (swf_soundstreamblock *) calloc (1, sizeof (swf_soundstreamblock))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    block->mp3header_list = NULL;
    block->adpcm          = NULL;

    switch (context->stream_compression)
    {
	case 1:
		block->n_stream_samples          = context->n_stream_samples;
		block->stream_sample_stereo_mono = context->stream_sample_stereo_mono;
		block->stream_sample_size        = context->stream_sample_size;
		block->n_samples_adpcm           = context->n_samples_adpcm = 0;
		if ((block->adpcm                     = swf_parse_adpcm_decompress(context, error, block->n_stream_samples, block->stream_sample_stereo_mono, block->stream_sample_size, block->n_samples_adpcm)) == NULL)
		{
			goto FAIL;
		}
		break;

	case 2:
		block->samples_per_frame  = swf_parse_get_word (context);
		block->delay              = swf_parse_get_word (context);
		if ((block->mp3header_list     = swf_parse_get_mp3headers (context, error, block->samples_per_frame)) == NULL)
		{
			goto FAIL;
		}
		break;
	
	default:
		// todo simon?
		goto FAIL;
    }

    return block;

 FAIL:
    swf_destroy_soundstreamblock (block);
    return NULL;

}


void
swf_destroy_soundstreamblock (swf_soundstreamblock * block)
{
    if (block==NULL) {
        return;
    }

    swf_destroy_adpcm (block->adpcm);
    swf_destroy_mp3header_list (block->mp3header_list);
    swf_free (block);
	return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
