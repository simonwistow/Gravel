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
 * $Log: lib_swfextract.h,v $
 * Revision 1.1  2001/07/09 12:47:06  muttley
 * Initial revision
 *
 *
 */

//#define DEBUG

#include "swf_types.h"
#include "swf_parse.h"
#include "swf_destroy.h"

typedef struct swf_extractor  swf_extractor;

struct swf_extractor
{

    swf_parser * parser;
    int          num_strings;
    int          num_urls;
    char      ** strings;
    char      ** urls;

};

/*internally visible functions */
void get_text             (swf_extractor * swf,  int * error);
void add_string           (swf_extractor * swf,  int * error, char * string);
void add_url              (swf_extractor * swf,  int * error, char * string);
void add_text             (int * error, char *** list, int * num, int * max, char * string);


void examine_doactions    (swf_extractor * swf, int * error, swf_doaction_list * list);

void parse_doaction       (swf_extractor * swf, int * error);
void parse_defineedittext (swf_extractor * swf, int * error);
void parse_definetext     (swf_extractor * swf, int * error);
void parse_definefont     (swf_extractor * swf, int * error);
void parse_definefont2    (swf_extractor * swf, int * error);
void parse_definefontinfo (swf_extractor * swf, int * error);
void parse_definetext2    (swf_extractor * swf, int * error);
void parse_definebutton   (swf_extractor * swf, int * error);
void parse_definebutton2  (swf_extractor * swf, int * error);


/* externally visible functions */
extern swf_extractor * load_swf           (char * file_name, int * error);
extern void                 destroy_swf        (swf_extractor * swf);
extern int                  get_number_strings (swf_extractor * swf);
extern int                  get_number_urls    (swf_extractor * swf);
extern char **              get_strings        (swf_extractor * swf);
extern char **              get_urls           (swf_extractor * swf);