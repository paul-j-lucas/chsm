/*
**      CHSM Language System
**      src/c++/chsmc/util.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License, or
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

#ifndef chsmc_util_H
#define chsmc_util_H

/**
 * @file
 * Declares utility constants, macros, and functions.
 */

// local
#include "config.h"                     /* must go first */

/// @cond DOXYGEN_IGNORE

// standard
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>
#include <sysexits.h>

/// @endcond

///////////////////////////////////////////////////////////////////////////////

/** Gets the number of elements of the given array. */
#define ARRAY_SIZE(A)             (sizeof(A) / sizeof(0[A]))

/** Always assert and include a message. */
#define ASSERT_0_MSG(MSG)         assert( 0 && "" MSG )

/** Embeds the given statements into a compount statement block. */
#define BLOCK(...)                do { __VA_ARGS__ } while (0)

/** No-operation statement.  (Useful for a `goto` target.) */
#define NO_OP                     ((void)0)

/** Shorthand for calling **strerror**(3). */
#define STRERROR                  strerror( errno )

/**
 * Defines std::swap(CLASS&,CLASS&).
 *
 * @param CLASS The class to define swap for.
 * @hideinitializer
 */
#define DEFINE_STD_SWAP_FUNCTION(CLASS)       \
  namespace std {                             \
    inline void swap( CLASS &a, CLASS &b ) {  \
      a.swap( b );                            \
    }                                         \
    inline void swap( CLASS &&a, CLASS &b ) { \
      b.swap( a );                            \
    }                                         \
    inline void swap( CLASS &a, CLASS &&b ) { \
      a.swap( b );                            \
    }                                         \
  }                                           \
  typedef int fake_type_to_eat_semicolon

/**
 * Prints an error message and exits in response to an internal error.
 *
 * @hideinitializer
 */
#define INTERNAL_ERR(...) \
  PMESSAGE_EXIT( EX_SOFTWARE, "internal error: " << __VA_ARGS__ )

/**
 * Prints an error message to standard output and exits with \a STATUS code.
 *
 * @param STATUS The status code to **exit**(3) with.
 * @hideinitializer
 */
#define PMESSAGE_EXIT(STATUS,...) \
  BLOCK( std::cerr << me <<  ": " << __VA_ARGS__; exit( STATUS ); )

///////////////////////////////////////////////////////////////////////////////

inline std::filesystem::path operator+( std::filesystem::path const &p1,
                                        std::filesystem::path const &p2 ) {
  std::filesystem::path rv{ p1 };
  rv += p2;
  return rv;
}

inline std::filesystem::path operator+( std::filesystem::path const &p,
                                        std::string const &s ) {
  std::filesystem::path rv{ p };
  rv += s;
  return rv;
}

inline std::filesystem::path operator+( std::filesystem::path const &p,
                                        char c ) {
  std::filesystem::path rv{ p };
  rv += c;
  return rv;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__

/**
 * Specifies that \a EXPR is \e very likely (as in 99.99% of the time) to be
 * non-zero (true) allowing the compiler to better order code blocks for
 * magrinally better performance.
 *
 * @param EXPR An expression that can be cast to `bool`.
 * @see http://lwn.net/Articles/255364/
 * @hideinitializer
 */
#define likely(EXPR)              __builtin_expect( !!(EXPR), 1 )

/**
 * Specifies that \a EXPR is \e very unlikely (as in .01% of the time) to be
 * non-zero (true) allowing the compiler to better order code blocks for
 * magrinally better performance.
 *
 * @param EXPR An expression that can be cast to `bool`.
 * @see http://lwn.net/Articles/255364/
 * @hideinitializer
 */
#define unlikely(EXPR)            __builtin_expect( !!(EXPR), 0 )

#else
# define likely(EXPR)             (EXPR)
# define unlikely(EXPR)           (EXPR)
#endif /* __GNUC__ */

///////////////////////////////////////////////////////////////////////////////

/**
 * Begins a "type switch" to perform a `switch` based on the runtime type of a
 * pointer.  For example:
 * @code
 *    TYPE_SWITCH( p ) {
 *      TYPE_CASE( t1, T1 ) {
 *        // t1 is set to p iff *p is of type T1
 *        break;
 *      }
 *      TYPE_CASE( t2, T2 ) {
 *        // t2 is set to p iff *p is of type T2
 *        break;
 *      }
 *      // ...
 *      TYPE_DEFAULT {                  // optional
 *        // p is not of any listed type
 *      }
 *    }
 * @endcode
 * @note To prevent fallthrough between cases, `break` is needed (just like a
 * real `switch` statement).
 *
 * @param EXPR An expression that must result in a pointer to a user-defined
 * type that has at least one `virtual` member function.
 * @hideinitializer
 */
#define TYPE_SWITCH(EXPR) \
  if ( auto const type_switch_ptr = (EXPR) ) switch (0) case 0:

/**
 * Begins a "type case" to perform an individual case for a "type switch."
 *
 * @param VAR A variable cast to a pointer to the particular type.
 * @param ... The type for this case that must be a user-defined derived type.
 * @see TYPE_SWITCH(EXPR)
 * @hideinitializer
 */
#define TYPE_CASE(VAR,...) \
  if ( auto const VAR = dynamic_cast<__VA_ARGS__*>(type_switch_ptr) )

/**
 * Begins a "type default" to perform a default case for a "type switch."
 *
 * @see TYPE_SWITCH(EXPR)
 * @hideinitializer
 */
#define TYPE_DEFAULT              /* nothing */

////////// extern functions ///////////////////////////////////////////////////

namespace PJL {

typedef std::function<std::ostream&(std::ostream&)> ostream_manip;

/**
 * Inserts a std::function (lambda) into an ostream.
 *
 * @tparam charT The ostream's character type.
 * @tparam Traits The ostream's traits type.
 * @param o The ostream to insert into.
 * @param f The function to insert.
 * @return Returns \a o.
 */
template<typename charT,typename Traits>
inline std::basic_ostream<charT,Traits>&
operator<<( std::basic_ostream<charT,Traits> &o,
            std::function<
              std::basic_ostream<charT,Traits>&
              (std::basic_ostream<charT,Traits>&)
            > const &f ) {
  return f( o );
}

/**
 * Extracts the base portion of a path-name.
 * Unlike **basename**(3):
 *  + Trailing `/` characters are not deleted.
 *  + \a path is never modified (hence can therefore be `const`).
 *  + Returns a pointer within \a path (hence is multi-call safe).
 *
 * @param path The path-name to extract the base portion of.
 * @return Returns a pointer to the last component of \a path.
 * If \a path consists entirely of `/` characters, a pointer to the string `/`
 * is returned.
 */
char const* base_name( char const *path );

/**
 * Clears the given stack.
 *
 * @tparam T The stack's element type.
 * @param s The stack to clear.
 */
template<typename T>
inline void clear( std::stack<T> *s ) {
  while ( !s->empty() )
    s->pop();
}

/**
 * Checks whether \a s ends with \a suffix.
 *
 * @param s The string to check.
 * @param suffix The suffix to check for.
 * @return Returns `true` only if \a s ends with \a suffix.
 */
bool ends_with( std::string const &s, std::string const &suffix );

/**
 * Converts a string into a valid C identifier by:
 *
 *  1. Replacing all characters not in `[A-Za-z0-9_]` with `_`.
 *  2. Two or more characters converted to `_` are collapsed into a single `_`.
 *
 * @param s The string to convert.
 * @return Returns a valid C identifier.
 */
std::string identify( std::string const &s );

/**
 * Converts a `long` to a string, the opposite of atol().
 *
 * @note The string returned is from an internal pool of string buffers.  The
 * time you get into trouble is if you hang on to more than NUM_BUFS strings.
 * This doesn't normally happen in practice, however.
 *
 * @param n The `long` to convert.
 * @return Returns a pointer to a C string.
 */
char const* ltoa( long n );

/**
 * Converts an `int` to a string, the opposite of atoi().
 *
 * @param n The `int` to convert.
 * @return Returns a pointer to a C string.
 */
inline char const* itoa( int n ) {
  return ltoa( n );
}

/**
 * Given a "closing" character, gets it matching "opening" chcaracter.  The
 * opening characters are: `(<[{`.
 *
 * @param c The "closing" character.
 * @return Returns said "opening" character or the null byte if none.
 */
char opening_char( char c );

/**
 * Prints an error message for `errno` to standard error and exits.
 *
 * @param status The exit status code.
 */
void perror_exit( int status );

/**
 * Gets the full path to a unique temporary file.
 *
 * @param pattern A file name that \e must end with \e exactly six `X`s, e.g.,
 * `temp.XXXXXX`.
 * @return Returns said full path.
 */
std::filesystem::path temp_path( char const *pattern );

/**
 * Converts a string to lower case.
 *
 * @param s The string to convert.
 * @return Returns \a s converted to lower case.
 */
inline std::string tolower( char const *s ) {
  std::string result;
  std::transform(
    s, s + std::strlen( s ), std::back_inserter( result ),
    []( unsigned char c ) { return std::tolower( c ); }
  );
  return result;
}

/**
 * Converts a string to lower case.
 *
 * @param s The string to convert.
 * @return Returns \a s converted to lower case.
 */
inline std::string tolower( std::string const &s ) {
  std::string result;
  std::transform(
    s.begin(), s.end(), result.begin(),
    []( unsigned char c ) { return std::tolower( c ); }
  );
  return result;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_util_H */
/* vim:set et sw=2 ts=2: */
