/*
**      CHSM Language System
**      src/java/chsm2java/set.c
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
#include "set.h"
#include "util.h"

using namespace PJL;
using namespace std;

CHSM_DEFINE_RTTI( set_info, parent_info, TYPE(set) )

//*****************************************************************************
//
// SYNOPSIS
//
        set_info::set_info( symbol const *parent, char const *derived )
//
// DESCRIPTION
//
//      Construct a set_info.
//
// PARAMETERS
//
//      parent      The symbol of the parent state of this state, if any.
//
//      derived     The derived class name, if any.
//
//*****************************************************************************
    : parent_info( make_class_name( "Set" ), parent, derived )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& set_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this set's declaration.
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
        ostream& set_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this set's definition.
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
    return common_definition( o );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& set_info::emit_initializer( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this set's initializer.
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
    char base_name[ 256 ];
    ::strcpy( base_name, state_base( symbol_->name() ) );

    o << '\t' << state_base( symbol_->name() ) << " = new "
      << class_prefix_ << base_name;

    return common_initializer( o ) << " )";
}
/* vim:set et ts=4 sw=4: */
