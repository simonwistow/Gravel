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
 * $Log: swf_error.h,v $
 * Revision 1.6  2001/06/22 17:16:51  muttley
 * Fixed get_textrecords and get_textrecord and associated destructors and printers
 *
 */

#ifndef SWF_ERROR_H

#define SWF_ERROR_H_H

#define SWF_ENoError                        (0)
#define SWF_ENODIPSY                        (1)
#define SWF_EMallocFailure                  (2)
#define SWF_EReallocFailure                 (3)
#define SWF_EEeeeeEeeeeEeeeeDolphinNoises   (4)
#define SWF_EFileOpenFailure                (5)
#define SWF_EFileReadError                  (6)
#define SWF_ENotValidSWF                    (7)
#define SWF_EDoNotLeaveNotItIsReal          (8)
#define SWF_EFileTooSmall                   (9)
#define SWF_ENoBassInMyHeadMan              (10)
#define SWF_EInvalidMP3Frame                (11)
#define SWF_EInvalidMP3Header               (12)
#define SWF_ETooManyFriends                 (13)
#define SWF_EByGum                          (14)



extern const char * swf_error_code_to_string (int);

#endif


