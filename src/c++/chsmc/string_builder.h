/*
**      PJL C++ Library
**      string_builder.h
**
**      Copyright (C) 2018  Paul J. Lucas
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
#ifndef PJL_string_builder_H
#define PJL_string_builder_H

// standard
#include <sstream>
#include <string>

namespace PJL {

////////// String building /////////////////////////////////////////////////////

/**
 * A %string_builder is used to build (concatenate) strings on-the-fly and pass
 * the resultant string to some function.  See the BUILD_STRING macro for
 * usage.  Note that you can use either `<<` or `,` to taste.
 *
 * See http://stackoverflow.com/q/5666678 as to why a class like this is needed
 * rather than just using an ostringstream directly.
 */
class string_builder {
public:

  /**
   * Clears then sets the string being built to the string representation of
   * the given object.
   *
   * @tparam T The object type.
   * @param t The object.
   * @return Returns \c *this.
   */
  template<typename T>
  string_builder& operator=( T const &t ) {
    oss_.str( "" );
    oss_ << t;
    return *this;
  }

  /**
   * Appends the string representation of the given object to the string being
   * built.
   *
   * @tparam T The object type.
   * @param t The object.
   * @return Returns \c *this.
   */
  template<typename T>
  string_builder& operator<<( T const &t ) {
    oss_ << t;
    return *this;
  }

  /**
   * Appends the string representation of the given object to the string being
   * built.
   *
   * @tparam T The object type.
   * @param t The object.
   * @return Returns \c *this.
   */
  template<typename T>
  string_builder& operator,( T const &t ) {
    oss_ << t;
    return *this;
  }

  /**
   * Explicit conversion to std::string.
   *
   * @return Returns a string comprising all the objects' string
   * representations concatenated together.
   */
  std::string str() const {
    return oss_.str();
  }

  /**
   * Implicit conversion to std::string.
   *
   * @return Returns a string comprising all the objects' string
   * representations concatenated together.
   */
  operator std::string() const {
    return str();
  }

private:
  std::ostringstream oss_;
};

/**
 * Emits a string_builder's string to the given ostream.
 *
 * @param o The ostream to emit to.
 * @param sb The string_builder to emit the string of.
 * @return Returns \a o.
 */
inline std::ostream& operator<<( std::ostream &o, string_builder const &sb ) {
  return o << sb.str();
}

/**
 * A convenience macro for using string_builder to de-uglify using it.
 * Using this macro, you can go from:
 * \code
 * f( string_builder() = "The answer is: ", answer )
 * \endcode
 * to the more function-like syntax of:
 * \code
 * f( BUILD_STRING( "The answer is: ", answer ) )
 * \endcode
 * \hideinitializer
 */
#define BUILD_STRING(...)         (::PJL::string_builder() << __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////

} // namespace

#endif /* PJL_string_builder_H */
/* vim:set et sw=2 ts=2: */
