# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use strict;
use Test;
BEGIN { plan tests => 3 };
use SWF::Parser;
use XML::Handler::BuildDOM;
ok(1); # If we made it this far, we're ok.

#########################

# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.

my $handler = XML::Handler::BuildDOM->new(KeepCDATA => 1);
my $parser = SWF::Parser->new(handler => $handler);
ok(ref($parser));

my $doc = $parser->parsefile("../swfs/test5.swf");

warn $doc->toString;

ok(1);


