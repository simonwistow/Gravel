#!perl

use strict;
use warnings;

use Gravel::Constants qw(:all);
use Gravel::Movie;
use Gravel::Shape;
use Gravel::Button;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();
#my $m = Gravel::Movie->new({bgcol => '#00ffff'});

$m->name('foo.swf');
$m->size(0, 0, 10000, 10000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(0, 0, 0, 500, 500, 500, 500, 0, );


my $s = {start => 1, end => 50, depth => 2, tx => 100, ty => 100};

my $bs = Gravel::ButtonState->new($g, {up => 1, down => 1});
my $bs2 = Gravel::ButtonState->new($g, {hit => 1});

my $b = Gravel::Button->new($s);

my $a = $b->make([$bs, $bs2]);

$m->event($a);

$m->make_timeline();

#print STDERR DumperX $m;
#print STDERR DumperX $e;

my $r = $m->bake_movie();

#print STDERR DumperX $m;
#print STDERR DumperX $r;



1;
