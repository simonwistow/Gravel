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
 *
 * $Log: text_extract.c,v $
 * Revision 1.3  2001/07/13 14:35:46  muttley
 * Check to see if we could actually open the swf for parsing
 *
 * Revision 1.2  2001/07/09 13:51:37  muttley
 * fixed minor bug in text_extract and lib_swfextract where it would try
 * and print out strings instead of urls :(
 *
 * Revision 1.1  2001/07/09 12:47:22  muttley
 * Initial revision
 *
 */

#include "lib_swfextract.h"
#include <stdio.h>

void usage (char * name);

void
usage (char * name)
{
    fprintf (stderr, "%s <filename>\n", name);

}


/*
 * The main function
 */
int
main (int argc, char *argv[])
{
    swf_extractor * swf;
    int error = SWF_ENoError;
    int num_string, num_url, i;
    char ** strings, ** urls;

    /* Check the argument count. */
    if (argc < 2) {
        /* Bad arguments. */
        usage(argv[0]);
        return -1;
    }


    swf        = load_swf (argv[1], &error);

    if (error!=SWF_ENoError)
    {
        fprintf (stderr, "Error creating parser : %s\n", swf_error_code_to_string (error));
        return error;
    }


    num_string = get_number_strings (swf);
    num_url    = get_number_urls (swf);
    strings    = get_strings (swf);
    urls       = get_urls (swf);


    printf ("Number of strings is %d\n", num_string);


    for (i=0; i<num_string; i++)
    {
	    printf ("\t ... %s\n",strings[i]);
    }


    printf ("\n\n");





    printf ("Number of urls is %d\n", num_url);
    for (i=0; i<num_url; i++)
    {
	    printf ("\t ... %s\n",urls[i]);
    }


    destroy_swf (swf);

    return error;
}
