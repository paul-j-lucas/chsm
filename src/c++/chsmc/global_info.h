/*
**      CHSM Language System
**      src/c++/chsmc/global_info.h
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

#ifndef chsmc_global_info_H
#define chsmc_global_info_H

// local
#include "base_info.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %global_info is-a base_info such that a symbol having an info of
 * %global_info (or an info of a class derived from %global_info) will be
 * retained for the life of the compilation, i.e., is not subject to scope.
 */
struct global_info : base_info {
  CHSM_DECLARE_RTTI;

  global_info() : base_info{ SCOPE_GLOBAL } { }
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_global_info_H */
/* vim:set et ts=2 sw=2: */
