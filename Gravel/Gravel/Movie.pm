package Gravel::Movie;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;
use Gravel::Effect;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);

use Inline C => 'DATA',
#  VERSION => '0.10',
  NAME => 'Gravel::Movie',
  LIBS => ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/../'),
  OPTIMIZE => '-g';
#use Inline C => Config => STRUCTS => 'swf_header';

#

sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self = {};
    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		%conf = @_;
    }

    $self->{_library} = [];
    $self->{_events} = [];
    $self->{_timeline} = [];

    bless $self => $class;

    return $self;
}

#

sub event {
    my $self = shift;
    my $e = shift;
    push @{$self->{_events}}, $e;
    my $g = $e->shape();
    foreach (@{$self->{_library}}) {
		return if $_ == $g;
    }
    push @{$self->{_library}}, $g;
}

#

sub library {
    my $self = shift;
    my $g = shift;
    return $self->{_library} unless $g;
    push @{$self->{_library}}, $g;
}

#

sub make_timeline {
    my $self = shift;

    foreach my $e (@{$self->{_events}}) {
		my $ra_f = $e->frames();
		for(my $i = 0; $i < scalar(@$ra_f); ++$i) {
			$self->{_timeline}->[$i] = $ra_f->[$i] if $ra_f->[$i];
		}
    }
}

#

sub bake_movie {
    my $self = shift;

	$self->{_baked} = Gravel::Movie->_bake_movie();
	
}

#

1;

__DATA__

__C__

#include "swf_types.h"
#include "swf_movie.h"
#include "swf_parse.h"
#include "swf_destroy.h"

#define swf_free(x)       if( (x) ) free( (x) )
#define swf_realloc(x, y) ( (x) ? realloc( (x), (y) ) : calloc( 1, (y) ) )

typedef struct {
  swf_movie * movie;
} SWF_Movie;


void do_header(SV* obj, SV* self) {
  SWF_Movie* s = (SWF_Movie*)SvIV(SvRV(obj));
}

/*
SV* create(char* class, char* filename) {
  SV* obj_ref = newSViv(0);
  SV*     obj = newSVrv(obj_ref, class);
  SWF_Movie *s = malloc(sizeof(SWF_Movie));
  int error = SWF_ENoError;
  s->movie = swf_parse_create(filename, &error);

  if (error != SWF_ENoError) {
    die("Error creating SWF parser (correct file name?)\n");
  }

  sv_setiv(obj, (IV)s);
  SvREADONLY_on(obj);
  return obj_ref;
}
*/

SV* _bake_movie (char* class) {
	SV* obj_ref = newSViv(0);
	SV*     obj = newSVrv(obj_ref, class);
    SWF_Movie * m;
    int error;
    swf_tagrecord * temp;

    error = 0;

    if ((m = (SWF_Movie *) calloc (1, sizeof(SWF_Movie))) == NULL) {
		return NULL;
    }

    if ((m->movie = swf_make_movie(&error)) == NULL) {
		return NULL;
    }

	fprintf(stderr, "Foo 1\n");

    swf_make_header(m->movie, &error, -4000, 4000, -4000, 4000);
    m->movie->name = "ben1.swf\0";

	fprintf(stderr, "Foo 2\n");

    temp = swf_make_triangle(m->movie, &error);

	fprintf(stderr, "Foo 3\n");

    /* Need to calloc a (raw) buffer for temp... */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
		fprintf (stderr, "Calloc Fail\n");
        return NULL;
    }

	fprintf(stderr, "Foo 4\n");

    swf_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
    temp->serialised = 1;


//    swf_add_protect(movie, &error);
    swf_add_setbackgroundcolour(m->movie, &error, 0, 255, 0, 255);
    swf_dump_shape(m->movie, &error, temp);

    swf_add_showframe(m->movie, &error);
    swf_add_end(m->movie, &error);

    swf_make_finalise(m->movie, &error);

    swf_destroy_movie(m->movie);
    swf_free(temp->buffer->raw);


	sv_setiv(obj, (IV)m);
	SvREADONLY_on(obj);
	return obj_ref;
}
