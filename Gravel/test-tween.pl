#!perl

use strict;
use warnings;

use Gravel::Movie;
use Gravel::Shape;
use Gravel::Effect;
use Gravel::Effect::Tween;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();

my $g = Gravel::Shape->new();
$g->poly(400, 400, 400, 800, 800, 800, 800, 400, );

my $s = {start => 0, end => 2000, startpos => 1, endpos => 10};
my $e = Gravel::Effect::Tween->new($g, $s);

$m->event($e);

print STDERR DumperX $m;



1;
