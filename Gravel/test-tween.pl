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
$m->size(0, 0, 10000, 10000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(0, 0, 0, 500, 500, 500, 500, 0, );

my $s = {start => 0, end => 10, startx => 0, starty => 0, endx => 1000, endy => 1000};
my $e = Gravel::Effect::Tween->new($g, $s);

$m->event($e);


my $g2 = Gravel::Shape->new({fills => [{colour => BLUE, type => 'solid'}]});
$g2->poly(0, 0, 0, 1000, 1000, 1000,);
my $e2 = Gravel::Effect::Tween->new($g2, $s);

#$m->event($e2);

$m->make_timeline();


my $a = $m->bake_movie();

#print STDERR DumperX $m;
#print STDERR DumperX $a;



1;
