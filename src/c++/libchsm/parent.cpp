/*
**      CHSM Language System
**      src/c++/libchsm/parent.cpp -- Run-Time library implementation
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

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"

namespace CHSM_NS {

state::id const parent::NO_CHILD_ID_ = -1;

///////////////////////////////////////////////////////////////////////////////

void parent::deep_clear() {
  for ( auto &child : *this )
    child.deep_clear();
}

bool parent::switch_active_child_to( state* ) {
  // out-of-line since it's virtual.
  return true;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
