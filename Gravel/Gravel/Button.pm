package Gravel::Button;

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
	my $ra_g = shift;
	
	$self->{_states} = $ra_g;

	my $s = {start => $self->{_start}, end => $self->{_end}, 
			 startx => $self->{_tx}, endx => $self->{_tx},
			 starty => $self->{_ty}, endy => $self->{_ty},
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

/* This needs to know how to bake its component shapes */

/* We are baking the library in this function */
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
	SWF_U32 char_id, depth, hit_test, down, over, up, button_id;
	SV** p_num;
	swf_tagrecord * temp;


    temp = swf_make_tagrecord(error, tagDefineButton);

/*
  The logic here is basically:

1) Bake all the shapes we need for the button:
	foreach (states) {
		if (state->shape has been seen before) {
			next;
		}
		bake state->shape; (using Gravel::Shape::_bake )
	} 

*/

	p_st = hv_fetch(h, "_states", 7, 0);
	if (NULL == p_st) {
		return;
	}
	states = (AV *) SvRV(*p_st);

	curr = 0;
	num_st = av_len(states);
	if ((p_bst = (int *) calloc(num_st, sizeof(int))) == NULL) {
		return;
	}

	for (i=0; i<=num_st; ++i) {
		p_state = av_fetch(states, i, 0);
		if (NULL == p_state) {
			return;
		}
		bstate = (HV *) SvRV(*p_state);
		p_shape = hv_fetch(bstate, "_shape", 6, 0);
		if (NULL == p_shape) {
			return;
		}
		seen = 0;
		for (j=0; j<curr; ++j) {
			if (p_bst[j] == (int)*p_shape) {
				++seen;
			}
		}
		/* We havent seen this shape before */
		if (0 == seen) {
			p_bst[curr++] = (int)*p_shape;
			_call_foreign_method(*p_shape, "_bake", mov);			
		}
	}

/* FIXME: In this release, the depth attribute is taken from the
 * Button, not the ButtonState.
 */

/* 2) Bake the button itself:  

	spin up a definebutton.
	foreach (states) {
	    add_relevant_buttonrecs()
    }
    finalise;
    serialise_button;
    dump_tagrecord;

	*/

	p_num = hv_fetch(h, "_depth", 6, 0);
    if (NULL == p_num) {
	    return;
    }
    depth = (SWF_U32)(SvIV(*p_num));

    button_id = ++m->movie->max_obj_id;
	button = swf_make_definebutton(&error, button_id);
	hv_store(h, "_obj_id", 7, newSViv((IV)button_id), 0);

	for (i=0; i<=num_st; ++i) {
		p_state = av_fetch(states, i, 0);
		if (NULL == p_state) {
			return;
		}
		bstate = (HV *) SvRV(*p_state);

        /* First get the shape character ID */
	    p_shape = hv_fetch(bstate, "_shape", 6, 0);
		if (NULL == p_state) {
			return;
		}
        shape = (HV *) SvRV(*p_shape);
        p_num = hv_fetch(shape, "_obj_id", 7, 0);
        if (NULL == p_num) {
			return;
		}
        char_id = (SWF_U32)(SvIV(*p_num));

     	p_matrix = hv_fetch(bstate, "_matrix", 7, 0);
		if (NULL == p_matrix) {
			return;
		}

	    mx = (swf_matrix *)_get_struct(*p_matrix, "_make_struct");
        
        p_num = hv_fetch(bstate, "_up", 3, 0);
        if (NULL == p_num) {
			return;
		}
        up = (SWF_U32)(SvIV(*p_num));

        p_num = hv_fetch(bstate, "_down", 5, 0);
        if (NULL == p_num) {
			return;
		}
        down = (SWF_U32)(SvIV(*p_num));

        p_num = hv_fetch(bstate, "_hit", 4, 0);
        if (NULL == p_num) {
			return;
		}
        hit_test = (SWF_U32)(SvIV(*p_num));

        p_num = hv_fetch(bstate, "_over", 5, 0);
        if (NULL == p_num) {
			return;
		}
        over = (SWF_U32)(SvIV(*p_num));

        swf_add_buttonrec(button, &error, char_id, mx, depth, hit_test, down, over, up);
    }

	if ((temp->buffer->raw = (SWF_U8 *) calloc (10240, sizeof (SWF_U8))) == NULL) {
		fprintf (stderr, "Calloc Fail\n");
		return;
	}

	swf_serialise_definebutton(temp->buffer, &error, (swf_definebutton *) button);
	temp->serialised = 1;
	swf_dump_tag(m->movie, &error, temp);
	if (SWF_ENoError != error) {
		fprintf(stderr, "Non-zero error condition 10 detected\n");
	}


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


