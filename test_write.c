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



void
parse_frame (swf_parser * context, const char * str)
{
    context->frame++;
    printf("%stagShowFrame\n", str);
    printf("\n<----- dumping frame %ld file offset 0x%04x ----->\n", context->frame, swf_parse_tell(context));
}

void
parse_end (swf_parser * context, const char * str)
{
	printf("%stagEnd\n", str);
}

void
usage (void)
{
    fprintf (stderr, "You fucked up\n");
}

void serialise(swf_movie * movie, int * error);

void serialise(swf_movie * movie, int * error)
{
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

    printf("----- Reading the file header -----\n");
    header = swf_parse_header(swf, &error);

    if (header == NULL) {
        fprintf (stderr, "Failed to parse headers\n");
        goto FAIL;
    }

    printf("FWS\n");
    printf("File version \t%lu\n", header->version);
    printf("File size \t%lu\n", header->size);
    printf("Movie width \t%lu\n", (header->bounds->xmax - header->bounds->xmin) / 20);
    printf("Movie height \t%lu\n", (header->bounds->ymax - header->bounds->ymin) / 20);
    printf("Frame rate \t%lu\n", header->rate);
    printf("Frame count \t%lu\n", header->count);

    printf("\n----- Reading movie details -----\n");

    printf("\n<----- dumping frame %d file offset 0x%04x ----->\n", 0, swf_parse_tell(swf));

	movie->header = header;
    movie->header->size = 20; /* Set illegal size for serialisastion check */ 

	/* Go get a defineshape */

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
	
//        *(list->lastp) = temp;
//	     list->lastp = &(temp->next);

/* print out stuff */
	printf("Bytes returned: %li\n", buffy->size);
	
/*	for(i=0; i<buffy->size; i++) {
		printf("%i th byte: %i\n", i, buffy->buffer[i]);
		} */

	/* buffy now has a serialised defineshape tag in it */
	


	swf_free(buffy->buffer);
	swf_free(buffy);
	

    printf("\n***** Finished Dumping SWF File Information *****\n");

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
