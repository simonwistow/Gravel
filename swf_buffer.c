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
 * 	$Id: swf_buffer.c,v 1.1 2002/05/10 15:37:18 kitty_goth Exp $	
 */

#define SWF_OUT_STREAM 10240

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_buffer.h"
#include "swf_destroy.h"
#include "swf_serialise.h"

#include <stdio.h>

void 
swf_buffer_initbits(swf_buffer * buffer) 
{
    buffer->bitpos = 0;
    buffer->bitbuf = 0;
}

void 
swf_buffer_put_byte(swf_buffer * context, int * error, SWF_U8 byte) 
{
    swf_buffer_initbits(context);

    context->raw[context->size++] = byte;

    return;
}

void 
swf_buffer_put_bytes (swf_buffer * context, int * error, int nbytes, SWF_U8 * bytes) 
{
    int i;

    swf_buffer_initbits(context);

    for (i=0; i<nbytes; i++) {
	context->raw[context->size++] = bytes[i];
    }

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
swf_buffer_flush_bits (swf_buffer * context) 
{
  SWF_U32 bitbuf;
  SWF_U8 bitpos, bt;
  int error=0;

  bitbuf = context->bitbuf;
  bitpos = context->bitpos;

  swf_buffer_initbits(context);
  
  /* FIXME: error handling... */

  bt = bitbuf >> 24;
  swf_buffer_put_byte(context, &error, bt);
  if (bitpos <= 8) { return; }

  bt = (bitbuf << 8) >> 24;
  swf_buffer_put_byte(context, &error, bt);
  if (bitpos <= 16) { return; }

  bt = (bitbuf << 16) >> 24;
  swf_buffer_put_byte(context, &error, bt);
  if (bitpos <= 24) { return; }

  bt = (bitbuf << 24) >> 24;
  swf_buffer_put_byte(context, &error, bt);
}

/* FIXME: error handling... */

void 
swf_buffer_put_bits (swf_buffer * context, SWF_U8 n, SWF_U32 bits)
{
  SWF_S32 i;
  SWF_U32 bitbuf;
  SWF_U8 bt;
  int error=0;

  /* We take in the bits right-flushed. */

  i = context->bitpos;

  if (i + n > 32) {

    /* We need to flush the buffer and leave the residue */
    bitbuf = context->bitbuf;
    bitbuf += bits >> (i + n - 32);

    bt = bitbuf >> 24;
    swf_buffer_put_byte(context, &error, bt);
    bt = (bitbuf << 8) >> 24;
    swf_buffer_put_byte(context, &error, bt);
    bt = (bitbuf << 16) >> 24;
    swf_buffer_put_byte(context, &error, bt);
    bt = (bitbuf << 24) >> 24;
    swf_buffer_put_byte(context, &error, bt);
    swf_buffer_initbits(context);

    context->bitbuf = bits << (64 - (i + n));
    context->bitpos = i + n - 32;

  } else {
    /* Can coexist in the same bitbuf */
    context->bitbuf |= ((bits << (32 - n) ) >> i);
    context->bitpos = i + n;
  }

}

/*
 * Put n bits to the stream with sign extension.
 */

void 
swf_buffer_put_sbits (swf_buffer * context, SWF_U8 n, SWF_S32 bits)
{
  int i;

  i = context->bitpos;

  if (bits < 0) {
    bits += (1L << (n));
  }

  swf_buffer_put_bits(context, n, bits);
}


/*
 * Put a 16-bit word to the stream, and move the
 * parse head on by two.
 */

/* FIXME: Endian-ness */

void swf_buffer_put_word(swf_buffer * context, int * error, SWF_U16 word)
{
    swf_buffer_initbits(context);
    swf_buffer_put_byte(context, error, (SWF_U8)((word << 8) >> 8));
    swf_buffer_put_byte(context, error, (SWF_U8)(word >> 8));
}

/*
 * Put a 32-bit dword to the stream, and move the
 * parse head on by four.
 */

/* FIXME: Endian-ness */

void swf_buffer_put_dword(swf_buffer * context, int * error, SWF_U32 dword)
{
    swf_buffer_initbits(context);

    swf_buffer_put_byte(context, error, (SWF_U8)((dword << 24) >> 24));
    swf_buffer_put_byte(context, error, (SWF_U8)((dword << 16) >> 24));
    swf_buffer_put_byte(context, error, (SWF_U8)((dword << 8) >> 24));
    swf_buffer_put_byte(context, error, (SWF_U8)(dword >> 24));
}




