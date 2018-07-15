/*
**      CHSM Language System
**      src/c++/chsmc/lang_parser.cpp
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
#include "config.h"
#include "cpp_parser.h"
#ifdef ENABLE_JAVA
#include "java_parser.h"
#endif /* ENABLE_JAVA */
#include "lexer.h"
#include "string_builder.h"
#include "util.h"

// standard
#include <cassert>
#include <cctype>
#include <cstdio>                       /* for EOF */
#include <cstring>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

inline bool is_ident_1st( char c ) {
  return isalpha( c ) || c == '_';
}

inline bool is_ident_rest( char c ) {
  return isalnum( c ) || c == '_';
}

///////////////////////////////////////////////////////////////////////////////

lang_parser::exception::~exception() {
  // out-of-line since it's virtual
}

char const* lang_parser::exception::what() const noexcept {
  return msg_.c_str();
}

///////////////////////////////////////////////////////////////////////////////

bool operator==( lang_parser::token const &ti, lang_parser::token const &tj ) {
  return  ti.get_type() == tj.get_type() &&
          (ti.has_char() || ti.get_str() == tj.get_str());
}

ostream& operator<<( ostream &os, lang_parser::token::type tt ) {
  switch ( tt ) {
    case lang_parser::token::NONE:
      os << "none";
      break;
    case lang_parser::token::IDENTIFIER:
      os << "identifier";
      break;
    case lang_parser::token::NUMBER:
      os << "number";
      break;
    case lang_parser::token::AMPER:
    case lang_parser::token::LPAREN:
    case lang_parser::token::RPAREN:
    case lang_parser::token::STAR:
    case lang_parser::token::COMMA:
    case lang_parser::token::DOT:
    case lang_parser::token::SLASH:
    case lang_parser::token::COLON:
    case lang_parser::token::LANGLE:
    case lang_parser::token::EQUAL:
    case lang_parser::token::RANGLE:
    case lang_parser::token::LBRACKET:
    case lang_parser::token::RBRACKET:
      os << static_cast<char>( tt );
      break;
    default:
      ASSERT_0_MSG( "unexpected token value" );
  } // switch
  return os;
}

ostream& operator<<( ostream &os, lang_parser::token const &t ) {
  if ( t.has_str() )
    os << t.get_str();
  else
    os << t.get_type();
  return os;
}

lang_parser::token::token( type tt ) : type_( tt ) {
  assert( !::has_str( tt ) );
}

lang_parser::token::token( char c ) : type_( static_cast<type>( c ) ) {
#ifndef NDEBUG
  switch ( type_ ) {
    case token::IDENTIFIER:
    case token::NUMBER:
      ASSERT_0_MSG( "type requires string" );
    case token::NONE:
    case token::AMPER:
    case token::LPAREN:
    case token::RPAREN:
    case token::STAR:
    case token::COMMA:
    case token::DOT:
    case token::SLASH:
    case token::COLON:
    case token::LANGLE:
    case token::EQUAL:
    case token::RANGLE:
    case token::LBRACKET:
    case token::RBRACKET:
      return;
    default:
      ASSERT_0_MSG( "unexpected token character" );
  } // switch
#endif /* NDEBUG */
}

lang_parser::token::token( type tt, std::string const &s ) :
  str_{ s },
  type_{ tt }
{
  assert( has_str() );
}

lang_parser::token::token( token &&t ) :
  str_{ std::move( t.str_ ) },
  type_{ t.type_ }
{
  t.type_ = token::NONE;
}

lang_parser::token& lang_parser::token::operator=( token &&t ) {
  str_ = std::move( t.str_ );
  type_ = t.type_;
  t.type_ = token::NONE;
  return *this;
}

string lang_parser::token::as_str() const {
  if ( has_str() )
    return get_str();
  return string{ static_cast<char>( get_type() ) };
}

char lang_parser::token::get_char() const {
  assert( has_char() );
  return static_cast<char>( type_ );
}

std::string const& lang_parser::token::get_str() const {
  assert( has_str() );
  return str_;
}

void lang_parser::token::swap( token &t ) {
  std::swap( str_, t.str_ );
  std::swap( type_, t.type_ );
}

///////////////////////////////////////////////////////////////////////////////

lang_parser::token const lang_parser::NONE;

lang_parser::lang_parser( istream &is ) :
    is_{ is },
    started_with_lparen_{ false }
{
}

lang_parser::~lang_parser() {
  // out-of-line since it's virtual
}

void lang_parser::clear() {
  PJL::clear( &parens_ );
  peeked_token_.clear();
  started_with_lparen_ = false;
}

unique_ptr<lang_parser> lang_parser::create( lang l, istream &is ) {
  unique_ptr<lang_parser> rv;
  switch ( l ) {
    case lang::CPP:
      rv.reset( new cpp_parser{ is } );
      break;
#ifdef ENABLE_JAVA
    case lang::JAVA:
      rv.reset( new java_parser{ is } );
      break;
#endif /* ENABLE_JAVA */
    default:
      INTERNAL_ERR( static_cast<int>( l ) << ": unexpected value for lang\n" );
  } // switch
  return rv;
}

inline char lang_parser::get_char_from_istream_() {
  int const c_in = is_.get();
  return unlikely( c_in == EOF ) ? '\0' : static_cast<char>( c_in );
}

char lang_parser::get_char_() {
  char c = get_char_from_istream_();
  switch ( c ) {
    case '(':
    case '<':
    case '[':
      parens_.push( c );
      break;
    case ')':
    case '>':
    case ']':
      if ( parens_.empty() )
        throw exception{
          BUILD_STRING( '\'', c, "': unmatched character" )
        };
      if ( parens_.top() != PJL::opening_char( c ) )
        throw exception{
          BUILD_STRING( '\'', c, "': unmatched character" )
        };
      parens_.pop();
      if ( c == ')' && parens_.empty() && started_with_lparen_ ) {
        //
        // We parsed an param_list within parentheses, e.g.:
        //
        //      (int x, int y)
        //
        // and we just encountered the ')', so it should signal the end of the
        // parameter list just as if we encountered EOF.
        //
        c = '\0';
      }
      break;
  } // switch

  return c;
}

lang_parser::token lang_parser::get_token_() {
  if ( peeked_token_ )
    return std::move( peeked_token_ );

  for ( char c; (c = get_char_()) != '\0'; ) {
    if ( isspace( c ) )
      continue;
    if ( is_ident_1st( c ) )
      return token{ token::IDENTIFIER, parse_identifier_( c ) };
    if ( isdigit( c ) )
      return token{ token::NUMBER, parse_number_( c ) };
    switch ( c ) {
      case token::SLASH:
        if ( char c2 = get_char_() ) {
          parse_comment( c2 );
          continue;
        }
        // FALLTHROUGH
      case token::AMPER:
      case token::COLON:
      case token::COMMA:
      case token::DOT:
      case token::EQUAL:
      case token::LANGLE:
      case token::LBRACKET:
      case token::LPAREN:
      case token::RANGLE:
      case token::RBRACKET:
      case token::RPAREN:
      case token::STAR:
        return token{ c };
    } // switch
  } // for

  return token{};
}

lang_parser::token lang_parser::get_token_if_type_( token::type tt ) {
  token const t{ get_token_() };
  if ( t == tt )
    return t;
  unget_token_( t );
  return token{};
}

lang_parser::token lang_parser::get_token_of_type_( token::type tt ) {
  token const t{ get_token_() };
  if ( !t || t == tt )
    return t;
  unexpected_token( t, tt );
}

bool lang_parser::is_param_end_( token const &t ) {
  if ( !t )
    return true;
  if ( t != token::COMMA )
    return false;
  //
  // There are 2 cases:
  //
  //  1. Separating top-level parameters, e.g.:
  //
  //          int x, int y
  //
  //     In this case, the ',' is the end of the parameter.
  //
  //  2. Separating pointer-to-function parameters, e.g.:
  //
  //          void (*f)(int x, int y)
  //
  //     In this case, the ',' is NOT the end of the parameter.
  //
  // Whether this ',' is the last comma depends on whether we're within (...)
  // or not.
  //
  return !within_parens_();
}

fn_param_list lang_parser::parse_param_list() {
  clear();
  started_with_lparen_ = !!get_token_if_type_( token::LPAREN );

  fn_param_list param_list;
  for ( fn_param param; parse_param_( &param ); )
    param_list.push_back( std::move( param ) );

  if ( !parens_.empty() )
    unexpected_token();
  return param_list;
}

void lang_parser::parse_comment( char c ) {
  switch ( c ) {
    case '/':                           // //-style comment
      while ( (c = get_char_()) != '\0' && c != '\n' )
        ;
      break;
    case '*':                           // /*...*/-style comment
      while ( (c = get_char_()) != '\0' ) {
        if ( c == '*' && peek_char_() == '/' ) {
          (void)get_char_();
        }
      } // while
      break;
    default:
      unget_char_( c );
  } // switch
}

string lang_parser::parse_identifier_( char c ) {
  string identifier;
  identifier += c;
  while ( (c = get_char_()) != '\0' ) {
    if ( !is_ident_rest( c ) ) {
      unget_char_( c );
      break;
    }
    identifier += c;
  } // while
  return identifier;
}

string lang_parser::parse_number_( char c ) {
  string number;
  number += c;

  char const *charset;
  if ( c == '0' ) {                     // binary, octal, or hexadecimal
    switch ( (c = get_char_()) ) {
      case 'b':
        charset = "01";
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        charset = "01234567";
        break;
      case 'x':
      case 'X':
        charset = "0123456789ABCDEFabcdef";
        break;
      case '\0':
        return number;
      default:
        unexpected_char( c );
    } // switch
    number += c;
  } else {                              // decimal
    charset = "0123456789";
  }

  while ( (c = get_char_()) != '\0' ) {
    if ( strchr( charset, c ) == nullptr ) {
      unget_char_( c );
      break;
    }
    number += c;
  } // while

  return number;
}

char lang_parser::peek_char_() {
  int const c_in = is_.peek();
  return unlikely( c_in == EOF ) ? '\0' : static_cast<char>( c_in );
}

lang_parser::token lang_parser::peek_token_() {
  if ( !peeked_token_ )
    peeked_token_ = get_token_();
  return peeked_token_;
}

lang_parser::token lang_parser::require_token_() {
  token const t{ get_token_() };
  if ( !t )
    throw exception{ BUILD_STRING( "unexpected end of tokens" ) };
  return t;
}

lang_parser::token lang_parser::require_token_of_type_( token::type tt ) {
  token const t{ get_token_() };
  if ( !t || t != tt )
    unexpected_token( t, tt );
  return t;
}

void lang_parser::unexpected_char( char c ) {
  assert( c != '\0' );
  throw exception{ BUILD_STRING( "'", c, "': unexpected character" ) };
}

void lang_parser::unexpected_token( token const &t, token::type tt ) {
  if ( !t )
    throw exception{
      BUILD_STRING( "unexpected end of tokens; ", tt, " expected" )
    };
  assert( t != tt );
  throw exception{
    BUILD_STRING( "\"", t, "\": unexpected; ", tt, " expected" )
  };
}

void lang_parser::unget_char_( char c ) {
  assert( c != '\0' );
  is_.putback( c );

  switch ( c ) {
    case '(':
    case '<':
    case '[':
      assert( !parens_.empty() );
      parens_.pop();
      break;
    case ')':
    case '>':
    case ']':
      parens_.push( PJL::opening_char( c ) );
      break;
  } // switch
}

void lang_parser::unget_token_( token const &t ) {
  assert( !peeked_token_ );
  assert( t );
  peeked_token_ = t;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
