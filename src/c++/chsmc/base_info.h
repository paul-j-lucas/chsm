/*
**      CHSM Language System
**      src/c++/chsmc/base_info.h
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

#ifndef chsmc_base_info_H
#define chsmc_base_info_H

// local
#include "symbol.h"

class info_visitor;

// standard
#include <string>

//
// Macros used to declare/define run-time type-information for *_info's.
//
#define CHSM_DECLARE_RTTI                       \
  void accept( info_visitor& ) const override;  \
  symbol_type type() const override

#define CHSM_DEFINE_RTTI(DERIVED,SYMBOL_TYPE)                           \
  void DERIVED::accept( info_visitor &v ) const { v.visit( *this ); }   \
  base_info::symbol_type DERIVED::type() const { return SYMBOL_TYPE; }  \
  typedef int chsm_define_rtti_dummy_type_to_eat_semicolon

/**
 * Shorthand macro for accessing the `*_info` substructure hanging off a symbol
 * cast to the right derived type.
 *
 * @param TYPE The symbol type to cast to.
 * @param SYM A pointer to the symbol to get the info for.
 * @return Returns said synfo.
 * @hideinitializer
 */
#define INFO(TYPE,SYM) \
  (const_cast<TYPE##_info*>( INFO_CONST(TYPE,SYM) ))

/**
 * Shorthand macro for accessing the `*_info` substructure hanging off a symbol
 * cast to the right derived type.
 *
 * @param TYPE The symbol type to cast to.
 * @param SYM A pointer to the symbol to get the info for.
 * @return Returns said synfo.
 * @hideinitializer
 */
#define INFO_CONST(TYPE,SYM) \
  (dynamic_cast<TYPE##_info const*>( info_helper( SYM ) ))

/**
 * Shorthand for specifying an info type.
 *
 * @param NAME The type name (without the leading `T_`).
 * @see base_info
 */
#define TYPE(NAME)                base_info::T_##NAME

///////////////////////////////////////////////////////////////////////////////

/**
 * A %base_info is-a synfo serving as the base class for all info's in the CHSM
 * compiler.
 */
struct base_info : PJL::synfo {
  typedef unsigned symbol_type;

  static symbol_type const T_NONE       = 0x0000;
  static symbol_type const T_UNDECLARED = 0x0001;
  static symbol_type const T_GLOBAL     = 0x0002; // forward-referenced states
  static symbol_type const T_CHSM       = 0x0004;
  static symbol_type const T_CHILD      = 0x0008; // child-list declarations
  static symbol_type const T_STATE      = 0x0010;
  static symbol_type const T_CLUSTER    = 0x0020;
  static symbol_type const T_SET        = 0x0040;
  static symbol_type const T_PARENT     = T_CLUSTER | T_SET;
  static symbol_type const T_ENEX_EVENT = 0x0100;
  static symbol_type const T_USER_EVENT = 0x0200;
  static symbol_type const T_EVENT      = T_ENEX_EVENT | T_USER_EVENT;
  static symbol_type const T_TRANSITION = 0x0400;

  unsigned const  first_ref_;           ///< Line no. first referenced on.
  bool            used_;                ///< True only if actually used.

  virtual void accept( info_visitor &v ) const = 0;

  virtual symbol_type type() const = 0;

protected:
  /**
   * Constructs a %base_info.
   *
   * @param scope The scope to put the info in, or the current scope by
   * default.
   */
  explicit base_info( scope_type scope = PJL::symbol_table::scope() );

  std::ostream& emit( std::ostream& ) const override;
};

////////// inlines ////////////////////////////////////////////////////////////

/**
 * @internal
 * This helper function exists so that \a sy is evaluated only once with the
 * INFO macros.
 */
inline PJL::synfo const* info_helper( PJL::symbol const *sy ) {
  return sy != nullptr ? sy->info() : nullptr;
}

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_base_info_H */
/* vim:set et ts=2 sw=2: */
