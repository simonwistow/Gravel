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

/*
 * Shapestyle's are reasonably complicated types.
 *
 * I think what is needed here is for the ith line/fillstyle
 * to be free'd (with a check to make sure it points at something)
 *
 * There's no need to keep track of how many we've actually allocated
 * (for the case where we crash out halfway through) as we're
 * using calloc for memory allocation, so test-for-NULL will do
 * it for us.
 */

void
swf_destroy_shapestyle (swf_shapestyle * style)
{
    int i;

    if (style==NULL) {
        return;
    }

    for (i=0; i<style->nlines && style->lines[i] != NULL; i++) {
       swf_free (style->lines[i]);
    }

    swf_free (style->lines);

    for (i=0; i<style->nfills && style->fills[i] != NULL; i++) {
        swf_destroy_fillstyle (style->fills[i]);
    }
    swf_free (style->fills);

    swf_free (style);

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
swf_destroy_defineshape (swf_defineshape * shape)
{

    if (shape==NULL) {
        return;
    }
    swf_destroy_rect (shape->rect);
    swf_destroy_shapestyle (shape->style);
    swf_destroy_shaperecord_list (shape->record);
    swf_free(shape);
    return;
}

void
swf_destroy_textrecord (swf_textrecord * record)
{
    int i=0;

    if (record==NULL) {
        return;
    }

    for (i=0; i<record->glyph_count; i++) {
        swf_free (record->glyphs[i]);
    }
    swf_free (record->glyphs);
    swf_free (record);

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
swf_destroy_definemorphshape (swf_definemorphshape * shape)
{
    int i, j;

    if (shape==NULL) {
        return;
    }

    swf_destroy_rect (shape->r1);
    swf_destroy_rect (shape->r2);

    for (i=0; i<shape->nfills; i++) {
        swf_free (shape->fills[i]->matrix1);
    	swf_free (shape->fills[i]->matrix2);

	for (j=0; j<shape->fills[i]->ncolours; j++) {
	    swf_free (shape->fills[i]->colours[j]);
	}

	swf_free (shape->fills[i]->colours);
    }

    swf_free (shape->fills);

    for (i=0; i<shape->nlines; i++) {
    	swf_free (shape->lines[i]);
    }

    swf_free (shape->lines);

    swf_destroy_shaperecord_list(shape->records1);
    swf_destroy_shaperecord_list(shape->records2);

    swf_free (shape);

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
swf_destroy_rect (swf_rect * rect)
{
    if (rect==NULL) {
        return;
    }

    swf_free (rect);

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
swf_destroy_cxform (swf_cxform * cxform)
{
    if (cxform==NULL) {
        return;
    }

    swf_free (cxform);

    return;
}

void
swf_destroy_matrix (swf_matrix * matrix)
{
    if (matrix==NULL) {
        return;
    }

    swf_free (matrix);
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
swf_destroy_imageguts (swf_imageguts * guts)
{
    if (guts==NULL) {
        return;
    }

    swf_free (guts->data);
    swf_free (guts);
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
swf_destroy_doaction (swf_doaction * action)
{
    if (action==NULL)
    {
        return;
    }

    swf_free (action->url);
    swf_free (action->target);
    swf_free (action->goto_label);
    swf_free (action->push_data_string);

    swf_free (action);

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
    swf_free (list->actions);
    swf_free (list);

    return;
}

void
swf_destroy_setbackgroundcolour (swf_setbackgroundcolour * tag)
{
    if (tag==NULL) {
        return;
    }
    swf_destroy_colour (tag->colour);
    swf_free (tag);

    return;
}

void
swf_destroy_definesound (swf_definesound * sound)
{

    if (sound==NULL) {
        return;
    }
    swf_destroy_mp3header_list (sound->mp3header_list);
    swf_destroy_adpcm (sound->adpcm);
    swf_free (sound);

    return;
}

void
swf_destroy_definefont (swf_definefont * font)
{
    int i = 0;

    if (font==NULL) {
        return;
    }

    for (i=0; i<font->glyph_count && (font->shape_records != NULL); i++) {
        swf_destroy_shaperecord_list (font->shape_records[i]);
    }

    swf_free (font->shape_records);
    swf_free (font);

    return;
}

void
swf_destroy_definefont2 (swf_definefont2 * font)
{
    int i=0;

    if (font==NULL) {
        return;
    }

    for (i=0; i<font->glyph_count; i++) {
        swf_destroy_shaperecord_list (font->glyphs[i]);
        swf_destroy_rect (font->bounds[i]);
    }

    swf_free (font->glyphs);
    swf_free (font->name);
    swf_free (font->code_table);
    swf_free (font->bounds);

    for (i=0; i<font->nkerning_pairs; i++) {
        swf_destroy_kerningpair (font->kerning_pairs[i]);
    }

    swf_free (font);

    return;
}

void
swf_destroy_definefontinfo (swf_definefontinfo * info)
{
    if (info==NULL) {
        return;
    }

    swf_free (info->code_table);
    swf_free (info->fontname);
    swf_free (info);

    return;
}

void
swf_destroy_placeobject (swf_placeobject * object)
{
    if (object==NULL) {
        return;
    }

    swf_destroy_matrix (object->matrix);
    swf_destroy_cxform (object->cxform);

    swf_free (object);

    return;
}

void
swf_destroy_placeobject2 (swf_placeobject2 * object)
{
    if (object==NULL) {
        return;
    }

    swf_destroy_matrix (object->matrix);
    swf_destroy_cxform (object->cxform);

    swf_free (object->name);
    swf_free (object);

    return;
}



void
swf_destroy_freecharacter (swf_freecharacter * object)
{
    if (object==NULL) {
        return;
    }

    swf_free (object);

    return;
}

void
swf_destroy_namecharacter (swf_namecharacter * object)
{
    if (object==NULL) {
        return;
    }

    swf_free (object->label);
    swf_free (object);
	return;
}

void
swf_destroy_removeobject (swf_removeobject * object)
{
    if (object==NULL) {
        return;
    }


    swf_free (object);
	return;
}

void
swf_destroy_removeobject2 (swf_removeobject2 * object)
{
    if (object==NULL) {
        return;
    }

    swf_free (object);
	return;
}

void
swf_destroy_startsound (swf_startsound * sound)
{
    int i = 0;

    if (sound==NULL) {
        return;
    }


    for (i=0; i<sound->npoints; i++) {
        swf_destroy_soundpoint(sound->points[i]);
    }
    swf_free (sound->points);
    swf_free (sound);
	return;
}

void
swf_destroy_definebits (swf_definebits * bits)
{

    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);

    return;
}

void
swf_destroy_jpegtables (swf_jpegtables * bits)
{
    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);


    return;
}

void
swf_destroy_definebitsjpeg2 (swf_definebitsjpeg2 * bits)
{

    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);


    return;
}

void
swf_destroy_definebitsjpeg3 (swf_definebitsjpeg3 * bits)
{
    if (bits==NULL) {
        return;
    }

    swf_destroy_imageguts (bits->guts);
    swf_free (bits);


    return;
}

void
swf_destroy_definebitslossless (swf_definebitslossless * object)
{
    if (object==NULL) {
        return;
    }


    return;
}

void
swf_destroy_definetext (swf_definetext * text)
{
    if (text==NULL) {
        return;
    }

    swf_destroy_rect (text->rect);
    swf_destroy_matrix (text->matrix);
    swf_destroy_textrecord_list (text->records);
    swf_free (text);

    return;
}

void
swf_destroy_definetext2 (swf_definetext2 * text)
{
    if (text==NULL) {
        return;
    }

    swf_destroy_rect (text->rect);
    swf_destroy_matrix (text->matrix);
    swf_destroy_textrecord_list (text->records);
    swf_free (text);


    return;
}

void
swf_destroy_definebutton (swf_definebutton * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_buttonrecord_list(button->records);
    swf_destroy_doaction_list (button->actions);
    swf_free (button);

    return;
}

void
swf_destroy_definebutton2 (swf_definebutton2 * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_buttonrecord_list(button->records);
    swf_destroy_button2action_list (button->actions);
    swf_free (button);

    return;
}

void
swf_destroy_defineedittext (swf_defineedittext * text)
{

    if (text==NULL) {
        return;
    }

    swf_free (text->variable);
    swf_free (text->initial_text);

    swf_destroy_rect (text->bounds);

    swf_free (text);

    return;
}

void
swf_destroy_framelabel (swf_framelabel * label)
{
    if (label==NULL) {
        return;
    }

    swf_free (label->label);
    swf_free (label);
    return;
}

void
swf_destroy_definebuttoncxform (swf_definebuttoncxform * button)
{
    int i=0;

    if (button==NULL) {
        return;
    }


    for (i=0; i<button->ncxforms; i++) {
        swf_destroy_cxform (button->cxforms[i]);
    }
    swf_free (button->cxforms);

    swf_free (button);

	return;
}

void
swf_destroy_definebuttonsound (swf_definebuttonsound * button)
{
    if (button==NULL) {
        return;
    }

    swf_destroy_startsound (button->up_state);
    swf_destroy_startsound (button->over_state);
    swf_destroy_startsound (button->down_state);
    swf_free (button);

	return;
}

void
swf_destroy_soundstreamblock (swf_soundstreamblock * block)
{
    if (block==NULL) {
        return;
    }

    swf_destroy_adpcm (block->adpcm);
    swf_destroy_mp3header_list (block->mp3header_list);
    swf_free (block);
	return;
}

void
swf_destroy_soundstreamhead (swf_soundstreamhead * head)
{
    if (head==NULL) {
        return;
    }

    swf_free (head);

    return;
}




