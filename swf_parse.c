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
 */

#include "swf_parse.h"
#include "swf_destroy.h"
#include "handy.h"


/*
 * Create a new swf parser context
 * Open the file given and initialise
 * all buffers etc.
 */
swf_parser *
swf_parse_create (char * name, int * error)
{
	swf_parser * context;
	FILE * file;

	/* Open the file for reading. */
	file = fopen(name, "rb");

	/* Did we open the file? */
	if (file == NULL) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}

	if ((context = (swf_parser *) calloc (1, sizeof (swf_parser))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
	}
	
	context->file = file;
	context->name =  (char *) strdup (name);
	context->bitpos = 0;
	context->bitbuf = 0;
	context->headers_parsed = 0;
	context->frame  = 0;

	return context;
}


/*
 * Parse the headers of the file and extract the version,
 * file size, movie width and height, frame rate and count.
 * Dump all this information in a swf_header structure
 */


swf_header *
swf_parse_header (swf_parser * context, int * error)
{
    swf_header * header;
    SWF_U8 file_hdr[8];

    if ((header = (swf_header *) calloc (1, sizeof (swf_header))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
    }

    if (fread(file_hdr, 1, 8, context->file) != 8) {
		*error = SWF_EFileReadError;
		goto FAIL;
    }

    /* Verify the header and get the file size. */
    if (file_hdr[0] != 'F' || file_hdr[1] != 'W' || file_hdr[2] != 'S' ) {
		*error = SWF_ENotValidSWF;
		goto FAIL;
    }


    /* Get the file version. */
    header->version = (SWF_U16) file_hdr[3];
    header->size    = (SWF_U32) file_hdr[4] | ((SWF_U32) file_hdr[5] << 8) | ((SWF_U32) file_hdr[6] << 16) | ((SWF_U32) file_hdr[7] << 24);

    if (header->size < 21) {
		*error = SWF_EFileTooSmall;
		goto FAIL;
    }

    /* Reposition to the 8th byte in the stream */
    swf_parse_seek (context, 8);

    /* get the frame information. */
    header->bounds = swf_parse_get_rect(context, error);

    header->rate   = swf_parse_get_word(context) >> 8;
    header->count  = swf_parse_get_word(context);

    context->headers_parsed++;

    context->next_tag_pos = swf_parse_tell (context);

    return header;

 FAIL:
    swf_destroy_header (header);
    return NULL;

}

/*
 * Reset the parse head to a byte boundary in the stream.
 * Clear the buffer.
 * (ie a zero offset in bits)
 */

void
swf_parse_initbits (swf_parser * context)
{
    /* Reset the bit position and buffer. */
    context->bitpos = 0;
    context->bitbuf = 0;
}

/*
 * Get an 8-bit byte from the stream, and move the
 * parse head on by one.
 */

SWF_U8
swf_parse_get_byte(swf_parser * context)
{
    SWF_U8 byte;

    swf_parse_initbits(context);
    fread (&byte, 1, 1, context->file);
    context->filepos++;

    return byte;
}

/*
 * Ensure the parse head is byte-aligned and read the specified
 * number of bytes from the stream.
 */

SWF_U8 *
swf_parse_get_bytes (swf_parser * context, int nbytes)
{
    SWF_U8 * bytes;

    if ((bytes = (SWF_U8 *) calloc (nbytes, sizeof (SWF_U8))) == NULL) {
        /* todo : pass back errors ?*/
        return NULL;
    }

    swf_parse_initbits(context);

    fread (bytes, 1, nbytes, context->file);
    context->filepos += nbytes;

    return bytes;
}


/*
 * Get n bits from the stream.
 */

SWF_U32
swf_parse_get_bits (swf_parser * context, SWF_S32 n)
{
    SWF_U32 v = 0;
	
    while (1) {
        SWF_S32 s = n - context->bitpos;
        if (s > 0) {
            /* Consume the entire buffer */
            v |= context->bitbuf << s;
            n -= context->bitpos;
			
            /* Get the next buffer */
            context->bitbuf = swf_parse_get_byte(context);
            context->bitpos = 8;
        } else {
            /* Consume a portion of the buffer */
            v |= context->bitbuf >> -s;
            context->bitpos -= n;
            context->bitbuf &= 0xff >> (8 - context->bitpos); /* mask off the consumed bits. */

            return v;
        }
    }
}


/*
 * Get n bits from the stream with sign extension.
 */

SWF_S32
swf_parse_get_sbits(swf_parser * context, SWF_S32 n)
{
    /* Get the number as an unsigned value. */
    SWF_S32 v = (SWF_S32) swf_parse_get_bits(context, n);

    /* Is the number negative? */
    if (v & (1L << (n - 1))) {
        /* Yes. Extend the sign. */
        v |= -1L << n;
    }

    return v;
}


/*
 * Get a 16-bit word from the stream, and move the
 * parse head on by two.
 */

SWF_U16
swf_parse_get_word(swf_parser * context)
{
    swf_parse_initbits(context);

    return (SWF_U16) swf_parse_get_byte(context) | ((SWF_U16) swf_parse_get_byte(context) << 8);
}

/*
 * Get a 32-bit dword from the stream, and move the
 * parse head on by four.
 */

SWF_U32
swf_parse_get_dword(swf_parser * context)
{
    swf_parse_initbits(context);

    return (SWF_U32) swf_parse_get_byte(context) | ((SWF_U32) swf_parse_get_byte(context) << 8) | ((SWF_U32) swf_parse_get_byte(context) << 16) | ((SWF_U32) swf_parse_get_byte(context) << 24);
}

/*
 * Parse the ID code of the next tag from the stream.
 * Extract the tag length and use it to tell our context what
 * stream position the next tag starts at.
 *
 * Return the ID code of the tag.
 */

SWF_U32
swf_parse_nextid(swf_parser * context, int * error)
{
    SWF_U16 raw_code, id;

    if (context->headers_parsed == 0) {
        fprintf (stderr, "Headers, not parsed yet\n");
        return -1;
    }

    if (swf_parse_tell(context) != context->next_tag_pos) {
        swf_parse_seek(context, context->next_tag_pos);
    }

    /* Get the combined code and length of the tag. */
    raw_code = swf_parse_get_word(context);
    id = raw_code;

    /* The length is encoded in the tag. */
    context->cur_tag_len = id & 0x3f;

    /* Remove the length from the code. */
    id = id >> 6;

    /* Determine if another long word must be read to get the length. */
    if (context->cur_tag_len == 0x3f) {
        /* Long tag .. */
        context->cur_tag_len = (SWF_U32) swf_parse_get_dword(context);
    }

    context->next_tag_pos = swf_parse_tell(context) + context->cur_tag_len;

    return id;
}


/*
 * Seek a defined position in the stream. Useful for,
 * eg skipping over the header information to begin tag
 * parsing. The argument must be an absolute value of 8-bit
 * bytes in the stream.
 */

void
swf_parse_seek (swf_parser * context, int pos)
{
	fseek(context->file, pos, SEEK_SET);
	return;
}

/*
 * Skips ahead by the specified number of 8-bit
 * bytes. Presumably a negative offset skips back.
 */

void
swf_parse_skip (swf_parser * context, int bytes)
{
    swf_parse_seek (context, swf_parse_tell(context) + bytes);
    return;
}

/*
 * Returns the current offset within the stream.
 */

int
swf_parse_tell (swf_parser * context)
{
    return (int) ftell (context->file);
}

/* nsyms also flags creation */
swf_font_extra*
swf_fetch_font_extra(swf_parser* context, int fontid, int nsyms)
{
	swf_font_extra *extra, *last;

	last = NULL;
	extra = context->font_extras;
	while (extra) {
		if (fontid == extra->fontid) return extra;
		last = extra;
		extra = extra->next;
	}
	if (!nsyms) return NULL;
	if (!((extra = calloc(1, sizeof(swf_font_extra))) &&
		  (extra->glyphs = calloc(nsyms, sizeof(char))) &&
		  (extra->chars  = calloc(nsyms, sizeof(char))) ))
	{
		return NULL;
	}

	extra->fontid = fontid;
	extra->n = nsyms;
	if (last) {
		last->next = extra;
	}
	else {
		context->font_extras = extra;
	}
	
	return extra;
}


/*
 * Grabs a string of chars out of the stream.
 * NOTE - uses a hard-coded value of 255 for the maximum
 * length of a string buffer.
 */

char *
swf_parse_get_string (swf_parser * context, int * error)
{

    //todo : h-h-h-hack?

    char * string;
    char byte = (char) NULL;
    int sp    = 0;

    if ( (string = (char *) calloc( 255, sizeof(char) )) == NULL) {
        *error = SWF_EMallocFailure;
	return NULL;
    }


    while (sp < 255 ) {
	/** string++ = byte; */
	byte = (char) swf_parse_get_byte(context);
	if (byte == (char) NULL) {
	    break;
	}
	string[sp++] = byte;
	byte = (char) NULL;
    }

    string[sp]  = '\0';

    return string;
}


SWF_U32
swf_parse_get_colour (swf_parser * context, int * error, int with_alpha)
{
    SWF_U32 r,g,b,a;

    r = swf_parse_get_byte(context);
    g = swf_parse_get_byte(context);
    b = swf_parse_get_byte(context);
    a = 0xff;

    if (with_alpha) {
        a = swf_parse_get_byte(context);
    }

    return (a << 24) | (r << 16) | (g << 8) | b;
}


/*
 * ActionScript Condition handler
 */
swf_button2action_list *
swf_parse_get_button2actions (swf_parser * context, int * error)
{
    swf_button2action_list * list;
    swf_button2action      * temp;
    SWF_U32                  action_offset = 0;
    SWF_U32                  next_action   = 0;
    int test = 0;

    if ((list = (swf_button2action_list *) calloc (1, sizeof(swf_button2action_list))) == NULL)
    {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    list->first = NULL;
    list->lastp = &(list->first);



    while (TRUE)
    {
		action_offset = swf_parse_get_word (context);
		next_action   = swf_parse_tell (context) + action_offset - 2;
		
        dprintf ("[get_button2action_list : action_offset  = %lx %lx (%d)]\n", action_offset, next_action, test);

		if ((temp  = (swf_button2action *) calloc (1, sizeof(swf_button2action))) == NULL)
		{
            *error = SWF_EMallocFailure;
            goto FAIL;
		}


		temp->condition = swf_parse_get_word (context);
		if ((temp->doactions = (swf_doaction_list *) swf_parse_get_doactions (context, error)) == NULL)
		{
            swf_free (temp);
            goto FAIL;
		}

         *(list->lastp) = temp;
	     list->lastp = &(temp->next);


         if (action_offset == 0)
         {
			 break;
         }

         swf_parse_seek (context, next_action);
         test++;
    }


    return list;

    FAIL:
    swf_destroy_button2action_list (list);
    return NULL;
}


/*
 * ActionScript handler.
 */
swf_doaction_list *
swf_parse_get_doactions (swf_parser * context, int * error)
{
    swf_doaction_list * list;
    swf_doaction      * temp;

    if ((list = (swf_doaction_list *) calloc (1, sizeof (swf_doaction_list))) == NULL)
    {
        *error = SWF_EMallocFailure;
        goto FAIL;
    }

    list->first = NULL;
    list->lastp = &(list->first);


    do
    {
        if ((temp = swf_parse_get_doaction(context, error)) == NULL)
        {
            goto FAIL;
        }

         *(list->lastp) = temp;
	     list->lastp = &(temp->next);
    } while (temp->code != 0x00);

    return list;

    FAIL:
    swf_destroy_doaction_list (list);
    return NULL;
}





swf_textrecord_list  *
swf_parse_get_textrecords (swf_parser * context, int * error, int has_alpha, int glyph_bits, int advance_bits)
{
    swf_textrecord_list * list;
    swf_textrecord * temp;

    dprintf ("[get_textrecords : mallocing]\n");

    if ((list = (swf_textrecord_list *) calloc (1, sizeof (swf_textrecord_list))) == NULL)
    {
        goto FAIL;
    }

    list->first = NULL;
    list->lastp = &(list->first);

    while (1)
    {
		dprintf ("[get_textrecords : attempting to get  a record]\n");

		if ((temp = swf_parse_get_textrecord(context, error, has_alpha, glyph_bits, advance_bits)) == NULL)
		{
			//if (error != SWF_ENoError)
			//{
			//    goto FAIL;
			//}
			break;
		}

        dprintf ("[get_textrecords : got a record]\n");

		*(list->lastp) = temp;
		list->lastp = &(temp->next);
    }

    dprintf ("[get_textrecords : returning]\n");

    return list;

 FAIL:
    dprintf ("[get_text_records : FAILED!]\n");
    swf_destroy_textrecord_list (list);
    return NULL;
}


swf_shaperecord_list *
swf_parse_get_shaperecords (swf_parser * context, int * error)
{
    /* TODO */

    int xlast = 0;
    int ylast = 0;
    int at_end = FALSE;

    swf_shaperecord_list * list;
    swf_shaperecord * temp;


    if ((list = (swf_shaperecord_list *) calloc (1, sizeof (swf_shaperecord_list))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    list->first = NULL;
    list->lastp = &(list->first);

    while (!at_end)
    {
		if ((temp = swf_parse_get_shaperecord(context, error, &at_end, xlast, ylast, FALSE) ) == NULL)
		{
			if (*error != SWF_ENoError)
			{
				goto FAIL;
			}
			break;
		}
		
		*(list->lastp) = temp;
		list->lastp = &(temp->next);

		list->record_count++;
    }

    return list;

 FAIL:
    swf_destroy_shaperecord_list (list);
    return NULL;
}


swf_buttonrecord_list *
swf_parse_get_buttonrecords (swf_parser * context, int * error, int with_alpha)
{

    SWF_U32 button_end;
    swf_buttonrecord_list * list;
    swf_buttonrecord      * temp;

    if ((list = (swf_buttonrecord_list *) calloc (1, sizeof (swf_buttonrecord_list))) == NULL)
    {
        goto FAIL;
    }

    list->first = NULL;
    list->lastp = &(list->first);

    button_end = swf_parse_get_byte (context);

    do
    {
        if ((temp = swf_parse_get_buttonrecord(context, error, button_end, with_alpha)) == NULL)
        {
            goto FAIL;
			
        }

		*(list->lastp) = temp;
		list->lastp = &(temp->next);
    }
    while ((button_end = (SWF_U32) swf_parse_get_byte(context)) != 0);

    return list;

 FAIL:
    swf_destroy_buttonrecord_list (list);
    return NULL;
}


swf_buttonrecord *
swf_parse_get_buttonrecord (swf_parser * context, int * error, int byte, int with_alpha)
{
    swf_buttonrecord * button;
    SWF_U32 pad;
    int i;

    if ((button = (swf_buttonrecord *) calloc (1, sizeof (swf_buttonrecord))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    pad = byte >> 4;
    button->state_hit_test = (byte & 0x8);
    button->state_down     = (byte & 0x4);
    button->state_over     = (byte & 0x2);
    button->state_up       = (byte & 0x1);

    button->character = swf_parse_get_word (context);
    button->layer     = swf_parse_get_word (context);


    if ((button->matrix = swf_parse_get_matrix(context, error)) == NULL) {
        goto FAIL;
    }


    button->ncharacters = 0;
    button->characters = NULL;

    if (with_alpha) {
        /* charactersInButton always seems to be one */
        button->ncharacters = 1;

        if ((button->characters = (swf_cxform **) calloc (button->ncharacters, sizeof (swf_cxform *))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }

        for (i=0; i<button->ncharacters; i++)
        {
            button->characters[i] = NULL;
            if ((button->characters[i] = swf_parse_get_cxform (context, error, TRUE)) == NULL)  /* could be false here? */
            {
                goto FAIL;
            }

        }
    }

    return button;

 FAIL:
    swf_destroy_buttonrecord (button);
    return NULL;
}

/*
 * CHECKME
 */

swf_shaperecord *
swf_parse_get_shaperecord (swf_parser * context, int * error, int * at_end, int xlast, int ylast, int with_alpha)
{
    /* Determine if this is an edge. */
    swf_shaperecord * record;
    SWF_S16 nbits;

    if ((record = (swf_shaperecord *) calloc (1, sizeof (swf_shaperecord))) == NULL) {
        *error = SWF_EMallocFailure;
        *at_end = FALSE;
        return NULL;
    }

    record->is_edge = swf_parse_get_bits (context, 1);


    if (!record->is_edge) {
        /* Handle a state change */
        record->flags = swf_parse_get_bits(context, 5);

        /* Are we at the end? */
        if (record->flags == 0) {
            dprintf("\tEnd of shape.\n\n");
            *at_end = TRUE;
            return record;
        }

        /* Process a move to. */
        if (record->flags & eflagsMoveTo) {
            nbits = (SWF_U16) swf_parse_get_bits(context, 5);
            record->x = swf_parse_get_sbits(context, nbits);
            record->y = swf_parse_get_sbits(context, nbits);
            xlast = record->x;
            ylast = record->y;
        }

        /* Get new fill info. */
        if (record->flags & eflagsFill0) {
            record->fillstyle0 = swf_parse_get_bits(context, context->fill_bits);
        }

        if (record->flags & eflagsFill1) {
            record->fillstyle1 = swf_parse_get_bits(context, context->fill_bits);
        }
        /* Get new line info */
        if (record->flags & eflagsLine) {
            record->linestyle = swf_parse_get_bits(context, context->line_bits);
        }

        /* Check to get a new set of styles for a new shape layer. */
        if (record->flags & eflagsNewStyles) {

            /* Parse the style. */
            record->shapestyle = swf_parse_get_shapestyle(context, error, with_alpha);

            /* Reset. */
            context->fill_bits = (SWF_U16) swf_parse_get_bits (context, 4);
            context->line_bits = (SWF_U16) swf_parse_get_bits (context, 4);
        }

        *at_end = record->flags & eflagsEnd ? TRUE : FALSE;

		return record;
    } 
	else { /* not strictly needed - the if branch returned! */
        if (swf_parse_get_bits(context, 1)) {
            /* Handle a line */
            nbits = (SWF_U16) swf_parse_get_bits(context, 4) + 2;   /* nbits is biased by 2 */

            /* Save the deltas */
            if (swf_parse_get_bits(context, 1)) {
                /* Handle a general line. */
                record->x = swf_parse_get_sbits(context, nbits);
                record->y = swf_parse_get_sbits(context, nbits);
                xlast += record->x;
                ylast += record->y;

            } else {
                /* Handle a vert or horiz line. */
                if (swf_parse_get_bits(context, 1)) {
                    /* Vertical line */
                    record->y = swf_parse_get_sbits(context, nbits);
                    // record->x = NULL;
                    ylast += record->y;

                } else {
                    /* Horizontal line */
                    record->x = swf_parse_get_sbits(context, nbits);
                    // record->y = NULL;
                    xlast += record->x;
                }
            }
        } else {
            /* Handle a curve */
            nbits = (SWF_U16) swf_parse_get_bits(context, 4) + 2;   /* nBits is biased by 2 */

            /* Get the control */
            record->cx = swf_parse_get_sbits(context, nbits);
            record->cy = swf_parse_get_sbits(context, nbits);
            xlast += record->cx;
            ylast += record->cy;



            /* Get the anchor */
            record->ax = swf_parse_get_sbits(context, nbits);
            record->ay = swf_parse_get_sbits(context, nbits);
            xlast += record->ax;
            ylast += record->ay;
        }

        *at_end = FALSE;
        return record;
    }

/* TODO - not necessary ?
 FAIL:
    swf_destroy_shaperecord (record);
    return NULL;
*/
}



char *
swf_parse_textrecords_to_text         (swf_parser * context, int * error, swf_textrecord_list * list)
{
    int g;
    int font_id = -1;
    char * str = NULL;
    swf_textrecord *node, *tmp;
	swf_font_extra *font;

    if (list==NULL)
    {
        /* todo simon : should eal with thsi more cleanly */
        return NULL;
    }

    dprintf ("[textrecords_to_text : list is not null]\n");
    node = list->first;

    dprintf ("[textrecords_to_text : node is %s]\n", (node==NULL)?"NULL":"Ok");

    /* todo simon : if node == NULL need to cope */

    while (node != NULL)
    {
        dprintf ("[textrecords_to_text : node is not NULL]\n");

        if (node->flags & isTextControl)
        {
            dprintf ("[textrecords_to_text : it's a text control]\n");

            if ( node->flags & textHasFont)
            {
                font_id = node->font_id;
                dprintf ("[textrecords_to_text : font_id is %d]\n", font_id);
            }
        }
		else {
            dprintf ("[textrecords_to_text : mallocing string %d : fontid now %d]\n", node->glyph_count, font_id);

            /* malloc to the size of the string */
            if ((str = (char *) calloc (node->glyph_count+1, sizeof (char))) == NULL)
            {
                *error = SWF_EMallocFailure;
                return NULL;
            }

            dprintf ("[textrecords_to_text : malloced string]\n");

            if (!(font = swf_fetch_font_extra(context, font_id, 0)))
            {
                *error = SWF_EFontNotSet;
                return NULL;
            }

            dprintf ("[textrecords_to_text : number of glyphs is  %d]\n", node->glyph_count);
			
            /* ... and then set it */
            for (g=0; g< node->glyph_count; g++)
            {
				str[g] = font->chars[node->glyphs[g][0]];
            }
            str[g]='\0';

            dprintf ("[textrecords_to_text : string is  '%s']\n", str);

            /* todo :sometimes there's more than one peice of text ... we shoudl probably cope with that as well */
            return str;
        }


        tmp = node;
        node = node->next;
    }


    dprintf ("[textrecords_to_text : attempting to return]\n");

    if (font_id == -1) {
        /* somethings gone wrong */
        dprintf (stderr, "[textrecords_to_text : EFontNotSet]\n");
        //todo simon : why is this not working *error = SWF_EFontNotSet;
        return NULL;
    }

    if (str == NULL) {
        /* somethings gone wrong */
        dprintf ("[textrecords_to_text : EStringNotSet]\n");
        *error = SWF_EFontNotSet; /*todo :  need to change error message */
        return NULL;
    }
    /* et voila */

    dprintf ("[textrecords_to_text : returning]\n");

    return str;
}



/*
 * $Log: swf_parse.c,v $
 * Revision 1.45  2001/07/23 22:34:45  muttley
 * Fix warning about comment like text in the logs
 *
 * Revision 1.44  2001/07/20 01:59:18  clampr
 * move cvs log to the end of the file
 *
 * switch to handy.h, dprintf tidy
 *
 * Revision 1.43  2001/07/16 15:05:15  clampr
 * get rid of glib due to randomness (I suspect it may have been a dynamic linking issue)
 *
 * add in a homebrew linked list type for font_extras (ick)
 *
 * Revision 1.42  2001/07/16 01:41:25  clampr
 * glib version of font management
 *
 * Revision 1.41  2001/07/15 15:12:53  clampr
 * move the mp3 stuff to definesound
 *
 * Revision 1.40  2001/07/15 14:09:46  clampr
 * slice swf_parse.c and swf_destroy.c into tag/.c files
 *
 * Revision 1.39  2001/07/14 23:45:48  clampr
 * one bigass layout delta
 *
 * Revision 1.38  2001/07/14 23:28:01  clampr
 * spurious line deltas 'o the day
 *
 * Revision 1.37  2001/07/14 23:17:11  clampr
 * tweak up some layout
 *
 * Revision 1.36  2001/07/14 23:03:18  clampr
 * avoid a calloc(0)
 *
 * Revision 1.35  2001/07/14 22:14:09  clampr
 * avoid calloc(0)
 *
 * Revision 1.34  2001/07/14 21:35:02  clampr
 * swf_parse_definebuttonsound don't free something being returned
 *
 * Revision 1.33  2001/07/14 00:17:55  clampr
 * added emacs file variables to avoid clashing with existing style (now I know what it is)
 *
 * Revision 1.32  2001/07/13 15:58:08  muttley
 * Bad Simon! Don't turn on DEBUG in a file, use make
 *
 * Revision 1.31  2001/07/13 15:22:07  clampr
 * another realloc of NULL
 *
 * Revision 1.30  2001/07/13 14:58:44  clampr
 * avoid a calloc(0)
 *
 * Revision 1.29  2001/07/13 14:51:41  clampr
 * paper over some cracks fond in 3deng_01.swf
 *
 * Revision 1.28  2001/07/13 14:47:34  clampr
 * malloc the right type
 *
 * Revision 1.27  2001/07/13 14:35:18  muttley
 * Fix bug in parsing button2actions
 *
 * Revision 1.26  2001/07/13 14:02:51  clampr
 * s/free/swf_free/
 *
 * Revision 1.25  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.23  2001/07/13 00:57:48  clampr
 * fixed a memory leak in swf_parser->font_chars deallocation
 * documented a magic number that needs slaying
 *
 * Revision 1.22  2001/07/13 00:32:31  clampr
 * juggle to avoid calloc(0, sizeof(SWF_U8))
 *
 * Revision 1.21  2001/07/09 15:48:54  acme
 * Renamed U32 to SWF_U32 and so on
 *
 * Revision 1.20  2001/07/09 12:38:57  muttley
 * A few bug fixes
 *
 * Revision 1.15  2001/06/30 12:33:18  kitty_goth
 * Move to a linked list representation of shaperecords - I was getting
 * SEGFAULT due to not large enough free chunk's. Seems much faster now.
 * --Kitty
 *
 * Revision 1.14  2001/06/30 09:55:35  kitty_goth
 * Synch with Simons mods
 *
 * Revision 1.13  2001/06/29 15:10:11  muttley
 * The printing of the actual text of a DefineText (and DefineText2 now)
 * is no longer such a big hack. Font information is kept in the swf_parser
 * context and the function that will take a text_record_list and print out
 * the text (textrecord_list_to_text) has been moved to swf_parse.c ...
 *
 * A couple of potential bugs have also been fixed and some more 'todo's added
 *
 * Revision 1.12  2001/06/27 12:42:15  kitty_goth
 * Debug shaperecord handling --Kitty
 *
 * Revision 1.11  2001/06/26 17:40:30  kitty_goth
 * Bug fix for swf_parse_get_bytes to fix sound stream stuff. --Kitty
 *
 * Revision 1.10  2001/06/26 13:43:02  muttley
 * Fix text_record and text_record_list parsing
 *
 */



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
