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
 * CHECKME
 */

swf_shaperecord *
swf_parse_get_shaperecord (swf_parser * context, int * error, int * at_end, int xlast, int ylast, int with_alpha)
{
    /* Determine if this is an edge. */
    swf_shaperecord * record;
    SWF_S16 nbits;

    if ((record = (swf_shaperecord *) calloc (1, sizeof (swf_shaperecord))) == NULL) {
        *error = SWF_EMallocFailure;
        *at_end = FALSE;
        return NULL;
    }

    record->is_edge = swf_parse_get_bits (context, 1);


    if (!record->is_edge) {
        /* Handle a state change */
        record->flags = swf_parse_get_bits(context, 5);

        /* Are we at the end? */
        if (record->flags == 0) {
            dprintf("\tEnd of shape.\n\n");
            *at_end = TRUE;
            return record;
        }

        /* Process a move to. */
        if (record->flags & eflagsMoveTo) {
            nbits = (SWF_U16) swf_parse_get_bits(context, 5);
            record->x = swf_parse_get_sbits(context, nbits);
            record->y = swf_parse_get_sbits(context, nbits);
            xlast = record->x;
            ylast = record->y;
        }

        /* Get new fill info. */
        if (record->flags & eflagsFill0) {
            record->fillstyle0 = swf_parse_get_bits(context, context->fill_bits);
        }

        if (record->flags & eflagsFill1) {
            record->fillstyle1 = swf_parse_get_bits(context, context->fill_bits);
        }
        /* Get new line info */
        if (record->flags & eflagsLine) {
            record->linestyle = swf_parse_get_bits(context, context->line_bits);
        }

        /* Check to get a new set of styles for a new shape layer. */
        if (record->flags & eflagsNewStyles) {

            /* Parse the style. */
            record->shapestyle = swf_parse_get_shapestyle(context, error, with_alpha);

            /* Reset. */
            context->fill_bits = (SWF_U16) swf_parse_get_bits (context, 4);
            context->line_bits = (SWF_U16) swf_parse_get_bits (context, 4);
        }

        *at_end = record->flags & eflagsEnd ? TRUE : FALSE;

		return record;
    } 
	else { /* not strictly needed - the if branch returned! */
        if (swf_parse_get_bits(context, 1)) {
            /* Handle a line */
            nbits = (SWF_U16) swf_parse_get_bits(context, 4) + 2;   /* nbits is biased by 2 */

            /* Save the deltas */
            if (swf_parse_get_bits(context, 1)) {
                /* Handle a general line. */
                record->x = swf_parse_get_sbits(context, nbits);
                record->y = swf_parse_get_sbits(context, nbits);
                xlast += record->x;
                ylast += record->y;

            } else {
                /* Handle a vert or horiz line. */
                if (swf_parse_get_bits(context, 1)) {
                    /* Vertical line */
                    record->y = swf_parse_get_sbits(context, nbits);
                    // record->x = NULL;
                    ylast += record->y;

                } else {
                    /* Horizontal line */
                    record->x = swf_parse_get_sbits(context, nbits);
                    // record->y = NULL;
                    xlast += record->x;
                }
            }
        } else {
            /* Handle a curve */
            nbits = (SWF_U16) swf_parse_get_bits(context, 4) + 2;   /* nBits is biased by 2 */

            /* Get the control */
            record->cx = swf_parse_get_sbits(context, nbits);
            record->cy = swf_parse_get_sbits(context, nbits);
            xlast += record->cx;
            ylast += record->cy;



            /* Get the anchor */
            record->ax = swf_parse_get_sbits(context, nbits);
            record->ay = swf_parse_get_sbits(context, nbits);
            xlast += record->ax;
            ylast += record->ay;
        }

        *at_end = FALSE;
        return record;
    }

/* TODO - not necessary ?
 FAIL:
    swf_destroy_shaperecord (record);
    return NULL;
*/
}


swf_shaperecord_list *
swf_parse_get_shaperecords (swf_parser * context, int * error)
{
    /* TODO */

    int xlast = 0;
    int ylast = 0;
    int at_end = FALSE;

    swf_shaperecord_list * list;
    swf_shaperecord * temp;


    if ((list = (swf_shaperecord_list *) calloc (1, sizeof (swf_shaperecord_list))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    list->first = NULL;
    list->lastp = &(list->first);

    while (!at_end)
    {
		if ((temp = swf_parse_get_shaperecord(context, error, &at_end, xlast, ylast, FALSE) ) == NULL)
		{
			if (*error != SWF_ENoError)
			{
				goto FAIL;
			}
			break;
		}
		
		*(list->lastp) = temp;
		list->lastp = &(temp->next);

		list->record_count++;
    }

    return list;

 FAIL:
    swf_destroy_shaperecord_list (list);
    return NULL;
}





/* We need to pass the shapestyle in here as otherwise we don't know the
 * bitwidth of our lines or fills....
 */

void
swf_buffer_shaperecord(swf_buffer * buffer, int * error, swf_shaperecord * s, swf_shapestyle * st)
{
	SWF_U8 i;	
	SWF_U32 max;

	swf_buffer_put_bits(buffer, 1, s->is_edge);
	
	if (!s->is_edge) {
		/* State change */
		
		swf_buffer_put_bits(buffer, 5, s->flags);

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
			swf_buffer_put_bits(buffer, 5, i);

			swf_buffer_put_sbits(buffer, i, s->x);
			swf_buffer_put_sbits(buffer, i, s->y);				
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
			fprintf(stderr, "Unsupported New Styles Flag\n");
        }

		return;
	} else {

		if (s->x | s->y) {
			if (s->ax | s->ay | s->cx | s->cy) {
				dprintf("Putting weirdness..\n");
				/* We're trying to be a line and a curve all at once */
				*error = SWF_ENotValidSWF;
				return;
			}

			/* we're a straight line */

			swf_buffer_put_bits(buffer, 1, 1);
			
			max = 0;
			if (abs(s->y) > max) {
				max = abs(s->y);
			}
			if (abs(s->x) > max) {
				max = abs(s->x);
			}
			
			i = 0; /* 2 for sbits, -2 from spec */
			while (1 < max) {
				i++;
				max = max >> 1;
			}
			swf_buffer_put_bits(buffer, 4, i);
			i += 2;

			if ((0 == s->x) || (0 == s->y)) {
				/* Vertical / Horizontal line */
				swf_buffer_put_bits(buffer, 1, 0);
				
				if (0 == s->x) {
					swf_buffer_put_bits(buffer, 1, 1);
					swf_buffer_put_sbits(buffer, i, s->y);
				} else {
					swf_buffer_put_bits(buffer, 1, 0);
					swf_buffer_put_sbits(buffer, i, s->x);
				}

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

			max = 0;
			if (abs(s->cy) > max) {
				max = abs(s->cy);
			}
			if (abs(s->cx) > max) {
				max = abs(s->cx);
			}
			if (abs(s->ay) > max) {
				max = abs(s->ay);
			}
			if (abs(s->ax) > max) {
				max = abs(s->ax);
			}
			
			i = 0; /* 2 for sbits, -2 from spec */
			while (1 < max) {
				i++;
				max = max >> 1;
			}
			swf_buffer_put_bits(buffer, 4, i);
			i += 2;

			swf_buffer_put_sbits(buffer, i, s->cx);
			swf_buffer_put_sbits(buffer, i, s->cy);

			swf_buffer_put_sbits(buffer, i, s->ax);
			swf_buffer_put_sbits(buffer, i, s->ay);

			/*            nbits = (SWF_U16) swf_parse_get_bits(context, 4) + 2;

            record->cx = swf_parse_get_sbits(context, nbits);
            record->cy = swf_parse_get_sbits(context, nbits);
            xlast += record->cx;
            ylast += record->cy;

            record->ax = swf_parse_get_sbits(context, nbits);
            record->ay = swf_parse_get_sbits(context, nbits);
            xlast += record->ax;
            ylast += record->ay; */

		}
		
	}

}


swf_shaperecord *
swf_make_shaperecord(int * error, int isEdge) 
{
    swf_shaperecord * record;

    if ((record = (swf_shaperecord *) calloc (1, sizeof (swf_shaperecord))) == NULL) {
      *error = SWF_EMallocFailure;
      return NULL;
    }

    record->is_edge = isEdge;
    record->flags = 0;

    record->x =0;
    record->y =0;

    record->ax =0;
    record->ay =0;
    record->cx =0;
    record->cy =0;

    return record;
}

// FIXME: Macro-ise this type of function
void 
swf_add_shaperecord (swf_shaperecord_list * list, int * error, swf_shaperecord * temp) 
{
    *(list->lastp) = temp;
    list->lastp = &(temp->next);
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
