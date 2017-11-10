/*
**      CHSM Language System
**      src/common/chsm2xxx/lex.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef CHSM_lex_H
#define CHSM_lex_H

//*****************************************************************************
//
// SYNOPSIS
//
        class lexer
//
// DESCRIPTION
//
//      This class is used to control the lexical analyzer's behavior from the
//      grammar to do context-dependent lexical analysis.  See the comments in
//      lex.l for more information.
//
//*****************************************************************************
{
public:
    enum state_type {
        normal_processing,
        condition_processing,           // event conditions
        arg_list_processing             // event parameters
    };

    state_type  state() const           { return state_; }
    void        push_state( state_type s ) {
                    stack_[ ++stack_p_ ] = state_;
                    state_ = s;
                }
    void        pop_state() { state_ = stack_[ stack_p_-- ]; }

    //
    // Text of current token.  Since lex's yytext is sometimes declared as a
    // pointer to char and other times as a vector of char (depending on the
    // implementation), we'll use this instead.
    //
    char const* text;

    lexer();
private:
    state_type  state_;
    state_type  stack_[ 5 ];            // big enough for what we're doing
    int         stack_p_;
};
extern lexer    g_lexer;                // the singleton instance of the lexer

extern "C" int  yylex( void );          // lexical analyzer -- defined by lex

#endif  /* CHSM_lex_H */
/* vim:set et ts=4 sw=4: */
