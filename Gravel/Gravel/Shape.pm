package Gravel::Shape;

use strict;
use warnings;

use lib '../';

use Gravel::Edge;

use Data::Dumper qw/DumperX/;

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

	$self->{_edges} = [];
	$self->{_vertices} = [];

    bless $self => $class;

    return $self;
}

sub last_vertex {
    my $self = shift;

	my $last = scalar(@{$self->{_vertices}}) - 1;
	return $self->{_vertices}->[$last];
}

#

sub line {
    my $self = shift;
    my ($x1, $y1, $x2, $y2) = @_;
    
	my $e = Gravel::Edge->new({x1 => $x1, y1 => $y1, x2 => $x2, y2 => $y2});
    push @{$self->{_edges}}, $e;

	my $ra_v = $self->last_vertex();

	unless ( ($ra_v->[0] == $x1) && ($ra_v->[1] == $y1) ) {
		push @{$self->{_vertices}}, [$x1, $y1];
	}
	push @{$self->{_vertices}}, [$x2, $y2];

    return scalar(@{$self->{_edges}});
}

#

sub line_to {
    my $self = shift;
    my ($x, $y) = @_;

	my $ra_v = $self->last_vertex();
    
	my $e = Gravel::Edge->new({x1 => $ra_v->[0], y1 => $ra_v->[1], x2 => $x, y2 => $y});
    push @{$self->{_edges}}, $e;
	push @{$self->{_vertices}}, [$x, $y];

    return scalar(@{$self->{_edges}});
}

#

sub arc {
    my $self = shift;
	my %p;
	@p{'x1', 'y1', 'x2', 'y2', 'ax', 'ay'} = @_;
    
	$p{TYPE} = 'QUADRATIC';

	my $e = Gravel::Edge->new(\%p);

    push @{$self->{_edges}}, $e;

	my $ra_v = $self->last_vertex();

	if (defined $ra_v) {
		unless ( ($ra_v->[0] == $p{x1}) && ($ra_v->[1] == $p{y1}) ) {
			push @{$self->{_vertices}}, [$p{x1}, $p{y1}];
		}
	} else {
		push @{$self->{_vertices}}, [$p{x1}, $p{y1}];
	}
	push @{$self->{_vertices}}, [$p{x2}, $p{y2}];

    return scalar(@{$self->{_edges}});
}

#

sub poly {
    my $self = shift;
    while (my $x = shift) {
		my $y = shift;

		my $e = Gravel::Edge->new({x => $x, y => $y});
		push @{$self->{_edges}}, $e;
		push @{$self->{_vertices}}, [$x, $y];
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
