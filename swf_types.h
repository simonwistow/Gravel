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


#ifndef SWF_TYPES_H
#define SWF_TYPES_H

#include <stdio.h>

#define TRUE (1)
#define FALSE (0)

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

/* Global Types*/
typedef unsigned long U32, *P_U32, **PP_U32;
typedef signed long S32, *P_S32, **PP_S32;
typedef unsigned short U16, *P_U16, **PP_U16;
typedef signed short S16, *P_S16, **PP_S16;
typedef unsigned char U8, *P_U8, **PP_U8;
typedef signed char S8, *P_S8, **PP_S8;
typedef signed long SFIXED, *P_SFIXED;
typedef signed long SCOORD, *P_SCOORD;




typedef struct swf_parser swf_parser;
typedef struct swf_tag swf_tag;
typedef struct swf_rect swf_rect;
typedef struct swf_header swf_header;
typedef struct swf_colour swf_colour;
typedef struct swf_gradcolour swf_gradcolour;
typedef struct swf_cxform swf_cxform;
typedef struct swf_matrix swf_matrix;

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
typedef struct swf_defineshape2 swf_defineshape2;
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


struct swf_header {
    U32 size;
    U32 version;
    U32 rate;
    U32 count;
    swf_rect * bounds;
};


struct swf_parser {
    FILE * file;
    char * name;
    swf_header * header;
    
    int headers_parsed;
    
    U32 cur_tag_len;
    U32 filepos;
    U32 next_tag_pos;
    U32 frame;
    
    /* Bit Handling. */
    S32 bitpos;
    U32 bitbuf;
    
    /* Tag parsing information. */
    U32 tagstart;
    U32 tagzero;
    U32 tagend;
    
    U32 taglen;
    
    int fill_bits;
    int line_bits;

    	/* Font glyph counts (gotta save it somewhere!) */
    int glyph_counts [256];

    U8* src_adpcm;
    U32 bitbuf_adpcm;       /* this should always contain at least 24 bits of data */
    S32 bitpos_adpcm;
    U32 n_samples_adpcm;    /* number of samples decompressed so far */

    /* Streaming sound info from SoundStreamHead tag */
    int stream_compression;
    int stream_sample_rate;
    int stream_sample_size;
    int stream_sample_stereo_mono;
    int n_stream_samples;
    
};

struct swf_tag {
    U16 id;
    U32 len;
};

struct swf_rect {
    SCOORD xmin;
    SCOORD xmax;
    SCOORD ymin;
    SCOORD ymax;
};

struct swf_colour {
    U32 r;
    U32 g;
    U32 b;
    U32 a;
};

struct swf_cxform {
    S32 flags;
    S16 aa;    /* a is multiply factor ... */
    S16 ab;    /* ... b is addition factor */
    S16 ra;
    S16 rb;
    S16 ga;
    S16 gb;
    S16 ba;
    S16 bb;

};

struct swf_matrix {
    SFIXED a;
    SFIXED b;
    SFIXED c;
    SFIXED d;
    SCOORD tx;
    SCOORD ty;
};

struct swf_setbackgroundcolour {
    swf_colour * colour;
};


struct swf_definefont {

	U32 fontid;
	int offset;
	int glyph_count;
    swf_shaperecord_list ** shape_records;

};


struct swf_definefontinfo {

	U32 fontid;
	U8 flags;
	int namelen;
	int * code_table;
	char * fontname;

};

struct swf_placeobject {

    U32 tagid;
    U32 depth;
    swf_matrix * matrix;
    swf_cxform * cxform;

};

struct swf_placeobject2 {

	U8 flags;
	U32 depth;
	U32 tag;
	swf_matrix * matrix;
	swf_cxform * cxform;
	U32 ratio;
	U32 clip_depth;
	char * name;


};

struct swf_defineshape {

    U32 tagid;
    swf_rect * rect;
    swf_shapestyle * style;
    swf_shaperecord_list * record;

};

struct swf_shapestyle {

    U16 nfills;
    swf_fillstyle ** fills;

    U16 nlines;
    swf_linestyle ** lines;
};

struct swf_fillstyle {

    U16 fill_style;
    U16 ncolours;
    U16 bitmap_id;
    swf_matrix * matrix;
    swf_rgba_pos ** colours;
    U32 colour;

};

struct swf_linestyle {

    U16 width;
    U32 colour;

};

struct swf_rgba_pos {

    U32 rgba;
    U8 pos;

};


struct swf_freecharacter {

    U32 tagid;
};


struct swf_removeobject {

    U32 tagid;
    U32 depth;
};

struct swf_removeobject2 {

    U32 depth;
};

struct swf_startsound {

    U32 tagid;
    U32 code;
    U32 inpoint;
    U32 outpoint;
    U32 loops;
    U8  npoints;
    swf_soundpoint ** points;

};

struct swf_soundpoint {

    U32 mark;
    U32 lc;
    U32 rc;

};

struct swf_definebits {

    U32 tagid;
    swf_imageguts * guts;

};

struct swf_definebitsjpeg2 {

    U32 tagid;
    swf_imageguts * guts;

};

struct swf_definebitsjpeg3 {

    U32 tagid;
    swf_imageguts * guts;

};

struct swf_imageguts {

    U32 nbytes;
    U8  * data;
};

struct swf_jpegtables {

    swf_imageguts * guts;

};

struct swf_definetext {

    U32 tagid;
    swf_rect    * rect;
    swf_matrix  * matrix;
    swf_textrecord_list* records;
};


struct swf_definetext2 {

    U32 tagid;
    swf_rect    * rect;
    swf_matrix  * matrix;
    swf_textrecord_list  * records;
};


struct swf_definebutton {

    U32 tagid;
    swf_buttonrecord_list * records;
    swf_doaction_list     * actions;

};


struct swf_definebutton2 {

    U32 tagid;
    swf_buttonrecord_list * records;
    //swf_doaction_list     * actions;
    swf_button2action_list * actions;
};



struct swf_defineedittext {

	U32 tagid;
	swf_rect * bounds;
	U16 flags;
	U16 font_id;
	U16 font_height;
	U32 colour;
	int max_length;
	int align;
	U16 left_margin;
	U16 right_margin;
	U16 indent;
	U16 leading;
	char * variable;
	char * initial_text;

};

struct swf_definefont2 {

	U32 tagid;
	U16 flags;
	int name_len;
	char * name;
	U16 nglyphs;
	swf_shaperecord_list ** glyphs;
	U32 * code_table;
	S16 ascent;
	S16 descent;
	S16 leading;
	swf_rect ** bounds;
	S16 nkerning_pairs;
	swf_kerningpair ** kerning_pairs;

};

struct swf_kerningpair {

	U16 code1;
	U16 code2;
	S16 adjust;
};



struct swf_definemorphshape  {
    U32 tagid;
    swf_rect * r1;
    swf_rect * r2;
    int nfills;
    swf_fillstyle2 ** fills;
    int nlines;
    swf_linestyle2 ** lines;
    swf_shaperecord_list * records1;
    swf_shaperecord_list * records2;
};

struct swf_gradcolour {

    U8 r1;
    U8 r2;
    U32 c1;
    U32 c2;
};

struct swf_fillstyle2 {
    int style;
    swf_matrix * matrix1;
    swf_matrix * matrix2;
    int ncolours;
    swf_gradcolour ** colours;
    U16 tag;
    U32 rgb1;
    U32 rgb2;

};

struct swf_linestyle2 {
    U16 thick1;
    U16 thick2;
    U32 rgb1;
    U32 rgb2;

};



struct swf_definesound {
    U32 tagid;
    int compression;
    int sample_rate;
    int sample_size;
    int stereo_mono;
    int sample_count;
    int delay;
    swf_adpcm * adpcm;
    swf_mp3header_list * mp3header_list;

};

struct  swf_adpcm {
    U32 tagid;

};

struct swf_framelabel {

    char * label;

};


struct swf_namecharacter {

    U32 tagid;
    char * label;

};

struct swf_definebuttoncxform {

    U32 tagid;
    int ncxforms;
    swf_cxform ** cxforms;
};


struct swf_soundstreamblock {

    U32 tagid;
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
    U32 tagid;
    swf_startsound * up_state;
    swf_startsound * over_state;
    swf_startsound * down_state;
};

struct swf_soundstreamhead {

    U32 tagid;
    int mix_format;
    int stream_compression;
    int stream_sample_rate;
    int stream_sample_size;
    int stream_sample_stereo_mono;
    int n_stream_samples;

};

struct swf_definebitslossless {

    U32 tagid;
    int format;
    int width;
    int height;
    int colourtable_size;
    U8  * data;

};

struct swf_mp3header {

    int ver;
    int layer;
    int pad;
    int stereo;
    int freq;
    int rate;
    U8 * data;
    int encoded_frame_size;
    int decoded_frame_size;
};

struct swf_mp3header_list {
        swf_mp3header ** headers;
        int header_count;
};

struct swf_textrecord {

    U8 flags;
    long font_id;
    U32 colour;
    int xoffset;
    int yoffset;
    int font_height;
    int glyph_count;
    int ** glyphs;


};


struct swf_textrecord_list {
        swf_textrecord ** records;
        int record_count;
};

struct swf_buttonrecord {


    U32 state_hit_test;
    U32 state_down;
    U32 state_over;
    U32 state_up;
    U32 character;
    U32 layer;


    swf_matrix * matrix;
    int ncharacters;
    swf_cxform ** characters;
};



struct swf_buttonrecord_list {
        swf_buttonrecord ** records;
        int record_count;
};


struct swf_doaction {

    U32 tagid;

};

struct swf_doaction_list {
        swf_doaction ** actions;
        int action_count;
};


struct swf_shaperecord {

    int is_edge;
    U16 flags;
    S32 x;
    S32 y;
    int fillstyle0;
    int fillstyle1;
    int linestyle;
    swf_shapestyle * shapestyle;
    S32 ax;
    S32 ay;
    S32 cx;
    S32 cy;

};

struct swf_shaperecord_list {
        swf_shaperecord ** records;
        int record_count;
};

struct swf_button2action {

	U32 tagid;
};

struct swf_button2action_list {
        swf_button2action ** actions;
        int action_count;
};


#endif /*SWFTYPES*/



