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
			break;
			if ((sound->adpcm = swf_parse_adpcm_decompress(context, error, sound->sample_count, sound->stereo_mono, sound->sample_size, n_samples_adpcm)) == NULL)
            {
				goto FAIL;
            }
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



/* MPEG tables */
const int swf_MPEG_VerTab[4]={2,3,1,0};
const int swf_MPEG_FreqTab[4]={44100,48000,32000};
const int swf_MPEG_RateTab[2][3][16]=
{
	{
		{  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,  0},
		{  0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,  0},
		{  0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,  0},
	},
	{
		{  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,  0},
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
		{  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
	},
};
//todo : since these are exported do they need to be declared final?
const char * swf_MPEG_Ver[4] = {"1","2","2.5","3?"};



swf_mp3header_list *
swf_parse_get_mp3headers (swf_parser * context, int * error, int samples_per_frame)
{

    int frame_count = 0;
    int header_store_size  = 0;
    swf_mp3header_list * header_list;
    SWF_U8 hdr[4];
    int i;


    if ((header_list = (swf_mp3header_list *) calloc (1, sizeof (swf_mp3header_list))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    if (samples_per_frame > 0)
    {
        while (TRUE)
        {
            if (header_store_size==frame_count)
            {
                header_store_size += 10;
                if (header_list->headers) {
					header_list->headers = (swf_mp3header **) realloc (header_list->headers, sizeof(swf_mp3header *) * header_store_size);
				}
				else {
					header_list->headers = (swf_mp3header **) calloc (header_store_size, sizeof(swf_mp3header *));
				}
                if (!header_list->headers) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }
            }
			
            header_list->headers[frame_count] = NULL;
            if ((header_list->headers[frame_count] = (swf_mp3header *) calloc (1, sizeof(swf_mp3header))) == NULL)
            {
                *error = SWF_EMallocFailure;
                goto FAIL;
            }

            /* Get the MP3 frame header */
            for (i=0; i<4; i++) {
                hdr[i] = swf_parse_get_byte (context);
            }

            /* Decode the MP3 frame header */
            header_list->headers[frame_count]->ver     = swf_MPEG_VerTab[((hdr[1] >> 3) & 3)];
            header_list->headers[frame_count]->layer   = 3 - ((hdr[1] >> 1) & 3);
            header_list->headers[frame_count]->pad     = (hdr[2] >>1 ) & 1;
            header_list->headers[frame_count]->stereo  = ((hdr[3] >> 6) & 3) != 3;
            header_list->headers[frame_count]->freq    = 0;
            header_list->headers[frame_count]->rate    = 0;

            if (hdr[0] != 0xFF || hdr[1] < 0xE0 || header_list->headers[frame_count]->ver==3 || header_list->headers[frame_count]->layer != 2)
            {
                *error = SWF_EInvalidMP3Header;
                goto FAIL;
            }
            else
            {
                header_list->headers[frame_count]->freq = swf_MPEG_FreqTab[(hdr[2] >>2 ) & 3] >> header_list->headers[frame_count]->ver;
                header_list->headers[frame_count]->rate = swf_MPEG_RateTab[header_list->headers[frame_count]->ver ? 1 : 0][header_list->headers[frame_count]->layer][(hdr[2] >> 4) & 15] * 1000;

                if (!header_list->headers[frame_count]->freq || !header_list->headers[frame_count]->rate)
                {
                    *error = SWF_EInvalidMP3Frame;
                    goto FAIL;
                }
            }

            /* Get the size of a decoded MP3 frame */
            header_list->headers[frame_count]->decoded_frame_size = (576 * (header_list->headers[frame_count]->stereo + 1));
            if (!header_list->headers[frame_count]->ver) {
                header_list->headers[frame_count]->decoded_frame_size  *= 2;
            }

            /* Get the size of this encoded MP3 frame */
            header_list->headers[frame_count]->encoded_frame_size  = ((header_list->headers[frame_count]->ver ? 72 : 144) * header_list->headers[frame_count]->rate) / header_list->headers[frame_count]->freq + header_list->headers[frame_count]->pad - 4;

            /* Decode the MP3 frame */
            header_list->headers[frame_count]->data = swf_parse_get_bytes(context, header_list->headers[frame_count]->encoded_frame_size);

            /* Move to the next frame */
            if (swf_parse_tell (context) >= context->tagend) {
                break;
            }
            frame_count++;

        }
    }

    header_list->header_count = ++frame_count;
    return header_list;

 FAIL:
    swf_destroy_mp3header_list (header_list);
    return NULL;
}

void
swf_destroy_mp3header (swf_mp3header * header)
{
    if (header==NULL) {
        return;
    }
    swf_free (header->data);
    swf_free (header);

    return;
}


swf_adpcm *
swf_parse_adpcm_decompress (swf_parser * context, int * error, int count, int stereo_mono, int size, int nsamples)
{
    // todo Simon
    return NULL;
}


void
swf_destroy_adpcm (swf_adpcm * adpcm)
{
    if (adpcm==NULL) {
        return;
    }
    swf_free (adpcm);
    return;
}


void
swf_destroy_mp3header_list (swf_mp3header_list * list)
{
    int i=0;

    if (list==NULL) {
        return;
    }

    for (i=0; i<list->header_count; i++) {
        swf_destroy_mp3header(list->headers[i]);
    }
    swf_free (list->headers);
    swf_free (list);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
