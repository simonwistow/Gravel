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
 * Thin wrapper over swf_parse_defineshape_aux.
 * The spec has defineshape, defineshape2, etc but rather than
 * aping that, I think it's better to have these thin wrappers. --BE
 */

swf_defineshape *
swf_parse_defineshape (swf_parser * context, int * error)
{
    return (swf_defineshape *) swf_parse_defineshape_aux (context, error, FALSE);
}


swf_defineshape *
swf_parse_defineshape_aux (swf_parser * context, int * error, int with_alpha)
{

    swf_defineshape * shape;
	
    if ((shape = (swf_defineshape *) calloc (1, sizeof (swf_defineshape))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    shape->tagid  = (SWF_U32) swf_parse_get_word (context);
    shape->rect   = NULL;
    shape->style  = NULL;
    shape->record = NULL;

    /* Get the bounding rectangle */

    if ((shape->rect =  swf_parse_get_rect (context, error)) == NULL) {
		*error = SWF_ENotValidSWF;
        goto FAIL;
    }

	//	printf("Got rect OK..\n");

    if ((shape->style = swf_parse_get_shapestyle(context, error, with_alpha)) == NULL) {
        goto FAIL;
    }

	//	printf("Got style OK..\n");
/*
 * Bug!  this was not in the original swf_parse.cpp
 * Required to reset bit counters and read byte aligned.
 */

    swf_parse_initbits (context);

    context->fill_bits = (SWF_U16) swf_parse_get_bits (context, 4);
    context->line_bits = (SWF_U16) swf_parse_get_bits (context, 4);

/* TODO simon */
    if ((shape->record = swf_parse_get_shaperecords (context,error)) == NULL) {
        goto FAIL;
    }

	//	printf("Got shapes OK..\n");
    return shape;

 FAIL:
    swf_destroy_defineshape (shape);
    return NULL;
}


/* We need to pass the shapestyle in here as otherwise we don't know the
 * bitwidth of our lines or fills....
 */

void
swf_buffer_shaperecord(swf_buffer * buffer, int * error, swf_shaperecord * s, swf_shapestyle * st)
{
	SWF_U8 i, max;	

	swf_buffer_put_bits(buffer, 1, s->is_edge);
	
	if (!s->is_edge) {
		/* State change */
		
		printf("Putting non-edge.. flags : %u\n", s->flags);
		swf_buffer_put_bits(buffer, 5, s->flags);

        /* Are we at the end? */
        if (s->flags == 0) {
            return;
        }

        /* TODO: Process a move to. */
        if (s->flags & eflagsMoveTo) {


			/*            nbits = (SWF_U16) swf_parse_get_bits(context, 5);
            s->x = swf_parse_get_sbits(context, nbits);
            s->y = swf_parse_get_sbits(context, nbits);
            xlast = s->x;
            ylast = s->y; */
        }

        /* Process new fill info. */
        if (s->flags & eflagsFill0) {
			swf_buffer_put_bits(buffer, st->fillbits, s->fillstyle0);
        }

        if (s->flags & eflagsFill1) {
			swf_buffer_put_bits(buffer, st->fillbits, s->fillstyle1);
        }

        /* Get new line info */
        if (s->flags & eflagsLine) {
			swf_buffer_put_bits(buffer, st->linebits, s->linestyle);
        }

        /* TODO: Process new styles*/
        if (s->flags & eflagsNewStyles) {
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

			swf_buffer_put_bits(buffer, 1, 1);
			
			max = 0;
			if (abs(s->y) > max) {
				max = abs(s->y);
			}
			if (abs(s->x) > max) {
				max = abs(s->x);
			}
			
			i = 2; /* 2 for sbits, -2 from spec */
			while (1 < max) {
				i++;
				max = max >> 1;
			}
			swf_buffer_put_bits(buffer, 4, i);
			i += 2;

			if ((0 == s->x) || (0 == s->y)) {
				/* Vertical / Horizontal line */
				swf_buffer_put_bits(buffer, 1, 0);
				
				swf_buffer_put_bits(buffer, 1, 0 == s->x ? 1 : 0);
				swf_buffer_put_bits(buffer, i, s->x | s->y);

			} else {
				/* General Line */
				swf_buffer_put_bits(buffer, 1, 1);

				swf_buffer_put_sbits(buffer, i, s->x);
				swf_buffer_put_sbits(buffer, i, s->y);				
			}

		} else {
			/* TODO: Curves... */
			/* we're a curve */
			swf_buffer_put_bits(buffer, 1, 0);
			printf("Putting curve..\n");

		}
		
	}

}

void
swf_serialise_defineshape (swf_buffer * buffer, int * error, swf_defineshape * shape)
{
	SWF_U8 i, max;
	swf_shaperecord *node, *temp;

	swf_buffer_initbits(buffer);
	swf_buffer_put_word(buffer, error, shape->tagid);
	swf_buffer_rect(buffer, error, shape->rect);

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

	swf_buffer_shapestyle(buffer, error, shape->style);

	node = shape->record->first;

	while (node != NULL) {
		temp = node;
		node = node->next;

		swf_buffer_shaperecord(buffer, error, temp, shape->style);
	}

}

void
swf_destroy_defineshape (swf_defineshape * shape)
{

    if (shape==NULL) {
        return;
    }
    swf_destroy_rect (shape->rect);
	/* FIXME: URGENT : There's a bug in destroy_shapestyle which 
	 * causes swf_parse to bug out if the next line is uncommented...
	 */
    //swf_destroy_shapestyle (shape->style);
    swf_destroy_shaperecord_list (shape->record);
    swf_free(shape);
    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
