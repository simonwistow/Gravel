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

#endif 

/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
