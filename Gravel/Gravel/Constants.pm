package Gravel::Constants;

require Exporter;

use strict;
use warnings;

use base qw( Exporter );

use constant BLACK => '#000000';
use constant RED   => '#ff0000';
use constant GREEN => '#00ff00';
use constant BLUE  => '#0000ff';
use constant WHITE => '#ffffff';

use constant HAZY_RED   => '#ff0000cc';

our @EXPORT_OK   =   qw( BLACK RED GREEN BLUE WHITE HAZY_RED);
our %EXPORT_TAGS = (
    'all'      => [ @EXPORT_OK ],
);



1;

