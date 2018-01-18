/*
**      CHSM Language System
**      src/c++/chsmc/parent_info.h
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

#ifndef chsmc_parent_info_H
#define chsmc_parent_info_H

// local
#include "state_info.h"

// standard
#include <iostream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %parent_info is-a parent_data and-a state_info for holding the information
 * in common by a cluster and a set namely the children.
 *
 * @note This is an abstract class.  (Therefore, it doesn't need either of the
 * CHSM_DECLARE_RTTI or CHSM_DEFINE_RTTI macros since there will never be an
 * instance of it.)
 */
struct parent_info : state_info {
  typedef std::vector<PJL::symbol*> child_list;

  child_list children_;                 ///< Child states.

protected:
  /**
   * Constructs a %parent_info.
   *
   * @param sy_parent The parent symbol, if any.
   * @param derived The derived class name, if any.
   */
  parent_info( PJL::symbol const *sy_parent, char const *derived );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_parent_info_H */
/* vim:set et ts=2 sw=2: */
