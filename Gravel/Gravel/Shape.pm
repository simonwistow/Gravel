package Gravel::Shape;

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

    bless $self => $class;

    return $self;
}

sub to {
    my $self = shift;
    my ($x, $y) = @_;
    
    push @{$self->{_points}}, [$x, $y];
    return scalar(@{$self->{_points}});
}

# hack for now

sub set_stroke {
    my $self = shift;
#    my 

#    $self->{_styles}->{0} = 
}

# hack for now

sub set_fill {
}



1;
