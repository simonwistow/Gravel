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

	$self->{_button} = $conf{button};
	$self->{_effects} = [$conf{effect}];

	unless ($self->{_button}) {
		$self->{_shape} = $conf{shape};
	}

	$self->{_current} = 0;
	
	$self->{_start} = $conf{start};
	$self->{_end} = $conf{end};

    bless $self => $class;

    return $self;
}

#

sub add {
	my $self = shift;
	my $e = shift;

	if (defined $e->start) { 
		$self->{_start} = undef;		
	} else {
		$e->start($self->{_start});
	}

	if (defined $e->end) {
		$self->{_end} = undef;
	} else {
		$e->end($self->{_end});
	}

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

sub merge_matrix {
	my $self = shift;
	my $ra_m = shift;
	my $j = shift;

	my $m = Gravel::Matrix->new();
	my $num_fx = scalar(@{$self->{_effects}});

	for (my $i = 0; $i < $num_fx; ++$i) {
		$m->combine($ra_m->[$i]->[$j]) if defined $ra_m->[$i]->[$j];
	}

	return $m;
}


# FIXME: Multiple effects
sub frames {
	my $self = shift;

	my $m = [];
	my $ra_m = [];

	my $num_fx = scalar(@{$self->{_effects}});

	return $self->frames_single if $num_fx == 1;

	for (my $i = 0; $i < $num_fx; ++$i ) {
		$ra_m->[$i] = $self->{_effects}->[$i]->matrices();
	}

	my $start = $self->start;
	my $end = $self->end;

	# FIXME: Depth merging...
	my $depth = $self->{_effects}->[0]->depth;

#	print STDERR DumperX $ra_m;

	my $s;
	if ($self->{_shape}) {
		$s = {shape => $self->{_shape}, };
	} elsif ($self->{_button}) {
		$s = {button => $self->{_button}, };
	} else {
		$s = {};
	}

	for (my $j = $start; $j < $end; ++$j ) {
		my $mx = $self->merge_matrix($ra_m, $j);
		my %s = ((matrix => $mx, depth => $depth), %$s);
		my $f = Gravel::Frame->new(\%s);

		$m->[$j] = $f;
	}

	return $m;
}

#

sub frames_single {
	my $self = shift;

	my $m = [];

	my $s;
	if ($self->{_shape}) {
		$s = {shape => $self->{_shape}, };
	} elsif ($self->{_button}) {
		$s = {button => $self->{_button}, };
	} else {
		$s = {};
	}

	my $ra_m = $self->{_effects}->[0]->matrices();
	my $start = $self->{_effects}->[0]->start;
	my $end = $self->{_effects}->[0]->end;
	my $depth = $self->{_effects}->[0]->depth;
	for (my $i = $start; $i < $end; ++$i ) {
		my $mx = $ra_m->[$i];
		my %s = ((matrix => $mx, depth => $depth), %$s);
		my $f = Gravel::Frame->new(\%s);

		$m->[$i] = $f;
	}

	return $m;
}

#

sub contents {
	my $self = shift;
	return $self->{_shape} || $self->{_button};
}

#

sub shape   {return (shift)->{_shape};}
sub button  {return (shift)->{_button};}
sub effects {return (shift)->{_effects};}

#

1;

__DATA__

__C__

