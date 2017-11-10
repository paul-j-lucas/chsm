/*
**	CHSM Language System
**	src/c++/chsm2c++/parent.c
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
#include "chsm.h"
#include "chsm_compiler.h"
#include "manip.h"
#include "parent.h"
#include "util.h"

using namespace PJL;
using namespace std;

char const	parent_info::class_prefix_[]	= "state_";

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

	o << indent << "class " << class_prefix_ << base_name << " : public "
	  << class_name() << " { // " << ::serial( symbol_ ) ENDL
	  << indent << "public:" ENDL;

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

	o ENDL
	  << indent << '\t' << class_prefix_ << base_name
	  << "( CHSM_STATE_ARGS";
	if ( formal_args ) o << ", " << formal_args;
	o << " );" ENDL
	  << indent << "private:" ENDL
	  << indent << "\tstatic int const children_[];" ENDL
	  << indent << "} " << base_name << ';';
	return o;
}

//*****************************************************************************
//
// SYNOPSIS
//
	ostream& parent_info::common_definition(
		ostream &o, char const *formal_args, char const *actual_args
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
//	formal_args	Additional arguments, if any.
//
//	actual_args	Additional arguments, if any.
//
// RETURN VALUE
//
//	Returns the passed-in ostream.
//
//*****************************************************************************
{
	char const *const name = symbol_->name();

	static char conv_buf[ 256 ];
	{ // local scope
		//
		// "Inline function" to convert a state name:
		//
		//	x.y.z -> Px::Py::Pz
		//
		// (where P is a prefix) leaving the result in: conv_buf.
		//
		static int const prefix_length = ::strlen( class_prefix_ );
		::strcpy( conv_buf, class_prefix_ );
		register char *end = ::strchr( conv_buf, '\0' );
		for ( register char const *c = name; *c; ++c )
			if ( *c == '.' ) {
				::strcpy( end, "::" );
				end += 2;
				::strcpy( end, class_prefix_ );
				end += prefix_length;
			} else
				*end++ = *c;
		*end = '\0';
	} // end scope

	//
	// emit child state vector
	//
	o	<< "int const " << g.chsm_->name() << "::" << conv_buf
		<< "::children_[] = {" ENDL
		<< '\t';

	FOR_EACH( child_list, children_, child )
		o << ::serial( *child ) << ", ";

	o	<< -1 ENDL
		<< "};" ENDL;


	//
	// emit state constructor
	//
	o << g.chsm_->name() << "::" << conv_buf << "::" << class_prefix_
	  << state_base( name ) << "( CHSM_STATE_ARGS";
	if ( formal_args ) o << ", " << formal_args;
	o << " ) :" ENDL
	  << '\t' << class_name() << "( CHSM_STATE_INIT, children_";
	if ( actual_args ) o << ", " << actual_args;
	o << " )";

	if ( this != INFO_CONST( parent, ROOT_SYM ) ) {
		//
		// Emit initializers for child states only if this is not the
		// root state.  The root state is "special" in that its
		// children are not lexically enclosed by it.
		//
		FOR_EACH( child_list, children_, child ) {
			o << ',' ENDL
			  << initializer( INFO_CONST( state, *child ) );
		}
	}
	o ENDL
	  << '{' ENDL
	  << '}' ENDL;

	return o;
}
/* vim:set noet ts=8 sw=8: */
