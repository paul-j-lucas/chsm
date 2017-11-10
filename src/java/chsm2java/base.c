/*
**      CHSM Language System
**      src/java/chsm2java/base.c
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
#include "base.h"
#include "chsm.h"
#include "chsm_compiler.h"

using namespace PJL;
using namespace std;

CHSM_DEFINE_RTTI( base_info, synfo, TYPE(unknown) )

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& base_info::emit( ostream &o ) const
//
// DESCRIPTION
//
//      Emit a base_info.
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
    o << type_string( type() ) << ' ';
    return synfo::emit( o );
}

//*****************************************************************************
//
// SYNOPSIS
//
        base_info::base_info( int scope ) :
//
// DESCRIPTION
//
//      Construct a base_info.
//
//*****************************************************************************
    synfo( scope ),
    first_ref_( g.source_ ? g.source_->line_no_ : 0 ),
    used_( false )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* base_info::make_class_name( char const *name )
//
// DESCRIPTION
//
//      Make a string of a class name prepended by a namespace.
//
// PARAMETERS
//
//      name    The name of the class.
//
// RETURN VALUE
//
//      Returns a dynamically allocated string containing the fully qualified
//      class name.
//
//*****************************************************************************
{
    char *const buf = new char[
        ::strlen( chsm_info::chsm_package_name_ ) + ::strlen( name ) + 1 + 1
    ];
    ::strcpy( buf, chsm_info::chsm_package_name_ );
    ::strcat( buf, "." );
    ::strcat( buf, name );
    return buf;
}
/* vim:set et ts=4 sw=4: */
