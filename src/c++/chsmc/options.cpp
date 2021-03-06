/*
**      CHSM Language System
**      src/c++/chsmc/options.cpp
**
**      Copyright (C) 1996-2018  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 3 of the License, or
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

/**
 * @file
 * Defines global variables and functions for command-line options.
 */

// local
#include "config.h"                     /* must go first */
#include "options.h"
#include "util.h"

/// @cond DOXYGEN_IGNORE

// standard
#include <cassert>
#include <cstdlib>                      /* for exit(3) */
#include <iostream>
#include <sstream>
#include <string>
#include <sysexits.h>
#include <unordered_map>

/// @endcond

using namespace std;
namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////

// extern option variables
bool        opt_codegen_only;
fs::path    opt_declaration_path;
fs::path    opt_definition_path;
lang        opt_lang;
bool        opt_line_directives = true;
#ifdef ENABLE_STACK_DEBUG
bool        opt_stack_debug;
#endif /* ENABLE_STACK_DEBUG */

// other extern variables
char const *me;

/**
 * Long options.
 *
 * @hideinitializer
 */
static struct option const LONG_OPTS[] = {
  { "declaration",  required_argument,  nullptr, 'd' },
  { "definition",   required_argument,  nullptr, 'D' },
  { "stdout",       no_argument,        nullptr, 'E' },
#ifdef ENABLE_JAVA
  { "java",         required_argument,  nullptr, 'j' },
#endif /* ENABLE_JAVA */
  { "no-line",      no_argument,        nullptr, 'P' },
#ifdef ENABLE_STACK_DEBUG
  { "stack-debug",  no_argument,        nullptr, 'S' },
#endif /* ENABLE_STACK_DEBUG */
  { "version",      no_argument,        nullptr, 'v' },
  { "language",     required_argument,  nullptr, 'x' },
#ifdef YYDEBUG
  { "yydebug",      no_argument,        nullptr, 'y' },
#endif /* YYDEBUG */
  { nullptr,        0,                  nullptr, 0   }
};

/**
 * Short options.
 *
 * @hideinitializer
 */
static char const   SHORT_OPTS[] = "c:d:D:Eh:Pvx:"
#ifdef ENABLE_JAVA
  "j:"
#endif /* ENABLE_JAVA */
#ifdef ENABLE_STACK_DEBUG
  "S"
#endif /* ENABLE_STACK_DEBUG */
#ifdef YYDEBUG
  "y"
#endif /* YYDEBUG */
;

// local variables
static char         opts_given[ 128 ];  ///< Table of options that were given.

// local functions
static string       format_opt( char );
static char const*  get_long_opt( char );
static void         usage();

////////// inline functions ///////////////////////////////////////////////////

/**
 * Gets whether \a opt was given.
 *
 * @param opt The short option character to check for.
 * @return Returns `true` only if \a opt was given.
 */
inline bool options_gave( char opt ) {
  return opts_given[ static_cast<unsigned char>( opt ) ] != '\0';
}

/**
 * Marks an option as having been given.
 *
 * @param opt The short option character that was given.
 */
inline void mark_option( char opt ) {
  opts_given[ static_cast<unsigned char>( opt ) ] = opt;
}

////////// local functions ////////////////////////////////////////////////////

/**
 * Checks that no options were given that are among the two given mutually
 * exclusive sets of short options.
 * Prints an error message and exits if any such options are found.
 *
 * @param opts1 The first set of short options.
 * @param opts2 The second set of short options.
 */
static void check_mutually_exclusive( char const *opts1, char const *opts2 ) {
  assert( opts1 != nullptr );
  assert( opts2 != nullptr );

  unsigned gave_count = 0;
  char const *opt = opts1;
  char gave_opt1 = '\0';

  for ( unsigned i = 0; i < 2; ++i ) {
    for ( ; *opt != '\0'; ++opt ) {
      if ( options_gave( *opt ) ) {
        if ( ++gave_count > 1 ) {
          char const gave_opt2 = *opt;
          PMESSAGE_EXIT( EX_USAGE,
            format_opt( gave_opt1 ) << " and " << format_opt( gave_opt2 )
            << " are mutually exclusive\n"
          );
        }
        gave_opt1 = *opt;
        break;
      }
    } // for
    if ( gave_count == 0 )
      break;
    opt = opts2;
  } // for
}

/**
 * For each option in \a opts that was given, checks that at least one of
 * \a req_opts was also given.
 * If not, prints an error message and exits.
 *
 * @param opts The set of short options.
 * @param req_opts The set of required options for \a opts.
 */
static void check_required( char const *opts, char const *req_opts ) {
  assert( opts != nullptr );
  assert( req_opts != nullptr );

  for ( char const *opt = opts; *opt; ++opt ) {
    if ( options_gave( *opt ) ) {
      for ( char const *req_opt = req_opts; *req_opt; ++req_opt )
        if ( options_gave( *req_opt ) )
          return;
      bool const reqs_multiple = strlen( req_opts ) > 1;
      PMESSAGE_EXIT( EX_USAGE,
        format_opt( *opt ) << " requires "
        << (reqs_multiple ? "one of " : "")
        << "the -" << req_opts << " option" << (reqs_multiple ? "s" : "")
        << " to be given also\n"
      );
    }
  } // for
}

/**
 * Formats an option as `[--%%s/]-%%c` where `%%s` is the long option (if any)
 * and `%%c` is the short option.
 *
 * @param short_opt The short option (along with its corresponding long option,
 * if any) to format.
 * @return Returns the formatted option.
 */
static string format_opt( char short_opt ) {
  ostringstream oss;
  if ( char const *const long_opt = get_long_opt( short_opt ) )
    oss << "--" << long_opt << '/';
  oss << '-' << short_opt;
  return oss.str();
}

/**
 * Gets the corresponding name of the long option for the given short option.
 *
 * @param short_opt The short option to get the corresponding long option for.
 * @return Returns the said option or null if none.
 */
static char const* get_long_opt( char short_opt ) {
  for ( struct option const *long_opt = LONG_OPTS; long_opt->name != nullptr;
        ++long_opt ) {
    if ( long_opt->val == short_opt )
      return long_opt->name;
  } // for
  return nullptr;
}

/**
 * Parses command-line options.
 *
 * @param pargc A pointer to the argument count from `main()`.
 * @param pargv A pointer to the argument values from `main()`.
 */
static void parse_options( int *pargc, char const ***pargv ) {
  ::optind = ::opterr = 1;
  bool print_version = false;

	for (;;) {
    char const opt = options_get( pargc, pargv, SHORT_OPTS, LONG_OPTS );
    if ( opt == '\0' )
      break;
    switch ( opt ) {
      case 'h': opt_lang              = lang::CPP;      // no break;
      case 'd': opt_declaration_path  = optarg;               break;

      case 'c': opt_lang              = lang::CPP;      // no break;
      case 'D': opt_definition_path   = optarg;               break;

#ifdef ENABLE_JAVA
      case 'j': opt_lang              = lang::JAVA;
                opt_declaration_path  = optarg;
                opt_definition_path   = optarg;               break;
#endif /* ENABLE_JAVA */

      case 'E': opt_codegen_only      = true;                 break;
      case 'P': opt_line_directives   = false;                break;
#ifdef ENABLE_STACK_DEBUG
      case 'S': opt_stack_debug       = true;                 break;
#endif /* ENABLE_STACK_DEBUG */
      case 'v': print_version         = true;                 break;
      case 'x': opt_lang              = parse_lang( optarg ); break;
#ifdef YYDEBUG
      case 'y': ::yydebug             = true;                 break;
#endif /* YYDEBUG */
      default : usage();
    } // switch
  } // for

  check_mutually_exclusive( "ch", "jx" );
  check_mutually_exclusive( "E", "cdDhj" );
  check_mutually_exclusive( "j", "cdDEhx" );
  check_mutually_exclusive( "v", "cdDEhjPSxy" );

  check_required( "cD", "dh" );
  check_required( "dh", "cD" );

  if ( print_version ) {
    cerr << PACKAGE_STRING << endl;
    ::exit( EX_OK );
  }

  if ( *pargc != 1 )
    usage();
}

/**
 * Prints the usage message to standard error.
 */
static void usage() {
  cerr <<
"usage: " << me << " [options] infile\n"
"\n"
"options:\n"
"  -c file                Same as --definition/-D; implies -xc++\n"
"  --declaration/-d file  Set declaration file.\n"
"  --definition/-D file   Set definition file.\n"
"  -h file                Same as --declaration/-d; implies -xc++.\n"
#ifdef ENABLE_JAVA
"  --java/-j file         Same as -d and -D; implies -xjava.\n"
#endif /* ENABLE_JAVA */
"  --language/-x lang     Set language to generate [default: C++].\n"
"  --no-line/-P           Suppress #line directives in generated C++ code.\n"
#ifdef ENABLE_STACK_DEBUG
"  --stack-debug/-S       Enable stack debugg output.\n"
#endif /* ENABLE_STACK_DEBUG */
"  --stdout/-E            Generate all code to stdout.\n"
"  --version/-v           Print version and exit.\n"
#ifdef YYDEBUG
"  --yydebug/-y           Enable Bison debug output.\n"
#endif /* YYDEBUG */
  ;
  exit( EX_USAGE );
}

////////// extern functions ///////////////////////////////////////////////////

char options_get( int *pargc, char const ***pargv, char const *short_opts,
                  struct option const long_opts[] ) {
    int const opt = getopt_long(
      *pargc, const_cast<char**>( *pargv ), short_opts, long_opts, nullptr
    );
    if ( opt == -1 ) {
      *pargc -= ::optind, *pargv += ::optind;
      return '\0';
    }
    mark_option( opt );
    return static_cast<char>( opt );
}

void options_init( int *pargc, char const ***pargv ) {
  me = PJL::base_name( (*pargv)[0] );
  parse_options( pargc, pargv );
}

lang parse_lang( char const *s ) {
  typedef unordered_map<string,lang> lang_map_type;
  static lang_map_type const lang_map {
    { "c++",  lang::CPP   },
#ifdef ENABLE_JAVA
    { "java", lang::JAVA  },
#endif /* ENABLE_JAVA */
  };
  assert( s != nullptr );
  auto const found = lang_map.find( PJL::tolower( s ) );
  if ( found == lang_map.end() ) {
    PMESSAGE_EXIT( EX_USAGE,
      '"' << s << "\": unsupported language for " << format_opt( 'x' ) << '\n'
    );
  }

  return found->second;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
