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
 * $Log: swf_tags.h,v $
 * Revision 1.2  2001/07/14 00:17:55  clampr
 * added emacs file variables to avoid clashing with existing style (now I know what it is)
 *
 * Revision 1.1  2001/07/12 11:28:14  muttley
 * Initial revision
 * Removed all the tags from swf_types.h and put them in here
 * Wrote function to convert tag code to string
 *
 */

#ifndef SWF_TAGS_H
#define SWF_TAGS_H



extern const char * swf_tag_to_string (int);


#endif /* SWF_TAGS_H */


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
