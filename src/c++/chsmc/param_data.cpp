/*
**      CHSM Language System
**      src/c++/chsmc/param_data.cpp
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
#include "list_sep.h"
#include "param_data.h"

// standard
#include <cassert>
#include <cstring>

using namespace std;

param_data::emit_mask param_data::default_emit_flags_ = 0;
char const            param_data::PARAM_PREFIX_[]     = "P";

///////////////////////////////////////////////////////////////////////////////

param_data::~param_data() {
  // out-of-line since it's virtual
}

ostream& param_data::emit_param_list( ostream &o, emit_mask emit_flags ) const {
  list_sep comma;
  if ( (emit_flags & EMIT_COMMA) != 0 )
    comma.print();
  return emit_param_list_aux( o, comma, emit_flags );
}

ostream& param_data::emit_param_list_aux( ostream &o, list_sep &comma,
                                          emit_mask emit_flags ) const {
  for ( auto const &param : param_list_ ) {
    o << comma;
    if ( (emit_flags & EMIT_ACTUAL) != 0 ) {
      //
      // Emit an actual parameter list, i.e., just the names.
      //
      o << ((emit_flags & EMIT_PREFIX) != 0 ? PARAM_PREFIX_ : "")
        << param.name_;
    } else {
      //
      // Emit a formal parameter list, i.e., the types and names.
      //
      o << stuff_decl( param.decl_,
        ((emit_flags & EMIT_PREFIX) != 0 ? PARAM_PREFIX_ : ""),
        ((emit_flags & EMIT_FORMAL) != 0 ? param.name_ : "")
      );
    }
  } // for
  return o;
}

char const* param_data::stuff_decl( char const *decl, char const *s1,
                                    char const *s2 ) {
  static char decl_buf[ 256 ];                // should be big enough
  ::strcpy( decl_buf, decl );
  char *const x = ::strchr( decl_buf, '$' );  // x marks the spot
  assert( x != nullptr );
  ::strcpy( x, s1 );
  ::strcat( x, s2 );
  ::strcpy( x + ::strlen( s1 ) + ::strlen( s2 ), decl + (x - decl_buf + 1) );
  return decl_buf;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
