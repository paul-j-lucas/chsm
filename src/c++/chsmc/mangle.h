/*
**      CHSM Language System
**      src/c++/chsmc/mangle.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas & Fabio Riccardi
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

#ifndef chsmc_mangle_H
#define chsmc_mangle_H

// standard
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * "Mangles" an identifier:
 *
 *      root.trunk.branch.leaf -> M4root5trunk6branch4leaf
 *
 * to generate a unique identifier in class scope.  Mangled names are needed
 * for enter/exit events/actions.
 *
 * @warning The string returned is from a static buffer.  The time you get into
 * trouble is if you hang on to more than one return value.  This doesn't
 * happen in the code, however.
 *
 * @param s The string to be mangled.
 * @return Returns the mangled identifier.
 */
char const* mangle( char const *state_name );

inline char const* mangle( std::string const &state_name ) {
  return mangle( state_name.c_str() );
}

/**
 * "Demangles" an identifier, e.g.:
 *
 *      M4root5trunk6branch4leaf -> root.trunk.branch.leaf
 *
 * @warning The string returned is from a static buffer.  The time you get into
 * trouble is if you hang on to more than one return value.  This doesn't
 * happen in the code, however.
 *
 * @param s The string to be demangled.
 * @return Returns the demangled identifier.
 */
char const* demangle( char const *state_name );

inline char const* demangle( std::string const &state_name ) {
  return demangle( state_name.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_mangle_H */
/* vim:set et ts=2 sw=2: */
