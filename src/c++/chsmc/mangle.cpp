/*
**      CHSM Language System
**      src/c++/chsmc/mangle.cpp
**
**      Copyright (C) 1996  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 3 of the License, or
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

// local
#include "config.h"                     /* must go first */
#include "util.h"

// standard
#include <cctype>                       /* for isdigit() */
#include <cstdlib>                      /* for atoi() */
#include <cstring>

using namespace std;
using namespace PJL;

static char         g_mangle_buf[ 256 ];
static char const   g_mangle_prefix[]   = "M";

///////////////////////////////////////////////////////////////////////////////

char const* mangle( char const *s ) {
  ::strcpy( g_mangle_buf, g_mangle_prefix );
  char const *in = s, *dot;
  char *out = g_mangle_buf + 1;
  int len;
  do {
    // length until '.' or to end of string if no '.'
    dot = ::strchr( in, '.' );
    len = dot ? dot - in : ::strlen( in );

    // paste length in
    ::strcpy( out, ::itoa( len ) );
    out += 1 + (len > 9);

    // paste name in
    while ( len-- )
      *out++ = *in++;
    ++in;
  } while ( dot );

  *out = '\0';
  return g_mangle_buf;
}

char const* demangle( char const *s ) {
  static int const g_mangle_prefix_len = ::strlen( g_mangle_prefix );

  char const *in = s + g_mangle_prefix_len;
  if ( !isdigit( *in ) )                // not mangled to begin with
    return s;

  char *out = g_mangle_buf;
  int len;

  while ( (len = ::atoi( in )) != 0 ) {
    in += 1 + (len > 9);
    while ( len-- )
      *out++ = *in++;
    *out++ = '.';
  } // while

  *--out = '\0';
  return g_mangle_buf;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
