package Gravel::Effect::Tween;

use strict;
use warnings;

use lib '../../';

use Gravel::Shape;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

# FIXME

sub new {
    my $class = shift;
    my $g = shift;

    my $self = $class->SUPER::new($g, @_);

    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
	%conf = %{$_[0]};
    } else {
	%conf = @_;
    }

    $self->{_start} = $conf{start};
    $self->{_end} = $conf{end};

    $self->{_startpos} = $self->{startpos};
    $self->{_endpos} = $self->{endpos};

    return $self;
}

#



#



#

1;

__DATA__

__C__

