package Gravel::Shape;

use strict;
use warnings;

use lib '../';

use Gravel::Edge;

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

	$self->{_start}->{x} = {$conf{start}}->{x} || 0;
	$self->{_start}->{y} = {$conf{start}}->{y} || 0;

    bless $self => $class;

    return $self;
}

sub to {
    my $self = shift;
    my ($x, $y) = @_;
    
	my $e = Gravel::Edge->new({x => $x, y => $y});

    push @{$self->{_edges}}, $e;
    return scalar(@{$self->{_edges}});
}

sub poly {
    my $self = shift;
    while (my $x = shift) {
		my $y = shift;

		my $e = Gravel::Edge->new({x => $x, y => $y});
		push @{$self->{_edges}}, $e;
	}
	my $e = Gravel::Edge->new({x => $self->{_start}->{x}, y => $self->{_start}->{y}, });
	push @{$self->{_edges}}, $e;

    return scalar(@{$self->{_edges}});
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
