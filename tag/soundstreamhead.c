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

swf_soundstreamhead *
swf_parse_soundstreamhead (swf_parser * context, int * error)
{
    swf_soundstreamhead * head;
    if ((head = (swf_soundstreamhead *) calloc (1, sizeof(swf_soundstreamhead))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    head->mix_format = swf_parse_get_byte (context);

    /* The stream settings */
    head->stream_compression         = context->stream_compression = swf_parse_get_bits (context, 4);
    head->stream_sample_rate         = context->stream_sample_rate = swf_parse_get_bits (context, 2);
    head->stream_sample_size         = context->stream_sample_size = swf_parse_get_bits (context, 1);
    head->stream_sample_stereo_mono  = context->stream_sample_stereo_mono = swf_parse_get_bits (context, 1);
    head->n_stream_samples           = context->n_stream_samples = swf_parse_get_word (context);

    return head;
}


void
swf_destroy_soundstreamhead (swf_soundstreamhead * head)
{
    if (head==NULL) {
        return;
    }

    swf_free (head);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
