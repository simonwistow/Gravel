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

sub make {
	my $self = shift;
	my $g = shift;
	
	my $s = {start => $self->{_start}, end => $self->{_end}, 
			 startx => $self->{_tx}, endx => $self->{_tx},
			 starty => $self->{_ty}, endy => $self->{_ty},
		 };

	my $e = Gravel::Effect::Tween->new($s);
	return $g->action($e);
}

#


1;

