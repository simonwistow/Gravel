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
#include "print_utils.h"
#include "swf_destroy.h"

#include <stdio.h>

int main (int argc, char *argv[]) {
    swf_matrix *m1, *m2, *m3;
    int error;

    if ((m1 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((m2 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }
    if ((m3 = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
      error = SWF_EMallocFailure;
      return 1;
    }

    m1->a  = m1->d  = 2 * 256 * 256;
    m1->b  = m1->c  = 0;
    m1->tx = 0 * 20;
    m1->ty = 0 * 20;

    m2->a  = 1.2 * 256 * 256;
    m2->b  = 3.4 * 256 * 256;
    m2->c  = 2 * 256 * 256;
    m2->d  = 0;

    m2->tx = 0 * 20;
    m2->ty = 0 * 20;

    printf ("Matrix 1\n");
    print_matrix(m1, "aaah");
    printf ("\nMatrix 2\n");
    print_matrix(m2, "baah");

    m3 = swf_matrix_mult(m1, m2);

    printf ("\nMatrix 3\n");
    print_matrix(m3, "mewp");

    swf_free(m3);
    swf_free(m2);
    swf_free(m1);


    printf("\nDone\n");
    return 0;
}






