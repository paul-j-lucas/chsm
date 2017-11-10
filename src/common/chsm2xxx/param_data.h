/*
**      CHSM Language System
**      src/common/chsm2xxx/param_data.h
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

#ifndef CHSM_param_data_H
#define CHSM_param_data_H

// standard
#include <iostream>
#include <list>
#include <string>

// local
class list_comma;                               // in list_comma.h
#include "manip.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct param_data
//
// DESCRIPTION
//
//      A param_data is-a base class for holding parameter-list information.
//
//*****************************************************************************
{
    //
    // Used to give parameter formal argument names a different name from the
    // data members.  For example:
    //
    //      param_block( T Pparam ) : param( Pparam ) { }
    //                     ^                 ^
    static char const param_prefix_[];

    struct param {
        int line_no_;
        //
        // Given a declaration such as:
        //
        //      T (*const foo)( U )
        //
        // we store it as:
        //
        //      declaration: T (*const $)( U )
        //      variable   : foo
        //
        // See the comments for the arg_cat and arg_cat_ident productions in
        // grammar.y.
        //
        std::string declaration_;
        std::string variable_;

        param() { }
        param( int i, char const *d, char const *v ) :
            line_no_( i ), declaration_( d ), variable_( v ) { }
    };
    typedef std::list<param> param_list_type;
    param_list_type param_list_;

    //
    // Flags used to control how parameter lists are emitted.
    //
    enum {
        emit_comma  = 0x01,
        emit_prefix = 0x02,
        emit_formal = 0x04,
        emit_actual = 0x08
    };
    static unsigned default_emit_flags_;

    friend info_manip<param_data>
    param_list( param_data const *p, unsigned flags = 0 ) {
        return info_manip<param_data>( &param_data::emit_param_list, p, flags );
    }
protected:
    param_data() { }
    virtual ~param_data() { }

    std::ostream& emit_param_list( std::ostream&, unsigned = 0 ) const;

    virtual std::ostream& emit_param_list_aux( std::ostream&,
                                               list_comma&,
                                               unsigned = 0 ) const;

    static char const* stuff_decl( char const *decl, char const*, char const* );

    static char const* stuff_decl( char const *decl, char const *a,
                                   std::string const &b ) {
        return stuff_decl( decl, a, b.c_str() );
    }

    static char const* stuff_decl( char const *decl, std::string const &a,
                                   char const *b ) {
        return stuff_decl( decl, a.c_str(), b );
    }

    static char const* stuff_decl( char const *decl, std::string const &a,
                                   std::string const &b ) {
        return stuff_decl( decl, a.c_str(), b.c_str() );
    }

    static char const* stuff_decl( std::string const &decl, char const *a,
                                   std::string const &b ) {
        return stuff_decl( decl.c_str(), a, b.c_str() );
    }

    static char const* stuff_decl( std::string const &decl,
                                   std::string const &a, char const *b ) {
        return stuff_decl( decl.c_str(), a.c_str(), b );
    }

    static char const* stuff_decl( std::string const &decl,
                                   std::string const &a,
                                   std::string const &b ) {
        return stuff_decl( decl.c_str(), a.c_str(), b.c_str() );
    }
};

#endif  /* CHSM_param_data_H */
/* vim:set et ts=4 sw=4: */
