package Gravel::ButtonTest;

use strict;
use warnings;

use lib '../';

use Gravel::Effect;
use Gravel::Matrix;
use Gravel::Effect::Tween;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);

use Inline C => 'DATA',
#  VERSION => '0.10',
  NAME => 'Gravel::Button',
  LIBS => '-L' . abs_path(cwd . '/../../') . ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/../'),
  OPTIMIZE => '-g';
#use Inline C => Config => STRUCTS => 'swf_header';


sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		%conf = @_;
    }

	my $self = {};

	# Set default for the menu flag
	$self->{_menu} = 0;

	$self->{_start} = $conf{start};
	$self->{_shape} = $conf{shape};
    $self->{_end} = $conf{end};
    $self->{_depth} = $conf{depth} || 1;

    $self->{_tx} = $conf{tx};
    $self->{_ty} = $conf{ty};

	bless $self => $class;

	return $self;
}

#

# A buttons vertices are the extremal vertices of all
# its' potential composite shapes

sub vertices {
	my $self = shift;

	my $vert = [];
	foreach my $state (@{$self->{_states}}) {
		push @$vert, $state->shape->vertices;
	}

	return $vert;
}

#

sub make {
	my $self = shift;

	my $s = {start => $self->{_start}, end => $self->{_end}, 
			 startx => $self->{_tx}, endx => $self->{_tx},
			 starty => $self->{_ty}, endy => $self->{_ty},
			 depth => $self->{_depth}, 
		 };

	my $e = Gravel::Effect::Tween->new($s);
	return Gravel::Actor->new({button => $self, effect => $e, start => $e->start, end => $e->end,});
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

#define HASH_LENGTH 22

int _check_obj_hash(HV * shape, char ** bst, int * curr) 
{
	int i, match;
	SV** p_hash;
	char * tmp;
	char * hash;

	if ((hash = (char *) calloc(HASH_LENGTH, sizeof(char))) == NULL) {
		return 0;
	}	

	p_hash = hv_fetch(shape, "_hash", 5, 0);
	if (NULL == p_hash) {
		return 0;
	}
	tmp = (char *)SvPVX(*p_hash);
	for (i=0; i<HASH_LENGTH; ++i) {
		hash[i] = tmp[i];
	}

	match = 0;
	for (i=0; i<=*curr; ++i) {
		match = strcmp((const char *) hash, (const char *)(bst[i]));
		if (0 == match) {
			return match;
		}
	}
	bst[(*curr)++] = hash;

	return match;
}



/* This is test code.... */

void _bake (SV * shape, SV * mov) 
{
	SWF_Movie* m = (SWF_Movie*)SvIV(SvRV(mov));
	int error = SWF_ENoError;
	HV* h = (HV *)SvRV(shape); 
	SV** p_st;
	SV** p_state;
	SV** p_shape;
	AV* states;
	SV** p_bstate;
	HV* bstate;
	HV* shape;
	int i, j, num_st, curr, seen;
	int* p_bst;
	swf_definebutton * button;
	SV** p_matrix;
	swf_matrix * mx;
	SWF_U32 char_id, depth, hit_test, down, over, up, button_id, layer;
	SV** p_num;
	swf_tagrecord * temp;
	SV* tag_id;

	p_shape = hv_fetch(h, "_shape", 6, 0);
	if (NULL == p_shape) {
		return;
	}
	_call_foreign_method(*p_shape, "_bake", mov);			

	p_num = hv_fetch(h, "_depth", 6, 0);
    if (NULL == p_num) {
	    return;
    }
    depth = (SWF_U32)(SvIV(*p_num));

    char_id = m->movie->max_obj_id;
    button_id = 12 + m->movie->max_obj_id;

    swf_add_definebutton(m->movie, &error, button_id, char_id);

	tag_id = newSViv((IV)button_id);
	hv_store(h, "_obj_id", 7, tag_id, 0);
	shape = newRV_inc((SV *) h);


}



void * 
_get_struct(SV* obj, char* method) {
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




void _call_foreign_method(SV* shape, char* method, SV * mov) {
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


// Old code from Movie.pm - needed when we're actually baking the buttons.
// .....
/*		p_butt = hv_fetch(h_sh, "_is_button", 10, 0);
		if (NULL != p_butt) {
			button = (SWF_U8)(SvIV(*p_butt));
			if (button) {
				tmp = swf_make_tagrecord(&error, tagDefineButton);
				tmp_tag = (swf_definebutton *) tmp->tag;
				_bake_button(&error, tmp_tag, h_sh);
				swf_serialise_definebutton(tmp->buffer, &error, tmp_tag);
				tmp->serialised = 1;
				swf_dump_tag(m->movie, &error, tmp);
				if (SWF_ENoError != error) {
					fprintf(stderr, "Non-zero error condition 10a detected\n");
				}
			}
		}
*/


