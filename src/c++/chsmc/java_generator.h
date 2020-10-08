/*
**      CHSM Language System
**      src/c++/chsmc/java_generator.h
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

#ifndef chsmc_java_generator_H
#define chsmc_java_generator_H

// local
#include "code_generator.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %java_generator is-a code_generator that generates Java code for a CHSM.
 */
class java_generator : public code_generator {
public:
  /**
   * Creates a new %java_generator.
   *
   * @return Returns said %java_generator.
   */
  static std::unique_ptr<code_generator> create();

private:
  void emit_action_block_begin() const final;
  void emit_action_block_end() const final;
  void emit_condition_expr_begin() const final;
  void emit_condition_expr_end() const final;
  void emit_enter_exit_begin( char const*, PJL::symbol const* ) const final;
  void emit_enter_exit_end() const final;
  void emit_precondition_expr_begin( PJL::symbol const* ) const final;
  void emit_precondition_expr_end() const final;
  void emit_precondition_func_begin( PJL::symbol const* ) const final;
  void emit_precondition_func_end( PJL::symbol const* ) const final;
  void emit_text( char const *text ) const final;
  void emit_transition_target_begin() const final;
  void emit_transition_target_end() const final;

  std::pair<std::filesystem::path,std::filesystem::path>
  get_filename_exts() const final;
};

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_java_generator_H */
/* vim:set et sw=2 ts=2: */
