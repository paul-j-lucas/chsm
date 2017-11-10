/*
**      CHSM Language System
**      src/c++/chsm2c++/util.c
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

// local
#include "chsm_compiler.h"
#include "util.h"

using namespace PJL;
using namespace std;

//*****************************************************************************
//
// SYNOPSYS
//
        bool type_check(
            symbol const *s, int types, type_condition condition,
            char const *more, int in
        )
//
// DESCRIPTION
//
//      Perform type-checking of a symbol.
//
// PARAMETERS
//
//      s           The symbol to check.
//
//      types       The allowable types.
//
//      condition   The condition under which a type must match.
//
//      more        Additional error string.
//
//      in          How far "in" to look for the symbol's info.
//
// RETURN VALUE
//
//      Returns true only if the type is OK.
//
//*****************************************************************************
{
    if ( !s->info( in ) && condition != must_exist ) {
        //
        // If the symbol does not have any info (hence, no type), it's
        // acceptable so long as the condition is not that the symbol must have
        // been previously declared (hence, have a type).
        //
        return true;
    }

    base_info::symbol_type type = type_of( s, in );
    if ( !(type & types) ) {
        //
        // Regardless of the condition, if the symbol has a type and it doesn't
        // match, it's unacceptable.
        //
        g.source_->error() << type_string( type ) << " \"" << s->name()
            << "\": " << type_string( types ) << " expected " << more << '\n';
        return false;
    }

    //
    // The type matches, but return true only if the condition is not that no
    // info must be associated with the symbol, i.e., we've just encountered
    // the symbol for the first time.  This prevents info from being added more
    // than once.
    //
    return condition != no_info;
}

//*****************************************************************************
//
// SYNOPSIS
//
        base_info::symbol_type type_of( symbol const *s, int in )
//
// DESCRIPTION
//
//      Given a symbol, return its type (the type of the first *_info struct
//      hanging off of it).
//
// PARAMETERS
//
//      s   The symbol to use.
//
//      in  How far "in" to go.
//
// RETURN VALUE
//
//      Returns said type.
//
//*****************************************************************************
{
    synfo const *const i = s->info( in );
    return i ? static_cast<base_info const*>( i )->type() : TYPE(unknown);
}

//*****************************************************************************
//
// SYNOPSYS
//
        char const* type_string( base_info::symbol_type types )
//
// DESCRIPTION
//
//      Turn the encoded types into a string of the type-names; note: there may
//      be more than one.
//
// PARAMETERS
//
//      types   The types to convert.
//
// RETURN VALUE
//
//      Returns a string containing or "or"-separated list of type names.
//
//*****************************************************************************
{
    static char type_buf[ 256 ];
    *type_buf = '\0';
    if ( !types ) ++types;              // turn unknown_t into undeclared_t
    bool append_or = false;
    for ( register unsigned bit = 1; bit; bit <<= 1 )
        if ( types & bit ) {
            if ( append_or )
                ::strcat( type_buf, " or " );
            else
                append_or = true;
            switch ( bit ) {
                case TYPE(child):
                    ::strcat( type_buf, "child" );
                    break;
                case TYPE(chsm):
                    ::strcat( type_buf, "chsm" );
                    break;
                case TYPE(state):
                case TYPE(global):
                    ::strcat( type_buf, "state" );
                    break;
                case TYPE(cluster):
                    ::strcat( type_buf, "cluster" );
                    break;
                case TYPE(set):
                    ::strcat( type_buf, "set" );
                    break;
                case TYPE(enex_event):
                    ::strcat( type_buf, "enter/exit-event");
                    break;
                case TYPE(user_event):
                    ::strcat( type_buf, "event" );
                    break;
                default:
                    ::strcat( type_buf, "undeclared" );
                    break;
            }
        }
    return type_buf;
}
/* vim:set et sw=4 ts=4: */
