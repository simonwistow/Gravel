package Gravel::Effect;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;

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
    my $self = {};

    bless $self => $class;

    return $self;
}

#

# start and end refer to start and end frames

sub start {
	my $self = shift;
	my $v = shift;
	return $self->{_start} unless defined $v;
	return $self->{_start} = $v;
}

sub end {
	my $self = shift;
	my $v = shift;
	return $self->{_end} unless defined $v;
	return $self->{_end} = $v;
}


sub depth { return (shift)->{_depth}; }

#


#

1;

__DATA__

__C__

