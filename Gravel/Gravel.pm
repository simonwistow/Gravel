package Gravel;

use 5.006;
use strict;
use warnings;

require Exporter;
use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Gravel ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);
our $VERSION = '0.01';


# Preloaded methods go here.

sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self = {};
    my %conf = @_;

    $self->{_points} = $conf{_points} || [];
    $self->{_styles} = $conf{_styles} || {};

    bless $self => $class;

    return $self;
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

Gravel - Perl extension for the gravel SWF libraries

=head1 SYNOPSIS

  use Gravel;
  blah blah blah

=head1 DESCRIPTION

Some description.

Blah blah blah.

=head2 EXPORT

None by default.


=head1 AUTHOR

Ben Evans, E<lt>kitty@cpan.org<gt>

=head1 SEE ALSO

L<perl>.

=cut
