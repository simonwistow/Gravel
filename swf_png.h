#ifndef SWF_PNG_H
#define SWF_PNG_H

#include <png.h>

typedef struct swf_png_data swf_png_data;

struct swf_png_data {
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	SWF_U8 num_palette;
	int channels;
	png_colorp palette;
	SWF_U8 *data;
};

swf_png_data * swf_png_make_swf_png(int * error);

void swf_png_get_IHDR(swf_png_data * png, int * error, png_structp png_ptr, png_infop info_ptr);

void swf_png_get_PLTE(swf_png_data * png, int * error, png_structp png_ptr, png_infop info_ptr);

swf_tagrecord * swf_read_png(const char * filename, int * error);
swf_buffer * swf_write_dbl(int * error, swf_png_data * png, SWF_U16 bitmap_id);
void writeDBL(FILE *f, swf_png_data * png, int * error);
swf_shaperecord_list * swf_make_shaperecords_for_box(int * error);
swf_tagrecord * swf_make_box(swf_movie * movie, int * error);

#endif 

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
