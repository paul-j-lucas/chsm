/*
**      CHSM Language System
**      src/c++/chsmc/code_generator.cpp
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
#include "code_generator.h"
#include "chsm_compiler.h"
#include "cpp_generator.h"
#include "java_generator.h"
#include "options.h"

// standard
#include <cassert>
#include <unordered_map>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

unique_ptr<code_generator> code_generator::create( lang l ) {
  switch ( l ) {
    case lang::CPP:
      return cpp_generator::create();
    case lang::JAVA:
      return java_generator::create();
    default:
      INTERNAL_ERR( static_cast<int>( l ) << ": unexpected value for lang\n" );
  } // switch
}

void code_generator::emit_source_line_no( ostream &o, unsigned alt_no ) const {
  if ( opt_line_directives )
    o << "//#line " << (alt_no != 0 ? alt_no : cc.source_->line_no_)
      << " \"" << cc.source_->path() << "\"\n";
}

void code_generator::emit_the_end() const {
  T_OUT T_ENDL
        << section_comment << "THE END" T_ENDL;
}

lang code_generator::map_ext_to_lang( string const &ext ) {
  typedef std::unordered_map<string,lang> ext_map_type;
  static ext_map_type const ext_map {
    { "chsmc", lang::CPP  },
    { "chsmj", lang::JAVA },
  };
  auto const found = ext_map.find( ext );
  return found != ext_map.end() ? found->second : lang::NONE;
}

ostream& code_generator::section_comment( ostream &o ) {
  return o << "///// " << PACKAGE_NAME << ": ";
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
