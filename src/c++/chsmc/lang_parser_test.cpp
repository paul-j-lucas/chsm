/*
**      CHSM Language System
**      src/c++/chsmc/lang_parser_test.cpp
**
**      Copyright (C) 2018  Paul J. Lucas
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// local
#include "config.h"                     /* must go first */
#include "cpp_parser.h"
#include "options.h"
#include "util.h"

// standard
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sysexits.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// extern variable definitions
extern char const  *me;                 // executable name

// local variables
static string       declaration;

/**
 * Long options.
 *
 * @hideinitializer
 */
static option const LONG_OPTS[] = {
  { "c++",      no_argument,        nullptr, 'c' },
#ifdef ENABLE_JAVA
  { "java",     no_argument,        nullptr, 'j' },
#endif /* ENABLE_JAVA */
  { "language", required_argument,  nullptr, 'x' },
  { nullptr,    0,                  nullptr, 0   }
};

/**
 * Short options.
 *
 * @hideinitializer
 */
static char const   SHORT_OPTS[] = "cx:"
#ifdef ENABLE_JAVA
  "j"
#endif /* ENABLE_JAVA */
;

////////// local functions ////////////////////////////////////////////////////

static void usage( void ) {
  cerr <<
"usage : " << me << " [options] declaration\n"
"\n"
"options:\n"
"  --c++/-c            Same as -xc++\n"
#ifdef ENABLE_JAVA
"  --java/-j           Same as -xjava.\n"
#endif /* ENABLE_JAVA */
"  --language/-x lang  Set language to generate [default: C++].\n"
  ;
  exit( EX_USAGE );
}

////////// main ///////////////////////////////////////////////////////////////

int main( int argc, char const *argv[] ) {
  me = PJL::base_name( argv[0] );
  ::optind = ::opterr = 1;

	for (;;) {
    char const opt = options_get( &argc, &argv, SHORT_OPTS, LONG_OPTS );
    if ( opt == '\0' )
      break;
    switch ( opt ) {
      case 'c': opt_lang = lang::CPP;            break;
#ifdef ENABLE_JAVA
      case 'j': opt_lang = lang::JAVA;           break;
#endif /* ENABLE_JAVA */
      case 'x': opt_lang = parse_lang( optarg ); break;
      default : usage();
    } // switch
  } // for

  if ( argc < 1 )
    usage();
  for ( int i = 0; i < argc; ++i ) {
    if ( i > 0 )
      declaration += ' ';
    declaration += argv[i];
  } // for

  istringstream iss{ declaration };
  unique_ptr<lang_parser> parser{ lang_parser::create( opt_lang, iss ) };
  try {
    auto const param_list{ parser->parse_param_list() };
    for ( auto const &p : param_list )
      cout << "decl=\"" << p.decl_ << "\" name=\"" << p.name_ << '"' << endl;
  }
  catch ( lang_parser::exception const &e ) {
    cerr << e.what() << endl;
    exit( EX_SOFTWARE );
  }

  exit( EX_OK );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
