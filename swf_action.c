/*
 * Copyright (C) 2002 Ben Evans <kitty@cpan.org>
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

#include <stdlib.h>

#include "swf_types.h"
#include "swf_action.h"
#include "swf_buffer.h"

#include "swf_error.h"


void 
swf_action_put_gotoframe(swf_buffer * buffer, int * error, SWF_U16 frame) 
{
  swf_buffer_put_byte(buffer, error, sactionGotoFrame);
  swf_buffer_put_word(buffer, error, 2);
  swf_buffer_put_word(buffer, error, frame);
}

void 
swf_action_put_play(swf_buffer * buffer, int * error) 
{
  swf_buffer_put_byte(buffer, error, sactionPlay);
}

swf_doaction * 
swf_make_action(int * error, SWF_U8 actionCode) 
{
  swf_doaction * context;

  if ((context = (swf_doaction *) calloc (1, sizeof (swf_doaction))) == NULL) {
    *error = SWF_EMallocFailure;
    return NULL;
  }

  context->next = NULL;
  context->code = actionCode;
  context->frame = 0;
  context->skip_count = 0;

  context->url = NULL;
  context->target = NULL;
  context->goto_label = NULL;

  return context;
}

swf_doaction * 
swf_make_action_play(int * error) 
{
  return swf_make_action(error, sactionPlay);
}

swf_doaction * 
swf_make_action_gotoframe(int * error, int toFrame) 
{
  swf_doaction * action;

  action = swf_make_action(error, sactionGotoFrame);
  action->frame = toFrame;

  return action;
}



