package Gravel::Effect::Enlarge;

use strict;
use warnings;

use lib '../../';

use Gravel::Shape;
use Gravel::Effect;
use Gravel::Frame;

use base qw(Gravel::Effect);

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

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

    $self->{_stsize} = $conf{start_size} || 1;
    $self->{_endsize} = $conf{end_size};

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
	my $ds = ( $self->{_endsize} - $self->{_stsize} ) / $numf;

	my $lambda = $self->{_stsize};
	for (my $i = $self->{_start}; $i < $self->{_end}; ++$i) {

		my $m = Gravel::Matrix->new(a => $lambda, d => $lambda);

		$lambda += $ds;
		$ra_m->[$i] = $m;
	}

	return $ra_m;
}


#

#

1;

__DATA__

__C__

