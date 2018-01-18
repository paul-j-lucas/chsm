/*
**      CHSM Language System
**      src/c++/chsmc/event_info.h
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

#ifndef chsmc_event_info_H
#define chsmc_event_info_H

// local
#include "global_info.h"

// standard
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * An %event_info is-a global_info for holding the list of transitions
 * potentially taken when this event is broadcast.
 */
struct event_info : global_info {
  CHSM_DECLARE_RTTI;

  typedef std::vector<int> transition_id_list;

  /**
   * Event kind.
   */
  enum kind {
    KIND_USER,                          ///< User event.
    KIND_ENTER,                         ///< Enter event.
    KIND_EXIT                           ///< Exit event.
  };

  kind const kind_;                     ///< Kind of event.

  /** The id's of the transitions taken by this event. */
  transition_id_list transition_ids_;

  /**
   * The state that this event is either an enter or exit event for.
   * Used only if kind_ is not KIND_USER.
   */
  PJL::symbol const *const sy_state_;

  /**
   * Constructs an %event_info.
   *
   * @param k The kind of event.
   * @param sy_state The state symbol that this evene is an enter or exit event
   * for, but only if this event is not a user_event.
   */
  explicit event_info( kind k = KIND_USER,
                       PJL::symbol const *sy_state = nullptr );

};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_event_info_H */
/* vim:set et ts=2 sw=2: */
