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

    $self->{_stang} = $conf{start_ang} * 2 * 3.142 / 360;
    $self->{_endang} = $conf{end_ang} * 2 * 3.142 / 360;

    return $self;
}

#

sub matrices {
	my $self = shift;

	unless (defined $self->{_start} and defined $self->{_end}) {
		return undef;
	}
	
	my $ra_m = [];

	my $numf = $self->{_end} - $self->{_start};
	my $dth = ( $self->{_endang} - $self->{_stang} ) / $numf;

	my $theta = $self->{_stang};
	for (my $i = $self->{_start}; $i < $self->{_end}; ++$i) {

		my $cos = cos $theta;
		my $sin = sin $theta;

		my $m = Gravel::Matrix->new(a => $cos, b => -$sin,
									c => $sin, d => $cos );

		$theta += $dth;
		$ra_m->[$i] = $m;
	}

	return $ra_m;
}


#

#

1;

__DATA__

__C__

