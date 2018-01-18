/*
**      CHSM Language System
**      src/c++/chsmc/chsm_compiler.h
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

#ifndef chsmc_chsm_compiler_H
#define chsmc_chsm_compiler_H

// local
#include "base_info.h"
#include "file.h"
#include "compiler_util.h"

class code_generator;

// standard
#include <memory>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %chsm_compiler contains what would otherwise be global data for the
 * compiler while compiling a CHSM.
 */
struct chsm_compiler {
  typedef base_info::symbol_type symbol_type;

  //
  // Special-purpose struct used to process event parameters.
  //
  struct arg_type {
    unsigned  line_no_;                 // line# in source file of decl
    bool      got_variable_;
    char      ident_[2][128];           // var name is in ident_[ 0 ]
    char      decl_[256];               // declaration/type of variable

    void init( unsigned line_no ) {
      line_no_ = line_no;
      got_variable_ = false;
      *ident_[0] = *ident_[1] = '\0';
      decl_len_ = 0;
    }

    void cat( char const *s ) {
      ::strcpy( decl_ + decl_len_, s );
      decl_len_ += ::strlen( s );
    }

    void cat( char c ) {
      decl_[ decl_len_ ] = c;
      decl_[ ++decl_len_ ] = '\0';
    }

  private:
    size_t decl_len_;
  };
  arg_type                    arg_;

  std::unique_ptr<code_generator> code_gen_;

  /**
   * The symbol for the CHSM itself.
   */
  static PJL::symbol *sy_chsm_;

  PJL::symbol_table sym_table_;

  std::unique_ptr<source_file> source_;
  std::unique_ptr<target_file> target_;
  std::unique_ptr<user_code> user_code_;

  bool error_newlined;

  std::ostream dev_null;

  chsm_compiler();
  ~chsm_compiler();

  /**
   * For plain-events, "back-patch" whether states have an enter or exit event
   * that needs to be broadcast.
   */
  void backpatch_enter_exit_events();

  /**
   * Checks that all child states declared have actually been defined.
   */
  void check_child_states_defined();

  /**
   * Checks that all declared events have been used.
   */
  void check_events_used();

  /**
   * Checks that all states referenced have actually been defined.
   */
  void check_states_defined();

  /**
   * Checks that:
   *
   *  1. The target states are defined.
   *  2. No transition goes between two children of a set.
   */
  void check_transitions();

  /**
   * If the symbol has a type and either 'types' is unknown or the type is
   * one of the specified 'types', then the symbol already exists.
   */
  bool not_exists( PJL::symbol const *sy, symbol_type types = TYPE(NONE) );
};

///////////////////////////////////////////////////////////////////////////////

//
// Singleton global instance of chsm_compiler.
//
extern chsm_compiler cc;

/**
 * Shorthand for accessing the symbol info for the CHSM itself.
 */
#define CHSM                      INFO( chsm, cc.sy_chsm_ )

/**
 * Shorthand for accessing the symbol for the root state of the CHSM.
 */
#define SY_ROOT                   (CHSM->sy_root_)

//
// Code emission stuff.
//
#define T_OUT                     cc.target_->out()
#define T_ENDL                    << (++cc.target_->line_no_, '\n')
#define U_OUT                     cc.user_code_->out()

//
// Prints respective message to stderr regarding current compilation.  Unlike
// the functions in source above, these are to be used when the message has
// nothing to do with a particular source line.
//
std::ostream& cc_error();
std::ostream& cc_fatal();
std::ostream& cc_warning();

///////////////////////////////////////////////////////////////////////////////

#endif /* chsmc_chsm_compiler_H */
/* vim:set et ts=2 sw=2: */
