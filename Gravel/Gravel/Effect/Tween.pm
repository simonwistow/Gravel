package Gravel::Effect::Tween;

use strict;
use warnings;

use lib '../../';

use Gravel::Effect;
use Gravel::Matrix;

use Gravel::Frame;

use base qw(Gravel::Effect);

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

# FIXME: 

sub new {
    my $class = shift;
#    my $g = shift;

#	my $self = {};
    my $self = $class->SUPER::new(@_);

    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		%conf = @_;
    }

    $self->{_start} = $conf{start};
    $self->{_end} = $conf{end};
    $self->{_depth} = $conf{depth} || 1;

    $self->{_startx} = $conf{startx};
    $self->{_endx} = $conf{endx};

    $self->{_starty} = $conf{starty};
    $self->{_endy} = $conf{endy};

    return $self;
}

#

sub matrices {
	my $self = shift;
	
	my $ra_m = [];

	my $numf = $self->{_end} - $self->{_start};
	my ($x, $y) = ($self->{_startx}, $self->{_starty});

	my $dx = ( $self->{_endx} - $self->{_startx} ) / $numf;
	my $dy = ( $self->{_endy} - $self->{_starty} ) / $numf;

	for (my $i = $self->{_start}; $i < $self->{_end}; ++$i) {
		my $m = Gravel::Matrix->new(x => $x, y=> $y);

		$ra_m->[$i] = $m;
		$x += $dx;
		$y += $dy;
	}

	return $ra_m;
}

#

1;

__DATA__

__C__

