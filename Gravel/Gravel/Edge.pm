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

# TYPE should be:
# STRAIGHT
# QUADRATIC (the Flash arc) aka ARC aka CURVED
# CUBIC -- not implemented yet

    $self->{_EDGE_TYPE} = $conf{EDGE_TYPE} || $conf{TYPE} || 'STRAIGHT';

    bless $self => $class;

	$self->{_x1} = $conf{x1} || $conf{X1};
	$self->{_y1} = $conf{y1} || $conf{Y1};
	$self->{_x2} = $conf{x2} || $conf{X2};
	$self->{_y2} = $conf{y2} || $conf{Y2};

    unless ($self->is_straight) {
		$self->{_ax} = $conf{ax} || $conf{AX};
		$self->{_ay} = $conf{ay} || $conf{AY};
    }

    return $self;
}

sub is_straight() {  return (shift)->{_EDGE_TYPE} eq 'STRAIGHT' ? 1 : 0; }
sub type() {  return (shift)->{_EDGE_TYPE}; }

1;
