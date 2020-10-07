/*
**      CHSM Language System
**      src/c++/chsmc/main.cpp
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
#include "config.h"                     /* must go first */
#include "chsm_info.h"
#include "chsm_compiler.h"
#include "code_generator.h"
#include "child_info.h"
#include "event_info.h"
#include "state_info.h"
#include "options.h"
#include "transition_info.h"
#include "util.h"

// standard
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace PJL;
using namespace std;

symbol*         chsm_compiler::sy_chsm_;

chsm_compiler   cc;

///////////////////////////////////////////////////////////////////////////////

chsm_compiler::chsm_compiler() : dev_null{ nullptr } {
  error_newlined = false;
}

chsm_compiler::~chsm_compiler() {
  // out-of-line since it's non-trivial
}

void chsm_compiler::backpatch_enter_exit_events() {
  for ( auto const &sy_event : CHSM->events_ ) {
    event_info const &info = *INFO_CONST( event, sy_event );
    symbol const *const sy_state = info.sy_state_;
    if ( sy_state == nullptr )
      continue;
    if ( info.kind_ == event_info::KIND_USER ) {
      cc_fatal() << "data error in backpatch_enter_exit_events()" << endl;
      ::exit( EX_SOFTWARE );
    }
    auto &e = INFO( state, sy_state )->event_;
    (info.kind_ == event_info::KIND_ENTER ? e.has_enter_ : e.has_exit_) = true;
  } // for
}

void chsm_compiler::check_child_states_defined() {
  for ( auto const &sym : sym_table_ ) {
    auto const &sy_state = sym.second;
    if ( (type_of( sy_state ) & TYPE(CHILD)) != TYPE(NONE) &&
         sy_state.in_current_scope() &&
         !INFO_CONST( child, &sy_state )->defined_ ) {
      source_->warning( INFO_CONST( base, &sy_state )->first_ref_ )
        << type_string( sy_state )
        << " \"" << sy_state.name() << "\" not defined\n";
    }
  }
}

void chsm_compiler::check_events_used() {
  if ( CHSM == nullptr )
    return;
  for ( auto const &sy_event : CHSM->events_ ) {
    if ( (type_of( sy_event ) & TYPE(USER_EVENT)) != TYPE(NONE) &&
         !INFO_CONST( base, sy_event )->used_ ) {
      source_->warning( INFO_CONST( base, sy_event )->first_ref_ )
        << type_string( *sy_event ) << " \"" << sy_event->name()
        << "\" not used\n";
    }
  } // for
}

void chsm_compiler::check_states_defined() {
  for ( auto const &it : sym_table_ ) {
    auto const &sy_state = it.second;
    if ( (type_of( sy_state ) & TYPE(GLOBAL)) != TYPE(NONE) ) {
      source_->error( INFO_CONST( base, &sy_state )->first_ref_ )
        << type_string( sy_state ) << " \"" << sy_state.name()
        << "\" not defined\n";
    }
  } // for
}

void chsm_compiler::check_transitions() {
  if ( CHSM == nullptr )
    return;
  for ( auto const &sy_t : CHSM->transitions_ ) {
    transition_info const &info = *INFO_CONST( transition, sy_t );
    if ( info.sy_to_ == nullptr )       // internal or a computed target
      continue;

    char const *from = info.sy_from_->name(), *to = info.sy_to_->name();

    //
    // First, check whether the "to" state was defined.
    //
    symbol const &sy_to = sym_table_[ to ];
    if ( !sy_to.info() ) {
      source_->error( info.first_ref_ ) << '"' << to << "\" not defined\n";
      continue;
    }

    //
    // Find where the names differ.
    //
    char const *const from0 = from;
    while ( *from && *to && *from == *to )
      ++from, ++to;

    if ( *from != '\0' && *to != '\0' && from > from0 && from[-1] == '.' ) {
      //
      // The names differ and neither is a subset of the other.  Take the
      // common part of both names -- this is the name of the least-common-
      // ancestor state -- and see if it's a set.
      //
      char name[ 256 ];
      ::strcpy( name, from0 );
      name[ from - from0 - 1 ] = '\0';
      symbol const &sy_ancestor = sym_table_[ name ];
      if ( (type_of( sy_ancestor ) & TYPE(SET)) != TYPE(NONE) )
        source_->error( info.first_ref_ ) << "intra-set transition\n";
    }
  } // for
}

bool chsm_compiler::not_exists( symbol const *sy, symbol_type types ) {
  auto const type = type_of( sy );
  if ( type != TYPE(NONE) &&
       (types == TYPE(NONE) || (type & types) != TYPE(NONE)) ) {
      if ( *sy->name() != '<' ) {
        //
        // Print the error message only if the symbol is not one of our dummy
        // symbols that start with '<'.  (It's impossible for such a symbol to
        // be user-declared since '<' is not in the regular expression for an
        // identifier as specified in lexer.lpp.)
        //
        source_->error()
          << '"' << sy->name() << '"'
          << " previously declared/defined as " << type_string( sy ) << '\n';
      }
      return false;
  }
  return true;
}

std::ostream& cc_error() {
  return std::cerr << me << ": error: ";
}

std::ostream& cc_fatal() {
  return std::cerr << me << ": fatal error: ";
}

std::ostream& cc_warning() {
  return std::cerr << me << ": warning: ";
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
