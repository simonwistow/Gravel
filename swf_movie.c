/*
 * Copyright (C) 2001  Ben Evans <kitty@fuckedgoths.com>
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


#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_destroy.h"

#include <stdio.h>

void swf_make_header_blank (swf_movie * movie, int * error, int ver, int speed) {
    swf_header * header;

    if ((ver < 0) || (ver > MAXVER) || (speed < 0) || (speed > MAXSPEED) ) {
	*error = SWF_EMallocFailure;
	return;
    }

    if (0 == ver) {
	ver = MAXVER;
    }

    if ((header = (swf_header *) calloc (1, sizeof (swf_header))) == NULL) {
	*error = SWF_EMallocFailure;
	return;
    }

/* 
 * Hardwired, because this is an illegal size to indicate
 * this movie needs to be finalised - ie have its frames
 * counted, size checked, etc., before
 * serialisation 
 */
    header->size = 20; 


    header->version = ver; 
    header->rate = speed; 
    header->count = 1;
    header->bounds = NULL;

    movie->header = header;

    return;
}


void swf_make_header_raw (swf_movie * movie, int * error, swf_rect * rect) {
    swf_header * header;

    header = movie->header;

    if (!header) {
	swf_make_header_blank(movie, error, MAXVER, TYPICALSPEED);
	header = movie->header;
    }

    /* Throw the error further up the stack */
    if (*error) {
	return;
    }

    header->bounds = rect;

    return;
}

void swf_make_header (swf_movie * movie, int * error, SCOORD x1, SCOORD x2, SCOORD y1, SCOORD y2) {
    swf_rect * rect;

    if ((rect = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
	*error = SWF_EMallocFailure;
	return;
    }

    rect->xmin = x1;
    rect->xmax = x2;
    rect->ymin = y1;
    rect->ymax = y2;

    swf_make_header_raw(movie, error, rect);

    return;
}

swf_movie * swf_make_movie (int * error) {
    swf_movie * movie;

    if ((movie = (swf_movie *) calloc (1, sizeof (swf_movie))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    movie->header = NULL;

    movie->max_obj_id = 0;

    movie->first = NULL;
    movie->lastp = &(movie->first);

    return movie;
}



void swf_destroy_tagrecord (swf_tagrecord * tagrec) {
    void * tmp;
    int tagid;

    tagid = tagrec->id;
    tmp = tagrec->tag;

    switch (tagid) {
	case tagEnd :
//	    swf_destroy_tag_end((swf_tag_end *) tmp);
	    break;
	    
	case tagShowFrame:
//	    swf_destroy_tag_showframe((swf_tag_showframe *) tmp);
	    break;

	case tagDefineShape:
	    swf_destroy_defineshape((swf_defineshape *) tmp);
	    break;

	    
	case tagSetBackgroundColour:
//	    swf_destroy_tag_setbackgroundcolour((swf_tag_setbackgroundcolour *) tmp);
	    break;

	default:
	    printf ("Ooook!\n");
	    break;
    }

    swf_free(tagrec);

    return;
}




void swf_destroy_movie (swf_movie * movie) {
    swf_header * header;
    swf_tagrecord *tmp, *node;

    header = movie->header;
    swf_destroy_header(header);

    *(movie->lastp) = NULL;
    node = movie->first;

    while (node != NULL) {
        tmp = node;
        node = node->next;

	swf_destroy_tagrecord(tmp);
    }

    swf_free(movie);

    return;
}

swf_fillstyle * swf_make_fillstyle(int * error) {
    swf_fillstyle * mystyle;

    if ((mystyle = (swf_fillstyle *) calloc (1, sizeof (swf_fillstyle))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    mystyle->fill_style = 0; /* Just do solid fill for now */
    mystyle->ncolours = 0; /* Just do solid fill for now */
    mystyle->bitmap_id = 0; /* Just do solid fill for now */
    mystyle->matrix = NULL; /* Just do solid fill for now */
    mystyle->colours = &(mystyle->matrix); /* Just do solid fill for now */

    mystyle->colour = 0; /* Black shape */

    return mystyle;
}

swf_linestyle * swf_make_linestyle(int * error) {
    swf_linestyle * mystyle;

    if ((mystyle = (swf_linestyle *) calloc (1, sizeof (swf_linestyle))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    mystyle->colour = 0; /* Black shape */
    mystyle->width = 2; /* Thin lines */

    return mystyle;
}


swf_shaperecord_list * swf_make_shaperecords_for_triangle(int * error) {
    swf_shaperecord_list * list;

    if ((list = (swf_shaperecord_list *) calloc (1, sizeof (swf_shaperecord_list))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    list->record_count = 0;

/* First, we need a non-edge, change of linestyle record, to select a linestyle from our array */ 

/* UB[1] = 0, UB[1] = 0, UB[1] = 1, UB[1] = 0, UB[1] = 0, UB[1] = 0 */

/*    00100000 == 32 == 0x20 */

/* Then we need a non-edge, change of fillstyle record. */ 

/* Then we need three edges */

/* Then we need an end-of-shape edge */

//    list->first) = ;

    *(list->lastp) = NULL;

    return list;
}


void swf_get_raw_shape (swf_parser * swf, int * error, swf_tagrecord * mybuffer) {
    SWF_U32 startpos;
    int length;    

    startpos = length = 0;

    startpos = swf->filepos;
    length = swf->cur_tag_len;

/* CHECKME: Do we need this ? */
    swf_parse_seek(swf, startpos);
    
    mybuffer->buffer = swf_parse_get_bytes(swf, length);
    mybuffer->size = length;
}


void swf_get_nth_shape (swf_parser * swf, int * error, int which_shape, swf_tagrecord * mybuffer) {
/*
 * Get a raw shape from the define shape frames.
 */

    int next_id, i, done;

    i = done = 0;
    
    /* parse all the tags, looking for a defineshape  */
    do {
        /* reset the error, just to be paranoid */
        *error = SWF_ENoError;
	
        /* get the next id */
        next_id = swf_parse_nextid(swf, error);

        /* if there's been an error, bug out */
        if (*error != SWF_ENoError) {
			return;
	}

        switch (next_id) {
	    case tagDefineShape:
		if (i  == which_shape ) {
		    swf_get_raw_shape(swf, error, mybuffer);
		    mybuffer->id = next_id;
		    mybuffer->serialised = TRUE;
		    done = 1;
		}
		i++;
		break;
		
	    case tagDefineShape2:
		if (i  == which_shape ) {
		    swf_get_raw_shape(swf, error, mybuffer);
		    mybuffer->id = next_id;
		    mybuffer->serialised = TRUE;
		    done = 1;
		}
		i++;
		break;
				
	    case tagDefineShape3:
		if (i  == which_shape ) {
		    swf_get_raw_shape(swf, error, mybuffer);
		    mybuffer->id = next_id;
		    mybuffer->serialised = TRUE;
		    done = 1;
		}
		i++;
		
		break;
        }
    } while ((!*error) && next_id && !done);

    return;
}



/* To make a triangle, we need
   A shaperecord, comprising...
 * Fillstyle (just one for now)
 * Linestyle (just one for now)
 * three edges (straight lines)
 */
swf_tagrecord * swf_make_triangle_as_tag(swf_movie * movie, int * error) {
    swf_tagrecord * triangle;
    swf_defineshape * shape;
    swf_rect * canvas;
    swf_shapestyle * mystyle;

    if ((triangle = (swf_tagrecord *) calloc (1, sizeof (swf_tagrecord))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    if ((shape = (swf_defineshape *) calloc (1, sizeof (swf_defineshape))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    if ((canvas = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    if ((mystyle = (swf_shapestyle *) calloc (1, sizeof (swf_shapestyle))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    shape->tagid = ++(movie->max_obj_id);

    canvas->xmin = -20;
    canvas->xmax = 20;
    canvas->ymin = -20;
    canvas->ymax = 20;

    shape->rect = canvas;

    mystyle->nfills = 1;
    mystyle->nlines = 1;

    *(mystyle->fills) = swf_make_fillstyle(error);
    *(mystyle->lines) = swf_make_linestyle(error);

    shape->style = mystyle;

/* TODO: implement this shit... */


    shape->record = swf_make_shaperecords_for_triangle(error);

    triangle->id = tagDefineShape;
    triangle->next = NULL;

    triangle->tag = (void *) shape;

    return triangle;

 FAIL:
    swf_destroy_tagrecord(triangle);
    *error = SWF_EMallocFailure;
    return NULL;
}

/* Calculate size et all, and stream out */

void swf_make_finalise(swf_movie * movie, int * error) {

/*    file_hdr[0] = 'F';
    file_hdr[1] = 'W';
    file_hdr[2] = 'S'; */
/* Reminder to fix these up in finalise, before write out */



}


void swf_movie_make_translation_matrix(swf_movie * movie, int * error, SWF_U8 x, SWF_U8 y) {
    SWF_U8 fix, b1, b2, b3;
    SWF_U32 pad;

    fix = 8; /* Code for a pure translate matrix */
    pad = fix << 17 | x << 9 | y << 1;
    b3 = pad & 0xff;
    b2 = (pad >> 8) & 0xff;
    b1 = (pad >> 16) & 0xff;

    fprintf(stderr, "b1: %i ; b2: %i ; b3: %i\nx: %i ; y: %i ; pad: %i\n", b1, b2, b3, x, y, pad);

    /* output code goes here */

    return;
}

void swf_movie_initbits(swf_movie * movie) {
    movie->bitpos = 0;
    movie->bitbuf = 0;
}

/*
 * Get an 8-bit byte from the stream, and move the
 * parse head on by one.
 */

void swf_movie_put_byte(swf_movie * context, int * error, SWF_U8 byte) {
    swf_movie_initbits(context);

/* FIXME: Need a write error code */

    if (fwrite(&byte, 1, 1, context->file) != 1) {
	*error = SWF_ETooManyFriends;
	return;
    }
    context->filepos++;

    return;
}

/*
 * Ensure the parse head is byte-aligned and read the specified
 * number of bytes from the stream.
 */

void swf_movie_put_bytes (swf_movie * context, int * error, int nbytes, SWF_U8 * bytes) {
    swf_movie_initbits(context);

/* FIXME: Need a write error code */

    if( fwrite(bytes, 1, nbytes, context->file) != nbytes){
	*error = SWF_ETooManyFriends;
	return;
    }
    context->filepos += nbytes;

    return;
}


/*
 * Get n bits from the stream.
 */

void swf_movie_put_bits (swf_movie * context, SWF_S32 n, SWF_S32 bits)
{
/*    SWF_U32 v = 0; */
	
/*    while (1) {
        SWF_S32 s = n - context->bitpos;
        if (s > 0) {

            v |= context->bitbuf << s;
            n -= context->bitpos;
			

            context->bitbuf = swf_parse_get_byte(context);
            context->bitpos = 8;
        } else {

            v |= context->bitbuf >> -s;
            context->bitpos -= n;
            context->bitbuf &= 0xff >> (8 - context->bitpos); 

            return v;
        }
	} */
}


/*
 * Get n bits from the stream with sign extension.
 */

void swf_movie_put_sbits (swf_movie * context, SWF_S32 n, SWF_S32 bits)
{
}


/*
 * Put a 16-bit word to the stream, and move the
 * parse head on by two.
 */

void swf_movie_put_word(swf_movie * context, int * error, SWF_U16 word)
{
    // wrapper over put_bytes.
}

/*
 * Put a 32-bit dword to the stream, and move the
 * parse head on by four.
 */


void swf_movie_put_dword(swf_movie * context, int * error, SWF_U32 dword)
{
    swf_movie_initbits(context);

   // wrapper over put_bytes.
}

