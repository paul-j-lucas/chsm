/*
**      CHSM Language System
**      src/c++/chsmc/xxx_info.cpp
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

/**
 * @file
 * Defines out-of-line constructors and member functions for info classes.
 */

// local
#include "base_info.h"
#include "child_info.h"
#include "chsm_compiler.h"
#include "chsm_info.h"
#include "cluster_info.h"
#include "event_info.h"
#include "global_info.h"
#include "info_visitor.h"
#include "parent_info.h"
#include "set_info.h"
#include "state_info.h"
#include "transition_info.h"
#include "user_event_info.h"

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

base_info::base_info( scope_type scope ) :
  synfo{ scope },
  first_ref_{ cc.source_ != nullptr ? cc.source_->line_no_ : 0 },
  used_{ false }
{
}

ostream& base_info::emit( ostream &o ) const {
  o << type_string( type() ) << ' ';
  return synfo::emit( o );
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( child_info, TYPE(CHILD) );

child_info::child_info( symbol const *sy_parent ) :
  //
  // When the scope of a cluster or set is exited, the child-states' symbols
  // need to be destroyed.  A cluster's or set's scope is exited upon
  // encountering the closing } describing its body, which is in the next
  // *higher* scope:
  //
  //     cluster x(a,b) is { /* ... */ } // ...
  //     ^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^ ^^^^^^
  //             |               |          |
  //          scope n        scope n+1   scope n
  //
  // Hence, to get the child-states' symbols destroyed, it is necessary to
  // "push" them into the next scope, hence the + 1 in the mem-initialzer
  // below.
  //
  base_info{ symbol_table::scope() + 1 },
  sy_parent_{ sy_parent },
  defined_{ false }
{
}

///////////////////////////////////////////////////////////////////////////////

char const chsm_info::PREFIX_ACTION[]     = "A";
char const chsm_info::PREFIX_CONDITION[]  = "C";
char const chsm_info::PREFIX_ENTER[]      = "E";
char const chsm_info::PREFIX_EXIT[]       = "X";
char const chsm_info::PREFIX_TARGET[]     = "T";

CHSM_DEFINE_RTTI( chsm_info, TYPE(CHSM) );

chsm_info::chsm_info( symbol *sy_root, char const *derived, bool java_public ) :
  sy_root_{ sy_root },
  derived_{ derived != nullptr ? derived : "" },
  java_public_{ java_public }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( cluster_info, TYPE(CLUSTER) );

cluster_info::cluster_info( symbol const *sy_parent, char const *derived,
                            bool history ) :
  parent_info{ sy_parent, derived },
  history_{ history }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( event_info, TYPE(ENEX_EVENT) );

event_info::event_info( kind k, PJL::symbol const *sy_state ) :
  kind_{ k },
  sy_state_{ sy_state }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( global_info, TYPE(GLOBAL) );

///////////////////////////////////////////////////////////////////////////////

parent_info::parent_info( PJL::symbol const *sy_parent, char const *derived ) :
  state_info{ sy_parent, derived }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( set_info, TYPE(SET) );

set_info::set_info( PJL::symbol const *sy_parent, char const *derived ) :
  parent_info{ sy_parent, derived }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( state_info, TYPE(STATE) );

state_info::state_info( symbol const *sy_parent, char const *derived ) :
  sy_parent_{ sy_parent },
  derived_{ derived != nullptr ? derived : "" }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( transition_info, TYPE(TRANSITION) );

transition_info::transition_info( condition_id cid, symbol const *sy_from,
                                  symbol const *sy_to, target_id tid,
                                  action_id aid ) :
  sy_from_{ sy_from },
  sy_to_{ sy_to },
  condition_id_{ cid },
  target_id_{ tid },
  action_id_{ aid }
{
}

///////////////////////////////////////////////////////////////////////////////

CHSM_DEFINE_RTTI( user_event_info, TYPE(USER_EVENT) );

user_event_info::user_event_info( symbol const *sy_base_event ) :
  sy_base_event_{ sy_base_event },
  precondition_{ PRECONDITION_NONE }
{
}

ostream& user_event_info::emit_param_list_aux( ostream &o, list_sep &comma,
                                               emit_mask emit_flags ) const {
  if ( sy_base_event_ != nullptr ) {
    INFO_CONST( user_event, sy_base_event_ )
      ->emit_param_list_aux( o, comma, emit_flags );
  }

  return param_data::emit_param_list_aux( o, comma, emit_flags );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
