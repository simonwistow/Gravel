package SWF::Parser;

use strict;

require Exporter;
use vars qw(@ISA %EXPORT_TAGS @EXPORT_OK @EXPORT $VERSION);

@ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use SWF::ExtractText ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
%EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

@EXPORT = qw(
	
);
$VERSION = '0.10';

use Cwd qw(cwd abs_path);
use Inline C => 'DATA',
  VERSION => '0.10',
  NAME => 'SWF::Parser',
  LIBS => '-L' . abs_path(cwd . '/..') . ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/..'),
  OPTIMIZE => '-g';
use Inline C => Config => STRUCTS => 'swf_header';

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;
  my $self = {};
  my %conf = @_;

  $self->{HANDLER} = $conf{handler};
#  warn "HANDLER: $conf{handler}\n";
  bless($self, $class);
  return $self;
}

sub start {
#  warn "start: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->start_element({Name => $arg});
}

sub end {
#  warn "end: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->end_element({Name => $arg});
}

sub chars {
#  warn "chars: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->characters({Data => $arg});
}

sub parsefile {
  my $self = shift;
  my $filename = shift;
  my $handler = $self->{HANDLER};

#  warn "Parsing file!\n";

  my $o = SWF::Parser->create($filename);
  $handler->start_document({});
  $self->start("swf");

  $self->start("header");
  $o->do_header($self);
  $self->end("header");

  $self->start("body");
  $o->do_body($self);
  $self->end("body");

  $self->end("swf");
  my $doc = $handler->end_document({});
  $o->destroy;
  return $doc;
}


use Inline Config =>
  FORCE_BUILD => 1,
  CLEAN_AFTER_BUILD => 0;

# Preloaded methods go here.

=head1 NAME

SWF::Parser - Parse SWF files and generate SAX events

=head1 SYNOPSIS

  use SWF::Parser;
  use XML::Handler::YAWriter;

  my $ya = XML::Handler::YAWriter->new(
    AsFile => "-",
    'Pretty' => {
      PrettyWhiteIndent => 1,
      PrettyWhiteNewline => 1,
    });

  my $parser = SWF::Parser->new(handler => $ya);
  $parser->parsefile(shift || die("No file!\n"));

  # Prints out something like:
  # <?xml version="1.0" encoding="UTF-8"?>
  # <swf>
  #  <header>
  #    <filename>../swfs/test5.swf
  #    </filename>
  #    <size>829
  #    </size>
  #    <version>3
  #    </version>
  #    <rate>12
  #    </rate>
  #    <count>1
  #    </count>
  # ...
  #  </header>
  #  <body>
  # ...
  #  </body>
  # </swf>

=head1 DESCRIPTION

There are currently no real docs. Note that the internal format of the
SWF parser is in a state of flux, and so is the XML structure output
here. Be wary.

=head1 AUTHOR

Leon Brocard E<lt>F<acme@astray.com>E<gt>

=head1 COPYRIGHT

Copyright (C) 2001, Leon Brocard

This module is free software; you can redistribute it or modify it
under the same terms as Perl itself.

=cut

1;

__DATA__

__C__
#include "swf_types.h"
#include "swf_parse.h"
#include "swf_destroy.h"

#define swf_free(x)       if( (x) ) free( (x) )
#define swf_realloc(x, y) ( (x) ? realloc( (x), (y) ) : calloc( 1, (y) ) )

typedef struct {
  swf_parser* parser;
} SWF_Parser;


SV* create(char* class, char* filename) {
  SV* obj_ref = newSViv(0);
  SV*     obj = newSVrv(obj_ref, class);
  SWF_Parser *s = malloc(sizeof(SWF_Parser));
  int error = SWF_ENoError;
  s->parser = swf_parse_create(filename, &error);

  if (error != SWF_ENoError) {
    die("Error creating SWF parser (correct file name?)\n");
  }

  sv_setiv(obj, (IV)s);
  SvREADONLY_on(obj);
  return obj_ref;
}


void do_header(SV* obj, SV* self) {
  SWF_Parser* s = (SWF_Parser*)SvIV(SvRV(obj));

  int error = SWF_ENoError;
  swf_header* header = swf_parse_header(s->parser, &error);

  my_call_simple(self, "filename", newSVpvf(s->parser->name));
  my_call_simple(self, "size", newSViv(header->size));
  my_call_simple(self, "version", newSViv(header->version));
  my_call_simple(self, "rate", newSViv(header->rate));
  my_call_simple(self, "count", newSViv(header->count));

  my_call_method(self, "start", newSVpvf("bounds"));
  do_rect(header->bounds, self);
  my_call_method(self, "end", newSVpvf("bounds"));

  swf_destroy_header(header);
}


void do_body(SV* obj, SV* self) {
  SWF_Parser* s;
  const char** tag;
  SWF_U32 next_id;
  int error;
  const char * str = "";

  s = (SWF_Parser*)SvIV(SvRV(obj));
  error = SWF_ENoError;
  tag = (const char **) init_tags();


  /* parse all the tags */
  do
  {

    next_id = swf_parse_nextid(s->parser, &error);
    if (error != SWF_ENoError)
      {
	fprintf (stderr, "ERROR: There was an error parsing the next id  : '%s'\n",  swf_error_code_to_string(error));
      }

    error = SWF_ENoError;

  switch (next_id)
    {
      case tagSetBackgroundColour:
        do_setbackgroundcolour(s->parser, self);
      break;

      case tagDefineFont:
        do_definefont(s->parser, self);
      break;

/*
      case tagDefineFontInfo:
        do_definefontinfo(s->parser, self);
      break;
*/

      case tagDefineText:
        do_definetext(s->parser, self);
      break;

      case tagDefineButton2:
        do_definebutton2(s->parser, self);
      break;

      case tagPlaceObject2:
        do_placeobject2(s->parser, self);
      break;

      case tagEnd :
	do_end(s->parser, self);
      break;

      case tagShowFrame:
        do_frame(s->parser, self);
      break;

    default:
      my_call_simple(self, "unknown_tag", newSVpvf(tag[next_id]));
      break;
    }
  }
  while (next_id > 0);

  free (tag);
}


void do_rect(swf_rect* r, SV* self) {
  my_call_method(self, "start", newSVpvf("rect"));
  my_call_simple(self, "xmin", newSViv(r->xmin));
  my_call_simple(self, "xmax", newSViv(r->xmax));
  my_call_simple(self, "ymin", newSViv(r->ymin));
  my_call_simple(self, "ymax", newSViv(r->ymax));
  my_call_method(self, "end", newSVpvf("rect"));
}


void do_colour(swf_colour* r, SV* self) {
  my_call_method(self, "start", newSVpvf("colour"));
  my_call_simple(self, "r", newSViv(r->r));
  my_call_simple(self, "g", newSViv(r->g));
  my_call_simple(self, "b", newSViv(r->b));
  my_call_simple(self, "a", newSViv(r->a));
  my_call_method(self, "end", newSVpvf("colour"));
}


void do_matrix (swf_matrix* matrix, SV* self) {
  if (matrix==NULL) {
    return;
  }
  my_call_method(self, "start", newSVpvf("matrix"));
  my_call_simple(self, "a", newSVnv((double)matrix->a/65536.0));
  my_call_simple(self, "b", newSVnv((double)matrix->b/65536.0));
  my_call_simple(self, "c", newSVnv((double)matrix->c/65536.0));
  my_call_simple(self, "d", newSVnv((double)matrix->d/65536.0));
  my_call_simple(self, "tx", newSVnv((double)matrix->tx/20.0));
  my_call_simple(self, "ty", newSVnv((double)matrix->ty/20.0));
  my_call_method(self, "end", newSVpvf("matrix"));
}


void do_textrecord(swf_textrecord* node, SV* self) {
  int g;

  my_call_method(self, "start", newSVpvf("text_record"));

  my_call_simple(self, "flags", newSViv(node->flags));
//  printf("\n%s\tflags: 0x%02x\n", str, node->flags);

  if (node->flags & isTextControl) {
    if (node->flags & textHasFont) {
      my_call_simple(self, "font_id", newSViv(node->font_id));
//      printf("%s\tfontId: %ld\n", str, node->font_id);
    }

    if (node->flags & textHasColour) {
      my_call_simple(self, "colour", newSViv(node->colour));
//      printf("%s\tfontColour: %06lx\n", str, node->colour);
    }

    if (node->flags & textHasXOffset) {
      my_call_simple(self, "xoffset", newSViv(node->xoffset));
//      printf("%s\tX-offset: %d\n", str, node->xoffset);
    }

    if (node->flags & textHasYOffset) {
      my_call_simple(self, "yoffset", newSViv(node->yoffset));
//      printf("%s\tY-offset: %d\n", str, node->yoffset);
    }

    if (node->flags & textHasFont) {
      my_call_simple(self, "font_height", newSViv(node->font_height));
//      printf("%s\tFont Height: %d\n", str, node->font_height);
    }
  } else {
      my_call_simple(self, "glyph_count", newSViv(node->glyph_count));
//    printf("%s\tnumber of glyphs: %d\n", str, node->glyph_count);

//    printf("%s\t", str);

    for (g = 0; g < node->glyph_count; g++) {
      my_call_simple(self, "g0", newSViv(node->glyphs[g][0]));
      my_call_simple(self, "g1", newSViv(node->glyphs[g][1]));
//      printf("[%d,%d] ", node->glyphs[g][0], node->glyphs[g][1]);
    }
  }

  my_call_method(self, "end", newSVpvf("text_record"));
}

void do_textrecords (swf_textrecord_list* list, swf_parser* context, SV* self) {
  swf_textrecord * tmp;
  swf_textrecord * node;
  int error = SWF_ENoError;
  char * text = NULL;

  my_call_method(self, "start", newSVpvf("text_records"));

  text = swf_parse_textrecords_to_text(context, &error, list);

  node = list->first;

  while (node != NULL) {
    tmp = node;
    node = node->next;
    do_textrecord(tmp, self);
  }

  if (text!=NULL) {
    my_call_simple(self, "text_representation", newSVpvf(text));
//    printf ("%s\ttext representation : %s\n", str, text);
  }

  my_call_method(self, "end", newSVpvf("text_records"));
  swf_free(text);
}

void do_buttonrecords(swf_buttonrecord_list * list, SV* self) {
  swf_buttonrecord * node;
  int j;

  if (list==NULL){
    return;
  }

  my_call_method(self, "start", newSVpvf("button_records"));

  node = list->first;

  while (node != NULL) {
    my_call_method(self, "start", newSVpvf("button_record"));
    my_call_simple(self, "char", newSViv(node->character));
    my_call_simple(self, "layer", newSViv(node->layer));

/*
    if (node->state_hit_test != 0)  printf("HIT ");
    if (node->state_down != 0)      printf("DOWN ");
    if (node->state_over != 0)      printf("OVER ");
    if (node->state_up != 0)        printf("UP ");
*/

/*    printf("\n"); */


    do_matrix(node->matrix, self);
		
    for (j=0; j<node->ncharacters; j++) {
//      print_cxform(node->characters[j], str); // TODO
    }

    node = node->next;
    my_call_method(self, "end", newSVpvf("button_record"));
  }
  my_call_method(self, "end", newSVpvf("button_records"));
}


void do_end(swf_parser* s, SV* self) {
  my_call_method(self, "start", newSVpvf("tag_end"));
  my_call_method(self, "end", newSVpvf("tag_end"));
}



void do_setbackgroundcolour(swf_parser* context, SV* self) {
  int error = SWF_ENoError;
  SWF_U32 colour;

  swf_setbackgroundcolour* back = swf_parse_setbackgroundcolour(context, &error);

  my_call_method(self, "start", newSVpvf("set_background_colour"));
  do_colour(back->colour, self);
  my_call_method(self, "end", newSVpvf("set_background_colour"));

  swf_destroy_setbackgroundcolour(back);
}


void do_definefont (swf_parser* context, SV* self) {
  int error = SWF_ENoError;
  int n=0;

  swf_definefont* font = swf_parse_definefont(context, &error);

  if (font==NULL) {
    fprintf (stderr, "ERROR: couldn't parse DefineFont  : '%s'\n", swf_error_code_to_string(error));
    return;
  }

  my_call_method(self, "start", newSVpvf("define_font"));
  my_call_simple(self, "font_id", newSViv(font->fontid));
  my_call_simple(self, "offset", newSViv(font->offset));
  my_call_simple(self, "glyph_count", newSViv(font->glyph_count));

  while (n < font->glyph_count && font->shape_records != NULL) {
    n++;
/*    print_shaperecords (font->shape_records[n++], str); // todo */
  }

  my_call_method(self, "end", newSVpvf("define_font"));
  swf_destroy_definefont (font);
}


/*
# note: i get warnings about not finding swf_font_extra
# ask richardc

void do_definefontinfo (swf_parser* context, SV* self) {
  int error = SWF_ENoError;
  int n;
  swf_font_extra* extra;
  swf_definefontinfo* info = swf_parse_definefontinfo(context, &error);

  if (!info || error) {
    fprintf (stderr, "ERROR: couldn't parse DefineFontInfo : '%s'\n", swf_error_code_to_string(error));
    return;
  }

  extra = swf_fetch_font_extra(context, info->fontid, 0);

  my_call_method(self, "start", newSVpvf("define_font_info"));

  printf("%stagDefineFontInfo \tFont ID %-5ld\n", str, info->fontid);
  printf("%s\tNameLen: '%i'\n", str, info->namelen);
  printf("%s\tFontName: '%s'\n", str, info->fontname);
  printf("%s\t", str);


  for(n=0; n < extra->n; n++) {
    printf("[%d,'%c'] ", info->code_table[n], (char) info->code_table[n]);
  }
  printf("\n\n");

  my_call_method(self, "end", newSVpvf("define_font_info"));
  swf_destroy_definefontinfo(info);
}

*/


void do_definetext (swf_parser* context, SV* self) {
  int error = SWF_ENoError;
  swf_definetext * text = swf_parse_definetext(context, &error);

  if (text == NULL) {
    fprintf (stderr, "ERROR : can't parse DefineText : '%s'\n", swf_error_code_to_string(error));
    return;
  }

  my_call_method(self, "start", newSVpvf("define_text"));
  my_call_simple(self, "tagid", newSViv(text->tagid));
  do_rect(text->rect, self);
  do_matrix(text->matrix, self);
  do_textrecords(text->records, context, self);

  my_call_method(self, "end", newSVpvf("define_text"));

  swf_destroy_definetext (text);
}



void do_definebutton2 (swf_parser* context, SV* self) {
  int error = SWF_ENoError;
  swf_definebutton2* button = swf_parse_definebutton2(context, &error);

  if (button == NULL) {
    fprintf (stderr, "ERROR : could not parse DefineButton2 : '%s'\n", swf_error_code_to_string(error));
    return;
  }

  my_call_method(self, "start", newSVpvf("define_button2"));
  do_buttonrecords(button->records, self);

/*
  printf("%stagDefineButton2 \ttagid %-5lu\n", str, button->tagid);
*/

/*
  print_buttonrecords (button->records, str);
  print_button2actions (button->actions, str);
*/

  my_call_method(self, "end", newSVpvf("define_button2"));
  swf_destroy_definebutton2 (button);
}




void do_placeobject2(swf_parser * context, SV* self) {
  int error = SWF_ENoError;
  swf_placeobject2 * place = swf_parse_placeobject2 (context, &error);

  if (place == NULL) {
    fprintf (stderr, "ERROR : couldn't parse PlaceObject2 : '%s'\n", swf_error_code_to_string(error));
    return;
  }

  my_call_method(self, "start", newSVpvf("place_object2"));
  my_call_simple(self, "flags", newSViv(place->flags));
  my_call_simple(self, "depth", newSViv(place->depth));

//  printf("%stagPlaceObject2 \tflags %-5u depth %-5u ", str,  (int) place->flags, (int) place->depth);

  // TODO: generate tags for the flags!

//  if (place->flags & splaceMove)
//    printf("move ");

  /* Get the tag if specified. */
  if (place->flags & splaceCharacter) {
//    printf("tag %-5lu\n", place->tag);
  } else {
//    printf("\n");
  }

  /* Get the matrix if specified. */
  if (place->flags & splaceMatrix) {
    do_matrix(place->matrix, self);
  }

  /* Get the color transform if specified. */
  if (place->flags & splaceColorTransform) {
//    print_cxform(place->cxform, str); // TODO
  }

  /* Get the ratio if specified. */
  if (place->flags & splaceRatio) {
    my_call_simple(self, "ratio", newSViv(place->ratio));
  }

  /* Get the clipdepth if specified. */
  if (place->flags & splaceDefineClip) {
    my_call_simple(self, "clip_depth", newSViv(place->clip_depth));
  }

  /* Get the instance name */
  if (place->flags & splaceName) {
    my_call_simple(self, "name", newSViv(place->name));
  }

  my_call_method(self, "end", newSVpvf("place_object2"));
  swf_destroy_placeobject2 (place);
}


void do_frame(swf_parser* context, SV* self) {
  context->frame++;

  my_call_method(self, "start", newSVpvf("showframe"));
//  printf("%stagShowFrame\n", str);
//  printf("\n<----- dumping frame %ld file offset 0x%04x ----->\n", context->frame, swf_parse_tell(context));
  my_call_simple(self, "number", newSViv(context->frame));
  my_call_simple(self, "offset", newSViv(swf_parse_tell(context)));
  my_call_method(self, "end", newSVpvf("showframe"));
}







void my_call_method(SV* self, char* method, SV* arg) {

  dSP;
  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  XPUSHs(self);
  XPUSHs(arg);
  PUTBACK;

  call_method(method, G_DISCARD);

  FREETMPS;
  LEAVE;
}


void my_call_simple(SV* self, char* name, SV* value) {
  my_call_method(self, "start", newSVpvf(name));
  my_call_method(self, "chars", value);
  my_call_method(self, "end", newSVpvf(name));
}


void destroy(SV* obj) {
  SWF_Parser* s = (SWF_Parser*)SvIV(SvRV(obj));
  swf_destroy_parser(s->parser);
}


const char** init_tags (void) {
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
