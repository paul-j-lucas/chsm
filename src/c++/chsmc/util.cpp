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

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

char const* base_name( char const *path ) {
  assert( path != nullptr );
  if ( char const *const slash = std::strrchr( path, '/' ) )
    return slash[1] != '\0' ? slash + 1 : slash;
  return path;
}

string base_name( string const &path ) {
  string::size_type const slash_pos = path.find_last_of( '/' );
  if ( slash_pos != string::npos )
    return path.substr( slash_pos + (slash_pos < path.size()) );
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
  int const BUF_SIZE = 20;
  int const NUM_BUFS = 10;

  static char buf[ NUM_BUFS ][ BUF_SIZE ];
  static int  b;

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

void path_append( string *path, string const &component ) {
  assert( path != nullptr );
  if ( !path->empty() ) {
    if ( (*path)[ path->size() - 1 ] != '/' )
      *path += '/';
    *path += component;
  }
}

string path_ext( string const &path ) {
  string const temp = base_name( path );
  string::size_type const dot_pos = temp.find_last_of( '.' );
  if ( dot_pos == string::npos )
    return string();
  return temp.substr( dot_pos + 1 );
}

string path_noext( string const &path ) {
  string::size_type const dot_pos = path.find_last_of( '.' );
  if ( dot_pos == string::npos )
    return path;
  string::size_type const slash_pos = path.find_last_of( '/' );
  if ( slash_pos != string::npos && dot_pos < slash_pos )
    return path;
  string temp( path );
  temp.resize( dot_pos );
  return temp;
}

void perror_exit( int status ) {
  //perror( me );
  ::exit( status );
}

char const* temp_dir() {
  static char const *const dir = getenv( "TMPDIR" );
  return dir != nullptr ? dir : "/tmp";
}

string temp_path( char const *pattern ) {
  string pattern_str( pattern );
  assert( ends_with( pattern_str, "XXXXXX" ) );

  string path_str( temp_dir() );
  path_append( &path_str, pattern_str );
  unique_ptr<char[]> const mktemp_buf { new char[ path_str.size() + 1 ] };
  ::strcpy( mktemp_buf.get(), path_str.c_str() );
  return string( ::mktemp( mktemp_buf.get() ) );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace

/* vim:set et sw=2 ts=2: */
