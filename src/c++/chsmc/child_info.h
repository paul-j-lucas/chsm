/*
**      CHSM Language System
**      src/c++/chsmc/child_info.h
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

#ifndef chsmc_child_info_H
#define chsmc_child_info_H

// local
#include "base_info.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %child_info is-a base_info for identifiers in child declaration lists:
 *
 *      cluster c(x,y) is { // ...
 *                ^ ^
 *
 * Being derived from base_info and not global_info, they are destroyed when
 * the scope closes.
 */
struct child_info final : base_info {
  CHSM_DECLARE_RTTI;

  PJL::symbol const *const sy_parent_;  ///< Symbol of parent state.
  bool defined_;                        ///< Has this child been defined?

  /**
   * Constructs a %child_info.
   *
   * @param sy_parent The symbol of the parent state.
   */
  explicit child_info( PJL::symbol const *sy_parent );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_child_info_H */
/* vim:set et ts=2 sw=2: */
