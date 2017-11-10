/*
**      CHSM Language System
**      src/c++/chsm2c++/state.c
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
#include "mangle.h"
#include "state.h"
#include "util.h"

using namespace PJL;
using namespace std;

int state_info::nesting_depth_  = 1;

CHSM_DEFINE_RTTI( state_info, global_info, TYPE(state) )

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& indent( ostream &o )
//
// DESCRIPTION
//
//      Emit the right number of tab characters for the current child nesting
//      depth.
//
// PARAMETERS
//
//      o   The ostream to write to.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    for ( register int i = state_info::nesting_depth_; i > 0; --i )
        o << '\t';
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        state_info::state_info( symbol const *parent, char const *derived ) :
//
// DESCRIPTION
//
//      Construct a state_info.
//
// PARAMETERS
//
//      parent      The symbol of the parent state of this state, if any.
//
//      derived     The derived class name, if any.
//
//*****************************************************************************
    class_name_( make_class_name( "state" ) ),
    parent_( parent ),
    derived_( derived ? derived : "" )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        state_info::state_info( char const *class_name, symbol const *parent,
                                char const *derived )
//
// DESCRIPTION
//
//      Construct a state_info.
//
// PARAMETERS
//
//      class_name  The class name for this state.
//
//      parent      The symbol of the parent state of this state, if any.
//
//      derived     The derived class name for this state, if any.
//
//*****************************************************************************
    : class_name_( class_name ), parent_( parent ),
      derived_( derived ? derived : "" )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* state_info::class_name() const
//
// DESCRIPTION
//
//      Return this class's name: if a class has a derived name, return that
//      one.
//
// RETURNS
//
//      Returns said class name.
//
//*****************************************************************************
{
    return derived_.length() ? derived_.c_str() : class_name_;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& state_info::common_declaration( ostream &o ) const
//
// DESCRIPTION
//
//      Emit the common portion of this state's declaration.
//
// PARAMETERS
//
//      o   The ostream to emit to.
//
// RETURNS
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    o << indent << class_name() << ' ' << state_base( symbol_->name() )
      << "; // " << ::serial( symbol_ );
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& state_info::common_initializer( ostream &o ) const
//
// DESCRIPTION
//
//      o   The ostream to emit to.
//
// PARAMETERS
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    char const *const m_name = mangle( symbol_->name() );
    //
    // If we're emitting a mem-initializer for the CHSM constructor, the
    // reference to the CHSM is *this; otherwise, we're emitting for a parent
    // class in which case the reference to the CHSM is the chsm_ data member.
    //
    char const *const chsm_ref
        = chsm_info::emitting_constructor_ ? "*this" : "chsm_machine_";

    o << '\t' << state_base( symbol_->name() ) << "( " << chsm_ref
      << ", \"" << symbol_->name() << "\", "
      << ( parent_ ?
            //
            // The root state is "special" in that its children are not
            // lexically enclosed by it; hence, we have to emit "&root" rather
            // than "this".
            //
            ( parent_ == ROOT_SYM ? "&root" : "this" ) :
            //
            // The root state is the only state with no parent.
            //
            "0"
         )
      << ", ";

    //
    // enter/exit actions
    //
    if ( action_.enter_ )
        o << "(" << chsm_info::chsm_ns_alias_
          << "::state::action)&" << g.chsm_->name() << "::"
          << chsm_info::enter_prefix_
          << chsm_info::action_prefix_ << m_name;
    else
        o << 0;
    o << ", ";
    if ( action_.exit_ )
        o << "(" << chsm_info::chsm_ns_alias_
          << "::state::action)&" << g.chsm_->name() << "::"
          << chsm_info::exit_prefix_
          << chsm_info::action_prefix_ << m_name;
    else
        o << 0;
    o << ", ";

    //
    // enter/exit events
    //
    if ( event_.enter_ )
        o << "&((" << g.chsm_->name() << "&)" << chsm_ref << ")."
          << chsm_info::enter_prefix_ << m_name;
    else
        o << 0;
    o << ", ";
    if ( event_.exit_ )
        o << "&((" << g.chsm_->name() << "&)" << chsm_ref << ")."
          << chsm_info::exit_prefix_ << m_name;
    else
        o << 0;

    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& state_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this state's declaration.
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
    return common_declaration( o );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& state_info::emit_initializer( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this state's initializer.
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
    return common_initializer( o ) << " )";
}
/* vim:set et ts=4 sw=4: */
