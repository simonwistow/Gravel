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

#include "parser.h"
#include "swf_parse.h"
#include "swf_destroy.h"
#include "print_utils.h"

#define SWF_PARSER_MAX_TAG_ID 48

const char **
init_tags (void)
{

    const char ** tag  = (const char **) calloc (1 + SWF_PARSER_MAX_TAG_ID, sizeof (char *));

    tag[0] = "End";
    tag[1] = "ShowFrame";
    tag[2] = "DefineShape";
    tag[3] = "FreeCharacter";
    tag[4] = "PlaceObject";
    tag[5] = "RemoveObject";
    tag[6] = "DefineBits";
    tag[7] = "DefineButton";
    tag[8] = "JPEGTables";
    tag[9] = "SetBackgroundColor";
    tag[10] = "DefineFont";
    tag[11] = "DefineText";
    tag[12] = "DoAction";
    tag[13] = "DefineFontInfo";
    tag[14] = "DefineSound";        /* Event sound tags. */
    tag[15] = "StartSound";
    tag[17] = "DefineButtonSound";
    tag[18] = "SoundStreamHead";
    tag[19] = "SoundStreamBlock";
    tag[20] = "DefineBitsLossless"; /* A bitmap using lossless zlib compression. */
    tag[21] = "DefineBitsJPEG2";    /* A bitmap using an internal JPEG compression table. */
    tag[22] = "DefineShape2";
    tag[23] = "DefineButtonCxform";
    tag[24] = "Protect";            /* This file should not be importable for editing. */

    /*  These are the new tags for Flash 3. */
    tag[26] = "PlaceObject2";        /* The new style place w/ alpha color transform and name. */
    tag[28] = "RemoveObject2";       /* A more compact remove object that omits the character tag (just depth). */
    tag[32] = "DefineShape3";        /* A shape V3 includes alpha values. */
    tag[33] = "DefineText2";         /* A text V2 includes alpha values. */
    tag[34] = "DefineButton2";       /* A button V2 includes color transform, alpha and multiple actions */
    tag[35] = "DefineBitsJPEG3";     /* A JPEG bitmap with alpha info. */
    tag[36] = "DefineBitsLossless2"; /* A lossless bitmap with alpha info. */
    tag[37] = "DefineEditText";      /* An editable Text Field */
    tag[39] = "DefineSprite";        /* Define a sequence of tags that describe the behavior of a sprite. */
    tag[40] = "NameCharacter";       /* Name a character definition, character id and a string, (used for buttons, bitmaps, sprites and sounds). */
    tag[43] = "FrameLabel";          /* A string label for the current frame. */
    tag[45] = "SoundStreamHead2";    /* For lossless streaming sound, should not have needed this... */
    tag[46] = "DefineMorphShape";    /* A morph shape definition */
    tag[48] = "DefineFont2";

    return tag;
}

void * 
init_parser (void) {
    void (**parse)();
    void (**masked)();
	int i;

	parse  = calloc((1 + SWF_PARSER_MAX_TAG_ID), sizeof(void * ));
	masked = calloc((1 + SWF_PARSER_MAX_TAG_ID), sizeof(void * ));

	/* CHECKME:
	 * I'm not sure calloc'ing to the sizeof(void *) is 
	 * totally correct here, but it works over here. --BE
	 */

	/* Set all the parse functions to dummy, then 
	 * just add the ones we actually want...
	 */
	for (i=0; i <= SWF_PARSER_MAX_TAG_ID; i++) {
		masked[i] = dummy;
	}

	/* FIXME:
	 * Replace the hard numerical constants here with
	 * our English #define's ?
	 */

	parse[0]  = parse_end;
	parse[1]  = parse_frame;
	parse[2]  = parse_defineshape;
	parse[3]  = parse_freecharacter;
	parse[4]  = parse_placeobject;
	parse[5]  = parse_removeobject;
	parse[6]  = parse_definebits;
	parse[7]  = parse_definebutton;
	parse[8]  = parse_jpegtables;
	parse[9]  = parse_setbackgroundcolour;
	parse[10] = parse_definefont;
	parse[11] = parse_definetext;
	parse[12] = parse_doaction;
	parse[13] = parse_definefontinfo;
	parse[14] = parse_definesound;
	parse[15] = parse_startsound;
	parse[16] = dummy;
	parse[17] = parse_definebuttonsound;
	parse[18] = parse_soundstreamhead;
	parse[19] = parse_soundstreamblock;
	parse[20] = parse_definebitslossless;
	parse[21] = parse_definebitsjpeg2;
	parse[22] = parse_defineshape2;
	parse[23] = parse_definebuttoncxform;
	parse[24] = dummy;         // parse_protect
	parse[25] = dummy; 

    //  These are the new tags for Flash 3.
	parse[26] = parse_placeobject2;
	parse[27] = dummy; 
	parse[28] = parse_removeobject2;
	parse[29] = dummy; 
	parse[30] = dummy; 
	parse[31] = dummy; 
	parse[32] = parse_defineshape3;
	parse[33] = parse_definetext2;
	parse[34] = parse_definebutton2;
	parse[35] = parse_definebitsjpeg3;
	parse[36] = dummy;         // parse_definebitslossless2
	parse[37] = parse_defineedittext;
	parse[38] = dummy; 
	parse[39] = dummy;         // parse_definesprite
	parse[40] = parse_namecharacter;
	parse[41] = dummy; 
	parse[42] = dummy; 
	parse[43] = parse_framelabel;
	parse[44] = dummy; 
	parse[45] = parse_soundstreamhead2;
	parse[46] = parse_definemorphshape;
	parse[47] = dummy; 
	parse[48] = parse_definefont2; 
	
	/* 
	 * Now set up which tags we want to handle, based on some
	 * criteria 
	 */

	masked[tagDefineShape] = parse[tagDefineShape];
	masked[tagDefineShape2] = parse[tagDefineShape2];
	masked[tagDefineShape3] = parse[tagDefineShape3];

	return (void *) parse;
}


/*
 * The main function
 */

int
main (int argc, char *argv[])
{
    swf_parser * swf;
    swf_header * header;
    int        error = SWF_ENoError;

    void (**parse)();	

    const char ** tag;
    const char * str = "";


    SWF_U32 next_id;
    tag = (const char **) init_tags();


    /* Check the argument count. */
    if (argc < 2) {
        /* Bad arguments. */
        usage();
        return -1;
    }

    swf = swf_parse_create(argv[1], &error);

     parse = init_parser();

    if (swf == NULL) {
		fprintf (stderr, "Failed to create SWF context\n");
		return -1;
    }
    printf ("Name of file is '%s'\n", swf->name);

    printf("----- Reading the file header -----\n");
    header = swf_parse_header(swf, &error);

    if (header == NULL) {
        fprintf (stderr, "Failed to parse headers\n");
        goto FAIL;
    }

    printf("FWS\n");
    printf("File version \t%"pSWF_U32"\n", header->version);
    printf("File size \t%"pSWF_U32"\n", header->size);
    printf("Movie width \t%lu\n", (header->bounds->xmax - header->bounds->xmin) / 20);
    printf("Movie height \t%lu\n", (header->bounds->ymax - header->bounds->ymin) / 20);
    printf("Frame rate \t%"pSWF_U32"\n", header->rate);
    printf("Frame count \t%"pSWF_U32"\n", header->count);

	swf_destroy_header(header);
    printf("\n----- Reading movie details -----\n");

    printf("\n<----- dumping frame %d file offset 0x%04x ----->\n", 0, swf_parse_tell(swf));

    /* parse all the tags */
    do
    {

        next_id = swf_parse_nextid(swf, &error);
        if (error != SWF_ENoError) {
	        fprintf (stderr, "ERROR: There was an error parsing the next id  : '%s'\n",  swf_error_code_to_string(error));
        }

        error = SWF_ENoError;

                // printf ("%s [%"pSWF_U32"]\n", tag[next_id], next_id);
		if (next_id <= SWF_PARSER_MAX_TAG_ID) {
			parse[next_id](swf, str);
			
		}
		
		/* FIXME: 
		 * This is useless, as &error isn't an argument to the parse
		 * function. --BE
		 */
        if (error != SWF_ENoError) {
	        fprintf (stderr, "ERROR: There was an error parsing the last tag : '%s'  : '%s'\n", tag[next_id], swf_error_code_to_string(error));
        }

    }
    while (next_id > 0);

    printf("\n***** Finished Dumping SWF File Information *****\n");


    free(tag);
	free(parse);
    swf_destroy_parser(swf);

    return 0;

 FAIL:
    swf_destroy_parser(swf);
    return -1;
}


void 
dummy(swf_parser * context, const char * str)
{
	return;
}

void 
foo(swf_parser * context, const char * str)
{
	printf("\n foo: %s\n", str);
}




void
parse_frame (swf_parser * context, const char * str)
{
    context->frame++;
    printf("%stagShowFrame\n", str);
    printf("\n<----- dumping frame %"pSWF_U32" file offset 0x%04x ----->\n", context->frame, swf_parse_tell(context));
}

void
parse_end (swf_parser * context, const char * str)
{
	printf("%stagEnd\n", str);
}

void
parse_setbackgroundcolour (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
	SWF_U32 colour;

	swf_setbackgroundcolour * back = swf_parse_setbackgroundcolour (context, &error);

	if (back == NULL)
	{
		fprintf (stderr, "ERROR: couldn't parse SetBackgroundColour : '%s'\n", swf_error_code_to_string(error));
		return;
	}

	colour = (back->colour->r << 16) | (back->colour->g << 8) | back->colour->b;

	printf("%stagSetBackgroundColor \tRGB_HEX %06"pSWF_U32"x\n", str, colour);

    swf_destroy_setbackgroundcolour (back);
}




void
parse_definefont (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    int n=0;

	swf_definefont * font = swf_parse_definefont (context, &error);

	if (font==NULL)
	{
		fprintf (stderr, "ERROR: couldn't parse DefineFont  : '%s'\n", swf_error_code_to_string(error));
		return;
	}

    printf("%stagDefineFont \t\tFont ID %-5"pSWF_U32"\n", str, font->fontid);
    printf("%s\tiOffset: 0x%04x\n", str, font->offset);
	printf("%s\tnumber of glyphs: %d\n", str, font->glyph_count);

    while (n < font->glyph_count && font->shape_records[n] != NULL)
    {
        print_shaperecords (font->shape_records [n++], str);
    }

	swf_destroy_definefont (font);
}




void
parse_definefontinfo (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
	int n;
	swf_font_extra *extra;
	swf_definefontinfo * info = swf_parse_definefontinfo (context, &error);

	if (!info || error)
	{
		fprintf (stderr, "ERROR: couldn't parse DefineFontInfo : '%s'\n", swf_error_code_to_string(error));
		return;
	}

	extra = swf_fetch_font_extra(context, info->fontid, 0);

    printf("%stagDefineFontInfo \tFont ID %-5"pSWF_U32"\n", str, info->fontid);
    printf("%s\tNameLen: '%i'\n", str, info->namelen);
  	printf("%s\tFontName: '%s'\n", str, info->fontname);
	printf("%s\t", str);

	for(n=0; n < extra->n; n++)
	{
        	printf("[%d,'%c'] ", info->code_table[n], (char) info->code_table[n]);
    }

	printf("\n\n");

    swf_destroy_definefontinfo (info);
}




void
parse_placeobject (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_placeobject * place = (swf_placeobject *) swf_parse_placeobject (context, &error);

    if (place == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse PlaceObject : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagPlaceObject \ttagid %-5"pSWF_U32" depth %-5"pSWF_U32"\n", str, place->tagid, place->depth);

    print_matrix(place->matrix, str);

    if (place->cxform != NULL) {
        print_cxform(place->cxform, str);
    }

    swf_destroy_placeobject (place);
}





void
parse_placeobject2 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_placeobject2 * place = swf_parse_placeobject2 (context, &error);

    if (place == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse PlaceObject2 : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagPlaceObject2 \tflags %-5u depth %-5u ", str,  (int) place->flags, (int) place->depth);

    if (place->flags & splaceMove)
        printf("move ");

    /* Get the tag if specified. */
    if (place->flags & splaceCharacter)
    {
        printf("tag %-5"pSWF_U32"\n", place->tag);
    }
    else
    {
        printf("\n");
    }

    /* Get the matrix if specified. */
    if (place->flags & splaceMatrix)
	{
        print_matrix(place->matrix, str);
    }

    /* Get the color transform if specified. */
    if (place->flags & splaceColorTransform)
    {
        print_cxform(place->cxform, str);
    }

    /* Get the ratio if specified. */
    if (place->flags & splaceRatio)
    {
        INDENT;
        printf("ratio %"pSWF_U32"\n", place->ratio);
    }

    /* Get the clipdepth if specified. */
    if (place->flags & splaceDefineClip)
    {
        INDENT;
        printf("clipDepth %"pSWF_S32"\n", place->clip_depth);
    }

    /* Get the instance name */
    if (place->flags & splaceName)
    {
        INDENT;
        printf("instance name %s\n", place->name);
    }

    swf_destroy_placeobject2 (place);
}



void
usage (void)
{
    fprintf (stderr, "You fucked up\n");
}




void
parse_defineshape_aux (swf_parser * context, int with_alpha, const char * str)
{
    int error = SWF_ENoError;
    swf_defineshape * shape = swf_parse_defineshape_aux (context, &error, with_alpha);


    if (shape == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse DefineShape : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagDefineShape \ttagid %-5"pSWF_U32"\n", str, shape->tagid);

    print_shapestyle (shape->style, str);
	/* TODO */
    print_shaperecords (shape->record, str);

    swf_destroy_defineshape (shape);
}

void
parse_defineshape (swf_parser * context, const char * str)
{
    parse_defineshape_aux (context, FALSE, str);
}

void
parse_defineshape2 (swf_parser * context, const char * str)
{
    printf("%stagDefineShape2 -> ", str);
    parse_defineshape_aux (context, FALSE, str);
}

void
parse_defineshape3 (swf_parser * context, const char * str)
{
    printf("%stagDefineShape3 -> ", str);
    parse_defineshape_aux (context, TRUE, str);
}

void
parse_freecharacter (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_freecharacter * character = swf_parse_freecharacter (context, &error);

    if (character == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse FreeCharacter : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagFreeCharacter \ttagid %-5"pSWF_U32"\n", str, character->tagid);

    swf_destroy_freecharacter (character);
}

void
parse_removeobject (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_removeobject * object = swf_parse_removeobject (context, &error);

    if (object == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse RemoveObject : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagRemoveObject \ttagid %-5"pSWF_U32" depth %-5"pSWF_U32"\n", str, object->tagid, object->depth);

    swf_destroy_removeobject (object);
}

void
parse_removeobject2 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_removeobject2 * object = swf_parse_removeobject2 (context, &error);

    if (object == NULL)
    {
    	fprintf (stderr, "ERROR : couldn't parse RemoveObject2 : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagRemoveObject2 depth %-5"pSWF_U32"\n", str, object->depth);

    swf_destroy_removeobject2 (object);
}

void
parse_startsound (swf_parser *  context, const char * str)
{

   int error = SWF_ENoError;
    swf_startsound * sound = swf_parse_startsound (context, &error);

    printf("%stagStartSound \ttagid %-5"pSWF_U32"\n", str, sound->tagid);
    print_startsound (sound, str);

    swf_destroy_startsound (sound);
}


void
parse_stopsound (swf_parser * context, const char * str)
{
    printf("%stagStopSound\n", str);
}

void
parse_definebits (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebits * bits  = swf_parse_definebits (context, &error);

    if (bits == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineBits : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineBits \ttagid %-5"pSWF_U32"\n", str, bits->tagid);

    print_imageguts (bits->guts, str);
    swf_destroy_definebits (bits);
}

void
parse_definebitslossless (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
	int j1;
    swf_definebitslossless * bits;  

	bits = swf_parse_definebitslossless(context, &error);

	fprintf(stderr, "current pos: %i\n", swf_parse_tell(context));

    if (bits == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineBitsLossless : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineBitsLossless \ttagid %-5"pSWF_U32"\n", str, bits->tagid);

	//    print_imageguts (bits->guts, str);
    swf_destroy_definebitslossless(bits);
}


void
parse_definebitsjpeg2 (swf_parser * context, const char * str)
{
   int error = SWF_ENoError;
    swf_definebitsjpeg2 * bits = swf_parse_definebitsjpeg2 (context, &error);

    if (bits == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineBitsJPEG2 : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineBitsJpeg2 \ttagid %-5"pSWF_U32"\n", str, bits->tagid);

    print_imageguts (bits->guts, str);

    swf_destroy_definebitsjpeg2 (bits);
}

void
parse_definebitsjpeg3 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebitsjpeg3 * bits = swf_parse_definebitsjpeg3 (context, &error);

    if (bits == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineBitsJPEG3 : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineBitsJpeg3 \ttagid %-5"pSWF_U32"\n", str, bits->tagid);

    print_imageguts (bits->guts, str);

    swf_destroy_definebitsjpeg3 (bits);
}

void
parse_jpegtables (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_jpegtables * tables = swf_parse_jpegtables (context, &error);

    if (tables == NULL) {
        fprintf (stderr, "ERROR : can't parse JPEGTables : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagJPEGTables\n", str);
    print_imageguts (tables->guts, str);

    swf_destroy_jpegtables (tables);
}

void
parse_definetext (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definetext * text = swf_parse_definetext (context, &error);

    if (text == NULL) {
        fprintf (stderr, "ERROR : can't parse DefineText : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineText \t\ttagid %-5"pSWF_U32"\n", str, text->tagid);

    print_rect(text->rect, str);

    print_matrix(text->matrix, str);

    printf("%s\tnGlyphBits: nAdvanceBits:\n", str);

    print_textrecords (text->records, str, context);
    printf("\n");

    swf_destroy_definetext (text);
}

void
parse_definetext2 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definetext2 * text = swf_parse_definetext2 (context, &error);

    if (text == NULL) {
        fprintf (stderr, "ERROR : can't parse DefineText2 : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineText2 \t\ttagid %-5"pSWF_U32"\n", str, text->tagid);

    print_rect(text->rect, str);

    print_matrix(text->matrix, str);

    printf("%s\tnGlyphBits: nAdvanceBits:\n", str);

    print_textrecords (text->records, str, context);
    printf("\n");

    swf_destroy_definetext2 (text);
}

void
parse_definebutton (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebutton * button = swf_parse_definebutton (context, &error);

    if (button == NULL) {
        fprintf (stderr, "ERROR : could not parse DefineButton : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineButton \ttagid %-5"pSWF_U32"\n", str, button->tagid);

    print_buttonrecords (button->records, str);
    print_doactions (button->actions, str);

    swf_destroy_definebutton (button);
}


void
parse_definebutton2 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebutton2 * button = swf_parse_definebutton2 (context, &error);

    if (button == NULL) {
        fprintf (stderr, "ERROR : could not parse DefineButton2 : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineButton2 \ttagid %-5"pSWF_U32"\n", str, button->tagid);

    print_buttonrecords (button->records, str);
    print_button2actions (button->actions, str);

    swf_destroy_definebutton2 (button);
}



void
parse_defineedittext (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_defineedittext * text = swf_parse_defineedittext (context, &error);

    if (text == NULL) {
    	fprintf (stderr, "ERROR : couldn't parse DefineEditText : '%s'\n", swf_error_code_to_string(error));
	    return;
    }

    printf("%stagDefineEditText: tagid %"pSWF_U32" flags:%04x ", str, text->tagid, text->flags);

    if (text->flags & seditTextFlagsHasFont)
    {
        printf("FontId:%d FontHeight:%d ", text->font_id, text->font_height);
    }

    if (text->flags & seditTextFlagsHasMaxLength)
    {
        printf("length:%d ", text->max_length);
    }


    printf("variable:%s ", text->variable);

    if (text->flags & seditTextFlagsHasText )
    {
        printf("text:%s ", text->initial_text);
    }

    printf("\n");

    swf_destroy_defineedittext (text);
}


void
parse_definefont2 (swf_parser * context, const char * str)
{
    int i, n;
    int error = SWF_ENoError;
    swf_definefont2 * font = swf_parse_definefont2 (context, &error);

    if (font == NULL) {
    	fprintf (stderr, "ERROR : couldn't parse DefineFont2 : '%s'\n", swf_error_code_to_string(error));
	    return;
    }

    printf("%stagDefineFont2 \ttagid %-5"pSWF_U32" flags:%04x nGlyphs:%d\n", str, font->fontid, font->flags, font->glyph_count);

    if (font->glyph_count > 0)
    {
        /* Get the Glyphs */
        for(n=0; n<font->glyph_count; n++)
        {
            printf("\n\t%s>>> Glyph:%d", str, n);
    	    /* todo simon : printshaperecords */
        }


        /* Get the CodeTable */

        printf("\n%sCodeTable:\n%s", str, str);

        for (i=0; i<font->glyph_count; i++)
        {
            if (font->flags & sfontFlagsWideOffsets) {
                printf("%02x:[%04"pSWF_U32"x] ", i, font->code_table[i]);
            } 
			else {
                printf("%02x:[%c] ", i, (char) font->code_table[i]);
			}

            if ((i & 7) == 7) {
                printf("\n%s", str);
			}
        }
        printf("\n");
    }

    if (font->flags & sfontFlagsHasLayout)
    {
        /* Get "layout" fields */
        printf("\n%sHasLayout: iAscent:%d iDescent:%d iLeading:%d\n", str, font->ascent, font->descent, font->leading);

        /* Get BoundsTable */
        for (i=0; i<font->glyph_count; i++)
        {
            printf("rBounds: (%ld,%ld)(%ld,%ld)\n", font->bounds[i]->xmin, font->bounds[i]->ymin, font->bounds[i]->xmax, font->bounds[i]->ymax);
        }


        /* Get Kerning Pairs */
        printf("\n%sKerning Pair Count:%d\n", str, font->nkerning_pairs);
        for (i=0; i<font->nkerning_pairs; i++)
        {
            printf("%sKerningPair:%-4d %c <--> %c : %d\n", str, i, font->kerning_pairs[i]->code1, font->kerning_pairs[i]->code2, font->kerning_pairs[i]->adjust);
        }

    }

    swf_destroy_definefont2 (font);
}




void
parse_definemorphshape (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definemorphshape * shape = swf_parse_definemorphshape (context, &error);

    if (shape == NULL) {
    	fprintf (stderr, "ERROR : couldn't parse DefineMorphShape : '%s'\n", swf_error_code_to_string(error));
	    return;
    }


    printf("%stagDefineMorphShape: tagid:%"pSWF_U32"\n", str, shape->tagid);

    /* todo simon : no info is shown here - hmmm */


    printf("\n\t--- StartShape ---");
    print_shape_records (shape->records1);

    printf("\t--- EndShape ---");
    print_shape_records (shape->records2);

    swf_destroy_definemorphshape (shape);
}



void
print_shape_records (swf_shaperecord_list * records)
{
    /* todo simon */
    return;

}


void
parse_definesound (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;

    const char* Compression[3]    = {"uncompressed", "ADPCM", "MP3"};
    const char* SampleRate[4]     = {"5.5", "11", "22", "44"};

    swf_definesound * sound = swf_parse_definesound (context, &error);

    const char* SampleSize        = (sound->sample_size == 0 ? "8" : "16");
    const char* StereoMono        = (sound->stereo_mono == 0 ? "mono" : "stereo");


    if (sound == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineSound : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineSound: ", str);


    printf("%s %skHz %s-bit %s NumberOfSamples:%d (%08x)\n",
        Compression[sound->compression], SampleRate[sound->sample_rate],
        SampleSize, StereoMono, sound->sample_count, sound->sample_count);


    switch (sound->compression)
    {
	case 0:
		printf("%s  uncompressed samples\n", str);
		break;

	case 1:
		print_adpcm (sound->adpcm, str);
		break;
	
	case 2:
		printf("%s  MP3: delay:%d\n", str, sound->delay);
		print_mp3header_list (sound->mp3header_list, str);
		break;
    }

    printf ("\n");

    swf_destroy_definesound (sound);
}


void
parse_framelabel (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_framelabel *label = swf_parse_framelabel (context, &error);

    if (label == NULL) {
        fprintf (stderr, "ERROR : couldn't parse FrameLabel : '%s'\n", swf_error_code_to_string(error));
        return;
    }
    printf("%stagFrameLabel lable \"%s\"\n", str, label->label);

    swf_destroy_framelabel (label);
}


void
parse_namecharacter (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_namecharacter *name = swf_parse_namecharacter (context, &error);

    if (name == NULL) {
        fprintf (stderr, "ERROR : couldn't parse NameCharacter : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagNameCharacter \ttagid %-5"pSWF_U32" label '%s'\n", str, name->tagid, name->label);

    swf_destroy_namecharacter(name);
}


void
parse_definebuttoncxform (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebuttoncxform * button = swf_parse_definebuttoncxform (context, &error);
    int i;

    if (button == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineButtonCXform : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    for (i=0; i<button->ncxforms; i++){
        print_cxform(button->cxforms[i], str);
    }

    swf_destroy_definebuttoncxform (button);
}

void
parse_definebuttonsound (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_definebuttonsound * button = swf_parse_definebuttonsound (context, &error);

    if (button == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineButtonSound : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDefineButtonSound \ttagid %-5"pSWF_U32"\n", str, button->tagid);

    INDENT;
    printf("%supState \ttagid %-5"pSWF_U32"\n", str, button->up_state->tagid);
    if (button->up_state->tagid) {  print_startsound (button->up_state, str); }
    INDENT;
    printf("%soverState \ttagid %-5"pSWF_U32"\n", str, button->over_state->tagid);
    if (button->over_state->tagid) { print_startsound (button->over_state, str); }
    INDENT;
    printf("%sdownState \ttagid %-5"pSWF_U32"\n", str, button->down_state->tagid);
    if (button->down_state->tagid) { print_startsound (button->down_state, str); }

    swf_destroy_definebuttonsound (button);
}

void
parse_soundstreamblock (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_soundstreamblock * block = swf_parse_soundstreamblock (context, &error);

    if (block == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DefineSoundStreamBlock : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagSoundStreamBlock: ", str);

    switch (block->stream_compression)
    {
	case 0:
		printf("%s  uncompressed samples\n", str);
		break;
		
	case 1:
		print_adpcm (block->adpcm, str);
		break;
		
	case 2:
		printf("%s  MP3: SamplesPerFrame:%d Delay:%d\n", str, block->samples_per_frame, block->delay);
		print_mp3header_list (block->mp3header_list, str);
    }

    swf_destroy_soundstreamblock (block);
}

void
parse_soundstreamhead (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_soundstreamhead * head = swf_parse_soundstreamhead (context, &error);

    if (head == NULL) {
        fprintf (stderr, "ERROR : couldn't parse SoundStreamHead : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    print_soundstreamhead (head, str, FALSE);

    swf_destroy_soundstreamhead (head);
}


void
parse_soundstreamhead2 (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_soundstreamhead * head = swf_parse_soundstreamhead (context, &error);

    if (head == NULL) {
        fprintf (stderr, "ERROR : couldn't parse SoundStreamHead2 : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    print_soundstreamhead (head, str, TRUE);
    swf_destroy_soundstreamhead (head);
}

void parse_doaction (swf_parser * context, const char * str)
{
    int error = SWF_ENoError;
    swf_doaction_list * actions = swf_parse_get_doactions (context, &error);

    if (actions == NULL) {
        fprintf (stderr, "ERROR : couldn't parse DoAction : '%s'\n", swf_error_code_to_string(error));
        return;
    }

    printf("%stagDoAction\n",  str);
    print_doactions (actions, str);

    swf_destroy_doaction_list (actions);
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
