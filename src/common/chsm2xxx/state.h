/*
**      CHSM Language System
**      src/common/chsm2xxx/state.h
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

#ifndef CHSM_state_info_H
#define CHSM_state_info_H

// standard
#include <iostream>
#include <string>

// local
#include "global.h"
#include "serial_number.h"
#include "symbol.h"
#include "util.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct state_info : global_info, PJL::serial_number<state_info>
//
// DESCRIPTION
//
//      A state_info is-a global_info and-a serial_number<state_info> for
//      information on states (and also clusters and sets that are derived from
//      it).  States have a serial number that is used in child and transition
//      tables.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    PJL::symbol const *const parent_;           // our parent, if any

    struct events {                             // whether we have these
        bool enter_, exit_;
        events() { enter_ = exit_ = false; }
    } event_;

    struct actions {                            // whether we have these
        bool enter_, exit_;
        actions() { enter_ = exit_ = false; }
    } action_;

    static int nesting_depth_;                  // tabs in declarations

    state_info( PJL::symbol const *parent = 0, char const *derived = 0 );

    //
    // Our class name in the emitted C++ code:
    //
    //      state   -> CHSM::state
    //      cluster -> CHSM::cluster
    //      set     -> CHSM::set
    //
    // or a derived type, if specified.
    //
    char const* class_name() const;

    virtual std::ostream& emit_declaration( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_initializer( std::ostream&, unsigned = 0 ) const;
protected:
    state_info( char const *class_name, PJL::symbol const *parent,
                char const *derived );

    std::ostream& common_declaration( std::ostream& ) const;
    std::ostream& common_initializer( std::ostream& ) const;
private:
    char const *const class_name_;
    std::string const derived_;                 // our derived type, if any
};

////////// Inlines ////////////////////////////////////////////////////////////

inline char const* state_base( char const *s ) {
    //
    // Return the "base name" of a state name:
    //
    //      root.trunk.branch.leaf -> leaf
    //
    char const *const p = ::strrchr( s, '.' );
    return p ? p + 1 : s;
}

std::ostream& indent( std::ostream& );
    //
    // Emit the right number of tab characters for the current child nesting
    // depth.
    //

inline PJL::serial_number_base::value_type serial( PJL::symbol const *sym ) {
    //
    // Shorthand for accessing the serial number of a state.  Used in emitting
    // indices in child and transition tables.
    //
    return sym ? INFO_CONST( state, sym )->serial()
        -1  // Due to a dummy state defined in grammar.y for error recovery.
        -1  // Due to the root_ state inside chsm_info.
    :
        -1  // null pointer: return -1 for index
    ;
}

#endif  /* CHSM_state_info_H */
/* vim:set et ts=4 sw=4: */
