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

#include "swf_types.h"
#include "swf_action.h"
#include "swf_buffer.h"

void swf_action_put_gotoframe(swf_buffer * buffer, int * error, SWF_U16 frame) 
{
  swf_buffer_put_byte(buffer, error, sactionGotoFrame);
  swf_buffer_put_word(buffer, error, 2);
  swf_buffer_put_word(buffer, error, frame);
}

void swf_action_put_play(swf_buffer * buffer, int * error) 
{
  swf_buffer_put_byte(buffer, error, sactionPlay);
}
