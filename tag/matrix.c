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
 * Flash has an idea of a 'matrix' which is not strictly a matrix
 * in the mathematical sense, but rather a general 2D transformation
 * with translation terms.
 *
 * This function grabs such a transformation from the input stream
 * and stores it.
 */

swf_matrix *
swf_parse_get_matrix (swf_parser * context, int * error)
{
    swf_matrix * matrix;
    int n_bits;

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    swf_parse_initbits (context);

    /* Scale terms */
    if (swf_parse_get_bits (context, 1)) {
		n_bits = (int) swf_parse_get_bits (context, 5);
		matrix->a = swf_parse_get_sbits(context, n_bits);
		matrix->d = swf_parse_get_sbits(context, n_bits);
    } else {
		matrix->a = matrix->d = 0x00010000L;
    }

    /* Rotate/skew terms */
    if (swf_parse_get_bits (context, 1)) {
		n_bits = swf_parse_get_bits (context, 5);
		matrix->b = swf_parse_get_sbits(context, n_bits);
		matrix->c = swf_parse_get_sbits(context, n_bits);
    } else {
		matrix->b = matrix->c = 0;
    }

    /* Translate terms */
    n_bits = (int) swf_parse_get_bits (context, 5);
    matrix->tx = swf_parse_get_sbits(context, n_bits);
    matrix->ty = swf_parse_get_sbits(context, n_bits);

    return matrix;
}



void
swf_serialise_matrix (swf_movie * movie, int * error, swf_matrix * mym)
{
	SWF_U32 max, i;

    swf_movie_initbits(movie);

	/* 'Scale' bits first... */
	if (mym->a | mym->c) {
		swf_movie_put_bits(movie, 1, 1);

		max = 0;
		i = 2;

		if (abs(mym->a) > max) {
			max = abs(mym->a);
		}
		if (abs(mym->c) > max) {
			max = abs(mym->c);
		}
		while (1 < max) {
			i++;
			max = max >> 1;
		}

		swf_movie_put_bits(movie, 5, i);
		swf_movie_put_sbits(movie, i, mym->a);
		swf_movie_put_sbits(movie, i, mym->c);
	}

	/* 'Rotate' bits next... */
	if (mym->b | mym->d) {
		swf_movie_put_bits(movie, 1, 1);

		max = 0;
		i = 2;

		if (abs(mym->b) > max) {
			max = abs(mym->b);
		}
		if (abs(mym->d) > max) {
			max = abs(mym->d);
		}
		while (1 < max) {
			i++;
			max = max >> 1;
		}

		swf_movie_put_bits(movie, 5, i);
		swf_movie_put_sbits(movie, i, mym->b);
		swf_movie_put_sbits(movie, i, mym->d);
	}
	
	/* Non-optional translation bits next... */

	max = 0;
	i = 2;

	if (abs(mym->tx) > max) {
		max = abs(mym->tx);
	}
	if (abs(mym->ty) > max) {
		max = abs(mym->ty);
	}
	while (1 < max) {
		i++;
		max = max >> 1;
	}

	swf_movie_put_bits(movie, 5, i);
	swf_movie_put_sbits(movie, i, mym->tx);
	swf_movie_put_sbits(movie, i, mym->ty);

    swf_movie_flush_bits(movie);
}

void
swf_add_matrix (swf_movie * movie, int * error, swf_matrix * mym)
{
    swf_movie_initbits (movie);

	/* swf_movie_put_bits (movie, 5, mym->n_bits); */
}

void
swf_destroy_matrix (swf_matrix * matrix)
{
    if (matrix==NULL) {
        return;
    }

    swf_free (matrix);
    return;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
