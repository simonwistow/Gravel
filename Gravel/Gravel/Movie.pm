package Gravel::Movie;

use strict;
use warnings;

use lib '../';

use Gravel::Shape;
use Gravel::Effect;

use Data::Dumper qw/DumperX/;

our $VERSION = '0.10';

use Cwd qw(cwd abs_path);
use Inline C => 'DATA',
  VERSION => '0.10',
  NAME => 'Gravel::Movie',
  LIBS => '-L' . abs_path(cwd . '/../../') . ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/../../'),
  OPTIMIZE => '-g';
use Inline C => Config => STRUCTS => 'swf_header';

#

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

    $self->{_library} = [];

    bless $self => $class;

    return $self;
}

#

#

1;

__DATA__

__C__

