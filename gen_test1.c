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
 */

/* Test the background colour test flag */

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_serialise.h"
#include "swf_destroy.h"

#include <stdio.h>

void usage (char * name);

void usage (char * name) {
    fprintf (stderr, "%s <filename>\n", name);
}

int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error;
    swf_tagrecord * temp;

    error = 0;

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

    swf_make_header(movie, &error, -4000, 4000, -4000, 4000);
    movie->name = "ben1.swf\0";


    temp = swf_make_triangle(movie, &error);

    swf_add_protect(movie, &error);
    swf_add_setbackgroundcolour(movie, &error, 0, 255, 0, 255);
    //    swf_dump_shape(movie, &error, temp);

    swf_add_showframe(movie, &error);
    swf_add_end(movie, &error);

    swf_make_finalise(movie, &error);

    swf_destroy_movie(movie);

    fprintf (stderr, "OK\n");
    return 0;
}









