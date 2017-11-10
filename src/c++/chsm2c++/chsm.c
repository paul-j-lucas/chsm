/*
**      CHSM Language System
**      src/c++/chsm2c++/chsm.c
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
#include <iostream>

// local
#include "chsm.h"
#include "chsm_compiler.h"
#include "cluster.h"
#include "event.h"
#include "mangle.h"
#include "state.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const  chsm_info::action_prefix_[]      = "A";
char const  chsm_info::chsm_ns_alias_[]      = "CHSM_ns_alias";
char const  chsm_info::condition_prefix_[]   = "C";
bool        chsm_info::emitting_constructor_ = false;
char const  chsm_info::enter_prefix_[]       = "E";
char const  chsm_info::exit_prefix_[]        = "X";
char const  chsm_info::target_prefix_[]      = "T";

CHSM_DEFINE_RTTI( chsm_info, global_info, TYPE(chsm) )

//*****************************************************************************
//
// SYNOPSIS
//
        chsm_info::chsm_info( symbol *root, char const *derived ) :
//
// DESCRIPTION
//
//      Construct a chsm_info object.
//
// PARAMETERS
//
//      root        The symbol of the root cluster of the CHSM.
//
//      derived     The derived class name, if any.
//
//*****************************************************************************
    root_( root ),
    derived_( derived ? derived : "" )
{
    // do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& chsm_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit a CHSM's C++ class declaration.
//
// PARAMETERS
//
//      o   The ostream to emit to.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    o << "class " << symbol_->name() << " : public ";

    if ( derived_.length() )
        o << derived_.c_str();
    else
        o << chsm_ns_alias_ << "::machine";

    o << " {" ENDL
      << "public:" ENDL;

    //
    // emit constructor declaration
    //
    o << '\t' << symbol_->name() << "( " << param_list( this )
      << ");" ENDL;

    //
    // emit destructor declaration
    //
    o << "\t~" << symbol_->name() << "();" ENDL;

    //
    // emit state declarations, recursively
    //
    o ENDL
      << "\t// states" ENDL
      << declaration( INFO_CONST( state, root_ ) ) ENDL;

    parent_info::child_list const &children =
        INFO_CONST( parent, root_ )->children_;

    FOR_EACH( parent_info::child_list, children, child )
        o << declaration( INFO_CONST( state, *child ) ) ENDL;

    //
    // emit event declarations
    //
    o ENDL
      << "\t// events" ENDL;

    FOR_EACH( event_queue, event_q_, event )
        o << declaration( INFO_CONST( event, *event ) ) ENDL;

    //
    // emit transition condition member function declarations
    //
    o ENDL
      << "\t// transition conditions" ENDL;
    for ( int i = 1; i <= id_.condition_; ++i )
        o << "\tbool " << condition_prefix_ << i
          << "( " << chsm_ns_alias_ << "::event const& );" ENDL;

    //
    // emit transition target member function declarations
    //
    o ENDL
      << "\t// transition targets" ENDL;
    for ( int i = 1; i <= id_.target_; ++i )
        o << '\t' << chsm_ns_alias_ << "::state* "<< target_prefix_ << i
          << "( " << chsm_ns_alias_ << "::event const& );" ENDL;

    //
    // emit transition action member function declarations
    //
    o ENDL
      << "\t// transition actions" ENDL;
    for ( int i = 1; i <= id_.action_; ++i )
        o << "\tvoid " << action_prefix_ << i
          << "( " << chsm_ns_alias_ << "::event const& );" ENDL;

    //
    // emit enter/exit action member function declarations
    //
    o ENDL
      << "\t// enter/exit actions" ENDL;

    emit_actions( o, root_ );

    FOR_EACH( state_queue, state_q_, state )
        emit_actions( o, *state );

    //
    // emit data members
    //
    o << "private:" ENDL
      << '\t' << chsm_ns_alias_ << "::state *state_[ "
      << state_q_.size() + 1
      << " ];" ENDL
      << "\tstatic " << chsm_ns_alias_
      << "::transition const transition_[];" ENDL
      << '\t' << chsm_ns_alias_ << "::event const *taken_[ "

      // degenerate case: no transitions; silly, but legal
      << ( transition_q_.size() ? transition_q_.size() : 1 )

      << " ];" ENDL

      << '\t' << chsm_ns_alias_ << "::state *target_[ "
      << ( transition_q_.size() ? transition_q_.size() : 1 )
      << " ];" ENDL

      << "};" ENDL;
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& chsm_info::emit_actions( ostream &o,
                                          symbol const *state ) const
//
// DESCRIPTION
//
//      Emit the enter and exit actions, if any, for a state.
//
// PARAMETERS
//
//      o       The ostream to emit to.
//
//      state   The state whose actions are to be emitted.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    state_info::actions const &action = INFO_CONST( state, state )->action_;

    if ( action.enter_ )
        o << "\tvoid " << enter_prefix_ << action_prefix_
          << mangle( state->name() )
          << "( " << chsm_ns_alias_ << "::state const&, "
          << chsm_ns_alias_ << "::event const& );" ENDL;
    if ( action.exit_ )
        o << "\tvoid " << exit_prefix_ << action_prefix_
          << mangle( state->name() )
          << "( " << chsm_ns_alias_ << "::state const&, "
          << chsm_ns_alias_ << "::event const& );" ENDL;
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& chsm_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit a CHSM's C++ class definition.
//
// PARAMETERS
//
//      o   The ostream to emit to.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    //
    // Set to true to alter how states emit their mem-initializers.
    //
    emitting_constructor_ = true;

    o << symbol_->name() << "::" << symbol_->name() << "( "
      << param_list( this, param_data::emit_prefix | param_data::emit_formal )
      << " ) :" ENDL
      << '\t';

    if ( derived_.length() )
        o << derived_.c_str();
    else
        o << chsm_ns_alias_ << "::machine";

    o << "( state_, root, transition_, taken_, target_, "
      << transition_q_.size()
      << param_list( this,
            param_data::emit_comma |
            param_data::emit_actual |
            param_data::emit_prefix )
      << " )";

    //
    // emit member-initializers for states
    //
    o << ',' ENDL
      << initializer( INFO_CONST( parent, root_ ) );

    parent_info::child_list const &children =
        INFO_CONST( parent, root_ )->children_;

    FOR_EACH( parent_info::child_list, children, child )
        o << ',' ENDL
          << initializer( INFO_CONST( state, *child ) );

    //
    // emit member-initializers for events
    //
    FOR_EACH( event_queue, event_q_, event )
        o << ',' ENDL
          << initializer( INFO_CONST( event, *event ) );
    o ENDL
      << '{' ENDL;

    //
    // emit state vector initialization
    //
    int states = 0;

    FOR_EACH( state_queue, state_q_, state )
        o << "\tstate_[ " << states++ << " ] = &"
          << (*state)->name() << ';' ENDL;
    o << "\tstate_[ " << states << " ] = 0;" ENDL;

#ifdef CHSM_AUTO_ENTER_EXIT
    //
    // emit the statement to enter the CHSM
    //
    o ENDL
      << "\tenter();" ENDL;
#endif

    o << '}' ENDL;

    //
    // emit destructor definition
    //
    o << symbol_->name() << "::~" << symbol_->name() << "() {" ENDL
#ifdef CHSM_AUTO_ENTER_EXIT
      << "\texit();" ENDL
#endif
      << '}' ENDL;

    return o;
}
/* vim:set et ts=4 sw=4: */
