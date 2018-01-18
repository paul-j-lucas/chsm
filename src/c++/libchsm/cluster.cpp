/*
**      CHSM Language System
**      src/c++/libchsm/cluster.cpp -- Run-Time library implementation
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
#include "util.h"

namespace CHSM_NS {

///////////////////////////////////////////////////////////////////////////////

cluster::cluster( CHSM_CLUSTER_ARGS ) :
  parent( CHSM_PARENT_INIT ),
  history_( chsm_history_ )
{
  active_child_ = last_child_ = nullptr;
}

void cluster::deep_clear() {
  clear();
  parent::deep_clear();
}

bool cluster::enter( event const &trigger, state *from_child ) {
  if ( !state::enter( trigger ) )
    return false;

  if ( unlikely( empty() ) )            // guard against degenerate case
    return true;

  if ( from_child != nullptr ) {
    //
    // One of our child states is being entered directly: therefore, we must
    // merely make a note of that fact; it will take care of entering itself.
    //
    active_child_ = last_child_ = from_child;
  }
  else {
    if ( last_child_ == nullptr || !history_ ) {
      //
      // We were never active before (and thus have no last_child_) or we have
      // no history: in either case, we must enter our default child state.
      //
      last_child_ = &front();
    }
    (active_child_ = last_child_)->enter( trigger );
  }

  return true;
}

bool cluster::exit( event const &trigger, state *to ) {
  if ( !active() )
    return false;
  active_child_->exit( trigger );       // exit our active child first
  return state::exit( trigger, to );
}

bool cluster::switch_active_child_to( state *child ) {
  if ( active_child_->active() ) {
    //
    // We're being asked by a currently-inactive child state to become our
    // active child state, but we already have an active child state --
    // permission denied.
    //
    return false;
  }
  active_child_ = last_child_ = child;
  return true;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
