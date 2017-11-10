##
#     PJL Script Library
#     config/makedepend.pl
#
#     Copyright (C) 1996-2013  Paul J. Lucas
#
#     This program is free software; you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program; if not, write to the Free Software
#     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

########## You shouldn't have to change anything below this line. #############

use File::Basename;

$ME = basename( $0 );                   # basename of executable

sub usage {
  die "usage: $ME [-I dir]... file\n";
}

##
# Go though all command-line arguments and capture -I options; ignore all
# others; stop when we encounter a non-option.
##
my $arg_is_next;
my @dirs = ( '.' );
while ( defined( $_ = shift( @ARGV ) ) ) {
  /(.+)/ && $arg_is_next && do {
    push( @dirs, $1 );
    $arg_is_next = 0;
    next;
  };
  /^[^-]/ && do {
    unshift( @ARGV, $_ );
    last;
  };
  /^-I(.*)/ && do {
    if ( $1 ) {
      push( @dirs, $1 );
    } else {
      $arg_is_next = 1;
    }
    next;
  };
}

$#ARGV + 1 == 1 or usage();

( $SOURCE_FILE ) = @ARGV;
push( @files, $SOURCE_FILE );           # prime the pump

map { s!/$!!; } @dirs;                  # get rid of trailing /'s
map { $dir_set{ $_ } = 1; } @dirs;

##
# Process all files.
##
while ( my $file = shift( @files ) ) {
  ##
  # See if the current file contains a path in a subdirectory, e.g.:
  #
  #   #include "sub/dir/foo.h"
  #
  # If so, add the subdirectory to the list/set of directories.
  ##
  my $file_dir = dirname( $file );
  unless ( exists $dir_set{ $file_dir } ) {
    $dir_set{ $file_dir } = 1;
    push( @dirs2, $file_dir );
  }

  ##
  # Look in all directories in @dirs for the current file.
  ##
  my $found_dir;
  for my $dir ( @dirs ) {
    my $path = "$dir/$file";
    if ( open( SOURCE, $path ) ) {
      $found_dir = $dir;
      goto FOUND;
    }
    for my $dir2 ( @dirs2 ) {
      $path = "$dir/$dir2/$file";
      if ( open( SOURCE, $path ) ) {
        $found_dir = "$dir/$dir2";
        goto FOUND;
      }
    }
    next;
FOUND:    $dep_set{ $path } = 1;
    last;
  }
  die "$ME: error: can not open $file\n" unless $found_dir;

  ##
  # Pluck files #include'd and add them to the list of files to process only if
  # we haven't seen them before.
  ##
  while ( <SOURCE> ) {
    next unless /^#\s*include\s+"([^"]+)"/;
    push( @files, $1 ) unless exists $dep_set{ "$found_dir/$1" };
  }
  close( SOURCE );
}

##
# Print the file and its dependencies.
##
( $OBJECT_FILE = $SOURCE_FILE ) =~ s/\.\w+$/\.o/;
print "$OBJECT_FILE : ", join( ' ', keys %dep_set ), "\n";

# vim:set et sw=2 ts=2:
