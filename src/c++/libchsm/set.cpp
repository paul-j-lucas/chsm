/*
**      CHSM Language System
**      src/c++/libchsm/set.cpp -- Run-Time library implementation
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

///////////////////////////////////////////////////////////////////////////////

bool set::enter( event const &trigger, state* ) {
  if ( !state::enter( trigger ) )
    return false;

  // enter all of our children
  for ( auto &child : *this )
    child.enter( trigger );

  return true;
}

bool set::exit( event const &trigger, state *to ) {
  if ( !active() )
    return false;

  // exit all of our children
  for ( auto &child : *this )
    child.exit( trigger );

  return state::exit( trigger, to );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
