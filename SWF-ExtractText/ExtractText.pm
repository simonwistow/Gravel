package SWF::ExtractText;

use strict;
use Tie::Hash;

require Exporter;
use vars qw(@ISA %EXPORT_TAGS @EXPORT_OK @EXPORT $VERSION);

@ISA = qw(Exporter Tie::Hash);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Tie::GHash ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
%EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

@EXPORT = qw(
	
);
$VERSION = '0.10';

use Cwd qw(cwd abs_path);
use Inline C => 'DATA',
  VERSION => '0.10',
  NAME => 'SWF::ExtractText',
  LIBS => '-L' . abs_path(cwd . '/..') . ' -llib_swfextract',
  INC => '-I' . abs_path(cwd . '/..');

use Inline Config =>
  FORCE_BUILD => 1,
  CLEAN_AFTER_BUILD => 0;

# Preloaded methods go here.

=head1 NAME

SWF::ExtractText - Extract text and URLs from SWF files

=head1 SYNOPSIS

  my $swf = SWF::ExtractText->new("turkstud.swf");
  my @strings = $swf->strings;
  my @urls = $swf->urls;

=head1 DESCRIPTION

This module extracts the strings and URLs from a SWF flash file. It
uses the lib_swfextract library from the gravel libswfparse project
(available from http://sourceforge.net/projects/gravel/).

=head1 BUGS

It does not quite work yet.

=head1 AUTHOR

Leon Brocard E<lt>F<acme@astray.com>E<gt>

=head1 COPYRIGHT

Copyright (C) 2001, Leon Brocard

This module is free software; you can redistribute it or modify it
under the same terms as Perl itself.

=cut

1;

__DATA__

__C__
#include "lib_swfextract.h"

SV* new(char* class, char* filename) {
  SV* obj_ref = newSViv(0);
  SV*     obj = newSVrv(obj_ref, class);
  swf_extractor* swf;
  int error = SWF_ENoError;

  printf("Loading %s...\n", filename);
  swf = load_swf(filename, &error);
  printf("Loaded!\n");

  sv_setiv(obj, (IV)swf);
  SvREADONLY_on(obj);
  return obj_ref;
}

void DESTROY(SV* obj) {
  swf_extractor* swf = (swf_extractor*)SvIV(SvRV(obj));
  destroy_swf(swf);
}

void strings(SV * obj) {
  int num, i;
  char** strings;
  Inline_Stack_Vars;

  swf_extractor* swf = (swf_extractor*)SvIV(SvRV(obj));

  Inline_Stack_Reset;

  num = get_number_strings(swf);
  strings = get_strings(swf);

  for (i = 0; i < num; i++) {
    printf("  string: %s\n", strings[i]);
    Inline_Stack_Push(newSVpv(strings[i], PL_na));
  }

  Inline_Stack_Done;
}

void urls(SV * obj) {
  int num, i;
  char** urls;
  Inline_Stack_Vars;

  swf_extractor* swf = (swf_extractor*)SvIV(SvRV(obj));

  Inline_Stack_Reset;

  num = get_number_urls(swf);
  urls = get_urls(swf);

  for (i = 0; i < num; i++) {
    printf("  url: %s\n", urls[i]);
    Inline_Stack_Push(newSVpv(urls[i], PL_na));
  }

  Inline_Stack_Done;
}







__END__
