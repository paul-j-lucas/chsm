/*
**      CHSM Language System
**      src/java/chsm2java/cluster.c
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
#include "util.h"

using namespace PJL;
using namespace std;

CHSM_DEFINE_RTTI( cluster_info, parent_info, TYPE(cluster) )

//*****************************************************************************
//
// SYNOPSIS
//
        cluster_info::cluster_info( symbol const *parent, char const *derived,
                                    bool history )
//
// DESCRIPTION
//
//      Construct a cluster_info.
//
// PARAMETERS
//
//      parent      The symbol of the parent state of this state, if any.
//
//      derived     The derived class name, if any.
//
//      history     True only if this cluster has a history.
//
//*****************************************************************************
    : parent_info( make_class_name( "Cluster" ), parent, derived ),
      history_( history )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& cluster_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this cluster's declaration.
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
    return common_declaration( o, "boolean chsmH" );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& cluster_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this cluster's definition.
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
    return common_definition( o, "boolean chsmH", "chsmH" );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& cluster_info::emit_initializer( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this cluster's initializer.
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
    char const *const base_name = state_base( symbol_->name() );

    o << base_name << " = new " << class_prefix_ << base_name;
    return  common_initializer( o ) << ", "
            << ( history_ ? "true" : "false" ) << " )";
}
/* vim:set et ts=4 sw=4: */
