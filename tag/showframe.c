#include "tag_handy.h"

void 
swf_add_showframe(swf_movie * movie, int * error) 
{
    swf_tagrecord * temp;
    temp = swf_make_tagrecord(error, tagShowFrame);

    if (*error) {
	return;
    }

    temp->serialised = 1;

    swf_dump_shape(movie, error, temp);

    return;
}

