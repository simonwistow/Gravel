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
 *
 */

/* Test the background colour test flag */

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_serialise.h"
#include "swf_destroy.h"

#include <stdio.h>

#define NUMFRAMES 60
#define FRAMERATE 15

void usage (char * name);

void 
usage (char * name) 
{
    fprintf (stderr, "Usage: %s <filename> <object number>\n", name);
}


SWF_U16 swf_get_object_id(swf_tagrecord * mytag, int * error);

SWF_U16 
swf_get_object_id(swf_tagrecord * mytag, int * error)
{
    return (((SWF_U16) mytag->buffer->raw[1]) << 8) | (SWF_U16) mytag->buffer->raw[0];
}

int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error = SWF_ENoError;
    swf_parser * parser;
    swf_header * hdr;
    swf_tagrecord * temp;
    swf_matrix * m3;

    if ((m3 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    
/* First, get a parser up */

    parser = swf_parse_create("swfs/ibm.swf", &error);

    if (parser == NULL) {
	fprintf (stderr, "Failed to create SWF context\n");
	return -1;
    }
    printf ("Name of file is '%s'\n", parser->name);


    printf("----- Reading the file input header -----\n");
    hdr = swf_parse_header(parser, &error);

    if (hdr == NULL) {
        fprintf (stderr, "Failed to parse headers\n");
        exit(1);
    }

    printf("FWS\n");
    printf("File version \t%"pSWF_U32"\n", hdr->version);
    printf("File size \t%"pSWF_U32"\n", hdr->size);
    printf("Movie width \t%lu\n", (hdr->bounds->xmax - hdr->bounds->xmin) / 20);
    printf("Movie height \t%lu\n", (hdr->bounds->ymax - hdr->bounds->ymin) / 20);
    printf("Xmin: \t%lu\n", (hdr->bounds->xmin / 20));
    printf("Xmax: \t%lu\n", (hdr->bounds->xmax / 20));
    printf("Ymin: \t%lu\n", (hdr->bounds->ymin / 20));
    printf("Ymax: \t%lu\n", (hdr->bounds->ymax / 20));
    printf("Frame rate \t%"pSWF_U32"\n", hdr->rate);
    printf("Frame count \t%"pSWF_U32"\n", hdr->count);

    printf("\n----- Reading movie details -----\n");



/* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

    /* Right, now we need a tagrecord.. */
    temp = swf_make_triangle(movie, &error);

    /* Need to calloc a (raw) buffer for temp... */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
	fprintf (stderr, "Calloc Fail\n");
        return 1;
    }

    printf("foo 1\n");
    swf_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
    printf("foo 2\n");
    temp->serialised = 1;


/* Ensure we import a good header... */

    movie->header = hdr;
    movie->name = (char *) "ben4.swf";

    movie->header->rate = FRAMERATE * 256;

    /* Do the frames */
    swf_add_setbackgroundcolour(movie, &error, 0, 255, 0, 255);
    swf_dump_shape(movie, &error, temp);

    m3->a  = m3->d  = 1 * 256 * 256;
    m3->b  = m3->c  = 0;
    m3->tx = 100 * 20;
    m3->ty = 100 * 20;


    swf_add_placeobject(movie, &error, m3, 1, 2);
    swf_add_showframe(movie, &error);
    swf_add_end(movie, &error);

    swf_make_finalise(movie, &error);

    printf("foo 3\n");

    swf_destroy_movie(movie);
    printf("foo 4\n");
    swf_destroy_parser(parser);

    swf_free(m3);

    fprintf (stderr, "OK\n");
    return 0;
}






