package Gravel::ButtonState;

use strict;
use warnings;

use lib '../';

use Gravel::Effect;
use Gravel::Effect::Tween;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';


sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
	my $shape = shift;

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

	$self->{_hit} = $conf{hit};
    $self->{_up} = $conf{up};
    $self->{_down} = $conf{down} || 1;

	# FIXME
	$self->{_shape} = $shape;

	bless $self => $class;

	return $self;
}

#

sub shape { return (shift)->{_shape}; }

#


1;

