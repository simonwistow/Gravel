package Gravel::Matrix;

use strict;
use warnings;

use lib '../';

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

#use Cwd qw(cwd abs_path);
#use Inline C => 'DATA',
#  VERSION => '0.10',
#  NAME => 'Gravel::Effect',
#  LIBS => '-L' . abs_path(cwd . '/../../') . ' -lswfparse',
#  INC => '-I' . abs_path(cwd . '/../../'),
#  OPTIMIZE => '-g';
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

