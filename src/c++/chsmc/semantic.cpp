/*
**      CHSM Language System
**      src/c++/chsmc/semantic.cpp
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
#include "config.h"                     /* must go first */
#include "chsm_compiler.h"
#include "options.h"
#include "semantic.h"
#include "util.h"

// standard
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace PJL;
using namespace std;

static semantic sem_stack[ 100 ];
static int      sem_stack_p = -1;

///////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_STACK_DEBUG

#define DUMP(LABEL,VALUE) BLOCK(                      \
  if ( opt_stack_debug ) {                            \
    cc.target_->out() << "///// line: ";              \
    cc.target_->out().width( 4 );                     \
    cc.target_->out() << line << ": " << (LABEL)      \
    << reinterpret_cast<void const*>(VALUE) << endl;  \
  } )

void semantic::check_tag( semantic::type_tag t, void const *p ) const {
  if ( unlikely( t != tag_ ) ) {
    cc_fatal() << "(internal): illegal pop attempt: ";
    switch ( tag_ ) {
      case int_t:
        cerr << reinterpret_cast<uintptr_t>( p ) << ": ptr expected";
        break;
      case ptr_t:
        cerr << p << ": int expected";
        break;
      default:
        cerr << p << ": corrupted stack";
    } // switch
    cerr << endl;
    ::exit( EX_SOFTWARE );
  }
}

#else /* !ENABLE_STACK_DEBUG */

#define DUMP(LABEL,VALUE)               /* nothing */

#endif /* ENABLE_STACK_DEBUG */

template<typename T>
void stack_push( T v ) {
  sem_stack[ ++sem_stack_p ] = v;
}

template<typename T>
void stack_pop( T *v ) {
  assert( sem_stack_p >= 0 );
  *v = sem_stack[ sem_stack_p-- ];
}

template<typename T>
void stack_peek( T *v, unsigned depth ) {
  assert( static_cast<decltype(sem_stack_p)>( depth ) <= sem_stack_p );
  *v = sem_stack[ sem_stack_p - depth ];
}

void push_line( int i, unsigned line ) {
  DUMP( "Push: ", i );
  stack_push( i );
}

void pop_line( int &i, unsigned line ) {
  stack_pop( &i );
  DUMP( "Pop : ", i );
}

void peek_line( int &i, unsigned line, unsigned depth ) {
  stack_peek( &i, depth );
  DUMP( "Peek: ", i );
}

void push_line( void const *p, unsigned line ) {
  DUMP( "Push: ", p );
  stack_push( const_cast<void*>( p ) );
}

void pop_line( void *&p, unsigned line ) {
  stack_pop( &p );
  DUMP( "Pop : ", p );
}

void peek_line( void *&p, unsigned line, unsigned depth ) {
  stack_peek( &p, depth );
  DUMP( "Peek: ", p );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
