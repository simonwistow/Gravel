#!/usr/bin/perl -w

# convert dos line endings to Unix ones

use strict;

use IO::File;

# my $file = $ARGV[0];



unless (@ARGV)
{
	my $name = $0;
	$name =~ s!^.*/!!;

	print STDERR "usage : $name [filename(s)]\n";

	exit 1;
}

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
