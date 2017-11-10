/*
**      CHSM Language System
**      src/c++/lib/set.c -- Run-Time library implementation
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

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"

namespace CHSM_NS {

//=============================================================================

bool set::enter( event const &trigger, state* ) {
    if ( !state::enter( trigger ) )
        return false;

    // enter all of our children
    for ( iterator child = begin(); child != end(); ++child )
        child->enter( trigger );

    return true;
}

//=============================================================================

bool set::exit( event const &trigger, state *to ) {
    if ( !active() )
        return false;

    // exit all of our children
    for ( iterator child = begin(); child != end(); ++child )
        child->exit( trigger );

    return state::exit( trigger, to );
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
