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
 * $Log: swf_types.h,v $
 * Revision 1.27  2002/05/27 21:58:45  kitty_goth
 * Make tags use the swf_movie function swf_dump_shape
 *
 * Revision 1.26  2002/05/10 17:07:51  kitty_goth
 * Right, this leaks like a son of a bitch, and I've only tested it against
 * gen_test1, but the buffer delta should work now.
 *
 * Please hack on it lots, it needs it.
 *
 * Revision 1.25  2001/08/09 10:18:36  acme
 * Added todo for simon ;-)
 *
 * Revision 1.24  2001/07/16 15:05:16  clampr
 * get rid of glib due to randomness (I suspect it may have been a dynamic linking issue)
 *
 * add in a homebrew linked list type for font_extras (ick)
 *
 * Revision 1.23  2001/07/16 01:41:25  clampr
 * glib version of font management
 *
 * Revision 1.22  2001/07/14 00:17:55  clampr
 * added emacs file variables to avoid clashing with existing style (now I know what it is)
 *
 * Revision 1.21  2001/07/13 13:46:47  clampr
 * use  swf_memory.h not swf_sizes.h
 *
 * Revision 1.20  2001/07/13 13:26:55  muttley
 * Added handling for button2actions.
 * We should be able to parse all URLs now
 *
 * Revision 1.19  2001/07/12 13:16:28  muttley
 * Whoops, needed to uncomment the definitions for SCOORD
 *
 * Revision 1.18  2001/07/12 13:00:47  muttley
 * Use proper, configure generated architecture independant type sizes
 *
 * Revision 1.17  2001/07/12 11:33:25  muttley
 * *cough* remove a couple of commented out bits so it actually works
 *
 * Revision 1.16  2001/07/12 11:27:08  muttley
 * Remove all the tags and put them in swf_tags
 * Add comments for every type
 * Put in lots of TODOs :P
 *
 * Revision 1.15  2001/07/09 15:48:54  acme
 * Renamed U32 to SWF_U32 and so on
 *
 * Revision 1.14  2001/07/09 12:47:59  muttley
 * Changes for lib_swfextract and text_extract
 *
 * Revision 1.12  2001/06/30 12:33:19  kitty_goth
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
 * Revision 1.10  2001/06/26 17:40:30  kitty_goth
 * Bug fix for swf_parse_get_bytes to fix sound stream stuff. --Kitty
 *
 * Revision 1.9  2001/06/26 13:45:58  muttley
 * Change swf_text_records
 *
 */


#ifndef SWF_TYPES_H
#define SWF_TYPES_H

#define TRUE (1)
#define FALSE (0)

#include <stdio.h>

#define tagEnd                 (0)
#define tagShowFrame           (1)
#define tagDefineShape         (2)
#define tagFreeCharacter       (3)
#define tagPlaceObject         (4)
#define tagRemoveObject        (5)
#define tagDefineBits          (6)
#define tagDefineButton        (7)
#define tagJPEGTables          (8)
#define tagSetBackgroundColour (9)
#define tagDefineFont          (10)
#define tagDefineText          (11)
#define tagDoAction            (12)
#define tagDefineFontInfo      (13)
#define tagDefineSound         (14)   /* Event sound tags. */
#define tagStartSound          (15)
#define tagDefineButtonSound   (17)
#define tagSoundStreamHead     (18)
#define tagSoundStreamBlock    (19)
#define tagDefineBitsLossless  (20)   /* A bitmap using lossless zlib compression. */
#define tagDefineBitsJPEG2     (21)   /* A bitmap using an internal JPEG compression table.*/
#define tagDefineShape2        (22)
#define tagDefineButtonCxform  (23)
#define tagProtect             (24)   /* This file should not be importable for editing.*/

/* These are the new tags for Flash 3.*/
#define tagPlaceObject2        (26)   /* The new style place w/ alpha color transform and name.*/
#define tagRemoveObject2       (28)   /* A more compact remove object that omits the character tag (just depth).*/
#define tagDefineShape3        (32)   /* A shape V3 includes alpha values.*/
#define tagDefineText2         (33)   /* A text V2 includes alpha values.*/
#define tagDefineButton2       (34)   /* A button V2 includes color transform) alpha and multiple actions*/
#define tagDefineBitsJPEG3     (35)   /* A JPEG bitmap with alpha info.*/
#define tagDefineBitsLossless2 (36)   /* A lossless bitmap with alpha info.*/
#define tagDefineEditText      (37)   /* An editable Text Field*/
#define tagDefineSprite        (39)   /* Define a sequence of tags that describe the behavior of a sprite.*/
#define tagNameCharacter       (40)   /* Name a character definition) character id and a string) (used for buttons) bitmaps) sprites and sounds).*/
#define tagFrameLabel          (43)   /* A string label for the current frame.*/
#define tagSoundStreamHead2    (45)   /* For lossless streaming sound) should not have needed this...*/
#define tagDefineMorphShape    (46)   /* A morph shape definition*/
#define tagDefineFont2         (48)


#define FONT_UNICODE     (0x20)
#define FONT_SHIFT_JIS   (0x10)
#define FONT_ANSI        (0x08)
#define FONT_ITALIC      (0x04)
#define FONT_BOLD        (0x02)
#define FONT_WIDE_CODES  (0x01)


#define splaceMove           (0x01) /* this place moves an exisiting object */
#define splaceCharacter      (0x02) /* there is a character tag (if no tag) must be a move) */
#define splaceMatrix         (0x04) /* there is a matrix (matrix) */
#define splaceColorTransform (0x08) /* there is a color transform (cxform with alpha) */
#define splaceRatio          (0x10) /* there is a blend ratio (word) */
#define splaceName           (0x20) /* there is an object name (string) */
#define splaceDefineClip     (0x40) /* this shape should open or close a c */


/* Edit Text Flags */
#define seditTextFlagsHasFont       (0x0001)
#define seditTextFlagsHasMaxLength  (0x0002)
#define seditTextFlagsHasTextColor  (0x0004)
#define seditTextFlagsReadOnly      (0x0008)
#define seditTextFlagsPassword      (0x0010)
#define seditTextFlagsMultiline     (0x0020)
#define seditTextFlagsWordWrap      (0x0040)
#define seditTextFlagsHasText       (0x0080)
#define seditTextFlagsUseOutlines   (0x0100)
#define seditTextFlagsBorder        (0x0800)
#define seditTextFlagsNoSelect      (0x1000)
#define seditTextFlagsHasLayout     (0x2000)

/* Edit Text Fields */
#define sfontFlagsBold          (0x01)
#define sfontFlagsItalic        (0x02)
#define sfontFlagsWideCodes     (0x04)
#define sfontFlagsWideOffsets   (0x08)
#define sfontFlagsANSI          (0x10)
#define sfontFlagsUnicode       (0x20)
#define sfontFlagsShiftJIS      (0x40)
#define sfontFlagsHasLayout     (0x80)

#define fillGradient        (0x10)
#define fillLinearGradient  (0x10)
#define fillRadialGradient  (0x12)
#define fillMaxGradientColors   (8)
/* Texture/bitmap fills */
#define fillBits            (0x40)    /* if this bit is set) must be a bitmap pattern */


/* Start Sound Flags */
#define soundHasInPoint     (0x01)
#define soundHasOutPoint    (0x02)
#define soundHasLoops       (0x04)
#define soundHasEnvelope    (0x08)
/* the upper 4 bits are reserved for synchronization flags */

/* Flags for defining a shape character */

/* These flag codes are used for state changes*/
#define eflagsMoveTo       (0x01)
#define eflagsFill0        (0x02)
#define eflagsFill1        (0x04)
#define eflagsLine         (0x08)
#define eflagsNewStyles    (0x10)

#define eflagsEnd          (0x80)  /* a state change with no change marks the end */

/* Text Flags */
#define isTextControl  (0x80)
#define textHasFont    (0x08)
#define textHasColour  (0x04)
#define textHasYOffset (0x02)
#define textHasXOffset (0x01)


/* Action Codes */
#define sactionNone                     (0x00)
#define sactionNextFrame                (0x04)
#define sactionPrevFrame                (0x05)
#define sactionPlay                     (0x06)
#define sactionStop                     (0x07)
#define sactionToggleQuality            (0x08)
#define sactionStopSounds               (0x09)
#define sactionAdd                      (0x0A)
#define sactionSubtract                 (0x0B)
#define sactionMultiply                 (0x0C)
#define sactionDivide                   (0x0D)
#define sactionEqual                    (0x0E)
#define sactionLessThan                 (0x0F)
#define sactionLogicalAnd               (0x10)
#define sactionLogicalOr                (0x11)
#define sactionLogicalNot               (0x12)
#define sactionStringEqual              (0x13)
#define sactionStringLength             (0x14)
#define sactionSubString                (0x15)
#define sactionInt                      (0x18)
#define sactionEval                     (0x1C)
#define sactionSetVariable              (0x1D)
#define sactionSetTargetExpression      (0x20)
#define sactionStringConcat             (0x21)
#define sactionGetProperty              (0x22)
#define sactionSetProperty              (0x23)
#define sactionDuplicateClip            (0x24)
#define sactionRemoveClip               (0x25)
#define sactionTrace                    (0x26)
#define sactionStartDragMovie           (0x27)
#define sactionStopDragMovie            (0x28)
#define sactionStringLessThan           (0x29)
#define sactionRandom                   (0x30)
#define sactionMBLength                 (0x31)
#define sactionOrd                      (0x32)
#define sactionChr                      (0x33)
#define sactionGetTimer                 (0x34)
#define sactionMBSubString              (0x35)
#define sactionMBOrd                    (0x36)
#define sactionMBChr                    (0x37)
#define sactionHasLength                (0x80)
#define sactionGotoFrame                (0x81) /* frame num (WORD)        */
#define sactionGetURL                   (0x83) /* url (STR)) window (STR) */
#define sactionWaitForFrame             (0x8A) /* frame needed (WORD))    */
                                               /* actions to skip (BYTE)  */
#define sactionSetTarget                (0x8B) /* name (STR)              */
#define sactionGotoLabel                (0x8C) /* name (STR)              */
#define sactionWaitForFrameExpression   (0x8D) /* frame needed on stack)  */
                                               /* actions to skip (BYTE)  */
#define sactionPushData                 (0x96)
#define sactionBranchAlways             (0x99)
#define sactionGetURL2                  (0x9A)
#define sactionBranchIfTrue             (0x9D)
#define sactionCallFrame                (0x9E)
#define sactionGotoExpression           (0x9F)


/* Global Types*/

/*
 * Hmm, this throws up lots of warnings so revoke to having
 * everything
 */
#include "swf_memory.h"

typedef signed long SFIXED, *P_SFIXED;
typedef signed long SCOORD, *P_SCOORD;


typedef struct swf_buffer swf_buffer;
typedef struct swf_tag swf_tag;
typedef struct swf_tagrecord swf_tagrecord;
typedef struct swf_rect swf_rect;
typedef struct swf_colour swf_colour;
typedef struct swf_gradcolour swf_gradcolour;
typedef struct swf_cxform swf_cxform;
typedef struct swf_matrix swf_matrix;

typedef struct swf_header swf_header;
typedef struct swf_parser swf_parser;
typedef struct swf_movie swf_movie;


typedef struct swf_shapestyle swf_shapestyle;
typedef struct swf_linestyle swf_linestyle;
typedef struct swf_linestyle2 swf_linestyle2;
typedef struct swf_fillstyle swf_fillstyle;
typedef struct swf_fillstyle2 swf_fillstyle2;
typedef struct swf_rgba_pos swf_rgba_pos;
typedef struct swf_soundpoint swf_soundpoint;
typedef struct swf_imageguts swf_imageguts;

typedef struct swf_kerningpair swf_kerningpair;
typedef struct swf_adpcm swf_adpcm;
typedef struct swf_mp3header swf_mp3header;
typedef struct swf_mp3header_list swf_mp3header_list;

typedef struct swf_textrecord swf_textrecord;
typedef struct swf_textrecord_list swf_textrecord_list;

typedef struct swf_buttonrecord swf_buttonrecord;
typedef struct swf_buttonrecord_list swf_buttonrecord_list;

typedef struct swf_doaction swf_doaction;
typedef struct swf_doaction_list swf_doaction_list;

typedef struct swf_shaperecord swf_shaperecord;

typedef struct swf_shaperecord_list swf_shaperecord_list;

typedef struct swf_button2action swf_button2action;
typedef struct swf_button2action_list swf_button2action_list;

typedef struct swf_setbackgroundcolour swf_setbackgroundcolour;
typedef struct swf_definesound swf_definesound;
typedef struct swf_definefont swf_definefont;
typedef struct swf_definefont2 swf_definefont2;
typedef struct swf_definefontinfo swf_definefontinfo;
typedef struct swf_placeobject swf_placeobject;
typedef struct swf_placeobject2 swf_placeobject2;
typedef struct swf_defineshape swf_defineshape;
typedef struct swf_definemorphshape swf_definemorphshape;
typedef struct swf_freecharacter swf_freecharacter;
typedef struct swf_namecharacter swf_namecharacter;
typedef struct swf_removeobject swf_removeobject;
typedef struct swf_removeobject2 swf_removeobject2;
typedef struct swf_startsound swf_startsound;
typedef struct swf_definebits swf_definebits;
typedef struct swf_jpegtables swf_jpegtables;
typedef struct swf_definebitsjpeg2 swf_definebitsjpeg2;
typedef struct swf_definebitsjpeg3 swf_definebitsjpeg3;
typedef struct swf_definebitslossless swf_definebitslossless;
typedef struct swf_definetext swf_definetext;
typedef struct swf_definetext2 swf_definetext2;
typedef struct swf_definebutton swf_definebutton;
typedef struct swf_definebutton2 swf_definebutton2;
typedef struct swf_defineedittext swf_defineedittext;
typedef struct swf_framelabel swf_framelabel;
typedef struct swf_definebuttoncxform swf_definebuttoncxform;
typedef struct swf_definebuttonsound swf_definebuttonsound;
typedef struct swf_soundstreamblock swf_soundstreamblock;
typedef struct swf_soundstreamhead swf_soundstreamhead;

typedef struct swf_font_extra swf_font_extra;


struct swf_header {
    SWF_U32 size;      /* size of file in bytes */
    SWF_U32 version;   /* single byte file version */
    SWF_U32 rate;      /* frame rate. delay in 8.8 fixed number of frames per second */
    SWF_U32 count;     /* total number of frames in a movie*/
    swf_rect * bounds; /* the xmin, xmax, ymin and ymax of the movie */
};

struct swf_buffer {
    /* Bit Handling. */
    SWF_S32 bitpos;       /* what position we're at in the bit buffer */
    SWF_U32 bitbuf;       /* the bit buffer, used for storing bits */

    SWF_U8 * raw;
    SWF_U32 size;
};

struct swf_parser {
    FILE * file;          /* The file handle of the SWF file we're parsing */
    char * name;          /* the name of the file we're handling */
    swf_header * header;  /* the headers of this swf */
	swf_buffer * buffer;   /* The buffer structure */

    int headers_parsed;   /* have the headers been parsed yet */

    SWF_U32 cur_tag_len;  /* the length of the current tag we're parsing */
    SWF_U32 filepos;
    SWF_U32 next_tag_pos; /* what the position of the next tag is */
    SWF_U32 frame;        /* what the current frame we're on is */

    /* Tag parsing information. */

    SWF_U32 tagstart;
    SWF_U32 tagzero;
    SWF_U32 tagend;

    SWF_U32 taglen;

    int fill_bits;       /* the current number of fill bits we're using */
    int line_bits;       /* the current number of fill bits we're using */
                         /* todo simon : not sure these should be in the context */

    SWF_U8* src_adpcm;          /* the src of the adpcm we're parsing*? ?* todo simon: should this be in the context?*/
    SWF_U32 bitbuf_adpcm;       /* adpcm bit buffer : this should always contain at least 24 bits of data */
    SWF_S32 bitpos_adpcm;       /* the current position in the adpcm bit buffer */
    SWF_U32 n_samples_adpcm;    /* number of samples decompressed so far */

    /* Streaming sound info from SoundStreamHead tag */
    int stream_compression;
    int stream_sample_rate;
    int stream_sample_size;
    int stream_sample_stereo_mono;
    int n_stream_samples;

	swf_font_extra *font_extras;
};

struct swf_tagrecord {
    SWF_U16 id; /* ID of tag */
    void * tag; /* This is a pointer to the tag structure */

    int serialised; /* Flag, used to indicate that a buffer is fully parsed and RTG*/

    swf_buffer * buffer; /* Buffer structure excludes tag header */
    swf_tagrecord *  next;     /* pointer to the next element in the list */
};

struct swf_movie {
    FILE * file;          /* The file handle of the SWF file we're parsing */
    char * name;          /* the name of the file we're handling */
    swf_header * header;
	swf_buffer * buffer;  /* Scratchpad buffer just in case */

    SWF_U32 filepos;      /* We will need to do seeks before outputing*/

    SWF_U32 max_obj_id;    /* the highest id of the objects we've created */

    swf_tagrecord *  first;     /* pointer to the first element in the list */
    swf_tagrecord ** lastp;    /* pointer to the last element in the list */
};

struct swf_font_extra {
	int fontid;
	swf_font_extra *next;
	int n;
	char *glyphs;
	char *chars;
};

/* a raw swf tag */
/* todo simon : this should have the raw data in as well */
struct swf_tag {
    SWF_U16 id;   /* the id of the tag */
    SWF_U32 len;  /* how long it is */
};

/* just holds the bounds of a rectangle */
struct swf_rect {
    SCOORD xmin;  /* these are fairly self explanatory */
    SCOORD xmax;
    SCOORD ymin;
    SCOORD ymax;
};

/* structure for holding a colour */
struct swf_colour {
    SWF_U32 r; /* red */
    SWF_U32 g; /* green */
    SWF_U32 b; /* blue */
    SWF_U32 a; /* alpha/transparency */
};



/*
 * A colour transform
 * a is multiply factor ...
 * ... b is addition factor
 */
struct swf_cxform {
    SWF_S32 flags; /* todo simon: is this necessary ?*/
    SWF_S16 ra;    /* red */
    SWF_S16 rb;
    SWF_S16 ga;    /* green */
    SWF_S16 gb;
    SWF_S16 ba;    /* blue */
    SWF_S16 bb;
    SWF_S16 aa;    /* alpha */
    SWF_S16 ab;
};

/* a transformation matrix */
struct swf_matrix {
    SFIXED a;  /* scale  x */  /* todo simon : check these, possibly rename ? */
    SFIXED b;  /* scale  y */
    SFIXED c;  /* rotate x */
    SFIXED d;  /* rotate y */
    SCOORD tx; /* transform x */
    SCOORD ty; /* transform y */
};

/* set the back ground colour */
struct swf_setbackgroundcolour {
    swf_colour * colour;
};


/* define a font */
struct swf_definefont {
    SWF_U32 fontid;  /* the font we're defining */
    int offset;      /* its offset in the glyph tables */ /* todo simon : check this */
    int glyph_count; /* the number of glyphs it has */
    swf_shaperecord_list ** shape_records; /* an array of shapes which define how the glyphs look*/
};


struct swf_definefontinfo {
    SWF_U32 fontid;   /* the id of the font this is referring to */
    SWF_U8 flags;     /* some flags to set whether it's wide, italic, bold etc etc */ /* todo simon : is this necessary */
    int namelen;      /* how long the name of the font is */ /* todo simon: is this necessary ? */
    int * code_table; /* the ascii values of each of the glyphs defined */
    char * fontname;  /* the name of the font */
};

struct swf_placeobject {
    SWF_U32 tagid;        /* the id of the object to place */ /* todo simon : should this be renamed */
    SWF_U32 depth;        /* the depth to place it with */
    swf_matrix * matrix;  /* the transformation matrix to place it with */
    swf_cxform * cxform;  /* the colour transformation matrix to place this with */

};

/*
 * TODO
 * Should we change the property tag to be called tagid to
 * match swf_placeobject? --BE
 */

struct swf_placeobject2 {
    SWF_U8 flags;           /* flags defning whther or not it has a matrix, name etc etc */ /* todo simon : is this necessary?*/
    SWF_U32 depth;          /* the depth that we place the object at */
    SWF_U32 tag;            /* the id of the object we're placing c.f comment from BE above */
    swf_matrix * matrix;    /* the transformation matrix to palce it with */
    swf_cxform * cxform;    /* the colour transformation matrix to place it with */
    SWF_U32 ratio;          /* Used in tweening. Indicates the amount of target shape in an intermediate tween shape */
    SWF_U32 clip_depth;     /* todo simon : whatis this? it doesn't seem to be in the specs */
    char * name;            /* name of the character */
};

struct swf_defineshape {
    SWF_U32 tagid;                   /* the id of the object we're creating */
    swf_rect * rect;                 /* the bounds of the shape */
    swf_shapestyle * style;          /* the style of the shape */
    swf_shaperecord_list * record;   /* the records defining the shape */

};

struct swf_shapestyle {
    SWF_U16 nfills;                  /* the number of fills it has */
    swf_fillstyle ** fills;          /* the styles of thos fills */

    SWF_U16 nlines;                  /* the number of lines it has */
    swf_linestyle ** lines;          /* the style of those lines */
};

struct swf_fillstyle {
    SWF_U16 fill_style;              /* the type : 0x00 = solid fill,
                                                   0x10 = linear gradient fill,
                                                   0x12 = radial gradient fill,
                                                   0x40 = tiled bitmap fill,
                                                   0x41 = clipped bitmap fill
                                      */
    SWF_U16 ncolours;                 /* the number of colours used in a gradient fill */
    SWF_U16 bitmap_id;                /* the id of bitmap used in a bitmap fill */
    swf_matrix * matrix;              /* the transformation matrix for the bitmap */
    swf_rgba_pos ** colours;          /* the colours used in the gradient fill and the positions they're in*/
    SWF_U32 colour;                   /* the colour used in the solid fill */
};

struct swf_linestyle {
    SWF_U16 width;                    /* the width of the line */
    SWF_U32 colour;                   /* the colour fo the line */
};

struct swf_rgba_pos {
    SWF_U32 rgba;                    /* an rgba value */ /* todo simon : shoudl this be a swf_colour type? */
    SWF_U8 pos;                      /* the position of the colour */
                                     /* todo simon : I'm not sure, looking at the spec, that this is correct. */
};


struct swf_freecharacter {
    SWF_U32 tagid;                   /* the id of the object we're freeing */
};


struct swf_removeobject {
    SWF_U32 tagid;                   /* the id of the object we're removing from the screen */
    SWF_U32 depth;                   /* the depth of the object we're removing */
};

struct swf_removeobject2 {
    SWF_U32 depth;                   /* the depth of the object we're removing */
};

struct swf_startsound {
    SWF_U32 tagid;                   /* the id of the sound to start*/
    SWF_U32 code;                    /* flags to deteremine the what this has */ /* todo simon : is this necessary? */
    SWF_U32 inpoint;                 /* sound in point value */
    SWF_U32 outpoint;                /* sound out point value */ /* todo simon : I ahve NO idea what these actually are */
    SWF_U32 loops;                   /* how many loops this has */
    SWF_U8  npoints;                 /* how many points this has */
    swf_soundpoint ** points;        /* the sound points */
};

struct swf_soundpoint {
    SWF_U32 mark;                    /* mark 44 information */
    SWF_U32 lc;                      /* level 0 info */
    SWF_U32 rc;                      /* level 1 info */ /* todo simon : this differes from the spec */
};


/* Defines a bitmap character with JPEG compression.
 * This tag contains only the JPEG compressed image data
 * (as defined by the Independent JPEG Group). JPEGTables
 * contains the JPEG encoding data used to encode this image.
 */

struct swf_definebits {
    SWF_U32 tagid;                  /* the id */
    swf_imageguts * guts;           /* the JPEG compressed image */

};

/* Defines a bitmap character with JPEG compression.
 * This tag differs from DefineBits in that the record
 * contains both the JPEG encoding table and the JPEG image data
 *
 * Note: The JPEG encoding data and image data contained within
 * this record are in two separate JPEG streams. Each JPEG stream
 * has a beginning of stream tag (0xFF, 0xD8) and an end of stream
 * tag (0xFF, 0xD9). This differs from a standard JPEG image which
 * combines the encoding data and image data into a single stream
 */

struct swf_definebitsjpeg2 {
    SWF_U32 tagid;              /* the id of this image */
    swf_imageguts * guts;       /* the image data */

};

/*
 * Defines a bitmap character with JPEG compression.
 * This tag differs from DefineBitsJPEG2 because it
 * contains alpha channel data. This record allows
 * multiple JPEG images with differing compression
 * ratios to be defined within a single SWF file.
 *
 * The streams are delimited in the same way as DefineBitsJPEG2
 */
struct swf_definebitsjpeg3 {
    SWF_U32 tagid;              /* the id of this image */
    swf_imageguts * guts;       /* the image data */

};

struct swf_imageguts {
    SWF_U32 nbytes;             /* the number of bytes of data */
    SWF_U8  * data;             /* the data */
};

/*
 * the jpeg encoding table for definbits -
 * there should only ever be one of these
 * all the other definebits use it
 */
struct swf_jpegtables {
    swf_imageguts * guts;       /* the encoding table */
};


struct swf_definetext {
    SWF_U32 tagid;                  /* the id of this text */
    swf_rect    * rect;             /* the bounds of the text */
    swf_matrix  * matrix;           /* the transformation matrix of this text */
    swf_textrecord_list * records;  /* the list of text records - duh! */
};


/*
 * this only differs from definetext in that it
 * can have alpha transparency values in the text records
 */
struct swf_definetext2 {
    SWF_U32 tagid;                  /* the id of this text */
    swf_rect    * rect;             /* the bounds of this text */
    swf_matrix  * matrix;           /* the transformation matrix of this text */
    swf_textrecord_list  * records; /* the list of text records */
};


struct swf_definebutton {
    SWF_U32 tagid;                      /* the id for this button */
    swf_buttonrecord_list * records;    /* a list of button records */
    swf_doaction_list     * actions;    /* a list of actions */
};

/* FIXME: Doesn't this need actions as well? */
struct swf_definebutton2 {
    SWF_U32 tagid;                      /* the id for this button */
    swf_buttonrecord_list * records;    /* the button records */
    swf_button2action_list * actions;   /* the button2 action conditions */
};



struct swf_defineedittext {
    SWF_U32 tagid;          /* todo simon */
    swf_rect * bounds;
    SWF_U16 flags;
    SWF_U16 font_id;
    SWF_U16 font_height;
    SWF_U32 colour;
    int max_length;
    int align;
    SWF_U16 left_margin;
    SWF_U16 right_margin;
    SWF_U16 indent;
    SWF_U16 leading;
    char * variable;
    char * initial_text;
};

/*
 * Defines the mapping from a font object to a device font
 * so that the player can optionally use a TrueType or ATM font
 * that may be installed on the computer instead of the stored glyph outlines.
 */
struct swf_definefont2 {
	SWF_U32 fontid;                   /* the font id for this information */
	SWF_U16 flags;                    /* various flags for whether it has layout/is unicode etc etc. */ /* todo simon: is this necessary ?*/
	int name_len;                     /* the length of the name */ /* todo simon : is this necessary ? */
	char * name;                      /* the font name */
	SWF_U16 glyph_count;              /* the number of glyphs */ /* todo simon  is this necessary ? */
	swf_shaperecord_list ** glyphs;   /* the glyphs */
	SWF_U32 * code_table;             /* the code table */
	SWF_S16 ascent;                   /* the ascent of this font */
	SWF_S16 descent;                  /* the descent */
	SWF_S16 leading;                  /* the leading of this font */
	swf_rect ** bounds;               /* the bounds of each glyph of the font */
	SWF_S16 nkerning_pairs;           /* the number of kerning pairs */
	swf_kerningpair ** kerning_pairs; /* the pairs them selves */
    /* todo simon - the spec has offset tables but no kerning tables ... hmmm */

};

struct swf_kerningpair {

	SWF_U16 code1;  /* todo simon : not sure exactly what these mean */
	SWF_U16 code2;
	SWF_S16 adjust;
};


/* Defines the metamorphosis of one shape (Shape1) into another (Shape2) */
struct swf_definemorphshape  {
    SWF_U32 tagid;                      /* the id of this morph shape */
    swf_rect * r1;                      /* bounds of shape 1 */
    swf_rect * r2;                      /* bounds of shape 2 */
    int nfills;                         /* the number of fills */
    swf_fillstyle2 ** fills;            /* the fills themselves */   /* todo simon :  should these be morph fill/line styles ?*/
    int nlines;                         /* the number of lines */
    swf_linestyle2 ** lines;            /* the lines themselves */
    swf_shaperecord_list * records1;    /* shape 1 */
    swf_shaperecord_list * records2;    /* shape 2 */
};

/* todo simon : maybe this should renamed */
struct swf_gradcolour {


    SWF_U8 r1;      /* ratio for first shape */
    SWF_U8 r2;      /* ratio for the second shape */
    SWF_U32 c1;     /* colour for first shape */
    SWF_U32 c2;     /* colour for second shape */
};

/* todo simon : maybe this should be renamed */
struct swf_fillstyle2 {
    int style;                  /* the style of the fill
                                 * 0x00 = solid fill
                                 * 0x10 = linear gradient fill
                                 * 0x12 = radial gradient fill
                                 * 0x41 = clipped bitmap fill
                                 */

    swf_matrix * matrix1;       /* matrix for the gradient of the first shape */
    swf_matrix * matrix2;       /* matrix for the gradient of the second shape */
    int ncolours;               /* the number of colours */ /* todo simon : is this always 2 ? */
    swf_gradcolour ** colours;  /* grad colours */ /* todo simon : check this, not sure if it's right */
    SWF_U16 tag;                /* the bitmap for a bitmap fill */
    SWF_U32 rgb1;               /* colour for first shape (if solid fill) */
    SWF_U32 rgb2;               /* colour for second shape (if solid fill) */
};

/* should this be renamed ?*/
struct swf_linestyle2 {
    SWF_U16 thick1;             /* thickness of the first line in twips*/
    SWF_U16 thick2;             /* thickness of the second line in twips */
    SWF_U32 rgb1;               /* colour of the first line with alpha */
    SWF_U32 rgb2;               /* colour of the second line with alpha */

};



struct swf_definesound {
    SWF_U32 tagid;              /*  the id for this sound */ /* todo should this be renamed ? */
    int compression;            /* how is it compressed 0 = None 1 = ADPCM */
    int sample_rate;            /* sample rate 0=5k, 1 =11k, 2=22k, 3=44k */
    int sample_size;            /* the sample size 0 = 8 bit, 1 = 16 bit */
    int stereo_mono;            /* is it stereo or mono ? 0 = Mono, 1 = Stereo */
    int sample_count;           /* the number of samples */
    int delay;                  /* the delay */  /*  todo simon : this is not in the spec */
    swf_adpcm * adpcm;          /* adpcm data */ /*  todo simon : this is not in the spec */
    swf_mp3header_list * mp3header_list; /* the mp3 headers */ /*  todo simon : this is not in the spec */

};

/* todo simon */
struct  swf_adpcm {
    SWF_U32 tagid;

};

struct swf_framelabel {

    char * label;   /* label this frame with a word */

};


/* todo simon : not in spec ? */
struct swf_namecharacter {

    SWF_U32 tagid;
    char * label;

};

/*
 * Defines the color transform for each shape and text
 * character in a button. Not used for DefineButton2
 */
struct swf_definebuttoncxform {

    SWF_U32 tagid;          /* the button Id for this information */
    int ncxforms;           /* the number of cxforms */
    swf_cxform ** cxforms;  /* the cxforms */
    /* todo simon : according to the spec there is only one cxform for each button record */
};


/* todo simon : I think this is all wrong */
struct swf_soundstreamblock {

    SWF_U32 tagid;
    int n_samples_adpcm;
    int stream_compression;
    int stream_sample_rate;
    int stream_sample_size;
    int stream_sample_stereo_mono;
    int n_stream_samples;
    int delay;
    swf_adpcm * adpcm;
    swf_mp3header_list * mp3header_list;
    int samples_per_frame;

};


struct swf_definebuttonsound {
    SWF_U32 tagid;                  /* the id of the button this refers to */
    swf_startsound * up_state;      /* the sound that plays in the UP state */
    swf_startsound * over_state;    /* the sound that plays in the OVER state */
    swf_startsound * down_state;    /* the sound that plays in the DOWN state */
};

struct swf_soundstreamhead {

    SWF_U32 tagid;                  /* todo simon : not in spec */
    int mix_format;                 /* reccomended mix format */
    int stream_compression;         /* compression format : always 1 for ADPCM */
    int stream_sample_rate;         /* sample rate  : not in spec*/ /* todo simon : check this */
    int stream_sample_size;         /* sample size  : always 1 for 16 bit */
    int stream_sample_stereo_mono;  /* stereo or mono sound 0 = mono, 1 = stereo */
    int n_stream_samples;           /* the number of samples */

};

/* Defines a loss-less bitmap character that contains raw ZLIB compressed image data. */
struct swf_definebitslossless {

    SWF_U32 tagid;          /* the bitmap id for this character */
    int format;             /* the format 3 = 8 bit data, 4 = 16 bit data, 5 = 32 bit data */
    int width;              /* width of the image */
    int height;             /* height of the image */
    int colourtable_size;   /* the size of the data */
    SWF_U8  * data;         /* the ZLib compressed data */

};

/* todo simon : doesn't appear to be in spec :( */
struct swf_mp3header {

    int ver;
    int layer;
    int pad;
    int stereo;
    int freq;
    int rate;
    SWF_U8 * data;
    int encoded_frame_size;
    int decoded_frame_size;
};

struct swf_mp3header_list {
        swf_mp3header ** headers;
        int header_count;
};

struct swf_textrecord {

    swf_textrecord * next;  /* pointer to the next text record in the linked list */
    SWF_U8 flags;           /* the flags for this text record */ /* todo simon : is this necessary */
    long font_id;           /* the id of the font we use */ /* todo simon : long? */
    SWF_U32 colour;         /* the colour of this text */ /* todo simon: break into swf_colour */
    int xoffset;            /* the xoffset of this text */
    int yoffset;            /* the yoffset of this text */
    int font_height;        /* the height of the text */
    int glyph_count;        /* the number of glyphs */
    int ** glyphs;          /* all the glyphs */
    /* not all these get set at the same time */


};


struct swf_textrecord_list {
        swf_textrecord * first;     /* pointer to the first element in the list */
        swf_textrecord ** lastp;    /* pointer to the last element in the list */
};

struct swf_buttonrecord {

    swf_buttonrecord * next;        /* the next element in the list */

    SWF_U32 state_hit_test;         /* hit test flag */
    SWF_U32 state_down;             /* state down flag */
    SWF_U32 state_over;             /* state over flag */
    SWF_U32 state_up;               /* state up flag */
    SWF_U32 character;              /* button character id */
    SWF_U32 layer;                  /* button character layer */


    swf_matrix * matrix;            /* button charcetr matrix */

    int ncharacters;                /* todo simon : */
    swf_cxform ** characters;       /* hmm, thse aren't in the spec */

};



struct swf_buttonrecord_list {
        swf_buttonrecord * first;   /* the first element in the list */
        swf_buttonrecord ** lastp;  /* pointer to the last element in the list */
};


/* Flags that the specified actions should be performed
 * when the frame is complete 
 * Can either be included in Definebutton2 or as its own tag 
 */
struct swf_doaction {

    swf_doaction * next;        /* next item in the list */ /* todo simon : the rest of these are action records in the spec. Change it ? */
    int   code;                 /* what type it is */
    int   frame;                /* goto this frame index (code = 0x81 or 0x8A)*/
    int   skip_count;           /* how many frames to skip if frame is not loaded (code = 0x8A) */
    char* url;                  /* the url to jump to (code = 0x83) */
    char* target;               /* the target window to open (code = 0x83) or target of action target (code = 0x8B) */
    char* goto_label;           /* the frame label to to (code = 0x8C) */

    /* none of the rest of these apear to be in the spec */
    SWF_U32   push_data_type;
    union
    {
        SWF_U32 dw;
        float f;
    } push_data_float;


    char* push_data_string;
    SWF_U16   branch_offset;
    SWF_U8    url2_flag;
    SWF_U8    stop_flag;



};

struct swf_doaction_list {
	swf_doaction * first;   /* the first element in the list */
	swf_doaction ** lastp;  /* pointer to the last element in the list */
};


struct swf_shaperecord {

    int is_edge;                 /* is this an edge? 1 = yes, 0 = no */
    SWF_U16 flags;               /* some flags */ /* todo simon - is this necessary? */
    SWF_S32 x;                   /* move to X value, relative to shape origin i.e draw line from last point to here */
    SWF_S32 y;                   /* move to Y value, relative to shape origin */
    int fillstyle0;              /* fill style 0 change flag */
    int fillstyle1;              /* fill style 1 change flag */
    int linestyle;               /* line style change flag */
    swf_shapestyle * shapestyle; /* the stle of the shape */
    SWF_S32 ax;                  /* the x anchor for a curve */
    SWF_S32 ay;                  /* the y anchor for a curve */
    SWF_S32 cx;                  /* the x end point for a curve */
    SWF_S32 cy;                  /* the y end point for a curve */

    swf_shaperecord * next;      /* next element in the list */

    /* todo simon : this is extremley b0rked at the moment I think */
};

struct swf_shaperecord_list {
    int record_count;

    swf_shaperecord * first;    /* todo simon : this needs to be fixed */
    swf_shaperecord ** lastp;
};

/*
 * all this does is define a condition
 * that must be satisfied and then some
 * DoActions
 */
struct swf_button2action {
	SWF_U32 condition;
    swf_doaction_list * doactions;
    swf_button2action * next;
};

struct swf_button2action_list {
	swf_button2action *  first;
	swf_button2action ** lastp;
};


#endif /*SWFTYPES*/

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
