package Gravel::Effect::Spin;

use strict;
use warnings;

use lib '../../';

use Gravel::Shape;
use Gravel::Effect;
use Gravel::Frame;

use base qw(Gravel::Effect);

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

# FIXME: 

sub new {
    my $class = shift;
    my $g = shift;

    my $self = $class->SUPER::new($g, @_);

    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		%conf = @_;
    }

    $self->{_start} = $conf{start};
    $self->{_end} = $conf{end};

    $self->{_stang} = $conf{start_ang};
    $self->{_endang} = $conf{end_ang};

    return $self;
}

#


sub frames {
	my $self = shift;
	
	my $ra_f = [];

	my $numf = $self->{_end} - $self->{_start};
	my ($theta0, $theta1) = ($self->{_stang}, $self->{_endang});

	my $dth = ( $self->{_endang} - $self->{_stang} ) / $numf;

	for (my $i = $self->{_start}; $i < $self->{_end}; ++$i) {
		my $f = Gravel::Frame->new();

		# This needs much fixing....
		$f->shape($self->shape);
#		$f->place($x, $y);

		$ra_f->[$i] = $f;

	}

	return $ra_f;
}


#



#

1;

__DATA__

__C__

