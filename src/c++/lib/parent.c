/*
**      CHSM Language System
**      src/c++/lib/parent.c -- Run-Time library implementation
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

int const parent::no_child_id_ = -1;

//=============================================================================

parent::parent( CHSM_PARENT_ARGS ) :
    state( CHSM_STATE_INIT ),
    children_( chsm_children_ )
{
    // do nothing else
}

//=============================================================================

void parent::deep_clear() {
    for ( iterator child = begin(); child != end(); ++child )
        child->deep_clear();
}

//=============================================================================

bool parent::switch_child( state* ) {
    return true;
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
