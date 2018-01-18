/*
**      CHSM Language System
**      src/c++/chsmc/main.cpp
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

// local
#include "config.h"                     /* must go first */
#include "chsm_compiler.h"
#include "code_generator.h"
#include "options.h"

// standard
#include <sysexits.h>

using namespace std;

// extern functions
extern int yyparse();

////////// main ///////////////////////////////////////////////////////////////

/**
 * The main entry point.
 *
 * @param argc The command-line argument count.
 * @param argv The command-line argument values.
 * @return Returns 0 on success, non-zero on failure.
 */
int main( int argc, char const *argv[] ) {
  options_init( &argc, &argv );

  if ( opt_lang == lang::NONE ) {       // map filename extension to language
    string const ext = PJL::path_ext( argv[0] );
    opt_lang = code_generator::map_ext_to_lang( ext );
    if ( opt_lang == lang::NONE ) {
      PMESSAGE_EXIT( EX_DATAERR,
        '"' << ext << "\": unsupported filename extension\n"
      );
    }
  }

  cc.code_gen_ = code_generator::create( opt_lang );

  if ( !opt_codegen_only ) {
    string declaration_ext, definition_ext;
    cc.code_gen_->get_filename_exts( &declaration_ext, &definition_ext );
    if ( opt_declaration_file.empty() )
      opt_declaration_file = PJL::path_noext( argv[0] ) + '.' + declaration_ext;
    if ( opt_definition_file.empty() )
      opt_definition_file = PJL::path_noext( argv[0] ) + '.' + definition_ext;
  }

  cc.source_.reset( new source_file( argv[0] ) );
  if ( opt_codegen_only )
    cc.target_.reset( new target_file( cout ) );
  else
    cc.target_.reset( new target_file( opt_declaration_file ) );
  cc.user_code_.reset( new user_code );

  return ::yyparse() == 0 && cc.source_->errors_ == 0 ? EX_OK : EX_DATAERR;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
