/*
**      CHSM Language System
**      src/c++/libchsm/machine.cpp -- Run-Time library implementation
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

static unsigned const DEBUG_INDENT_SIZE = 2;  // spaces per indent

state const *const  machine::NIL_         = nullptr;
state::id const     machine::NO_STATE_ID_ = -1;

event const         machine::PRIME_EVENT_
                      ( nullptr, nullptr, "<prime>", nullptr );

///////////////////////////////////////////////////////////////////////////////

machine::machine( CHSM_MACHINE_ARGS ) :
  state_( chsm_state_ ),
  root_( chsm_root_ ),
  transition_( chsm_transition_ ),
  transitions_in_machine_( chsm_transitions_in_machine_ ),
  taken_( chsm_taken_ ),
  target_( chsm_target_ ),
  in_progress_( false ),
  debug_indent_( 0 ),
  debug_state_( DEBUG_NONE )
{
  for ( unsigned i = 0; i < transitions_in_machine_; ++i ) {
    taken_[i] = nullptr;
    target_[i] = nullptr;
  } // for
}

machine::~machine() {
  // out-of-line since it's virtual
}

void machine::algorithm() {
  if ( in_progress_ )
    return;
  in_progress_ = true;

  if ( is_debug( DEBUG_ALGORITHM ) ) {
    dout() << "ALGORITHM BEGINNING" ENDL;
    ++debug_indent_;
  }

  while ( !event_queue_.empty() ) {
    event_queue::size_type const events_in_step = event_queue_.size();
    event_queue::size_type i;
    event_queue::iterator e_it;

    if ( is_debug( DEBUG_ALGORITHM ) )
      dout() << "events in micro-step: " << events_in_step ENDL;

    //
    // Phase I: Exit the "from" states
    //
    // Iterate through all the events and their transitions and exit the "from"
    // states.  Also perform the transitions' actions, if any.
    //
    // Note: We needn't worry about ensuring parent transitions dominate those
    // of children since the CHSM-to-C++ compiler defines transitions in just
    // the right order.
    //
    if ( is_debug( DEBUG_ALGORITHM ) ) {
      dout() << "ALGORITHM PHASE I: Exit \"from\" states" ENDL;
      ++debug_indent_;
    }

    for ( i = 0, e_it = event_queue_.begin(); i < events_in_step;
          ++i, ++e_it ) {
      event const &cur_event = **e_it;
      //
      // Set our base events' param_blocks to share ours.
      //
      for ( auto base = cur_event.base_event_; base != nullptr;
            base = base->base_event_ ) {
        base->param_block_ = cur_event.param_block_;
      } // for

      if ( is_debug( DEBUG_ALGORITHM ) ) {
        dout() << "iterating transitions of: " << cur_event.name() ENDL;
        ++debug_indent_;
      }

      for ( auto t = cur_event.begin(); t != cur_event.end(); ++t ) {
        if ( taken_[ t.id() ] != &cur_event )
          continue;
        state *const from = state_[ t->from_id_ ];
        if ( !from->active() ) {
          //
          // If the "from" state isn't active, it means a previous (outer)
          // transition dominated it: unmark this transition as taken.
          //
          taken_[ t.id() ] = nullptr;
          continue;
        }

        if ( is_debug( DEBUG_ALGORITHM ) ) {
          if ( !t->is_internal() ) {
            dout()
              << "performing: " << from->name() << " -> "
              << target_[ t.id() ]->name()
            ENDL;
          }
          ++debug_indent_;
        }

        //
        // If the transition isn't internal, exit the "from" state.  If it
        // actually exited and there's an action to perform, perform it.
        //
        if ( (t->is_internal() || from->exit( cur_event, target_[ t.id() ] ))
             && t->action_ != nullptr ) {
          if ( is_debug( DEBUG_ALGORITHM ) ) {
            dout() << "performing action" ENDL;
            ++debug_indent_;
          }

          try {
            (this->*(t->action_))( cur_event );
          }
          catch ( ... ) {
            //
            // Ignore any exception the action may have thrown.
            //
          }

          if ( is_debug( DEBUG_ALGORITHM ) )
            --debug_indent_;
        }

        if ( is_debug( DEBUG_ALGORITHM ) )
          --debug_indent_;
      } // for

      if ( is_debug( DEBUG_ALGORITHM ) )
        --debug_indent_;
    } // for

    //
    // Phase II: Enter the "to" states
    //
    // Iterate through all the events and their transitions again to enter the
    // inactive "to" states.  Also unmark the transitions.
    //
    if ( is_debug( DEBUG_ALGORITHM ) ) {
      --debug_indent_;
      dout() << "ALGORITHM PHASE II: Enter \"to\" states" ENDL;
      ++debug_indent_;
    }

    for ( i = 0, e_it = event_queue_.begin(); i < events_in_step;
          ++i, ++e_it ) {
      event const &cur_event = **e_it;
      if ( is_debug( DEBUG_ALGORITHM ) ) {
        dout() << "iterating transitions of: " << cur_event.name() ENDL;
        ++debug_indent_;
      }

      for ( auto t = cur_event.begin(); t != cur_event.end(); ++t ) {
        if ( taken_[ t.id() ] != &cur_event )
          continue;

        if ( is_debug( DEBUG_ALGORITHM ) ) {
          if ( !t->is_internal() ) {
            dout()
              << "performing: " << state_[ t->from_id_ ]->name() << " -> "
              << target_[ t.id() ]->name()
            ENDL;
          }
          ++debug_indent_;
        }

        if ( !t->is_internal() ) {
          //
          // This is a real transition as opposed to an internal one -- enter
          // the "to" state.
          //
          target_[ t.id() ]->enter( cur_event );
        }
        taken_[ t.id() ] = nullptr;

        if ( is_debug( DEBUG_ALGORITHM ) )
          --debug_indent_;
      }

      if ( is_debug( DEBUG_ALGORITHM ) )
        --debug_indent_;
    } // for

    //
    // Phase III: Dequeue events
    //
    // All the events in the current micro-step have now been processed --
    // remove them from the queue.
    //
    if ( is_debug( DEBUG_ALGORITHM ) ) {
      --debug_indent_;
      dout() << "ALGORITHM PHASE III: Dequeue events" ENDL;
      ++debug_indent_;
    }

    for ( i = 0; i < events_in_step; ++i ) {
      event &cur_event = *event_queue_.front();
      cur_event.broadcasted();
      event_queue_.pop_front();

      if ( is_debug( DEBUG_EVENTS ) )
        dout() << "dequeued: " << cur_event.name() ENDL;
    } // for

    if ( is_debug( DEBUG_ALGORITHM ) )
      --debug_indent_;
  } // while

  in_progress_ = false;

  if ( is_debug( DEBUG_ALGORITHM ) ) {
    --debug_indent_;
    dout() << "ALGORITHM COMPLETE" ENDL;
  }
}

ostream& machine::dout() const {
  cerr << '|';
  for ( unsigned i = debug_indent_ * DEBUG_INDENT_SIZE; i > 0; --i )
    cerr << ' ';
  return cerr;
}

void machine::dump_state() const {
  lock_type const lock( mutex_ );
  dout() << "current state:" ENDL;
  for ( auto const &state : *this )
    dout() << ' ' << (state.active() ? '*' : ' ') << state.name() ENDL;
}

bool machine::enter( event const &trigger ) {
  // out-of-line since it's virtual
  return root_.enter( trigger );
}

bool machine::exit( event const &trigger ) {
  // out-of-line since it's virtual
  return root_.exit( trigger );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
