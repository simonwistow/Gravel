/*
 * Copyright (C) 2001  Ben Evans <kitty@fuckedgoths.com>
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
 */

#include "parser.h"
#include "swf_parse.h"
#include "swf_types.h"
#include "swf_destroy.h"
#include "print_utils.h"
#include "swf_movie.h"


const char **
init_tags (void)
{

    const char ** tag  = (const char **) calloc (49, sizeof (char *));

    tag[0] = "End";
    tag[1] = "ShowFrame";
    tag[2] = "DefineShape";
    tag[3] = "FreeCharacter";
    tag[4] = "PlaceObject";
    tag[5] = "RemoveObject";
    tag[6] = "DefineBits";
    tag[7] = "DefineButton";
    tag[8] = "JPEGTables";
    tag[9] = "SetBackgroundColor";
    tag[10] = "DefineFont";
    tag[11] = "DefineText";
    tag[12] = "DoAction";
    tag[13] = "DefineFontInfo";
    tag[14] = "DefineSound";        /* Event sound tags. */
    tag[15] = "StartSound";
    tag[17] = "DefineButtonSound";
    tag[18] = "SoundStreamHead";
    tag[19] = "SoundStreamBlock";
    tag[20] = "DefineBitsLossless"; /* A bitmap using lossless zlib compression. */
    tag[21] = "DefineBitsJPEG2";    /* A bitmap using an internal JPEG compression table. */
    tag[22] = "DefineShape2";
    tag[23] = "DefineButtonCxform";
    tag[24] = "Protect";            /* This file should not be importable for editing. */

    /*  These are the new tags for Flash 3. */
    tag[26] = "PlaceObject2";        /* The new style place w/ alpha color transform and name. */
    tag[28] = "RemoveObject2";       /* A more compact remove object that omits the character tag (just depth). */
    tag[32] = "DefineShape3";        /* A shape V3 includes alpha values. */
    tag[33] = "DefineText2";         /* A text V2 includes alpha values. */
    tag[34] = "DefineButton2";       /* A button V2 includes color transform, alpha and multiple actions */
    tag[35] = "DefineBitsJPEG3";     /* A JPEG bitmap with alpha info. */
    tag[36] = "DefineBitsLossless2"; /* A lossless bitmap with alpha info. */
    tag[37] = "DefineEditText";      /* An editable Text Field */
    tag[39] = "DefineSprite";        /* Define a sequence of tags that describe the behavior of a sprite. */
    tag[40] = "NameCharacter";       /* Name a character definition, character id and a string, (used for buttons, bitmaps, sprites and sounds). */
    tag[43] = "FrameLabel";          /* A string label for the current frame. */
    tag[45] = "SoundStreamHead2";    /* For lossless streaming sound, should not have needed this... */
    tag[46] = "DefineMorphShape";    /* A morph shape definition */
    tag[48] = "DefineFont2";

    return tag;
}

void usage (void);

void usage (void)
{
    fprintf (stderr, "You fucked up\n");
}

void serialise(swf_movie * movie, int * error);

void serialise(swf_movie * movie, int * error)
{
	SWF_U32 movie_length;
	SWF_U8 * stream;
	SWF_U8 * r;
	char * ch;
	swf_tagrecord * temp;
	SWF_U8 bitsize, pad;
	SWF_U16 big_pad, xmin, ymin, xmax, ymax;
    FILE * myfile;

	fprintf(stderr, "Ready to serialise\n");


	if ((r = (SWF_U8 *) calloc (10, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	if ((ch = (char *) calloc (4, sizeof (char))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }


	myfile = fopen("kitty_out.swf", "wb+");

	/* Did we open the file? */
	if (myfile == NULL) {
		*error = SWF_EFileOpenFailure;
		return NULL;
	}

	ch = "FWS5";

	if( fwrite(ch, 1, 4, myfile) != 4){
		*error = SWF_ETooManyFriends;
		return;
    }

	/* calculate movie length */
	movie_length = 0;

/* Just do the case where everything is pre-serialised */

	temp = movie->first;

	do {
		if (temp->serialised != 1) {
			*error = SWF_EFileOpenFailure; /* Use this error code for now */
			return;
		}

		movie_length += temp->size;
		temp = temp->next;

	} while (NULL != temp);

	fprintf(stderr, "Body length = %li\n", movie_length);

	/* Now, serialise the movie length, using r */

	movie_length += 10; /* for the r array */
	movie_length += 12; /* for the fixed header overhead */

	r[0] = movie_length >> 24; 
	r[1] = (movie_length << 8) >> 24; 
	r[2] = (movie_length << 16) >> 24; 
	r[3] = (movie_length << 24) >> 24; 

	if( fwrite(r, 1, 4, myfile) != 4){
		*error = SWF_ETooManyFriends;
		return;
    }


	/* Now, get the header set */

	bitsize = 0x0f; /* 15 bits wide for up to 1024 x 1024 movies */

	xmin = movie->header->bounds->xmin;
	ymin = movie->header->bounds->ymin;
	xmax = movie->header->bounds->xmax;
	ymax = movie->header->bounds->ymax;

	big_pad = (bitsize << 11) | (xmin >> 4); 
/* shift by 4 instead of 5 because we have a spurious bit induced by using a U16 to represent a 15-bit wide quantity */

	r[1] = big_pad & 0xff;
	r[0] = (big_pad >> 8) & 0xff;

	big_pad = (xmin << 12) | (xmax >> 3); /* This is the last four bits of xmin now bitshifted correctly */

	r[3] = big_pad & 0xff;
	r[2] = (big_pad >> 8) & 0xff;

	big_pad = (xmax << 13) | (ymin >> 2); 

	r[5] = big_pad & 0xff;
	r[4] = (big_pad >> 8) & 0xff;

	big_pad = (ymin << 14) | (ymax >> 1); 

	r[7] = big_pad & 0xff;
	r[6] = (big_pad >> 8) & 0xff;

	big_pad = (ymax << 15);

/* should this be word or byte aligned ? */

	r[9] = big_pad & 0xff;
	r[8] = (big_pad >> 8) & 0xff;

	if( fwrite(r, 1, 10, myfile) != 10){
		*error = SWF_ETooManyFriends;
		return;
    }

	r[0] = movie->header->rate;
	r[1] = 0;

	r[2] = (movie->header->count >> 8);
	r[3] = movie->header->count & 0xff;

	if( fwrite(r, 1, 4, myfile) != 4){
		*error = SWF_ETooManyFriends;
		return;
    }
	   

	fclose(myfile);

	free(ch);

	swf_free(r);
	return;
}

void add_serialised_placeobject(swf_movie * movie, int * error, SWF_U8 x, SWF_U8 y, SWF_U16 obj_id);

void add_serialised_placeobject(swf_movie * movie, int * error, SWF_U8 x, SWF_U8 y, SWF_U16 obj_id)
{
	swf_tagrecord * po;
	SWF_U16 depth = 1; /* Hardcoded depth */
	SWF_U8 * tfmn;
	SWF_U8 b0, b1, length, d0, d1, o1, o0;
	SWF_U16 pad;

	if ((po = (swf_tagrecord *) calloc (1, sizeof (swf_tagrecord))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	if ((po->buffer = (SWF_U8 *) calloc (9, sizeof (SWF_U8))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }


	po->size = 9;
	po->serialised = 1;
	po->id = tagPlaceObject;


/* As we know the matrix is of size 3, we can put the taglength in now, otherwise, we'd have to calculate it at the end */
	length = 9;



	pad = po->id << 6;

	pad &= length;

	b1 = pad & 0xff;
    b0 = (pad >> 8) & 0xff; 

	o1 = obj_id & 0xff;
    o0 = (obj_id >> 8) & 0xff; 

	d1 = depth & 0xff;
    d0 = (depth >> 8) & 0xff; 

	tfmn = swf_movie_make_streamed_translation_matrix(movie, error, x, y);

	po->buffer[0] = b0;
	po->buffer[1] = b1;
	po->buffer[2] = o0;
	po->buffer[3] = o1;
	po->buffer[4] = d0;
	po->buffer[5] = d1;
	po->buffer[6] = tfmn[0];
	po->buffer[7] = tfmn[1];
	po->buffer[8] = tfmn[2];

	swf_free(tfmn);

	movie->first->next = po;

	movie->lastp = &(po->next);	
	
	return;
}

/*
 * The main function
 */

int
main (int argc, char *argv[])
{
    swf_parser * swf;
    swf_header * header;
	int shape_num;
    int error = SWF_ENoError;
	swf_tagrecord * buffy;

	swf_movie * movie;

    const char ** tag;

    tag = (const char **) init_tags();


    /* Check the argument count. */
    if (argc < 3) {
        /* Bad arguments. */
        usage();
        return -1;
    }

	fprintf(stderr, "%u\n\n", sizeof(SWF_U8));

    swf = swf_parse_create(argv[1], &error);
	shape_num = atoi(argv[2]);

    if (swf == NULL) {
		fprintf (stderr, "Failed to create SWF context\n");
		return -1;
    }
    printf ("Name of file is '%s'\n", swf->name);

	/* start with a 10K buffer */
	if ((movie = (swf_movie *) calloc (1, sizeof (swf_movie))) == NULL) {
		error = SWF_EMallocFailure;
		goto FAIL;
    }

	movie->first = NULL;
	movie->lastp = &movie->first;

    fprintf(stderr, "----- Stealing the file header -----\n");
    header = swf_parse_header(swf, &error);

    if (header == NULL) {
        fprintf (stderr, "Failed to parse headers\n");
        goto FAIL;
    }

/*
    fprintf(stderr, "FWS\n");
    fprintf(stderr, "File version \t%lu\n", header->version);
    fprintf(stderr, "File size \t%lu\n", header->size);
    fprintf(stderr, "Movie width \t%lu\n", (header->bounds->xmax - header->bounds->xmin) / 20);
    fprintf(stderr, "Movie height \t%lu\n", (header->bounds->ymax - header->bounds->ymin) / 20);
    fprintf(stderr, "Frame rate \t%lu\n", header->rate);
    fprintf(stderr, "Frame count \t%lu\n", header->count);

    fprintf(stderr, "\n----- Reading movie details -----\n");

    fprintf(stderr, "\n<----- dumping frame %d file offset 0x%04x ----->\n", 0, swf_parse_tell(swf));
*/


	movie->header = header;
    movie->header->size = 20; /* Set illegal size for serialisastion check */ 


	/* Make a body */

	/* Steal a defineshape */

    fprintf(stderr, "----- Stealing the shape -----\n");

	/* start with a 10K buffer */
	if ((buffy = (swf_tagrecord *) calloc (1, sizeof (swf_tagrecord))) == NULL) {
		error = SWF_EMallocFailure;
		goto FAIL;
    }

	buffy->size = 10240;

	if ((buffy->buffer = (SWF_U8 *) calloc (buffy->size, sizeof(SWF_U8))) == NULL) {
		error = SWF_EMallocFailure;
		goto FAIL;
    }	

	swf_get_nth_shape(swf, &error, shape_num, buffy);


	movie->first = buffy;

	fprintf(stderr, "Bytes returned: %li\n", movie->first->size);
	
	/* buffy now has a serialised defineshape tag in it */
	
	/* Now we need a placeobject */
	add_serialised_placeobject(movie, &error, 10, 20, buffy->code);

	fprintf(stderr, "Shape size check: %li\n", movie->first->size);

	/* End section */

	add_serialised_showframe(movie, &error);
	add_serialised_end(movie, &error);

	/* Now stream it out */

	serialise(movie, &error);

	swf_free(buffy->buffer);
	swf_free(buffy);
	
    fprintf(stderr, "\n***** Finished Dumping New SWF File *****\n");

    free (tag);
    swf_destroy_header(header);

    swf_destroy_parser(swf);



    return 0;

 FAIL:
    swf_destroy_parser(swf);
    return -1;
}



/* 
Local Variables:
mode: C
c-basic-offset: 4
tab-width: 4
End:
*/
