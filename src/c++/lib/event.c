/*
**      CHSM Language System
**      src/c++/lib/event.c -- Run-Time library implementation
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

// standard
#include <cassert>

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"
#include "pjl_threads.h"
#include "util.h"

using namespace std;

namespace CHSM_NS {

int const   event::no_transition_id_ = -1;

//=============================================================================

event::event( CHSM_EVENT_ARGS ) :
    machine_( *chsm_machine_ ),
    transitions_( chsm_transition_list_ ),
    name_( chsm_name_ ),
    base_event_( chsm_base_event_ ),
    in_progress_( 0 ),
    param_block_( 0 )
{
    // do nothing else
}

//=============================================================================

event::~event() {
    // do nothing else
}

//=============================================================================

void event::broadcast( void *pb ) {
    if ( in_progress_ )
        return;
    //
    // Mark ourselves and our base event(s), if any, as being in progress.
    //
    for ( event *e = this; e; e = e->base_event_ )
        ++e->in_progress_;

    if ( machine_.debug() & machine::D_event )
        ECHO << "broadcast: " << name() ENDL;

    if ( param_block_ = pb ) {
        //
        // Evaluate the event's precondition.  Note that since it's a virtual
        // function, the most-derived precondition gets executed.  However, the
        // code emitted by the CHSM-to-C++ compiler is of the form:
        //
        //      return base_event::precondition() && user_precondition();
        //
        // so the most-base precondition really gets evaluated first.  If a
        // base event's precondition is false, the short-circuit semantics of
        // && means no more derived preconditions get evaluated and the
        // call-stack simply unwinds.  The final result is that an event's
        // precondition is the logical and of all base preconditions executed
        // in base-to-derived order.
        //
        bool const result = static_cast<param_block*>( pb )->precondition();

        if ( machine_.debug() & machine::D_event )
            ECHO << "+ precondition: " << ( result ? "true" : "false" ) ENDL;

        if ( !result ) {
            //
            // This event's precondition evaluated to false -- cancel its
            // broadcast.
            //
            goto cancel_broadcast;
        }
    }

    { // local scope to suppress warning about goto skipping initialization

    //
    // Iterate through our event's transitions finding those that will be
    // taken, if any.
    //
    bool found_transition = false;

    if ( machine_.debug() & machine::D_event )
        ECHO << "+ checking transitions" ENDL;

    for ( const_iterator t = begin(); t != end(); ++t ) {
        state &from = *machine_.state_[ t->from_id_ ];
        //
        // We must check specifically for "plain active" so we don't make more
        // than one nondeterministic transition from the same state.  This is
        // the only place in the code that makes a distinction between "plain
        // active" and "active disabled."
        //
        if ( from.state_ != state::S_active || (
            //
            // If the transition has a condition, evaluate it to see whether we
            // should continue.
            //
            t->condition_ && !( machine_.*(t->condition_) )( *this )
        ) )
            continue;

        //
        // Mark this transition as taken using the event that triggered it.
        //
        if ( !machine_.taken_[ t.id() ] )
            machine_.taken_[ t.id() ] = this;

        if ( !t->is_internal() ) {
            if ( t->target_ ) {
                //
                // The transition has a target function: execute it to compute
                // the target state.
                //
                if ( state *const to = (machine_.*(t->target_))( *this ) ) {
                    assert( transition::is_legal( &from, to ) );
                    machine_.target_[ t.id() ] = to;
                } else {
                    //
                    // The target function returned null: skip this transition.
                    //
                    continue;
                }
            } else {
                //
                // The transition goes to a hard-coded target state.
                //
                machine_.target_[ t.id() ] = machine_.state_[ t->to_id_ ];
            }

            //
            // Leave the "from" state active, but mark as disabled to prevent
            // making more than one nondeterministic transition from the same
            // state.
            //
            from.state_ = state::S_active_disabled;
        }

        found_transition = true;

        if ( machine_.debug() & machine::D_event ) {
            ECHO << "+ found  : " << from.name();
            if ( !t->is_internal() )
                MORE << " -> " << machine_.target_[ t.id() ]->name();
            else
                MORE << " (internal)";
            MORE ENDL;
        }
    } // for

    if ( !found_transition ) {
        //
        // This event has no possible transitions to take -- cancel its
        // broadcast.
        //
        goto cancel_broadcast;
    }

    //
    // Queue ourselves and run algorithm.
    //
    machine_.event_queue_.push_back( this );

    if ( machine_.debug() & machine::D_event )
        ECHO << "queued   : " << name() ENDL;

    } // end local scope

    machine_.algorithm();
    return;

cancel_broadcast:
    //
    // This event is not to be queued: destroy its parameter block and return.
    //
    if ( machine_.debug() & machine::D_event )
        ECHO << "broadcast: " << name() << " -- cancelled" ENDL;

    broadcasted();
}

//=============================================================================

void event::broadcasted() {
    //
    // This check isn't strictly necessary...but I feel better having it here.
    //
    if ( in_progress_ ) {
        //
        // Unmark ourselves and our base events, if any, as being in progress.
        //
        for ( event *e = this; e; e = e->base_event_ )
            if ( e->in_progress_ )
                --e->in_progress_;

        if ( param_block_ ) {
            //
            // Since a param_block object is never created nor destroyed (in
            // terms of allocation and deallocation), we have to call its
            // destructor explicitly to destroy (but not deallocate) it.  Yes,
            // this is a legal thing to do in C++: it's just not often
            // necessary.  See [Stroustrup], p. 256.
            //
            static_cast<param_block*>( param_block_ )
                ->param_block::~param_block();
        }
    }
}

//=============================================================================

void event::lock_broadcast() {
    MUTEX_LOCK( &machine_.machine_lock_, true );
    broadcast( 0 );
    MUTEX_UNLOCK();
}

//=============================================================================

event::const_iterator::const_iterator( const_pointer t, transition_list id,
                                       event const *b ) :
    t_( t ), t_id_( id ), base_event_( b )
{
    bump();
}

//=============================================================================

void event::const_iterator::bump() {
    while ( *t_id_ == no_transition_id_ && base_event_ ) {
        //
        // We've exhausted our transitions; proceed to those of our base
        // event's, if any.
        //
        t_id_ = base_event_->transitions_;
        base_event_ = base_event_->base_event_;
    }
}

//=============================================================================

event::param_block::~param_block() {
    // do nothing
}

//=============================================================================

bool event::param_block::precondition() const {
    return true;
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
