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
 * 	$Id: swf_file.c,v 1.1 2002/05/31 10:54:27 muttley Exp $
 */


#include "swf_types.h"
#include "swf_parse.h"
#include "swf_file.h"
#include "swf_movie.h"
#include "swf_destroy.h"
#include "swf_serialise.h"


#include <stdio.h>


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

