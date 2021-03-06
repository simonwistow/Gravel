package Gravel::Movie;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;
use Gravel::Effect;
use Gravel::Matrix;

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
	$self->{_bgcol} = $conf{bgcol};
	$self->{_protect} = $conf{protect};

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
    my $g = $e->contents();
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
			push @{$self->{_timeline}->[$i]}, $ra_f->[$i] if $ra_f->[$i];
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

sub protect {
	my $self = shift;
	my $pt = shift;
	$self->{_protect} = $pt if defined $pt;
	return $self->{_protect};
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

	# Set up the canvases for the individual shapes
	# FIXME: Do this in C instead.
	foreach my $s (@{$self->library}) {
		my %tmp = %$s;
		my ($xmin, $ymin, $xmax, $ymax) = @tmp{'_xmin', '_ymin', '_xmax', '_ymax'};
		foreach my $v (@{$s->vertices}) {
			$xmin = $v->[0] unless defined $xmin;
			$ymin = $v->[1] unless defined $ymin;
			$xmax = $v->[0] unless defined $xmax;
			$ymax = $v->[1] unless defined $ymax;

			$xmin = $v->[0] if $v->[0] < $xmin; 
			$ymin = $v->[1] if $v->[1] < $ymin; 
			$xmax = $v->[0] if $v->[0] > $xmax; 
			$ymax = $v->[1] if $v->[1] > $ymax; 
		}
		($s->{_xmin}, $s->{_ymin}, $s->{_xmax}, $s->{_ymax}) 
			= ($xmin, $ymin, $xmax, $ymax);
	}

}

sub bake_movie {
    my $self = shift;

	$self->_prepare();

	my $b = Gravel::Movie->_create_baked();
	$b->_bake_header($self);
	$b->_bake_preamble($self, $self->protect);

#	print STDERR DumperX $self;

	$b->_bake_library($self);

	print STDERR DumperX $self;

	$b->_bake_frames($self);

	$b->_bake_end($self);
	$b->_finalise($self);

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
#include "gravel/util.h"
#include "gravel.h"

#define swf_free(x)       if( (x) ) free( (x) )
#define swf_realloc(x, y) ( (x) ? realloc( (x), (y) ) : calloc( 1, (y) ) )
#define RATE 15

U16 _count_frames(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	swf_tagrecord *temp, *node;
	U16 num = 0;
	
	node = m->movie->first;

	while (node != NULL) {
		temp = node;
		node = node->next;
		++num;
	}

	return num;
}

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
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 0 detected\n");
	}

	p_name = hv_fetch(h, "_name", 5, 0);
	if (NULL != p_name) {
		m->movie->name = (char *)SvPVX(*p_name);
		m->movie->header->rate = RATE * 256;
	}
}


void _bake_preamble(SV* obj, SV* self, U32 protect) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV*  h = (HV *)SvRV(self); 
	SV** p_col;
	swf_colour * col = NULL;

	p_col = hv_fetch(h, "_bgcol", 6, 0);

	if (NULL != p_col) {
		if (!SvOK(*p_col)) {
			col = swf_make_colour(&error, 255, 255, 255);
		} else {
			col = gravel_parse_colour((char *)(SvPVX(*p_col)));
		}
	}

    swf_add_setbgcol(m->movie, &error, col);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 1 detected\n");
	}
	if (protect) {
		swf_add_protect(m->movie, &error);
	}

	/* We have value-copied the contents of col so throw it
	 * away now.
	 */
	swf_free(col);
}

void _bake_library(SV* obj, SV* self) 
{
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 

	SV** p_lib;
	SV** p_shape;
	AV* lib;
	I32 lib_size, i;

	p_lib = hv_fetch(h, "_library", 8, 0);
	if (NULL == p_lib) {
		return;
	}
	lib = (AV *)SvRV(*p_lib);
	lib_size = av_len(lib);
	for (i=0; i<=lib_size; ++i) {
		p_shape = av_fetch(lib, i, 0);
		if (NULL == p_shape) {
			return;
		}
		_call_foreign_method(*p_shape, "_bake", obj);
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
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 11 detected\n");
		return NULL;
	}

	sv_setiv(obj, (IV)m);
	SvREADONLY_on(obj);
	return obj_ref;
}

/* This prototype uses a SV* rather than a HV* to allow us to
 * change to a method call way of getting this instead. Am
 * thinking clips here.
 */

SWF_U16 _shape_id(int * error, SV* shape) {
	HV*  h_shape;
	SV** p_id;

	h_shape = (HV *)SvRV(shape); 
	p_id = hv_fetch(h_shape, "_obj_id", 7, 0);	
	if (NULL != p_id) {
		return (SWF_U16)(SvIV(*p_id));
	}

	return 0;
}

void _bake_remove(swf_movie * movie, int * error, HV * h_place) {
	SWF_U16 depth, obj_id;
	SV** p_depth;
	SV** p_shape;
	SV*  shape;

	obj_id = 0;
	depth = 1;

	// When do we actually want to throw an exception?
	if (*error) {
		fprintf(stderr, "non-zero error code on entry to _bake_remove\n");
		return;
	}

	p_shape = hv_fetch(h_place, "_contents", 9, 0);	
	if (NULL == p_shape) {  
		return;
	}
	obj_id = _shape_id(error, *p_shape);

	p_depth = hv_fetch(h_place, "_depth", 6, 0);
	if (NULL == p_depth) {  
		return;
	}
	depth = (SWF_U16)(SvIV(*p_depth));

	/* Paranoia */
	*error = 0;

	swf_add_removeobject(movie, error, obj_id, depth);

	return;
}


void _bake_frames(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	HV* h_tl = (HV *)SvRV(self); 
    int error = 0;
	SV** pa_frames;
	AV*  a_frames;
	SV** pa_frame;
	AV*  a_frame;
	SV** ph_frame;
	I32  i, j, num_frames;
	U16  tmp = 0;

    pa_frames = hv_fetch(h_tl, "_timeline", 9, 0);	
	if (NULL != pa_frames) {
		a_frames = (AV *)SvRV(*pa_frames); 

		for (i=0; i<=av_len(a_frames); ++i) {
			pa_frame = av_fetch(a_frames, i, 0);
			if (NULL != pa_frame) {
				a_frame = (AV *)SvRV(*pa_frame);

				for (j=0; j<=av_len(a_frame); ++j) {
					ph_frame = av_fetch(a_frame, j, 0);
					if (NULL == ph_frame) {
						return;
					}
					_call_foreign_method(*ph_frame, "_bake", obj);
				}
				swf_add_showframe(m->movie, &error);

				/* If this isnt the last frame, remove all the shapes for the
                   start of the next frame */
				if (i < av_len(a_frames)) {
					for (j=0; j<=av_len(a_frame); ++j) {
						ph_frame = av_fetch(a_frame, j, 0);
						if (NULL == ph_frame) {
							return;
						}
						_bake_remove(m->movie, &error, (HV *)SvRV(*ph_frame));
					}
				}

				if (SWF_ENoError != error) {
					fprintf(stderr, "Non-zero error condition 12 detected\n");
				}
			}
			tmp = swf_movie_tag_count(m->movie, &error);
		}
	}
	return;
}

void _bake_end(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;

    swf_add_end(m->movie, &error);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 13 detected\n");
	}
}


void _finalise(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;

    swf_make_finalise(m->movie, &error);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 14 detected\n");
	}

    swf_destroy_movie(m->movie);
}

void _call_foreign_method(SV* shape, char* method, SV * mov) 
{
	dSP;
	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(shape);
	XPUSHs(mov);
	PUTBACK;

	call_method(method, G_DISCARD);

	FREETMPS;
	LEAVE;
}


