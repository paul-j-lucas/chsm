/*
**      CHSM Language System
**      src/c++/chsm2c++/chsm.h
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

#ifndef CHSM_chsm_info_H
#define CHSM_chsm_info_H

// standard
#include <iostream>
#include <list>
#include <string>

// local
#include "global.h"
#include "param_data.h"
#include "symbol.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct chsm_info : global_info, param_data
//
// DESCRIPTION
//
//      A chsm_info is-a global_info for the one and only CHSM:
//
//          chsm my_chsm is { // ...
//
//      The class also holds a lot of miscellanous stuff relating to the CHSM
//      as a whole.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    PJL::symbol         *const root_;           // root state of CHSM
    std::string const   derived_;               // our derived type, if any

    //
    // Sequence numbers used for compiler-generated function names for
    // conditions and actions.
    //
    struct id {
        int condition_;
        int target_;
        int action_;

        id() : condition_( 0 ), target_( 0 ), action_( 0 ) { }
    };
    id id_;

    //
    // Strings used in compiler-generated names.
    //
    static char const action_prefix_[];
    static char const chsm_ns_alias_[];
    static char const condition_prefix_[];
    static char const enter_prefix_[];
    static char const exit_prefix_[];
    static char const target_prefix_[];

    //
    // We must remember states in declaration order to make sure that the first
    // state declared is the one first entered.
    //
    typedef std::list<PJL::symbol*> state_queue;
    state_queue state_q_;

    //
    // We must remember events in declaration order to emit code in the right
    // order for derived events, i.e., the base event must be emitted before
    // any events derived from it.
    //
    typedef std::list<PJL::symbol*> event_queue;
    event_queue event_q_;

    //
    // There's no need to put transitions into the symbol table, but we have to
    // remember them nonetheless.  We also want to define them in declaration
    // order so that when a parent and a child both have a transition on a
    // given event, the one in the parent will dominate.
    //
    typedef std::list<PJL::symbol*> transition_queue;
    transition_queue transition_q_;

    //
    // Used when emitting mem-initializers for states to alter how they emit
    // themselves.  Set to true only when emitting the CHSM constructor.
    //
    static bool emitting_constructor_;

    chsm_info( PJL::symbol *root, char const *derived = 0 );

    std::ostream& emit_declaration( std::ostream&, unsigned = 0 ) const;
    std::ostream& emit_definition ( std::ostream&, unsigned = 0 ) const;
private:
    std::ostream& emit_actions( std::ostream&, PJL::symbol const* ) const;
};

#endif  /* CHSM_chsm_info_H */
/* vim:set et ts=4 sw=4: */
