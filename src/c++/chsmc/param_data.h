/*
**      CHSM Language System
**      src/c++/chsmc/param_data.h
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

#ifndef chsmc_param_data_H
#define chsmc_param_data_H

class list_sep;                         /* in list_sep.h */

// standard
#include <functional>
#include <iostream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %param_data is-a base class for holding parameter-list information.
 */
struct param_data {
  /*
   * Gives parameter formal argument names a different name from the data
   * members.  For example:
   *
   *      param_block( T Pparam ) : param( Pparam ) { }
   *                     ^                 ^
   */
  static char const PARAM_PREFIX_[];

  /**
   * An individual parameter.
   * Given a declaration such as:
   *
   *      T (*const foo)( U )
   *
   * we store it as:
   *
   *      declaration: T (*const $)( U )
   *      variable   : foo
   *
   * See the comments for the `arg_cat` and `arg_cat_ident` productions in
   * `parser.ypp`.
   */
  struct param {
    unsigned const    line_no_;         ///< Line number of declaration.
    std::string const declaration_;     ///< Parameter declaration.
    std::string const variable_;        ///< Parameter name.

    param( unsigned line_no, char const *declaration, char const *variable ) :
      line_no_( line_no ), declaration_( declaration ), variable_( variable )
    {
    }
  };

  typedef std::vector<param> param_list_type;
  param_list_type param_list_;

  /** Bitmask for emit flags. */
  typedef unsigned emit_mask;

  //
  // Flags used to control how parameter lists are emitted.
  //
  static emit_mask const EMIT_COMMA  = 0x01;
  static emit_mask const EMIT_PREFIX = 0x02;
  static emit_mask const EMIT_FORMAL = 0x04;
  static emit_mask const EMIT_ACTUAL = 0x08;

  static emit_mask default_emit_flags_;

  /**
   * Creates a function that, when inserted into an ostream, emits the
   * parameter list for \a p.
   *
   * @param p The param_data to emit.
   * @param emit_flags The emit flags to use.
   * @return Returns said function.
   */
  friend std::function<std::ostream&(std::ostream&)>
  param_list( param_data const &p, emit_mask emit_flags = 0 ) {
    return [=,&p]( std::ostream &o ) -> std::ostream& {
      return p.emit_param_list( o, emit_flags );
    };
  }

  /**
   * Creates a function that, when inserted into an ostream, emits the
   * parameter list for \a *p.
   *
   * @param p A pointer to the param_data to emit.
   * @param emit_flags The emit flags to use.
   * @return Returns said function.
   */
  friend std::function<std::ostream&(std::ostream&)>
  param_list( param_data const *p, emit_mask emit_flags = 0 ) {
    return [=]( std::ostream &o ) -> std::ostream& {
      return p->emit_param_list( o, emit_flags );
    };
  }

  /**
   * Takes a declaration string like:
   *
   *      void (*const $)( int )
   *
   * and stuffs the strings \a s1 followed by \a s2 where the `$` is, the
   * position of the variable name.
   *
   * @param decl A declaration string with an embedded `$`.
   * @param s1 The first string to be stuffed where the `$`.
   * @param s2 The second string to be stuffed immediately after \a s1.
   * @return Returns a pointer to the "stuffed" declaration.  Although it is in
   * a `static` buffer, it is emitted immediately, so there is no danger of
   * overwriting the value before we're finished with it.
   */
  static char const* stuff_decl( char const *decl, char const *s1,
                                 char const *s2 );

  static char const* stuff_decl( char const *decl, char const *s1,
                                 std::string const &s2 ) {
    return stuff_decl( decl, s1, s2.c_str() );
  }

  static char const* stuff_decl( char const *decl, std::string const &s1,
                                 char const *s2 ) {
    return stuff_decl( decl, s1.c_str(), s2 );
  }

  static char const* stuff_decl( char const *decl, std::string const &s1,
                                 std::string const &s2 ) {
    return stuff_decl( decl, s1.c_str(), s2.c_str() );
  }

  static char const* stuff_decl( std::string const &decl, char const *s1,
                                 std::string const &s2 ) {
    return stuff_decl( decl.c_str(), s1, s2.c_str() );
  }

  static char const* stuff_decl( std::string const &decl,
                                 std::string const &s1, char const *s2 ) {
    return stuff_decl( decl.c_str(), s1.c_str(), s2 );
  }

  static char const* stuff_decl( std::string const &decl,
                                 std::string const &s1,
                                 std::string const &s2 ) {
    return stuff_decl( decl.c_str(), s1.c_str(), s2.c_str() );
  }

protected:
  param_data() { }
  virtual ~param_data();

  /**
   * We need to use a single list_sep object across all the recursion; hence,
   * we declare one here and call a second function to do the recursion passing
   * a reference to the comma object along.
   *
   * @param o The ostream to emit the parameter list to.
   * @param emit_flags Flags to control how things are emitted.
   * @return Returns \a o.
   */
  std::ostream& emit_param_list( std::ostream &o,
                                 emit_mask emit_flags = 0 ) const;

  /**
   * Emits a parameter list in a form dictated by various flags.
   *
   * @param o The ostream to emit the parameter list to.
   * @param sep The list_sep whose state should be preserved across calls.
   * @param emit_flags Flags to control how things are emitted.
   * @return Returns \a o.
   */
  virtual std::ostream& emit_param_list_aux( std::ostream &o, list_sep &sep,
                                             emit_mask emit_flags = 0 ) const;
};

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_param_data_H */
/* vim:set et ts=2 sw=2: */
