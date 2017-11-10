/*
**      CHSM Language System
**      src/common/chsm2xxx/transition.h
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

#ifndef CHSM_transition_H
#define CHSM_transition_H

// standard
#include <iostream>

// local
#include "global.h"
#include "symbol.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct transition_info : global_info
//
// DESCRIPTION
//
//      A transition_info is-a global_info for holding information about
//      transitions.  Transitions are different in that there's no need for
//      them to have a name; hence there's no need to put them into the symbol-
//      table.  It's stored there just for convenience so we don't have to
//      create yet another class.
//
//*****************************************************************************
{
    PJL::symbol const   *const from_, *const to_;
    int const           condition_id_, target_id_, action_id_;

    //
    // Suffix used in compiler-generated names.
    //
    static char const vector_suffix_[];

    transition_info( int condition_id, PJL::symbol const *from,
                     PJL::symbol const *to, int target_id, int action_id );

    virtual std::ostream& emit_definition( std::ostream&, unsigned = 0 ) const;
};

#endif  /* CHSM_transition_H */
/* vim:set et ts=4 sw=4: */
