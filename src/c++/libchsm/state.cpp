/*
**      CHSM Language System
**      src/c++/libchsm/state.cpp -- Run-Time library implementation
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

using namespace std;

namespace CHSM_NS {

///////////////////////////////////////////////////////////////////////////////

state::state( CHSM_STATE_ARGS ) :
  machine_{ chsm_machine_ },
  name_{ chsm_name_ },
  parent_{ chsm_parent_ },
  state_{ STATE_INACTIVE },
  enter_action_{ chsm_enter_action_ }, exit_action_{ chsm_exit_action_ },
  enter_event_{ chsm_enter_event_ }, exit_event_{ chsm_exit_event_ }
{
  // do nothing else
}

state::~state() {
  // do nothing else
}

void state::deep_clear() {
  // do nothing
}

bool state::enter( event const &trigger, state* ) {
  if ( active() )
    return false;

  if ( parent_of() != nullptr ) {
    if ( !parent_of()->active() ) {
      //
      // If we are being entered from another state directly (bypassing our
      // parent), we have to enter our parent first.  (This is the only place
      // where the second argument to enter() is not null.)
      //
      parent_of()->enter( trigger, this );
      //
      // In the case where our parent is a set, it will also have entered all
      // of its child states (including us).  We must therefore check for this
      // by seeing if we're active now: if so, return and do not enter
      // ourselves again.
      //
      if ( active() )
        return false;
    }
    else {
      //
      // Otherwise, we have to ask our parent for permission to be entered.
      // (See the comment in cluster::switch_active_child_to() for further
      // details.)
      //
      if ( !parent_of()->switch_active_child_to( this ) )
        return false;
    }
  }

  if ( machine_.is_debug( machine::DEBUG_ENTER_EXIT ) )
    machine_.dout() << "entering: " << name() ENDL;

  state_ = STATE_ACTIVE;

  //
  // For this state, broadcast entered(*this), but only if there are any
  // transitions on it.  The value for the enter_event_ pointer is determined
  // by the CHSM-to-C++ compiler.
  //
  if ( enter_event_ != nullptr )
    enter_event_->lock_broadcast();

  //
  // Perform the enter action resulting from an "upon enter" statement, if any.
  // The value for the enter_action_ pointer is determined by the CHSM-to-C++
  // compiler.
  //
  if ( enter_action_ != nullptr ) {
    try {
      (machine_.*enter_action_)( *this, trigger );
    }
    catch ( ... ) {
      //
      // Ignore any exception that an enter action may have thrown.
      //
    }
  }

  return true;
}

bool state::exit( event const &trigger, state *to ) {
  if ( !active() )
    return false;

  state_ = STATE_INACTIVE;

  if ( machine_.is_debug( machine::DEBUG_ENTER_EXIT ) )
    machine_.dout() << "exiting : " << name() ENDL;

  //
  // For this state, broadcast exited(*this), but only if there are any
  // transitions on it.  The value for the exit_event_ pointer is determined by
  // the CHSM-to-C++ compiler.
  //
  if ( exit_event_ != nullptr )
    exit_event_->lock_broadcast();

  //
  // Perform the exit action resulting from an "upon exit" statement, if any.
  // The value for the exit_action_ pointer is determined by the CHSM-to-C++
  // compiler.
  //
  if ( exit_action_ != nullptr ) {
    try {
      (machine_.*exit_action_)( *this, trigger );
    }
    catch ( ... ) {
      //
      // Ignore any exception that an exit action may have thrown.
      //
    }
  }

  if ( to != nullptr && parent_of() != nullptr ) {
    //
    // The "to" state argument is supplied only to the "from" state being
    // exited directly.  It means we are responsible for ensuring our parent
    // state is exited if necessary.
    //
    // To do this, check the ancestors of the "to" state to see if one of them
    // is our parent.  If it is, nothing needs to be done; if not, then we must
    // force our parent to exit.
    //
    state *ancestor = to->parent_of();
    while ( ancestor != nullptr && ancestor != parent_of() )
      ancestor = ancestor->parent_of();

    if ( ancestor == nullptr ) {
      //
      // Our parent is not an ancestor of the "to" state: force it to exit.
      //
      parent_of()->exit( trigger, to );
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
