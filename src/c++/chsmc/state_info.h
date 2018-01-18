/*
**      CHSM Language System
**      src/c++/chsmc/state_info.h
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

#ifndef chsmc_state_info_H
#define chsmc_state_info_H

// local
#include "global_info.h"
#include "serial_number.h"
#include "symbol.h"
#include "compiler_util.h"

// standard
#include <cstring>
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %state_info is-a global_info and-a serial_number<state_info> for
 * information on states (and also clusters and sets that are derived from it).
 * States have a serial number that is used in child and transition tables.
 */
struct state_info : global_info, PJL::serial_number<state_info> {
  CHSM_DECLARE_RTTI;

  PJL::symbol const *const sy_parent_;  ///< Parent state, if any.
  std::string const derived_;           ///< Derived class name, if any.

  struct events {                       // whether we have these
    bool has_enter_, has_exit_;
    events() { has_enter_ = has_exit_ = false; }
  } event_;

  struct actions {                      // whether we have these
    bool has_enter_, has_exit_;
    actions() { has_enter_ = has_exit_ = false; }
  } action_;

  /**
   * Constructs a %state_info.
   *
   * @param sy_parent The symbol of the parent state of this state, if any.
   * @param derived The derived class name, if any.
   */
  explicit state_info( PJL::symbol const *sy_parent = nullptr,
                       char const *derived = nullptr );
};

////////// inlines ////////////////////////////////////////////////////////////

/**
 * Gets the "base name" of a state name.  E.g.:
 *
 *      root.trunk.branch.leaf -> leaf
 *
 * @param state_name The state name to get the base name of.
 * @return Returns said base name.
 */
inline char const* state_base_name( char const *state_name ) {
  char const *const p = ::strrchr( state_name, '.' );
  return p != nullptr ? p + 1 : state_name;
}

/**
 * Shorthand for accessing the serial number of a state.  Used in emitting
 * indices in child and transition tables.
 */
inline PJL::serial_number_base::value_type serial( PJL::symbol const *sy ) {
  return sy != nullptr ? INFO_CONST( state, sy )->serial()
    -1  // Due to the root_ state inside chsm_info.
  :
    -1  // null pointer: return -1 for index
  ;
}

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_state_info_H */
/* vim:set et ts=2 sw=2: */
