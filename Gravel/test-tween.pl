#!perl

use strict;
use warnings;

use Gravel::Constants qw(:all);
use Gravel::Movie;
use Gravel::Shape;
use Gravel::Effect;
use Gravel::Effect::Tween;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();
$m->name('foo.swf');
$m->size(0, 0, 20000, 20000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(10, 10, 10, 4010, 4010, 4010, 4010, 10, );

my $s = {start => 0, end => 10, startx => 0, starty => 0, endx => 1000, endy => 1000};
my $e = Gravel::Effect::Tween->new($g, $s);

$m->event($e);

$m->make_timeline();

print STDERR DumperX $g;

my $a = $m->bake_movie();

print STDERR DumperX $a;



1;
