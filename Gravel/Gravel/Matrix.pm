package Gravel::Matrix;

use strict;
use warnings;

use lib '../';

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);
use Inline C => 'DATA',
#  VERSION => '0.10',
  NAME => 'Gravel::Matrix',
  LIBS => ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/../'),
  OPTIMIZE => '-g';
#use Inline C => Config => STRUCTS => 'swf_header';

# FIXME

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

# FIXME: add colour transform
    my $self = {_a => 1, _b => 0,
				_c => 0, _d => 1,
				_x => 0, _y => 0,
			};

	$self->{_x} = $conf{x} if defined $conf{x};
	$self->{_y} = $conf{y} if defined $conf{y};

	$self->{_a} = $conf{a} if defined $conf{a};
	$self->{_c} = $conf{c} if defined $conf{c};

	$self->{_b} = $conf{b} if defined $conf{b};
	$self->{_d} = $conf{d} if defined $conf{d};

    bless $self => $class;

    return $self;
}

#

sub combine {
	my $self = shift;
	my $comb = shift;

	$self->{_x} += $comb->{_x};
	$self->{_y} += $comb->{_y};

	my ($a, $b, $c, $d);

	$a = $self->{_a} * $comb->{_a} + $self->{_b} * $comb->{_c};
	$b = $self->{_a} * $comb->{_b} + $self->{_b} * $comb->{_d};

	$c = $self->{_c} * $comb->{_a} + $self->{_d} * $comb->{_c};
	$d = $self->{_c} * $comb->{_b} + $self->{_d} * $comb->{_d};

	($self->{_a}, $self->{_b}) = ($a, $b);
	($self->{_c}, $self->{_d}) = ($c, $d);
}

#

sub add {
	my $self = shift;
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

void * _make_struct(SV * self) 
{
	swf_matrix * matrix;
	SV** p_num;
	HV*  h_matrix;
	double num;

	if ((matrix = (swf_matrix *) calloc(1, sizeof(swf_matrix))) == NULL) {
		return NULL;
	}
	
	h_matrix = (HV *)SvRV(self);
	
	p_num = hv_fetch(h_matrix, "_a", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	num = (double)(SvNV(*p_num));
	matrix->a = (SFIXED)(MATRIX_SCALE * num);

	p_num = hv_fetch(h_matrix, "_b", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	num = (double)(SvNV(*p_num));
	matrix->b = (SFIXED)(MATRIX_SCALE * num);

	p_num = hv_fetch(h_matrix, "_c", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	num = (double)(SvNV(*p_num));
	matrix->c = (SFIXED)(MATRIX_SCALE * num);

	p_num = hv_fetch(h_matrix, "_d", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	num = (double)(SvNV(*p_num));
	matrix->d = (SFIXED)(MATRIX_SCALE * num);
	
	p_num = hv_fetch(h_matrix, "_x", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	matrix->tx = (SCOORD)(SvIV(*p_num));

	p_num = hv_fetch(h_matrix, "_y", 2, 0);
	if (NULL == p_num) {
		return NULL;
	}
	matrix->ty = (SCOORD)(SvIV(*p_num));

	return (void *)matrix;
}
