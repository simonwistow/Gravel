#!/usr/bin/perl -w

use strict;
use lib 'blib/arch';
use lib 'blib/lib';
use SWF::Parser;
use XML::Handler::YAWriter;

my $ya = XML::Handler::YAWriter->new(
  AsFile => "-",
  'Pretty' => {
    PrettyWhiteIndent => 1,
    PrettyWhiteNewline => 1,
  });

my $parser = SWF::Parser->new(handler => $ya);
$parser->parsefile(shift || die("No file!\n"));

