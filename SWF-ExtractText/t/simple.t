# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use strict;
use Test;
BEGIN { plan tests => 4 };
use SWF::ExtractText;
ok(1); # If we made it this far, we're ok.

#########################

# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.

my $swf = SWF::ExtractText->new("../swfs/turkstud.swf");
ok(ref($swf));

my @strings = $swf->strings;
my @urls = $swf->urls;
ok(scalar(@strings), 8);
ok(scalar(@urls), 0);

#foreach my $s ($swf->strings) { warn "string: $s\n"; }
#foreach my $u ($swf->urls) { warn "   url: $u\n"; }


