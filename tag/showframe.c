#include "tag_handy.h"

void 
swf_add_showframe(swf_movie * movie, int * error) 
{
    swf_tagrecord * temp;
    temp = swf_make_tagrecord(error);

    if (*error) {
	return;
    }

    temp->next = NULL;
    temp->id = 1;
    temp->tag = NULL;
    temp->serialised = 1;

    *(movie->lastp) = temp;
    movie->lastp = &(temp->next);

    return;
}

