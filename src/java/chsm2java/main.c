/*
**	CHSM Language System
**	src/java/chsm2java/main.c
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
#include <iostream>
#include <unistd.h>

// local
#include "chsm.h"
#include "chsm_compiler.h"

using namespace PJL;
using namespace std;

//
// Define inherited static data members.
//
char const	compiler::ident_[] = "CHSM-to-Java Compiler";
char const	compiler::usage_[] = "[-ESvY] file.chsmj file.java";

//
// globals
//
extern "C" {
	extern char*	optarg;
	extern int	optind, opterr;
}

//*****************************************************************************
//
// SYNOPSYS
//
	int main( int argc, char *argv[] )
//
// DESCRIPTION
//
//	Initialize and call the parser.
//
// PARAMETERS
//
//	argc	The number of arguments.
//
//	argv	A vector of the arguments; argv[argc] is null.  Aside from the
//		options below, the arguments are the names of the files and
//		directories to be indexed.
//
// SEE ALSO
//
//	Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
//	Addison-Wesley, Reading, MA, 1997.  pp. 116-118.
//
//*****************************************************************************
{
	compiler::initialize( argc, (char const**)argv );

	::opterr = 1;
	//
	// parse command-line options
	//
	for ( int opt; (opt = ::getopt( argc, (char**)argv, "ESvY" )) != EOF; )
		switch ( opt ) {

			case 'E': // Write to standard output.
				g.options_.to_stdout_ = true;
				break;

			case 'S':
#ifdef PJL_STACK_DEBUG
				g.base_options_.stack_debug_ = true;
#else
				g.warning() << "sorry: not compiled with "
					"PJL_STACK_DEBUG defined; "
					"option ignored\n";
#endif
				break;

			case 'v': // Print version and exit.
				compiler::version();

			case 'Y': // Emit yacc debugging output.
#ifdef YYDEBUG
				::yydebug = true;
#else
				g.warning() << "sorry: not compiled with "
					"YYDEBUG defined; option ignored\n";
#endif
				break;

			case '?': // Print help.
				compiler::usage();
		}

	//
	// check argument count
	//
	// There should be 2 arguments unless -E is specified in which case
	// there should be 1.
	//
	argc -= ::optind, argv += ::optind;
	if ( argc != 1 + !g.options_.to_stdout_ )
		compiler::usage();

	//
	// set up files
	//
	g.source_ = new compiler::source( argv[0] );
	if ( !g.options_.to_stdout_ ) {
		g.target_ = new compiler::target( argv[1] );
		g.definition_file_name_ = argv[2];
	} else
		g.target_ = new compiler::target( cout );
	g.user_code_ = new chsm_compiler::user_code;

	//
	// Start compiling and generating code.
	//
	EMIT	<< compiler::ident_stamp ENDL
		ENDL
		<< "/***** user-declarations *****/" ENDL
		ENDL;

	return ::yyparse() || g.source_->errors_ ?
		compiler::Source_Error : compiler::Success;
}
/* vim:set noet ts=8 sw=8: */
