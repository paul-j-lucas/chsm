/*
**      CHSM Language System
**      src/common/chsm2xxx/user_event.h
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

#ifndef CHSM_user_event_info_H
#define CHSM_user_event_info_H

// standard
#include <iostream>
#include <list>

// local
#include "event.h"
class list_comma;                               // in list_comma.h
#include "manip.h"
#include "param_data.h"
#include "symbol.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct user_event_info : event_info, param_data
//
// DESCRIPTION
//
//      A user_event_info is-an event_info that emits itself differently.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    PJL::symbol const *const base_event_;       // if any
    enum {
        no_precondition,
        expr_precondition,                      // event A [ expr ];
        func_precondition                       // event A %{ return expr; %};
    } precondition_;

    //
    // Used to form class name for user-events.  For example, an event "alpha"
    // is of type alpha_event.
    //
    static char const class_suffix_[];

    user_event_info( PJL::symbol const *base_event = 0 );

    std::ostream&         emit_base_class ( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_declaration( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_definition ( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_initializer( std::ostream&, unsigned = 0 ) const;
private:
    virtual std::ostream& emit_param_list_aux( std::ostream&, list_comma&,
                                               unsigned = 0 ) const;

    bool has_any_parameters() const;
    bool base_has_any_parameters() const {
        return  base_event_ && 
                INFO_CONST( user_event, base_event_ )->has_any_parameters();
    }
};

////////// Inlines ////////////////////////////////////////////////////////////

inline info_manip<user_event_info> base_class( user_event_info const *p ) {
    return info_manip<user_event_info>(
        &user_event_info::emit_base_class, p
    );
}

#endif  /* CHSM_user_event_info_H */
/* vim:set et ts=4 sw=4: */
