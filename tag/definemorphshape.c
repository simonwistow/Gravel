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

swf_definemorphshape *
swf_parse_definemorphshape (swf_parser * context, int * error)
{
    swf_definemorphshape * shape;
    SWF_U32 offset, end_shape_pos;
    int with_alpha;
    int i, j;

    if ((shape = (swf_definemorphshape *) calloc (1, sizeof (swf_definemorphshape))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }

    shape->nfills = 0;
    shape->fills  = NULL;
    shape->nlines = 0;
    shape->lines  = NULL;

    shape->tagid = swf_parse_get_word (context);

    if ((shape->r1 = swf_parse_get_rect (context, error)) == NULL) { goto FAIL; }
    if ((shape->r2 = swf_parse_get_rect (context, error)) == NULL) { goto FAIL; }

    /* Calculate the position of the end shape edges */
    offset = swf_parse_get_dword(context);
    end_shape_pos = swf_parse_tell(context) + offset;

    /* Always get RGBA not RGB for DefineMorphShape */
    with_alpha = TRUE;



    /* Get the fills */
    shape->nfills = swf_parse_get_byte (context);
    if (shape->nfills >= 255 ) {
        shape->nfills = swf_parse_get_word(context);
    }

	if (shape->nfills) {
		if ((shape->fills = (swf_fillstyle2 **) calloc (shape->nfills, sizeof (swf_linestyle2 *))) == NULL)
		{
			*error = SWF_EMallocFailure;
			goto FAIL;
		}
	}

    for (i = 0; i < shape->nfills; i++ )
    {
        if ((shape->fills[i] = (swf_fillstyle2 *) calloc (1, sizeof (swf_fillstyle2))) == NULL) {
    	    *error = SWF_EMallocFailure;
	        goto FAIL;
	    }

        shape->fills[i]->style = swf_parse_get_byte(context);
    	shape->fills[i]->colours = NULL;

        if (shape->fills[i]->style & fillGradient)
        {
            /* Gradient fill */
    	    shape->fills[i]->matrix1 = swf_parse_get_matrix (context, error);
    	    shape->fills[i]->matrix2 = swf_parse_get_matrix (context, error);

            /* Get the gradient color points */
            shape->fills[i]->ncolours = swf_parse_get_byte (context);

       	    if ((shape->fills[i]->colours = (swf_gradcolour **) calloc (shape->fills[i]->ncolours, sizeof (swf_gradcolour *))) == NULL) {
	    	    *error = SWF_EMallocFailure;
		        goto FAIL;
	        }

            for (j = 0; j < shape->fills[i]->ncolours; j++)
            {

           	    if ((shape->fills[i]->colours[j] = (swf_gradcolour *) calloc (1, sizeof (swf_gradcolour ))) == NULL) {
	                *error = SWF_EMallocFailure;
                    goto FAIL;
                 }

	    	    shape->fills[i]->colours[j]->r1 = swf_parse_get_byte (context);
		        shape->fills[i]->colours[j]->c1 = swf_parse_get_colour (context, error,  with_alpha);
		        shape->fills[i]->colours[j]->r2 = swf_parse_get_byte (context);
    	   	    shape->fills[i]->colours[j]->c2 = swf_parse_get_colour (context, error,  with_alpha);

            }
        }
        else if (shape->fills[i]->style & fillBits)
        {
            /* A bitmap fill */
            shape->fills[i]->tag = swf_parse_get_word (context); /* the bitmap tag */
    	    if ((shape->fills[i]->matrix1 = swf_parse_get_matrix (context, error)) == NULL) { goto FAIL; }
	        if ((shape->fills[i]->matrix2 = swf_parse_get_matrix (context, error)) == NULL) { goto FAIL; }

        }
        else
        {
            /* A solid color */
            shape->fills[i]->rgb1 = swf_parse_get_colour (context, error,  with_alpha);
            shape->fills[i]->rgb2 = swf_parse_get_colour (context, error,  with_alpha);
        }
    }

    /* Get the lines */
    shape->nlines = swf_parse_get_byte (context);
    if ( shape->nlines >= 255 ) {
        shape->nlines = swf_parse_get_word (context);
    }


    if ((shape->lines = (swf_linestyle2 **) calloc (shape->nlines, sizeof (swf_linestyle2 *))) == NULL)
    {
   	    *error = SWF_EMallocFailure;
	    goto FAIL;
    }

    for (i = 0; i < shape->nlines; i++ )
    {

    	if ((shape->lines[i] = (swf_linestyle2 *) calloc (1, sizeof (swf_linestyle2 ))) == NULL)
    	{
   	        *error = SWF_EMallocFailure;
    	    goto FAIL;
    	}


        /* get the thickness */
        shape->lines[i]->thick1 = swf_parse_get_word (context);
        shape->lines[i]->thick2 = swf_parse_get_word (context);

        /* get the color */
        shape->lines[i]->rgb1 = swf_parse_get_colour (context, error,  with_alpha);
        shape->lines[i]->rgb2 = swf_parse_get_colour (context, error,  with_alpha);
    }


    /* Get the bits per style index for the start shape */
    /* todo simon : fills bits in context? Not convinced */
    swf_parse_initbits (context);
    context->fill_bits = swf_parse_get_bits(context, 4);
    context->line_bits = swf_parse_get_bits(context, 4);


    /* Parse the start shape */
    if ((shape->records1 = swf_parse_get_shaperecords (context, error)) == NULL) { goto FAIL; }

    if (swf_parse_tell(context) != end_shape_pos)
    {
    	/* todo simon : probably should handle this after I've written get_shaperecords */
    }

    /*
     * Get the bits per style index for the end shape
     * THIS IS POINTLESS -- THERE ARE NO STYLES ?!
     */

    /* todo simon : fills bits in context? Not convinced */
    swf_parse_initbits (context);
    context->fill_bits = swf_parse_get_bits(context, 4); /* not sure if we should save these to n_FillBits & nLineBits */
    context->line_bits = swf_parse_get_bits(context, 4); /* there are no styles so none of this make sense. */


    /*
     * Parse the end shape
     */

    if ((shape->records2 = swf_parse_get_shaperecords (context, error)) == NULL) { goto FAIL; }
    /* todo simon : check for null returns and panic */

    return shape;

 FAIL:
    swf_destroy_definemorphshape (shape);
    return NULL;
}


void
swf_destroy_definemorphshape (swf_definemorphshape * shape)
{
    int i, j;

    if (shape==NULL) {
        return;
    }

    swf_destroy_rect (shape->r1);
    swf_destroy_rect (shape->r2);

    for (i=0; i<shape->nfills; i++) {
        swf_free (shape->fills[i]->matrix1);
    	swf_free (shape->fills[i]->matrix2);

		for (j=0; j<shape->fills[i]->ncolours; j++) {
			swf_free (shape->fills[i]->colours[j]);
		}

		swf_free (shape->fills[i]->colours);
		swf_free (shape->fills[i]);
    }

    swf_free (shape->fills);

    for (i=0; i<shape->nlines; i++) {
    	swf_free (shape->lines[i]);
    }

    swf_free (shape->lines);

    swf_destroy_shaperecord_list(shape->records1);
    swf_destroy_shaperecord_list(shape->records2);

    swf_free (shape);
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
