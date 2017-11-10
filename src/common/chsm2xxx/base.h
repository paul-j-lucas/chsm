/*
**      CHSM Language System
**      src/common/chsm2xxx/base.h
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

#ifndef CHSM_base_info_H
#define CHSM_base_info_H

// standard
#include <list>

// local
#include "rtti.h"
#include "symbol.h"

//
// Macros used to declare/define run-time type-information for *_info's.
//
#define CHSM_DECLARE_RTTI   \
        DECLARE_RTTI        \
        virtual symbol_type type() const;

#define CHSM_DEFINE_RTTI( DERIVED, BASE, T )    \
        DEFINE_RTTI( DERIVED, BASE )            \
        base_info::symbol_type DERIVED::type() const { return T; }

//*****************************************************************************
//
// SYNOPSIS
//
        struct base_info : PJL::synfo
//
// DESCRIPTION
//
//      A base_info is-a synfo serving as the base class for all info's in the
//      CHSM compiler.
//
//*****************************************************************************
{
    bool        used_;                  // whether actually used
    int const   first_ref_;             // line no. first ref'd on

    enum {
        unknown_t       = 0x0000,
        undeclared_t    = 0x0001,
        global_t        = 0x0002,       // for forward-referenced states
        chsm_t          = 0x0004,
        child_t         = 0x0008,       // for child-list declarations
        state_t         = 0x0010,
        cluster_t       = 0x0020,
        set_t           = 0x0040,
        parent_t        = cluster_t | set_t,
        enex_event_t    = 0x0100,
        user_event_t    = 0x0200,
        event_t         = enex_event_t | user_event_t
    };
    typedef unsigned symbol_type;

    CHSM_DECLARE_RTTI
protected:
    base_info( int scope = PJL::symbol_table::scope() );

    virtual std::ostream&   emit( std::ostream& ) const;
    static char const*      make_class_name( char const* );
};

#endif  /* CHSM_base_info_H */
/* vim:set et ts=4 sw=4: */
