/*
**      CHSM Language System
**      src/c++/chsm2c++/lex.l
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

%{

// standard
#include <iostream>
#include <cstdlib>                      /* for exit() */
#include <cstring>

// local
#include "chsm_compiler.h"
#include "lex.h"
#include "y.tab.h"

using namespace PJL;
using namespace std;

/*****************************************************************************
 *  Global variables
 *****************************************************************************/

lexer                   g_lexer;        // the singleton instance

//
// Used during condition and argument-list processing.
//
static int              g_balance = 0;  // used to balance []'s

//
// Used during substate processing.  (See additional comments below.)
//
static int              g_just_did_ident = 0;

/*****************************************************************************
 *  Implement a start-condition stack
 *****************************************************************************/

#ifdef FLEX_SCANNER
    typedef int         stack_type;
#   define LEX_STATE    yy_start
#else /* assume lex */
    typedef yysvf*      stack_type;
#   define LEX_STATE    yybgin
#endif
static int              lex_stack_p = -1;
static stack_type       lex_stack[ 5 ]; // arbitrary...should be enough
#define CALL( s )       { lex_stack[ ++lex_stack_p ] = LEX_STATE; BEGIN( s ); }
#define RESUME          LEX_STATE = lex_stack[ lex_stack_p-- ]

/*****************************************************************************
 *  Alter lex things for C++
 *****************************************************************************/

#ifdef FLEX_SCANNER

#   ifdef YY_FATAL_ERROR
#       undef YY_FATAL_ERROR
#   endif
#   define YY_FATAL_ERROR( msg ) {              \
        g.fatal() << msg << endl;               \
        ::exit( compiler::File_Read_Error );    \
    }

#   ifdef YY_INPUT
#       undef YY_INPUT
#   endif
#   define YY_INPUT( buf, result, max_size ) {                  \
        if ( g.source_->in().read( buf, max_size ).bad() )      \
            YY_FATAL_ERROR( "read() in flex scanner failed" );  \
        g.source_->in().clear( ios_base::goodbit );             \
        result = g.source_->in().gcount();                      \
    }

#else /* assume lex */

#   ifdef input
#       undef input
#   endif
#   define input() ((yytchar = g.source_->in().get()) == EOF ? 0 : yytchar)
#   ifdef lex_input
#       undef lex_input
#       define lex_input() input()
#   endif

#   ifdef output
#       undef output
#   endif
#   define output(c) g.target_->out().put( (char)(c) )
#   ifdef lex_output
#       undef lex_output
#       define lex_output(c) output( c )
#   endif

#   ifdef unput
#       undef unput
#   endif
#   define unput(c) g.source_->in().putback( (char)(c) )
#   ifdef lex_unput
#       undef lex_unput
#       define lex_unput(c) unput( c )
#   endif

    //
    // If you get linkage errors from the C++ compiler, try changing the 0
    // to 1.
    //
#if 0
    extern "C" {
        int yyback( int*, int );
        void    yyless( int );
        int yylook( void );
        int yywrap( void );
    }
#endif

#endif  /* FLEX_SCANNER */

#ifdef ECHO
#   undef ECHO
#endif
#define ECHO        g.target_->out() << ::yytext

// This is used to divert code to the temporary user code file.
#define U_ECHO      g.user_code_->out() << ::yytext

/*****************************************************************************
 *  Miscellany
 *****************************************************************************/

inline void lex_error( char const* msg ) {
    g.source_->error() << "(lexical) " << msg << endl;
}

//
// We must use this macro to return tokens to ensure that the global text
// pointer gets set.  Ordinarily, one wouldn't have to do this for simple
// tokens, but the subroutine production arg_cat in grammar.y needs all the
// text to glue together.  See the comments in grammar.y for more info.
//
#define RETURN(TOKEN)   { g_lexer.text = ::yytext; return TOKEN; }

%}

/*****************************************************************************
 *  Start states
 *****************************************************************************/

%s MAIN
    /*
    ** The MAIN state is used to process the CHSM description section, i.e.,
    ** after the first %% and before the second, if any.
    */

%s COND
    /*
    ** The COND state is used between [ and ] for event conditions that are
    ** arbitrary C++ expressions.  Text is diverted to the temporary user- code
    ** file.  During this state, the $ token is recognized as an escape for
    ** specifying embedded constructs; see the comment for EMBED.
    */

%s CODE
    /*
    ** The CODE state is used between %{ and %} for arbitrary C++ code.  Text
    ** is diverted to the temporary user-code file.  During this state, the $
    ** token is recognized as an escape for specifying embedded constructs; see
    ** the comment for EMBED.
    */

%s EMBED
    /*
    ** The EMBED state is used as a sub-state during either COND or CODE for
    ** recognizing the embedded constructs:
    **
    **      $enter(s) $exit(s) $in(s) ${s}
    */

%s TEMPLATE VECTOR
    /*
    ** Both the TEMPLATE and the VECTOR states are used between ( and ) for
    ** event parameter declarations that are C++ declarations.  Text between <
    ** and > for TEMPLATE and [ and ] for VECTOR is irrelevant as far as we're
    ** concerned, so it is passed along to the underlying C++ compiler.
    */

%s DONE 
    /*
    ** The DONE state is used after the second %%, if any.  Once in this state,
    ** all text is copied straight through verbatim.  This state is never left.
    */

/*****************************************************************************
 *  Character classes
 *****************************************************************************/

white       [ \b\f\r\t\v]
letter      [A-Za-z_]
digit       [0-9]

ident       {letter}({letter}|{digit})*
substates   (\.{ident})+

/*****************************************************************************
 *  Lexical description
 *****************************************************************************/

%%
    /*
    ** This code is executed upon every call to yylex() before it does anything
    ** else.
    **
    ** If we returned T_IDENT on our last return, we have to leave the
    ** "g_just_did_ident" flag set so that T_SUBSTATES will be recognized if it
    ** occurs.  The _next_ time we enter, reset the flag.
    */
    if ( g_just_did_ident == 1 )
        ++g_just_did_ident;
    else
        g_just_did_ident = 0;

"/*"                        {   /*
                                ** Handle C-style comment.
                                **
                                ** See also: John Levine, Tony Mason, & Doug
                                ** Brown.  "lex & yacc," 2nd edition, O'Reilly
                                ** & Associates, Inc., October 1992, p. 158.
                                */
                                for ( int c = yyinput(), prev = 0;
                                      c != EOF; prev = c, c = yyinput() )
                                {
                                    if ( prev == '*' && c == '/' )
                                        break;
                                    if ( c == '\n' )
                                        ++g.source_->line_no_;
                                }
                            }
"//".*                      /* Handle C++-style comment */;

<INITIAL>^%%                { BEGIN( MAIN ); return T_PERCENT_PERCENT; }
<MAIN>^%%                   { BEGIN( DONE ); return T_PERCENT_PERCENT; }
<INITIAL,DONE>.+            { ECHO; }

<INITIAL>\n                 { ++g.source_->line_no_; ECHO; }
<MAIN,TEMPLATE,VECTOR>\n    { ++g.source_->line_no_; }
<COND,CODE,EMBED>\n         { ++g.source_->line_no_; U_ECHO; }
<MAIN,EMBED>{white}+        { /* nothing */; }

<COND,CODE>\"               {   /*
                                ** Handle string literal.
                                */
                                U_ECHO;
                                for ( int c = yyinput(), prev = 0;
                                    c != EOF; prev = c, c = yyinput()
                                ) {
                                    g.user_code_->out() << (char)c;

                                    if ( prev == '\\' ) {
                                        if ( c == '\n' ) {
                                            /*
                                            ** Keep the line number correct
                                            ** even when a string literal
                                            ** contains a newline.
                                            */
                                            ++g.source_->line_no_;
                                        }
                                        continue;
                                    }

                                    if ( c == '\n' ) {
                                        lex_error("newline in string literal");
                                        ++g.source_->line_no_;
                                        break;
                                    }

                                    if ( c == '"' )
                                        break;
                                }
                            }

<COND,CODE>'(.|\')+'        { U_ECHO; }
<COND,CODE>'                |
<COND,CODE>''               { lex_error( "bad character literal" ); }

<MAIN,EMBED>{ident}         {
                                struct keyword {
                                    char const* name;
                                    int         token;
                                };
                                static keyword const keywords[] = {
                                    /*
                                    ** in alphabetical order
                                    */
                                    "chsm",     T_CHSM,
                                    "cluster",  T_CLUSTER,
                                    "const",    T_CONST_OR_VOLATILE,
                                    "deep",     T_DEEP,
                                    "enter",    T_ENTER,
                                    "event",    T_EVENT,
                                    "exit",     T_EXIT,
                                    "history",  T_HISTORY,
                                    "in",       T_IN,
                                    "is",       T_IS,
                                    "set",      T_SET,
                                    "state",    T_STATE,
                                    "upon",     T_UPON,
                                    "volatile", T_CONST_OR_VOLATILE,
                                    0,          0
                                };
                                /*
                                ** Check to see if it's a keyword; if not, it's
                                ** just a T_IDENT.  A simple linear search is
                                ** good enough for such a short list.
                                */
                                for ( register keyword const*
                                      k = keywords; k->name; ++k )
                                {
                                    int cmp = ::strcmp( ::yytext, k->name );
                                    if ( cmp < 0 )
                                        break;
                                    if ( cmp == 0 )
                                        RETURN( k->token );
                                }
                                g_just_did_ident = 1;
                                RETURN( T_IDENT );
                            }

<MAIN,EMBED>{substates}     {
                                /*
                                ** There's an ambiguity between T_SUBSTATES and
                                ** T_DOTS when T_DOTS is just one dot, e.g.,
                                **
                                **      .s  "TDOTS T_IDENT" or "T_SUBSTATES" ?
                                **
                                ** To resolve this, we rely on a global flag
                                ** "g_just_did_ident"; T_SUBSTATES is only
                                ** valid in the grammar after a T_IDENT, so
                                ** ".s" not preceded by a T_IDENT is taken to
                                ** be "T_DOTS T_IDENT."
                                */
                                if ( g_just_did_ident )
                                    RETURN( T_SUBSTATES );
                                yyless( 1 );
                                RETURN( T_DOTS );
                            }

<MAIN>"["                   {
                                /*
                                ** Normally, []'s are just returned as ordinary
                                ** tokens.  However, if we're doing special
                                ** processing, do not return text up to the
                                ** matching ]; instead divert it for full
                                ** syntax checking by the underlying C++
                                ** compiler later (behind yacc's back).  While
                                ** doing this, we still have to count and
                                ** balance []'s to know when to stop the covert
                                ** activity.
                                */
                                switch ( g_lexer.state() ) {
                                    case lexer::arg_list_processing:
                                        BEGIN( VECTOR );
                                        break;

                                    case lexer::condition_processing:
                                        BEGIN( COND );
                                        break;
                                }
                                g_balance = 1;
                                RETURN( ::yytext[0] );
                            }

<COND>"]"                   {
                                if ( !--g_balance ) {
                                    /*
                                    ** The []'s are now balanced: return to our
                                    ** normal state.
                                    */
                                    BEGIN( MAIN );
                                    RETURN( ::yytext[0] );
                                } else
                                    U_ECHO;
                            }

<VECTOR>"]"                 {
                                if ( !--g_balance ) {
                                    /*
                                    ** The []'s are now balanced: return to our
                                    ** normal state.  Note that ::yytext
                                    ** contains all the text between [ and ].
                                    */
                                    BEGIN( MAIN );
                                    RETURN( ']' );  // can not do ::yytext[0]
                                } else
                                    yymore();
                            }

<MAIN>"<"                   {
                                /*
                                ** Similarly to []'s above, <>'s are normally
                                ** returned as ordinary tokens.  However, if
                                ** we're doing argument list processing, do not
                                ** return text up to the matching >; instead
                                ** accumulate it into ::yytext by using
                                ** yymore() for full syntax checking by the
                                ** underlying C++ compiler later (behind yacc's
                                ** back).  Again, we still have to count and
                                ** balance <>'s to know when to stop the covert
                                ** activity.
                                */
                                switch ( g_lexer.state() ) {
                                    case lexer::arg_list_processing:
                                        BEGIN( TEMPLATE );
                                        break;
                                }
                                g_balance = 1;
                                RETURN( ::yytext[0] );
                            }

<TEMPLATE>">"               {
                                if ( !--g_balance ) {
                                    /*
                                    ** The <>'s are now balanced: return to our
                                    ** normal state.  Note that ::yytext
                                    ** contains all the text between < and >.
                                    */
                                    BEGIN( MAIN );
                                    RETURN( '>' );  // can not do ::yytext[0]
                                } else
                                    yymore();
                            }

<COND>"["                   { ++g_balance; U_ECHO; }
<TEMPLATE>"<"               |
<VECTOR>"["                 { ++g_balance; yymore(); }
<TEMPLATE>[^>\n]+           |
<VECTOR>[^\]\n]+            { yymore(); }

<MAIN>[,;()>{}]             |
<EMBED>[({]                 { RETURN( ::yytext[0] ); }
<EMBED>[)}]                 { RESUME; RETURN( ::yytext[0] ); }
<MAIN>"->"                  { RETURN( T_ARROW ); }
<MAIN>[*&]                  { RETURN( T_PTR_OPERATOR ); }
<MAIN>::{white}*\*          { RETURN( T_COLON_COLON_STAR ); }
<MAIN,EMBED>"::"            { RETURN( T_COLON_COLON ); }
<MAIN,EMBED>"."+            { RETURN( T_DOTS ); }
<MAIN>"%{"                  { BEGIN( CODE ); return T_LCODE; }
<CODE>"%}"                  { BEGIN( MAIN ); return T_RCODE; }
<COND,CODE>"$"              { CALL( EMBED ); return ::yytext[0]; }

<COND,CODE>.                { U_ECHO; }
<MAIN,EMBED>.               { return T_ERROR; }

%%

lexer::lexer() : state_( normal_processing ), stack_p_( -1 ) {
    // no nothing else
}
/* vim:set et ts=4 sw=4: */
