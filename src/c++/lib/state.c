/*
**      CHSM Language System
**      src/c++/lib/state.c -- Run-Time library implementation
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"
#include "util.h"

namespace CHSM_NS {

//=============================================================================

state::state( CHSM_STATE_ARGS ) :
    machine_( chsm_machine_ ),
    name_( chsm_name_ ),
    parent_( chsm_parent_ ),
    enter_action_( chsm_enter_action_ ), exit_action_( chsm_exit_action_ ),
    enter_event_( chsm_enter_event_ ), exit_event_( chsm_exit_event_ ),
    state_( S_inactive )
{
    // do nothing else
}

//=============================================================================

state::~state() {
    // do nothing else
}

//=============================================================================

void state::deep_clear() {
    // do nothing
}

//=============================================================================

bool state::enter( event const &trigger, state* ) {
    if ( active() )
        return false;

    if ( parent_of() )
        if ( !parent_of()->active() ) {
            //
            // If we are being entered from another state directly (bypassing
            // our parent), we have to enter our parent first.  (This is the
            // only place where the second argument to enter() is not null.)
            //
            parent_of()->enter( trigger, this );
            //
            // In the case where our parent is a set, it will also have entered
            // all of its child states (including us).  We must therefore check
            // for this by seeing if we're active now: if so, return and do not
            // enter ourselves again.
            //
            if ( active() )
                return false;
        } else {
            //
            // Otherwise, we have to ask our parent for permission to be
            // entered.  (See the comment in cluster::switch_child() for
            // further details.)
            //
            if ( !parent_of()->switch_child( this ) )
                return false;
        }

    if ( machine_.debug() & machine::D_enex )
        ECHO << "entering: " << name() ENDL;

    state_ = S_active;

    //
    // For this state, broadcast entered(*this), but only if there are any
    // transitions on it.  The value for the enter_event_ pointer is determined
    // by the CHSM-to-C++ compiler.
    //
    if ( enter_event_ )
        enter_event_->lock_broadcast();

    //
    // Perform the enter action resulting from an "upon enter" statement, if
    // any.  The value for the enter_action_ pointer is determined by the
    // CHSM-to-C++ compiler.
    //
    if ( enter_action_ )
        (machine_.*enter_action_)( *this, trigger );

    return true;
}

//=============================================================================

bool state::exit( event const &trigger, state *to ) {
    if ( !active() )
        return false;

    state_ = S_inactive;

    if ( machine_.debug() & machine::D_enex )
        ECHO << "exiting : " << name() ENDL;

    //
    // For this state, broadcast exited(*this), but only if there are any
    // transitions on it.  The value for the exit_event_ pointer is determined
    // by the CHSM-to-C++ compiler.
    //
    if ( exit_event_ )
        exit_event_->lock_broadcast();

    //
    // Perform the exit action resulting from an "upon exit" statement, if any.
    // The value for the exit_action_ pointer is determined by the CHSM-to-C++
    // compiler.
    //
    if ( exit_action_ )
        (machine_.*exit_action_)( *this, trigger );

    if ( to && parent_of() ) {
        //
        // The "to" state argument is supplied only to the "from" state being
        // exited directly.  It means we are responsible for ensuring our
        // parent state is exited if necessary.
        //
        // To do this, check the ancestors of the "to" state to see if one of
        // them is our parent.  If it is, nothing needs to be done; if not,
        // then we must force our parent to exit.
        //
        state *ancestor = to->parent_of();
        while ( ancestor && ancestor != parent_of() )
            ancestor = ancestor->parent_of();
        if ( !ancestor ) {
            //
            // Our parent is not an ancestor of the "to" state: force it to
            // exit.
            //
            parent_of()->exit( trigger, to );
        }
    }
    return true;
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
