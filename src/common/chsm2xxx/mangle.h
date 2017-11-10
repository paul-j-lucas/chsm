/*
**      CHSM Language System
**      src/common/chsm2xxx/mangle.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
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

#ifndef CHSM_mangle_H
#define CHSM_mangle_H

// standard
#include <string>

char const*         mangle( char const *state_name );
inline char const*  mangle( std::string const &state_name ) {
                        return mangle( state_name.c_str() );
                    }

char const*         demangle( char const *state_name );
inline char const*  demangle( std::string const &state_name ) {
                        return demangle( state_name.c_str() );
                    }

#endif  /* CHSM_mangle_H */
/* vim:set et ts=4 sw=4: */
