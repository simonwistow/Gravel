/*
 * Copyright (C) 2002  Ben Evans <kitty@cpan.org>
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
 * 	$Id: swf_buffer.h,v 1.4 2002/06/20 17:02:15 kitty_goth Exp $	
 */

#include "swf_types.h"

extern void  swf_buffer_initbits(swf_buffer * buffer);
extern void  swf_buffer_put_byte(swf_buffer * context, int * error, SWF_U8 byte);
extern void  swf_buffer_put_bytes (swf_buffer * context, int * error, int nbytes, SWF_U8 * bytes);
extern void  swf_buffer_flush_bits (swf_buffer * context);
extern void  swf_buffer_put_bits (swf_buffer * context, SWF_U8 n, SWF_U32 bits);
extern void  swf_buffer_put_sbits (swf_buffer * context, SWF_U8 n, SWF_S32 bits);
extern void  swf_buffer_put_word(swf_buffer * context, int * error, SWF_U16 word);
extern void  swf_buffer_put_dword(swf_buffer * context, int * error, SWF_U32 dword);
extern void  swf_buffer_put_string(swf_buffer * context, int * error, char * mystring);
extern void swf_buffer_colour(swf_buffer * buffer, int * error, swf_colour * col, int with_alpha);
