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
 * $Log: swf_parse.c,v $
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

#include "swf_parse.h"
#include "swf_destroy.h"
//#define DEBUG

/* MPEG tables */
const int swf_MPEG_VerTab[4]={2,3,1,0};
const int swf_MPEG_FreqTab[4]={44100,48000,32000};
const int swf_MPEG_RateTab[2][3][16]=
{
  {
    {  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,  0},
    {  0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,  0},
   {  0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,  0},
  },
  {
    {  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,  0},
    {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
    {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
  },
};
//todo : since these are exported do they need to be declared final?
const char * swf_MPEG_Ver[4] = {"1","2","2.5","3?"};



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

      /* todo: fix this magic number */
      if ((context->font_chars = (char **) calloc (256, sizeof (char *))) == NULL)
      {
	    *error = SWF_EMallocFailure;
	    return NULL;
      }
      context->number_of_fonts = 0;


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
 * Setup a rectangle (ie a structure of type swf_rect) and
 * set its dimensions by reading the correct number
 * of bits from the stream.
 */

swf_rect *
swf_parse_get_rect (swf_parser * context, int * error)
{
    swf_rect * rect;
    int nbits;

    if ((rect = (swf_rect *) calloc (1, sizeof (swf_rect))) == NULL) {
                *error = SWF_EMallocFailure;
                return NULL;
    }

    swf_parse_initbits(context);
    nbits = (int) swf_parse_get_bits(context, 5);
    rect->xmin = swf_parse_get_sbits(context, nbits);
    rect->xmax = swf_parse_get_sbits(context, nbits);
    rect->ymin = swf_parse_get_sbits(context, nbits);
    rect->ymax = swf_parse_get_sbits(context, nbits);

    return rect;
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

/*
 * Read the id and name of the font, and read in the glyphs
 * corresponding to this font.
 */

swf_definefontinfo *
swf_parse_definefontinfo (swf_parser * context, int * error)
{
    int n, glyph_count;

    swf_definefontinfo * info;

    if ((info = (swf_definefontinfo *) calloc (1, sizeof (swf_definefontinfo))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    info->fontid = (SWF_U32) swf_parse_get_word (context);
    info->namelen = swf_parse_get_byte (context);

    /* Allocate enough space to hold the name of the font */
    if ( ( info->fontname = (char *) calloc ((1 + info->namelen), sizeof(char))) == NULL ){
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    /* Read the fontname from the stream*/
    for(n=0; n < info->namelen; n++) {
        info->fontname [n] = (char) swf_parse_get_byte (context);
    }

    info->fontname [n] = '\0';

    info->flags = swf_parse_get_byte (context);

    /* The glyph counts (ie, how many symbols are in each font) have
       presumably already been read into place by a DefineFont tag */
    glyph_count = context->glyph_counts[info->fontid];

    if ((info->code_table = (int *) calloc (glyph_count, sizeof(int) ) ) == NULL) {
        *error = SWF_EMallocFailure;
        goto FAIL;
    }

    /* Read in the glyphs present in this font, taking account of whether
       they are 8-bit or 16-bit values.
     */
    for(n=0; n < glyph_count; n++)
    {
        if (info->flags & FONT_WIDE_CODES) {
            info->code_table[n] = (int) swf_parse_get_word (context);
        } else {
	        info->code_table[n] = (int) swf_parse_get_byte (context);
        }

        context->font_chars [info->fontid][n] = (char) info->code_table[n];

    }



    return info;

 FAIL:
    swf_destroy_definefontinfo (info);
    return NULL;
}

/*
 * This is very similar to swf_parse_placeobject, except that this more
 * complicated type includes ratio, clipdepth, a name and some
 * additional flags.
 */

swf_placeobject2 *
swf_parse_placeobject2 (swf_parser * context, int * error)
{
    swf_placeobject2 * place;

    if ((place = (swf_placeobject2 *) calloc (1, sizeof (swf_placeobject2))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    place->flags = swf_parse_get_byte (context);
    place->depth = swf_parse_get_word (context);
    place->matrix = NULL;
    place->cxform = NULL;


    /* Get the tag if specified. */
    if (place->flags & splaceCharacter) {
        place->tag = swf_parse_get_word (context);
    }

    /* Get the matrix if specified. */
    if (place->flags & splaceMatrix) {
        if ((place->matrix  = (swf_matrix *) swf_parse_get_matrix (context, error)) == NULL) {
            goto FAIL;
	}
    }

    /* Get the color transform if specified. */
    if (place->flags & splaceColorTransform) {
        if ((place->cxform = (swf_cxform *) swf_parse_get_cxform(context, error, TRUE)) == NULL) {
            goto FAIL;
    	}
    }

    /* Get the ratio if specified. */
    if (place->flags & splaceRatio) {
        place->ratio = swf_parse_get_word (context);
    }

    /* Get the clipdepth if specified. */
    if (place->flags & splaceDefineClip) {
        place->clip_depth = swf_parse_get_word (context);
    }

    /* Get the instance name */

    if (place->flags & splaceName){
        if ((place->name = (char *) swf_parse_get_string(context, error)) == NULL) {
            goto FAIL;
	}
    }


    return place;

 FAIL:
    swf_destroy_placeobject2 (place);
    return NULL;
}

/*
 * Flash has an idea of a 'matrix' which is not strictly a matrix
 * in the mathematical sense, but rather a general 2D transformation
 * with translation terms.
 *
 * This function grabs such a transformation from the input stream
 * and stores it.
 */

swf_matrix *
swf_parse_get_matrix (swf_parser * context, int * error)
{
    swf_matrix * matrix;
    int n_bits;

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    swf_parse_initbits (context);

    /* Scale terms */
    if (swf_parse_get_bits (context, 1)) {
	n_bits = (int) swf_parse_get_bits (context, 5);
	matrix->a = swf_parse_get_sbits(context, n_bits);
	matrix->d = swf_parse_get_sbits(context, n_bits);
    } else {
	matrix->a = matrix->d = 0x00010000L;
    }

    /* Rotate/skew terms */
    if (swf_parse_get_bits (context, 1)) {
	n_bits = swf_parse_get_bits (context, 5);
	matrix->b = swf_parse_get_sbits(context, n_bits);
	matrix->c = swf_parse_get_sbits(context, n_bits);
    } else {
	matrix->b = matrix->c = 0;
    }

    /* Translate terms */
    n_bits = (int) swf_parse_get_bits (context, 5);
    matrix->tx = swf_parse_get_sbits(context, n_bits);
    matrix->ty = swf_parse_get_sbits(context, n_bits);

    return matrix;
}


/*
 *
 */

swf_cxform *
swf_parse_get_cxform (swf_parser * context, int * error, int has_alpha)
{
    swf_cxform * cxform;

    int need_add, need_mul, n_bits;

    if ((cxform = (swf_cxform *) calloc (1, sizeof (swf_cxform))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    swf_parse_initbits (context);

    /* !!! The spec has these bits reversed !!! */
    need_add = swf_parse_get_bits (context, 1);
    need_mul = swf_parse_get_bits (context, 1);
    /* !!! The spec has these bits reversed !!! */


    n_bits = (int) swf_parse_get_bits(context, 4);

    cxform->aa = 256;
    cxform->ab = 0;

    if (need_mul) {
	cxform->ra = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	cxform->ga = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	cxform->ba = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	if (has_alpha) {
	    cxform->aa = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	}
    } else {
	cxform->ra = cxform->ga = cxform->ba = 256;
    }

    if (need_add) {
	cxform->rb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	cxform->gb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	cxform->bb = (SWF_S16) swf_parse_get_sbits (context, n_bits);
	if (has_alpha) {
	    cxform->ab = (SWF_S16)swf_parse_get_sbits (context, n_bits);
	}
    } else {
	cxform->rb = cxform->gb = cxform->bb = 0;
    }

    return cxform;
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

swf_definefont *
swf_parse_definefont (swf_parser * context, int * error)
{
    swf_definefont * font;
    int n, start, xlast, ylast;
    int * offset_table;
    SWF_U16 fillbits, linebits;

    if ((font = (swf_definefont *) calloc (1, sizeof (swf_definefont))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    font->shape_records = NULL;
    font->fontid = (SWF_U32) swf_parse_get_word(context);

    start = swf_parse_tell(context);
    font->offset = swf_parse_get_word (context);

    font->glyph_count = font->offset/2;


    context->glyph_counts[font->fontid] = font->glyph_count;

    context->number_of_fonts++;
    /* todo :
     * maybe the number of fonts and the font id will get out of sync.
     * Should we have a lookup up table?
     * Also, what happens if we define more than 255 fonts? Should
     * put checks in for that
     */

    if ((context->font_chars [font->fontid] = (char *) calloc (font->glyph_count, sizeof (char))) == NULL)
    {
        *error = SWF_EMallocFailure;
        goto FAIL;

    }


    if  ((offset_table =  (int *) calloc (font->glyph_count, sizeof (int))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    offset_table[0] = font->offset;

    for(n=1; n<font->glyph_count; n++) {
	offset_table[n] = swf_parse_get_word(context);
    }

    if  ((font->shape_records =  (swf_shaperecord_list **) calloc (font->glyph_count, sizeof (swf_shaperecord_list *))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    for(n=0; n<font->glyph_count; n++) {
        font->shape_records[n] = NULL;

        swf_parse_seek(context, offset_table[n] + start);

        swf_parse_initbits(context); /* reset bit counter */

        fillbits = (SWF_U16) swf_parse_get_bits(context, 4);
        linebits = (SWF_U16) swf_parse_get_bits(context, 4);

        xlast = 0;
        ylast = 0;

        font->shape_records[n] = NULL;
/* TODO swf_parse_get_shaperecords(context, error);*/
    }

    swf_free (offset_table);

    return font;

 FAIL:
    swf_destroy_definefont (font);
    return NULL;
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

swf_setbackgroundcolour *
swf_parse_setbackgroundcolour (swf_parser * context, int * error)
{
    swf_setbackgroundcolour * back;

    if ((back = (swf_setbackgroundcolour *) calloc (1, sizeof (swf_setbackgroundcolour))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    if ((back->colour = (swf_colour *) calloc (1, sizeof (swf_colour))) == NULL) {
	*error = SWF_EMallocFailure;
	    goto FAIL;
    }

    back->colour->r = swf_parse_get_byte(context);
    back->colour->g = swf_parse_get_byte(context);
    back->colour->b = swf_parse_get_byte(context);

    return back;

 FAIL:
    swf_destroy_setbackgroundcolour (back);
    return NULL;
}


/*
 * Thin wrapper over swf_parse_defineshape_aux.
 * The spec has defineshape, defineshape2, etc but rather than
 * aping that, I think it's better to have these thin wrappers. --BE
 */

swf_defineshape *
swf_parse_defineshape (swf_parser * context, int * error)
{
    return (swf_defineshape *) swf_parse_defineshape_aux (context, error, FALSE);
}

/*
 * Get the shapestyle from the input stream. We need the linestyles and the
 * fillstyles from the stream.
 */

swf_shapestyle *
swf_parse_get_shapestyle (swf_parser * context, int * error, int with_alpha)
{
    SWF_U16 i = 0;
    SWF_U16 j = 0;
    swf_shapestyle * styles;

    if ((styles = (swf_shapestyle *) calloc (1, sizeof (swf_shapestyle))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    styles->nfills = 0;
    styles->nlines = 0;
    styles->fills  = NULL;
    styles->lines  = NULL;

    /* Get the number of fills. */
    styles->nfills = swf_parse_get_byte (context);


    /* Do we have a larger number? */
    if (styles->nfills == 255) {
        /* Get the larger number. */
        styles->nfills = swf_parse_get_word(context);
    }

    if ((styles->fills = (swf_fillstyle **) calloc (styles->nfills, sizeof (swf_fillstyle *))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    /* Get each of the fill style. */
    for (i = 0; i <styles->nfills; i++) {
        if ((styles->fills[i] = (swf_fillstyle *) calloc (1, sizeof (swf_fillstyle))) == NULL) {
	    *error = SWF_EMallocFailure;
	    goto FAIL;
        }

        styles->fills[i]->ncolours = 0;
        styles->fills[i]->colours  = NULL;
        styles->fills[i]->matrix   = NULL;

        styles->fills[i]->fill_style = swf_parse_get_byte (context);

        if (styles->fills[i]->fill_style & fillGradient) {
            /* Get the gradient matrix. */
            styles->fills[i]->matrix = (swf_matrix *) swf_parse_get_matrix (context, error);

            /* Get the number of colors. */
            styles->fills[i]->ncolours = (SWF_U16) swf_parse_get_byte(context);

            if ((styles->fills[i]->colours = (swf_rgba_pos **) calloc (styles->fills[i]->ncolours, sizeof (swf_rgba_pos *))) == NULL) {
                *error = SWF_EMallocFailure;
                goto FAIL;
            }

            /* Get each of the colours. */
            for (j = 0; j< styles->fills[i]->ncolours; j++) {
                if ((styles->fills[i]->colours[j] = (swf_rgba_pos *) calloc (1, sizeof (swf_rgba_pos))) == NULL) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }

                styles->fills[i]->colours[j]->pos  = swf_parse_get_byte   (context);
                styles->fills[i]->colours[j]->rgba = swf_parse_get_colour (context, error,  with_alpha);

            }
        } else if (styles->fills[i]->fill_style & fillBits) {
            /* Get the bitmap matrix. */
            styles->fills[i]->bitmap_id = swf_parse_get_word (context);
            styles->fills[i]->matrix = swf_parse_get_matrix (context, error);
        } else {
            /* A solid color */
            styles->fills[i]->colour = swf_parse_get_colour (context, error,  with_alpha);
        }
    }

    /* Get the number of lines. */
    styles->nlines = swf_parse_get_byte (context);

    /* Do we have a larger number? */
    if (styles->nlines == 255) {
        /* Get the larger number. */
        styles->nlines = swf_parse_get_word (context);
    }

    if ((styles->lines = (swf_linestyle **) calloc (styles->nlines, sizeof (swf_linestyle *))) == NULL) {
	*error = SWF_EMallocFailure;
	goto FAIL;
    }

    /* Get each of the line styles. */
    for (i = 0; i < styles->nlines; i++) {
        if ((styles->lines[i] = (swf_linestyle *) calloc (1, sizeof (swf_linestyle))) == NULL) {
	    *error = SWF_EMallocFailure;
	    goto FAIL;
        }

        styles->lines[i]->width  = swf_parse_get_word   (context);
        styles->lines[i]->colour = swf_parse_get_colour (context, error,  with_alpha);
    }

    return styles;

 FAIL:
    swf_destroy_shapestyle (styles);
    return NULL;
}

swf_defineshape *
swf_parse_defineshape_aux (swf_parser * context, int * error, int with_alpha)
{

    swf_defineshape * shape;

    if ((shape = (swf_defineshape *) calloc (1, sizeof (swf_defineshape))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    shape->tagid  = (SWF_U32) swf_parse_get_word (context);
    shape->rect   = NULL;
    shape->style  = NULL;
    shape->record = NULL;

    return shape;

    /* Get the bounding rectangle */

    if ((shape->rect =  swf_parse_get_rect (context, error)) == NULL) {
        goto FAIL;
    }

    if ((shape->style = swf_parse_get_shapestyle(context, error, with_alpha)) == NULL) {
        goto FAIL;
    }

/*
 * Bug!  this was not in the original swf_parse.cpp
 * Required to reset bit counters and read byte aligned.
 */

    swf_parse_initbits (context);

    context->fill_bits = (SWF_U16) swf_parse_get_bits (context, 4);
    context->line_bits = (SWF_U16) swf_parse_get_bits (context, 4);

/* TODO simon */
    if ((shape->record = swf_parse_get_shaperecords (context,error)) == NULL) {
        goto FAIL;
    }

    return shape;

 FAIL:
    swf_destroy_defineshape (shape);
    return NULL;
}




swf_placeobject *
swf_parse_placeobject (swf_parser * context, int * error)
{
    swf_placeobject * place;

    if ((place = (swf_placeobject *) calloc (1, sizeof (swf_placeobject))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    place->tagid = (SWF_U32) swf_parse_get_word(context);
    place->depth = (SWF_U32) swf_parse_get_word(context);

    place->cxform = NULL;
    place->matrix = NULL;

    if ((place->matrix = swf_parse_get_matrix (context, error)) == NULL) {
        goto FAIL;
    }

    if (swf_parse_tell(context) < context->tagend) {
	if ((place->cxform = swf_parse_get_cxform (context, error, 0)) == NULL) {
            goto FAIL;
	}
    }

    return place;

 FAIL:
    swf_destroy_placeobject (place);
    return NULL;
}


swf_freecharacter *
swf_parse_freecharacter (swf_parser * context, int * error)
{
    swf_freecharacter * character;

    if ((character = (swf_freecharacter *) calloc (1, sizeof (swf_freecharacter))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    character->tagid = swf_parse_get_word (context);

    return character;
}


swf_removeobject *
swf_parse_removeobject(swf_parser * context, int * error)
{
    swf_removeobject * object;

    if ((object = (swf_removeobject *) calloc (1, sizeof (swf_removeobject))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    object->tagid = (SWF_U32) swf_parse_get_word (context);
    object->depth = (SWF_U32) swf_parse_get_word (context);

    return object;
}


swf_removeobject2 *
swf_parse_removeobject2(swf_parser * context, int * error)
{

    swf_removeobject2 * object;

    if ((object = (swf_removeobject2 *) calloc (1, sizeof (swf_removeobject2))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    object->depth = (SWF_U32) swf_parse_get_word (context);

    return object;
}

swf_startsound *
swf_parse_startsound (swf_parser * context, int * error)
{
    int i = 0;
    swf_startsound * sound;

    if ((sound = (swf_startsound *) calloc (1, sizeof (swf_startsound))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    sound->tagid = swf_parse_get_word (context);

    if (sound->tagid) {
        sound->code =  swf_parse_get_byte (context);

        if ( sound->code & soundHasInPoint ) {
            sound->inpoint = swf_parse_get_dword (context);
        }

        if ( sound->code & soundHasOutPoint ) {
            sound->outpoint = swf_parse_get_dword (context);
        }

        if ( sound->code & soundHasLoops ) {
            sound->loops = swf_parse_get_dword (context);
        }

        sound->npoints = 0;

        if (  sound->code & soundHasEnvelope ) {
            sound->npoints = swf_parse_get_byte (context);

			if (sound->npoints) {
				if ((sound->points = (swf_soundpoint **) calloc (sound->npoints, sizeof (swf_soundpoint *))) == NULL) {
					*error = SWF_EMallocFailure;
					goto FAIL;
				}
			}

            for (i = 0; i < sound->npoints; i++ ) {
		sound->points[i] = NULL;
		if ((sound->points[i] = (swf_soundpoint *) calloc (1, sizeof (swf_soundpoint))) == NULL) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;
                }

                sound->points[i]->mark = swf_parse_get_dword (context);
                sound->points[i]->lc   = swf_parse_get_dword (context);
                sound->points[i]->rc   = swf_parse_get_dword (context);
            }
        }
    }

    return sound;

 FAIL:
    swf_destroy_startsound (sound);
    return NULL;
}

swf_definebits *
swf_parse_definebits (swf_parser * context, int * error)
{
    swf_definebits * bits;

    if ((bits = (swf_definebits *) calloc (1, sizeof (swf_definebits))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);

    if ((bits->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        swf_destroy_definebits (bits);
        return NULL;
    }

    return bits;
}


swf_jpegtables *
swf_parse_jpegtables (swf_parser * context, int * error)
{
    swf_jpegtables * tables;

    if ((tables = (swf_jpegtables *) calloc (1, sizeof (swf_jpegtables))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    if ((tables->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        swf_destroy_jpegtables (tables);
        return NULL;
    }

    return tables;
}

swf_definebitsjpeg2 *
swf_parse_definebitsjpeg2 (swf_parser * context, int * error)
{
    swf_definebitsjpeg2 * bits;

    if ((bits = (swf_definebitsjpeg2 *) calloc (1, sizeof (swf_definebitsjpeg2))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);
    if ((bits->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        goto FAIL;

    }


    return bits;

 FAIL:
    swf_destroy_definebitsjpeg2 (bits);
    return NULL;
}

swf_definebitsjpeg3 *
swf_parse_definebitsjpeg3 (swf_parser * context, int * error)
{
    swf_definebitsjpeg3 * bits;

    if ((bits = (swf_definebitsjpeg3 *) calloc (1, sizeof (swf_definebitsjpeg3))) == NULL) {
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);
    if ((bits->guts  = swf_parse_get_imageguts (context, error)) == NULL) {
        swf_destroy_definebitsjpeg3 (bits);
        return NULL;
    }



    return bits;

}

swf_imageguts *
swf_parse_get_imageguts (swf_parser * context, int * error)
{
    int count=0;

    int size = 0;
    #define SWF_IMAGE_GUTS_BLOCK_SIZE (256)

    swf_imageguts * guts = NULL;

    //return NULL;

    if ((guts = (swf_imageguts *) calloc (1, sizeof (swf_imageguts))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }


    while (swf_parse_tell (context) < context->next_tag_pos) {
        if ((count % SWF_IMAGE_GUTS_BLOCK_SIZE) == 0) {
            size+= SWF_IMAGE_GUTS_BLOCK_SIZE;
	    if (guts->data != NULL) {
		    guts->data = (SWF_U8 *) realloc (guts->data, sizeof(SWF_U8) * size);
	    }
	    else {
		    guts->data = (SWF_U8 *) calloc (size, sizeof(SWF_U8));
	    }
	    if (guts->data == NULL) {
                *error = SWF_EReallocFailure;
                goto FAIL;
            }
        }

        guts->data[count++] = swf_parse_get_byte (context);
    }

    guts->nbytes = count;

    while ((count % SWF_IMAGE_GUTS_BLOCK_SIZE) != 0) {
        guts->data [count++] = (SWF_U8) NULL;
    }

    return guts;

 FAIL:
    swf_destroy_imageguts (guts);
    return NULL;

}

swf_definetext *
swf_parse_definetext (swf_parser * context, int * error)
{

    swf_definetext * text;
    int n_glyph_bits, n_advance_bits;


    if ((text = (swf_definetext *) calloc (1, sizeof (swf_definetext))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    text->tagid  = swf_parse_get_word   (context);
    text->rect   = swf_parse_get_rect   (context, error);
    text->matrix = swf_parse_get_matrix (context, error);


    n_glyph_bits   = (int)swf_parse_get_byte (context);
    n_advance_bits = (int)swf_parse_get_byte (context);



    if ((text->records  =  swf_parse_get_textrecords (context, error, FALSE, n_glyph_bits, n_advance_bits)) == NULL) {
            goto FAIL;
    }



    return text;

    FAIL:
    swf_destroy_definetext (text);
    return NULL;



}

/*
 * Yes, I know this is exactly the same as definetext
 * but I want them as two seperate functions 'cos I'm sure
 * that the specs must be lying about this. Or maybe not.
 * Whatever.
 */
swf_definetext2 *
swf_parse_definetext2 (swf_parser * context, int * error)
{

    swf_definetext2 * text;
    int n_glyph_bits, n_advance_bits;


    if ((text = (swf_definetext2 *) calloc (1, sizeof (swf_definetext2))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }


    text->tagid   = swf_parse_get_word   (context);
    text->rect    = swf_parse_get_rect   (context, error);
    text->matrix  = NULL;
    text->records = NULL;
    if ((text->matrix  = swf_parse_get_matrix (context, error)) == NULL) {
        goto FAIL;
    }


    n_glyph_bits   = (int)swf_parse_get_byte (context);
    n_advance_bits = (int)swf_parse_get_byte (context);


    if ((text->records  =  swf_parse_get_textrecords (context, error, TRUE, n_glyph_bits, n_advance_bits)) == NULL) {
        goto FAIL;
    }

    return text;

    FAIL:
    swf_destroy_definetext2 (text);
    return NULL;


}





swf_definebutton *
swf_parse_definebutton (swf_parser * context, int * error)
{
    swf_definebutton * button;

    if ((button = (swf_definebutton *) calloc (1, sizeof (swf_definebutton))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid   = swf_parse_get_word (context);

    if ((button->records = swf_parse_get_buttonrecords (context, error, FALSE)) == NULL)
    {
        goto FAIL;
    }

    if ((button->actions = swf_parse_get_doactions (context, error)) == NULL)
    {
        goto FAIL;
    }

    return button;

    FAIL:
    swf_destroy_definebutton (button);
    return NULL;

}

swf_definebutton2 *
swf_parse_definebutton2 (swf_parser *  context, int * error)
{

    swf_definebutton2 * button;
    SWF_U32 track_as_menu, offset, next_action;

    if ((button = (swf_definebutton2 *) calloc (1, sizeof (swf_definebutton2))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid = swf_parse_get_word (context);

    track_as_menu = swf_parse_get_byte(context);

    /* Get offset to first "Button2ActionCondition"
     * This offset is not in the spec!
     */
    offset = swf_parse_get_word(context);
    next_action = swf_parse_tell(context) + offset - 2;

    /* Parse Button Records */
    button->records = swf_parse_get_buttonrecords (context, error, TRUE);

    /*
     * Parse Button2ActionConditions
     */

    swf_parse_seek(context, next_action);

    #ifdef DEBUG
    fprintf (stderr, "[parse_definebutton2 : getting button2actions]\n");
    #endif

    if ((button->actions = swf_parse_get_button2actions (context, error)) == NULL)
    {
	    goto FAIL;
	}
    #ifdef DEBUG
    fprintf (stderr, "[parse_definebutton2 : button2actions are not NULL]\n");
    #endif
    return button;


 FAIL:
 swf_destroy_definebutton2 (button);
 return NULL;
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

         #ifdef DEBUG
         fprintf (stderr, "[get_button2action_list : action_offset  = %lx %lx (%d)]\n", action_offset, next_action, test);
         #endif

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


swf_doaction *
swf_parse_get_doaction (swf_parser * context, int * error)
{
        swf_doaction * action;
        int len = 0;
        SWF_S32 pos;

        if ((action = (swf_doaction *) calloc (1, sizeof (swf_doaction))) == NULL)
        {
            *error = SWF_EMallocFailure;
            return NULL;
        }

        action->next = NULL;
        action->url  = NULL;
        action->target = NULL;
        action->goto_label = NULL;
        action->push_data_string = NULL;


        /* Handle the action */
        action->code = swf_parse_get_byte(context);
        if (action->code == 0)
        {

            return action;
        }


        if (action->code & sactionHasLength)
        {
            len = swf_parse_get_word (context);
        }

        pos = swf_parse_tell (context) + len;

        switch ( action->code )
        {


            case sactionGotoFrame:
            {
                action->frame = swf_parse_get_word(context);
                break;
            }

            case sactionGetURL:
            {
                action->url    = swf_parse_get_string(context, error);
                action->target = swf_parse_get_string(context, error);
                break;
            }

            case sactionWaitForFrame:
            {
                action->frame      = swf_parse_get_word(context);
                action->skip_count = swf_parse_get_byte(context);
                break;
            }

            case sactionSetTarget:
            {
                action->target = swf_parse_get_string (context, error);
                break;
            }

            case sactionGotoLabel:
            {

                action->goto_label = swf_parse_get_string (context, error);
                break;
            }

            case sactionWaitForFrameExpression:
            {
                action->skip_count = swf_parse_get_byte(context);
                break;
            }

            case sactionPushData:
            {
                action->push_data_type = swf_parse_get_byte (context);

                /* property ids are pushed as floats for some reason */
                if ( action->push_data_type == 1 )
                {

                    action->push_data_float.dw = swf_parse_get_dword (context);
                }
                else
                if ( action->push_data_type == 0 )
                {
                    action->push_data_string = swf_parse_get_string (context, error);
                }
                break;
            }

            case sactionBranchAlways:
            {
                action->branch_offset = swf_parse_get_word(context);
                break;
            }

            case sactionGetURL2:
            {
                action->url2_flag = swf_parse_get_byte(context);
                break;
            }

            case sactionBranchIfTrue:
            {
                action->branch_offset = swf_parse_get_word(context);
                break;
            }


            case sactionGotoExpression:
            {
                action->stop_flag = swf_parse_get_byte(context);
            }
        }

        swf_parse_seek(context, pos);

        return action;
}



swf_defineedittext *
swf_parse_defineedittext (swf_parser * context, int * error)
{

    swf_defineedittext * text;

    if ((text = (swf_defineedittext *) calloc (1, sizeof(swf_defineedittext))) == NULL)
    {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    text->tagid  = swf_parse_get_word (context);
    text->bounds = swf_parse_get_rect(context, error);
    text->flags  = swf_parse_get_word (context);

    if (text->flags & seditTextFlagsHasFont) {
        text->font_id = swf_parse_get_word (context);
        text->font_height = swf_parse_get_word (context);
    }

    if (text->flags & seditTextFlagsHasTextColor) {
        text->colour = swf_parse_get_colour (context, error,  TRUE);
    }

    if (text->flags & seditTextFlagsHasMaxLength) {
        text->max_length = swf_parse_get_word (context);
    }


    if (text->flags & seditTextFlagsHasLayout) {
        text->align        = swf_parse_get_byte (context);
       	text->left_margin  = swf_parse_get_word (context);
        text->right_margin = swf_parse_get_word (context);
        text->indent       = swf_parse_get_word (context);
        text->leading      = swf_parse_get_word (context);
    }

    if ((text->variable = swf_parse_get_string (context, error)) == NULL) {
        goto FAIL;
    }

    if (text->flags & seditTextFlagsHasText ) {
        if ((text->initial_text = swf_parse_get_string (context, error)) == NULL) {
	    goto FAIL;
        }
    }


    return text;

 FAIL:
    swf_destroy_defineedittext (text);
    return NULL;
}

swf_definefont2 *
swf_parse_definefont2 (swf_parser * context, int * error)
{
    swf_definefont2 * font;
    int i, n, data_pos;
    SWF_U32 code_offset;
    SWF_U32 * offset_table;

    if ((font = (swf_definefont2 *) calloc (1, sizeof (swf_definefont2))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    font->name = NULL;
    font->code_table = NULL;
    font->glyphs = NULL;
    font->kerning_pairs = NULL;
    font->bounds = NULL;

    font->fontid = swf_parse_get_word (context);





    font->flags = swf_parse_get_word (context);
    font->name_len = swf_parse_get_byte (context);

    if ((font->name = (char *) calloc (font->name_len, sizeof(char))) == NULL) {
  	    *error = SWF_EMallocFailure;
    	goto FAIL;
    }

    for (i=0; i<font->name_len; i++) {
        font->name[i] = (char) swf_parse_get_byte(context);
    }

    /* Get the number of glyphs. */
    font->glyph_count = swf_parse_get_word(context);


    context->glyph_counts[font->fontid] = font->glyph_count;

    context->number_of_fonts++;
    /* todo :
     * maybe the number of fonts and the font id will get out of sync.
     * Should we have a lookup up table?
     * Also, what happens if we define more than 255 fonts? Should
     * put checks in for that
     */

    if (font->glyph_count > 0)
    {

	    if ((context->font_chars [font->fontid] = (char *) calloc (font->glyph_count, sizeof (char))) == NULL)
	    {
		    *error = SWF_EMallocFailure;
		    goto FAIL;
		    
	    }


    data_pos = swf_parse_tell(context);



        /* Get the FontOffsetTable */


        if ((offset_table = (SWF_U32 *) calloc (font->glyph_count, sizeof (SWF_U32))) == NULL)
        {
	        *error = SWF_EMallocFailure;
            goto FAIL;
        }

        for (n=0; n<font->glyph_count; n++) {
            if (font->flags & sfontFlagsWideOffsets) {
                offset_table[n] = swf_parse_get_dword(context);
            } else {
                offset_table[n] = swf_parse_get_word(context);
            }
    	}

        /* Get the CodeOffset */


        code_offset = 0;
        if (font->flags & sfontFlagsWideOffsets) {
            code_offset = swf_parse_get_dword(context);
        } else {
            code_offset = swf_parse_get_word(context);
	}

        /* Get the Glyphs */
	if ((font->glyphs = (swf_shaperecord_list **) calloc (font->glyph_count, sizeof(swf_shaperecord_list *))) == NULL) {
	    goto FAIL;
	}


        for(n=0; n<font->glyph_count; n++) {

    	    swf_parse_seek (context, data_pos + offset_table[n]);
    	    swf_parse_initbits (context); /* reset bit counter */

    	    /* todo simon : do these really need to be set in the context? */
            context->fill_bits = (SWF_U16) swf_parse_get_bits(context, 4);
            context->line_bits = (SWF_U16) swf_parse_get_bits(context, 4);

    	    font->glyphs[n] = (swf_shaperecord_list *) swf_parse_get_shaperecords(context, error);

        }

    	swf_free (offset_table);


        if (swf_parse_tell (context) != data_pos + code_offset)
        {
	        /* todo simon : should I return NULL here? */
    	    swf_parse_seek(context, data_pos + code_offset);
        }

    	if ((font->code_table = (SWF_U32 *) calloc (font->glyph_count, sizeof (SWF_U32))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
	    }

    	/* Get the CodeTable */
        for (i=0; i<font->glyph_count; i++) {

            if (font->flags & sfontFlagsWideOffsets) {
                font->code_table [i] = swf_parse_get_word (context);
            } else {
                font->code_table [i] = swf_parse_get_byte (context);
	    }
        }
    }

    if (font->flags & sfontFlagsHasLayout) {

        /* Get "layout" fields */

        font->ascent  = swf_parse_get_word (context);
        font->descent = swf_parse_get_word (context);
        font->leading = swf_parse_get_word (context);

        /* Skip Advance table */
	/* todo simon : does this need to be done ???*/
        swf_parse_skip (context, font->glyph_count * 2);



        /* Get BoundsTable */
	if ((font->bounds = (swf_rect **) calloc (font->glyph_count, sizeof(swf_rect*))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
	    }

        for (i=0; i<font->glyph_count; i++)
        {
           if  ((font->bounds[i] = swf_parse_get_rect (context, error)) == NULL) {
                goto FAIL;
            }
        }

        /*
         * Get Kerning Pairs
         */

    	font->nkerning_pairs = swf_parse_get_word (context);

        if (font->nkerning_pairs && (font->kerning_pairs = (swf_kerningpair **) calloc (font->nkerning_pairs, sizeof(swf_kerningpair *))) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }


        for (i=0; i<font->nkerning_pairs; i++)
        {
	        if ((font->kerning_pairs[i] = (swf_kerningpair *) calloc (1, sizeof(swf_kerningpair))) == NULL) {
                *error = SWF_EMallocFailure;
	    	    goto FAIL;
	        }

            if (font->flags & sfontFlagsWideOffsets)
            {
                font->kerning_pairs[i]->code1 = swf_parse_get_word (context);
                font->kerning_pairs[i]->code2 = swf_parse_get_word (context);
            }
            else
            {
                font->kerning_pairs[i]->code1 = swf_parse_get_byte (context);
                font->kerning_pairs[i]->code2 = swf_parse_get_byte (context);
            }
            font->kerning_pairs[i]->adjust = swf_parse_get_word(context);

        }


    }


    return font;

    FAIL:
    swf_destroy_definefont2 (font);
    return NULL;

}



swf_definemorphshape *
swf_parse_definemorphshape (swf_parser * context, int * error)
{
    swf_definemorphshape * shape;
    SWF_U32 offset, end_shape_pos;
    int with_alpha;
    int i, j;

    if ((shape = (swf_definemorphshape *) calloc (1, sizeof (swf_definemorphshape))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }

    shape->nfills = 0;
    shape->fills  = NULL;
    shape->nlines = 0;
    shape->lines  = NULL;

    shape->tagid = swf_parse_get_word (context);

    if ((shape->r1 = swf_parse_get_rect (context, error)) == NULL) { goto FAIL; }
    if ((shape->r2 = swf_parse_get_rect (context, error)) == NULL) { goto FAIL; }

    /* Calculate the position of the end shape edges */
    offset = swf_parse_get_dword(context);
    end_shape_pos = swf_parse_tell(context) + offset;

    /* Always get RGBA not RGB for DefineMorphShape */
    with_alpha = TRUE;



    /* Get the fills */
    shape->nfills = swf_parse_get_byte (context);
    if (shape->nfills >= 255 ) {
        shape->nfills = swf_parse_get_word(context);
    }

	if (shape->nfills) {
		if ((shape->fills = (swf_fillstyle2 **) calloc (shape->nfills, sizeof (swf_linestyle2 *))) == NULL)
		{
			*error = SWF_EMallocFailure;
			goto FAIL;
		}
	}

    for (i = 0; i < shape->nfills; i++ )
    {
        if ((shape->fills[i] = (swf_fillstyle2 *) calloc (1, sizeof (swf_fillstyle2))) == NULL) {

    	    *error = SWF_EMallocFailure;
	        goto FAIL;
	    }

        shape->fills[i]->style = swf_parse_get_byte(context);
    	shape->fills[i]->colours = NULL;

        if (shape->fills[i]->style & fillGradient)
        {
            /* Gradient fill */
    	    shape->fills[i]->matrix1 = swf_parse_get_matrix (context, error);
    	    shape->fills[i]->matrix2 = swf_parse_get_matrix (context, error);

            /* Get the gradient color points */
            shape->fills[i]->ncolours = swf_parse_get_byte (context);

       	    if ((shape->fills[i]->colours = (swf_gradcolour **) calloc (shape->fills[i]->ncolours, sizeof (swf_gradcolour *))) == NULL) {
	    	    *error = SWF_EMallocFailure;
		        goto FAIL;
	        }

            for (j = 0; j < shape->fills[i]->ncolours; j++)
            {

           	    if ((shape->fills[i]->colours[j] = (swf_gradcolour *) calloc (1, sizeof (swf_gradcolour ))) == NULL) {
	                *error = SWF_EMallocFailure;
                    goto FAIL;
                 }

	    	    shape->fills[i]->colours[j]->r1 = swf_parse_get_byte (context);
		        shape->fills[i]->colours[j]->c1 = swf_parse_get_colour (context, error,  with_alpha);
		        shape->fills[i]->colours[j]->r2 = swf_parse_get_byte (context);
    	   	    shape->fills[i]->colours[j]->c2 = swf_parse_get_colour (context, error,  with_alpha);

            }
        }
        else if (shape->fills[i]->style & fillBits)
        {
            /* A bitmap fill */
            shape->fills[i]->tag = swf_parse_get_word (context); /* the bitmap tag */
    	    if ((shape->fills[i]->matrix1 = swf_parse_get_matrix (context, error)) == NULL) { goto FAIL; }
	        if ((shape->fills[i]->matrix2 = swf_parse_get_matrix (context, error)) == NULL) { goto FAIL; }

        }
        else
        {
            /* A solid color */
            shape->fills[i]->rgb1 = swf_parse_get_colour (context, error,  with_alpha);
            shape->fills[i]->rgb2 = swf_parse_get_colour (context, error,  with_alpha);
        }
    }

    /* Get the lines */
    shape->nlines = swf_parse_get_byte (context);
    if ( shape->nlines >= 255 ) {
        shape->nlines = swf_parse_get_word (context);
    }


    if ((shape->lines = (swf_linestyle2 **) calloc (shape->nlines, sizeof (swf_linestyle2 *))) == NULL)
    {
   	    *error = SWF_EMallocFailure;
	    goto FAIL;
    }

    for (i = 0; i < shape->nlines; i++ )
    {

    	if ((shape->lines[i] = (swf_linestyle2 *) calloc (1, sizeof (swf_linestyle2 ))) == NULL)
    	{
   	        *error = SWF_EMallocFailure;
    	    goto FAIL;
    	}


        /* get the thickness */
        shape->lines[i]->thick1 = swf_parse_get_word (context);
        shape->lines[i]->thick2 = swf_parse_get_word (context);

        /* get the color */
        shape->lines[i]->rgb1 = swf_parse_get_colour (context, error,  with_alpha);
        shape->lines[i]->rgb2 = swf_parse_get_colour (context, error,  with_alpha);
    }


    /* Get the bits per style index for the start shape */
    /* todo simon : fills bits in context? Not convinced */
    swf_parse_initbits (context);
    context->fill_bits = swf_parse_get_bits(context, 4);
    context->line_bits = swf_parse_get_bits(context, 4);


    /* Parse the start shape */
    if ((shape->records1 = swf_parse_get_shaperecords (context, error)) == NULL) { goto FAIL; }

    if (swf_parse_tell(context) != end_shape_pos)
    {
    	/* todo simon : probably should handle this after I've written get_shaperecords */
    }

    /*
     * Get the bits per style index for the end shape
     * THIS IS POINTLESS -- THERE ARE NO STYLES ?!
     */

    /* todo simon : fills bits in context? Not convinced */
    swf_parse_initbits (context);
    context->fill_bits = swf_parse_get_bits(context, 4); /* not sure if we should save these to n_FillBits & nLineBits */
    context->line_bits = swf_parse_get_bits(context, 4); /* there are no styles so none of this make sense. */



    /*
     * Parse the end shape
     */

    if ((shape->records2 = swf_parse_get_shaperecords (context, error)) == NULL) { goto FAIL; }
    /* todo simon : check for null returns and panic */

    return shape;

    FAIL:
    swf_destroy_definemorphshape (shape);
    return NULL;
}






swf_definesound *
swf_parse_definesound (swf_parser * context, int * error)
{
    swf_definesound * sound;
    int n_samples_adpcm;


    if ((sound = (swf_definesound *) calloc (1, sizeof (swf_definesound))) == NULL) {
        *error = SWF_EMallocFailure;
    	return NULL;
    }


    sound->tagid = swf_parse_get_word (context);


    sound->compression    = swf_parse_get_bits (context, 4);      /* uncompressed, ADPCM or MP3 */
    sound->sample_rate    = swf_parse_get_bits (context, 2);
    sound->sample_size    = swf_parse_get_bits (context, 1);
    sound->stereo_mono    = swf_parse_get_bits (context, 1);
    sound->sample_count   = swf_parse_get_dword (context);
    sound->adpcm          = NULL;
    sound->mp3header_list = NULL;



    switch (sound->compression)
    {
        case 1:
        {
            n_samples_adpcm = 0;
            /*
            if ((sound->adpcm = swf_parse_adpcm_decompress(context, error, sound->sample_count, sound->stereo_mono, sound->sample_size, n_samples_adpcm)) == NULL)
            {
                goto FAIL;
            }
            */
            break;
        }
        case 2:
        {
            sound->delay = swf_parse_get_word(context);
            if ((sound->mp3header_list = swf_parse_get_mp3headers (context, error, sound->sample_count)) == NULL)
            {
                goto FAIL;
            }
            break;
        }
    }

    return sound;

    FAIL:
    swf_destroy_definesound (sound);
    return NULL;
}


swf_mp3header_list *
swf_parse_get_mp3headers (swf_parser * context, int * error, int samples_per_frame)
{

    int frame_count = 0;
    int header_store_size  = 0;
    swf_mp3header_list * header_list;
    SWF_U8 hdr[4];
    int i;


    if ((header_list = (swf_mp3header_list *) calloc (1, sizeof (swf_mp3header_list))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    if (samples_per_frame > 0)
    {
        while (TRUE)
        {
            if (header_store_size==frame_count)
            {
                header_store_size += 10;
                if (header_list->headers) {
			header_list->headers = (swf_mp3header **) realloc (header_list->headers, sizeof(swf_mp3header *) * header_store_size);
		}
		else {
			header_list->headers = (swf_mp3header **) calloc (header_store_size, sizeof(swf_mp3header *));
		}
                if (!header_list->headers) {
                    *error = SWF_EMallocFailure;
                    goto FAIL;

                }
            }

            header_list->headers[frame_count] = NULL;
            if ((header_list->headers[frame_count] = (swf_mp3header *) calloc (1, sizeof(swf_mp3header))) == NULL)
            {
                *error = SWF_EMallocFailure;
                goto FAIL;

            }


            /* Get the MP3 frame header */
            for (i=0; i<4; i++) {
                hdr[i] = swf_parse_get_byte (context);
            }

            /* Decode the MP3 frame header */
            header_list->headers[frame_count]->ver     = swf_MPEG_VerTab[((hdr[1] >> 3) & 3)];
            header_list->headers[frame_count]->layer   = 3 - ((hdr[1] >> 1) & 3);
            header_list->headers[frame_count]->pad     = (hdr[2] >>1 ) & 1;
            header_list->headers[frame_count]->stereo  = ((hdr[3] >> 6) & 3) != 3;
            header_list->headers[frame_count]->freq    = 0;
            header_list->headers[frame_count]->rate    = 0;

            if (hdr[0] != 0xFF || hdr[1] < 0xE0 || header_list->headers[frame_count]->ver==3 || header_list->headers[frame_count]->layer != 2)
            {
                *error = SWF_EInvalidMP3Header;
                goto FAIL;
            }
            else
            {
                header_list->headers[frame_count]->freq = swf_MPEG_FreqTab[(hdr[2] >>2 ) & 3] >> header_list->headers[frame_count]->ver;
                header_list->headers[frame_count]->rate = swf_MPEG_RateTab[header_list->headers[frame_count]->ver ? 1 : 0][header_list->headers[frame_count]->layer][(hdr[2] >> 4) & 15] * 1000;

                if (!header_list->headers[frame_count]->freq || !header_list->headers[frame_count]->rate)
                {
                    *error = SWF_EInvalidMP3Frame;
                    goto FAIL;
                }
            }

            /* Get the size of a decoded MP3 frame */
            header_list->headers[frame_count]->decoded_frame_size = (576 * (header_list->headers[frame_count]->stereo + 1));
            if (!header_list->headers[frame_count]->ver) {
                header_list->headers[frame_count]->decoded_frame_size  *= 2;
            }

            /* Get the size of this encoded MP3 frame */
            header_list->headers[frame_count]->encoded_frame_size  = ((header_list->headers[frame_count]->ver ? 72 : 144) * header_list->headers[frame_count]->rate) / header_list->headers[frame_count]->freq + header_list->headers[frame_count]->pad - 4;

            /* Decode the MP3 frame */
            header_list->headers[frame_count]->data = swf_parse_get_bytes(context, header_list->headers[frame_count]->encoded_frame_size);

            /* Move to the next frame */
            if (swf_parse_tell (context) >= context->tagend) {
                break;
            }
            frame_count++;

        }
    }

    header_list->header_count = ++frame_count;
    return header_list;

    FAIL:
    swf_destroy_mp3header_list (header_list);
    return NULL;

}

swf_adpcm *
swf_parse_adpcm_decompress (swf_parser * context, int * error, int count, int stereo_mono, int size, int nsamples)
{
    // todo Simon
    return NULL;
}

swf_framelabel *
swf_parse_framelabel (swf_parser * context, int * error)
{
    swf_framelabel * label;

    if ((label = (swf_framelabel *) calloc (1, sizeof(swf_framelabel))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    label->label = swf_parse_get_string (context, error);

    return label;

}

swf_namecharacter *
swf_parse_namecharacter (swf_parser * context, int * error)
{
    swf_namecharacter * name;

    if ((name = (swf_namecharacter *) calloc (1, sizeof(swf_namecharacter))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    name->tagid = swf_parse_get_word (context);
    name->label = swf_parse_get_string (context, error);

    return name;

}


swf_definebuttoncxform *
swf_parse_definebuttoncxform (swf_parser * context, int * error)
{


    int size=0;
    swf_definebuttoncxform * button;

    if ((button = (swf_definebuttoncxform *) calloc (1, sizeof (swf_definebuttoncxform))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid = swf_parse_get_word (context);


    while (swf_parse_tell (context) < context->tagend)
    {

        button->ncxforms = size++;
        if ((button->cxforms = (swf_cxform **) realloc (button->cxforms, sizeof (swf_cxform) * size)) == NULL)
        {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }

        if ((button->cxforms[size-1] = swf_parse_get_cxform (context, error, FALSE)) == NULL) {
            *error = SWF_EMallocFailure;
            goto FAIL;
        }

    }

    return button;

    FAIL:
    swf_destroy_definebuttoncxform (button);
    return NULL;

}

swf_soundstreamblock *
swf_parse_soundstreamblock (swf_parser * context, int * error)
{
    swf_soundstreamblock * block;

    if ((block = (swf_soundstreamblock *) calloc (1, sizeof (swf_soundstreamblock))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    block->mp3header_list = NULL;
    block->adpcm          = NULL;

    switch (context->stream_compression)
    {

        case 1:
        {
            block->n_stream_samples          = context->n_stream_samples;
            block->stream_sample_stereo_mono = context->stream_sample_stereo_mono;
            block->stream_sample_size        = context->stream_sample_size;
            block->n_samples_adpcm           = context->n_samples_adpcm = 0;
            if ((block->adpcm                     = swf_parse_adpcm_decompress(context, error, block->n_stream_samples, block->stream_sample_stereo_mono, block->stream_sample_size, block->n_samples_adpcm)) == NULL)
            {
                goto FAIL;
            }
            break;
        }
        case 2:
        {
            block->samples_per_frame  = swf_parse_get_word (context);
            block->delay              = swf_parse_get_word (context);
            if ((block->mp3header_list     = swf_parse_get_mp3headers (context, error, block->samples_per_frame)) == NULL)
            {
                goto FAIL;
            }
            break;
        }
        default:
        {

            // todo simon?
            goto FAIL;
        }


    }

    return block;

    FAIL:
    swf_destroy_soundstreamblock (block);
    return NULL;

}


swf_definebuttonsound *
swf_parse_definebuttonsound (swf_parser * context, int * error)
{
    swf_definebuttonsound * button;
    int i;
    swf_startsound * state;

    if ((button = (swf_definebuttonsound *) calloc (1, sizeof (swf_definebuttonsound))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    button->tagid      = swf_parse_get_word (context);
    button->up_state   = NULL;
    button->over_state = NULL;
    button->down_state = NULL;

    for (i=0; i<3; i++)
    {

        if ((state = swf_parse_startsound (context, error)) == NULL)
        {
            goto FAIL;
        }


        switch (i)
        {
            case 0:
                button->up_state = state;
                break;
            case 1:
                button->over_state = state;
                break;
            case 2:
                button->down_state = state;
                break;
        }




    }

    return button;

    FAIL:
    swf_destroy_definebuttonsound (button);
    return NULL;
}


swf_soundstreamhead *
swf_parse_soundstreamhead (swf_parser * context, int * error)
{
    swf_soundstreamhead * head;
    if ((head = (swf_soundstreamhead *) calloc (1, sizeof(swf_soundstreamhead))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    head->mix_format = swf_parse_get_byte (context);

    /* The stream settings */

    head->stream_compression         = context->stream_compression = swf_parse_get_bits (context, 4);
    head->stream_sample_rate         = context->stream_sample_rate = swf_parse_get_bits (context, 2);
    head->stream_sample_size         = context->stream_sample_size = swf_parse_get_bits (context, 1);
    head->stream_sample_stereo_mono  = context->stream_sample_stereo_mono = swf_parse_get_bits (context, 1);
    head->n_stream_samples           = context->n_stream_samples = swf_parse_get_word (context);

    return head;

}

swf_soundstreamhead *
swf_parse_soundstreamhead2 (swf_parser * context, int * error)
{
    return swf_parse_soundstreamhead (context, error);
}

swf_definebitslossless *
swf_parse_definebitslossless (swf_parser * context, int * error)
{
    swf_definebitslossless * bits;

    if ((bits = (swf_definebitslossless *) calloc (1, sizeof(swf_definebitslossless))) == NULL) {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    bits->tagid = swf_parse_get_word (context);

    bits->format = swf_parse_get_byte(context);
    bits->width  = swf_parse_get_word(context);
    bits->height = swf_parse_get_word(context);
    bits->colourtable_size = 0;

    if (bits->format == 3) {
        bits->colourtable_size = swf_parse_get_byte (context);
    }

    bits->colourtable_size++;

    bits->data = swf_parse_get_bytes (context, context->tagend - swf_parse_tell(context));


    return bits;




}

swf_definebitslossless *
swf_parse_definebitslossless2 (swf_parser * context, int * error)
{
    return swf_parse_definebitslossless (context, error);

}


swf_textrecord *
swf_parse_get_textrecord (swf_parser * context, int * error, int has_alpha, int glyph_bits, int advance_bits)
{
    int g;
    swf_textrecord * record;
    SWF_U8 flags = swf_parse_get_byte(context);



    if (flags == 0) { return NULL; }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecord : mallocing]\n");
    #endif

    if ((record = (swf_textrecord *) calloc (1, sizeof(swf_textrecord))) == NULL)
    {
        *error = SWF_EMallocFailure;
        return NULL;
    }

    record->flags = flags;
    record->next = NULL;


    if (flags & isTextControl)
    {
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : is text control ");
        #endif
        if (flags & textHasFont)
        {

            record->font_id = swf_parse_get_word(context);
            #ifdef SWF_PARSE_DEBUG
            fprintf(stderr,"font id is %ld", record->font_id);
            #endif
        }
        if (flags & textHasColour)
        {
            record->colour = swf_parse_get_colour (context, error,  has_alpha);
        }
        if (flags & textHasXOffset)
        {
            record->xoffset = swf_parse_get_word(context);
        }
        if (flags & textHasYOffset)
        {
            record->yoffset = swf_parse_get_word(context);
        }
        if (flags & textHasFont)
        {
            record->font_height = swf_parse_get_word(context);

        }
        #ifdef SWF_PARSE_DEBUG
        fprintf(stderr,"]\n");
        #endif
    }
    else
    {
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : is glyphs]\n");
        #endif

        record->glyph_count = flags;
        if ((record->glyphs = (int **) calloc (record->glyph_count, sizeof (int *))) == NULL)
        {
           *error = SWF_EMallocFailure;
            goto FAIL;
        }

        swf_parse_initbits(context);     // reset bit counter

        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[get_textrecord : getting %d glyphs]\n", record->glyph_count);
        #endif

        for ( g = 0; g < record->glyph_count; g++)
        {
            record->glyphs[g] = NULL;
            if ((record->glyphs[g] = (int *) calloc (2, sizeof(int))) == NULL)
            {
               *error = SWF_EMallocFailure;
                goto FAIL;

            }

            record->glyphs[g][0] = swf_parse_get_bits (context, glyph_bits);    // index
            record->glyphs[g][1] = swf_parse_get_bits (context, advance_bits);  // advance
        }


    }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecord : returning]\n");
    #endif

    return record;

    FAIL:
    swf_destroy_textrecord (record);
    return NULL;

}

swf_textrecord_list  *
swf_parse_get_textrecords (swf_parser * context, int * error, int has_alpha, int glyph_bits, int advance_bits)
{
    swf_textrecord_list * list;
    swf_textrecord * temp;

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecords : mallocing]\n");
    #endif

    if ((list = (swf_textrecord_list *) calloc (1, sizeof (swf_textrecord_list))) == NULL)
    {
        goto FAIL;
    }

    list->first = NULL;
    list->lastp = &(list->first);

    while (1)
    {
            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[get_textrecords : attempting to get  a record]\n");
            #endif

	        if ((temp = swf_parse_get_textrecord(context, error, has_alpha, glyph_bits, advance_bits)) == NULL)
            {
                //if (error != SWF_ENoError)
                //{
                //    goto FAIL;
                //}
               break;
	        }

            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[get_textrecords : got a record]\n");
            #endif

            *(list->lastp) = temp;
	        list->lastp = &(temp->next);
    }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_textrecords : returning]\n");
    #endif

    return list;

    FAIL:
    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[get_text_records : FAILED!]\n");
    #endif
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
            #ifdef SWF_PARSE_DEBUG
            fprintf(stderr, "\tEnd of shape.\n\n");
            #endif
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

    } else {

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

    if (list==NULL)
    {
        /* todo simon : should eal with thsi more cleanly */
        return NULL;
    }

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[textrecords_to_text : list is not null]\n");
    #endif
    node = list->first;

    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[textrecords_to_text : node is %s]\n", (node==NULL)?"NULL":"Ok");
    #endif

    /* todo simon : if node == NULL need to cope */

    while (node != NULL)
    {
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[textrecords_to_text : node is not NULL]\n");
        #endif

        if (node->flags & isTextControl)
        {
            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[textrecords_to_text : it's a text control]\n");
            #endif

            if ( node->flags & textHasFont)
            {
                font_id = node->font_id;
                #ifdef SWF_PARSE_DEBUG
                fprintf (stderr, "[textrecords_to_text : font_id is %d]\n", font_id);
                #endif
            }
        }else{

            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[textrecords_to_text : mallocing string]\n");
            #endif

            /* malloc to the size of the string */
            if ((str = (char *) calloc (node->glyph_count+1, sizeof (char))) == NULL)
            {
                *error = SWF_EMallocFailure;
                return NULL;
            }

            /* check fontchars[font_id] */
            if (context->font_chars[font_id] == NULL)
            {
                *error = SWF_EFontNotSet;
                return NULL;
            }

            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[textrecords_to_text : number of glyphs is  %d]\n", node->glyph_count);
            #endif

            /* ... and then set it */
            for (g=0; g< node->glyph_count; g++)
            {
                /* this requires that font_chars[font_id] has actually been set of course*/
                str[g] = context->font_chars[font_id][node->glyphs[g][0]];

            }
            str[g]='\0';

            /* todo :sometimes there's more than one peice of text ... we shoudl probably cope with that as well */
            #ifdef SWF_PARSE_DEBUG
            fprintf (stderr, "[textrecords_to_text : string is  '%s']\n", str);
            #endif

            return str;
        }



        tmp = node;
        node = node->next;
    }



    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[textrecords_to_text : attempting to return]\n");
    #endif

    if (font_id == -1) {
        /* somethings gone wrong */
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[textrecords_to_text : EFontNotSet]\n");
        #endif
        //todo simon : why is this not working *error = SWF_EFontNotSet;
        return NULL;
    }

    if (str == NULL) {
        /* somethings gone wrong */
        #ifdef SWF_PARSE_DEBUG
        fprintf (stderr, "[textrecords_to_text : EStringNotSet]\n");
        #endif
        *error = SWF_EFontNotSet; /*todo :  need to change error message */
        return NULL;
    }
    /* et voila */
    #ifdef SWF_PARSE_DEBUG
    fprintf (stderr, "[textrecords_to_text : returning]\n");
    #endif

    return str;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
