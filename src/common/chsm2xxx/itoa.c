/*
**      PJL C++ Library
**      itoa.c
**
**      Copyright (C) 2002  Paul J. Lucas
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

#include <algorithm>

namespace PJL {

int const Buf_Size      = 20;
int const Num_Buffers   = 10;

//*****************************************************************************
//
// SYNOPSIS
//
        char const* ltoa( register long n )
//
// DESCRIPTION
//
//      Converts a long integer to a string.  The string returned is from an
//      internal pool of string buffers.  The time you get into trouble is if
//      you hang on to more than Num_Buffers strings.  This doesn't normally
//      happen in practice, however.
//
// PARAMETERS
//
//      n   The long integer to be converted.
//
// RETURN VALUE
//
//      A pointer to the string.
//
// CAVEAT
//
//      This function is NOT thread-safe.
//
// SEE ALSO
//
//      Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language,
//      2nd ed."  Addison-Wesley, Reading, MA, 1988.  pp. 63-64.
//
//*****************************************************************************
{
    static char buf[ Num_Buffers ][ Buf_Size ];
    static int  b;

    register char *s = buf[ b ];
    bool const is_neg = n < 0;

    if ( is_neg ) n = -n;
    do {                                // generate digits in reverse
        *s++ = n % 10 + '0';
    } while ( n /= 10 );
    if ( is_neg ) *s++ = '-';
    *s = '\0';

    std::reverse( buf[ b ], s );
    s = buf[ b ];
    b = (b + 1) % Num_Buffers;

    return s;
}

} // namespace PJL
/* vim:set et ts=4 sw=4: */
