package Gravel::Shape;

use strict;
use warnings;

use lib '../';

use Gravel::Actor;
use Gravel::Edge;
use Gravel::Constants qw(:all);

use Data::Dumper qw/DumperX/;
use Digest::MD5 qw(md5_base64);

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);
use Inline C => 'DATA',
#  VERSION => '0.10',
  NAME => 'Gravel::Shape',
  LIBS => '-L' . abs_path(cwd . '/../../') . ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/../'),
  OPTIMIZE => '-g';
#use Inline C => Config => STRUCTS => 'swf_header';


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

    $self->{_edges} = [];
    $self->{_vertices} = [];
    $self->{_styles} = $conf{styles} || [{colour => BLACK, width => 20}];
    $self->{_fills} = $conf{fills} || [{colour => BLACK, type => 'solid'}];

    bless $self => $class;

    return $self;
}

#

sub hash {
    my $self = shift;
	$self->{_hash} = md5_base64(DumperX($self));
}

#

sub last_vertex {
    my $self = shift;

    my $last = scalar(@{$self->{_vertices}}) - 1;
    return $self->{_vertices}->[$last];
}

#

sub vertex {
    my $self = shift;
    my $x = shift;
    my $y = shift;

    push @{$self->{_vertices}}, [$x, $y];
}

#

sub vertices { return (shift)->{_vertices}; }

#

sub line {
    my $self = shift;
    my ($x1, $y1, $x2, $y2) = @_;
    
    my $e = Gravel::Edge->new({x1 => $x1, y1 => $y1, x2 => $x2, y2 => $y2});
    push @{$self->{_edges}}, $e;

    my $ra_v = $self->last_vertex();
    
    unless ( ($ra_v->[0] == $x1) && ($ra_v->[1] == $y1) ) {
	push @{$self->{_vertices}}, [$x1, $y1];
    }
    push @{$self->{_vertices}}, [$x2, $y2];
    
    return scalar(@{$self->{_edges}});
}

#

# We will still need to check that the first and last vertex
# are the same at bake time.

sub line_to {
    my $self = shift;
    my ($x, $y) = @_;

    my $ra_v = $self->last_vertex();
	my $rh = {x1 => $ra_v->[0], y1 => $ra_v->[1], x2 => $x, y2 => $y};
   
	my $e = Gravel::Edge->new($rh);

    push @{$self->{_edges}}, $e;
    push @{$self->{_vertices}}, [$x, $y];

    return scalar(@{$self->{_edges}});
}

#

sub arc {
    my $self = shift;
    my %p;
    @p{'x1', 'y1', 'x2', 'y2', 'ax', 'ay'} = @_;
    
    $p{TYPE} = 'QUADRATIC';
    
    my $e = Gravel::Edge->new(\%p);
    
    push @{$self->{_edges}}, $e;
    
    my $ra_v = $self->last_vertex();
    
    if (defined $ra_v) {
		unless ( ($ra_v->[0] == $p{x1}) && ($ra_v->[1] == $p{y1}) ) {
			push @{$self->{_vertices}}, [$p{x1}, $p{y1}];
		}
    } else {
		push @{$self->{_vertices}}, [$p{x1}, $p{y1}];
    }
    push @{$self->{_vertices}}, [$p{x2}, $p{y2}];
    
    return scalar(@{$self->{_edges}});
}

#

sub arc_to {
    my $self = shift;
    my %p;
    @p{'x2', 'y2', 'ax', 'ay'} = @_;
    
    my $ra_v = $self->last_vertex();
    
    $p{TYPE} = 'QUADRATIC';
    @p{'x1', 'y1'} = @$ra_v;
    
    my $e = Gravel::Edge->new(\%p);
    
    push @{$self->{_edges}}, $e;
    
    push @{$self->{_vertices}}, [$p{x2}, $p{y2}];
    
    return scalar(@{$self->{_edges}});
}

#

sub poly {
    my $self = shift;
    my $x0 = shift;
    my $y0 = shift;
    
    $self->vertex($x0, $y0);
    
	my $x = shift;
    while (defined $x) {
		my $y = shift;
	
		$self->line_to($x, $y);
		$x = shift;
    }
    $self->line_to($x0, $y0);

    return scalar(@{$self->{_edges}});
}

# hack for now

sub set_stroke {
    my $self = shift;
#    my 

#    $self->{_styles}->{0} = 
}

# hack for now

sub set_fill {
}

#

sub action {
    my $self = shift;
	my $e = shift;
	return Gravel::Actor->new({shape => $self, effect => $e, start => $e->start, end => $e->end,});
}



1;

__DATA__

__C__

#include "swf_types.h"
#include "swf_movie.h"
#include "swf_parse.h"
#include "swf_destroy.h"
#include "gravel/util.h"
#include "gravel.h"


void _bake_styles(int * error, swf_defineshape * mytag, HV * h_sh) 
{
	SV** p_sty;
	AV*  a_sty;
	SV** ph_sty;
	HV*  h_sty;
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
			} else if ( (0 == strcmp(fill_type, "solid")) 
						|| (0 == strcmp(fill_type, "solid")) ) {
				 /* FIXME: Do the other styles */
			}

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
		} else if (0 == strcmp(type, "CUBIC")) {
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


void _bake (SV * shape, SV * mov) 
{
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(mov));
	int error = SWF_ENoError;
	SV** p_shape;
	SV** p_butt;
	SWF_U8 button;
	AV* lib;
	HV* h_sh;
	SCOORD x1, x2, y1, y2;
	SV** p_num;
	SV* tag_id;
	swf_tagrecord * temp;
	swf_defineshape * mytag;

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
	
	temp = gravel_make_shape(m->movie, &error, x1, x2, y1, y2);
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
	swf_dump_tag(m->movie, &error, temp);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 10 detected\n");
	}

	return;
}	


