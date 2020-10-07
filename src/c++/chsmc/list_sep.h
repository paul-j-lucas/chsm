/*
**      PJL C++ Library
**      list_sep.h
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

#ifndef pjl_list_sep_H
#define pjl_list_sep_H

// standard
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %list_sep is a class that, when inserted into an `ostream`, doesn't print
 * a separator the first time around.  This is useful for emitting separated
 * lists, e.g.:
 *
 *      ( A, B, C )
 */
class list_sep {
public:
  /**
   * Constructs a %list_sep.
   *
   * @param sep The separator to use.
   */
  explicit list_sep( std::string const &sep = ", " ) :
    print_{ false },
    sep_{ sep }
  {
  }

  /**
   * Emits \a sep to \a o.
   *
   * @param o The ostream to emit to.
   * @param sep The list_sep to emit.
   * @return Returns \a o.
   */
  friend std::ostream& operator<<( std::ostream &o, list_sep &sep ) {
    if ( sep.print_ )
      o << sep.sep_;
    else
      sep.print();
    return o;
  }

  void print() {
    print_ = true;
  }

  /**
   * Resets the state of this %list_sep.
   */
  void reset() {
    print_ = false;
  }

private:
  bool print_;
  std::string sep_;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* pjl_list_sep_H */
/* vim:set et ts=2 sw=2: */
