/*
**      PJL C++ Library
**      clone_ptr.h
**
**      Copyright (C) 2002-2018  Paul J. Lucas
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

#ifndef pjl_pointer_H
#define pjl_pointer_H

// standard
#include <utility>                      /* for swap */

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * A %clone_ptr is-a "smart pointer" where:
 *
 *  + Copying clones the object pointed to.
 *  + Assignment deletes the old object pointed to.
 *  + Deletion deletes the object pointed to.
 *
 * A %clone_ptr is useful in the case where it points to an instance of some
 * class C or some instance of a class D derived from C.
 *
 * @tparam T The class that must have a `clone()` member function.
 */
template<class T>
class clone_ptr {
public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  clone_ptr( pointer p = pointer() ) : p_( p ) {
  }

  clone_ptr( clone_ptr const &r ) :
    p_( r.p_ != nullptr ? r.p_->clone() : nullptr )
  {
  }

  clone_ptr( clone_ptr &&r ) :
    p_( r.p_ )
  {
    r.p_ = nullptr;
  }

  ~clone_ptr() {
    delete this->p_;
  }

  clone_ptr& operator=( clone_ptr const &r ) {
    if ( &r != this )
      set_( r.p_ != nullptr ? r.p_->clone() : nullptr );
    return *this;
  }

  clone_ptr& operator=( clone_ptr &&r ) {
    set_( r.p_ );
    r.p_ = nullptr;
    return *this;
  }

  clone_ptr& operator=( pointer p ) {
    if ( p != this->p_ )
      set_( p );
    return *this;
  }

  pointer get() const {
    return p_;
  }

  reference operator*() const {
    return *p_;
  }

  pointer operator->() const {
    return p_;
  }

  explicit operator bool() const {
    return p_ != nullptr;
  }

  void reset( pointer p = pointer() ) {
    if ( p != this->p_ )
      set_( p );
  }

  void swap( clone_ptr &r ) {
    std::swap( this->p_, r.p_ );
  }

protected:
  void set_( pointer p ) {
    delete this->p_;
    this->p_ = p;
  }

  pointer p_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

///////// inlines /////////////////////////////////////////////////////////////

namespace std {

template<class T>
inline void swap( PJL::clone_ptr<T> &p, PJL::clone_ptr<T> &q ) {
  p.swap( q );
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

#endif  /* pjl_pointer_H */
/* vim:set et ts=2 sw=2: */
