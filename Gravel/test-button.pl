#!perl

use strict;
use warnings;

use Gravel::Constants qw(:all);
use Gravel::Movie;
use Gravel::Matrix;
use Gravel::Shape;
use Gravel::Button;
use Gravel::ButtonState;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();
#my $m = Gravel::Movie->new({bgcol => '#00ffff'});

$m->name('foo.swf');
$m->size(0, 0, 20000, 20000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(0, 0, 0, 500, 500, 500, 500, 0, );
my $g2 = Gravel::Shape->new({fills => [{colour => BLUE, type => 'solid'}]});
$g2->poly(0, 0, 0, 500, 500, 500, );
my $g3 = Gravel::Shape->new({fills => [{colour => GREEN, type => 'solid'}]});
$g3->poly(0, 0, 0, 500, 500, 500, );
my $g4 = Gravel::Shape->new({fills => [{colour => HAZY_RED,type => 'solid'}]});
$g4->poly(0, 0, 500, 0, 500, 500, );

my $mx = Gravel::Matrix->new({a => 1.8, d => 2, });

my $s = {start => 1, end => 2, depth => 2, tx => 2000, ty => 2000};

my $bs = Gravel::ButtonState->new($g, {down => 1, layer => 3});
#my $bs2 = Gravel::ButtonState->new($g2, {up => 1, matrix => $mx, layer => 3});
my $bs2 = Gravel::ButtonState->new($g2, {over => 1, layer => 3, });
my $bs3 = Gravel::ButtonState->new($g3, {down => 1, layer => 2});
my $bs4 = Gravel::ButtonState->new($g4, {up => 1, layer => 2});

my $b = Gravel::Button->new($s);

my $a = $b->make([$bs, $bs2, $bs3, $bs4]);
#my $a = $b->make([$bs, $bs2, $bs4]);

$m->event($a);

$m->make_timeline();

#print STDERR DumperX $m;
#print STDERR DumperX $e;

my $r = $m->bake_movie();

#print STDERR DumperX $m;
#print STDERR DumperX $r;



1;
