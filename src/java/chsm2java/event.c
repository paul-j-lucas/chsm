/*
**	    CHSM Language System
**	    src/java/chsm2java/event.c
**
**	    Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
**
**	    This program is free software; you can redistribute it and/or modify
**	    it under the terms of the GNU General Public License as published by
**	    the Free Software Foundation; either version 2 of the License, or
**	    (at your option) any later version.
** 
**	    This program is distributed in the hope that it will be useful,
**	    but WITHOUT ANY WARRANTY; without even the implied warranty of
**	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	    GNU General Public License for more details.
** 
**	    You should have received a copy of the GNU General Public License
**	    along with this program; if not, write to the Free Software
**	    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// standard
#include <iostream>

// local
#include "chsm_compiler.h"
#include "event.h"
#include "manip.h"
#include "list_comma.h"
#include "mangle.h"
#include "transition.h"
#include "util.h"

using namespace PJL;
using namespace std;

CHSM_DEFINE_RTTI( event_info, global_info, TYPE(enex_event) )

//*****************************************************************************
//
// SYNOPSIS
//
        event_info::event_info( kind k, symbol const *state ) :
//
// DESCRIPTION
//
//	    Construct an event_info.
//
// PARAMETERS
//
//	    k	The kind of event.
//
//	    state	The state symbol that this evene is an enter or exit event for,
//		        but only if this event is not a user_event.
//
//*****************************************************************************
	kind_( k ), state_( state )
{
	// do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* event_info::class_name() const
//
// DESCRIPTION
//
//	    Returns this class's name.
//
// RETURN VALUE
//
//	    Returns said name.
//
//*****************************************************************************
{
	static char const *const name = make_class_name( "Event" );
	return name;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::common_declaration( ostream &o ) const
//
// DESCRIPTION
//
//	    Emit the common portion of this event's declaration, to wit: the
//	    declaration of the vector of id's of transitions taken by this event.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	common_definition( o );
	return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::common_definition( ostream &o ) const
//
// DESCRIPTION
//
//	    Emit the common portion of this event's definition, to wit: the vector
//	    of id's of transitions taken by this event.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	o << "\tprivate final static int "
	  << symbol_->name() << transition_info::vector_suffix_
	  << "[] = { ";

	list_comma comma;
	FOR_EACH( transition_id_list, transition_id_list_, id )
		o << comma << *id;

	return o << " };" ENDL;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::common_initializer( ostream &o ) const
//
// DESCRIPTION
//
//	    Emit the common portion of this evenet's initializer.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	o << "( this, " << symbol_->name()
	  << transition_info::vector_suffix_ << ", \"";

	if ( kind_ == user_event )
		o << symbol_->name();
	else
		o << ( kind_ == enter_state ? "enter" : "exit" )
		  << '(' << demangle( symbol_->name() + 1 ) << ')';

	o << "\", ";

	return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::emit_declaration( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//	    Emit this event's declaration.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	common_declaration( o )
		<< "\tprotected final " << class_name() << ' '
		<< symbol_->name() << ';';
	return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::emit_definition( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//	    Emit this evene't definition.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	return common_definition( o );
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream& event_info::emit_initializer( ostream &o, unsigned ) const
//
// DESCRIPTION
//
//	    Emit this event's initializer.
//
// PARAMETERS
//
//	    o	The ostream to emit to.
//
// RETURN VALUE
//
//	    Returns the passed-in ostream.
//
//*****************************************************************************
{
	o << symbol_->name() << " = new " << class_name();
	return common_initializer( o ) << "null )";
}
/* vim:set et ts=4 sw=4: */
