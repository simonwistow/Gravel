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
 *
 *	$Id: swf_file.h,v 1.1 2002/05/31 10:54:27 muttley Exp $
 */


#include <stdio.h>
#include "swf_types.h"

void swf_movie_initbits(swf_movie * movie);
void swf_movie_flush_bits (swf_movie * context);

void swf_movie_seek (swf_movie * context, int pos);

void swf_movie_put_byte(swf_movie * context, int * error, SWF_U8 byte);
void swf_movie_put_bytes (swf_movie * context, int * error, int nbytes, SWF_U8 * bytes);

void swf_movie_put_bits (swf_movie * context, SWF_U8 n, SWF_U32 bits);
void swf_movie_put_sbits (swf_movie * context, SWF_U8 n, SWF_S32 bits);

void swf_movie_put_word(swf_movie * context, int * error, SWF_U16 word);
void swf_movie_put_dword(swf_movie * context, int * error, SWF_U32 dword);

void swf_movie_put_string(swf_movie * context, int * error, char * mystring);
