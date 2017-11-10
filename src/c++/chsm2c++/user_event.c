/*
**      CHSM Language System
**      src/c++/chsm2c++/user_event.c
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
#include "chsm.h"
#include "chsm_compiler.h"
#include "list_comma.h"
#include "user_event.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const user_event_info::class_suffix_[] = "_event";

CHSM_DEFINE_RTTI( user_event_info, event_info, TYPE(user_event) )

//*****************************************************************************
//
// SYNOPSIS
//
        user_event_info::user_event_info( symbol const *base_event ) :
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
    base_event_( base_event ),
    precondition_( no_precondition )
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
    if ( base_event_ ) {
        o << g.chsm_->name() << "::";
        o << base_event_->name() << class_suffix_;
    } else
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
        << "\tclass " << symbol_->name() << class_suffix_
        << " : public " << base_class( this ) << " {" ENDL
        << "\tpublic:" ENDL;

    //
    // Emit a declaration of a typedef for the base event's param_block for
    // later use.
    //
    o << "\t\ttypedef " << base_class( this )
      << "::param_block base_param_block;" ENDL;

    //
    // emit param_block data member declarations
    //
    o   << "\t\tstruct param_block : " << base_class( this )
        << "::param_block {" ENDL;
    FOR_EACH( param_list_type, param_list_, param ) {
        if ( !g.options_.no_line_ )
            o << "#line " << (*param).line_no_
              << " \"" << g.source_->name_ << '"' ENDL;
        o << "\t\t\t" << stuff_decl(
            (*param).declaration_, "", (*param).variable_
          )
          << ';' ENDL;
    }

    //
    // emit param_block constructor declaration
    //
    // If this event has no parameters, inline the constructor.
    //
    o << "\t\t\tparam_block( "
      << chsm_info::chsm_ns_alias_ << "::event const &";
    if ( has_any_parameters() )
        o << param_list( this, emit_comma ) << ");" ENDL;
    else
        o << "event ) : " ENDL
          << "\t\t\t\t" << base_class( this )
          << "::param_block( event ) { }" ENDL;

    //
    // start of protected section
    //
    o << "\t\tprotected:" ENDL;

    //
    // emit param_block destructor declaration
    //
    o << "\t\t\tvirtual ~param_block();" ENDL;

    //
    // emit precondition declaration
    //
    if ( precondition_ )
        o << "\t\t\tbool precondition() const;" ENDL;

    o << "\t\t};" ENDL;

    if ( has_any_parameters() ) {
        //
        // emit operator-> definition
        //
        o << "\t\tparam_block* operator->() const {" ENDL
          << "\t\t\treturn (param_block*)param_block_;" ENDL
          << "\t\t}" ENDL;
    }

    if ( has_any_parameters() || precondition_ ) {
        //
        // emit event operator() declaration
        //
        o << "\t\tvoid operator()( "
          << param_list( this ) << ");" ENDL;
    }

    //
    // emit event constructor definition
    //
    o <<"\tprotected:" ENDL
      << "\t\t" << symbol_->name() << class_suffix_
      << "( CHSM_EVENT_ARGS ) : " ENDL
      << "\t\t\t" << base_class( this )
      << "( CHSM_EVENT_INIT ) { }" ENDL;

    //
    // emit rest of event declaration
    //
    o << "\t\tfriend class " << g.chsm_->name() << ';' ENDL
      << "\t} " << symbol_->name() << ';' ENDL
      << "\tchar " << symbol_->name()
      << "_param_block[ sizeof( " << symbol_->name() << class_suffix_
      << "::param_block ) ];" ENDL;

    if ( precondition_ == func_precondition )
        o << "\tbool " << symbol_->name()
          << "_precondition( " << param_list( this )
          << ") const;" ENDL;

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
    common_definition( o );
    if ( has_any_parameters() ) {
        //
        // emit param_block constructor definition
        //
        // The parameter formal argument names can not have the same names as
        // the data members, so we prefix each one by a string, i.e.:
        //
        //      param_block( T Pparam ) : param( Pparam ) { }
        //                 ^                 ^
        o << g.chsm_->name() << "::" << symbol_->name()
          << class_suffix_ << "::param_block::param_block( "
          << chsm_info::chsm_ns_alias_ << "::event const &event"
          << param_list( this, emit_comma | emit_prefix | emit_formal )
          << " ) :" ENDL;

        //
        // emit base event's param_block initializer
        //
        o << '\t' << base_class( this ) << "::param_block( event";
        if ( base_has_any_parameters() ) {
            o << param_list(
                INFO_CONST( user_event, base_event_ ),
                emit_comma | emit_prefix | emit_actual
              );
        }
        o << " )";

        FOR_EACH( param_list_type, param_list_, param )
            o << ", " << (*param).variable_ << "( "
              << param_prefix_ << (*param).variable_ << " )";

        o ENDL
          << '{' ENDL
          << '}' ENDL;
    }

    //
    // emit param_block destructor definition
    //
    // Because it's declared as virtual in the run-time library, we define it
    // out-of-line even though it's empty.
    //
    o << g.chsm_->name() << "::" << symbol_->name() << class_suffix_
      << "::param_block::~param_block() { }" ENDL;

    if ( has_any_parameters() || precondition_ ) {
        //
        // emit operator() definition
        //
        o << "void " << g.chsm_->name() << "::" << symbol_->name()
          << class_suffix_ << "::operator()( "
          << param_list( this, emit_formal ) << " )" ENDL
          << '{' ENDL
          << "#ifdef\tCHSM_MULTITHREADED" ENDL
          << "\tmachine_lock const lock( machine_ );" ENDL
          << "#endif" ENDL
          << "\tif ( !in_progress_ )" ENDL
          << "\t\tbroadcast( new( ((" << g.chsm_->name()
          << "&)machine_)." << symbol_->name()
          << "_param_block ) param_block( *this"
          << param_list( this, emit_comma | emit_actual )
          << " ) );" ENDL
          << '}' ENDL;
    }

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
    common_initializer( o );

    if ( base_event_ )
        o << '&' << base_event_->name();
    else
        o << 0;

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
        INFO_CONST( user_event, base_event_ )
            ->emit_param_list_aux( o, comma, flags );

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
