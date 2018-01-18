/*
**      CHSM Language System
**      src/c++/chsmc/code_generator.h
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

#ifndef chsmc_code_generator_H
#define chsmc_code_generator_H

// local
#include "info_visitor.h"
#include "options.h"

namespace PJL {
class symbol;
}

// standard
#include <iostream>
#include <memory>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %code_generator is-an info_visitor that generates code for a CHSM.
 */
class code_generator : public info_visitor {
public:
  /**
   * Maps a filename extension to a language to generate.
   *
   * @param ext The filename extension to map.
   * @return Returns said language or `NONE` if \a ext doesn't map to a
   * language.
   */
  static lang map_ext_to_lang( std::string const &ext );

  /**
   * Creates a %code_generator for the given language.
   *
   * @param l The language to create the %code_generator for.
   * @return Returns said %code_generator.
   */
  static std::unique_ptr<code_generator> create( lang l );

  /**
   * Emits the code for a CHSM.
   */
  virtual void emit() = 0;

  /**
   * Emits a comment containing the CHSM source file line number and name.
   *
   * @param o The ostream to emit to.
   * @param alt_no If non-zero, the alternate line number to print.
   */
  virtual void emit_source_line_no( std::ostream &o,
                                    unsigned alt_no = 0 ) const;

  /**
   * Emits the action block beginning code.
   */
  virtual void emit_action_block_begin() const = 0;

  /**
   * Emits the action block ending code.
   */
  virtual void emit_action_block_end() const = 0;

  /**
   * Emits the event condition expression beginning code.
   */
  virtual void emit_condition_expr_begin() const = 0;

  /**
   * Emits the event condition expression ending code.
   */
  virtual void emit_condition_expr_end() const = 0;

  /**
   * Emits the enter/exit beginning code.
   */
  virtual void emit_enter_exit_begin( char const *kind,
                                      PJL::symbol const *sy_state ) const = 0;

  /**
   * Emits the enter/exit ending code.
   */
  virtual void emit_enter_exit_end() const = 0;

  /**
   * Emits the event precondition expression beginning code.
   *
   * @param sy_event A pointer to the symbol of the event having the
   * precondition.
   */
  virtual
  void emit_precondition_expr_begin( PJL::symbol const *sy_event ) const = 0;

  /**
   * Emits the event precondition expression ending code.
   */
  virtual void emit_precondition_expr_end() const = 0;

  /**
   * Emits the event precondition function beginning code.
   *
   * @param sy_event A pointer to the symbol of the event having the
   * precondition.
   */
  virtual
  void emit_precondition_func_begin( PJL::symbol const *sy_event ) const = 0;

  /**
   * Emits the event precondition function ending code.
   */
  virtual
  void emit_precondition_func_end( PJL::symbol const *sy_event ) const = 0;

  /**
   * Emits \a text to the target file.
   *
   * @param text The text to emit.
   */
  virtual void emit_text( char const *text ) const = 0;

  /**
   * Emits the computed transition target beginning code.
   */
  virtual void emit_transition_target_begin() const = 0;

  /**
   * Emits the computed transition target ending code.
   */
  virtual void emit_transition_target_end() const = 0;

  /**
   * Gets the generated code filename extension(s).
   *
   * @param declaration_ext A pointer to receive the declaration filename
   * extension.
   * @param definition_ext A pointer to receive the definition filename
   * extension.
   */
  virtual void get_filename_exts( std::string *declaration_ext,
                                  std::string *definition_ext ) const = 0;

  static std::ostream& section_comment( std::ostream &o );

protected:
  /**
   * Emits a comment denoting the end of CHSM-generated code.
   */
  void emit_the_end() const;
};

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_code_generator_H */
/* vim:set et sw=2 ts=2: */
