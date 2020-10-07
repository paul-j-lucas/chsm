/*
**      CHSM Language System
**      src/c++/chsmc/user_event_info.h
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

#ifndef chsmc_user_event_info_H
#define chsmc_user_event_info_H

// local
#include "event_info.h"
#include "param_data.h"
#include "symbol.h"

class list_sep;                         /* in list_sep.h */

// standard
#include <list>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %user_event_info is-an event_info and-a param_data that emits itself
 * differently.
 */
struct user_event_info final : event_info, param_data {
  CHSM_DECLARE_RTTI;

  /**
   * The precondition type.
   */
  enum precondition_type {
    PRECONDITION_NONE,                  ///< No precondition.
    PRECONDITION_EXPR,                  ///< For `event A [ expr ];`.
    PRECONDITION_FUNC                   ///< For `event A %{ return expr; %};`.
  };

  PJL::symbol const *const sy_base_event_; ///< This event's base event, if any.
  precondition_type precondition_;      ///< Precondition type, if any.

  /**
   * Constructs a %user_event_info.
   *
   * @param sy_base_event The symbol of the event this event is derived from.
   */
  explicit user_event_info( PJL::symbol const *sy_base_event = nullptr );

   /**
   * Gets whether this event's base event (if any) has any parameters.
   *
   * @return Returns `true` only if this event has a base event and it has any
   * parameters (including those inherited from base events).
   */
  bool base_has_any_parameters() const {
    return  sy_base_event_ != nullptr &&
            INFO_CONST( user_event, sy_base_event_ )->has_any_parameters();
  }

  /**
   * Gets whether this event has any parameters (including those inherited from
   * base events).
   *
   * @return Returns `true` only if this event has any parameters.
   */
  bool has_any_parameters() const {
    return !param_list_.empty() || base_has_any_parameters();
  }

private:
  std::ostream& emit_param_list_aux( std::ostream &o, list_sep &sep,
                                     emit_mask emit_flags = 0 ) const override;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_user_event_info_H */
/* vim:set et ts=2 sw=2: */
