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

#include "swf_png.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <math.h>

#include <png.h>
#include <zlib.h>

#define NUMFRAMES 60
#define FRAMERATE 15

#define SWF_PNG_SIG_SIZE 8

void usage (char * name);

void 
usage (char * name) 
{
    fprintf (stderr, "Usage: %s <PNG filename>\n", name);
}

swf_tagrecord *
swf_read_png(char * filename, int * error) 
{
	FILE * fp;
	png_structp png_ptr;
	png_infop info_ptr, end_info;
	png_bytep *row_pointers;
	int i, rowbytes, depth;
	char png_header[SWF_PNG_SIG_SIZE];
	
	swf_png_data * png;
	swf_tagrecord * self;
	
	fp = fopen(filename, "rb");
	
	if (fp == NULL) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}
	
  /* Check sig */
	
	fread(png_header, 1, SWF_PNG_SIG_SIZE, fp);
	
	if(png_check_sig(png_header, SWF_PNG_SIG_SIZE)) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}
	
	/* Make png_ptr */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if(!png_ptr) {
		*error = SWF_EMallocFailure;
		return NULL;
	}
	
	/* Get info and other bits and bobs */
	
	info_ptr = png_create_info_struct(png_ptr);
	
	if(!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		*error = SWF_EMallocFailure;
		return NULL;
	}

	end_info = png_create_info_struct(png_ptr);
	
	if(!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		*error = SWF_EMallocFailure;
		return NULL;
	}
	
	if(setjmp(png_ptr->jmpbuf)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		*error = SWF_EFileReadError;
		return NULL;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	png = swf_png_make_swf_png(error);
	swf_png_get_IHDR(png, error, png_ptr, info_ptr);

	if (png->color_type == PNG_COLOR_TYPE_PALETTE) {
		swf_png_get_PLTE(png, error, png_ptr, info_ptr);
	}

	/* force bitdepth of 8 if necessary */
	
	depth = png->bit_depth;
	
	if(png->bit_depth < 8) {
		png_set_packing(png_ptr);
	}
	
	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);
	
	if(png->bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	
	if(png->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	
	if(png->color_type == PNG_COLOR_TYPE_RGB) {
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	}
	
  /* update info w/ the set transformations */
  png_read_update_info(png_ptr, info_ptr);

  swf_png_get_IHDR(png, error, png_ptr, info_ptr);

  png->channels = png_get_channels(png_ptr, info_ptr);


  if(png->color_type == PNG_COLOR_TYPE_GRAY) {
    /* turn it into a palettized image, use the gray values as indices to
       an rgb color table */

    int i;

    png->color_type = PNG_COLOR_TYPE_PALETTE;
    png->num_palette = 1 << depth;

    //    if ((m3 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
    //    }

	//    png->palette = malloc(sizeof(png_color) * png->num_palette);
    png->palette = calloc(png->num_palette, sizeof(png_color));

    for(i=0; i<(int)png->num_palette; ++i)
      png->palette[i].red = png->palette[i].green = png->palette[i].blue = 
	(i*255)/(png->num_palette - 1);
  }


  /* malloc stuff */
  row_pointers = malloc(png->height*sizeof(png_bytep));
  rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  png->data = malloc(png->height * rowbytes);

  for(i=0; i<png->height; ++i)
    row_pointers[i] = png->data + rowbytes*i;

  png_read_image(png_ptr, row_pointers);

  if(png->color_type == PNG_COLOR_TYPE_RGB_ALPHA
  || png->color_type == PNG_COLOR_TYPE_RGB) {
    /* alpha has to be pre-applied, bytes shifted */

    int x, y;
    unsigned char *p;
    int alpha;
    unsigned char r, g, b;

    for(y=0; y<png->height; ++y) {
      for(x=0; x<png->width; ++x) {
	p = png->data + rowbytes*y + 4*x;

        r = p[0];
        g = p[1];
        b = p[2];

	alpha = p[3];

	p[0] = alpha;
	p[1] = (char)((int)(r*alpha)>>8);
	p[2] = (char)((int)(g*alpha)>>8);
	p[3] = (char)((int)(b*alpha)>>8);
      }
    }
  }

  /* Got the PNG data. Now spin up a swf_tagrecord */

  self = swf_make_tagrecord(error, tagDefineBitsLossless);


  return NULL;
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
    
    // FIXME: Make a fresh header and calc it properly...
/* First, get a parser up, to steal a header */

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
    swf_print_header(hdr, &error);

    printf("\n----- Reading PNG file -----\n");

    temp = swf_read_png("test1.png", &error);

/* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

/* Ensure we import a good header... */

    movie->header = hdr;
    movie->name = (char *) "ben5.swf";

    movie->header->rate = FRAMERATE * 256;

    /* Do the frames */
    swf_add_setbackgroundcolour(movie, &error, 0, 255, 0, 255);


    //    swf_dump_shape(movie, &error, temp);

    m3->a  = m3->d  = 1 * 256 * 256;
    m3->b  = m3->c  = 0;
    m3->tx = 0 * 20;
    m3->ty = 0 * 20;


    swf_add_placeobject(movie, &error, m3, 1, 2);
    swf_add_showframe(movie, &error);
    swf_add_end(movie, &error);

    swf_make_finalise(movie, &error);

    swf_destroy_movie(movie);
    swf_destroy_parser(parser);
    swf_free(m3);
    
    fprintf (stderr, "OK\n");
    return 0;
}

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
