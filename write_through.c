#include "swf_types.h"
#include "swf_tags.h"
#include "swf_parse.h"
#include "swf_movie.h"
#include "swf_serialise.h"
#include "swf_destroy.h"
#include "print_utils.h"


#include <stdio.h>

#define SWF_PARSER_MAX_TAG_ID 48
#define FRAMERATE 15
#define SWF_BUFFER_SIZE 10240

void usage            (char * name);
void *                init_parser (void);
swf_tagrecord * dummy (swf_parser * context, int * error, SWF_U16 next_id);
swf_tagrecord * extract_tagrecord(swf_parser * context, int * error, SWF_U16 next_id);



int long_tags = 0;
int total_size = 21;
int tags  = 0;
int frames = 1;
int longtag = 0;

void 
usage (char * name) 
{
    fprintf (stderr, "Usage: %s <input filename> <output filename>\n", name);
}

swf_tagrecord *
dummy(swf_parser * context, int * error, SWF_U16 next_id)
{
  return NULL;
}

swf_tagrecord *
extract_tagrecord(swf_parser * context, int * error, SWF_U16 next_id)
{
  swf_tagrecord * temp;

  temp = swf_make_tagrecord(error, next_id);
  temp->buffer->raw = swf_parse_get_bytes(context, context->cur_tag_len); 
  temp->buffer->size = context->cur_tag_len;
  total_size += context->cur_tag_len + 2;
  longtag = 0;
  if (context->cur_tag_len >= 0x3f)
  {
	longtag = 1;
	long_tags++;
	total_size += 4;
  }

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
      masked[i] = (void *) extract_tagrecord;
    }

    free(parse);

    return (void *) masked;
}


int main (int argc, char *argv[]) {
    swf_movie * movie;
    int error = SWF_ENoError;
    swf_parser * parser;
    swf_header * hdr;
    swf_tagrecord * temp;
    SWF_U32 next_id = 0;
    void* (**parse)();	

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
    printf("\n----- Output movie details -----\n");

    parse = init_parser();

  /* Now generate the output movie */

    if ((movie = swf_make_movie(&error)) == NULL) {
	fprintf (stderr, "Fail\n");
	return 1;
    }

    /* FIXME: We're copying by reference here, which is a bit crufty... */
    movie->header = hdr;
    movie->name = (char *) argv[2];
    /* FIXME h-h-hack. This should be done in serialisation */ 
    movie->header->rate = movie->header->rate * 256;

    do {
      tags++;
      next_id = swf_parse_nextid(parser, &error);
      if (error != SWF_ENoError) {
	fprintf (stderr, "ERROR: parsing id : '%s'\n",  swf_error_code_to_string(error));
      }
    
      printf ("[%d] (%d) %s %s\n", frames, swf_parse_tell(parser), swf_tag_to_string(next_id), (1==longtag)?"*":"");
      if (1 == next_id) {
      	frames++;
	//	fprintf (stderr, "--------------------[ %d ]-------------------\n", frames++);
      }
      
      error = SWF_ENoError;
      
      if (next_id <= SWF_PARSER_MAX_TAG_ID) {
		temp = (swf_tagrecord*) (parse[next_id](parser, error, next_id));
		if (temp) {
		  swf_dump_tag(movie, &error, temp);
		}
      }

      if (error != SWF_ENoError) {
	fprintf (stderr, "ERROR parsing tag : '%s'  : '%s'\n", swf_tag_to_string(next_id), swf_error_code_to_string(error));
      }
    } while (next_id > 0);


    printf("Total movie size is %d\n", total_size);
    printf("The number of long tags was %d\n", long_tags);


    swf_make_finalise(movie, &error);

    /* Kill block */
    free(parse);

    swf_destroy_movie(movie);
    swf_destroy_parser(parser);

    return 0;
}




