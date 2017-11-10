/*
**      CHSM Language System
**      src/java/chsm2java/user_event.c
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

// local
#include "arg_macros.h"
#include "chsm.h"
#include "chsm_compiler.h"
#include "list_comma.h"
#include "manip.h"
#include "user_event.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const user_event_info::class_suffix_[] = "Event";

CHSM_DEFINE_RTTI( user_event_info, event_info, TYPE(user_event) )

//*****************************************************************************
//
// SYNOPSIS
//
        user_event_info::user_event_info( symbol const *base_event )
//
// DESCRIPTION
//
//      Construct a user_event_info.
//
// PARAMETERS
//
//      base_event  The symbol of the event this event is derived from.
//
//*****************************************************************************
    : base_event_( base_event ), precondition_( no_precondition )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& user_event_info::emit_base_class( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      If a user_event has a base class, emit the base class name; otherwise
//      emit the user_event's class name.
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
    if ( base_event_ )
        o << base_event_->name() << class_suffix_;
    else
        o << class_name();
    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& user_event_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this event's declaration.
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
    common_declaration( o )
        << "\tprotected static class "
        << symbol_->name() << class_suffix_
        << " extends " << base_class( this ) << " {" ENDL;

    //
    // emit ParamBlock data member declarations
    //
    o << "\t\tprotected static class ParamBlock extends "
      << base_class( this ) << ".ParamBlock {" ENDL;
    FOR_EACH( param_list_type, param_list_, param ) {
        o << "\t\t\t"
          << stuff_decl( (*param).declaration_, "", (*param).variable_ )
          << ';' ENDL;
    }

    //
    // emit ParamBlock constructor declaration
    //
    o << "\t\t\tprotected ParamBlock( "
      << chsm_info::chsm_package_name_ << ".Event event"
      << param_list( this, emit_comma | emit_prefix | emit_formal )
      << " ) {" ENDL;
    emit_definition( o );
    o << "\t\t\t}" ENDL;

    //
    // emit precondition declaration
    //
    if ( precondition_ )
        o << "\t\t\tprotected boolean precondition() {" ENDL
          << "\t\t\t\treturn super.precondition() &&\n"
          << "\t\t\t\t\t((" << g.chsm_->name() << ")event_.machine())."
          << symbol_->name() << "_precondition( "
          << param_list( this, emit_actual )
          << " );" ENDL
          << "\t\t\t}" ENDL;

    o << "\t\t}" ENDL;

    //
    // emit event constructor definition
    //
    o << "\t\t" << symbol_->name() << class_suffix_
      << "( " CHSM_EVENT_ARGS " ) {" ENDL
      << "\t\t\tsuper( " CHSM_EVENT_INIT " );" ENDL
      << "\t\t}" ENDL;

    if ( has_any_parameters() || precondition_ )
        o << "\t\tpublic void broadcast( " 
          << param_list( this, emit_formal ) << " ) {" ENDL
          << "\t\t\tbroadcast( new ParamBlock( this"
          << param_list( this, emit_comma | emit_actual )
          << " ) );" ENDL
          << "\t\t}" ENDL
          << "\t\tpublic final ParamBlock " << symbol_->name()
          << "Param() {" ENDL
          << "\t\t\treturn (ParamBlock)paramBlock_;" ENDL
          << "\t\t}" ENDL;

    o << "\t}" ENDL;

    //
    // emit rest of event declaration
    //
    o << "\tprotected final " << symbol_->name() << class_suffix_ << ' '
      << symbol_->name() << ';' ENDL;

    //
    // emit short-hand method for broadcasting an event
    //
    o << "\tpublic final void " << symbol_->name() << "( "
      << param_list( this , emit_formal ) << " ) {" ENDL
      << "\t\t" << symbol_->name() << ".broadcast( ";
    if ( has_any_parameters() )
        o << param_list( this, emit_actual );
    else
        o << "null";
    o << " );" ENDL
      << "\t}" ENDL;

    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& user_event_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this event's definition.
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
    o << "\t\t\t\tsuper( event";
    if ( has_any_parameters() ) {
        //
        // emit ParamBlock constructor definition
        //
        // The parameter formal argument names can not have the same names as
        // the data members, so we prefix each one by a string, i.e.:
        //
        //      ParamBlock( T Pparam ) : param( Pparam ) { }
        //                    ^                 ^

        //
        // emit base event's ParamBlock initializer
        //
        if ( base_has_any_parameters() ) {
            o << param_list(
                    INFO_CONST( user_event, base_event_ ),
                    emit_comma | emit_prefix | emit_actual
                 );
        }
        o << " );" ENDL;

        FOR_EACH( param_list_type, param_list_, param )
            o << "\t\t\t\t" << (*param).variable_ 
              << " = " << param_prefix_ << (*param).variable_ 
              << ';' ENDL;
    } else
        o << " );" ENDL;

    return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& user_event_info::emit_initializer( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//      Emit this event's initializer.
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
    o << symbol_->name() << " = new " << symbol_->name() << class_suffix_;
    common_initializer( o );

    if ( base_event_ )
        o << base_event_->name();
    else
        o << "null";

    return o << " )";
}

//*****************************************************************************
//
// SYNOPSIS
//
    ostream& user_event_info::emit_param_list_aux( ostream &o,
                                                   list_comma &comma,
                                                   unsigned flags ) const
//
// DESCRIPTION
//
//      Recursively print the parameter list starting from the most base class
//      and working back to the most derived.
//
// PARAMETERS
//
//      o       The ostream to emit to.
//
//      comma   The list_comma to use.
//
// RETURN VALUE
//
//      Returns the passed-in ostream.
//
//*****************************************************************************
{
    if ( base_event_ )
        INFO_CONST( user_event, base_event_ )->
            emit_param_list_aux( o, comma, flags );

    return param_data::emit_param_list_aux( o, comma, flags );
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool user_event_info::has_any_parameters() const
//
// DESCRIPTION
//
//      Returns true only if this event has any parameters including those
//      inherited from base event(s).
//
// RETURN VALUE
//
//      Returns said value.
//
//*****************************************************************************
{
    return !param_list_.empty() || base_has_any_parameters();
}
/* vim:set et ts=4 sw=4: */
