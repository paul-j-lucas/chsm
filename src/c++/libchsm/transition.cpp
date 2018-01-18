/*
**      CHSM Language System
**      src/c++/libchsm/transition.cpp -- Run-Time library implementation
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

// standard
#include <deque>

using namespace std;

namespace CHSM_NS {

///////////////////////////////////////////////////////////////////////////////

bool transition::is_legal( state const *si, state const *sj ) {
  typedef deque<state const*> state_list;
  state_list si_list, sj_list;

  for ( auto s = si; s != nullptr; s = s->parent_of() )
    si_list.push_front( s );
  for ( auto s = sj; s != nullptr; s = s->parent_of() )
    sj_list.push_front( s );

  state const *nearest_common_ancestor = nullptr;

  for ( auto i = si_list.begin(), j = sj_list.begin();
        i != si_list.end() && j != sj_list.end() && *i == *j; ++i, ++j ) {
    nearest_common_ancestor = *i;
  } // for

  return dynamic_cast<set const*>( nearest_common_ancestor ) == nullptr;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
