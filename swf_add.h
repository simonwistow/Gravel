/*
 * Copyright (C) 2003  Ben Evans <kitty@cpan.org>
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


extern swf_definebutton * swf_make_definebutton(int * error, SWF_U32 button_id);

extern void swf_add_buttonrec(swf_definebutton * button, int * error, SWF_U32 char_id, swf_matrix * matrix, SWF_U32 depth, SWF_U32 hit_test, SWF_U32 down, SWF_U32 over, SWF_U32 up);


extern void swf_add_buttonrecord (swf_definebutton * button, int * error, swf_buttonrecord * temp);
