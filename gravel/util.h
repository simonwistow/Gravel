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
 *
 */

swf_colour * gravel_parse_colour (char * s);
SWF_U8 gravel_parse_hex(char * s);
swf_shaperecord * gravel_make_line (int * error, SWF_S32 x1, SWF_S32 y1, SWF_S32 x2, SWF_S32 y2);

#define MATRIX_SCALE 65536
