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
 * $Log: lib_swfextract.c,v $
 * Revision 1.23  2002/05/09 01:33:33  clampr
 *
 * couple of dprintfs written as fprintfs, couple reversed, axed a few Log tags
 *
 * Revision 1.22  2002/05/09 01:12:02  clampr
 * valgrind found a leak
 *
 * Revision 1.21  2001/07/20 01:50:03  clampr
 * handy.h - bringing dprintf to reduce the amount of code
 *
 * Revision 1.20  2001/07/20 01:20:28  clampr
 * ignore FSCommand: urls (lots in puyopuyo.swf)
 *
 * Revision 1.19  2001/07/19 23:23:30  clampr
 * last few(?) pointer deference bugs
 *
 * Revision 1.18  2001/07/19 23:12:18  clampr
 * dereference the pointer
 *
 * Revision 1.17  2001/07/19 23:05:07  clampr
 * less returning from void functions
 *
 * Revision 1.16  2001/07/16 00:39:07  clampr
 * goddammit - dereferenced pointers!
 *
 * Revision 1.15  2001/07/16 00:37:14  clampr
 * dereference error before it's tested
 *
 * Revision 1.14  2001/07/16 00:02:37  clampr
 * pointer big
 *
 * Revision 1.13  2001/07/15 23:07:20  clampr
 * gargh, make a null pointer?
 *
 * Revision 1.12  2001/07/14 00:42:35  clampr
 * swf_realloc - does the 'right' thing for realloc(NULL, n)
 *
 * Revision 1.11  2001/07/14 00:28:07  clampr
 * spurious layout delta :)
 *
 * Revision 1.10  2001/07/14 00:24:33  clampr
 * add file variables
 * quell a warning under debug
 *
 * Revision 1.9  2001/07/13 14:34:37  muttley
 * Fix bug in parsing button2actions
 *
 * Revision 1.8  2001/07/13 14:00:42  clampr
 * s/free/swf_free/
 *
 * Revision 1.7  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.6  2001/07/12 23:00:06  clampr
 * another test before free
 *
 * Revision 1.5  2001/07/12 22:21:29  clampr
 * do the inital calloc rather than realloc(NULL, x)
 *
 * Revision 1.4  2001/07/12 22:10:50  clampr
 * off by one error
 *
 * Revision 1.3  2001/07/12 20:33:05  clampr
 * tweak around so we don't realloc NULL pointers - some reallocs *really* don't like that
 *
 * Revision 1.2  2001/07/09 13:51:37  muttley
 * fixed minor bug in text_extract and lib_swfextract where it would try
 * and print out strings instead of urls :(
 *
 * Revision 1.1  2001/07/09 12:47:06  muttley
 * Initial revision
 *
 *
 */

#include "lib_swfextract.h"
#include "handy.h"
#ifdef DEBUG
#include <stdio.h>
#include "swf_tags.h"
#endif

int current_max_strings = 0;
int current_max_urls    = 0;

/*
 * Loads an SWF file and checks that the header is valid.
 * Return an error code. 0 == No Error.
 */
swf_extractor *
load_swf (char * file, int * error)
{
    swf_header *header;	
    swf_extractor * swf;
    #ifdef DEBUG
    int i;
    #endif

    dprintf("[load_swf : mallocing]\n");

    /* instantiate the context */
    if ((swf = (swf_extractor *) calloc (1, sizeof (swf_extractor))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    /* and set everything to null */
    swf->parser      = NULL;
    swf->num_strings = 0;
    swf->strings     = NULL;
    swf->num_urls    = 0;
    swf->urls        = NULL;


    dprintf("[load_swf : loading swf '%s']\n", file);

    /* create the parser */
    swf->parser = swf_parse_create(file, error);
    if (*error!=0)
    {
        goto FAIL;
    }

    dprintf("[load_swf : parsing headers]\n");

    /* check the headers */
    header = swf_parse_header(swf->parser, error);
    if (*error!=0)
    {
        goto FAIL;
    }
    swf_destroy_header(header);

    dprintf("[load_swf : getting text]\n");

    /* parse the file and store the strings and urls in the context */
    (void) get_text (swf, error);
    if (*error !=0)
    {
        goto FAIL;
    }

    dprintf ("[load_swf : success! destroying parser]\n");

    /* everything's gone OK! */
    /* so we kill the parser since we don't need it any more */
    swf_destroy_parser(swf->parser);
    /* and set it to NULL to prevent free-ing errors later */
    swf->parser = NULL;

    #ifdef DEBUG
    dprintf ("[load_swf : number of strings is %d]\n",swf->num_strings);
    for (i=0; i<swf->num_strings; i++)
    {
        dprintf ("[load_swf : string %d = '%s']\n", i, (swf->strings)[i]);
    }

    dprintf ("\n[load_swf : number of urls is %d]\n",swf->num_urls);
    for (i=0; i<swf->num_urls; i++)
    {
        dprintf ("[load_swf : url %d = '%s']\n", i, (swf->urls)[i]);
    }

    dprintf ("[load_swf : returning]\n");
    #endif

    return swf;


    /* it's all gone horribly wrong */
 FAIL:
 
    dprintf ("[load_swf : FAIL :( destroying context]\n");

    /* clean up after ourselves */
    destroy_swf (swf);
    return NULL;
}



/* clean up after ourselves */
void
destroy_swf (swf_extractor * swf)
{
    int i;

    dprintf ("[destroy_swf : freeing %d strings]\n", swf->num_strings);

    /* delete each individual string */
    for (i=0; i< swf->num_strings; i++)
    {
	    swf_free (swf->strings[i]);
    }
    swf->num_strings = 0;
    /* delete the pointer to the strings */
    swf_free (swf->strings);

    dprintf ("[destroy_swf : freeing %d urls]\n", swf->num_urls);

    /* delete each individual url */
    for (i=0; i<swf->num_urls; i++)
    {
        swf_free (swf->urls[i]);
    }
    swf->num_urls = 0;

    /* delete the pointer to the urls */
    swf_free (swf->urls);

    dprintf ("[destroy_swf : destroying parser]\n");

    /* destroy the parser, which may be NULL */
    swf_destroy_parser (swf->parser);

    dprintf ("[destroy_swf : destroying context]\n");

    /* free the context */
    swf_free (swf);

    /* and we're done! */

}


/*
 * parse the files and extract the strings and the urls,
 * storing them in the context
 */
void
get_text (swf_extractor * swf, int * error)
{
    int next_id;

    dprintf ("[get_text : mallocing strings]\n");

    /* parse all the tags */
    do {
        /* reset the error, just to be paranoid */
        *error = SWF_ENoError;

        /* get the next id */
        next_id = swf_parse_nextid(swf->parser, error);
        /* if there's been an error, bug out */
        if (*error != SWF_ENoError) return;

        dprintf ("[get_text : next_id is %s]\n", swf_tag_to_string(next_id));

        switch (next_id)
	    {
		case tagDoAction:
			parse_doaction (swf, error);
			break;
				
		case tagDefineEditText:
			parse_defineedittext (swf, error);
			break;
			
		case tagDefineText:
			parse_definetext (swf, error);
			break;

		case tagDefineText2:
			parse_definetext2 (swf, error);
			break;

		case tagDefineButton:
			parse_definebutton (swf, error);
			break;

		case tagDefineButton2:
			parse_definebutton2 (swf, error);
			break;

		case tagDefineFont:
			parse_definefont (swf, error);
			break;
			
		case tagDefineFont2:
			parse_definefont2 (swf, error);
			break;

		case tagDefineFontInfo:
			parse_definefontinfo (swf, error);
			break;
        }
    } while ((!*error) && next_id);

    dprintf ("[get_text: finished parsing ]\n");
}


void
parse_doaction (swf_extractor * swf, int * error)
{
    swf_doaction_list * list = swf_parse_get_doactions (swf->parser, error);

    dprintf ("[parse_doaction: list is %s ]\n", (list==NULL)?"NULL":"Ok");

    if (list == NULL || *error != SWF_ENoError)
    {
        return;
    }

    examine_doactions (swf, error, list);

    swf_destroy_doaction_list (list);
}

void
examine_doactions (swf_extractor * swf, int * error, swf_doaction_list * list)
{
    swf_doaction * node;

    if (list == NULL || *error != SWF_ENoError)
    {
        return;
    }

    dprintf ("[examine_doactions : list is not null]\n");

    node = list->first;

    while (node != NULL)
    {
		/* take urls that aren't internal FSCommand: calls */
        if (node->code == sactionGetURL && strncmp("FSCommand:", node->url, 10))
        {
            dprintf ("[examine_doactions : got a URL '%s']\n", node->url);

            add_url (swf, error, node->url);
        }

        /* todo : is this necessary? */
        if ((node->code == sactionPushData) && (node->push_data_type == 0))
        {
            dprintf ("[examine_doactions : got a push_data_string '%s']\n", node->push_data_string);

            add_string (swf, error, node->push_data_string);
        }

        node = node->next;
    }
}

void
parse_defineedittext (swf_extractor * swf, int * error)
{
    swf_defineedittext * text = swf_parse_defineedittext (swf->parser, error);

    dprintf ("[parse_defineedittext: text is %s ]\n", (text==NULL)?"NULL":"Ok");

    if (text == NULL || *error != SWF_ENoError)
    {
        return;
    }

    if (text->flags & seditTextFlagsHasText )
    {
        add_string (swf, error, text->initial_text);
    }

    swf_destroy_defineedittext (text);
}

void
parse_definetext (swf_extractor * swf, int * error)
{
    swf_definetext * text = text = swf_parse_definetext (swf->parser, error);
    char * string = NULL;

    dprintf ("[parse_definetext: text is %s ]\n", (text==NULL)?"NULL":"Ok");

    if (text == NULL || *error != SWF_ENoError)
    {
        dprintf ("[parse_definetext : fail! error was '%s']\n", swf_error_code_to_string (*error));
        return;
    }

    dprintf ("[parse_definetext: getting text records ]\n");
    string = swf_parse_textrecords_to_text(swf->parser, error, text->records);

    dprintf ("[parse_definetext: got text record]\n");

    if (string==NULL)
    {
        goto FAIL;
    }

    dprintf ("[parse_definetext: text record is '%s']\n", string);

    if (*error == SWF_ENoError)
    {
        add_string (swf, error, string);
    }

    dprintf ("[parse_definetext: text record is '%s']\n", string);

    swf_free (string);

 FAIL:
    swf_destroy_definetext (text);

    dprintf ("[parse_definetext : have destroyed definetext]\n");
}

void
parse_definetext2 (swf_extractor * swf, int * error)
{
    swf_definetext2 * text = swf_parse_definetext2 (swf->parser, error);
    char * string = NULL;

    if (text == NULL || *error != SWF_ENoError)
    {
        return;
    }


    string = swf_parse_textrecords_to_text(swf->parser, error, text->records);

    if (*error == SWF_ENoError)
    {
        add_string (swf, error, string);
    }

    swf_free (string);
    swf_destroy_definetext2 (text);
}


void
parse_definebutton (swf_extractor * swf, int * error)
{
    swf_definebutton * button = swf_parse_definebutton (swf->parser, error);

    dprintf ("[parse_definebutton : button is %s]\n", (button==NULL)?"NULL":"Ok");

    if (button == NULL || *error != SWF_ENoError)
    {
        return;
    }

    dprintf ("[parse_definebutton : calling examine_doactions]\n");

    examine_doactions (swf, error, button->actions);

    swf_destroy_definebutton (button);
}


void
parse_definebutton2 (swf_extractor * swf, int * error)
{
    swf_definebutton2 * button = swf_parse_definebutton2 (swf->parser, error);
    swf_button2action * node;

    dprintf ("[definebutton2 : checking to see if valid button2]\n");

    if (!button || !button->actions|| *error != SWF_ENoError)
    {
        return;
    }

    dprintf ("[definebutton2 : examining button2 actions]\n");
    node = button->actions->first;

    dprintf ("[definebutton2 : first button2action is %s NULL]\n",(node==NULL)?"":"not ");

    while (node != NULL)
    {
        examine_doactions (swf, error, node->doactions);
        node = node->next;
    }

    swf_destroy_definebutton2 (button);
}

void
parse_definefont  (swf_extractor * swf, int * error)
{
	swf_definefont * font = swf_parse_definefont(swf->parser, error);

	if (font == NULL || *error != SWF_ENoError)
	{
		return;
	}

	swf_destroy_definefont (font);


}

void
parse_definefont2 (swf_extractor * swf, int * error)
{
    swf_definefont2 * font = swf_parse_definefont2(swf->parser, error);

    if (font == NULL || *error != SWF_ENoError)
    {
	    return;
    }

    swf_destroy_definefont2 (font);
}

void
parse_definefontinfo (swf_extractor * swf, int * error)
{

	swf_definefontinfo * info = swf_parse_definefontinfo(swf->parser, error);

	if (info == NULL || *error != SWF_ENoError)
	{
		return;
	}


    swf_destroy_definefontinfo (info);
}


void
add_string (swf_extractor * swf, int * error, char * string)
{
    dprintf ("[add_string : adding '%s']\n", string);

    add_text (error, &swf->strings, &swf->num_strings, &current_max_strings, string);
}


void
add_url (swf_extractor * swf, int * error, char * string)
{
    dprintf ("[add_url : adding '%s']\n", string);

    add_text (error, &swf->urls, &swf->num_urls, &current_max_urls, string);
}

void
add_text (int * error, char *** list, int * num, int * max, char * string)
{
    dprintf ("[add_text : adding text]\n");

    if (*num>=*max)
    {
        dprintf ("[add_text : increasing maximum limit]\n");

        *max+=255;
        /* malloc space for the strings*/
		if (!(*list = (char **) swf_realloc(*list, sizeof(char *) * *max))) {
			*error = SWF_EMallocFailure;
			return;
		}
		dprintf ("[add_text : memory (re)allocation worked]\n");
    }

    (*list)[*num] = (char *) calloc (strlen(string) + 1, sizeof (char));
    strcpy ((*list)[*num], string);

    dprintf ("[add_text : added the text '%s' the count for this list now stands at %d]\n", (*list)[*num], (*num)+1);

    (*num)++;
}


int
get_number_strings (swf_extractor * swf)
{
    return swf->num_strings;
}

int
get_number_urls (swf_extractor * swf)
{
    return swf->num_urls;
}


char **
get_strings (swf_extractor * swf)
{
    return (char **) swf->strings;
}

char **
get_urls (swf_extractor * swf)
{
    return swf->urls;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
