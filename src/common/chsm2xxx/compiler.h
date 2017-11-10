/*
**      PJL C++ Library
**      compiler.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas
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

#ifndef compiler_H
#define compiler_H

// standard
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>

// local
#include "symbol.h"

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        class compiler
//
// DESCRIPTION
//
//  A compiler is a framework for writing a compiler.
//
//*****************************************************************************
{
public:
    //
    // program name
    //
    // This is to be set in main() of the user-program to argv[0].
    //
    static char const*      me_;

    //
    // identification string and version
    //
    // To be emitted into the target code to identify the compiler and version
    // used to generate it.  Must be defined in user-program.
    //
    static char const       ident_[];   // what _we_ like to call us
    static char const       version_[];

    //
    // usage message
    //
    // Specifies command-line usage.  Must be defined in user-program.
    //
    static char const       usage_[];

    //
    // compiler base options
    //
    struct base_options {
        bool    stack_debug_;
        //      yydebug;
    };
    static base_options     base_options_;

    //
    // generic file info.
    //
    struct file_base {

        std::string const   name_;

        std::istream&       in()    { return *in_; }
        std::ostream&       out()   { return *out_; }
        std::fstream&       file()  { return *fio_; }

        file_base()         { }
        file_base( std::istream &i ) : in_ ( &i ) { }
        file_base( std::ostream &o ) : out_( &o ) { }
        file_base( char const *file_name, std::ios::openmode );
        ~file_base()        { if ( name_.length() ) fio_->close(); }
    protected:
        union {
            std::istream    *in_;
            std::ostream    *out_;
            std::fstream    *fio_;
        };
    };

    //
    // source-file info.
    //
    struct source : file_base {
        int             line_no_;
        int             errors_, warnings_;
        bool            check_only_;    // true if no errors

        //
        // Print respective message to stderr regarding current line number
        // (unless an alternate number is specified).
        //
        std::ostream&   warning( int alt_no = 0 );
        std::ostream&   error  ( int alt_no = 0 );
        std::ostream&   fatal  ( int alt_no = 0 );
        std::ostream&   sorry  ( int alt_no = 0 );

        source( std::istream &i ) : file_base( i ) { init(); }
        source( char const *file_name = 0 );
        ~source();
    protected:
        std::ostream&   complain_at( char const*, int line_no );
        void            init();
    };
    static source*          source_;

    //
    // target-file info.
    //
    struct target : file_base {
        int     line_no_;

        target( std::ostream &o ) : file_base( o ) { init(); }
        target( char const *file_name = 0 );
    protected:
        void    init() { line_no_ = 1; }
    };
    static target*          target_;

    //
    // Flag used by message-printing routines to synchronize with yyerror and
    // ensure newline characters are printed exactly when necessary.
    //
    static bool             newlined_;

    static void             initialize( int argc, char const *argv[] );
    static void             usage();
    static std::ostream&    ident_stamp( std::ostream& );
    static void             version();

    //
    // Print respective message to stderr regarding current compilation.
    // Unlike the functions in source above, these are to be used when the
    // message has nothing to do with a particular source line.
    //
    static std::ostream&    warning();
    static std::ostream&    error();
    static std::ostream&    fatal();

    //
    // Print more-detailed information about syntax error than yacc.  Used in
    // error productions in grammar.
    //
    static void             parse_error( char const* );

    enum exit_status {          // exit() codes
        Success         =  0,   // guess
        Usage_Error     =  1,   // user botched command line
        File_Open_Error =  2,   // problems opening files
        File_Read_Error =  3,   // problems reading files
        Source_Error    =  4,   // syntax or semantic errors in source
        Caught_Signal   =  5,   // caught signal, forced to terminate
        Internal_Error  =  6,   // programmer goofed
        derived_error   =  7    // (derived class errors start here)
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    static void             push_line( int , int line );
    static void             pop_line ( int&, int line );
    static void             peek_line( int&, int line, int depth );
    static void             push_line( void const*, int line );
    static void             pop_line ( void*&, int line );
    static void             peek_line( void*&, int line, int depth );
#ifdef  PJL_STACK_DEBUG
    static void             dump( char const* );
    static void             dump( symbol const* );
#endif

    class semantic {
    public:
        semantic( int x ) {
            i_ = x;
#           ifdef  PJL_STACK_DEBUG
            tag = int_t;
#           endif
        }
        semantic( void *x = 0 ) {
            p_ = x;
#           ifdef  PJL_STACK_DEBUG
            tag = ptr_t;
#           endif
        }

        operator int() const {
#           ifdef  PJL_STACK_DEBUG
            check( int_t, reinterpret_cast<void*>( i_ ) );
#           endif
            return i_;
        }
        operator void*() const {
#           ifdef  PJL_STACK_DEBUG
            check( ptr_t, p_ );
#           endif
            return p_;
        }
    private:
        union           { int i_; void *p_; };
#ifdef  PJL_STACK_DEBUG
        enum type_tag   { int_t, ptr_t } tag;

        void check( type_tag, void* ) const;
#endif
    };
};

////////// Inlines ////////////////////////////////////////////////////////////

inline std::ostream& compiler::ident_stamp( std::ostream &o ) {
    return o << "/***** <<" << ident_ << ' ' << version_ << ">> *****/";
}

inline std::ostream& compiler::warning() {
    return std::cerr << me_ << ": warning: ";
}

inline std::ostream& compiler::error() {
    return std::cerr << me_ << ": error: ";
}

inline std::ostream& compiler::fatal() {
    return std::cerr << me_ << ": fatal error: ";
}

inline void compiler::usage() {
    std::cerr << "usage: " << me_ << ' ' << usage_ << std::endl;
    std::exit( Usage_Error );
}

inline void compiler::version() {
    std::cout << ident_ << " version " << version_ << std::endl;
    std::exit( 0 );
}

inline std::ostream& compiler::source::error( int alt_no ) {
    ++errors_;
    check_only_ = true;
    return complain_at( "error", alt_no );
}

inline std::ostream& compiler::source::fatal( int alt_no ) {
    check_only_ = true;
    return complain_at( "fatal error", alt_no );
}

inline std::ostream& compiler::source::sorry( int alt_no ) {
    check_only_ = true;
    return complain_at( "sorry, not implemented", alt_no );
}

inline std::ostream& compiler::source::warning( int alt_no ) {
    ++warnings_;
    return complain_at( "warning", alt_no );
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

extern "C" {
    extern int  yydebug;

    int         yyparse( void );
    void        yyerror( char const* );
}

//*****************************************************************************
//  PUSHing, POPing, & PEEKing macro(s)
//*****************************************************************************

#ifdef  PJL_STACK_DEBUG
#   define COMP_DUMP( s )       PJL::compiler::dump( s )
#else
#   define COMP_DUMP( s )
#endif

#define PUSH( x )               PJL::compiler::push_line( x, __LINE__ )
#define PUSH_STRING( s )        { char const *const s00s = (s); \
                                PUSH( s00s ); COMP_DUMP( s00s ); }
#define PUSH_SYMBOL( s )        { PJL::symbol *const s00s = (PJL::symbol*)(s); \
                                PUSH( s00s ); COMP_DUMP( s00s ); }

#define POP( x )                PJL::compiler::pop_line( x, __LINE__ )
#define POP_INT( i )            int i; POP( i )
#define POP_TYPE( T, t )        T t; { POP_INT( i00i ); t = (T)i00i; }
#define POP_PTR( T, p )         T *p; { void *v00v; POP( v00v ); p = (T*)v00v; }
#define POP_STRING( s )         POP_PTR( char, s ); COMP_DUMP( s );
#define POP_SYMBOL( s )         POP_PTR( PJL::symbol, s ); COMP_DUMP( s );

#define PEEK( x, d )            PJL::compiler::peek_line( x, __LINE__ , d )
#define PEEK_INT( i, d )        int i; PEEK( i, d )
#define PEEK_TYPE( T, t, d )    T t; { PEEK_INT( i00i, d ); t = (T)i00i; }
#define PEEK_PTR( T, p, d )     T *p; { \
                                void *v00v; PEEK( v00v, d ); p = (T*)v00v; }
#define PEEK_SYMBOL( s, d )     PEEK_PTR( PJL::symbol, s, d ); COMP_DUMP( s )

#define TOP_INT( i )            PEEK_INT( i, 0 )
#define TOP_TYPE( T, t )        PEEK_TYPE( T, t, 0 )
#define TOP_PTR( T, p )         PEEK_PTR( T, p, 0 )
#define TOP_SYMBOL( s )         PEEK_SYMBOL( s, 0 )

#endif  /* compiler_H */
/* vim:set et ts=4 sw=4: */
