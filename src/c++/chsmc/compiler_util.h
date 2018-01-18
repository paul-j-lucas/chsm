/*
**      CHSM Language System
**      src/c++/chsmc/compiler_util.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas
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

#ifndef chsmc_compiler_util_H
#define chsmc_compiler_util_H

// local
#include "base_info.h"
#include "symbol.h"

///////////////////////////////////////////////////////////////////////////////

enum type_condition {
  MAY_EXIST,
  NO_INFO,
  MUST_EXIST
};

/**
 * Performs type-checking of a symbol.
 *
 * @param sy The symbol to check.
 * @param types The allowable types.
 * @param condition The condition under which a type must match.
 * @param more Additional error string.
 * @param in How far "in" to look for the symbol's info.
 * @return Returns `true` only if the type is OK.
 */
bool type_check( PJL::symbol const *sy, base_info::symbol_type types,
                 type_condition condition, char const *more = "",
                 unsigned in = 0 );

/**
 * Given a symbol, return its type (the type of the first *_info struct
 * hanging off of it).
 *
 * @param sy The symbol to use.
 * @param in How far "in" to go.
 * @return Returns said type or T_NONE.
 */
base_info::symbol_type type_of( PJL::symbol const *sy, unsigned in = 0 );

/**
 * Turns the encoded types into a string of one or more type-names.
 *
 * @param types The types to convert.
 * @return Returns a string containing or "or"-separated list of type names.
 */
char const* type_string( base_info::symbol_type t );

////////// inlines ////////////////////////////////////////////////////////////

/**
 * Gets the type of the symbol at the scope level.
 *
 * @param sy The symbol to get the type of.
 * @param in How far "in" to go for the scope.
 * @return Returns the symbol type or T_NONE.
 */
inline base_info::symbol_type
type_of( PJL::symbol const &sy, unsigned in = 0 ) {
  return type_of( &sy, in );
}

/**
 * Gets the type of the symbol at the scope level.
 *
 * @param i The iterator referring to the symbol to get the type of.
 * @param in How far "in" to go for the scope.
 * @return Returns the symbol type or T_NONE.
 */
inline base_info::symbol_type
type_of( PJL::symbol_table::iterator const &i, unsigned in = 0 ) {
  return type_of( i->second, in );
}

/**
 * Gets the type of the symbol at the scope level.
 *
 * @param i The iterator referring to the symbol to get the type of.
 * @param in How far "in" to go for the scope.
 * @return Returns the symbol type or T_NONE.
 */
inline base_info::symbol_type
type_of( PJL::symbol_table::const_iterator const &i, unsigned in = 0 ) {
  return type_of( i->second, in );
}

/**
 * Gets the type of the symbol at the scope level.
 *
 * @tparam ContainerType The container type.
 * @param i The iterator referring to the symbol to get the type of.
 * @param in How far "in" to go for the scope.
 * @return Returns the symbol type or T_NONE.
 */
template<template<typename> class ContainerType>
inline base_info::symbol_type
type_of( typename ContainerType<PJL::symbol*>::iterator const &i,
         unsigned in = 0 ) {
  return type_of( *i, in );
}

/**
 * Gets the type of the symbol at the scope level.
 *
 * @tparam ContainerType The container type.
 * @param i The iterator referring to the symbol to get the type of.
 * @param in How far "in" to go for the scope.
 * @return Returns the symbol type or T_NONE.
 */
template<template<typename> class ContainerType>
inline base_info::symbol_type
type_of( typename ContainerType<PJL::symbol*>::const_iterator const &i,
         unsigned in = 0 ) {
  return type_of( *i, in );
}

/**
 * Gets the type string for the given symbol.
 *
 * @param p A pointer to the symbol.
 * @return Returns said type string.
 */
inline char const* type_string( PJL::symbol const *p ) {
  return type_string( type_of( p ) );
}

/**
 * Gets the type string for the given symbol.
 *
 * @param sy The symbol.
 * @return Returns said type string.
 */
inline char const* type_string( PJL::symbol const &sy ) {
  return type_string( type_of( sy ) );
}

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_compiler_util_H */
/* vim:set et sw=2 ts=2: */
