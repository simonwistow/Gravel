swf_tagrecord * gravel_make_shape(swf_movie * movie, int * error, SCOORD xmin, SCOORD xmax, SCOORD ymin, SCOORD ymax);
void gravel_add_style_to_shape(swf_tagrecord* tag, int* error, SWF_U16 fill0, SWF_U16 fill1, SWF_U16 line, SWF_S32 x, SWF_S32 y);
void gravel_add_line_to_shape(swf_tagrecord* tag, int* error, SWF_S32 x, SWF_S32 y);
void gravel_end_shape(swf_tagrecord* tag, int* error);
swf_colour * gravel_parse_colour (char * s);
SWF_U8 gravel_parse_hex(char * s);
swf_shaperecord * gravel_make_line (int * error, SWF_S32 x1, SWF_S32 y1, SWF_S32 x2, SWF_S32 y2);

typedef struct gravel_cubic gravel_cubic;

/* This structure is absolute, not relative. So A and D are
 * endpoints and B, C are the midpoints
 */

struct gravel_cubic {
    SWF_S32 ax;
    SWF_S32 ay;

    SWF_S32 bx;
    SWF_S32 by;

    SWF_S32 cx;
    SWF_S32 cy;

    SWF_S32 dx;
    SWF_S32 dy;
};

/* Helper typedef for perl/XS */

typedef struct {
  swf_movie * movie;
} SWF_Movie;

