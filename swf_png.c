/* This is a test of some PNG capabilities, using a lot of code from
 * the util/png2dbl.c file of Ming. 
 */

#include <png.h>
#include <zlib.h>

#include "swf_png.h"
#include "swf_error.h"

#define SWF_PNG_SIG_SIZE 8

void 
swf_png_get_IHDR(swf_png_data * png, int * error, png_structp png_ptr, png_infop info_ptr)
{
	png_uint_32 width=0;
	png_uint_32 height=0;
	int bits=0;
	int colours=0;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bits, &colours, NULL, NULL, NULL);

	png->width  = width;
	png->height = height;
	png->bit_depth = bits;
	png->color_type = colours;
}

void 
swf_png_get_PLTE(swf_png_data * png, int * error, png_structp png_ptr, png_infop info_ptr)
{
	int num_palette=0;
	png_colorp palette;

	palette = png->palette;
	png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

	png->num_palette = num_palette;
	png->palette = palette;
}

swf_png_data *
swf_png_make_swf_png(int * error)
{
	swf_png_data * self;

	if ((self = (swf_png_data *) calloc (1, sizeof (swf_png_data))) == NULL) {
		*error = SWF_EMallocFailure;
		return NULL;
	}

	return self;
}


/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
