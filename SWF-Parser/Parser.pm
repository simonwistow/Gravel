package SWF::Parser;

use strict;

require Exporter;
use vars qw(@ISA %EXPORT_TAGS @EXPORT_OK @EXPORT $VERSION);

@ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use SWF::ExtractText ':all';
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
  NAME => 'SWF::Parser',
  LIBS => '-L' . abs_path(cwd . '/..') . ' -lswfparse',
  INC => '-I' . abs_path(cwd . '/..'),
  OPTIMIZE => '-g';
use Inline C => Config => STRUCTS => 'swf_header';

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;
  my $self = {};
  my %conf = @_;

  $self->{HANDLER} = $conf{handler};
#  warn "HANDLER: $conf{handler}\n";
  bless($self, $class);
  return $self;
}

sub start {
#  warn "start: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->start_element({Name => $arg});
}

sub end {
#  warn "end: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->end_element({Name => $arg});
}

sub chars {
#  warn "chars: @_\n";
  my $self = shift;
  my $handler = $self->{HANDLER};
  my $arg = shift;
  $handler->characters({Data => $arg});
}

sub parsefile {
  my $self = shift;
  my $filename = shift;
  my $handler = $self->{HANDLER};

#  warn "Parsing file!\n";

  my $o = SWF::Parser->create($filename);
  $handler->start_document({});
  $self->start("swf");
  $self->start("header");

  $o->do_header($self);

  $self->end("header");
  $self->end("swf");
  my $doc = $handler->end_document({});
  $o->destroy;
  return $doc;
}


use Inline Config =>
  FORCE_BUILD => 1,
  CLEAN_AFTER_BUILD => 0;

# Preloaded methods go here.

=head1 NAME

SWF::Parser - Parse SWF files and generate SAX events

=head1 SYNOPSIS

  my $handler = XML::Handler::BuildDOM->new(KeepCDATA => 1);
  my $parser = SWF::Parser->new(handler => $handler);
  ok(ref($parser));

  my $doc = $parser->parsefile("../swfs/test5.swf");

  my $xml = $doc->toString;

  # Returns something like:
  # <swf>
  #  <header>
  #   <filename>../swfs/test5.swf</filename>
  #   <size>829</size>
  #   <version>3</version>
  #   <rate>12</rate>
  #   <count>1</count>
  #  </header>
  # </swf>

=head1 DESCRIPTION

ENODOCS

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
#include "swf_types.h"
#include "swf_parse.h"

typedef struct {
  swf_parser* parser;
} SWF_Parser;


SV* create(char* class, char* filename) {
  SV* obj_ref = newSViv(0);
  SV*     obj = newSVrv(obj_ref, class);
  SWF_Parser *s = malloc(sizeof(SWF_Parser));
  int error = SWF_ENoError;
  s->parser = swf_parse_create(filename, &error);

  sv_setiv(obj, (IV)s);
  SvREADONLY_on(obj);
  return obj_ref;
}

char* filename(SV* obj) {
  SWF_Parser* s = (SWF_Parser*)SvIV(SvRV(obj));
  return s->parser->name;
}

void do_header(SV* obj, SV* self) {
  SWF_Parser* s = (SWF_Parser*)SvIV(SvRV(obj));

  int error = SWF_ENoError;
  swf_header* header = swf_parse_header(s->parser, &error);

  my_call_method(self, "start", newSVpvf("filename"));
  my_call_method(self, "chars", newSVpvf(s->parser->name));
  my_call_method(self, "end", newSVpvf("filename"));

  my_call_method(self, "start", newSVpvf("size"));
  my_call_method(self, "chars", newSViv(header->size));
  my_call_method(self, "end", newSVpvf("size"));

  my_call_method(self, "start", newSVpvf("version"));
  my_call_method(self, "chars", newSViv(header->version));
  my_call_method(self, "end", newSVpvf("version"));

  my_call_method(self, "start", newSVpvf("rate"));
  my_call_method(self, "chars", newSViv(header->rate));
  my_call_method(self, "end", newSVpvf("rate"));

  my_call_method(self, "start", newSVpvf("count"));
  my_call_method(self, "chars", newSViv(header->count));
  my_call_method(self, "end", newSVpvf("count"));


  swf_destroy_header(header);
}

void my_call_method(SV* self, char* method, SV* arg) {

  dSP;

/* warn("calling %s(%s)\n", method, SvPV_nolen(arg)); */

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  XPUSHs(self);
  XPUSHs(arg);
  PUTBACK;

  call_method(method, G_DISCARD);

  FREETMPS;
  LEAVE;
}

void destroy(SV* obj) {
  SWF_Parser* s = (SWF_Parser*)SvIV(SvRV(obj));
  swf_destroy_parser(s->parser);
}


