/*
**      CHSM Language System
**      src/common/chsm2xxx/child.c
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
#include "child.h"
#include "util.h"

using namespace PJL;
using namespace std;

CHSM_DEFINE_RTTI( child_info, base_info, TYPE(child) )

//*****************************************************************************
//
// SYNOPSIS
//
        child_info::child_info( symbol const *parent ) :
//
// DESCRIPTION
//
//      Construct a child_info.
//
//      When the scope of a cluster or set is exited, the child-states' symbols
//      need to be destroyed.  A cluster's or set's scope is exited upon
//      encountering the closing } describing its body, which is in the next
//      *higher* scope:
//
//          cluster x(a,b) is { /* ... */ } // ...
//          ^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^ ^^^^^^
//                  |               |          |
//               scope n        scope n+1   scope n
//
//      Hence, to get the child-states' symbols destroyed, it is necessary to
//      "push" them into the next scope, hence the + 1 in the mem-initialzer
//      below.
//
//*****************************************************************************
    base_info( symbol_table::scope() + 1 ),
    parent_( parent ),
    defined_( false )
{
    // do nothing else
}
/* vim:set et ts=4 sw=4: */
