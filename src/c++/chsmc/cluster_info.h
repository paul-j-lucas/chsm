/*
**      CHSM Language System
**      src/c++/chsmc/cluster_info.h
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

#ifndef chsmc_cluster_info_H
#define chsmc_cluster_info_H

// local
#include "parent_info.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %cluster_info is-a parent_info for a cluster.
 */
struct cluster_info : parent_info {
  CHSM_DECLARE_RTTI;

  bool const history_;

  /**
   * Constructs a %cluster_info.
   *
   * @param sy_parent The symbol of the parent state of this state, if any.
   * @param derived The derived class name, if any.
   * @param history `true` only if this cluster has a history.
   */
  explicit cluster_info( PJL::symbol const *sy_parent = nullptr,
                         char const *derived = nullptr,
                         bool history = false );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_cluster_info_H */
/* vim:set et ts=2 sw=2: */
