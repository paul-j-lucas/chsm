/*
**      CHSM Language System
**      src/common/chsm2xxx/param_data.c
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
#include <cstring>
#include <iostream>

// local
#include "list_comma.h"
#include "param_data.h"
#include "util.h"

using namespace std;

unsigned    param_data::default_emit_flags_ = 0;
char const  param_data::param_prefix_[]     = "P";

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& param_data::emit_param_list( ostream &o, unsigned flags ) const
//
// DESCRIPTION
//
//      We need to use a single list_comma object across all the recursion;
//      hence, we declare one here and call a second function to do the
//      recursion passing a reference to the comma object along.
//
// PARAMETERS
//
//      o       The ostream to emit the parameter list to.
//
//      flags   Flags to control how things are emitted.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    list_comma comma;
    if ( flags & emit_comma )
        comma.print();
    return emit_param_list_aux( o, comma, flags );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& param_data::emit_param_list_aux( ostream &o,
                                                  list_comma &comma,
                                                  unsigned flags ) const
//
// DESCRIPTION
//
//      Emit a parameter list in a form dictated by various flags.  This
//      function may be overridden by derived classes.
//
// PARAMETERS
//
//      o       The ostream to emit the parameter list to.
//
//      comma   The list_comma whose state should be preserved across calls.
//
//      flags   Flags to control how things are emitted.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    FOR_EACH( param_list_type, param_list_, param ) {
        o << comma;
        if ( flags & emit_actual ) {
            //
            // Emit an actual parameter list, i.e., just the names.
            //
            o << ( flags & emit_prefix ? param_prefix_ : "" )
              << (*param).variable_;
        } else {
            //
            // Emit a formal parameter list, i.e., the types and names.
            //
            o << stuff_decl( (*param).declaration_,
                ( flags & emit_prefix ? param_prefix_ : "" ),
                ( flags & emit_formal ? (*param).variable_ : "" )
            );
        }
    }
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* param_data::stuff_decl( char const *decl, char const *s1,
                                            char const *s2 )
//
// DESCRIPTION
//
//      Take a declaration string like:
//
//          void (*const $)( int )
//
//      and stuff the strings s1 followed by s2 where the $ is, the position of
//      the variable name.
//
// PARAMETERS
//
//      decl    A declaration string with an embedded '$'.
//
//      s1      The first string to be stuffed where the '$'.
//
//      s2      The second string to be stuffed immediately after s1.
//
// RETURN VALUE
//
//      Returns a pointer to the "stuffed" declaration.  Although it is in a
//      static buffer, it is emitted immediately, so there is no danger of
//      overwriting the value before we're finished with it.
//
//*****************************************************************************
{
    static char decl_buf[ 256 ];                // should be big enough
    ::strcpy( decl_buf, decl );
    char *const x = ::strchr( decl_buf, '$' );  // x marks the spot
    ::strcpy( x, s1 );
    ::strcat( x, s2 );
    ::strcpy( x + ::strlen( s1 ) + ::strlen( s2 ), decl + (x - decl_buf + 1) );
    return decl_buf;
}
/* vim:set et ts=4 sw=4: */
