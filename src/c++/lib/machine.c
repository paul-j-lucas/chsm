/*
**      CHSM Language System
**      src/c++/lib/machine.c -- Run-Time library implementation
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
#ifdef  CHSM_MULTITHREADED
#include <iostream>
#endif  /* CHSM_MULTITHREADED */

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"
#include "pjl_threads.h"
#include "util.h"

using namespace std;

namespace CHSM_NS {

event       machine::prime_( 0, 0, "<prime>", 0 );
void *const machine::nil_         = 0;
int const   machine::no_state_id_ = -1;

//=============================================================================

machine::machine( CHSM_MACHINE_ARGS ) :
    state_( chsm_state_ ),
    root_( chsm_root_ ),
    transition_( chsm_transition_ ),
    taken_( chsm_taken_ ),
    target_( chsm_target_ ),
    transitions_in_machine_( chsm_transitions_in_machine_ ),
    in_progress_( false ),
    debug_( D_none )
{
#ifdef  CHSM_MULTITHREADED
    pthread_mutexattr_t attr;
    if ( ::pthread_mutexattr_init( &attr ) ) {
        cerr << "could not init thread mutex attribute" << endl;
        ::exit( 1 );
    }
    if ( ::pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE ) ) {
         cerr << "could not set thread mutex type" << endl;
        ::exit( 1 );
    }
    if ( ::pthread_mutex_init( &machine_lock_, &attr ) ) {
        cerr << "could not init thread mutex" << endl;
        ::exit( 1 );
    }
#endif  /* CHSM_MULTITHREADED */
    for ( int i = 0; i < transitions_in_machine_; ++i ) {
        taken_[ i ] = 0;
        target_[ i ] = 0;
    }
}

//=============================================================================

machine::~machine() {
#ifdef  CHSM_MULTITHREADED
    ::pthread_mutex_destroy( &machine_lock_ );
#endif  /* CHSM_MULTITHREADED */
}

//=============================================================================

void machine::algorithm() {
    if ( in_progress_ )
        return;
    in_progress_ = true;

    if ( debug() & D_alg ) {
        ECHO << "ALGORITHM BEGINNING" ENDL;
        ++debug_indent;
    }

    while ( !event_queue_.empty() ) {
        int const events_in_step = event_queue_.size();
        int i;
        event_queue::iterator e;

        if ( debug() & D_alg )
            ECHO << "events in micro-step: " << events_in_step ENDL;

        //
        // Phase I: Exit the "from" states
        //
        // Iterate through all the events and their transitions and exit the
        // "from" states.  Also perform the transitions' actions, if any.
        //
        // Note: We needn't worry about ensuring parent transitions dominate
        // those of children since the CHSM-to-C++ compiler defines transitions
        // in just the right order.
        //
        if ( debug() & D_alg ) {
            ECHO << "ALGORITHM PHASE I: Exit \"from\" states" ENDL;
            ++debug_indent;
        }

        for ( i = 0, e = event_queue_.begin(); i < events_in_step; ++i, ++e ) {
            //
            // Set our base events' param_blocks to share ours.
            //
            for ( event* e2 = (*e)->base_event_; e2; e2 = e2->base_event_ )
                e2->param_block_ = (*e)->param_block_;

            if ( debug() & D_alg ) {
                ECHO << "iterating transitions of: " << (*e)->name() ENDL;
                ++debug_indent;
            }

            for ( event::const_iterator
                  t = (*e)->begin(); t != (*e)->end(); ++t )
            {
                if ( taken_[ t.id() ] != &**e )
                    continue;
                state *const from = state_[ t->from_id_ ];
                if ( !from->active() ) {
                    //
                    // If the "from" state isn't active, it means a previous
                    // (outer) transition dominated it: unmark this transition
                    // as taken.
                    //
                    taken_[ t.id() ] = 0;
                    continue;
                }

                if ( debug() & D_alg ) {
                    if ( !t->is_internal() ) {
                        ECHO << "performing: "
                             << from->name()
                             << " -> "
                             << target_[ t.id() ]->name()
                        ENDL;
                    }
                    ++debug_indent;
                }

                //
                // If the transition isn't internal, exit the "from" state.  If
                // it actually exited and there's an action to perform, perform
                // it.
                //
                if ( (t->is_internal() || from->exit( **e, target_[ t.id() ] ))
                     && t->action_ != 0 )
                {
                    if ( debug() & D_alg ) {
                        ECHO << "performing action" ENDL;
                        ++debug_indent;
                    }

                    ( this->*(t->action_) )( **e );

                    if ( debug() & D_alg )
                        --debug_indent;
                }
                if ( debug() & D_alg )
                    --debug_indent;
            }
            if ( debug() & D_alg )
                --debug_indent;
        }

        //
        // Phase II: Enter the "to" states
        //
        // Iterate through all the events and their transitions again to enter
        // the inactive "to" states.  Also unmark the transitions.
        //
        if ( debug() & D_alg ) {
            --debug_indent;
            ECHO << "ALGORITHM PHASE II: Enter \"to\" states" ENDL;
            ++debug_indent;
        }

        for ( i = 0, e = event_queue_.begin(); i < events_in_step; ++i, ++e ) {
            if ( debug() & D_alg ) {
                ECHO << "iterating transitions of: " << (*e)->name() ENDL;
                ++debug_indent;
            }

            for ( event::const_iterator
                  t = (*e)->begin(); t != (*e)->end(); ++t )
            {
                if ( taken_[ t.id() ] != &**e )
                    continue;

                if ( debug() & D_alg ) {
                    if ( !t->is_internal() ) {
                        ECHO << "performing: "
                             << state_[ t->from_id_ ]->name()
                             << " -> "
                             << target_[ t.id() ]->name()
                        ENDL;
                    }
                    ++debug_indent;
                }

                if ( !t->is_internal() ) {
                    //
                    // This is a real transition as opposed to an internal one
                    // -- enter the "to" state.
                    //
                    target_[ t.id() ]->enter( **e );
                }
                taken_[ t.id() ] = 0;

                if ( debug() & D_alg )
                    --debug_indent;
            }
            if ( debug() & D_alg )
                --debug_indent;
        }

        //
        // Phase III: Dequeue events
        //
        // All the events in the current micro-step have now been processed --
        // remove them from the queue.
        //
        if ( debug() & D_alg ) {
            --debug_indent;
            ECHO << "ALGORITHM PHASE III: Dequeue events" ENDL;
            ++debug_indent;
        }

        for ( i = 0; i < events_in_step; ++i ) {
            event &e = *event_queue_.front();
            e.broadcasted();
            event_queue_.pop_front();

            if ( debug() & D_event )
                ECHO << "dequeued: " << e.name() ENDL;
        }
        if ( debug() & D_alg )
            --debug_indent;
    }

    in_progress_ = false;

    if ( debug() & D_alg ) {
        --debug_indent;
        ECHO << "ALGORITHM COMPLETE" ENDL;
    }
}

//=============================================================================

void machine::dump_state() const {
    MUTEX_LOCK( &machine_lock_, false );
    ECHO << "current state:" ENDL;
    for ( const_iterator state = begin(); state != end(); ++state )
        ECHO << ' ' << (state->active() ? '*' : ' ') << state->name() ENDL;
    MUTEX_UNLOCK();
}

//=============================================================================

bool machine::enter( event const &trigger ) {
    return root_.enter( trigger );
}

//=============================================================================

bool machine::exit( event const &trigger ) {
    return root_.exit( trigger );
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
