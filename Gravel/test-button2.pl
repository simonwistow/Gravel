#!perl

use strict;
use warnings;

use Gravel::Constants qw(:all);
use Gravel::Movie;
use Gravel::Matrix;
use Gravel::Shape;
use Gravel::ButtonTest;
#use Gravel::ButtonState;

use Data::Dumper qw/DumperX/;

my $m = Gravel::Movie->new();
#my $m = Gravel::Movie->new({bgcol => '#00ffff'});

$m->name('foo.swf');
#$m->size(0, 0, 16000, 16000);
$m->size(0, 0, 20000, 20000);

my $g = Gravel::Shape->new({fills => [{colour => HAZY_RED, type => 'solid'}]});
$g->poly(1000, 1000, 1000, 3000, 3000, 3000, 3000, 1000, );

my $g2 = Gravel::Shape->new({fills => [{colour => BLUE, type => 'solid'}]});
$g2->poly(1000, 1000, 1000, 3000, 3000, 3000,);

my $mx = Gravel::Matrix->new({a => 1, d => 1, });

my $s = {start => 0, end => 5, depth => 2, tx => 100, ty => 100, shape => $g};
my $b = Gravel::ButtonTest->new($s);

my $a = $b->make();

$m->event($a);
$m->make_timeline();

print STDERR DumperX $m;
#print STDERR DumperX $e;

my $r = $m->bake_movie();

#print STDERR DumperX $m;
#print STDERR DumperX $r;



1;
