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
 */

#include "tag_handy.h"

/*
 * Get the shapestyle from the input stream. We need the linestyles and the
 * fillstyles from the stream.
 */

swf_shapestyle *
swf_parse_get_shapestyle (swf_parser * context, int * error, int with_alpha)
{
    SWF_U16 i = 0;
    SWF_U16 j = 0;
    swf_shapestyle * styles;

    if ((styles = (swf_shapestyle *) calloc (1, sizeof (swf_shapestyle))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    styles->nfills = 0;
    styles->nlines = 0;
    styles->fills  = NULL;
    styles->lines  = NULL;

    /* Get the number of fills. */
    styles->nfills = swf_parse_get_byte (context);


    /* Do we have a larger number? */
    if (styles->nfills == 255) {
        /* Get the larger number. */
        styles->nfills = swf_parse_get_word(context);
    }

    if ((styles->fills = (swf_fillstyle **) calloc (styles->nfills, sizeof (swf_fillstyle *))) == NULL) {
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    /* Get each of the fill style. */
    for (i = 0; i <styles->nfills; i++) {
        if ((styles->fills[i] = (swf_fillstyle *) calloc (1, sizeof (swf_fillstyle))) == NULL) {
			*error = SWF_EMallocFailure;
			goto FAIL;
        }

        styles->fills[i]->ncolours = 0;
        styles->fills[i]->colours  = NULL;
        styles->fills[i]->matrix   = NULL;

        styles->fills[i]->fill_style = swf_parse_get_byte (context);

        if (styles->fills[i]->fill_style & fillGradient) {
            /* Get the gradient matrix. */
            styles->fills[i]->matrix = (swf_matrix *) swf_parse_get_matrix (context, error);

            /* Get the number of colors. */
            styles->fills[i]->ncolours = swf_parse_get_byte(context);

            if ((styles->fills[i]->colours = (swf_rgba_pos **) calloc (styles->fills[i]->ncolours, sizeof (swf_rgba_pos *))) == NULL) {
                *error = SWF_EMallocFailure;
                goto FAIL;
            }

            /* Get each of the colours. */
            for (j = 0; j< styles->fills[i]->ncolours; j++) {
                if ((styles->fills[i]->colours[j] = (swf_rgba_pos *) calloc (1, sizeof (swf_rgba_pos))) == NULL) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }

                if ((styles->fills[i]->colours[j]->col = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }

                styles->fills[i]->colours[j]->pos  = swf_parse_get_byte   (context);
				// FIXME: These need to be proper colours...
//                styles->fills[i]->colours[j]->rgba = swf_parse_get_colour (context, error,  with_alpha);

                styles->fills[i]->colours[j]->col = swf_parse_get_col (context, error,  with_alpha);

            }
        } else if (styles->fills[i]->fill_style & fillBits) {
            /* Get the bitmap matrix. */
            styles->fills[i]->bitmap_id = swf_parse_get_word (context);
            styles->fills[i]->matrix = swf_parse_get_matrix (context, error);
        } else {
            /* A solid color */
            styles->fills[i]->col = swf_parse_get_col(context, error, with_alpha);
        }
    }

    /* Get the number of lines. */
    styles->nlines = swf_parse_get_byte (context);

    /* Do we have a larger number? */
    if (styles->nlines == 255) {
        /* Get the larger number. */
        styles->nlines = swf_parse_get_word (context);
    }

    if ((styles->lines = (swf_linestyle **) calloc (styles->nlines, sizeof (swf_linestyle *))) == NULL) {
		*error = SWF_EMallocFailure;
		goto FAIL;
    }

    /* Get each of the line styles. */
    for (i = 0; i < styles->nlines; i++) {
        if ((styles->lines[i] = (swf_linestyle *) calloc (1, sizeof (swf_linestyle))) == NULL) {
			*error = SWF_EMallocFailure;
			goto FAIL;
        }
        if ((styles->lines[i]->col = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
			*error = SWF_EMallocFailure;
			goto FAIL;
        }

        styles->lines[i]->width  = swf_parse_get_word(context);
        styles->lines[i]->col = swf_parse_get_col(context, error, with_alpha);
    }

    return styles;

 FAIL:
    swf_destroy_shapestyle (styles);
    return NULL;
}

/* TODO: Buffer out the shapestyles... */
// FIXME: Alpha channel handling...
void
swf_buffer_shapestyle(swf_buffer * buffer, int * error, swf_shapestyle * s)
{
	SWF_U8 i, j, type;

	swf_buffer_initbits(buffer);
	if (s->nfills < 255) {
		swf_buffer_put_byte (buffer, error, (SWF_U8)s->nfills);
	} else {
		swf_buffer_put_word (buffer, error, s->nfills);
	}

	/* FIXME: Alpha channels... */
	for (i=0; i < s->nfills; i++) {
		printf("putting fill %i\n", i);

		type = s->fills[i]->fill_style;
		swf_buffer_put_byte (buffer, error, type);

		/* FIXME: Do the bitmap fill type */
        if (type & fillGradient) {
			/* Gradient Fill */

			swf_serialise_matrix(buffer, error, s->fills[i]->matrix);
			swf_buffer_put_byte(buffer, error, s->fills[i]->ncolours);
			for (j=0; j < s->fills[i]->ncolours && s->fills[i]->colours[j] != NULL; j++) {
				swf_buffer_put_byte(buffer, error, s->fills[i]->colours[j]->pos);
				swf_buffer_colour(buffer, error, s->fills[i]->colours[j]->col, NO_ALPHA);
			}

        } else if (type & fillBits) {
			/* Bitmap Fill */
			fprintf(stderr, "Warning! Unsupported fill type...\n");
		} else {
			/* Solid Fill */
			swf_buffer_colour(buffer, error, s->fills[i]->col, NO_ALPHA);
		}
	}

	if (s->nlines < 255) {
		swf_buffer_put_byte (buffer, error, (SWF_U8)s->nlines);
	} else {
		swf_buffer_put_word (buffer, error, s->nlines);
	}

	/* FIXME: Alpha channels... */
	for (i=0; i < s->nlines; i++) {
		printf("putting line %i\n", i);
		swf_buffer_put_word (buffer, error, s->lines[i]->width);
		swf_buffer_colour(buffer, error, s->lines[i]->col, NO_ALPHA);
	}

	swf_buffer_put_bits(buffer, 4, s->fillbits);
	swf_buffer_put_bits(buffer, 4, s->linebits);

	swf_buffer_flush_bits(buffer);
}

/*
 * Shapestyle's are reasonably complicated types.
 *
 * I think what is needed here is for the ith line/fillstyle
 * to be free'd (with a check to make sure it points at something)
 *
 * There's no need to keep track of how many we've actually allocated
 * (for the case where we crash out halfway through) as we're
 * using calloc for memory allocation, so test-for-NULL will do
 * it for us.
 */

void
swf_destroy_shapestyle (swf_shapestyle * style)
{
    int i;

    if (style==NULL) {
        return;
    }

    for (i=0; i<style->nlines && style->lines[i] != NULL; i++) {
       swf_free (style->lines[i]);
    }

	swf_free (style->lines);

    for (i=0; i<style->nfills && style->fills[i] != NULL; i++) {
        swf_destroy_fillstyle (style->fills[i]);
    }
    swf_free (style->fills);

    swf_free (style);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
