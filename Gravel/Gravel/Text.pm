package Gravel::Text;

use strict;
use warnings;

use lib '../../';

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

# FIXME: 

sub new {
    my $class = shift;

    my $cf = ref($_[0]);
    my %conf;
    
    if ($cf eq 'HASH') {
		%conf = %{$_[0]};
    } else {
		%conf = @_;
    }

    return $self;
}

#

1;

__DATA__

__C__
