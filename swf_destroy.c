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
	swf_font_extra *extra, *last;
    if (!context) return;

    fclose(context->file);

	extra = context->font_extras;
	while (extra) {
		last = extra;
		extra = extra->next;
		swf_free(last->glyphs);
		swf_free(last->chars);
		swf_free(last);		
	}

    swf_destroy_header(context->header);
    swf_destroy_buffer(context->buffer);

	swf_free (context->src_adpcm);
    swf_free (context->name);
    swf_free (context);
}

void
swf_destroy_buffer (swf_buffer * buffy)
{
	if (buffy) { swf_free(buffy->raw); }
	swf_free(buffy);
}

/* FIXME:
 * I really want to pass shiva back as the return of this
 * function but trying to write the declaration made my head hurt
 */

void
init_destructors(void (**shiva)(), int * error) 
{

  shiva[tagDefineShape]         = swf_destroy_defineshape;
  /* Next two are test code */
  shiva[tagDefineShape2]        = swf_destroy_defineshape;
  shiva[tagDefineShape3]        = swf_destroy_defineshape;

  shiva[tagSetBackgroundColour] = swf_destroy_setbackgroundcolour;
  shiva[tagDefineButton]        = swf_destroy_definebutton;
  shiva[tagPlaceObject]         = swf_destroy_placeobject;
  shiva[tagPlaceObject2]        = swf_destroy_placeobject2;

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

	swf_free(list);
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



