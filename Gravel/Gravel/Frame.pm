package Gravel::Frame;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

# FIXME

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

#	$self->{_contents} = {};
	$self->{_shape} = $conf{shape};
	$self->{_depth} = $conf{depth};
	$self->{_matrix} = $conf{matrix};

    bless $self => $class;

    return $self;
}

sub contents { return (shift)->{_contents}; }

sub shape {
	my $self = shift;
	$self->{_contents}->{shape} = shift;
}

sub place {
	my $self = shift;
	$self->{_contents}->{x} = shift;
	$self->{_contents}->{y} = shift;
}

#



#

1;


