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

#	print STDERR DumperX $self;

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

#define swf_free(x)       if( (x) ) free( (x) )
#define swf_realloc(x, y) ( (x) ? realloc( (x), (y) ) : calloc( 1, (y) ) )

typedef struct {
  swf_movie * movie;
} SWF_Movie;


U16 _count_frames(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 
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
		m->movie->header->rate = 25 * 256;
	}
}


void _bake_preamble(SV* obj, SV* self, U32 protect) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(self); 

	// FIXME: Get background colour from self
    swf_add_setbackgroundcolour(m->movie, &error, 0, 255, 255, 255);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 1 detected\n");
	}
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
			*error = SWF_ENoError;
			mytag->style->lines[j] = swf_make_linestyle(error);
			if (SWF_ENoError != *error) {
				fprintf(stderr, "Non-zero error condition 2 detected\n");
			}

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

	/* paranoia */
	*error = SWF_ENoError;

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
				if (SWF_ENoError != *error) {
					fprintf(stderr, "Non-zero error condition 3 detected\n");
				}

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

	/* paranoia */
	*error = SWF_ENoError;

	/* First, we need a non-edge, change of style record */ 
	record = swf_make_shaperecord(error, 0);
	if (SWF_ENoError != *error) {
		fprintf(stderr, "Non-zero error condition 4 detected\n");
	}
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
	if (SWF_ENoError != *error) {
		fprintf(stderr, "Non-zero error condition 5 detected\n");
	}
	++mytag->record->record_count;
}

void _bake_edge(int * error, swf_defineshape * mytag, HV * h_edge) 
{
	swf_shaperecord * record;
	SWF_S32 x1, y1, x2, y2, ax, ay;
	SV** p_type;
	const char * type;
	SV** p_num;

	/* paranoia */
	*error = SWF_ENoError;

	record = swf_make_shaperecord(error, 1);
	if (SWF_ENoError != *error) {
		fprintf(stderr, "Non-zero error condition 6 detected\n");
	}

	p_type = hv_fetch(h_edge, "_EDGE_TYPE", 10, 0);	
	if (NULL != p_type) {

		type = (const char *)SvPVX(*p_type);		

		if ( (0 == strcmp(type, "STRAIGHT")) 
			 || (0 == strcmp(type, "LINE")) ){
			p_num = hv_fetch(h_edge, "_x1", 3, 0);	
			if (NULL != p_num) {
				x1 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_x2", 3, 0);	
			if (NULL != p_num) {
				x2 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_y1", 3, 0);	
			if (NULL != p_num) {
				y1 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_y2", 3, 0);	
			if (NULL != p_num) {
				y2 = (SWF_S32)(SvIV(*p_num));
			}

			record->x = x2 - x1;
			record->y = y2 - y1;

		} else if ( (0 == strcmp(type, "QUADRATIC"))  
					|| (0 == strcmp(type, "ARC"))
					|| (0 == strcmp(type, "CURVED")) ) {
			p_num = hv_fetch(h_edge, "_x1", 3, 0);	
			if (NULL != p_num) {
				x1 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_x2", 3, 0);	
			if (NULL != p_num) {
				x2 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_y1", 3, 0);	
			if (NULL != p_num) {
				y1 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_y2", 3, 0);	
			if (NULL != p_num) {
				y2 = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_ax", 3, 0);	
			if (NULL != p_num) {
				ax = (SWF_S32)(SvIV(*p_num));
			}

			p_num = hv_fetch(h_edge, "_ay", 3, 0);	
			if (NULL != p_num) {
				ay = (SWF_S32)(SvIV(*p_num));
			}

			record->cx = x2 - x1;
			record->cy = y2 - y1;

			/* spec says these are expressed as deltas */
			record->ax = ax - x1;
			record->ay = ay - y1;
		}


	}

	swf_add_shaperecord(mytag->record, error, record);
	if (SWF_ENoError != *error) {
		fprintf(stderr, "Non-zero error condition 7 detected\n");
	}
	++mytag->record->record_count;
}

void _bake_edges(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** pa_edges;
	AV*  a_edges;
	SV** ph_edge;
	I32  j, num_edges;
	swf_shaperecord * record;

	/* paranoia */
	*error = SWF_ENoError;

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


		record = swf_make_shaperecord(error, 0);
		if (SWF_ENoError != *error) {
			fprintf(stderr, "Non-zero error condition 8 detected\n");
		}
		swf_add_shaperecord(mytag->record, error, record);
		++mytag->record->record_count;
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
	SV* tag_id;
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
		if (SWF_ENoError != error) {
			fprintf(stderr, "Non-zero error condition 9 detected\n");
		}

        tag_id = newSViv((IV)mytag->tagid);

	    hv_store(h_sh, "_obj_id", 7, tag_id, 0);

		_bake_styles(&error, mytag, h_sh);
		_bake_fills(&error, mytag, h_sh);

		_get_to_start(&error, mytag, h_sh);
		_bake_edges(&error, mytag, h_sh);
		
		/* Need to calloc a (raw) buffer for temp... */
		if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
			fprintf (stderr, "Calloc Fail\n");
			return;
		}

		/* Now serialise and dump */
		swf_serialise_defineshape(temp->buffer, &error, (swf_defineshape *) temp->tag);
		temp->serialised = 1;
		swf_dump_shape(m->movie, &error, temp);
		if (SWF_ENoError != error) {
			fprintf(stderr, "Non-zero error condition 10 detected\n");
		}
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

void _bake_place(swf_movie * movie, int * error, HV * h_place) {
	swf_matrix * matrix;
	SWF_U16 depth, obj_id;
	SV** ph_details;
	HV*  h_details;
	SV** p_shape;
	SV*  shape;

	obj_id = 0;

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
		*error = SWF_EMallocFailure;
		return;
    }

	/* test code */
    matrix->a  = matrix->d  = 512 * 1000;
	matrix->tx = matrix->ty = 100 * 20;


	ph_details = hv_fetch(h_place, "_contents", 9, 0);	
	if (NULL != ph_details) {  
		h_details = (HV *)SvRV(*ph_details); 

		p_shape = hv_fetch(h_details, "shape", 5, 0);	
		if (NULL != p_shape) {  
			obj_id = _shape_id(error, *p_shape);
		}

		/* Paranoia */
		*error = 0;
		swf_add_placeobject(movie, error, matrix, obj_id, 1);
	}
	return;
}

void _bake_frames(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	HV* h_tl = (HV *)SvRV(self); 
    int error;
	SV** pa_frames;
	AV*  a_frames;
	SV** pa_frame;
	AV*  a_frame;
	SV** ph_shape;
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
					ph_shape = av_fetch(a_frame, j, 0);
					if (NULL != ph_shape) {
						_bake_place(m->movie, &error, (HV *)SvRV(*ph_shape));
					}
				}
				swf_add_showframe(m->movie, &error);
				if (SWF_ENoError != error) {
					fprintf(stderr, "Non-zero error condition 12 detected\n");
				}
			}
			tmp = swf_movie_tag_count(m->movie, &error);
		}
	}
	return;
}

void _bake_test(SV* obj, SV* self) {
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(obj));
	int error = SWF_ENoError;
	swf_matrix * matrix;

//    error = 0;

    if ((matrix = (swf_matrix *) calloc (1, sizeof (swf_matrix))) == NULL) {
		error = SWF_EMallocFailure;
		return;
    }

/* FIXME: Do object IDs properly */

/* FIXME: Test matrix */
    matrix->a  = matrix->d  = 512 * 1000;
	matrix->tx = matrix->ty = 100 * 20;

	swf_add_placeobject(m->movie, &error, matrix, 1, 1);
    swf_add_showframe(m->movie, &error);
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
