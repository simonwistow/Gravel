extern void swf_serialise_rect(swf_movie * p, int * error, swf_rect * rect);
extern void swf_add_end(swf_movie * movie, int * error);
extern void swf_add_showframe(swf_movie * movie, int * error);
extern void swf_add_setbackgroundcolour(swf_movie * movie, int * error, SWF_U8 red, SWF_U8 green, SWF_U8 blue, SWF_U8 alpha);
extern void swf_add_protect(swf_movie * movie, int * error);
extern void swf_serialise_matrix (swf_movie * movie, int * error, swf_matrix * mym);
