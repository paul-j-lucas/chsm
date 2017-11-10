/*
**      CHSM Language System
**      src/common/chsm2xxx/util.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas
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

#ifndef CHSM_util_H
#define CHSM_util_H

// local
#include "base.h"
#include "symbol.h"

enum                    type_condition { may_exist, no_info, must_exist };
bool                    type_check(
                            PJL::symbol const*, int types, type_condition,
                            char const *more = "", int in = 0
                        );

base_info::symbol_type  type_of( PJL::symbol const *s, int in = 0 );

char const*             type_string( base_info::symbol_type );


#define ABORT \
        cerr << "sorry: can not recover from previous error(s)\n"; YYABORT

//
// Shorthand for accessing the symbol info for the CHSM itself.
//
#define CHSM            INFO( chsm, g.chsm_ )

//
// Special macro for gluing two identifiers together.
//
#ifdef name2
#   undef name2
#endif
#define name2(a,b) a##b

//
// Shorthand macros for accessing the *_info substructure hanging off a symbol
// cast to the right derived type.
//
inline PJL::synfo const* info_helper( PJL::symbol const *sym ) {
    //
    // This helper inline function exists so that 'sym' is evaluated only once.
    //
    return sym ? sym->info() : 0;
}
#define INFO(T,sym)     (const_cast<name2(T,_info)*>( INFO_CONST(T,sym) ))
#define INFO_CONST(T,sym) \
        (dynamic_cast<name2(T,_info) const*>( info_helper( sym ) ))

//
// Shorthand for accessing the symbol for the root state of the CHSM.
//
#define ROOT_SYM        ( CHSM->root_ )

//
// Shorthand for specifying an info type (see base_info struct above).  There
// must be no spaces around the T.
//
#define TYPE(T)         base_info::name2(T,_t)

//
// Shorthand for iterating over a const STL container.
//
#define FOR_EACH(T,C,I) \
    for (   T::const_iterator I = static_cast<T const&>(C).begin(); \
        I != static_cast<T const&>(C).end(); ++I )

//
// Shorthand for iterating over a non-const STL container.
//
#define TRANSFORM_EACH(T,C,I) \
    for ( T::iterator I = (C).begin(); I != (C).end(); ++I )

////////// inlines ////////////////////////////////////////////////////////////

inline base_info::symbol_type
type_of( PJL::symbol const &s, int in = 0 ) {
    return type_of( &s, in );
}

inline base_info::symbol_type
type_of( PJL::symbol_table::iterator const &i, int in = 0 ) {
    return type_of( &*i, in );
}

inline base_info::symbol_type
type_of( PJL::symbol_table::const_iterator const &i, int in = 0 ) {
    return type_of( &*i, in );
}

inline base_info::symbol_type
type_of( std::list<PJL::symbol*>::iterator const &i, int in = 0 ) {
    return type_of( *i, in );
}

inline base_info::symbol_type
type_of( std::list<PJL::symbol*>::const_iterator const &i, int in = 0 ) {
    return type_of( *i, in );
}

inline char const* type_string( PJL::symbol const *p ) {
    return type_string( type_of( p ) );
}

inline char const* type_string( PJL::symbol const &s ) {
    return type_string( type_of( s ) );
}

#endif  /* CHSM_util_H */
/* vim:set et sw=4 ts=4: */
