/*
**      PJL C++ Library
**      compiler.c
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

// standard
#include <csignal>
#include <cstdlib>

// local
#include "compiler.h"

using namespace std;

namespace PJL {

char const*     compiler::me_;
compiler::base_options  compiler::base_options_;
compiler::source*   compiler::source_;
compiler::target*   compiler::target_;
bool            compiler::newlined_     = true;

/*****************************************************************************
 *      semantic stack
 *****************************************************************************/

static compiler::semantic   stack[ 100 ];
static int                  stack_p = -1;

//*****************************************************************************
//
// SYNOPSIS
//
        extern "C" void catch_signal( int sig_id )
//
// DESCRIPTION
//
//      Catch various signals and print messages for them.
//
// PARAMETERS
//
//      sig_id  The signal that was caught.
//
//*****************************************************************************
{
    bool dump = false;

    compiler::fatal() << "received ";
    switch ( sig_id ) {
        case SIGINT : cerr << "INTERRUPT";                      break;
        case SIGQUIT: cerr << "QUIT";                           break;
        case SIGILL : cerr << "ILLEGAL INSTRUCTION";    ++dump; break;
        case SIGBUS : cerr << "BUS ERROR";              ++dump; break;
        case SIGSEGV: cerr << "SEGMENTATION VIOLATION"; ++dump; break;
        default     : cerr << "SIGNAL-ID " << sig_id;
    }
    cerr << " signal" << endl;
    if ( dump )
        ::abort();
    ::exit( compiler::Caught_Signal );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */
        void compiler::initialize( int /*argc*/, char const *argv[] )
//
// DESCRIPTION
//
//      Set our executable name and set-up to catch various signals.
//
// PARAMETERS
//
//      argv    A vector of the arguments.
//
//*****************************************************************************
{
    me_ = argv[0];
    //
    // set up to catch signals
    //
    ::signal( SIGINT,  &catch_signal );
    ::signal( SIGQUIT, &catch_signal );
    ::signal( SIGILL,  &catch_signal );
    ::signal( SIGBUS,  &catch_signal );
    ::signal( SIGSEGV, &catch_signal );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::parse_error( char const *msg )
//
// DESCRIPTION
//
//      Emit a given parser error message to standard error.  Also set the
//      check_only flag.
//
// PARAMETERS
//
//      msg     The message to emit.
//
//*****************************************************************************
{
    if ( !newlined_ ) {
        //
        // Print only if yyerror() was just called, i.e., yacc isn't
        // suppressing errors; we can tell this by newlined_.
        //
        cerr << ": " << msg << endl;
        newlined_ = true;
    }
    source_->check_only_ = true;
}

//*****************************************************************************
//
// SYNOPSIS
//
        compiler::file_base::file_base( char const *file_name,
                                        ios::openmode mode )
//
// DESCRIPTION
//
//      Construct a file_base object.
//
// PARAMETERS
//
//      file_name   The name of the file.
//
//      mode        The mode to open the file in.
//
//*****************************************************************************
    : name_( file_name )
{
    if ( name_.empty() )
        return;
    if ( !( fio_ = new fstream( name_.c_str(), mode ) ) || fio_->fail() ) {
        error() << "could not open \"" << name_ << "\" for "
        << ( mode == ios::in ? "input" : "output" ) << endl;
        ::exit( File_Open_Error );
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        compiler::source::source( char const *file_name )
//
// DESCRIPTION
//
//      Construct the source file-name object.
//
// PARAMETERS
//
//      file_name   The source file name.
//
//*****************************************************************************
    : compiler::file_base( file_name, ios::in )
{
    init();
}

//*****************************************************************************
//
// SYNOPSIS
//
        compiler::source::~source()
//
// DESCRIPTION
//
//      Clean-up and destroy a compiler::source file object:
//      1. Print a newline if needed.
//      2. Print the number of errors and warnings, if any.
//
//*****************************************************************************
{
    if ( !newlined_ )
        cerr << endl;
    if ( errors_ ) {
        cerr << errors_ << " error";
        if ( errors_ > 1 ) cerr << 's';
        cerr << endl;
    }
    if ( warnings_ ) {
        cerr << warnings_ << " warning";
        if ( warnings_ > 1 ) cerr << 's';
        cerr << endl;
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        ostream&
        compiler::source::complain_at( char const *error_type, int alt_no )
//
// DESCRIPTION
//
//      Emit an error of a certain type regarding a line the source file.
//
// PARAMETERS
//
//      error_type  The error message type.
//
//      alt_no      The line number to refer to instead of the default.
//
// RETURN VALUE
//
//      Returns the ostream used to emit to.
//
//*****************************************************************************
{
    if ( name_.length() ) cerr << '\"' << name_ << "\", ";
    return  cerr << "line " << (alt_no ? alt_no : line_no_) << ": "
            << error_type << ": ";
}

//*****************************************************************************
//
// SYNOPSIS
//
        void compiler::source::init()
//
// DESCRIPTION
//
//      Initialize a compiler::source object.
//
//*****************************************************************************
{
    line_no_ = 1;
    errors_ = warnings_ = 0;
    check_only_ = false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        compiler::target::target( char const *file_name )
//
// DESCRIPTION
//
//      Construct the target file-name object.
//
// PARAMETERS
//
//      file_name   The target file name.
//
//*****************************************************************************
    : compiler::file_base( file_name, ios::out )
{
    init();
}

//*****************************************************************************
//
// SYNOPSIS
//
        extern "C" void yyerror( char const *msg )
//
// DESCRIPTION
//
//      This is the function that YACC calls to emit an error message.
//
// PARAMETERS
//
//      msg     The error message generated by yacc.
//
//*****************************************************************************
{
    if ( !compiler::newlined_ )
        cerr << '\n';
    compiler::source_->error() << msg;
    compiler::newlined_ = false;
}

/*****************************************************************************
 *      Debugging function(s) implementation
 *****************************************************************************/

#ifdef  PJL_STACK_DEBUG
#define DUMP( label, value )                                \
    if ( base_options_.stack_debug_ ) {                     \
        compiler::target_->out() << "/***** line: ";        \
        compiler::target_->out().width( 4 );                \
        compiler::target_->out() << line << ": " << label   \
        << (void*)value << " *****/" << endl;               \
    }
#else
#define DUMP( label, value )            /* nothing */
#endif

#define STACK_PUSH( x )     stack[ ++stack_p ] = (x)
#define STACK_POP( x )      x = stack[ stack_p-- ]
#define STACK_PEEK( x, d )  x = stack[ stack_p - (d) ]

#ifdef  PJL_STACK_DEBUG
//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::dump( char const *s )
//
// DESCRIPTION
//
//      Dump a string to the output file being generated.  The string is dumped
//      inside a comment.
//
// PARAMETERS
//
//  s   The string to be dumped.
//
//*****************************************************************************
{
    if ( base_options_.stack_debug_ ) {
        compiler::target_->out() << "/***** string: " << flush;
        if ( s )
            compiler::target_->out() << s;
        else
            compiler::target_->out() << "(NULL)";
        compiler::target_->out() << " *****/" << endl;
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::dump( symbol const *s )
//
// DESCRIPTION
//
//      Dump a symbol to the output file being generated.  The symbol is dumped
//      inside a comment.
//
// PARAMETERS
//
//      s   The symbol to be dumped.
//
//*****************************************************************************
{
    if ( base_options_.stack_debug_ ) {
        compiler::target_->out() << "/***** symbol: " << flush;
        if ( s )
            compiler::target_->out() << *s;
        else
            compiler::target_->out() << "(NIL)";
        compiler::target_->out() << " *****/" << endl;
    }
}
#endif  /* PJL_STACK_DEBUG */

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::push_line( int i, int line )
//
// DESCRIPTION
//
//      Push an integer onto the semantic stack.
//
// PARAMETERS
//
//      i       The integer to be pushed.
//
//      line    The line number in the source code the PUSH was done on.
//
//*****************************************************************************
{
    DUMP( "Push: ", i );
    STACK_PUSH( i );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::pop_line ( int &i, int line )
//
// DESCRIPTION
//
//      Pop an integer from the semantic stack.
//
// PARAMETERS
//
//      i       The integer to be popped into.
//
//      line    The line number in the source code the PUSH was done on.
//
//*****************************************************************************
{
    STACK_POP( i );
    DUMP( "Pop : ", i );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::peek_line( int &i, int line, int depth )
//
// DESCRIPTION
//
//      Peek into the semantic stack for an integer.
//
// PARAMETERS
//
//      i       The integer to be peeked into.
//
//      line    The line number in the source code the PUSH was done on.
//
//      depth   How far down to peek.
//
//*****************************************************************************
{
    STACK_PEEK( i, depth );
    DUMP( "Peek: ", i );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::push_line( void const *p, int line )
//
// DESCRIPTION
//
//      Push a pointer onto the smenatic stack.
//
// PARAMETERS
//
//      p       The pointer to be pushed.
//
//      line    The line number in the source code the PUSH was done on.
//
//*****************************************************************************
{
    DUMP( "Push: ", p );
    STACK_PUSH( (void*)p );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::pop_line ( void *&p, int line )
//
// DESCRIPTION
//
//      Pop a pointer from the semantic stack.
//
// PARAMETERS
//
//      p       The pointer to be popped into.
//
//      line    The line number in the source code the PUSH was done on.
//
//*****************************************************************************
{
    STACK_POP( p );
    DUMP( "Pop : ", p );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void compiler::peek_line( void *&p, int line, int depth )
//
// DESCRIPTION
//
//      Peek into the semantic stack for a pointer.
//
// PARAMETERS
//
//      p       The pointer to be peeked into.
//
//      line    The line number in the source code the PUSH was done on.
//
//      depth   How far down to peek.
//
//*****************************************************************************
{
    STACK_PEEK( p, depth );
    DUMP( "Peek: ", p );
}

#ifdef  PJL_STACK_DEBUG
//*****************************************************************************
//
// SYNOPSIS
//
        void compiler::semantic::check( semantic::type_tag t, void *p ) const
//
// DESCRIPTION
//
//      Check that type of the value on the semantic stack is of the expected
//      type.
//
// PARAMETERS
//
//  t   The semantic tag type.
//
//  p   The pointer to the object that was on the top of the stack.
//
//*****************************************************************************
{
    if ( t != tag ) {
        compiler::fatal() << "(internal): illegal pop attempt: ";
        switch ( tag ) {
            case int_t:
                cerr << reinterpret_cast<int>( p ) << ": ptr expected";
                break;
            case ptr_t:
                cerr << p << ": int expected";
                break;
            default:
                cerr << p << ": corrupted stack";
        }
        cerr << endl;
        ::exit( compiler::Internal_Error );
    }
}
#endif  /* PJL_STACK_DEBUG */

} // namespace PJL
/* vim:set et ts=4 sw=4: */
