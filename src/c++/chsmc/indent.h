/*
**      PJL C++ Library
**      indent.h
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

#ifndef PJL_indent_H
#define PJL_indent_H

// local
#include "util.h"

// standard
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * An %ostream_indent attaches indentation to an ostream so that output can be
 * indented.  In addition to the indentation amount, the character (typically
 * either a space or a tab) and scaling factor (e.g., 4 spaces per indent) can
 * be specified.
 *
 * The defaults are:
 *  + amount: 0
 *  + scale: 1
 *  + char: `\t`
 */
class ostream_indent {
public:
  typedef unsigned value_type;

  /**
   * Increments the indent amount for \a os.
   *
   * @param os The ostream to increment the indent for.
   * @param amount How much to increment by.
   */
  template<typename charT,typename Traits>
  static void inc( std::basic_ostream<charT,Traits> &os,
                   value_type amount = 1 ) {
    instance( os )->oi_amount += amount;
  }

  /**
   * Decrements the indent amount for \a os.
   *
   * @param os The ostream to decrement the indent for.
   * @param amount How much to decrement by.
   * If \a amount exceeds the current indent, the current indent is set to 0.
   */
  template<typename charT,typename Traits>
  static void dec( std::basic_ostream<charT,Traits> &os,
                   value_type amount = 1 ) {
    auto &oi_amount = instance( os )->oi_amount;
    oi_amount -= amount <= oi_amount ? amount : oi_amount;
  }

  /**
   * Gets the indent amount for \a os.
   *
   * @param os The ostream to get the indent for.
   * @return Returns the new indent amount.
   */
  template<typename charT,typename Traits>
  static value_type get_indent( std::basic_ostream<charT,Traits> &os ) {
    return instance( os )->oi_amount;
  }

  /**
   * Sets the indent amount for \a os.
   *
   * @param os The ostream to set the indent for.
   * @param amount The new indent amount.
   */
  template<typename charT,typename Traits>
  static void set_indent( std::basic_ostream<charT,Traits> &os,
                          value_type amount ) {
    instance( os )->oi_amount = amount;
  }

  /**
   * Let \e width equal the current indent amount multiplied by the current
   * scale amount; then emits \e width + \e width * (\e count - 1) indent
   * characters.
   *
   * @param os The ostream to emit the space characters to.
   * @param count The number of times to indent.
   */
  template<typename charT,typename Traits>
  static void indent( std::basic_ostream<charT,Traits> &os,
                      value_type count = 1 ) {
    assert( count > 0 );
    auto const oi = instance( os );
    auto const width = oi->oi_amount * oi->oi_scale;
    os  << std::setfill( oi->oi_char )
        << std::setw( width + width * (count - 1) )
        << "";
  }

  /**
   * Gets the indent character in use.
   *
   * @param os The ostream to set the indent character for.
   * @return Returns the character in use.
   */
  template<typename charT,typename Traits>
  static char get_char( std::basic_ostream<charT,Traits> &os ) {
    return instance( os )->oi_char;
  }

  /**
   * Sets the indent character to use.
   *
   * @param os The ostream to set the indent character for.
   * @param c The character to use.
   */
  template<typename charT,typename Traits>
  static void set_char( std::basic_ostream<charT,Traits> &os, char c ) {
    instance( os )->oi_char = c;
  }

  /**
   * Gets the scaling factor in use.
   *
   * @param os The ostream to get the scaling factor for.
   * @return Returns the scaling factor in use.
   */
  template<typename charT,typename Traits>
  static value_type get_scale( std::basic_ostream<charT,Traits> &os ) {
    return instance( os )->oi_scale;
  }

  /**
   * Sets the scaling factor to use.
   *
   * @param os The ostream to set the scaling factor for.
   * @param scale The scaling factor to use.
   */
  template<typename charT,typename Traits>
  static void set_scale( std::basic_ostream<charT,Traits> &os,
                         value_type scale ) {
    instance( os )->oi_scale = scale;
  }

private:
  value_type  oi_amount;
  value_type  oi_scale;
  char        oi_char;

  /**
   * Default constructs an %ostream_indent.
   */
  ostream_indent() : oi_amount( 0 ), oi_scale( 1 ), oi_char( '\t' ) { }

  /**
   * Callback function for register_callback() to do memory management.
   *
   * @param event The stream event.
   * @param os The stream.
   * @param index The xalloc index.
   */
  static void callback( std::ios_base::event event, std::ios_base &os,
                        int index );

  /**
   * Gets the current %ostream_indent instance for the given ostream.
   *
   * @param os The ostream to get the instance for.
   * @return Returns said instance.
   */
  static ostream_indent* instance( std::ios_base &os );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An ostream manipulator version of ostream_indent::inc_indent().
 *
 * @param os The ostream to increment the indentation amount of.
 * @return Returns the given ostream.
 */
template<typename charT,typename Traits>
inline std::basic_ostream<charT,Traits>&
inc_indent( std::basic_ostream<charT,Traits> &os ) {
  ostream_indent::inc( os );
  return os;
}

/**
 * An ostream manipulator version of ostream_indent::dec_indent().
 *
 * @param os The ostream to decrement the indentation amount of.
 * @return Returns the given ostream.
 */
template<typename charT,typename Traits>
inline std::basic_ostream<charT,Traits>&
dec_indent( std::basic_ostream<charT,Traits> &os ) {
  ostream_indent::dec( os );
  return os;
}

/**
 * An ostream manipulator version of ostream_indent::indent().
 *
 * @param os The ostream to emit the space characters to.
 * @return Returns the given ostream.
 */
template<typename charT,typename Traits> inline
std::basic_ostream<charT,Traits>&
indent( std::basic_ostream<charT,Traits> &os ) {
  ostream_indent::indent( os );
  return os;
}

/**
 * An ostream manipulator version of ostream_indent::indent().
 *
 * @param os The ostream to emit the space characters to.
 * @param count The number of indents to emit.
 * @return Returns a function that, whem inserted into an ostream, performs the
 * indent.
 */
inline std::function<std::ostream&(std::ostream&)>
indent( ostream_indent::value_type count ) {
  return [=]( std::ostream &os ) -> std::ostream& {
    ostream_indent::indent( os, count );
    return os;
  };
}

/**
 * An ostream manipulator version of ostream_indent::set_char().
 *
 * @param c The character to use.
 * @return Returns a function that, whem inserted into an ostream, sets the
 * character.
 */
inline std::function<std::ostream&(std::ostream&)>
set_char( char c ) {
  return [=]( std::ostream &os ) -> std::ostream& {
    ostream_indent::set_char( os, c );
    return os;
  };
}

/**
 * An ostream manipulator version of ostream_indent::set_indent().
 *
 * @param indent The indent amount to use.
 * @return Returns a function that, whem inserted into an ostream, sets the
 * indent.
 */
inline std::function<std::ostream&(std::ostream&)>
set_indent( ostream_indent::value_type indent ) {
  return [=]( std::ostream &os ) -> std::ostream& {
    ostream_indent::set_indent( os, indent );
    return os;
  };
}

/**
 * An ostream manipulator version of ostream_indent::set_scale().
 *
 * @param scale The scaling factor to use.
 * @return Returns a function that, whem inserted into an ostream, sets the
 * scale.
 */
inline std::function<std::ostream&(std::ostream&)>
set_scale( ostream_indent::value_type scale ) {
  return [=]( std::ostream &os ) -> std::ostream& {
    ostream_indent::set_scale( os, scale );
    return os;
  };
}

///////////////////////////////////////////////////////////////////////////////

} // namespace

#endif /* PJL_indent_H */
/* vim:set et sw=2 ts=2: */
