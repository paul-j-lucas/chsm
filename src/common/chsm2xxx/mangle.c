/*
**      CHSM Language System
**      src/common/chsm2xxx/mangle.c
**
**      Copyright (C) 1996  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// standard
#include <cctype>                       /* for isdigit() */
#include <cstdlib>                      /* for atoi() */
#include <cstring>

// local
#include "itoa.h"

using namespace std;
using namespace PJL;

static char         g_mangle_buf[ 256 ];
static char const   g_mangle_prefix[]   = "M";

//*****************************************************************************
//
// SYNOPSIS
//
        char const* mangle( char const *s )
//
// DESCRIPTION
//
//      "Mangle" an identifier:
//
//          root.trunk.branch.leaf -> M4root5trunk6branch4leaf
//
//      to generate a unique identifier in class scope.  Mangled names are
//      needed for enter/exit events/actions.
//
//      The string returned is from a static buffer.  The time you get into
//      trouble is if you hang on to more than one return value.  This doesn't
//      happen in the code, however.
//
// PARAMETERS
//
//      s   The string to be mangled.
//
// RETURN VALUE
//
//      Returns the mangled identifier.
//
//*****************************************************************************
{
    ::strcpy( g_mangle_buf, g_mangle_prefix );
    register char const *in = s, *dot;
    register char *out = g_mangle_buf + 1;
    register int len;
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

//*****************************************************************************
//
// SYNOPSIS
//
        char const* demangle( char const *s )
//
// DESCRIPTION
//
//      "Demangle" an identifier, e.g.:
//
//          M4root5trunk6branch4leaf -> root.trunk.branch.leaf 
//
//      The string returned is from a static buffer.  The time you get into
//      trouble is if you hang on to more than one return value.  This doesn't
//      happen in the code, however.
//
// PARAMETERS
//
//      s   The string to be demangled.
//
// RETURN VALUE
//
//      Returns the demangled identifier.
//
//*****************************************************************************
{
    static int const g_mangle_prefix_len = ::strlen( g_mangle_prefix );

    register char const *in = s + g_mangle_prefix_len;
    if ( !isdigit( *in ) )              // not mangled to begin with
        return s;

    register char *out = g_mangle_buf;
    register int len;

    while ( len = ::atoi( in ) ) {
        in += 1 + (len > 9);
        while ( len-- )
            *out++ = *in++;
        *out++ = '.';
    }

    *--out = '\0';
    return g_mangle_buf;
}
/* vim:set et ts=4 sw=4: */
