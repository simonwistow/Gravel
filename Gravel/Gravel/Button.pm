package Gravel::Button;

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

