/*
 * Copyright (C) 2002  Ben Evans <kitty@cpan.org>
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
 * 	$Id: tagrecord.c,v 1.1 2002/05/29 16:40:47 kitty_goth Exp $	
 */

#include <stdlib.h>
#include <stdio.h>

#include "swf_types.h"
#include "swf_error.h"
#include "swf_destroy.h"


swf_tagrecord * 
swf_make_tagrecord (int * error, SWF_U16 myid) 
{
    swf_tagrecord * tag;

    if ((tag = (swf_tagrecord *) calloc (1, sizeof (swf_tagrecord))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }

    if ((tag->buffer = (swf_buffer *) calloc (1, sizeof (swf_buffer))) == NULL) {
	*error = SWF_EMallocFailure;
	return NULL;
    }
    tag->buffer->raw  = NULL;
    tag->buffer->size = 0;

    tag->next = NULL;
    tag->id = myid;
    tag->tag = NULL;
    tag->serialised = 0;

    return tag;
}



/* destroy a tag record */
void
swf_destroy_tagrecord (swf_tagrecord * tag)
{
	if (tag == NULL)
	{
	  return;
	}
	tag->next = NULL;

	if (tag->buffer != NULL) {
	  swf_free (tag->buffer->raw);
	}
	swf_free (tag->buffer);
	/* We don't want to free the next tag. It's probably still valid */
//	swf_free (tag->next);
	swf_free (tag->tag);
	swf_free (tag);
}


