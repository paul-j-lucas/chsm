/*
**      CHSM Language System
**      src/c++/chsmc/compiler_util.cpp
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
#include "chsm_compiler.h"

// standard
#include <cstring>

using namespace PJL;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

bool type_check( symbol const *sy, base_info::symbol_type types,
                 type_condition condition, char const *more, unsigned in ) {
  if ( !sy->info( in ) && condition != MUST_EXIST ) {
    //
    // If the symbol does not have any info (hence, no type), it's acceptable
    // so long as the condition is not that the symbol must have been
    // previously declared (hence, have a type).
    //
    return true;
  }

  auto const type = type_of( sy, in );
  if ( (type & types) == TYPE(NONE) ) {
    //
    // Regardless of the condition, if the symbol has a type and it doesn't
    // match, it's unacceptable.
    //
    cc.source_->error()
      << type_string( type ) << " \"" << sy->name()
      << "\": " << type_string( types ) << " expected " << more << '\n';
    return false;
  }

  //
  // The type matches, but return true only if the condition is not that no
  // info must be associated with the symbol, i.e., we've just encountered the
  // symbol for the first time.  This prevents info from being added more than
  // once.
  //
  return condition != NO_INFO;
}

base_info::symbol_type type_of( symbol const *sy, unsigned in ) {
  synfo const *const si = sy->info( in );
  return si != nullptr ?
    static_cast<base_info const*>( si )->type() : TYPE(NONE);
}

char const* type_string( base_info::symbol_type types ) {
  static char type_buf[ 256 ];
  *type_buf = '\0';
  if ( types == TYPE(NONE) )
    types = TYPE(UNDECLARED);
  bool append_or = false;

  for ( unsigned bit = 1; bit != 0; bit <<= 1 ) {
    if ( (types & bit) != TYPE(NONE) ) {
      if ( append_or )
        ::strcat( type_buf, " or " );
      else
        append_or = true;

      switch ( bit ) {
        case TYPE(CHILD):
          ::strcat( type_buf, "child" );
          break;
        case TYPE(CHSM):
          ::strcat( type_buf, "chsm" );
          break;
        case TYPE(STATE):
        case TYPE(GLOBAL):
          ::strcat( type_buf, "state" );
          break;
        case TYPE(CLUSTER):
          ::strcat( type_buf, "cluster" );
          break;
        case TYPE(SET):
          ::strcat( type_buf, "set" );
          break;
        case TYPE(ENEX_EVENT):
          ::strcat( type_buf, "enter/exit-event");
          break;
        case TYPE(USER_EVENT):
          ::strcat( type_buf, "event" );
          break;
        default:
          ::strcat( type_buf, "undeclared" );
          break;
      } // switch
    }
  } // for

  return type_buf;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
