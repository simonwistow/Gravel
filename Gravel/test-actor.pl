#!perl

use strict;
use warnings;

use Gravel::Constants qw(:all);
use Gravel::Movie;
use Gravel::Shape;
use Gravel::Effect;
use Gravel::Effect::Tween;
use Gravel::Effect::Spin;
use Gravel::Effect::Enlarge;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();
#my $m = Gravel::Movie->new({bgcol => '#00ffff'});

$m->name('foo.swf');
$m->size(0, 0, 10000, 10000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(0, 0, 0, 500, 500, 500, 500, 0, );

my $s = {start => 0, end => 20, depth => 2,
		 startx => 0, starty => 0, 
		 endx => 1500, endy => 3000};
my $e = Gravel::Effect::Tween->new($s);
my $a = $g->action($e);

my $th = {start_ang => 0, end_ang => 180};
my $e2 = Gravel::Effect::Spin->new($th);
$a->add($e2);

$th = {end_size => 4};
my $e3 = Gravel::Effect::Enlarge->new($th);
$a->add($e3);

#print STDERR DumperX $e2;

$m->event($a);

$m->make_timeline();

#print STDERR DumperX $m;
#print STDERR DumperX $e;

my $r = $m->bake_movie();

#print STDERR DumperX $m;
#print STDERR DumperX $r;



1;
