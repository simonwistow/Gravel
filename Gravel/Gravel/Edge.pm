package Gravel::Edge;

use strict;
use warnings;

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

    $self->{_EDGE_TYPE} = $conf{EDGE_TYPE} || 'STRAIGHT';

    bless $self => $class;

    if ($self->is_straight) {
		$self->{_dx} = $conf{dx} || $conf{DX} || $conf{x} || $conf{X};
		$self->{_dy} = $conf{dy} || $conf{DY} || $conf{y} || $conf{Y};
    } else {
		$self->{_cdx} = $conf{controldx} || $conf{cdx} || $conf{CDX};
		$self->{_cdy} = $conf{controldy} || $conf{cdy} || $conf{CDY};
		$self->{_adx} = $conf{anchordx} || $conf{adx} || $conf{ADX};
		$self->{_ady} = $conf{anchordy} || $conf{ady} || $conf{ADY};
    }

    return $self;
}

sub is_straight() { 
    return (shift)->{_EDGE_TYPE} eq 'STRAIGHT' ? 1 : 0;
}


1;
