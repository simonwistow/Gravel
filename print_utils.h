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
 * $Log: print_utils.h,v $
 * Revision 1.12  2001/07/14 00:17:55  clampr
 * added emacs file variables to avoid clashing with existing style (now I know what it is)
 *
 * Revision 1.11  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.10  2001/07/09 12:47:59  muttley
 * Changes for lib_swfextract and text_extract
 *
 * Revision 1.8  2001/06/29 15:10:11  muttley
 * The printing of the actual text of a DefineText (and DefineText2 now)
 * is no longer such a big hack. Font information is kept in the swf_parser
 * context and the function that will take a text_record_list and print out
 * the text (textrecord_list_to_text) has been moved to swf_parse.c ...
 *
 * A couple of potential bugs have also been fixed and some more 'todo's added
 *
 * Revision 1.7  2001/06/26 13:45:03  muttley
 * Add facilities for converting DefineFontInfo, DefineFont and DefineText info into 'real' text
 *
 */

#include "swf_types.h"
#include "swf_parse.h"

#ifndef PRINT_UTILS_H

#define PRINT_UTILS_H

#define INDENT  printf("      ");



extern void print_cxform                (swf_cxform *, const char *);
extern void print_matrix                (swf_matrix *, const char *);
extern void print_rect                  (swf_rect   *  , const char *);
extern void print_shaperecords          (swf_shaperecord_list *, const char *);
extern void print_textrecord            (swf_textrecord *, const char *);
extern void print_textrecords           (swf_textrecord_list *, const char *, swf_parser *);
extern void print_buttonrecords         (swf_buttonrecord_list *, const char *);
extern void print_doactions             (swf_doaction_list *, const char *);
extern void print_doaction              (swf_doaction *, const char *);
extern void print_imageguts             (swf_imageguts *, const char *);
extern void print_button2actions        (swf_button2action_list *, const char *);
extern void print_button2action         (swf_button2action *, const char *);
extern void print_shape_records         (swf_shaperecord_list *);
extern void print_mp3header_list        (swf_mp3header_list *, const char *);
extern void print_adpcm                 (swf_adpcm *, const char *);
extern void print_startsound            (swf_startsound *, const char *);
extern void print_soundstreamhead       (swf_soundstreamhead *, const char *, int);
extern void print_shapestyle            (swf_shapestyle * style, const char * str);


#endif


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
