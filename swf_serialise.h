/* Add-to-movie API */

extern void swf_add_end(swf_movie * movie, int * error);
extern void swf_add_showframe(swf_movie * movie, int * error);
extern void swf_add_setbackgroundcolour(swf_movie * movie, int * error, SWF_U8 red, SWF_U8 green, SWF_U8 blue, SWF_U8 alpha);
extern void swf_add_protect(swf_movie * movie, int * error);
extern void swf_add_matrix (swf_movie * movie, int * error, swf_matrix * mym);
extern void swf_add_placeobject (swf_movie * movie, int * error, SWF_U16 char_id, swf_matrix * mym);
extern void swf_add_removeobject(swf_movie * movie, int * error, SWF_U16 char_id);
extern void swf_add_placeobject2 (swf_movie * movie, int * error, SWF_U16 char_id, swf_matrix * mym, swf_cxform * mycx, char * myname);

/* Serialisation API */

extern void swf_serialise_rect(swf_movie * p, int * error, swf_rect * rect);
extern void swf_serialise_matrix (swf_buffer * buffer, int * error, swf_matrix * mym);
extern void swf_serialise_cxform(swf_buffer * buffer, int * error, swf_cxform * mycx);
