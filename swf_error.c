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
 * Revision 1.7  2001/06/22 17:16:51  muttley
 * Fixed get_textrecords and get_textrecord and associated destructors and printers
 *
 */
#include "swf_error.h"

const char *
swf_error_code_to_string (int code)
{

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

        default:
            return "invalid error";
    }


}


