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
 * $Log: swf_destroy.c,v $
 * Revision 1.25  2001/07/15 14:09:46  clampr
 * slice swf_parse.c and swf_destroy.c into tag/*.c files
 *
 * Revision 1.24  2001/07/14 00:17:55  clampr
 * added emacs file variables to avoid clashing with existing style (now I know what it is)
 *
 * Revision 1.23  2001/07/13 16:15:40  clampr
 * some fonts don't have bounds
 *
 * Revision 1.22  2001/07/13 13:48:08  clampr
 * doh!
 *
 * Revision 1.21  2001/07/13 13:47:02  clampr
 * move to swf_free over free
 *
 * Revision 1.20  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.19  2001/07/13 00:57:48  clampr
 * fixed a memory leak in swf_parser->font_chars deallocation
 * documented a magic number that needs slaying
 *
 * Revision 1.18  2001/07/13 00:42:16  clampr
 * more leak plugginng
 *
 * Revision 1.17  2001/07/12 23:24:29  clampr
 * fixed a leak found by kmtrace
 *
 * Revision 1.16  2001/07/12 20:27:53  clampr
 * more unchecked frees
 *
 * I guess these haven't shown themselves before now as glibc silently
 * discards free(NULL) whereas on other systems it's a definate no-no
 *
 * Revision 1.15  2001/07/12 20:08:22  clampr
 * in swf_destroy_textrecord check there are glyphs to destroy first, else you free(NULL) and badness occurs
 *
 * Revision 1.14  2001/07/09 12:47:59  muttley
 * Changes for lib_swfextract and text_extract
 *
 * Revision 1.12  2001/06/30 12:33:18  kitty_goth
 * Move to a linked list representation of shaperecords - I was getting
 * SEGFAULT due to not large enough free chunk's. Seems much faster now.
 * --Kitty
 *
 * Revision 1.11  2001/06/29 15:10:11  muttley
 * The printing of the actual text of a DefineText (and DefineText2 now)
 * is no longer such a big hack. Font information is kept in the swf_parser
 * context and the function that will take a text_record_list and print out
 * the text (textrecord_list_to_text) has been moved to swf_parse.c ...
 *
 * A couple of potential bugs have also been fixed and some more 'todo's added
 *
 * Revision 1.10  2001/06/27 12:42:15  kitty_goth
 * Debug shaperecord handling --Kitty
 *
 * Revision 1.9  2001/06/26 13:46:54  muttley
 * Changes for new swf_text_records type
 *
 */
#include "swf_destroy.h"
#include <stdlib.h>

/*
 * Todo :
 * - add in *error
 * - add banging out code
 */

/*
 * Free up the memory of context.
 */
void
swf_destroy_parser (swf_parser * context)
{
    int i;
    if (context==NULL) {
        return;
    }
    fclose(context->file);
    swf_free(context->name);
    swf_free(context->header);

    /* todo: fix this magic number */
    for (i=0; i<256; i++) {
	  swf_free (context->font_chars[i]);
    }
    swf_free(context->font_chars);
    swf_free (context);

    return;
}

void
swf_destroy_fillstyle (swf_fillstyle * style)
{
    int i;

    if (style==NULL) {
        return;
    }

    for (i=0; i<style->ncolours && style->colours!=NULL; i++) {
	swf_free (style->colours[i]);
    }
    swf_free (style->colours);

    swf_free (style->matrix);

    swf_free (style);

    return;
}
void
swf_destroy_textrecord_list (swf_textrecord_list * list)
{
    swf_textrecord *tmp, *node;
    if (list==NULL){
	return;
    }

    *(list->lastp) = NULL;
    node = list->first;

    while (node != NULL) {
        tmp = node;
        node = node->next;

	swf_destroy_textrecord(tmp);
    }
    swf_free(list);

    return;
}


void
swf_destroy_shaperecord_list (swf_shaperecord_list * list)
{
    swf_shaperecord *tmp, *node;

    if (list==NULL){
	return;
    }

    *(list->lastp) = NULL;
    node = list->first;

    while (node != NULL) {
        tmp = node;
        node = node->next;

	swf_destroy_shaperecord(tmp);
    }

    return;
}

void
swf_destroy_tag (swf_tag * tag)
{
    if (tag==NULL) {
        return;
    }
    swf_free (tag);

    return;
}

void
swf_destroy_header (swf_header * header)
{
    if (header==NULL) {
        return;
    }
    swf_destroy_rect (header->bounds);

    swf_free (header);

    return;
}

void
swf_destroy_colour (swf_colour * colour)
{
    if (colour==NULL) {
        return;
    }

    swf_free (colour);

    return;
}

void
swf_destroy_gradcolour (swf_gradcolour * colour)
{
    if (colour==NULL) {
        return;
    }

    swf_free (colour);

    return;
}

void
swf_destroy_linestyle (swf_linestyle * style)
{
    if (style==NULL) {
        return;
    }

    swf_free (style);
    return;
}

void
swf_destroy_linestyle2 (swf_linestyle2 * style)
{
    swf_free (style);

    return;
}



void
swf_destroy_fillstyle2 (swf_fillstyle2 * style)
{
    int i = 0;

    if (style==NULL) {
        return;
    }

    swf_destroy_matrix (style->matrix1);
    swf_destroy_matrix (style->matrix2);

    for (i=0; i<style->ncolours;i++) {
        swf_destroy_gradcolour (style->colours[i]);
    }

    swf_free (style->colours);

    swf_free (style);

    return;
}

void
swf_destroy_rgba_pos (swf_rgba_pos * colour)
{
    if (colour==NULL) {
        return;
    }
    swf_free (colour);

    return;
}

void
swf_destroy_soundpoint (swf_soundpoint * point)
{
    if (point==NULL) {
        return;
    }

    swf_free (point);
	return;
}

void
swf_destroy_kerningpair (swf_kerningpair * pair)
{
    if (pair==NULL) {
        return;
    }
    swf_free (pair);
    return;
}

void
swf_destroy_adpcm (swf_adpcm * adpcm)
{
    if (adpcm==NULL) {
        return;
    }
    swf_free (adpcm);
    return;
}

void
swf_destroy_mp3header (swf_mp3header * header)
{
    if (header==NULL) {
        return;
    }
    swf_free (header->data);
    swf_free (header);

    return;
}

void
swf_destroy_mp3header_list (swf_mp3header_list * list)
{
    int i=0;

    if (list==NULL) {
        return;
    }

    for (i=0; i<list->header_count; i++) {
        swf_destroy_mp3header(list->headers[i]);
    }
    swf_free (list->headers);
    swf_free (list);

    return;
}


void
swf_destroy_buttonrecord (swf_buttonrecord * record)
{
    int i;

    if (record==NULL) {
        return;
    }

    swf_destroy_matrix (record->matrix);
    for (i=0; i<record->ncharacters; i++) {
        swf_destroy_cxform (record->characters[i]);
    }
    swf_free (record->characters);
    swf_free (record);

    return;
}

void
swf_destroy_buttonrecord_list (swf_buttonrecord_list * list)
{


    swf_buttonrecord *tmp, *node;

    if (list==NULL)
    {
	    return;
    }

    *(list->lastp) = NULL;
    node = list->first;

    while (node != NULL)
    {
        tmp = node;
        node = node->next;

	    swf_destroy_buttonrecord(tmp);
    }

    swf_free (list);
    return;
}

void
swf_destroy_doaction_list (swf_doaction_list * list)
{
    swf_doaction *tmp, *node;

    if (list==NULL)
    {
	    return;
    }

    *(list->lastp) = NULL;
    node = list->first;

    while (node != NULL)
    {
        tmp = node;
        node = node->next;

	    swf_destroy_doaction(tmp);
    }

    swf_free (list);
    return;
}

void
swf_destroy_shaperecord (swf_shaperecord * record)
{
    if (record==NULL) {
        return;
    }

    swf_destroy_shapestyle (record->shapestyle);
    swf_free (record);

    return;
}


void
swf_destroy_button2action (swf_button2action * action)
{
    if (action==NULL) {
        return;
    }

    swf_destroy_doaction_list (action->doactions);
    swf_free (action);

    return;
}

void
swf_destroy_button2action_list (swf_button2action_list * list)
{
    swf_button2action * tmp, * node;

    if (list==NULL)  {
        return;
    }

    *(list->lastp) = NULL;
    node = list->first;

    while (node != NULL)
    {
        tmp = node;
        node = node->next;

	swf_destroy_button2action(tmp);
    }
    swf_free (list);

    return;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/



