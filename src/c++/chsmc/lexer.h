/*
**      CHSM Language System
**      src/c++/chsmc/lexer.h
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

#ifndef chsmc_lexer_H
#define chsmc_lexer_H

///////////////////////////////////////////////////////////////////////////////

/**
 * Controls the lexical analyzer's behavior from the grammar to do context-
 * dependent lexical analysis.
 *
 * See the comments in `lexer.lpp` for more information.
 */
class lexer {
public:
  /**
   * The lexical analyzer's tokenization state.
   */
  enum state_type {
    STATE_NORMAL,                       ///< Normal CHSM tokenization.
    STATE_MAYBE_CCLASS,                 ///< Tokenize a C++ class name.
    STATE_MAYBE_CEXPR,                  ///< Tokenize a C++ expression.
    STATE_MAYBE_CPARAMS                 ///< Tokenize a C++ parameter list.
  };

  /**
   * Gets the singleton instance of the %lexer.
   *
   * @return Returns said %lexer.
   */
  static lexer& instance();

  /**
   * Gets the current tokenization state.
   *
   * @return Return said state.
   */
  state_type state() const {
    return state_;
  }

  /**
   * Pushes a tokenization state.
   *
   * @param s The new state.
   */
  void push_state( state_type s ) {
    stack_[ ++stack_p_ ] = state_;
    state_ = s;
  }

  /**
   * Pops a tokenization state.
   */
  void pop_state();

  /**
   * Text of current token.  Since lex's `yytext` is sometimes declared as a
   * `char*` and other times as `char[]` (depending on the implementation),
   * we'll use this instead.
   */
  char *token;

private:
  lexer();

  state_type  state_;
  state_type  stack_[5];                // big enough for what we're doing
  int         stack_p_;
};

extern "C" int yylex( void );           ///< Lexical analyzer defined by lex.

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_lexer_H */
/* vim:set et ts=2 sw=2: */
