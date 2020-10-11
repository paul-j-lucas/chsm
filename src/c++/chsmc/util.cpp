/*
**      CHSM Language System
**      src/c++/chsmc/util.cpp
**
**      Copyright (C) 2018  Paul J. Lucas
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file
 * Defines utility functions.
 */

// local
#include "config.h"                     /* must go first */
#include "util.h"

/// @cond DOXYGEN_IGNORE

// standard
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <memory>

/// @endcond

using namespace std;
namespace fs = std::filesystem;

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

char const* base_name( char const *path ) {
  assert( path != nullptr );
  if ( char const *const slash = std::strrchr( path, '/' ) )
    return slash[1] != '\0' ? slash + 1 : slash;
  return path;
}

bool ends_with( string const &s, string const &suffix ) {
  return  s.size() >= suffix.size() &&
          s.compare( s.size() - suffix.size(), suffix.size(), suffix ) == 0;
}

string identify( string const &s ) {
  string result;
  bool underscore = false;
  for ( auto c : s ) {
    if ( isalnum( c ) || c == '_' ) {
      result += c;
      underscore = false;
    }
    else if ( !underscore ) {
      result += '_';
      underscore = true;
    }
  } // for
  return result;
}

char const* ltoa( long n ) {
  size_t const BUF_SIZE = 20;
  size_t const NUM_BUFS = 10;

  static char buf[ NUM_BUFS ][ BUF_SIZE ];
  static size_t b;

  //
  // See: Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language,
  // 2nd ed."  Addison-Wesley, Reading, MA, 1988.  pp. 63-64.
  //

  char *s = buf[ b ];
  bool const is_neg = n < 0;

  if ( is_neg )
    n = -n;

  do {                                  // generate digits in reverse
    *s++ = n % 10 + '0';
  } while ( n /= 10 );

  if ( is_neg )
    *s++ = '-';
  *s = '\0';

  std::reverse( buf[ b ], s );
  s = buf[ b ];
  b = (b + 1) % NUM_BUFS;

  return s;
}

char opening_char( char c ) {
  switch ( c ) {
    case ')': return '(' ;
    case '>': return '<' ;
    case ']': return '[' ;
    case '}': return '{' ;
    default : return '\0';
  } // switch
}

void perror_exit( int status ) {
  //perror( me );
  ::exit( status );
}

fs::path temp_path( char const *pattern ) {
  string const pattern_str{ pattern };
  assert( ends_with( pattern_str, "XXXXXX" ) );

  fs::path path{ fs::temp_directory_path() };
  path /= pattern_str;

  unique_ptr<char[]> const mktemp_buf{ new char[ path.native().size() + 1 ] };
  ::strcpy( mktemp_buf.get(), path.c_str() );
  return fs::path{ ::mktemp( mktemp_buf.get() ) };
}

///////////////////////////////////////////////////////////////////////////////

} // namespace

/* vim:set et sw=2 ts=2: */
