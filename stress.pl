#!/usr/bin/perl -w

use strict;
use IPC::Run qw(run timeout timer);
use BSD::Resource;

setrlimit(RLIMIT_DATA, 50 * 1024 * 1024, 50 * 1024 * 1024) || die "failed to set limit";

$| = 1;

my $time = localtime;
print qq|<html>
<head><title>libswfparse stress tester</title></head>
<body bgcolor="#ffaa00">
<font color="white" size="5" face="Geneva,Arial,Helvetica,sans-serif">libswfparse stress tester</font><br>
<font color="white" size="4" face="Geneva,Arial,Helvetica,sans-serif">$time</font>
<p>
|;

my $passed;
my $failed;

foreach my $swf (<swfs/*.swf>) {
  print qq|\n<a href="$swf">$swf</a>: |;
  my $stdout;
  my $stderr;
  my $t = timer(10, name => "hang timer");
  run ["/home/simon/gravel/libswfparse/text_extract", $swf], \undef, \$stdout, \$stderr, $t;
  my $reason = "";
  $reason = "Perl error: $!" if $!;
  $reason = "Error code: $?" if $?;
  $reason = "Timed out" if $t->is_expired;
  $reason = "Core dumped" if $? & 128;  

warn "$swf $reason\n";

  unless ($reason) {
	$passed++;
	print qq|<font color="green">OK</font><br>|;
	next;
	}
  $failed++;
  open(OUT, "> report/$failed.html") || die "couldn't open 'report/$failed.html' :  $!\n";
  print OUT qq|<html>
<head><title>libswfparse stress tester</title></head>
<body bgcolor="#ffaa00">
<font color="white" size="5" face="Geneva,Arial,Helvetica,sans-serif">libswfparse stress tester</font>
<p>
<h3><a href="../$swf">$swf</a></h3>
|;

  print qq|<font color="red"><b>NOT OK: $reason</b></font> <a href="report/$failed.html">Read report</a><br>|;	
  print OUT qq|<font color="red"><b>NOT OK: $reason</b></font><p>|;	
  print OUT "Returned: $?<p>";
  print OUT "Core dumped!<p>" if $? & 128;
  print OUT "Timed out!<p>" if $stdout =~ /hang timer/;
  print OUT "STDERR: <pre>$stderr</pre><p>";
  print OUT "STDOUT: <pre>$stdout</pre><p>";

  print OUT "</body></html>";
  close(OUT);
#  last;
}

print qq|
<p>
Total passed: $passed<br>
Total failed: $failed<br>
|;
print "Run at: $time";
print "</body></html>";
