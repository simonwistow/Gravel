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

void usage (char * name);

void 
usage (char * name) 
{
    fprintf (stderr, "%s <filename>\n", name);
}


void  swf_dump_shape (swf_movie * movie, int * error, swf_tagrecord * temp);

void 
swf_dump_shape (swf_movie * movie, int * error, swf_tagrecord * temp) 
{
    *(movie->lastp) = temp;
    movie->lastp = &(temp->next);
}

SWF_U16 swf_get_object_id(swf_tagrecord * mytag, int * error);

SWF_U16 
swf_get_object_id(swf_tagrecord * mytag, int * error)
{
    return (((SWF_U16) mytag->buffer->raw[1]) << 8) | (SWF_U16) mytag->buffer->raw[0];
}

int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error = 0;
    int shape_num;
    swf_parser * parser;
    swf_header * hdr;
    swf_tagrecord * temp;
    SWF_U16 obj_id;
    swf_matrix * matrix;

/* First, get a parser up */

    parser = swf_parse_create(argv[1], &error);
    shape_num = atoi(argv[2]);

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
    printf("Frame rate \t%"pSWF_U32"\n", hdr->rate);
    printf("Frame count \t%"pSWF_U32"\n", hdr->count);

    printf("\n----- Reading movie details -----\n");

/* Right, now we need a tagrecord.. */

    temp = swf_make_tagrecord(&error);

    if (error) {
	exit(1);
    }

    temp->next = NULL;
    temp->id = 0;
    temp->tag = NULL;
    temp->serialised = 0;


    swf_get_nth_shape(parser, &error, shape_num, temp);
    obj_id = swf_get_object_id(temp, &error);

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }

/* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

/* Ensure we steal a good header... */

    movie->header = hdr;
    movie->name = (char *) "ben3.swf";

    swf_add_setbackgroundcolour(movie, &error, 0, 255, 0, 255);
    swf_dump_shape(movie, &error, temp);
    printf("foo 5\n");

    printf("foo A\n");

    matrix->a  = matrix->c  = 512 * 100;
    matrix->b  = matrix->d  = 0;
    matrix->tx = 300 * 20;
    matrix->ty = 150 * 20;

    swf_add_placeobject(movie, &error, obj_id, matrix);
    printf("foo 6\n");
    swf_add_showframe(movie, &error);
    matrix->a  = matrix->c  = 512 * 100;
    matrix->b  = matrix->d  = 0;
    matrix->tx = 0;
    matrix->ty = 150 * 20;

    swf_add_placeobject(movie, &error, obj_id, matrix);
    swf_add_showframe(movie, &error);
    swf_add_end(movie, &error);

    swf_make_finalise(movie, &error);
    printf("foo 7\n");

    swf_free(matrix);
    swf_destroy_movie(movie);

    fprintf (stderr, "OK\n");
    return 0;
}









