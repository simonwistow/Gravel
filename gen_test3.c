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
    swf_matrix *matrix, *m2, *m3;
    swf_cxform *mycx, *cx2;
    int i;
    char * myname;
    

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
    printf("Xmin: \t%lu\n", (hdr->bounds->xmin / 20));
    printf("Xmax: \t%lu\n", (hdr->bounds->xmax / 20));
    printf("Ymin: \t%lu\n", (hdr->bounds->ymin / 20));
    printf("Ymax: \t%lu\n", (hdr->bounds->ymax / 20));
    printf("Frame rate \t%"pSWF_U32"\n", hdr->rate);
    printf("Frame count \t%"pSWF_U32"\n", hdr->count);

    printf("\n----- Reading movie details -----\n");

/* Right, now we need a tagrecord.. */

    temp = swf_make_tagrecord(&error, 0);

    if (error) {
	exit(1);
    }

    swf_get_nth_shape(parser, &error, shape_num, temp);
    obj_id = swf_get_object_id(temp, &error);

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((mycx = (swf_cxform *) calloc (1, sizeof (swf_cxform))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((m2 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((m3 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((cx2 = (swf_cxform *) calloc (1, sizeof (swf_cxform))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }

    mycx->ra = 0;
    mycx->ga = 0;
    mycx->ba = 0;

    mycx->aa = 0;
    mycx->ab = 0;

    cx2->ra = 0;
    cx2->ga = 0;
    cx2->ba = 0;

    cx2->aa = 0;
    cx2->ab = 0;


/* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

/* Ensure we import a good header... */

    movie->header = hdr;
    movie->name = (char *) "ben3.swf";

    movie->header->rate = FRAMERATE * 256;

    swf_add_setbackgroundcolour(movie, &error, 0, 255, 0, 255);
    swf_dump_shape(movie, &error, temp);
    swf_add_definebutton(movie, &error, 14, obj_id);
      
    /* Do the frames */

    myname = (char *) "pigdog";

    matrix->a  = matrix->d  = 256 * 256;
    matrix->b  = matrix->c  = 0;
    matrix->tx = 0 * 20;
    matrix->ty = 0 * 20;

    mycx->rb = 0;
    mycx->gb = 0;
    mycx->bb = 0;

    m2->a  = m2->d  = 2 * 256 * 256;
    m2->b  = m2->c  = 0;
    m2->tx = 100 * 20;
    m2->ty = 0 * 20;

    mycx->rb = 0;
    mycx->gb = 0;
    mycx->bb = 0;


    m3->a  = m3->d  = 1 * 256 * 256;
    m3->b  = m3->c  = 0;
    m3->tx = 100 * 20;
    m3->ty = 100 * 20;

    fprintf(stderr, "foo A\n");

    for (i=1; i<=NUMFRAMES; i++) {
      fprintf(stderr, "foo B\n");
      fprintf(stderr, "foo C\n");
      swf_add_placeobject2(movie, &error, matrix, obj_id, i, mycx, NULL);
      swf_add_placeobject2(movie, &error, m2, obj_id, 1, cx2, myname);
      swf_add_placeobject(movie, &error, m3, 14, 2);

      if (30 == i) {
	swf_add_doaction(movie, &error, sactionGotoFrame);
      }
      if (2 == i) {
	swf_add_doaction(movie, &error, sactionPlay);
      }

      swf_add_showframe(movie, &error);

      if (i < NUMFRAMES) {
	swf_add_removeobject2(movie, &error, i);
	swf_add_removeobject2(movie, &error, 1);
	swf_add_removeobject(movie, &error, 14, 2);
      }
      matrix->tx += 5 * 20;
      matrix->a += 4 * 256;
      matrix->d += 4 * 256;
      mycx->bb += 5;

      m2->ty += 2 * 20;
      cx2->gb += 4;
    }

    swf_add_end(movie, &error);

    swf_make_finalise(movie, &error);

    swf_free(cx2);
    swf_free(m3);
    swf_free(m2);
    swf_free(mycx);
    swf_free(matrix);
    swf_destroy_movie(movie);


    fprintf (stderr, "OK\n");
    return 0;
}






