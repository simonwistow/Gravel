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

my $s = {start => 0, end => 10, startx => 0, starty => 0, endx => 1000, endy => 1000};
my $e = Gravel::Effect::Tween->new($g, $s);

$m->event($e);

$m->make_timeline();

print STDERR DumperX $m;

my $a = $m->bake_movie();

print STDERR DumperX $a;



1;
