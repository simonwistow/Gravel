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

    # The first argument to an Effect constructor is a shape.
    $self->{_shape} = shift;

    bless $self => $class;

    return $self;
}

#

sub shape {return (shift)->{_shape};}

#


#

1;

__DATA__

__C__

