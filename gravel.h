swf_tagrecord * gravel_create_shape(swf_movie * movie, int * error, SCOORD xmin, SCOORD xmax, SCOORD ymin, SCOORD ymax);
void gravel_add_style_to_shape(swf_tagrecord* tag, int* error, SWF_U16 fill0, SWF_U16 fill1, SWF_U16 line, SWF_S32 x, SWF_S32 y);
void gravel_add_line_to_shape(swf_tagrecord* tag, int* error, SWF_S32 x, SWF_S32 y);
void gravel_end_shape(swf_tagrecord* tag, int* error);
swf_colour * gravel_parse_colour (char * s);
SWF_U8 gravel_parse_hex(char * s);
