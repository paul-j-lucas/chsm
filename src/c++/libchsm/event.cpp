/*
**      CHSM Language System
**      src/c++/libchsm/event.cpp -- Run-Time library implementation
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

// standard
#include <cassert>

using namespace std;

namespace CHSM_NS {

transition::id const event::NO_TRANSITION_ID_ = -1;

////////// inline functions ///////////////////////////////////////////////////

inline bool event::is_debug_events() const {
  return machine_.is_debug( machine::DEBUG_EVENTS );
}

inline ostream& event::dout() const {
  return machine_.dout();
}

///////////////////////////////////////////////////////////////////////////////

event::event( CHSM_EVENT_ARGS ) :
  machine_{ *chsm_machine_ },
  in_progress_{ 0 },
  param_block_{ nullptr },
  name_{ chsm_name_ },
  base_event_{ chsm_base_event_ },
  transitions_{ chsm_transition_list_ }
{
  // do nothing else
}

event::~event() {
  // out-of-line since it's virtual
}

void event::broadcast( void *pb ) {
  if ( in_progress_ > 0 )               // we're already in progress
    return;

  //
  // Mark ourselves and our base event(s), if any, as being in progress.
  //
  for ( auto e = this; e != nullptr; e = e->base_event_ )
    ++e->in_progress_;

  if ( is_debug_events() )
    dout() << "broadcast: " << name() ENDL;

  bool is_precondition_true;
  if ( (param_block_ = pb) != nullptr ) {
    //
    // Evaluate the event's precondition.  Note that since it's a virtual
    // function, the most-derived precondition gets executed.  However, the
    // code emitted by the CHSM-to-C++ compiler is of the form:
    //
    //      return base_event::precondition() && user_precondition();
    //
    // so the most-base precondition really gets evaluated first.  If a base
    // event's precondition is false, the short-circuit semantics of && means
    // no more derived preconditions get evaluated and the call-stack simply
    // unwinds.  The final result is that an event's precondition is the
    // logical and of all base preconditions executed in base-to-derived order.
    //
    try {
      is_precondition_true = static_cast<param_block*>( pb )->precondition();
    }
    catch ( ... ) {
      //
      // Consider an exception thrown by a precondition to mean false.
      //
      is_precondition_true = false;
    }

    if ( is_debug_events() )
      dout()
        << "+ precondition: " << (is_precondition_true ? "true" : "false") ENDL;
  }
  else {
    is_precondition_true = true;
  }

  if ( is_precondition_true && find_transition() ) {
    //
    // Queue ourselves and run algorithm.
    //
    machine_.event_queue_.push_back( this );
    if ( is_debug_events() )
      dout() << "queued   : " << name() ENDL;
    machine_.algorithm();
  }
  else {
    //
    // This event is not to be queued: destroy its parameter block and return.
    //
    if ( is_debug_events() )
      dout() << "broadcast: " << name() << " -- cancelled" ENDL;
    broadcasted();
  }
}

void event::broadcasted() {
  //
  // This check isn't strictly necessary...but I feel better having it here.
  //
  if ( in_progress_ > 0 ) {
    //
    // Unmark ourselves and our base events, if any, as being in progress.
    //
    for ( auto e = this; e != nullptr; e = e->base_event_ )
      if ( e->in_progress_ > 0 )
        --e->in_progress_;

    if ( param_block_ != nullptr ) {
      //
      // Since a param_block object is never created nor destroyed (in terms of
      // allocation and deallocation), we have to call its destructor
      // explicitly to destroy (but not deallocate) it.
      //
      static_cast<param_block*>( param_block_ )->param_block::~param_block();
    }
  }
}

bool event::find_transition() {
  if ( is_debug_events() )
    dout() << "+ checking transitions" ENDL;

  bool found = false;

  //
  // Iterate through our event's transitions finding those that will be taken,
  // if any.
  //
  for ( auto t = begin(); t != end(); ++t ) {
    try {
      state &from = *machine_.state_[ t->from_id_ ];
      //
      // We must check specifically for "plain active" so we don't make more
      // than one nondeterministic transition from the same state.  This is the
      // only place in the code that makes a distinction between "plain active"
      // and "active disabled."
      //
      if ( from.state_ != state::STATE_ACTIVE )
        continue;

      //
      // If the transition has a condition, evaluate it to see whether we
      // should continue.
      //
      if ( t->condition_ != nullptr && !(machine_.*(t->condition_))( *this ) )
        continue;

      //
      // Mark this transition as taken using the event that triggered it.
      //
      if ( machine_.taken_[ t.id() ] == nullptr )
        machine_.taken_[ t.id() ] = this;

      if ( !t->is_internal() ) {
        if ( t->target_ != nullptr ) {
          //
          // The transition has a target function: execute it to compute the
          // target state.
          //
          if ( auto to = (machine_.*(t->target_))( *this ) ) {
            assert( transition::is_legal( &from, to ) );
            machine_.target_[ t.id() ] = to;
          }
          else {
            //
            // The target function returned null: skip this transition.
            //
            continue;
          }
        }
        else {
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
        from.state_ = state::STATE_ACTIVE_DISABLED;
      }

      found = true;

      if ( is_debug_events() ) {
        dout() << "+ found  : " << from.name();
        if ( !t->is_internal() )
          MORE << " -> " << machine_.target_[ t.id() ]->name();
        else
          MORE << " (internal)";
        MORE ENDL;
      }
    }
    catch ( ... ) {
      //
      // Ignore any exception that either a condition or a target function may
      // have thrown.
      //
    }
  } // for

  return found;
}

void event::lock_broadcast() {
  machine_lock const lock{ machine_ };
  broadcast( nullptr );
}

///////////////////////////////////////////////////////////////////////////////

void event::const_iterator::bump() {
  while ( *t_id_ == NO_TRANSITION_ID_ && base_event_ != nullptr ) {
    //
    // We've exhausted our transitions; proceed to those of our base event's,
    // if any.
    //
    t_id_ = base_event_->transitions_;
    base_event_ = base_event_->base_event_;
  } // while
}

event::param_block::~param_block() {
  // out-of-line since it's virtual
}

bool event::param_block::precondition() const {
  // out-of-line since it's virtual
  return true;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace
/* vim:set et sw=2 ts=2: */
