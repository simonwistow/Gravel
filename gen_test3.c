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


swf_tagrecord *
swf_get_nth_bitmap (swf_parser * swf, int * error, int which_shape) 
{
    int next_id, i, done;
    swf_tagrecord * temp = NULL;
	

    if (*error != SWF_ENoError) 
    {
	fprintf(stderr,"error wasn't reset in get_nth_shape\n");
	return NULL;
    } 
    *error = SWF_ENoError;            




    i = done = 0;
    
    /* parse all the tags, looking for a defineshape  */
    do {
        /* reset the error, just to be paranoid */
        *error = SWF_ENoError;

        /* get the next id */
        next_id = swf_parse_nextid(swf, error);

        /* if there's been an error, bug out */
        if (*error != SWF_ENoError) {
	    goto FAIL;
	}

        /* Use default fall-through, just for perversity */

        switch (next_id) {
	    case tagDefineBitsLossless:
	    case tagDefineBitsLossless2:
		if (i  == which_shape ) {
		    temp = swf_get_raw_shape(swf, error);

		    if (temp == NULL || *error != SWF_ENoError)
		    {
			goto FAIL;
		    }

		    temp->id = next_id;
		    temp->serialised = TRUE;
		    done = 1;
		}
		i++;
		
		break;
        }
    } while ((!*error) && next_id && !done);

    if (i<=which_shape)
    {
	*error = SWF_ENoSuchShape;
	goto FAIL;
    } 

    return temp;

FAIL:
    swf_destroy_tagrecord(temp);
    return NULL;

}


int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error = SWF_ENoError;
    int shape_num;
    swf_parser * parser;
    swf_header * hdr;
    swf_tagrecord * temp;
    SWF_U16 obj_id;
    swf_matrix *matrix, *m2, *m3;
    swf_cxform *mycx, *cx2;
    int i;
    char * myname;
    
   if (argc<2)
   {
	usage(argv[0]);
	exit (1);
   }

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

    temp = swf_get_nth_bitmap(parser, &error, shape_num);

 
    if (error == SWF_ENoSuchShape) {
      fprintf(stderr,"No such shape : %d\n",shape_num);
      exit(1);
    } else if (error != SWF_ENoError || temp == NULL) {
      fprintf(stderr,"Error getting %dth shape : %s\n", shape_num, swf_error_code_to_string(error));
      exit(1);
    } 

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

    for (i=1; i<=NUMFRAMES; i++) {
      swf_add_placeobject2(movie, &error, matrix, obj_id, i, mycx, NULL);
      swf_add_placeobject2(movie, &error, m2, obj_id, 1, cx2, myname);
      swf_add_placeobject(movie, &error, m3, 14, 2);

      if (30 == i) {
//	swf_add_doaction(movie, &error, sactionGotoFrame);
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
    swf_destroy_parser(parser);

    fprintf (stderr, "OK\n");
    return 0;
}






