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
 * 	$Id: tagrecord.c,v 1.3 2002/06/07 17:18:01 kitty_goth Exp $	
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
  if (tag == NULL) {
    return;
  }
  tag->next = NULL;

  /* We don't want to free the next tag. It's probably still valid */
  /* Neither do we want to zap tag->tag - that should be taken care
     of by specialised cleanup, that knows what type it is */
  
  swf_destroy_buffer(tag->buffer);
  swf_free (tag);
}


