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
 *
 */


#include <stdio.h>
#include "swf_types.h"

/* 
 * Have never seen a movie in the wild with a frame rate > 15. 
 * So set maximum as 25 to be conservative. 
 */

#define MAXVER 5
#define MAXSPEED 25
#define TYPICALSPEED 12

#define LINESTYLEMASK 0x20

typedef struct swf_movie swf_movie;
typedef struct swf_tagrecord swf_tagrecord;
typedef struct swf_triangle swf_triangle;

/* 
 * NOTE: There's a lot of stream/file-orientated duplication here.
 * Perhaps we should refactor into a stream adaptor shared by the
 * generator and parser
 */

struct swf_movie {
    FILE * file;          /* The file handle of the SWF file we're parsing */
    char * name;          /* the name of the file we're handling */
    SWF_U32 filepos;    

    /* Bit Handling. */
    SWF_S32 bitpos;       /* what position we're at in the bit buffer */
    SWF_U32 bitbuf;       /* the bit buffer, used for storing bits */

    swf_header * header;

    SWF_U32 max_obj_id;    /* the highest id of the objects we've created */

    swf_tagrecord *  first;     /* pointer to the first element in the list */
    swf_tagrecord ** lastp;    /* pointer to the last element in the list */
};

struct swf_tagrecord {
    SWF_U16 id; /* ID of tag */
    void * tag; /* This is a pointer to the tag structure */

    int serialised;
    SWF_U32 size;
    SWF_U8 * buffer; /* Raw buffer excluding header */

    swf_tagrecord *  next;     /* pointer to the next element in the list */
};

/* 
 * High level for testing serialisation
 * (Dancing triangles flash movies, anyone?) 
 * FIXME: This 
 */

struct swf_triangle {

};


void swf_make_header_blank (swf_movie * movie, int * error, int ver, int speed);
void swf_make_header_raw (swf_movie * movie, int * error, swf_rect * rect);
void swf_make_header (swf_movie * movie, int * error, SCOORD x1, SCOORD x2, SCOORD y1, SCOORD y2);


void swf_make_finalise(swf_movie * movie, int * error);

swf_movie * swf_make_movie (int * error);
void swf_destroy_movie (swf_movie * movie);


swf_tagrecord * swf_make_tagrecord (int * error);

swf_shaperecord_list * swf_make_shaperecords_for_triangle(int * error);

swf_tagrecord * swf_make_triangle_as_tag(swf_movie * movie, int * error);


void swf_get_nth_shape (swf_parser * swf, int * error, int which_shape, swf_tagrecord * mybuffer);

void swf_get_raw_shape (swf_parser * swf, int * error, swf_tagrecord * mybuffer);

void swf_destroy_tagrecord (swf_tagrecord * tagrec);

swf_linestyle * swf_make_linestyle(int * error);
swf_fillstyle * swf_make_fillstyle(int * error);

void swf_movie_initbits(swf_movie * movie);
void swf_movie_flush_bits (swf_movie * context);

void swf_movie_seek (swf_movie * context, int pos);

void swf_movie_put_byte(swf_movie * context, int * error, SWF_U8 byte);
void swf_movie_put_bytes (swf_movie * context, int * error, int nbytes, SWF_U8 * bytes);

void swf_movie_put_bits (swf_movie * context, SWF_U8 n, SWF_U32 bits);
void swf_movie_put_sbits (swf_movie * context, SWF_U8 n, SWF_S32 bits);

void swf_movie_put_word(swf_movie * context, int * error, SWF_U16 word);
void swf_movie_put_dword(swf_movie * context, int * error, SWF_U32 dword);

