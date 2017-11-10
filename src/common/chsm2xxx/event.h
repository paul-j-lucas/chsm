/*
**      CHSM Language System
**      src/common/chsm2xxx/event.h
**
**      Copyright (C) 1996  Paul J. Lucas & Fabio Riccardi
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

#ifndef CHSM_event_info_H
#define CHSM_event_info_H

// standard
#include <iostream>
#include <list>

// local
#include "global.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct event_info : global_info
//
// DESCRIPTION
//
//      An event_info is-a global_info for holding the list of transitions
//      potentially taken when this event is broadcast.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    //
    // The id's of the transitions taken by this event.
    //
    typedef std::list<int> transition_id_list;
    transition_id_list transition_id_list_;

    //
    // The kind of event we really are.
    //
    enum kind { user_event, enter_state, exit_state };
    kind const kind_;

    //
    // The state symbol that we are either an enter or exit event for.
    // Used only if kind is not user_event.
    //
    PJL::symbol const *const state_;

    event_info( kind = user_event, PJL::symbol const *state = 0 );

    //
    // Our class name in the emitted C++ code:
    //      event       ->  CHSM::event
    //      user_event  ->  CHSM::event
    //
    // Originally, in the run-time library, there were two distinct event
    // classes; however, with the extension of the language, it became simpler
    // to have only one event class.  In the compiler, however, it's still
    // cleaner to keep the two separate.
    //
    virtual char const* class_name() const;

    virtual std::ostream& emit_declaration( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_definition ( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_initializer( std::ostream&, unsigned = 0 ) const;
protected:
    std::ostream& common_declaration( std::ostream& ) const;
    std::ostream& common_definition ( std::ostream& ) const;
    std::ostream& common_initializer( std::ostream& ) const;
};

#endif  /* CHSM_event_info_H */
/* vim:set et ts=4 sw=4: */
