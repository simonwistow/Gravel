#!/usr/bin/perl -w

use strict;

use IO::File;

# my $file = $ARGV[0];

foreach my $file (@ARGV) {

    my ($foo);
    
    my $fh = IO::File->new();
    if ($fh->open("< $file")) {
	
	{
	    local $/ = undef;
	    $foo = <$fh>;
	    
	}
	
	$fh->close();
    }

    $foo =~ s/\r\n/\n/g;

    my $old_file = $file;

    $file .= ".new";
        
    $fh = new IO::File;
    if ($fh->open("> $file")) {
	print $fh $foo;

	$fh->close();
    }

#    unlink($old_file);
    rename($file, $old_file);
}

1;
