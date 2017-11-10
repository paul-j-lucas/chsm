/*
**      CHSM Language System
**      src/java/chsm2java/transition.c
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
#include "state.h"
#include "transition.h"

using namespace PJL;
using namespace std;

char const transition_info::vector_suffix_[] = "Transitions";

//*****************************************************************************
//
// SYNOPSIS
//
        transition_info::transition_info( int condition_id, symbol const *from,
                                          symbol const *to, int target_id,
                                          int action_id )
//
// DESCRIPTION
//
//      Construct a transition_info.
//
// PARAMETERS
//
//      condition_id    The index ID of the condition to perform.
//
//      from            The symbol representing the "from" state.
//
//      to              The symbol representing the "to" state.
//
//      target_id       The index ID of the target function to perform, if any.
//
//      action_id       The index ID of the action to perform.
//
//*****************************************************************************
    : condition_id_( condition_id ),
      from_( from ), to_( to ), target_id_( target_id ),
      action_id_( action_id )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& transition_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit the definition for this transition to an ostream.
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
    o  << "new CHSM.Transition( ";

    if ( condition_id_ )
        o << chsm_info::condition_prefix_ << condition_id_;
    else
        o << "null";

    o << ", " << ::serial( from_ ) << ", " << ::serial( to_ ) << ", ";

    if ( target_id_ )
        o << chsm_info::target_prefix_ << target_id_;
    else
        o << "null";

    o << ", ";

    if ( action_id_ )
        o << chsm_info::action_prefix_ << action_id_;
    else
        o << "null";

    o << " )";

    return o;
}
/* vim:set et ts=4 sw=4: */
