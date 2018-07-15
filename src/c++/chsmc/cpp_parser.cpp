/*
**      CHSM Language System
**      src/c++/chsmc/cpp_parser.cpp
**
**      Copyright (C) 2018  Paul J. Lucas
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
#include "cpp_parser.h"
#include "string_builder.h"

// standard
#include <cassert>
#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

bool cpp_parser::parse_param_( fn_param *a ) {
  assert( a != nullptr );
  a->clear();
  //
  // Parsing a C++ function parameter is generally difficult; however, we rely
  // on the observation that the parameter name is the right-most identifier in
  // the left-most set of parentheses (if any).
  //
  //      int x
  //      int *x
  //      int (*x)(int i)
  //      int (*const x)(int i)
  //

  token t{ get_token_of_type_( token::IDENTIFIER ) };
  if ( !t )
    return false;

  vector<string> decl;
  bool found_rparen = false;
  size_t name_index = 0;
  token::type tt_prev{ t.get_type() };

  decl.push_back( t.get_str() );
  t = require_token_();

  while ( !is_param_end_( t ) ) {
    switch ( t.get_type() ) {
      case token::IDENTIFIER:
        switch ( tt_prev ) {
          case token::IDENTIFIER:
          case token::RANGLE:
            decl.push_back( " " );
            break;
          default:
            /* do nothing */;
        } // switch
        if ( !found_rparen ) {
          //
          // Since we haven't found a ')' yet, keep moving name_index along so
          // it's always the right-most identifier in the left-most set of
          // parentheses (if any).
          //
          name_index = decl.size();
        }
        break;
      case token::AMPER:
        if ( tt_prev == token::RANGLE ) {
          decl.push_back( " " );
          break;
        }
        // FALLTHROUGH
      case token::LPAREN:
      case token::STAR:
        if ( is_punct( t ) != is_punct( tt_prev ) )
          decl.push_back( " " );
        break;
      case token::RPAREN:
        found_rparen = true;
        break;
      default:
        /* do nothing */;
    } // switch

    decl.push_back( t.as_str() );
    if ( t == token::COMMA )
      decl.push_back( " " );

    tt_prev = t.get_type();
    t = get_token_();
  } // while

  if ( name_index > 0 ) {
    a->name_ = decl[ name_index ];
    decl[ name_index ] = "$";
    for ( auto const &i : decl )
      a->decl_ += i;
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
