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

sub name {
	my $self = shift;
	my $name = shift;
	$self->{_name} = $name if $name;
	return $self->{_name};
}


#

sub size {
    my $self = shift;

    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		@conf{'_x1', '_y1', '_x2', '_y2'} = @_;
    }

	%{$self} = (%{$self}, %conf);
}

#

sub _prepare {
    my $self = shift;

	foreach my $s (@{$self->library}) {
		
	}

}

sub bake_movie {
    my $self = shift;

	$self->_prepare();

	my $b = Gravel::Movie->_create_baked();
	$b->_bake_header($self);
	$b->_bake_library($self);


	$b->_bake_rest($self);

	return $b;
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


void _bake_header(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 
	SV** p_name;
	SV** p_num;
	SCOORD x1, x2, y1, y2;

	p_num = hv_fetch(h, "_x1", 3, 0);
	if (NULL != p_num) {
		x1 = (SCOORD)(SvIV(*p_num));
	}
	p_num = hv_fetch(h, "_x2", 3, 0);
	if (NULL != p_num) {
		x2 = (SCOORD)(SvIV(*p_num));
	}
	p_num = hv_fetch(h, "_y1", 3, 0);
	if (NULL != p_num) {
		y1 = (SCOORD)(SvIV(*p_num));
	}
	p_num = hv_fetch(h, "_y2", 3, 0);
	if (NULL != p_num) {
		y2 = (SCOORD)(SvIV(*p_num));
	}

    swf_make_header(m->movie, &error, x1, x2, y1, y2);

	p_name = hv_fetch(h, "_name", 5, 0);
	if (NULL != p_name) {
		m->movie->name = (char *)SvPVX(*p_name);
	}
}


void _bake_library(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 
	SV** p_lib;
	SV** p_shape;
	AV* lib;
	SV* shape;
	I32 lib_size, i;

	p_lib = hv_fetch(h, "_library", 8, 0);
	if (NULL != p_lib) {
		lib = (AV *)SvRV(*p_lib);
	}
	lib_size = av_len(lib);
	for (i=0; i<=lib_size; ++i) {
		p_shape = av_fetch(lib, i, 0);
		if (NULL != p_shape) {
			shape = *p_shape;
		}
		
// At this point, we have the shape to be turned 
// into an unserialised defineShape 
	
	    shape 

	}

}

SV* _create_baked(char* class) {
	SV* obj_ref = newSViv(0);
	SV*     obj = newSVrv(obj_ref, class);
	SWF_Movie *m;
	int error = SWF_ENoError;

	if ((m = (SWF_Movie *) calloc (1, sizeof(SWF_Movie))) == NULL) {
		return NULL;
	}

	if ((m->movie = swf_make_movie(&error)) == NULL) {
		return NULL;
    }


	sv_setiv(obj, (IV)m);
	SvREADONLY_on(obj);
	return obj_ref;
}


SV* _bake_rest(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
    int error;
    swf_tagrecord * temp;

    error = 0;

    temp = swf_make_triangle(m->movie, &error);

    /* Need to calloc a (raw) buffer for temp... */
    if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
		fprintf (stderr, "Calloc Fail\n");
        return NULL;
    }

	fprintf(stderr, "Foo 4\n");

    swf_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
    temp->serialised = 1;


	fprintf(stderr, "Foo 5\n");

//    swf_add_protect(movie, &error);
    swf_add_setbackgroundcolour(m->movie, &error, 0, 255, 0, 255);
    swf_dump_shape(m->movie, &error, temp);

    swf_add_showframe(m->movie, &error);
    swf_add_end(m->movie, &error);

    swf_make_finalise(m->movie, &error);

    swf_destroy_movie(m->movie);
    swf_free(temp->buffer->raw);

	fprintf(stderr, "Foo 6\n");

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
