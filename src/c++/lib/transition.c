/*
**      CHSM Language System
**      src/c++/lib/transition.c -- Run-Time library implementation
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
#include <list>

// local
#define CHSM_NO_ALIAS_NS
#include "chsm.h"

using namespace std;

namespace CHSM_NS {

//=============================================================================

bool transition::is_legal( state const *s1, state const *s2 ) {
    typedef list<state const*> state_list;
    state_list s1_list, s2_list;

    for ( state const *s = s1; s; s = s->parent_of() )
        s1_list.push_front( s );
    for ( state const *s = s2; s; s = s->parent_of() )
        s2_list.push_front( s );

    state_list::const_iterator i = s1_list.begin();
    state_list::const_iterator j = s2_list.begin();

    state const *nearest_common_ancestor = 0;
    while ( i != s1_list.end() && j != s2_list.end() && *i == *j ) {
        nearest_common_ancestor = *i;
        ++i, ++j;
    }
    return !dynamic_cast<set const*>( nearest_common_ancestor );
}

//=============================================================================

} // namespace
/* vim:set et sw=4 ts=4: */
