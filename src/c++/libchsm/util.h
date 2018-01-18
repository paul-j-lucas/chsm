/*
**      CHSM Language System
**      src/c++/libchsm/util.h
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

#ifndef chsm_util_H
#define chsm_util_H

///////////////////////////////////////////////////////////////////////////////

#define MORE std::cerr
#define ENDL << std::endl

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

#endif  /* chsm_util_H */
/* vim:set et sw=2 ts=2: */
