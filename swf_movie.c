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
 * 	$Id: swf_movie.c,v 1.33 2002/06/20 17:02:15 kitty_goth Exp $	
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


    swf_destroy_buffer(movie->buffer);

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

    mytag->buffer->raw = swf_parse_get_bytes(swf, length);
    mytag->buffer->size = length;

    return mytag;

    FAIL:
    swf_destroy_tagrecord(mytag);
    return NULL;
}


/*
 * Get a raw shape from the define shape frames.
 */

swf_tagrecord *
swf_get_nth_shape (swf_parser * swf, int * error, int which_shape) 
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
    if ((mystyle->colours = (swf_rgba_pos **) calloc (1, sizeof (swf_rgba_pos *))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }
    if ((mystyle->col = (swf_colour *) calloc (1, sizeof (swf_colour *))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    mystyle->fill_style = 0; /* Just do solid fill for now */
    mystyle->ncolours = 1; /* Just do solid fill for now */
    mystyle->bitmap_id = 0; /* Just do solid fill for now */
    mystyle->matrix = NULL; /* Just do solid fill for now */
    mystyle->colours = (swf_rgba_pos **) &(mystyle->matrix); /* Just do solid fill for now */

    mystyle->col->r = 0xff; 
    mystyle->col->g = 0; 
    mystyle->col->b = 0; 
    mystyle->col->a = 0xff; 

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
    if ((mystyle->col = (swf_colour *) calloc (1, sizeof (swf_colour *))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    mystyle->col->r = 0; 
    mystyle->col->g = 0; 
    mystyle->col->b = 0xff; 
    mystyle->col->a = 0xff; 

    mystyle->width = 2 * 20; /* Thin lines, 2 pixels wide */

    return mystyle;
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
