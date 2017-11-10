/*
**	CHSM Language System
**	src/java/chsm2java/parent.c
**
**	Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
** 
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
** 
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// standard
#include <cstring>
#include <iostream>

// local
#include "arg_macros.h"
#include "chsm_compiler.h"
#include "chsm.h"
#include "list_comma.h"
#include "manip.h"
#include "parent.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const	parent_info::class_prefix_[]	= "State_";

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& parent_info::common_declaration(
		ostream &o, char const *formal_args
	) const
//
// DESCRIPTION
//
//	Emit the common portion of a cluster or set's declaration.
//
// PARAMETERS
//
//	o		The ostream to emit to.
//
//	formal_args	Additional arguments, if any.
//
// RETURN VALUE
//
//	Returns the passed-in ostream.
//
//*****************************************************************************
{
	char const *const base_name = state_base( symbol_->name() );

	o << indent << "final static class " << class_prefix_ << base_name
	  << " extends " << class_name() << " { // "
	  << ::serial( symbol_ ) ENDL
	  << indent << "\tprivate static final int children_[] = new int[] { ";

	list_comma comma;
	FOR_EACH( child_list, children_, child )
	        o << comma << ::serial( *child );

	o << " };" ENDL;

	//
	// Recursively emit declarations for child states only if this is not
	// the root state.  The root state is "special" in that its children
	// are not lexically enclosed by it.
	//
	if ( this != INFO_CONST( parent, ROOT_SYM ) ) {
		++nesting_depth_;
		FOR_EACH( child_list, children_, child )
			o << declaration( INFO_CONST( state, *child ) ) ENDL;
		--nesting_depth_;
	}

	//
	// emit constructor declaration
	//
	o << indent << '\t' << class_prefix_ << base_name
	  << "( " CHSM_STATE_ARGS;
	if ( formal_args ) o << ", " << formal_args;
	o << " ) ";

	emit_definition( o );

	o << indent << '}' ENDL
	  << indent << "public final " << class_prefix_ << base_name << ' '
	  << base_name << ';' ENDL;

	return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& parent_info::common_definition(
		ostream &o, char const*, char const *actual_args
	) const
//
// DESCRIPTION
//
//	Emit the common portion of a cluster or set's definition.
//
// PARAMETERS
//
//	o		The ostream to emit to.
//
//	actual_args	Additional arguments, if any.
//
// RETURN VALUE
//
//	Returns the passed-in ostream.
//
//*****************************************************************************
{
	//
	// emit state constructor
	//
	o << '{' ENDL
	  << indent << "\t\tsuper( " CHSM_STATE_INIT ", children_";
        if ( actual_args ) o << ", " << actual_args;
       	o << " );" ENDL;

	if ( this != INFO_CONST( parent, ROOT_SYM ) ) {
		//
		// Emit initializers for child states only if this is not the
		// root state.  The root state is "special" in that its
		// children are not lexically enclosed by it.
		//
		FOR_EACH( child_list, children_, child )
			o << indent << "\t\t"
			  << initializer( INFO_CONST( state, *child ) )
			  << ';' ENDL;
	}

	return o << indent << "\t}" ENDL;
}
/* vim:set noet ts=8 sw=8: */
