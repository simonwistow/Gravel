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

#define MAX_ACTION_SIZE 8

swf_doaction *
swf_parse_get_doaction (swf_parser * context, int * error)
{
	swf_doaction * action;
	int len = 0;
	SWF_S32 pos;

	if ((action = (swf_doaction *) calloc (1, sizeof (swf_doaction))) == NULL)
	{
		*error = SWF_EMallocFailure;
		return NULL;
	}

	action->next = NULL;
	action->url  = NULL;
	action->target = NULL;
	action->goto_label = NULL;
	action->push_data_string = NULL;


	/* Handle the action */
	action->code = swf_parse_get_byte(context);
	if (action->code == 0)
	{
		return action;
	}


	if (action->code & sactionHasLength)
	{
		len = swf_parse_get_word (context);
	}

	pos = swf_parse_tell (context) + len;
	
	switch ( action->code )
	{
	case sactionGotoFrame:
		action->frame = swf_parse_get_word(context);
		break;

	case sactionGetURL:
		action->url    = swf_parse_get_string(context, error);
		action->target = swf_parse_get_string(context, error);
		break;

	case sactionWaitForFrame:
		action->frame      = swf_parse_get_word(context);
		action->skip_count = swf_parse_get_byte(context);
		break;

	case sactionSetTarget:
		action->target = swf_parse_get_string (context, error);
		break;

	case sactionGotoLabel:
		action->goto_label = swf_parse_get_string (context, error);
		break;

	case sactionWaitForFrameExpression:
		action->skip_count = swf_parse_get_byte(context);
		break;

	case sactionPushData:
		action->push_data_type = swf_parse_get_byte (context);
		
		/* property ids are pushed as floats for some reason */
		if ( action->push_data_type == 1 )
		{
			action->push_data_float.dw = swf_parse_get_dword (context);
		}
		else
			if ( action->push_data_type == 0 )
			{
				action->push_data_string = swf_parse_get_string (context, error);
			}
		break;
		
	case sactionBranchAlways:
		action->branch_offset = swf_parse_get_word(context);
		break;

	case sactionGetURL2:
		action->url2_flag = swf_parse_get_byte(context);
		break;

	case sactionBranchIfTrue:
		action->branch_offset = swf_parse_get_word(context);
		break;

	case sactionGotoExpression:
		action->stop_flag = swf_parse_get_byte(context);
		
	}

	swf_parse_seek(context, pos);
		
	return action;
}


void
swf_add_doaction (swf_movie * movie, int * error)
{
	swf_tagrecord * temp;
	SWF_U8 saction;
	SWF_U16 frame;

    temp = swf_make_tagrecord(error, tagDoAction);

    if (*error) {
		return;
    }
	/* */

    if ((temp->buffer->raw = (SWF_U8 *) calloc (MAX_ACTION_SIZE, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	// FIXME: Test code
	saction = sactionGotoFrame;
	frame = 1;

	swf_buffer_put_byte(temp->buffer, error, saction);
	swf_buffer_put_word(temp->buffer, error, 2);
	swf_buffer_put_word(temp->buffer, error, frame);

	/* End of action records marker */
	swf_buffer_put_byte(temp->buffer, error, 0);
    temp->serialised = 1;

    *(movie->lastp) = temp;
    movie->lastp = &(temp->next);

    return;
}   

void
swf_destroy_doaction (swf_doaction * action)
{
    if (action==NULL) {
        return;
    }

    swf_free (action->url);
    swf_free (action->target);
    swf_free (action->goto_label);
    swf_free (action->push_data_string);

    swf_free (action);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
