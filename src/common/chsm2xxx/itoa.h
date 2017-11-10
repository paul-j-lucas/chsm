/*
**      PJL C++ Library
**      itoa.h
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

#ifndef itoa_H
#define itoa_H

namespace PJL {

//
// Conversion from long and int to string: opposite of atol() and atoi().
//
extern char const*  ltoa( long );
inline char const*  itoa( int n )       { return ltoa( n ); }

} // namespace PJL

#endif  /* itoa_H */
/* vim:set et ts=4 sw=4: */
