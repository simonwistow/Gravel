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
#include "swf_error.h"
#include "swf_movie.h"
#include "swf_serialise.h"
#include "swf_destroy.h"

#include <stdio.h>


void test_buffer_initbits(swf_buffer * buffer);

void 
test_buffer_initbits(swf_buffer * buffer) 
{
    buffer->bitpos = 0;
    buffer->bitbuf = 0;
}

void test_buffer_put_byte(swf_buffer * context, int * error, SWF_U8 byte, int printing);

void 
test_buffer_put_byte(swf_buffer * context, int * error, SWF_U8 byte, int printing) 
{
    test_buffer_initbits(context);

    context->raw[context->size++] = byte;

    return;
}

void test_buffer_put_bytes (swf_buffer * context, int * error, int nbytes, SWF_U8 * bytes, int printing);

void 
test_buffer_put_bytes (swf_buffer * context, int * error, int nbytes, SWF_U8 * bytes, int printing) 
{
    int i;

    test_buffer_initbits(context);

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

void test_buffer_flush_bits (swf_buffer * context, int printing);

void 
test_buffer_flush_bits (swf_buffer * context, int printing) 
{
  SWF_U32 bitbuf;
  SWF_U8 bitpos, bt;
  int error=0;

  bitbuf = context->bitbuf;
  bitpos = context->bitpos;

  test_buffer_initbits(context);
  
  /* FIXME: error handling... */

  bt = bitbuf >> 24;
  test_buffer_put_byte(context, &error, bt, printing);
  if (bitpos <= 8) { return; }

  bt = (bitbuf << 8) >> 24;
  test_buffer_put_byte(context, &error, bt, printing);
  if (bitpos <= 16) { return; }

  bt = (bitbuf << 16) >> 24;
  test_buffer_put_byte(context, &error, bt, printing);
  if (bitpos <= 24) { return; }

  bt = (bitbuf << 24) >> 24;
  test_buffer_put_byte(context, &error, bt, printing);
}

/* FIXME: error handling... */

void test_buffer_put_bits (swf_buffer * context, SWF_U8 n, SWF_U32 bits, int printing);

void 
test_buffer_put_bits (swf_buffer * context, SWF_U8 n, SWF_U32 bits, int printing)
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
    test_buffer_put_byte(context, &error, bt, printing);
    bt = (bitbuf << 8) >> 24;
    test_buffer_put_byte(context, &error, bt, printing);
    bt = (bitbuf << 16) >> 24;
    test_buffer_put_byte(context, &error, bt, printing);
    bt = (bitbuf << 24) >> 24;
    test_buffer_put_byte(context, &error, bt, printing);
    test_buffer_initbits(context);

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

void test_buffer_put_sbits (swf_buffer * context, SWF_U8 n, SWF_S32 bits, int printing);

void 
test_buffer_put_sbits (swf_buffer * context, SWF_U8 n, SWF_S32 bits, int printing)
{
  int i;

  i = context->bitpos;

  if (bits < 0) {
    bits += (1L << (n));
  }

  test_buffer_put_bits(context, n, bits, printing);
}


/*
 * Put a 16-bit word to the stream, and move the
 * parse head on by two.
 */

void test_buffer_put_word(swf_buffer * context, int * error, SWF_U16 word, int printing);

void 
test_buffer_put_word(swf_buffer * context, int * error, SWF_U16 word, int printing)
{
    test_buffer_initbits(context);
    test_buffer_put_byte(context, error, (SWF_U8)((word << 8) >> 8), printing);
    test_buffer_put_byte(context, error, (SWF_U8)(word >> 8), printing);
}

/*
 * Put a 32-bit dword to the stream, and move the
 * parse head on by four.
 */

void test_buffer_put_dword(swf_buffer * context, int * error, SWF_U32 dword, int printing);

void 
test_buffer_put_dword(swf_buffer * context, int * error, SWF_U32 dword, int printing)
{
    test_buffer_initbits(context);

    test_buffer_put_byte(context, error, (SWF_U8)((dword << 24) >> 24), printing);
    test_buffer_put_byte(context, error, (SWF_U8)((dword << 16) >> 24), printing);
    test_buffer_put_byte(context, error, (SWF_U8)((dword << 8) >> 24));
    test_buffer_put_byte(context, error, (SWF_U8)(dword >> 24));
}

void 
test_buffer_put_string(swf_buffer * context, int * error, char * mystring)
{
    int sp = 0;

    test_buffer_initbits(context);

    while ( (sp < 255) && (mystring[sp]) ) {
      test_buffer_put_byte(context, error, mystring[sp++]);
    }
}

void 
test_buffer_rect(swf_buffer * p, int * error, swf_rect * rect) 
{
  SWF_U16 max;
  SWF_U8 i; 

  max = 0;
  if (abs(rect->xmin) > max) {
    max = abs(rect->xmin);
  }
  if (abs(rect->xmax) > max) {
    max = abs(rect->xmax);
  }
  if (abs(rect->ymin) > max) {
    max = abs(rect->ymin);
  }
  if (abs(rect->xmax) > max) {
    max = abs(rect->ymax);
  }

  i = 2; /* i=2 because we're doing signed bits here for the rect values.. */
  while (1 < max) {
    i++;
    max = max >> 1;
  }

  test_buffer_put_bits(p, 5, i);
  test_buffer_put_sbits(p, i, rect->xmin);
  test_buffer_put_sbits(p, i, rect->xmax);
  test_buffer_put_sbits(p, i, rect->ymin);
  test_buffer_put_sbits(p, i, rect->ymax);
  test_buffer_flush_bits(p);
}

void
test_buffer_shapestyle(swf_buffer * buffer, int * error, swf_shapestyle * s, int printing)
{
	SWF_U8 i, type;

	test_buffer_initbits(buffer);
	if (s->nfills < 255) {
		test_buffer_put_byte (buffer, error, (SWF_U8)s->nfills, printing);
	} else {
		test_buffer_put_word (buffer, error, s->nfills, printing);
	}

	/* FIXME: Alpha channels... */
	for (i=0; i < s->nfills; i++) {
		printf("putting fill %i\n", i);

		type = s->fills[i]->fill_style;
		test_buffer_put_byte (buffer, error, type);

		/* FIXME: Do the other fill types */
        if (type & fillGradient) {
			/* Gradient Fill */
			fprintf(stderr, "Warning! Unsupported fill type...\n");
        } else if (type & fillBits) {
			/* Bitmap Fill */
			fprintf(stderr, "Warning! Unsupported fill type...\n");
		} else {
			/* Solid Fill */
			/* Test code */
			test_buffer_put_byte(buffer, error, 0);
			test_buffer_put_byte(buffer, error, 0);
			test_buffer_put_byte(buffer, error, 0);

			/* FIXME: The code uses SWF_U32 instead of a colour type for fillstyle->colour atm */
			//			swf_serialise_cxform(buffer, error, s->fills[i]->colour);
		}
	}

	if (s->nlines < 255) {
		test_buffer_put_byte (buffer, error, (SWF_U8)s->nlines);
	} else {
		test_buffer_put_word (buffer, error, s->nlines);
	}

	/* FIXME: Alpha channels... */
	for (i=0; i < s->nlines; i++) {
		printf("putting line %i\n", i);
		test_buffer_put_word (buffer, error, s->lines[i]->width);
			/* Test code */
			test_buffer_put_byte(buffer, error, 0);
			test_buffer_put_byte(buffer, error, 0);
			test_buffer_put_byte(buffer, error, 0);

			/* FIXME: The code uses SWF_U32 instead of a colour type for fillstyle->colour atm */
			//		swf_serialise_cxform(buffer, error, s->lines[i]->colour);
	}

	test_buffer_put_bits(buffer, 4, s->fillbits);
	test_buffer_put_bits(buffer, 4, s->linebits);

	test_buffer_flush_bits(buffer);
}



void
test_buffer_shaperecord(swf_buffer * buffer, int * error, swf_shaperecord * s, swf_shapestyle * st)
{
	SWF_U8 i, n;	
	SWF_U32 max;

	test_buffer_put_bits(buffer, 1, s->is_edge);
	
	if (!s->is_edge) {
		/* State change */
		
		printf("Putting non-edge.. flags : %u\n", s->flags);
		test_buffer_put_bits(buffer, 5, s->flags);

        /* Are we at the end? */
        if (s->flags == 0) {
            return;
        }

        /* TODO: Process a move to. */
        if (s->flags & eflagsMoveTo) {

			max = 0;
			if (abs(s->y) > max) {
				max = abs(s->y);
			}
			if (abs(s->x) > max) {
				max = abs(s->x);
			}
			
			i = 2; /* 2 for sbits */
			while (1 < max) {
				i++;
				max = max >> 1;
			}
			test_buffer_put_bits(buffer, 5, i);

			test_buffer_put_sbits(buffer, i, s->x);
			test_buffer_put_sbits(buffer, i, s->y);				
        }

        /* Process new fill info. */
        if (s->flags & eflagsFill0) {
			test_buffer_put_bits(buffer, st->fillbits, s->fillstyle0);
        }

        if (s->flags & eflagsFill1) {
			test_buffer_put_bits(buffer, st->fillbits, s->fillstyle1);
        }

        /* Get new line info */
        if (s->flags & eflagsLine) {
			test_buffer_put_bits(buffer, st->linebits, s->linestyle);
        }

        /* TODO: Process new styles*/
        if (s->flags & eflagsNewStyles) {
			fprintf(stderr, "Unsupported New Styles Flag\n");
        }

		return;
	} else {
		printf("Putting edge..\n");

		if (s->x | s->y) {
			if (s->ax | s->ay | s->cx | s->cy) {
				printf("Putting weirdness..\n");
				/* We're trying to be a line and a curve all at once */
				*error = SWF_ENotValidSWF;
				return;
			}

			/* we're a straight line */
			printf("Putting line..\n");

			test_buffer_put_bits(buffer, 1, 1);
			
			max = 0;
			if (abs(s->y) > max) {
				max = abs(s->y);
			}
			if (abs(s->x) > max) {
				max = abs(s->x);
			}
			
			i = 0; /* 2 for sbits, -2 from spec */
			while (1 < max) {
				printf("max = %i ; i = %i\n", max, i);
				i++;
				max = max >> 1;
			}
			printf("putting bits : %i\n", i);
			test_buffer_put_bits(buffer, 4, i);
			i += 2;

			if ((0 == s->x) || (0 == s->y)) {
				/* Vertical / Horizontal line */
				test_buffer_put_bits(buffer, 1, 0);
				
				if (0 == s->x) {
					printf("Putting vert line..\n");
					test_buffer_put_bits(buffer, 1, 1);
					test_buffer_put_sbits(buffer, i, s->y);
				} else {
					printf("Putting horiz line..\n");
					test_buffer_put_bits(buffer, 1, 0);
					test_buffer_put_sbits(buffer, i, s->x);
				}

			} else {
				/* General Line */
				test_buffer_put_bits(buffer, 1, 1);

				test_buffer_put_sbits(buffer, i, s->x);
				test_buffer_put_sbits(buffer, i, s->y);				
			}

		} else {
			/* TODO: Curves... */
			/* we're a curve */
			test_buffer_put_bits(buffer, 1, 0);
			printf("Putting curve..\n");

		}
		
	}

}



void
test_serialise_defineshape (swf_buffer * buffer, int * error, swf_defineshape * shape)
{
	SWF_U8 i;
	SWF_U32 max;
	swf_shaperecord *node, *temp;

	test_buffer_initbits(buffer);
	test_buffer_put_word(buffer, error, shape->tagid, 0);
	test_buffer_rect(buffer, error, shape->rect, 0);

	i = 1; /* The nbits is a UB value, so i = 1 here*/
	max = shape->style->nfills;
	while (1 < max) {
		i++;
		max = max >> 1;
	}
	shape->style->fillbits = i;

	i = 1; /* The nbits is a UB value, so i = 1 here*/
	max = shape->style->nlines;
	while (1 < max) {
		i++;
		max = max >> 1;
	}
	shape->style->linebits = i;

	test_buffer_shapestyle(buffer, error, shape->style, 1);

	node = shape->record->first;

	while (node != NULL) {
		temp = node;
		node = node->next;

		test_buffer_shaperecord(buffer, error, temp, shape->style, 1);
	}

}




int main (int argc, char *argv[]) {
    int error = SWF_ENoError;
    swf_tagrecord * temp;
    swf_movie * movie;

    if ((movie = swf_make_movie(&error)) == NULL) {
	return 1;
    }

    /* Right, now we need a tagrecord.. */
    temp = swf_make_triangle(movie, &error);

    /* Need to calloc a (raw) buffer for temp... */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
        return 1;
    }

    printf("foo 1\n");
    test_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
    printf("foo 2\n");
    temp->serialised = 1;

    fprintf (stderr, "OK\n");
    return 0;
}



