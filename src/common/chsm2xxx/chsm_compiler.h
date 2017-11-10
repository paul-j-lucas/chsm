/*
**      CHSM Language System
**      src/common/chsm2xxx/chsm_compiler.h
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

#ifndef chsm_compiler_H
#define chsm_compiler_H

// local
#include "base.h"
#include "compiler.h"
#include "util.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct chsm_compiler : PJL::compiler
//
// DESCRIPTION
//
//      A chsm_compiler is-a compiler plus additional stuff for compiling
//      CHSMs.
//
//*****************************************************************************
{
    //
    // compiler options
    //
    struct options {
        //
        // Do not emit #line directives in resultant C++ code.
        //
        static bool no_line_;           // -d
        //
        // Emit resultant code to stdout rather than create file(s).
        //
        static bool to_stdout_;         // -E
    } options_;

    //
    // Used to remember definition file name when -E not specified.
    //
    char const*                 definition_file_name_;

    //
    // User-code file info: temporary file where user-code for conditions and
    // actions are diverted for later emission into the definition file code
    // stream.
    //
    struct user_code : file_base {
        user_code();
        ~user_code();

        void                emit( std::ostream& );
        static char const*  make_name();
    };
    user_code*                  user_code_;

    //
    // Special-purpose struct used to process event parameters.
    //
    struct arg_type {
        int     line_no_;               // line# in source file of decl
        bool    got_variable_;
        char    ident_[ 2 ][ 128 ];     // var name is in ident_[ 0 ]
        char    decl_[ 256 ];           // declaration/type of variable

        void    init( int line_no ) {
                    line_no_ = line_no;
                    got_variable_ = false;
                    *ident_[ 0 ] = *ident_[ 1 ] = '\0';
                    decl_len_ = 0;
                }
        void    cat( char const *s ) {
                    ::strcpy( decl_ + decl_len_, s );
                    decl_len_ += ::strlen( s );
                }
        void    cat( char c ) {
                    decl_[ decl_len_ ] = c;
                    decl_[ ++decl_len_ ] = '\0';
                }
    private:
        int     decl_len_;
    };
    arg_type                    arg_;

    //
    // The symbol for the CHSM itself.
    //
    static PJL::symbol*         chsm_;

    //
    // The dummy event and state name are used just to put some symbol on the
    // semantic stack when a syntax error occurs.
    //
    static PJL::symbol          dummy_event, dummy_state;

    //
    // The cluster symbol that is the outermost one to specify a deep history.
    //
    static PJL::symbol*         outer_deep_;

    //
    // The symbol of the current parent state.
    //
    static PJL::symbol*         parent_;

    static PJL::symbol_table    sym_table_;

    ~chsm_compiler();

    void        backpatch_enter_exit_events();
    void        check_child_states_defined();
    void        check_events_used();
    void        check_states_defined();
    void        check_transitions();
    bool        not_exists( PJL::symbol const*, unsigned = TYPE(unknown) );
};

///////////////////////////////////////////////////////////////////////////////

//
// Singleton global instance of chsm_compiler.
//
extern chsm_compiler    g;

//
// Code emission stuff.
//
#define EMIT            g.target_->out()
#define ENDL            << (++g.target_->line_no_, '\n')
#define U_EMIT          g.user_code_->out()

#endif  /* chsm_compiler_H */
/* vim:set et ts=4 sw=4: */
