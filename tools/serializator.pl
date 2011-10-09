#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Long;

my %TYPEMAP = (
  uint32_t => { name => 'uint32_t', typ => 'l', sz => 4 },
  uint64_t => { name => 'uint64_t', typ => 'L', sz => 8 },
);

my %O = (
  c_file  => undef,
  h_file  => undef,
  verbose => 0,
);

GetOptions(
  'c:s'     => \$O{c_file},
  'h:s'     => \$O{h_file},
  'verbose' => \$O{verbose},
) or usage();

my @files = ( @ARGV, @O{ 'c_file', 'h_file' } );
3 == grep defined, @files or usage();
serializator( @files );

sub serializator {
  my ( $hdr, $cf, $hf ) = @_;
  open my $fh, '<', $hdr or die "Can't read $hdr: $!\n";
  open my $ch, '>', $cf  or die "Can't write $cf: $!\n";
  open my $hh, '>', $hf  or die "Can't write $hf: $!\n";

  for ( [ $cf, $ch ], [ $hf, $hh ] ) {
    my ( $n, $h ) = @$_;
    print $h "/* $n - GENERATED FILE: DO NOT EDIT\n",
     " * generated from $hdr by $0\n",
     " */\n";
  }

  my $ser    = 0;
  my @struct = ();

  while ( <$fh> ) {
    chomp( my $line = $_ );
    if ( $ser ) {
      $line =~ s{/\*.+?\*/}{}g;
      $line =~ s{^\s+}{}g;
      $line =~ s{\s+$}{}g;
      if ( $line =~ m/^}\s+(\w+)\s*;$/ ) {
        my $def = parse_struct( $1, @struct );
        @struct = ();
        print $_ "\n/* $def->{name} */\n" for $ch, $hh;
        print $hh ser_defs( $def );
        print $ch ser_code( $def );
        $ser = 0;
      }
      else {
        push @struct, $line unless $line =~ m{^$};
      }
    }
    $ser = 1 if $line =~ m{^\s*/\*\s*SERIALIZE\s*\*/\s*$};
  }
}

sub parse_struct {
  my ( $name, undef, @struct ) = @_;
  my @fld;
  my $typ = '';
  my $sz  = 0;
  for my $ln ( @struct ) {
    die "Can't parse $ln\n" unless $ln =~ /^(\w+)\s+(\w+)\s*;$/;
    my ( $t, $n ) = ( $1, $2 );
    my $tm = $TYPEMAP{$t} or die "No match for type $t\n";
    $typ .= $tm->{typ};
    $sz += $tm->{sz};
    if ( $tm->{fld} ) {
      push @fld, map "$n.$_", @{ $tm->{fld} };
    }
    else {
      push @fld, $n;
    }
  }
  my $rec = {
    name => $name,
    typ  => $typ,
    sz   => $sz,
    fld  => \@fld,
  };
  $TYPEMAP{$name} = $rec;
  return $rec;
}

sub ser_defs {
  my $rec = shift;

  my $n   = $rec->{name};
  my $typ = $rec->{typ};
  my $sz  = $rec->{sz};

  my $mptr = join ', ', map "MPTR($_)", @{ $rec->{fld} };
  my $memb = join ', ', map "MEMB($_)", @{ $rec->{fld} };

  return
     "#define ${n}_SPEC \"$typ\"\n"
   . "#define ${n}_MPTR $mptr\n"
   . "#define ${n}_MEMB $memb\n"
   . "#define ${n}_SIZE $sz\n";

}

sub ser_code {
  my $rec = shift;
  my $n   = $rec->{name};

  return
     "rfile_bits_READER( ${n}_reader, $n );\n"
   . "rfile_bits_WRITER( ${n}_writer, $n );\n"
   . "rfile_bits_PIDDLE( ${n}_piddle, $n );\n"
   . "rfile_bits_GUZZLE( ${n}_guzzle, $n );\n";
}

sub usage {
  die "Usage: serializator.pl -c file.c -h file.h [-v] file.h.h\n";
}

# vim:ts=2:sw=2:sts=2:et:ft=perl

