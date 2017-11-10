/*
**      CHSM Language System
**      src/java/chsm2java/chsm.c
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
#include "list_comma.h"
#include "mangle.h"
#include "state.h"
#include "transition.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const  chsm_info::action_prefix_[]      = "A";
char const  chsm_info::chsm_package_name_[]  = "CHSM";
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
        chsm_info::chsm_info( symbol *root, bool is_public,
                              char const *derived )
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
    : root_( root ), derived_( derived ? derived : "" ), public_( is_public )
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
//      Emit a CHSM's Java class declaration.
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
    if ( public_ )
        o << "public ";
    o << "final class " << symbol_->name() << " extends ";

    if ( derived_.length() )
        o << derived_.c_str();
    else
        o << chsm_package_name_ << ".Machine";

    o   << " {" ENDL;

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
    // emit enter/exit action member function declarations
    //
    o ENDL
      << "\t// enter/exit actions" ENDL;
    emit_actions( o, root_ );
    FOR_EACH( state_queue, state_q_, state )
        emit_actions( o, *state );

    emit_definition( o );

    //
    // emit transition condition member function declarations
    //
    o ENDL
      << "\t// transition conditions" ENDL;
    for ( int i = 1; i <= id_.condition_; ++i ) 
        o << "\tprivate static final Thunk_"
          << condition_prefix_ << i << ' '
          << condition_prefix_ << i << " = new Thunk_"
          << condition_prefix_ << i << "();" ENDL;
          
    //
    // emit transition target member function declarations
    //
    o ENDL
      << "\t// transition targets" ENDL;
    for ( int i = 1; i <= id_.target_; ++i )
        o << "\tprivate static final Thunk_"
          << target_prefix_ << i << ' '
          << target_prefix_ << i << " = new Thunk_"
          << target_prefix_ << i << "();" ENDL;

    //
    // emit transition action member function declarations
    //
    o ENDL
      << "\t// transition actions" ENDL;
    for ( int i = 1; i <= id_.action_; ++i )
        o << "\tprivate static final Thunk_"
          << action_prefix_ << i << ' '
          << action_prefix_ << i << " = new Thunk_"
          << action_prefix_ << i << "();" ENDL;

    //
    // emit private data members
    //
    o ENDL
      << "\t// data members" ENDL
      << "\tprivate final "
      << chsm_package_name_ << ".State state_[] = new "
      << chsm_package_name_ << ".State[" << state_q_.size() << "];" ENDL
      << "\tprivate static final "
      << chsm_package_name_ << ".Transition transition_[] = { ";

    if ( CHSM->transition_q_.size() ) {
        list_comma comma;
        FOR_EACH( chsm_info::transition_queue, CHSM->transition_q_, t )
            o << comma ENDL
              << "\t\t" << definition( INFO_CONST( transition, *t ) );
    } else
        o << "null";

    o ENDL
      << "\t};" ENDL;

    g.user_code_->emit( o );
    return o << '}' ENDL;
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
//      Emit a CHSM's Java class definition.
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

    o ENDL
      << "\t// constructor definition" ENDL
      << "\tpublic " << symbol_->name() << "( "
      << param_list( this, param_data::emit_formal ) << " ) {" ENDL
      << "\t\tsuper( " << param_list( this, param_data::emit_actual )
      << " );" ENDL;

    //
    // emit initializers for events
    //
    o ENDL << "\t\t// initialize events" ENDL; 
    FOR_EACH( event_queue, event_q_, event )
        o << "\t\t" << initializer( INFO_CONST( event, *event ) )
          << ';' ENDL;

    //
    // emit initializers for states
    //
    o ENDL
      << "\t\t// initialize states" ENDL
      << indent << '\t'
      << initializer( INFO_CONST( parent, root_ ) ) << ';' ENDL;

    parent_info::child_list const &children =
        INFO_CONST( parent, root_ )->children_;

    FOR_EACH( parent_info::child_list, children, child )
        o << indent << '\t'
          << initializer( INFO_CONST( state, *child ) ) << ';' ENDL;

    //
    // emit state array initialization
    //
        o ENDL << "\t\t// initialize state array" ENDL; 

    int states = 0;
    FOR_EACH( state_queue, state_q_, state )
        o << "\t\tstate_[ " << states++ << " ] = "
          << (*state)->name() << ';' ENDL;

    o ENDL
      << "\t\tinit( " << root_->name() << ", state_, transition_ );" ENDL;

#ifdef CHSM_AUTO_ENTER_EXIT
    //
    // emit the statement to enter the CHSM
    //
    o ENDL
      << "\tenter();" ENDL;
#endif

    o << "\t}" ENDL;
    emitting_constructor_ = false;
    return o;
}
/* vim:set et ts=4 sw=4: */
