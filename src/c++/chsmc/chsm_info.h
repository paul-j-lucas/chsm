/*
**      CHSM Language System
**      src/c++/chsmc/chsm_info.h
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

#ifndef chsmc_chsm_info_H
#define chsmc_chsm_info_H

// local
#include "config.h"                     /* must go first */
#include "global_info.h"
#include "param_data.h"
#include "symbol.h"

// standard
#include <iostream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %chsm_info is-a global_info for the one and only CHSM:
 *
 *      chsm my_chsm is { // ...
 *
 * The class also holds a lot of miscellanous stuff relating to the CHSM as a
 * whole.
 */
struct chsm_info final : global_info, param_data {
  CHSM_DECLARE_RTTI;

  typedef unsigned id_type;
  typedef std::vector<PJL::symbol*> symbol_list;

  typedef symbol_list state_list;
  typedef symbol_list event_list;
  typedef symbol_list transition_list;

  static char const PREFIX_ACTION[];    ///< Prefix for actions.
  static char const PREFIX_CONDITION[]; ///< Prefix for conditions.
  static char const PREFIX_ENTER[];     ///< Prefix for enter events.
  static char const PREFIX_EXIT[];      ///< Prefix for exit events.
  static char const PREFIX_TARGET[];    ///< Prefix for targets.

  PJL::symbol *const  sy_root_;         ///< Root state of CHSM.
  std::string const   derived_;         ///< Derived type, if any.
  bool const          java_public_;     ///< Is Java class `public`?

  //
  // Sequence numbers used for compiler-generated function names for
  // conditions and actions.
  //
  struct id {
    id_type condition_;
    id_type target_;
    id_type action_;

    id() : condition_{ 0 }, target_{ 0 }, action_{ 0 } { }
  };

  id id_;

  /**
   * The machine's states in declaration order.
   */
  state_list states_;

  /**
   * The machine's events in declaration order.
   */
  event_list events_;

  /**
   * The machine's transitions in declaration order so when a parent and a
   * child both have a transition on a given event, the one in the parent will
   * dominate.
   */
  transition_list transitions_;

  /**
   * Constructs a %chsm_info.
   *
   * @param sy_root The symbol of the root cluster of the CHSM.
   * @param derived The derived class name, if any.
   * @param java_public If compiling to Java, is the CHSM class `public`?
   */
  explicit chsm_info( PJL::symbol *sy_root, char const *derived = nullptr,
                      bool java_public = false );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_chsm_info_H */
/* vim:set et ts=2 sw=2: */
