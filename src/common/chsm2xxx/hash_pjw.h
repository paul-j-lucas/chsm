/*
**      PJL C++ Library
**      hash_pjw.h
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

#ifndef hash_pjw_H
#define hash_pjw_H

namespace PJL {

//
// The "hashpjw" hashing algorithm, by P. J. Weinberger.  Taken from
// "Compilers: Principles, Techniques, and Tools," by Aho, Sethi, and Ullman,
// Addison-Wesley, Reading, MA, 1986, pp. 435-436.
//
typedef unsigned hash_value;
hash_value hash_pjw( char const *string, unsigned n_buckets );

} // namespace PJL

#endif  /* hash_pjw_H */
/* vim:set et ts=4 sw=4: */
