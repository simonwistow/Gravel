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

#include "swf_types.h"
#include "swf_movie.h"
#include "swf_error.h"
#include "swf_destroy.h"
#include "swf_serialise.h"
#include "gravel.h"

#include <stdio.h>

swf_shaperecord *
gravel_make_line (int * error, SWF_S32 x1, SWF_S32 y1, SWF_S32 x2, SWF_S32 y2)
{
	swf_shaperecord * line;

	line = swf_make_shaperecord(error, 1);
	if (*error) {
		return NULL;
	}

	line->x = x2 - x1;
	line->y = y2 - y1;

	return line;
}

swf_colour *
gravel_parse_colour (char * s)
{
	swf_colour * col;
	char * t = s;
	int has_alpha = 0;

	if ('#' != s[0]) {
		return NULL;
	}

	if (9 == strlen(s)) {
		has_alpha = 1;
	} else if (7 != strlen(s)) {
		return NULL;
	}

	if ((col = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
		return NULL;
	}

	col->r = gravel_parse_hex(++t);
	++t;
	col->g = gravel_parse_hex(++t);
	++t;
	col->b = gravel_parse_hex(++t);

	if (has_alpha) {
		++t;
		col->a = gravel_parse_hex(++t);
	} else {
		col->a = 255;
	}

	return col;
}

/* 48-57 are 0-9, 65-70 are A-F, 97-102 are a-f */

SWF_U8 
gravel_parse_hex(char * s)
{
	SWF_U8 r = 0;

	if ((s[0] > 47) && (s[0] < 58)) {
		r += s[0] - 48;
	} else if ((s[0] > 64) && (s[0] < 71)) {
		r += s[0] - 55;
	} else if ((s[0] > 96) && (s[0] < 103)) {
		r += s[0] - 87;
	} else {
		return 0;
	}

	r *= 16;

	if ((s[1] > 47) && (s[1] < 58)) {
		r += s[1] - 48;
	} else if ((s[0] > 64) && (s[1] < 71)) {
		r += s[1] - 55;
	} else if ((s[1] > 96) && (s[1] < 103)) {
		r += s[1] - 87;
	} else {
		return 0;
	}

	return r;
}

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
