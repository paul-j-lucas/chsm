/*
**      CHSM Language System
**      src/c++/chsmc/transition_info.h
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

#ifndef chsmc_transition_H
#define chsmc_transition_H

// local
#include "global_info.h"
#include "symbol.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %transition_info is-a global_info for holding information about
 * transitions.  Transitions are different in that there's no need for them to
 * have a name; hence there's no need to put them into the symbol-table.  It's
 * stored there just for convenience so we don't have to create yet another
 * class.
 */
struct transition_info : global_info {
  CHSM_DECLARE_RTTI;

  typedef unsigned condition_id;
  typedef unsigned target_id;
  typedef unsigned action_id;

  PJL::symbol const *const sy_from_;    ///< State trasitioning from.
  PJL::symbol const *const sy_to_;      ///< State trasitioning to.

  condition_id const condition_id_;     ///< Condition id if > 0.
  target_id const target_id_;           ///< Target state id if > 0.
  action_id const action_id_;           ///< Action id if > 0.

  /**
   * Constructs a %transition_info.
   *
   * @param cid The index ID of the condition to perform, if any.
   * @param sy_from The symbol representing the "from" state.
   * @param sy_to The symbol representing the "to" state.
   * @param tid The index ID of the target function to perform, if any.
   * @param aid The index ID of the action to perform, if any.
   */
  transition_info( condition_id cid, PJL::symbol const *sy_from,
                   PJL::symbol const *sy_to, target_id tdi, action_id aid );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_transition_H */
/* vim:set et ts=2 sw=2: */
