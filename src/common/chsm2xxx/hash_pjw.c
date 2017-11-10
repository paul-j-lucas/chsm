/*
**      PJL C++ Library
**      hash_pjw.c
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

#include "hash_pjw.h"

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        hash_value hash_pjw( register char const *s, unsigned n_buckets )
//
// DESCRIPTION
//
//      Compute a hash value of a string.
//
// PARAMETERS
//
//      s           The string to hash.
//
//      n_buckets   The number of "buckets" in the hash table.
//
// SEE ALSO
//
//      Alfred Aho, Ravi Sethi, and Jeffrey D. Ullman.  "The 'hashpjw' hashing
//      algorithm," by P. J. Weinberger, "Compilers: Principles, Techniques,
//      and Tools," Addison-Wesley, Reading, MA, 1986, pp. 435-436.
//
//*****************************************************************************
{
    register hash_value h = 0, g;
    while ( *s ) {
        h = (h << 4) + *s++;
        if ( g = h & 0xF0000000 )
            h ^= (g >> 24), h ^= g;
    }
    return h % n_buckets;
}

} // namespace PJL
/* vim:set et ts=4 sw=4: */
