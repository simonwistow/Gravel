/*
 * Copyright (C) 2001-2  Ben Evans <kitty@cpan.org>
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
 * 	$Id: swf_movie.c,v 1.23 2002/05/29 16:40:47 kitty_goth Exp $	
 */

#define SWF_OUT_STREAM 10240

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_destroy.h"
#include "swf_serialise.h"

#define SWF_DESTROY_MAX_TAG_ID 48

#include <stdio.h>


swf_movie * 
swf_make_movie (int * error) 
{
    swf_movie * movie;

    if ((movie = (swf_movie *) calloc (1, sizeof (swf_movie))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    if ((movie->buffer = (swf_buffer *) calloc (1, sizeof (swf_buffer))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    movie->header = NULL;

    movie->max_obj_id = 0;

    movie->first = NULL;
    movie->lastp = &(movie->first);

    return movie;
}

/* add a tagrecord to a movie */
/* Turn this into a macro */
void 
swf_dump_shape (swf_movie * movie, int * error, swf_tagrecord * temp) 
{
    *(movie->lastp) = temp;
    movie->lastp = &(temp->next);
}

void 
swf_destroy_movie (swf_movie * movie) 
{
    swf_header * header;
    swf_tagrecord *tmp, *node;
    void * tagrec;
    void (**shiva)();
    int tagid;
    int error = 0;

    shiva = calloc((1 + SWF_DESTROY_MAX_TAG_ID), sizeof(void * ));
    init_destructors(shiva, &error);

    header = movie->header;
    swf_destroy_header(header);

    *(movie->lastp) = NULL;
    node = movie->first;

    while (node != NULL) {
        tmp = node;
        node = node->next;

	tagid  = tmp->id;
	tagrec = tmp->tag;

	if (shiva[tagid]) {
	  shiva[tagid](tagrec);
	}

	swf_destroy_tagrecord(tmp);
    }

//    swf_destroy_tagrecord(movie->first);
    swf_destroy_tagrecord(*movie->lastp);

    swf_free(movie->buffer);
//    swf_free(*movie->lastp);
    swf_free(movie->first);
    swf_free(movie);
    swf_free(shiva);

    return;
}


swf_tagrecord *
swf_get_raw_shape (swf_parser * swf, int * error) 
{
    SWF_U32 startpos;
    int length;    
    swf_tagrecord * mytag;
     
    
    if (*error != SWF_ENoError)
    {
        fprintf(stderr,"error wasn't reset in get_raw_shape\n");
     	return NULL;
    }
	

    /* attempt to make a new tag record */
    mytag  = swf_make_tagrecord(error, 0);
    if (*error != SWF_ENoError)
    {
        fprintf(stderr,"couldn't get tag record in raw_shape\n");
     	return NULL;
    }

    startpos = length = 0;

    startpos = swf->filepos;
    length = swf->cur_tag_len;

    /* mytag should not have a calloc'd raw buffer */

    if (mytag->buffer->raw != NULL) {
        fprintf(stderr, "alloc fuckup 1\n");
	goto FAIL;
    }

    if ((mytag->buffer = (swf_buffer *) calloc (1, sizeof (swf_buffer))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    if ((mytag->buffer->raw = (SWF_U8 *) calloc (length, sizeof (SWF_U8))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

/* CHECKME: Do we need this ? */
//    swf_parse_seek(swf, startpos);
    
    mytag->buffer->raw = swf_parse_get_bytes(swf, length);
    mytag->buffer->size = length;

    return mytag;

    FAIL:
    swf_destroy_tagrecord(mytag);
    return NULL;
}


swf_tagrecord *
swf_get_nth_shape (swf_parser * swf, int * error, int which_shape) 
{
/*
 * Get a raw shape from the define shape frames.
 */


    int next_id, i, done;
    swf_tagrecord * temp = NULL;
	

    /* attempt to make a new tag record */
    //temp  = swf_make_tagrecord(error, 0);
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
	    case tagDefineShape:
	    case tagDefineShape2:
	    case tagDefineShape3:
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

/* FIXME: TESTCODE */

swf_fillstyle * 
swf_make_fillstyle(int * error) 
{
    swf_fillstyle * mystyle;

    if ((mystyle = (swf_fillstyle *) calloc (1, sizeof (swf_fillstyle))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    mystyle->fill_style = 0; /* Just do solid fill for now */
    mystyle->ncolours = 0; /* Just do solid fill for now */
    mystyle->bitmap_id = 0; /* Just do solid fill for now */
    mystyle->matrix = NULL; /* Just do solid fill for now */
    mystyle->colours = (swf_rgba_pos **) &(mystyle->matrix); /* Just do solid fill for now */

    mystyle->colour = 0; /* Black shape */

    return mystyle;
}

/* FIXME: TESTCODE */

swf_linestyle * 
swf_make_linestyle(int * error) 
{
    swf_linestyle * mystyle;

    if ((mystyle = (swf_linestyle *) calloc (1, sizeof (swf_linestyle))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    /* FIXME: This is borken in swf_types.h */
    mystyle->colour = 0; /* Black lines */
    mystyle->width = 2 * 20; /* Thin lines, 2 pixels wide */

    return mystyle;
}

swf_shaperecord *
swf_make_shaperecord(int * error) 
{
    swf_shaperecord * record;

    if ((record = (swf_shaperecord *) calloc (1, sizeof (swf_shaperecord))) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }

    return record;
}

// FIXME: Macro-ise this type of function
void 
swf_add_shaperecord (swf_shaperecord_list * list, int * error, swf_shaperecord * temp) 
{
    *(list->lastp) = temp;
    list->lastp = &(temp->next);
}

swf_shaperecord_list * 
swf_make_shaperecords_for_triangle(int * error) 
{
    swf_shaperecord_list * list;
    swf_shaperecord * record;

    if ((list = (swf_shaperecord_list *) calloc (1, sizeof (swf_shaperecord_list))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    list->record_count = 0;
    list->first = NULL;
    list->lastp = &(list->first);

    /* This list has five records in it */

/* First, we need a non-edge, change of style record */ 
    record = swf_make_shaperecord(error);




    swf_add_shaperecord(list, error, record);

/* Then we need three edges */
    record = swf_make_shaperecord(error);



    swf_add_shaperecord(list, error, record);

    record = swf_make_shaperecord(error);




    swf_add_shaperecord(list, error, record);

    record = swf_make_shaperecord(error);




    swf_add_shaperecord(list, error, record);

/* Then we need an end-of-shape edge */
    record = swf_make_shaperecord(error);




    swf_add_shaperecord(list, error, record);

    return list;
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

    triangle = swf_make_tagrecord(error, tagDefineShape);

    if (*error) {
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

    canvas->xmin = 0 * 20;
    canvas->xmax = 50 * 20;
    canvas->ymin = 0 * 20;
    canvas->ymax = 50 * 20;

    shape->rect = canvas;

    mystyle->nfills = 1;
    mystyle->nlines = 1;

    *(mystyle->fills) = swf_make_fillstyle(error);
    *(mystyle->lines) = swf_make_linestyle(error);

    shape->style = mystyle;

    shape->record = swf_make_shaperecords_for_triangle(error);

    triangle->tag = (void *) shape;

    return triangle;

 FAIL:
    swf_destroy_tagrecord(triangle);
    *error = SWF_EMallocFailure;
    return NULL;
}

/* Calculate size et all, and stream out */

/* NOTE: We'll need to keep the
 * size butes at 20, then random seek back to the 8th byte of the file
 * and overwrite, once we know the real size. */

void 
swf_make_finalise(swf_movie * movie, int * error) 
{
  SWF_U8 * file_buf;
  SWF_U32 tmp_32, tmp_size;
  SWF_U16 tmp_16;
  swf_tagrecord *temp, *node;
  int i;

  /* allocate a tag buffer */
  if ((file_buf = (SWF_U8 *) calloc (1, SWF_OUT_STREAM)) == NULL) {
    *error = SWF_EMallocFailure;
    return;
  }

  /* Open the file for writing. */
  movie->file = fopen(movie->name, "wb");
  
  /* Did we open the file? */
  if (movie->file == NULL) {
    *error = SWF_EFileOpenFailure;
    return;
  }

  file_buf[0] = 'F';
  file_buf[1] = 'W';
  file_buf[2] = 'S';
  tmp_32 = movie->header->version;
  file_buf[3] = (SWF_U8)(tmp_32 & 0xff); 

/* let's build up the size. 
 * start with the header, version and total length bytes.
 */
  tmp_size = 8; 

/* Because of the way swf_serialise_rect is, we need to have a 
 * temp value in length and overseek properly. Oh well...
 */
  file_buf[4] = tmp_size >> 24;
  file_buf[5] = (tmp_size << 8) >> 24;
  file_buf[6] = (tmp_size << 16) >> 24;
  file_buf[7] = (tmp_size << 24) >> 24;

  swf_movie_put_bytes(movie, error, 8, file_buf);
  swf_serialise_rect(movie, error, movie->header->bounds);

/* set the backup byte counter to be the next vacant place. In case we 
 * need to seek later due to fuckups.
 */
  i = movie->filepos; 
  tmp_size = i;

  printf("File position is: %i\n", i);

  /* Rate and count go next... */

  /* PROTO: Do a tree walk to determine frame count 
   * consider using a backseek in production code..?
   */
  
  tmp_16 = 0;

  node = movie->first;

  while (node != NULL) {
    temp = node;
    node = node->next;
    
/* printf("Walking waka-waka : id = %i\n", temp->id); */

    if (tagShowFrame == temp->id) {
      tmp_16++;
    }
  }

  movie->header->count = tmp_16;
  printf("Frame count = %i\n", tmp_16);

  /* rate and count....... */

  swf_movie_put_word(movie, error, (SWF_U16) movie->header->rate);
  swf_movie_put_word(movie, error, (SWF_U16) movie->header->count);

  tmp_size += 4; /* for the rate and count */

  /* Next: Walk the LL of tags...  
   * Build a size as we do this...
   */
  // LL walk...

  node = movie->first;

  while (node != NULL) {
    temp = node;
    node = node->next;
    
    if (0 == temp->serialised) {
      fprintf(stderr, "HELP: This tag isn't serialised...\n");
    }

    tmp_16 = temp->id << 6;

    /* FIXME: We don't handle long tags yet...*/
    tmp_16 |= (SWF_U16) temp->buffer->size;

    swf_movie_put_word(movie, error, tmp_16);
    tmp_size += 2; /* for the tag header */

    /* FIXME: We don't handle tags with content yet... */
    swf_movie_put_bytes(movie, error, temp->buffer->size, temp->buffer->raw);
    tmp_size += temp->buffer->size;

  }

  printf("File size is: %"pSWF_U16"\n", tmp_size);

  /* Backseek to fix up file size. */

  swf_movie_seek(movie, 4);
  swf_movie_put_dword(movie, error, tmp_size);

  fclose(movie->file);
  free(file_buf);
}

void 
swf_movie_initbits(swf_movie * movie) {
    movie->buffer->bitpos = 0;
    movie->buffer->bitbuf = 0;
}

void 
swf_movie_seek (swf_movie * context, int pos)
{
    fseek(context->file, pos, SEEK_SET);
    return;
}

/*
 * Get an 8-bit byte from the stream, and move the
 * parse head on by one.
 */

void 
swf_movie_put_byte(swf_movie * context, int * error, SWF_U8 byte) 
{
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

void 
swf_movie_put_bytes (swf_movie * context, int * error, int nbytes, SWF_U8 * bytes) 
{
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
 * Put n bits from the stream.
 */


/* The problem here is... we can't finish our flush until
 * we're sure we've got all the adjacent bit fields (and we may
 * have SB's and UB's next to each other, as well....
 * So, we have to separate out the flush function.
 */

void 
swf_movie_flush_bits (swf_movie * context) 
{
  SWF_U32 bitbuf;
  SWF_U8 bitpos, bt;
  int error=0;

  bitbuf = context->buffer->bitbuf;
  bitpos = context->buffer->bitpos;

  swf_movie_initbits(context);
  
  /* FIXME: error handling... */


  bt = bitbuf >> 24;
  swf_movie_put_byte(context, &error, bt);
  if (bitpos <= 8) { return; }

  bt = (bitbuf << 8) >> 24;
  swf_movie_put_byte(context, &error, bt);
  if (bitpos <= 16) { return; }

  bt = (bitbuf << 16) >> 24;
  swf_movie_put_byte(context, &error, bt);
  if (bitpos <= 24) { return; }

  bt = (bitbuf << 24) >> 24;
  swf_movie_put_byte(context, &error, bt);
}

/* FIXME: error handling... */

void 
swf_movie_put_bits (swf_movie * context, SWF_U8 n, SWF_U32 bits)
{
  SWF_S32 i;
  SWF_U32 bitbuf;
  SWF_U8 bt;
  int error=0;

  /* We take in the bits right-flushed. */

  i = context->buffer->bitpos;

  if (i + n > 32) {

    /* We need to flush the buffer and leave the residue */
    bitbuf = context->buffer->bitbuf;
    bitbuf += bits >> (i + n - 32);

    bt = bitbuf >> 24;
    swf_movie_put_byte(context, &error, bt);
    bt = (bitbuf << 8) >> 24;
    swf_movie_put_byte(context, &error, bt);
    bt = (bitbuf << 16) >> 24;
    swf_movie_put_byte(context, &error, bt);
    bt = (bitbuf << 24) >> 24;
    swf_movie_put_byte(context, &error, bt);
    swf_movie_initbits(context);

    context->buffer->bitbuf = bits << (64 - (i + n));
    context->buffer->bitpos = i + n - 32;

  } else {
    /* Can coexist in the same bitbuf */
    context->buffer->bitbuf |= ((bits << (32 - n) ) >> i);
    context->buffer->bitpos = i + n;
  }

}

/*
 * Put n bits to the stream with sign extension.
 */

void 
swf_movie_put_sbits (swf_movie * context, SWF_U8 n, SWF_S32 bits)
{
  int i;

  i = context->buffer->bitpos;

  if (bits < 0) {
    bits += (1L << (n));
  }

  swf_movie_put_bits(context, n, bits);
}


/*
 * Put a 16-bit word to the stream, and move the
 * parse head on by two.
 */

void 
swf_movie_put_word(swf_movie * context, int * error, SWF_U16 word)
{
    swf_movie_initbits(context);
    swf_movie_put_byte(context, error, (SWF_U8)((word << 8) >> 8));
    swf_movie_put_byte(context, error, (SWF_U8)(word >> 8));
}

/*
 * Put a 32-bit dword to the stream, and move the
 * parse head on by four.
 */

void 
swf_movie_put_dword(swf_movie * context, int * error, SWF_U32 dword)
{
    swf_movie_initbits(context);

    swf_movie_put_byte(context, error, (SWF_U8)((dword << 24) >> 24));
    swf_movie_put_byte(context, error, (SWF_U8)((dword << 16) >> 24));
    swf_movie_put_byte(context, error, (SWF_U8)((dword << 8) >> 24));
    swf_movie_put_byte(context, error, (SWF_U8)(dword >> 24));
}

void 
swf_movie_put_string(swf_movie * context, int * error, char * mystring)
{
}

