/*
**      CHSM Language System
**      src/c++/chsmc/semantic.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas
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

#ifndef chsmc_semantic_H
#define chsmc_semantic_H

// local
#include "config.h"                     /* must go first */
#include "util.h"

// standard
#include <cstdlib>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////

class semantic {
public:
  semantic( int x ) {
    i_ = x;
    set_tag( int_t );
  }

  semantic( void *x = nullptr ) {
    p_ = x;
    set_tag( ptr_t );
  }

  semantic& operator=( int x ) {
    i_ = x;
    set_tag( int_t );
    return *this;
  }

  semantic& operator=( void* x ) {
    p_ = x;
    set_tag( ptr_t );
    return *this;
  }

  operator int() const {
    check_tag( int_t, reinterpret_cast<void const*>( i_ ) );
    return i_;
  }

  operator void*() const {
    check_tag( ptr_t, p_ );
    return p_;
  }

private:
  union {
    int   i_;
    void *p_;
  };

  enum type_tag {
    int_t,
    ptr_t
  };

  /**
   * Checks that the type of the value on the semantic stack is of the expected
   * type.
   *
   * @param t The semantic tag type.
   * @param p The pointer to the object that was on the top of the stack.
   */
  void check_tag( type_tag t, void const *p ) const
#ifdef ENABLE_STACK_DEBUG
  ;

  type_tag tag_;
#else
  {
  }
#endif /* ENABLE_STACK_DEBUG */

  void set_tag( type_tag t ) {
#ifdef ENABLE_STACK_DEBUG
    tag_ = t;
#endif /* ENABLE_STACK_DEBUG */
  }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Pushes an integer onto the semantic stack.
 *
 * @param i The integer to be pushed.
 * @param line The line number in the source code the PUSH was done on.
 */
void push_line( int i, unsigned line );

/**
 * Pops an integer from the semantic stack.
 *
 * @param i The integer to be popped into.
 * @param line The line number in the source code the PUSH was done on.
 */
void pop_line( int &i, unsigned line );

/**
 * Peeks into the semantic stack for an integer.
 *
 * @param i The integer to be peeked into.
 * @param line The line number in the source code the PUSH was done on.
 * @param depth How far down to peek.
 */
void peek_line( int &i, unsigned line, unsigned depth );

/**
 * Pushes a pointer onto the smenatic stack.
 *
 * @param p The pointer to be pushed.
 * @param line The line number in the source code the PUSH was done on.
 */
void push_line( void const *p, unsigned line );

/**
 * Pops a pointer from the semantic stack.
 *
 * @param p The pointer to be popped into.
 * @param line The line number in the source code the PUSH was done on.
 */
void pop_line( void *&p, unsigned line );

/**
 * Peeks into the semantic stack for a pointer.
 *
 * @param p The pointer to be peeked into.
 * @param line The line number in the source code the PUSH was done on.
 * @param depth How far down to peek.
 */
void peek_line( void *&p, unsigned line, unsigned depth );

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_semantic_H */
/* vim:set et ts=2 sw=2: */
