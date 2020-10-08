/*
**      CHSM Language System
**      src/c++/chsmc/options.h
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

#ifndef chsmc_options_H
#define chsmc_options_H

/**
 * @file
 * Declares global variables and functions for command-line options.
 */

// local
#include "config.h"                     /* must go first */

/// @cond DOXYGEN_IGNORE

// standard
#include <filesystem>
#include <getopt.h>
#include <string>

/// @endcode

///////////////////////////////////////////////////////////////////////////////

/**
 * The language to generate.
 */
enum class lang {
  NONE,
  CPP,
  JAVA,
};

///////////////////////////////////////////////////////////////////////////////

// extern option variables
extern bool                   opt_codegen_only;
extern std::filesystem::path  opt_declaration_path;
extern std::filesystem::path  opt_definition_path;
extern lang                   opt_lang;
extern bool                   opt_line_directives;
#ifdef ENABLE_STACK_DEBUG
extern bool                   opt_stack_debug;
#endif /* ENABLE_STACK_DEBUG */

// other extern variables
extern char const            *me;       ///< Program name.
#ifdef YYDEBUG
extern "C" int                yydebug;
#define opt_bison_debug yydebug         ///< Bison debugging.
#endif /* YYDEBUG */

////////// extern functions ///////////////////////////////////////////////////

/**
 * Parse the next command-line option, if any.
 *
 * @param pargc A pointer to the argument count.  Upon final return, it is
 * decremented by the number of options leaving the count of non-option
 * arguments.
 * @param ppargv A pointer to the argument vector.  Upon final return, it is
 * incremented past the options to the first non-option argument.
 * @param short_opts A pointer to the short options string.
 * @param long_opts A pointer to the long options array.
 * @return Returns the next short option given or `\0` if there are no more
 * options.
 */
char options_get( int *pargc, char const ***pargv, char const *short_opts,
                  struct option const long_opts[] );

/**
 * Initializes command-line option variables.
 * On return, `*pargc` and `*pargv` are updated to reflect the remaining
 * command-line with the options removed.
 *
 * @param pargc A pointer to the argument count from `main()`.
 * @param pargv A pointer to the argument values from `main()`.
 */
void options_init( int *pargc, char const ***pargv );

/**
 * Parses a string for a language.
 *
 * @param s The language string to parse.
 */
lang parse_lang( char const *s );

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_options_H */
/* vim:set et ts=2 sw=2: */
