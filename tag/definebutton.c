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
#include "swf_action.h"
#include "swf_add.h"

#define MAX_BUTTON_SIZE 128

swf_definebutton *
swf_parse_definebutton (swf_parser * context, int * error)
{
    swf_definebutton * button;

    if ((button = (swf_definebutton *) calloc (1, sizeof (swf_definebutton))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid   = swf_parse_get_word (context);

    if ((button->records = swf_parse_get_buttonrecords (context, error, FALSE)) == NULL) {
        goto FAIL;
    }

    if ((button->actions = swf_parse_get_doactions (context, error)) == NULL) {
        goto FAIL;
    }

    return button;

    FAIL:
    swf_destroy_definebutton (button);
    return NULL;

}


void
swf_add_buttonrec(swf_definebutton * button, int * error, SWF_U32 char_id, 
				  swf_matrix * matrix, SWF_U32 depth, SWF_U32 hit_test, 
				  SWF_U32 down, SWF_U32 over, SWF_U32 up)
{
	swf_buttonrecord * temp;

    if ((temp = (swf_buttonrecord *) calloc (1, sizeof (swf_buttonrecord))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	temp->state_hit_test = hit_test;
	temp->state_down = down;
	temp->state_over = over;
	temp->state_up = up;

	temp->character = char_id;
	temp->layer = depth;
	temp->matrix = matrix;

    *(button->records->lastp) = temp;
    button->records->lastp = &(temp->next);	
	
	return;
}

// FIXME: Macro-ise this type of function
void 
swf_add_buttonrecord (swf_definebutton * button, int * error, swf_buttonrecord * temp) 
{
    *(button->records->lastp) = temp;
    button->records->lastp = &(temp->next);	
}


swf_definebutton *
swf_make_definebutton(int * error, SWF_U32 button_id)
{
	swf_definebutton* button;

    if ((button = (swf_definebutton *) calloc (1, sizeof (swf_definebutton))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

	button->tagid = button_id;

	return button;
}


/* FIXME: Test API while I knock this up */
// For now, just pass in a char_id, and use the same shape twice,
	// just reduce in size on MouseOver...

void 
swf_add_definebutton (swf_movie * movie, int * error, SWF_U16 button_id, SWF_U16 char_id)
{
	swf_tagrecord * temp;
	swf_matrix * m1;
	SWF_U16 depth, frame;

    if ((m1 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	depth = 1;

    temp = swf_make_tagrecord(error, tagDefineButton);

/* Define Button specifics */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (MAX_BUTTON_SIZE, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

    if (*error) {
		return;
    }

	m1->b  = m1->c  = 0;
	m1->tx = m1->ty = 100 * 20;

    swf_buffer_initbits(temp->buffer);
	swf_buffer_put_word(temp->buffer, error, button_id);

	m1->a  = m1->d  = 256 * 256;

	swf_buffer_put_bits(temp->buffer, 4, 0); /* Undocumented bits, assume 0 */
	swf_buffer_put_bits(temp->buffer, 1, 1); /* Used for HitTest? */
	swf_buffer_put_bits(temp->buffer, 1, 1); /* Used for Down? */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* Used for Over? */
	swf_buffer_put_bits(temp->buffer, 1, 1); /* Used for Up? */
    swf_buffer_flush_bits(temp->buffer);
	swf_buffer_put_word(temp->buffer, error, char_id);
	swf_buffer_put_word(temp->buffer, error, depth);
	swf_serialise_matrix(temp->buffer, error, m1);

	m1->a = m1->d = 128 * 256;

	swf_buffer_put_bits(temp->buffer, 4, 0); /* Undocumented bits, assume 0 */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* Used for HitTest? */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* Used for Down? */
	swf_buffer_put_bits(temp->buffer, 1, 1); /* Used for Over? */
	swf_buffer_put_bits(temp->buffer, 1, 0); /* Used for Up? */
    swf_buffer_flush_bits(temp->buffer);
	swf_buffer_put_word(temp->buffer, error, char_id);
	swf_buffer_put_word(temp->buffer, error, depth);
	swf_serialise_matrix(temp->buffer, error, m1);

	/* End of button records marker */
	swf_buffer_put_byte(temp->buffer, error, 0);
	
	/* Action records go here */
	/* FIXME: Test code */

	frame = 1;
	swf_action_put_gotoframe(temp->buffer, error, frame);

	/* End of action records marker */
	swf_buffer_put_byte(temp->buffer, error, 0);

    temp->serialised = 1;

	swf_dump_tag(movie, error, temp);

	swf_free(m1);

    return;
}


void 
swf_serialise_definebutton (swf_buffer * buffer, int * error, swf_definebutton * button)
{


	return;
}


void
swf_destroy_definebutton (swf_definebutton * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_buttonrecord_list(button->records);
    swf_destroy_doaction_list (button->actions);
    swf_free (button);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
