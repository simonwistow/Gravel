#include "swf_types.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_serialise.h"
#include "swf_destroy.h"

#include <stdio.h>

#define SWF_PARSER_MAX_TAG_ID 48
#define FRAMERATE 15
#define SWF_BUFFER_SIZE 10240

void usage (char * name);

void 
usage (char * name) 
{
    fprintf (stderr, "Usage: %s <input filename> <output filename>\n", name);
}

void 
dummy(swf_parser * context, int * error)
{
  return;
}

swf_tagrecord *
extract_tagrecord(swf_parser * context, int * error, SWF_U16 next_id)
{
  swf_tagrecord * temp;

  temp = swf_make_tagrecord(error, next_id);
  temp->buffer->raw = swf_parse_get_bytes(context, context->cur_tag_len); 
  temp->buffer->size = context->cur_tag_len;
  temp->serialised = 1;

  return temp;
}

void * 
init_parser (void) {
    void (**parse)();
    void (**masked)();
    int i;
    
    parse  = calloc((1 + SWF_PARSER_MAX_TAG_ID), sizeof(void * ));
    masked = calloc((1 + SWF_PARSER_MAX_TAG_ID), sizeof(void * ));
    
    /* Set all the parse functions to dummy, then 
     * just add the ones we actually want...
     */
    for (i=0; i <= SWF_PARSER_MAX_TAG_ID ; i++) {
//      masked[i] = dummy;
      masked[i] = extract_tagrecord;
    }

    free(parse);

    return (void *) masked;
}

const char **
init_tags (void)
{

    const char ** tag  = (const char **) calloc (1 + SWF_PARSER_MAX_TAG_ID, sizeof (char *));

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


int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error = SWF_ENoError;
    swf_parser * parser;
    swf_header * hdr;
    swf_tagrecord * temp;
    SWF_U32 next_id;
    void* (**parse)();	
    const char ** tag;
    
    tag = (const char **) init_tags();

    if (3 != argc) {
      usage(argv[0]);
      exit(1);
    }

    /* First, get a parser up */
    parser = swf_parse_create(argv[1], &error);

    if (parser == NULL) {
	fprintf (stderr, "Failed to create SWF context\n");
	return -1;
    }
    printf ("Name of file is '%s'\n", parser->name);


    printf("----- Reading the file input header -----\n");
    hdr = swf_parse_header(parser, &error);

    if (hdr == NULL) {
        fprintf (stderr, "Failed to parse headers\n");
        exit(1);
    }

    swf_print_header(hdr, &error);
    printf("\n----- Reading movie details -----\n");

    parse = init_parser();

/* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

    /* FIXME: We're copying by reference here, which is a bit crufty... */
    movie->header = hdr;
    movie->name = (char *) argv[2];

    movie->header->rate = FRAMERATE * 256;

    do {
      next_id = swf_parse_nextid(parser, &error);
      if (error != SWF_ENoError) {
	fprintf (stderr, "ERROR: parsing id : '%s'\n",  swf_error_code_to_string(error));
      }
      
//      printf("foo: %s\n", tag[next_id]);

      error = SWF_ENoError;
      
      if (next_id <= SWF_PARSER_MAX_TAG_ID) {
	temp = (swf_tagrecord*) (parse[next_id](parser, error, next_id));
	swf_dump_shape(movie, &error, temp);
	printf("Error = %i\n", error);
      }

      //swf_destroy_tagrecord(temp);
	
      if (error != SWF_ENoError) {
	fprintf (stderr, "ERROR parsing tag : '%s'  : '%s'\n", tag[next_id], swf_error_code_to_string(error));
      }
    } while (next_id > 0);


    swf_make_finalise(movie, &error);

    printf("Error = %i\n", error);

    /* Kill block */
    free(parse);
    free(tag);

    swf_destroy_movie(movie);

//    swf_destroy_header(hdr);
    swf_destroy_parser(parser);

    return 0;
}




