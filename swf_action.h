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


extern void swf_action_put_play(swf_buffer * buffer, int * error);
extern void swf_action_put_gotoframe(swf_buffer * buffer, int * error, SWF_U16 frame);


extern swf_doaction * swf_make_action(int * error, SWF_U8 actionCode);
extern swf_doaction * swf_make_action_play(int * error);
extern swf_doaction * swf_make_action_gotoframe(int * error, int toFrame);
