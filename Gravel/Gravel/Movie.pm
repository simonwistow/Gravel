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

sub protect {
	my $self = shift;
	my $pt = shift;
	$self->{_protect} = $pt if $pt;
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

#	print STDERR DumperX $self;

	my $b = Gravel::Movie->_create_baked();
	$b->_bake_header($self);
	$b->_bake_preamble($self, 0);
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


void _bake_preamble(SV* obj, SV* self, U32 protect) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 

	// FIXME: Get background colour from self
    swf_add_setbackgroundcolour(m->movie, &error, 0, 255, 0, 255);
	if (protect) {
		swf_add_protect(m->movie, &error);
	}

}

void _bake_styles(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** p_sty;
	AV* a_sty;
	SV** ph_sty;
	HV* h_sty;
	SV** p_col;
	SV** p_num;
	I32  j, num_styles, width;

	/* Now populate the line styles */
	p_sty = hv_fetch(h_sh, "_styles", 7, 0);
	if (NULL != p_sty) {
		a_sty = (AV *)SvRV(*p_sty); 
		
		num_styles = av_len(a_sty);
		mytag->style->nlines = 1 + num_styles;
		
		for (j=0; j<=num_styles; ++j) {
			mytag->style->lines[j] = swf_make_linestyle(error);
			
			ph_sty = av_fetch(a_sty, j, 0);
			if (NULL != ph_sty) {
				h_sty = (HV *)SvRV(*ph_sty); 
				p_num = hv_fetch(h_sty, "width", 5, 0);
				if (NULL != p_num) {
					mytag->style->lines[j]->width = (U16)(SvIV(*p_num));
				}

				p_col = hv_fetch(h_sty, "colour", 6, 0);
				if (NULL != p_col) {
					mytag->style->lines[j]->col = gravel_parse_colour((char *)(SvPVX(*p_col)));
				}
			}
		}
	}

}


void _bake_fills(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** p_sty;
	AV* a_sty;
	I32  j, num_styles;
	SV** p_fill;
	SV** ph_fill;
	HV* h_fill;
	const char * fill_type;
	SV** p_col;

	/* Now populate the fill styles */
    p_sty = hv_fetch(h_sh, "_fills", 6, 0);
	if (NULL != p_sty) {
		a_sty = (AV *)SvRV(*p_sty); 
		
		num_styles = av_len(a_sty);
		mytag->style->nfills = 1 + num_styles;
		for (j=0; j<=num_styles; ++j) {
			ph_fill = av_fetch(a_sty, j, 0);
			if (NULL != ph_fill) {
				h_fill = (HV *)SvRV(*ph_fill);
			}
			p_fill = hv_fetch(h_fill, "type", 4, 0);
			if (NULL != p_fill) {
				fill_type = (const char *) SvPVX(*p_fill);
			}	
			if ( (0 == strcmp(fill_type, "solid")) 
				 || (0 == strcmp(fill_type, "SOLID")) ){
				mytag->style->fills[j] = swf_make_solid_fillstyle(error);
				p_col = hv_fetch(h_fill, "colour", 6, 0);
				if (NULL != p_col) {
					mytag->style->fills[j]->col = gravel_parse_colour((char *)SvPVX(*p_col));
				}
			} /* FIXME: Do the other styles */
		}			
	}
}


void _get_to_start(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** pa_vert;
	AV*  a_vert;
	SV** p_vert;
	SV** p_num;
	AV*  a_num;
	swf_shaperecord * record;

	/* First, we need a non-edge, change of style record */ 
	record = swf_make_shaperecord(error, 0);
	record->flags = eflagsFill1 | eflagsMoveTo;

	record->fillstyle0 = 0;
	record->fillstyle1 = 1;
	record->linestyle = 1;

	record->x = 0;
	record->y = 0;

	/* initial start point determined by first vertex */
    pa_vert = hv_fetch(h_sh, "_vertices", 6, 0);
	if (NULL != pa_vert) {
		a_vert = (AV *)SvRV(*pa_vert); 
		p_vert = av_fetch(a_vert, 0, 0);

		if (NULL != p_vert) {
			a_num = (AV *)SvRV(*p_vert);

			p_num = av_fetch(a_num, 0, 0);
			if (NULL != p_num) {
				record->x = (SWF_S32)(SvIV(*p_num));
			}

			p_num = av_fetch(a_num, 0, 0);
			if (NULL != p_num) {
				record->y = (SWF_S32)(SvIV(*p_num));
			}
		}
	}

	swf_add_shaperecord(mytag->record, error, record);
	++mytag->record->record_count;
}

void _bake_edge(int * error, swf_defineshape * mytag, HV * h_edge) 
{
	swf_shaperecord * record;

	record = swf_make_shaperecord(error, 0);




	swf_add_shaperecord(mytag->record, error, record);
	++mytag->record->record_count;
}

void _bake_edges(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** pa_edges;
	AV*  a_edges;
	SV** ph_edge;
	I32  j, num_edges;

    pa_edges = hv_fetch(h_sh, "_edges", 6, 0);	
	if (NULL != pa_edges) {
		a_edges = (AV *)SvRV(*pa_edges); 

		num_edges = av_len(a_edges);
		for (j=0; j<=num_edges; ++j) {
			ph_edge = av_fetch(a_edges, j, 0);
			if (NULL != ph_edge) {
				_bake_edge(error, mytag, (HV *)SvRV(*ph_edge));
			}
		}
	}
}

void _bake_library(SV* obj, SV* self) 
{
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 
	SV** p_lib;
	SV** p_shape;
	AV* lib;
	SV* shape;
	HV* h_sh;
	I32 lib_size, i;
	SCOORD x1, x2, y1, y2;
	SV** p_num;
	swf_tagrecord * temp;
	swf_defineshape * mytag;

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
		
		/* At this point, we have the shape to be turned 
		   into an unserialised defineShape */
	    h_sh = (HV *) SvRV(shape); 

		p_num = hv_fetch(h_sh, "_xmin", 5, 0);
		if (NULL != p_num) {
			x1 = (SCOORD)(SvIV(*p_num));
		}
		p_num = hv_fetch(h_sh, "_xmax", 5, 0);
		if (NULL != p_num) {
			x2 = (SCOORD)(SvIV(*p_num));
		}
		p_num = hv_fetch(h_sh, "_ymin", 5, 0);
		if (NULL != p_num) {
			y1 = (SCOORD)(SvIV(*p_num));
		}
		p_num = hv_fetch(h_sh, "_ymax", 5, 0);
		if (NULL != p_num) {
			y2 = (SCOORD)(SvIV(*p_num));
		}

	    temp = gravel_create_shape(m->movie, &error, x1, x2, y1, y2);
		mytag = (swf_defineshape *) temp->tag;

		_bake_styles(&error, mytag, h_sh);
		_bake_fills(&error, mytag, h_sh);

		_get_to_start(&error, mytag, h_sh);
		/* Now do the edge records */

		_bake_edges(&error, mytag, h_sh);


		/* Need to calloc a (raw) buffer for temp... */
		if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
			fprintf (stderr, "Calloc Fail\n");
			return;
		}

		fprintf(stderr, "Getting to serialise...\n");

		/* Now serialise and dump */
		swf_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
		temp->serialised = 1;
		swf_dump_shape(m->movie, &error, temp);
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
