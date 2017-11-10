/*
**      CHSM Language System
**      src/c++/chsm2c++/main.c
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
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>                     /* for unlink(2) */

// local
#include "chsm.h"
#include "chsm_compiler.h"
#include "child.h"
#include "event.h"
#include "itoa.h"
#include "state.h"
#include "transition.h"
#include "util.h"

using namespace PJL;
using namespace std;

//
// Define inherited static data members.
//
char const  compiler::version_[]        = "4.4.1";

symbol*         chsm_compiler::chsm_;
symbol          chsm_compiler::dummy_event( "<event>", new event_info );
symbol          chsm_compiler::dummy_state( "<state>", new state_info );
bool            chsm_compiler::options::no_line_    = false;
bool            chsm_compiler::options::to_stdout_  = false;
symbol*         chsm_compiler::outer_deep_;
symbol*         chsm_compiler::parent_;
symbol_table    chsm_compiler::sym_table_;

chsm_compiler   g;

//*****************************************************************************
//
// SYNOPSYS
//
        chsm_compiler::~chsm_compiler()
//
// DESCRIPTION
//
//      Destroy a chsm_compiler.
//
//*****************************************************************************
{
    delete user_code_;
    delete target_;
    delete source_;
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::backpatch_enter_exit_events()
//
// DESCRIPTION
//
//      For plain-events, "back-patch" whether states have an enter or exit
//      event that needs to be broadcast.
//
//*****************************************************************************
{
    FOR_EACH( chsm_info::event_queue, CHSM->event_q_, event ) {
        event_info const &info = *INFO_CONST( event, *event );
        symbol const *const state = info.state_;
        if ( !state )
            continue;
        if ( info.kind_ == event_info::user_event ) {
            g.fatal() << "data error in backpatch_enter_exit_events()" << endl;
            ::exit( compiler::Internal_Error );
        }
        state_info::events &e = INFO( state, state )->event_;
        ( info.kind_ == event_info::enter_state ? e.enter_ : e.exit_ ) = true;
    }
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::check_child_states_defined()
//
// DESCRIPTION
//
//      Check that all child states declared have actually been defined.
//
//*****************************************************************************
{
    FOR_EACH( symbol_table, sym_table_, state )
        if ( type_of( state ) & TYPE(child)
             && state->in_current_scope()
             && !INFO_CONST( child, &*state )->defined_
        ) {
            source_->warning( INFO_CONST( base, &*state )->first_ref_ )
            << type_string( *state ) << " \"" << state->name()
            << "\" not defined\n";
        }
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::check_events_used()
//
// DESCRIPTION
//
//      Check that all declared events have been used.
//
//*****************************************************************************
{
    if ( !CHSM )
        return;
    FOR_EACH( chsm_info::event_queue, CHSM->event_q_, event )
        if ( type_of( event ) & TYPE(user_event)
             && !INFO_CONST( base, *event )->used_ )
        {
            source_->warning( INFO_CONST( base, *event )->first_ref_ )
            << type_string( *event ) << " \"" << (*event)->name()
            << "\" not used\n";
        }
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::check_states_defined()
//
// DESCRIPTION
//
//      Check that all states referenced have actually been defined.
//
//*****************************************************************************
{
    FOR_EACH( symbol_table, sym_table_, state )
        if ( type_of( state ) & TYPE(global) ) {
            source_->error( INFO_CONST( base, &*state )->first_ref_ )
                << type_string( *state ) << " \"" << state->name()
                << "\" not defined\n";
        }
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::check_transitions()
//
// DESCRIPTION
//
//      Check that:
//      1. The target states are defined.
//      2. No transition goes between two children of a set.
//
//*****************************************************************************
{
    if ( !CHSM )
        return;
    FOR_EACH( chsm_info::transition_queue, CHSM->transition_q_, t ) {
        transition_info const &info = *INFO_CONST( transition, *t );
        if ( !info.to_ )                // internal or a computed target
            continue;

        register char const *from = info.from_->name(), *to = info.to_->name();

        //
        // First check whether the "to" state was defined.
        //
        symbol const &to_sym = sym_table_[ to ];
        if ( !to_sym.info() ) {
            source_->error( info.first_ref_ )
                << '"' << to << "\" not defined\n";
            continue;
        }

        //
        // Find where the names differ.
        //
        char const *const from0 = from;
        while ( *from && *to && *from == *to )
            ++from, ++to;

        if ( *from && *to && from > from0 && from[-1] == '.' ) {
            //
            // The names differ and neither is a subset of the other.  Take the
            // common part of both names -- this is the name of the least-
            // common-ancestor state -- and see if it's a set.
            //
            char name[ 256 ];
            ::strcpy( name, from0 );
            name[ from - from0 - 1 ] = '\0';
            symbol const &ancestor = sym_table_[ name ];
            if ( type_of( ancestor ) & TYPE(set) )
                source_->error( info.first_ref_ ) << "intra-set transition\n";
        }
    }
}

//*****************************************************************************
//
// SYNOPSYS
//
        bool chsm_compiler::not_exists( symbol const *s, unsigned types )
//
// DESCRIPTION
//
//      If the symbol has a type and either 'types' is unknown or the type is
//      one of the specified 'types', then the symbol already exists.
//
//*****************************************************************************
{
    base_info::symbol_type type = type_of( s );
    if ( type && ( types == TYPE(unknown) || (type & types) ) ) {
        if ( *s->name() != '<' ) {
            //
            // Print the error message only if the symbol is not one of our
            // dummy symbols that start with '<'.  (It's impossible for such a
            // symbol to be user-declared since '<' is not in the regular
            // expression for an identifier as specified in lex.l.)
            //
            source_->error() << '"' << s->name()
            << "\" previously declared/defined as " << type_string( s ) << '\n';
        }
        return false;
    }
    return true;
}

//*****************************************************************************
//
// SYNOPSYS
//
        chsm_compiler::user_code::user_code()
//
// DESCRIPTION
//
//      Construct a user_code object.
//
//*****************************************************************************
    : compiler::file_base( make_name(), ios::out )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSYS
//
        void chsm_compiler::user_code::emit( ostream &o )
//
// DESCRIPTION
//
//      Copy the user's code in conditions and actions from the temporary file
//      into the code definition file.
//
//*****************************************************************************
{
    EMIT    ENDL
            << "/***** user-code *****/" ENDL
            ENDL;
    file().close();
    file().open( name_.c_str(), ios::in );
    if ( !file() ) {
        fatal() << "cannot re-open temporary file \""
                << name_ << '"' << endl;
        ::exit( compiler::File_Open_Error );
    }
    char buf[ 4096 ];
    int count;
    do {
        if ( file().read( buf, sizeof buf ).bad() ) {
            fatal() << "problems reading temporary file \"" << name_ << '"'
                    << endl;
            ::exit( compiler::File_Read_Error );
        }
        file().clear();
        count = file().gcount();
        o.write( buf, count );
    } while ( count == sizeof buf );
    o ENDL;
}

//*****************************************************************************
//
// SYNOPSYS
//
        char const* chsm_compiler::user_code::make_name()
//
// DESCRIPTION
//
//      Form a unique name for the temporary user-code file by using our
//      process ID.  This function is needed so the name can be passed as an a
//      mem-initializer in the user_code constructor above.
//
//*****************************************************************************
{
    static char name[15];
    if ( !*name ) {
        ::strcpy( name, "chsm_" );
        ::strcat( name, ::ltoa( ::getpid() ) );
    }
    return name;
}

//*****************************************************************************
//
// SYNOPSYS
//
        chsm_compiler::user_code::~user_code()
//
// DESCRIPTION
//
//      Destroy a user_code object: delete the user code file.
//
//*****************************************************************************
{
    ::unlink( name_.c_str() );
}
/* vim:set et ts=4 sw=4: */
