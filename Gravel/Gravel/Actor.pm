package Gravel::Actor;

use strict;
use warnings;

use lib '../';

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

#use Cwd qw(cwd abs_path);
#use Inline C => 'DATA',
#  VERSION => '0.10',
#  NAME => 'Gravel::Effect',
#  LIBS => '-L' . abs_path(cwd . '/../../') . ' -lswfparse',
#  INC => '-I' . abs_path(cwd . '/../../'),
#  OPTIMIZE => '-g';
#use Inline C => Config => STRUCTS => 'swf_header';

# FIXME

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

	$self->{_current} = 0;
	$self->{_shape} = $conf{shape};
	$self->{_effects} = [$conf{effect}];

    bless $self => $class;

    return $self;
}

#

sub add {
	my $self = shift;
	my $e = shift;
	$self->{_start} = undef;
	$self->{_end} = undef;
	push @{$self->{_effects}}, $e;
}


#

sub start {
	my $self = shift;
	return $self->{_start} if $self->{_start};

	my $start;

	foreach my $e (@{$self->{_effects}}) {
		my $st2 = $e->start;
		$start = $st2 unless defined $start;
		$start = $st2 if $start > $st2;
	}

	$self->{_start} = $start;
}

sub end {
	my $self = shift;
	return $self->{_end} if $self->{_end};

	my $end;

	foreach my $e (@{$self->{_effects}}) {
		my $en2 = $e->end;
		$end = $en2 unless defined $end;
		$end = $en2 if $en2 < $en2;
	}

	$self->{_end} = $end;
}

#

# FIXME: Multiple effects
sub frames {
	my $self = shift;
	return $self->{_effects}->[0]->frames($self->{_shape});
}

#

sub shape   {return (shift)->{_shape};}
sub effects {return (shift)->{_effects};}

#

1;

__DATA__

__C__

