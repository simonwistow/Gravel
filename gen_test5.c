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
#include "swf_buffer.h"
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
#define STD_8_BIT_FORMAT 3

#define CHAR_ID 2

int verbose = 0;

void usage ();

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
	
	/*	if(png_check_sig(png_header, SWF_PNG_SIG_SIZE)) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}
	*/
	
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
		/* turn it into a palettized image, use the gray 
		   values as indices to an rgb color table */

		int i;
		
		png->color_type = PNG_COLOR_TYPE_PALETTE;
		png->num_palette = 1 << depth;
		
		//    if ((m3 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
		//    }

		//    png->palette = malloc(sizeof(png_color) * png->num_palette);
		png->palette = calloc(png->num_palette, sizeof(png_color));

		for(i=0; i<(int)png->num_palette; ++i) {
			png->palette[i].red = png->palette[i].green = 
				png->palette[i].blue =  (i*255)/(png->num_palette - 1);
		}
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
	self->buffer = swf_write_dbl(error, png, 1);

	// FIXME: Expose the bitmap_id properly
	if (NULL == self->buffer) {
		*error = SWF_EFileWriteError;
		return NULL;
	}

	self->serialised = 1;

	return self;
}


swf_buffer *
swf_write_dbl(int * error, swf_png_data * png, SWF_U16 bitmap_id)
{
	SWF_U8 *data, *outdata;
	int size;
	long outsize;
	int tablesize;
	swf_buffer * self;
	
    if ((self = (swf_buffer *) calloc (1, sizeof (swf_buffer)) ) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }	

	if(png->bit_depth != 8) {
		*error = SWF_ENotValidSWF;
		return NULL;
	}

	size = png->width * png->height * png->channels;

	if(png->color_type == PNG_COLOR_TYPE_PALETTE) {
		tablesize = png->num_palette * sizeof(png_color);
		
		data = realloc(png->palette, tablesize + size);
		
		memcpy(data + tablesize, png->data, size);
		size += tablesize;
	} else {
		data = png->data;
	}

	outsize = (int)floor(size*1.01+12);
    if ((outdata = (SWF_U8 *) calloc(1, outsize)) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

	/* compress the RGB color table (if present) and image data one block */

	compress2(outdata, &outsize, data, size, 9);

    if ((self->raw = (SWF_U8 *) calloc (1, 100 + outsize) ) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }	
	
	swf_buffer_put_word(self, error, bitmap_id);

	//	swf_buffer_put_dword(self, error, outsize + 6);

	swf_buffer_put_byte(self, error, STD_8_BIT_FORMAT);
	swf_buffer_put_word(self, error, png->width);
	swf_buffer_put_word(self, error, png->height);

	fprintf(stderr, "width: %i height: %i size: %i palette: %i\n", png->width, png->height, outsize, png->num_palette - 1);

	swf_buffer_put_byte(self, error, png->num_palette - 1);
	swf_buffer_put_bytes(self, error, outsize, outdata);

	return self;
}


int main2 (int argc, char *argv[]) {
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

    temp = swf_read_png("pngs/png1.png", &error);

	if (temp == NULL) {
		fprintf(stderr, "Failed to read PNG: %i\n", error);
	}

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
    swf_add_setbackgroundcolour_noalpha(movie, &error, 192, 255, 0);

    swf_dump_shape(movie, &error, temp);

    m3->a  = m3->d  = 1 * 256 * 256;
    m3->b  = m3->c  = 0;
    m3->tx = 10 * 20;
    m3->ty = 10 * 20;

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

swf_tagrecord *
swf_read_dbl(char * filename, int * error) 
{
	FILE * fp;
	swf_tagrecord * self;
	SWF_U8 dbl_h[8];
	SWF_U8 * temp;
	SWF_U32 size;

	fp = fopen(filename, "rb");
	
	if (fp == NULL) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}

	fread(dbl_h, 1, 8, fp);

	size = ((SWF_U32)dbl_h[4] << 24) | ((SWF_U32)dbl_h[5] << 16) | ((SWF_U32)dbl_h[6] << 8) | dbl_h[7];

	fprintf(stderr, "Size == %lu\n", size);

	fprintf(stderr, "Header = %i %i %i %i %i %i %i %i\n", 
			dbl_h[0], dbl_h[1], dbl_h[2], dbl_h[3],
			dbl_h[4], dbl_h[5], dbl_h[6], dbl_h[7]);

	self = swf_make_tagrecord(error, tagDefineBitsLossless);

    if ((temp = (SWF_U8 *) calloc (1, 100 + size) ) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }
    if ((self->buffer->raw = (SWF_U8 *) calloc (1, 100 + size) ) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }	

	fread(temp, 1, size, fp);
	swf_buffer_put_word(self->buffer, error, CHAR_ID);
	swf_buffer_put_bytes(self->buffer, error, size, temp);

	self->serialised = 1;

	swf_free(temp);
	return self;
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

    temp = swf_read_dbl("../png/png1.dbl", &error);

	if (temp == NULL) {
		fprintf(stderr, "Failed to read DBL: %i\n", error);
	}

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
    swf_add_setbackgroundcolour_noalpha(movie, &error, 192, 255, 0);

    swf_dump_shape(movie, &error, temp);

    m3->a  = m3->d  = 1 * 256 * 256;
    m3->b  = m3->c  = 0;
    m3->tx = 10 * 20;
    m3->ty = 10 * 20;

    swf_add_placeobject(movie, &error, m3, CHAR_ID, 1);
    swf_add_showframe(movie, &error);
    swf_add_placeobject(movie, &error, m3, CHAR_ID, 1);
    swf_add_showframe(movie, &error);
    swf_add_placeobject(movie, &error, m3, CHAR_ID, 1);
    swf_add_showframe(movie, &error);
    swf_add_placeobject(movie, &error, m3, CHAR_ID, 1);
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
