package Gravel::Frame;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);

use Inline C => 'DATA',
#  VERSION => '0.10',
  NAME => 'Gravel::Frame',
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

	$self->{_shape} = $conf{shape};
	$self->{_button} = $conf{button};
	$self->{_depth} = $conf{depth};
	$self->{_matrix} = $conf{matrix};

	$self->{_contents} = $self->{_shape} || $self->{_button};

    bless $self => $class;

    return $self;
}

sub contents { return (shift)->{_contents}; }

sub shape {
	my $self = shift;
	$self->{_contents}->{shape} = shift;
}

sub place {
	my $self = shift;
	$self->{_contents}->{x} = shift;
	$self->{_contents}->{y} = shift;
}

#



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

void * 
_get_struct(SV* obj, char* method) 
{
	int count;
	int ret;

	dSP;
	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(obj);
	PUTBACK;

	count = call_method(method, G_SCALAR);

	SPAGAIN ;

	if (1 != count) {
		croak("Big trouble\n") ;
	}

	ret = POPi;

	PUTBACK;
	FREETMPS;
	LEAVE;

	return (void *)ret;
}

swf_matrix* _matrix_from_frame(int * error, HV * h_frame) {
	swf_matrix * matrix;
	SV** ph_matrix;
	HV*  h_matrix;
	SV** p_num;
	double num;

	if (*error) {
		return;
	}

	ph_matrix = hv_fetch(h_frame, "_matrix", 7, 0);	
	if (NULL == ph_matrix) {
		return NULL;
	}

	matrix = (swf_matrix *)_get_struct(*ph_matrix, "_make_struct");

	return matrix;
}

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

void _bake (SV * self, SV * mov) 
{
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(mov));
	int error = SWF_ENoError;
	HV* h_place = (HV *)SvRV(self); 
	swf_matrix * matrix;
	SWF_U16 depth, obj_id;
	SV** p_depth;
	SV** p_shape;
	SV*  shape;

	obj_id = 0;
	depth = 1;

	// When do we actually want to throw an exception?

	matrix = _matrix_from_frame(&error, h_place);

	p_shape = hv_fetch(h_place, "_contents", 9, 0);	
	if (NULL == p_shape) {  
		return;
	}
	obj_id = _shape_id(&error, *p_shape);

	p_depth = hv_fetch(h_place, "_depth", 6, 0);
	if (NULL == p_depth) {  
		return;
	}
	depth = (SWF_U16)(SvIV(*p_depth));

	swf_add_placeobject(m->movie, &error, matrix, obj_id, depth);

	return;
}

