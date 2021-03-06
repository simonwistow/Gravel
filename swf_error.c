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
 * $Log: swf_error.c,v $
 * Revision 1.9  2001/06/29 15:10:11  muttley
 * The printing of the actual text of a DefineText (and DefineText2 now)
 * is no longer such a big hack. Font information is kept in the swf_parser
 * context and the function that will take a text_record_list and print out
 * the text (textrecord_list_to_text) has been moved to swf_parse.c ...
 *
 * A couple of potential bugs have also been fixed and some more 'todo's added
 *
 * Revision 1.8  2001/06/26 14:41:19  muttley
 * Remove debugging code
 *
 */
#include "swf_error.h"
#include <stdio.h>
#include <stdlib.h>

const char *
swf_error_code_to_string (int code)
{
    /* Code to print out the error code passed, for debugging only
     *
     * char * str = calloc (256, sizeof (char));
     * sprintf (str, "%ld", code);
     * return str;
     */

    switch (code)
    {
        case SWF_ENoError:
                return "No error";
				break;
        case SWF_EMallocFailure:
                return "Failure in memory allocation";
				break;
        case SWF_EReallocFailure:
                return "Failure in realloc";
				break;
        case SWF_EFileOpenFailure:
                return "Couldn't open file";
				break;
        case SWF_EFileReadError:
                return "Couldn't read from file";
				break;
        case SWF_ENotValidSWF:
                return "This is not a valid SWF file";
				break;
        case SWF_EFileTooSmall:
                return "This file is too small, it cannot be an SWF file";
				break;
        case SWF_EInvalidMP3Frame:
                return "Badly formed MP3 fram";
				break;
        case SWF_EInvalidMP3Header:
                return "Badly formed MP3 header";
                break;
        case SWF_EFontNotSet:
                return "Attempting to accessa  font which has not been defined yet";
                break;
        default:
            return "invalid error";
    }


}


