/*
**      CHSM Language System
**      src/c++/chsmc/cpp_generator.cpp
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

// local
#include "config.h"                     /* must go first */
#include "base_info.h"
#include "chsm_info.h"
#include "chsm_compiler.h"
#include "cluster_info.h"
#include "cpp_generator.h"
#include "event_info.h"
#include "indent.h"
#include "mangle.h"
#include "options.h"
#include "param_data.h"
#include "set_info.h"
#include "transition_info.h"
#include "user_event_info.h"
#include "util.h"

// standard
#include <functional>

using namespace std;
using namespace PJL;

///////////////////////////////////////////////////////////////////////////////

namespace {

/**
 * A %cpp_declarer is-a cpp_generator that generates the declarations portion
 * (the `.h` file) of C++ code.
 */
class cpp_declarer : public cpp_generator {
private:
  void emit_actions( PJL::symbol const *sy_state );
  void emit_common( state_info const &si );
  void emit_common( event_info const &si );

  void emit_common( parent_info const &si,
                    char const *formal_args = nullptr );

  void emit() override;
  void visit( chsm_info const& ) override;
  void visit( child_info const& ) override;
  void visit( cluster_info const& ) override;
  void visit( event_info const& ) override;
  void visit( global_info const& ) override;
  void visit( set_info const& ) override;
  void visit( state_info const& ) override;
  void visit( transition_info const& ) override;
  void visit( user_event_info const& ) override;
};

/**
 * A %cpp_initializer is-a cpp_generator that generates the initialization
 * portion (the calls to constructors of data members) of C++ code.
 */
class cpp_initializer : public cpp_generator {
public:
  /**
   * Constructs a %cpp_initializer.
   */
  cpp_initializer();

  bool emitting_constructor_;

private:
  void emit_common( event_info const &si );
  void emit_common( state_info const &si );

  void emit() override;
  void visit( chsm_info const& ) override;
  void visit( child_info const& ) override;
  void visit( cluster_info const& ) override;
  void visit( event_info const& ) override;
  void visit( global_info const& ) override;
  void visit( set_info const& ) override;
  void visit( state_info const& ) override;
  void visit( transition_info const& ) override;
  void visit( user_event_info const& ) override;
};

/**
 * A %cpp_definer is-a cpp_generator that generates the definitions portion
 * (the `.cpp` file) of C++ code.
 */
class cpp_definer : public cpp_generator {
public:
  /**
   * Constructs a %cpp_definer.
   */
  cpp_definer();

  cpp_initializer initializer_;

private:
  void emit_chsm();

  void emit_common( event_info const &si );

  void emit_common( parent_info const &si,
                    char const *formal_args = nullptr,
                    char const *actual_args = nullptr );

  void emit_events();
  void emit_states();
  void emit_transitions();

  void emit() override;
  void visit( chsm_info const& ) override;
  void visit( child_info const& ) override;
  void visit( cluster_info const& ) override;
  void visit( event_info const& ) override;
  void visit( global_info const& ) override;
  void visit( set_info const& ) override;
  void visit( state_info const& ) override;
  void visit( transition_info const& ) override;
  void visit( user_event_info const& ) override;
};

} // namespace

static char const CHSM_NS_ALIAS[]       = "CHSM_ns_alias";
static char const EVENT_CLASS_SUFFIX[]  = "_event";
static char const PARENT_CLASS_PREFIX[] = "state_";
static char const TRANSITIONS_SUFFIX[]  = "_transitions";

///////////////////////////////////////////////////////////////////////////////

/**
 * Inserts the CHSM library class name for the given info.
 *
 * @param bi The info class to insert the library name for.
 * @return Returns an ostream manipulator that, when inserted into an ostream,
 * inserts the CHSM library class name.
 */
static ostream_manip lib_class_name( base_info const &bi ) {
  TYPE_SWITCH( &bi ) {
    TYPE_CASE( ci, cluster_info const ) {
      return [ci]( ostream &o ) -> ostream& {
        if ( !ci->derived_.empty() )
          return o << ci->derived_;
        else
          return o << CHSM_NS_ALIAS << "::cluster";
      };
    }
    TYPE_CASE( ei, event_info const ) {
      return []( ostream &o ) -> ostream& {
        return o << CHSM_NS_ALIAS << "::event";
      };
    }
    TYPE_CASE( si, set_info const ) {
      return [si]( ostream &o ) -> ostream& {
        if ( !si->derived_.empty() )
          return o << si->derived_;
        else
          return o << CHSM_NS_ALIAS << "::set";
      };
    }
    TYPE_CASE( si, state_info const ) { // must be after cluster & set
      return [si]( ostream &o ) -> ostream& {
        if ( !si->derived_.empty() )
          return o << si->derived_;
        else
          return o << CHSM_NS_ALIAS << "::state";
      };
    }
    TYPE_DEFAULT {
      INTERNAL_ERR( '"' << typeid( bi ).name() << "\": unexpected type\n" );
    }
  }
  return nullptr;
}

static ostream_manip base_class_name( user_event_info const &si ) {
  return [&si]( ostream &o ) -> ostream& {
    if ( si.sy_base_event_ != nullptr )
      o << cc.sy_chsm_->name() << "::"
        << si.sy_base_event_->name() << EVENT_CLASS_SUFFIX;
    else
      o << lib_class_name( si );
    return o;
  };
}

static ostream_manip class_name( user_event_info const &si ) {
  return [&si]( ostream &o ) -> ostream& {
    return o << si.get_symbol()->name() << EVENT_CLASS_SUFFIX;
  };
}

///////////////////////////////////////////////////////////////////////////////

unique_ptr<code_generator> cpp_generator::create() {
  return unique_ptr<code_generator>( new cpp_declarer );
}

void cpp_generator::emit_action_block_begin() const {
  U_OUT << "void " << cc.sy_chsm_->name() << "::"
        << chsm_info::PREFIX_ACTION << CHSM->id_.action_
        << "( " << CHSM_NS_ALIAS << "::event const &event ) {\n"
        << indent << "(void)event;\n";
  emit_source_line_no( U_OUT );
}

void cpp_generator::emit_action_block_end() const {
  U_OUT << "}\n";
}

void cpp_generator::emit_condition_expr_begin() const {
  U_OUT << "bool " << cc.sy_chsm_->name() << "::"
        << chsm_info::PREFIX_CONDITION << CHSM->id_.condition_
        << "( " << CHSM_NS_ALIAS << "::event const &event ) {\n"
        << indent << "(void)event;\n";
  emit_source_line_no( U_OUT );
  U_OUT << indent << "return ";
}

void cpp_generator::emit_condition_expr_end() const {
  U_OUT << "; }\n";
}

void cpp_generator::emit_enter_exit_begin( char const *kind,
                                           symbol const *sy_state ) const {
  state_info const &info = *INFO( state, sy_state );

  U_OUT << "void " << cc.sy_chsm_->name() << "::" << kind
        << chsm_info::PREFIX_ACTION << mangle( sy_state->name() )
        << "( " << CHSM_NS_ALIAS << "::state const &chsm_state_, "
        << CHSM_NS_ALIAS << "::event const &event ) {\n"
        << indent << "(void)event;\n";  // suppresses unused warning
  emit_source_line_no( U_OUT );
  U_OUT << indent << lib_class_name( info )
        << " const &state = dynamic_cast<" << lib_class_name( info )
        << " const&>(chsm_state_);\n"
        << indent << "(void)state;\n";  // suppresses unused warning
}

void cpp_generator::emit_enter_exit_end() const {
  U_OUT << "}\n";
}

void cpp_generator::
emit_precondition_expr_begin( symbol const *sy_event ) const {
  //
  // The C++ function generated for a precondition expression is the && of the
  // base event's precondition function and the C++ expression the user
  // specified.
  //
  // Note: The user's expression must be wrapped in ()'s so the resulting C++
  // expression has the proper precedence and is evaluated correctly.
  //
  U_OUT << "bool " << cc.sy_chsm_->name() << "::"
        << sy_event->name() << EVENT_CLASS_SUFFIX
        << "::param_block::precondition() const {\n"
        << indent << "return base_param_block::precondition() && (\n";
  emit_source_line_no( U_OUT );
}

void cpp_generator::emit_precondition_expr_end() const {
  U_OUT << ");\n"
        << "}\n";
}

void cpp_generator::
emit_precondition_func_begin( symbol const *sy_event ) const {
  //
  // For a precondition function, the user's code is placed into a function
  // unto itself.
  //
  U_OUT << "bool " << cc.sy_chsm_->name() << "::"
        << sy_event->name() << "_precondition( "
        << param_list( INFO( user_event, sy_event ), param_data::EMIT_FORMAL )
        << " ) const {\n";
  emit_source_line_no( U_OUT );
}

void cpp_generator::emit_precondition_func_end( symbol const *sy_event ) const {
  //
  // The C++ function generated for a precondition function is the && of the
  // base event's precondition function and the user's C++ function.
  //
  U_OUT << "}\n"
            "bool " << cc.sy_chsm_->name() << "::"
        << sy_event->name() << EVENT_CLASS_SUFFIX
        << "::param_block::precondition() const {\n"
        << indent << "return base_param_block::precondition() &&\n"
        << indent(2) << "((" << cc.sy_chsm_->name()
        << "&)chsm())." << sy_event->name() << "_precondition( "
        << param_list( INFO( user_event, sy_event ), param_data::EMIT_ACTUAL )
        << " );\n"
            "}\n";
}

void cpp_generator::emit_source_line_no( ostream &o, unsigned alt_no ) const {
  if ( opt_line_directives )
    o << "#line " << (alt_no != 0 ? alt_no : cc.source_->line_no_)
      << " \"" << cc.source_->path() << "\"\n";
}

void cpp_generator::emit_text( char const *text ) const {
  T_OUT << text;
}

void cpp_generator::emit_transition_target_begin() const {
  U_OUT << CHSM_NS_ALIAS << "::state* "
        << cc.sy_chsm_->name() << "::" << chsm_info::PREFIX_TARGET
        << CHSM->id_.target_ << "( " << CHSM_NS_ALIAS
        << "::event const &event ) {\n"
        << indent << "(void)event;\n";
  emit_source_line_no( U_OUT );
  U_OUT << "return ";
}

void cpp_generator::emit_transition_target_end() const {
  U_OUT << "; }\n";
}

void cpp_generator::get_filename_exts( string *declaration_ext,
                                       string *definition_ext ) const {
  assert( declaration_ext != nullptr );
  assert( definition_ext != nullptr );
  *declaration_ext = "h";
  *definition_ext = "cpp";
}

///////////////////////////////////////////////////////////////////////////////

void cpp_declarer::emit() {
  string const include_guard =
    PJL::identify( PJL::base_name( cc.source_->path() ) ) + "_H";

  T_OUT << inc_indent;

  T_OUT << "#ifndef "<< include_guard T_ENDL
        << "#define "<< include_guard T_ENDL
        T_ENDL
        << section_comment << "<<" << PACKAGE_STRING << ">>" T_ENDL
        T_ENDL
        << "#include <chsm.h>" T_ENDL
        << "namespace " << CHSM_NS_ALIAS << " = CHSM_NS;" T_ENDL
        T_ENDL
        << section_comment << "user declarations" T_ENDL
        T_ENDL;
  emit_source_line_no( T_OUT );

  CHSM->accept( *this );

  //
  // Switch from emitting the declaration file to emitting the definition file
  // only if we're not emitting to stdout.
  //
  if ( !opt_codegen_only ) {
    string const declaration_name( cc.target_->path() );
    cc.target_.reset( new target_file( opt_definition_file ) );
    T_OUT << inc_indent;
    T_OUT << "///// <<" << PACKAGE_STRING << ">>" T_ENDL
          T_ENDL
          << "#include \"" << declaration_name << '"' T_ENDL
          T_ENDL;
  }

  cc.code_gen_.reset( new cpp_definer );
  // don't touch *this now
  cc.code_gen_->emit();
}

void cpp_declarer::emit_actions( symbol const *sy_state ) {
  auto const &action = INFO_CONST( state, sy_state )->action_;

  if ( action.has_enter_ )
    T_OUT << indent(2) << "void "
          << chsm_info::PREFIX_ENTER << chsm_info::PREFIX_ACTION
          << mangle( sy_state->name() )
          << "( "
          << CHSM_NS_ALIAS << "::state const&, "
          << CHSM_NS_ALIAS << "::event const&"
          << " );" T_ENDL;

  if ( action.has_exit_ )
    T_OUT << indent(2) << "void "
          << chsm_info::PREFIX_EXIT << chsm_info::PREFIX_ACTION
          << mangle( sy_state->name() )
          << "( "
          << CHSM_NS_ALIAS << "::state const&, "
          << CHSM_NS_ALIAS << "::event const&"
          << " );" T_ENDL;
}

void cpp_declarer::emit_common( event_info const &si ) {
  symbol const *const sy = si.get_symbol();
  T_OUT << "private:" T_ENDL
        << indent << "static "
        << CHSM_NS_ALIAS << "::transition::id const "
        << sy->name() << TRANSITIONS_SUFFIX << "[];" T_ENDL
        << "public:" T_ENDL;
}

void cpp_declarer::emit_common( parent_info const &si,
                                char const *formal_args ) {
  symbol const *const sy = si.get_symbol();
  char const *const base_name = state_base_name( sy->name() );

  T_OUT << indent << "class " << PARENT_CLASS_PREFIX << base_name
        << " : public " << lib_class_name( si ) << " { // id = "
        << ::serial( sy ) T_ENDL
        << indent << "public:" T_ENDL;

  //
  // Recursively emit declarations for child states only if this is not the
  // root state.  The root state is "special" in that its children are not
  // lexically enclosed by it.
  //
  if ( &si != INFO_CONST( parent, SY_ROOT ) ) {
    T_OUT << inc_indent;
    for ( auto const &sy_child : si.children_ ) {
      INFO_CONST( state, sy_child )->accept( *this );
      T_OUT T_ENDL;
    } // for
    T_OUT << dec_indent T_ENDL;
  }

  T_OUT << indent(2)
        << PARENT_CLASS_PREFIX << base_name << "( CHSM_STATE_ARGS";

  if ( formal_args != nullptr )
    T_OUT << ", " << formal_args;

  T_OUT << " );" T_ENDL
        << indent << "private:" T_ENDL
        << indent(2) << "static state::id const children_[];" T_ENDL
        << indent << "} " << base_name << ';';
}

void cpp_declarer::emit_common( state_info const &si ) {
  symbol const *const sy = si.get_symbol();
  T_OUT << indent << lib_class_name( si )
        << ' ' << state_base_name( sy->name() )
        << "; // id = " << ::serial( sy );
}

void cpp_declarer::visit( chsm_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << section_comment << "machine class declaration" T_ENDL
        T_ENDL
        << "class " << sy->name() << " : public ";

  if ( !si.derived_.empty() )
    T_OUT << si.derived_;
  else
    T_OUT << CHSM_NS_ALIAS << "::machine";

  T_OUT << " {" T_ENDL
        << "public:" T_ENDL;

  // emit constructor declaration
  T_OUT << indent << sy->name() << '(' << param_list( si ) << ");" T_ENDL;

  // emit destructor declaration
  T_OUT << indent << '~' << sy->name() << "();" T_ENDL;

  // emit state declarations, recursively
  T_OUT T_ENDL
        << indent << "// states" T_ENDL;
  INFO_CONST( state, si.sy_root_ )->accept( *this );
  T_OUT T_ENDL;

  for ( auto const &child : INFO_CONST( parent, si.sy_root_ )->children_ ) {
    INFO_CONST( state, child )->accept( *this );
    T_OUT T_ENDL;
  } // for

  // emit event declarations
  T_OUT T_ENDL
        << indent << "// events" T_ENDL;
  for ( auto const &event : si.events_ ) {
    INFO_CONST( event, event )->accept( *this );
    T_OUT T_ENDL;
  } // for

  // emit transition condition member function declarations
  T_OUT T_ENDL
        << indent << "// transition conditions" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.condition_; ++id )
    T_OUT << indent << "bool " << chsm_info::PREFIX_CONDITION << id
          << "( " << CHSM_NS_ALIAS << "::event const& );" T_ENDL;

  // emit transition target member function declarations
  T_OUT T_ENDL
        << indent << "// transition targets" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.target_; ++id )
    T_OUT << indent << CHSM_NS_ALIAS << "::state* "
          << chsm_info::PREFIX_TARGET << id
          << "( " << CHSM_NS_ALIAS << "::event const& );" T_ENDL;

  // emit transition action member function declarations
  T_OUT T_ENDL
        << indent << "// transition actions" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.action_; ++id )
    T_OUT << indent << "void " << chsm_info::PREFIX_ACTION << id
          << "( " << CHSM_NS_ALIAS << "::event const& );" T_ENDL;

  // emit enter/exit action member function declarations
  T_OUT T_ENDL
        << indent << "// enter/exit actions" T_ENDL;
  emit_actions( si.sy_root_ );
  for ( auto const &state : si.states_ )
    emit_actions( state );

  // emit data members
  T_OUT T_ENDL
        << "private:" T_ENDL
        << indent << CHSM_NS_ALIAS << "::state *state_["
        << si.states_.size() + 1 << "];" T_ENDL

        << indent << "static " << CHSM_NS_ALIAS
        << "::transition const transition_[];" T_ENDL
        << indent << CHSM_NS_ALIAS << "::event const *taken_["
        // degenerate case: no transitions; silly, but legal
        << (si.transitions_.empty() ? 1 : si.transitions_.size())
        << "];" T_ENDL

        << indent << CHSM_NS_ALIAS << "::state *target_["
        << (si.transitions_.empty() ? 1 : si.transitions_.size())
        << "];" T_ENDL

        << "};" T_ENDL;

    emit_the_end();
    T_OUT << "#endif" T_ENDL;
}

void cpp_declarer::visit( child_info const& ) {
  // nothing to do
}

void cpp_declarer::visit( cluster_info const &si ) {
  emit_common( si, "bool chsm_history_" );
}

void cpp_declarer::visit( event_info const &si ) {
  symbol const *const sy = si.get_symbol();
  emit_common( si );
  T_OUT << indent << lib_class_name( si ) << ' ' << sy->name() << ';';
}

void cpp_declarer::visit( global_info const& ) {
  // nothing to do
}

void cpp_declarer::visit( set_info const &si ) {
  emit_common( si );
}

void cpp_declarer::visit( state_info const &si ) {
  emit_common( si );
}

void cpp_declarer::visit( transition_info const& ) {
  // nothing to do
}

void cpp_declarer::visit( user_event_info const &si ) {
  symbol const *const sy = si.get_symbol();
  emit_common( si );

  T_OUT << indent << "class " << class_name( si )
        << " : public " << base_class_name( si ) << " {" T_ENDL
        << indent(2)
        << "typedef " << base_class_name( si ) << " base_event;" T_ENDL
        << indent(2)
        << "typedef base_event::param_block base_param_block;" T_ENDL
        << indent << "public:" T_ENDL;

  // emit param_block data member declarations
  T_OUT << indent(2) << "struct param_block : base_param_block {" T_ENDL;
  for ( auto const &param : si.param_list_ ) {
    emit_source_line_no( T_OUT, param.line_no_ );
    T_OUT << indent(3)
          << si.stuff_decl( param.declaration_, "", param.variable_ )
          << ';' T_ENDL;
  } // for

  // emit param_block constructor declaration
  T_OUT << indent(2) << "param_block( " << CHSM_NS_ALIAS << "::event const&";
  if ( si.has_any_parameters() ) {
    T_OUT << param_list( si, param_data::EMIT_COMMA ) << ");" T_ENDL;
  } else {
    T_OUT << " event ) : " T_ENDL
          << indent(3) << "base_param_block( event ) { }" T_ENDL;
  }

  // start of protected section
  T_OUT << indent(2) << "protected:" T_ENDL;

  // emit param_block destructor declaration
  T_OUT << indent(3) << "virtual ~param_block();" T_ENDL;

  // emit precondition declaration
  if ( si.precondition_ != user_event_info::PRECONDITION_NONE )
    T_OUT << indent(3) << "bool precondition() const;" T_ENDL;

  T_OUT << indent(2) << "};" T_ENDL;

  if ( si.has_any_parameters() ) {
    // emit operator-> definition
    T_OUT << indent(2) << "param_block* operator->() const {" T_ENDL
          << indent(3) << "return static_cast<param_block*>(param_block_);" T_ENDL
          << indent(2) << '}' T_ENDL;
  }

  // emit event operator() declaration
  if ( si.has_any_parameters() ||
       si.precondition_ != user_event_info::PRECONDITION_NONE ) {
    T_OUT << indent(2) << "void operator()(" << param_list( si ) << ");" T_ENDL;
  }

  // emit event constructor definition
  T_OUT << indent << "protected:" T_ENDL
        << indent(2) << class_name( si )
        << "( CHSM_EVENT_ARGS ) : base_event( CHSM_EVENT_INIT ) { }" T_ENDL;

  // emit rest of event declaration
  T_OUT << indent(2) << "friend class " << cc.sy_chsm_->name() << ';' T_ENDL
        << indent << "} " << sy->name() << ';' T_ENDL
        << indent << "alignas(" << class_name( si ) << "::param_block) char "
        << sy->name()
        << "_param_block[ sizeof(" << class_name( si ) 
        << "::param_block) ];" T_ENDL;

  if ( si.precondition_ == user_event_info::PRECONDITION_FUNC ) {
    T_OUT << indent << "bool "
          << sy->name() << "_precondition( "
          << param_list( si )
          << ") const;" T_ENDL;
  }
}

///////////////////////////////////////////////////////////////////////////////

cpp_definer::cpp_definer() {
}

void cpp_definer::emit() {
  T_OUT << "#include <new>" T_ENDL
        T_ENDL;
  emit_states();
  emit_events();
  emit_transitions();
  emit_chsm();
  T_OUT T_ENDL
        << "// user-code" T_ENDL;
  cc.user_code_->copy_to( T_OUT );
  emit_source_line_no( T_OUT );
  emit_the_end();
}

void cpp_definer::emit_chsm() {
  CHSM->accept( *this );
}

void cpp_definer::emit_common( event_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << CHSM_NS_ALIAS << "::transition::id const "
        << cc.sy_chsm_->name() << "::"
        << sy->name() << TRANSITIONS_SUFFIX << "[] = {" T_ENDL
        << indent;

  for ( auto const &tid : si.transition_ids_ )
    T_OUT<< tid << ", ";

  T_OUT << -1 T_ENDL
        << "};" T_ENDL;
}

void cpp_definer::emit_common( parent_info const &si,
                               char const *formal_args,
                               char const *actual_args ) {
  symbol const *const sy = si.get_symbol();
  char const *const name = sy->name();

  static char conv_buf[ 256 ];
  { // local scope
    //
    // "Inline function" to convert a state name:
    //
    //      x.y.z -> Px::Py::Pz
    //
    // (where P is a prefix) leaving the result in: conv_buf.
    //
    static int const prefix_length = ::strlen( PARENT_CLASS_PREFIX );
    ::strcpy( conv_buf, PARENT_CLASS_PREFIX );
    char *end = ::strchr( conv_buf, '\0' );
    for ( char const *c = name; *c; ++c ) {
      if ( *c == '.' ) {
        ::strcpy( end, "::" );
        end += 2;
        ::strcpy( end, PARENT_CLASS_PREFIX );
        end += prefix_length;
      } else {
        *end++ = *c;
      }
    } // for
    *end = '\0';
  } // end scope

  // emit child state vector
  T_OUT << CHSM_NS_ALIAS << "::state::id const "
        << cc.sy_chsm_->name() << "::" << conv_buf << "::children_[] = {" T_ENDL
        << indent;

  for ( auto const &sy_child : si.children_ )
    T_OUT<< ::serial( sy_child ) << ", ";

  T_OUT << -1 T_ENDL
        << "};" T_ENDL;

  // emit state constructor
  T_OUT << cc.sy_chsm_->name() << "::" << conv_buf << "::"
        << PARENT_CLASS_PREFIX
        << state_base_name( name ) << "( CHSM_STATE_ARGS";

  if ( formal_args != nullptr )
    T_OUT << ", " << formal_args;

  T_OUT << " ) :" T_ENDL
        << indent << lib_class_name( si ) << "( CHSM_STATE_INIT, children_";

  if ( actual_args != nullptr )
    T_OUT << ", " << actual_args;

  T_OUT << " )";

  if ( &si != INFO_CONST( parent, SY_ROOT ) ) {
    //
    // Emit initializers for child states only if this is not the root state.
    // The root state is "special" in that its children are not lexically
    // enclosed by it.
    //
    for ( auto const &sy_child : si.children_ ) {
      T_OUT << ',' T_ENDL;
      INFO_CONST( state, sy_child )->accept( initializer_ );
    } // for
  }

  T_OUT T_ENDL
        << '{' T_ENDL
        << '}' T_ENDL;
}

void cpp_definer::emit_events() {
  T_OUT << section_comment << "event definitions" T_ENDL
        T_ENDL;
  param_data::default_emit_flags_ = param_data::EMIT_FORMAL;
  for ( auto const &sy_event : CHSM->events_ ) {
    INFO_CONST( event, sy_event )->accept( *this );
    T_OUT T_ENDL;
  } // for
}

void cpp_definer::emit_states() {
  T_OUT << section_comment << "state definitions" T_ENDL
        T_ENDL;
  INFO_CONST( parent, SY_ROOT )->accept( *this );
  T_OUT T_ENDL;

  for ( auto const &sy_state : CHSM->states_ ) {
    if ( (type_of( sy_state ) & TYPE(PARENT)) != TYPE(NONE) ) {
      INFO_CONST( parent, sy_state )->accept( *this );
      T_OUT T_ENDL;
    }
  } // for
}

void cpp_definer::emit_transitions() {
  T_OUT << section_comment << "transitions" T_ENDL
        T_ENDL
        << CHSM_NS_ALIAS << "::transition const "
        << cc.sy_chsm_->name() << "::transition_[] = {" T_ENDL;

  for ( auto const &sy_transition : CHSM->transitions_ ) {
    INFO_CONST( transition, sy_transition )->accept( *this );
    T_OUT T_ENDL;
  } // for

  T_OUT << indent << "{ nullptr, 0, 0, nullptr, nullptr }" T_ENDL
        << "};" T_ENDL
        T_ENDL;
}

void cpp_definer::visit( child_info const& ) {
  // nothing to do
}

void cpp_definer::visit( chsm_info const &si ) {
  // Set to true to alter how states emit their mem-initializers.
  initializer_.emitting_constructor_ = true;

  symbol const *const sy = si.get_symbol();

  T_OUT << section_comment << "CHSM constructor definition" T_ENDL
        T_ENDL
        << sy->name() << "::" << sy->name() << '('
        << param_list( si, param_data::EMIT_PREFIX | param_data::EMIT_FORMAL )
        << ") :" T_ENDL
        << indent;

  if ( !si.derived_.empty() )
    T_OUT << si.derived_;
  else
    T_OUT << CHSM_NS_ALIAS << "::machine";

  T_OUT << "( state_, root, transition_, taken_, target_, "
        << si.transitions_.size()
        << param_list( si,
            param_data::EMIT_COMMA |
            param_data::EMIT_ACTUAL |
            param_data::EMIT_PREFIX )
        << " )";

  // emit member-initializers for states
  T_OUT << ',' T_ENDL;
  INFO_CONST( parent, si.sy_root_ )->accept( initializer_ );
  for ( auto const &child : INFO_CONST( parent, si.sy_root_ )->children_ ) {
    T_OUT << ',' T_ENDL;
    INFO_CONST( state, child )->accept( initializer_ );
  } // for

  // emit member-initializers for events
  for ( auto const &event : si.events_ ) {
    T_OUT << ',' T_ENDL;
    INFO_CONST( event, event )->accept( initializer_ );
  } // for
  T_OUT T_ENDL
        << '{' T_ENDL;

  // emit state vector initialization
  unsigned state_idx = 0;
  for ( auto const &state : si.states_ ) {
    T_OUT << indent << "state_[" << state_idx++ << "] = &" << state->name()
          << ';' T_ENDL;
  } // for
  T_OUT << indent << "state_[" << state_idx << "] = nullptr;" T_ENDL;

#ifdef CHSM_AUTO_ENTER_EXIT
  // emit the statement to enter the CHSM
  T_OUT T_ENDL
        << indent << "enter();" T_ENDL;
#endif /* CHSM_AUTO_ENTER_EXIT */

  T_OUT << '}' T_ENDL;

  // emit destructor definition
  T_OUT T_ENDL
        << sy->name() << "::~" << sy->name() << "() {" T_ENDL
#ifdef CHSM_AUTO_ENTER_EXIT
        << indent << "exit();" T_ENDL
#endif /* CHSM_AUTO_ENTER_EXIT */
        << '}' T_ENDL;
}

void cpp_definer::visit( cluster_info const &si ) {
  emit_common( si, "bool chsm_history_", "chsm_history_" );
}

void cpp_definer::visit( event_info const &si ) {
  emit_common( si );
}

void cpp_definer::visit( global_info const& ) {
  // nothing to do
}

void cpp_definer::visit( set_info const &si ) {
  emit_common( si );
}

void cpp_definer::visit( state_info const& ) {
  // nothing to do
}

void cpp_definer::visit( transition_info const &si ) {
  T_OUT << indent << "{ ";

  if ( si.condition_id_ > 0 )
    T_OUT << "static_cast<" << CHSM_NS_ALIAS << "::transition::condition>(&"
          << cc.sy_chsm_->name() << "::"
          << chsm_info::PREFIX_CONDITION << si.condition_id_ << ')';
  else
    T_OUT << "nullptr";

  T_OUT << ", " << ::serial( si.sy_from_ )
        << ", " << ::serial( si.sy_to_ )
        << ", ";

  if ( si.target_id_ > 0 )
    T_OUT << "static_cast<" << CHSM_NS_ALIAS << "::transition::target>(&"
          << cc.sy_chsm_->name() << "::"
          << chsm_info::PREFIX_TARGET << si.target_id_ << ')';
  else
    T_OUT << "nullptr";

  T_OUT << ", ";

  if ( si.action_id_ > 0 )
    T_OUT << "static_cast<" << CHSM_NS_ALIAS << "::transition::action>(&"
          << cc.sy_chsm_->name() << "::"
          << chsm_info::PREFIX_ACTION << si.action_id_ << ')';
  else
    T_OUT << "nullptr";

  T_OUT << " },";
}

void cpp_definer::visit( user_event_info const &si ) {
  visit( static_cast<event_info const&>( si ) );
  symbol const *const sy = si.get_symbol();

  if ( si.has_any_parameters() ) {
    //
    // emit param_block constructor definition
    //
    // The parameter formal argument names can not have the same names as the
    // data members, so we prefix each one by a string, i.e.:
    //
    //      param_block( T Pparam ) : param( Pparam ) { }
    //                     ^                 ^
    //
    T_OUT << cc.sy_chsm_->name() << "::" << class_name( si )
          << "::param_block::param_block( "
          << CHSM_NS_ALIAS << "::event const &event"
          << param_list( si,
              param_data::EMIT_COMMA |
              param_data::EMIT_PREFIX |
              param_data::EMIT_FORMAL )
          << " ) :" T_ENDL;

    // emit base event's param_block initializer
    T_OUT << indent << "base_param_block( event";
    if ( si.base_has_any_parameters() ) {
      T_OUT << param_list(
                INFO_CONST( user_event, si.sy_base_event_ ),
                param_data::EMIT_COMMA |
                param_data::EMIT_PREFIX |
                param_data::EMIT_ACTUAL
              );
    }
    T_OUT << " )";

    for ( auto const &param : si.param_list_ )
      T_OUT << ", " << param.variable_ << "( "
            << param_data::PARAM_PREFIX_ << param.variable_ << " )";

    T_OUT T_ENDL
          << '{' T_ENDL
          << '}' T_ENDL;
  }

  //
  // emit param_block destructor definition
  //
  // Because it's declared as virtual in the run-time library, we define it
  // out-of-line even though it's empty.
  //
  T_OUT << cc.sy_chsm_->name() << "::" << class_name( si )
        << "::param_block::~param_block() { }" T_ENDL;

  if ( si.has_any_parameters() || si.precondition_ ) {
    //
    // emit operator() definition
    //
    T_OUT T_ENDL
          << "void " << cc.sy_chsm_->name() << "::" << class_name( si )
          << "::operator()"
          << '(' << param_list( si, param_data::EMIT_FORMAL ) << ") {" T_ENDL
          << indent << "machine_lock const lock( machine_ );" T_ENDL
          << indent << "if ( in_progress_ == 0 )" T_ENDL
          << indent(2) << "broadcast( new( static_cast<" << cc.sy_chsm_->name()
          << "&>(machine_)." << sy->name()
          << "_param_block ) param_block( *this"
          << param_list( si, param_data::EMIT_COMMA | param_data::EMIT_ACTUAL )
          << " ) );" T_ENDL
          << '}' T_ENDL;
  }
}

///////////////////////////////////////////////////////////////////////////////

cpp_initializer::cpp_initializer() : emitting_constructor_( false ) {
}

void cpp_initializer::emit() {
  // nothing to do
}

void cpp_initializer::emit_common( event_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << indent << sy->name() << "( this, " << sy->name()
        << TRANSITIONS_SUFFIX << ", \"";

  if ( si.kind_ == event_info::KIND_USER )
    T_OUT << sy->name();
  else
    T_OUT << (si.kind_ == event_info::KIND_ENTER ? "enter" : "exit")
          << '(' << demangle( sy->name() + 1 ) << ')';

  T_OUT << "\", ";
}

void cpp_initializer::emit_common( state_info const &si ) {
  symbol const *const sy = si.get_symbol();
  char const *const m_name = mangle( sy->name() );
  //
  // If we're emitting a mem-initializer for the CHSM constructor, the
  // reference to the CHSM is *this; otherwise, we're emitting for a parent
  // class in which case the reference to the CHSM is the sy_chsm_ data member.
  //
  char const *const chsm_ref =
    emitting_constructor_ ? "*this" : "chsm_machine_";

  T_OUT << indent << state_base_name( sy->name() ) << "( " << chsm_ref
        << ", \"" << sy->name() << "\", "
        << (si.sy_parent_ != nullptr ?
            //
            // The root state is "special" in that its children are not
            // lexically enclosed by it; hence, we have to emit "&root" rather
            // than "this".
            //
            (si.sy_parent_ == SY_ROOT ? "&root" : "this") :
            //
            // The root state is the only state with no parent.
            //
            "nullptr"
          )
        << ", ";

  // enter/exit actions
  if ( si.action_.has_enter_ )
    T_OUT << "static_cast<" << CHSM_NS_ALIAS << "::state::action>(&"
          << cc.sy_chsm_->name() << "::"
          << chsm_info::PREFIX_ENTER << chsm_info::PREFIX_ACTION << m_name
          << ')';
  else
    T_OUT << "nullptr";
  T_OUT << ", ";
  if ( si.action_.has_exit_ )
    T_OUT << "static_cast<" << CHSM_NS_ALIAS << "::state::action>(&"
          << cc.sy_chsm_->name() << "::"
          << chsm_info::PREFIX_EXIT << chsm_info::PREFIX_ACTION << m_name
          << ')';
  else
    T_OUT << "nullptr";
  T_OUT << ", ";

  // enter/exit events
  if ( si.event_.has_enter_ )
    T_OUT << "&((" << cc.sy_chsm_->name() << "&)" << chsm_ref << ")."
          << chsm_info::PREFIX_ENTER << m_name;
  else
    T_OUT << "nullptr";
  T_OUT << ", ";
  if ( si.event_.has_exit_ )
    T_OUT << "&((" << cc.sy_chsm_->name() << "&)" << chsm_ref << ")."
          << chsm_info::PREFIX_EXIT << m_name;
  else
    T_OUT << "nullptr";
}

void cpp_initializer::visit( child_info const& ) {
  // nothing to do
}

void cpp_initializer::visit( chsm_info const& ) {
  // nothing to do
}

void cpp_initializer::visit( cluster_info const &si ) {
  emit_common( si );
  T_OUT << ", " << (si.history_ ? "true" : "false") << " )";
}

void cpp_initializer::visit( event_info const &si ) {
  emit_common( si );
  T_OUT << 0 << " )";
}

void cpp_initializer::visit( global_info const& ) {
  // nothing to do
}

void cpp_initializer::visit( set_info const &si ) {
  emit_common( si );
  T_OUT << " )";
}

void cpp_initializer::visit( state_info const &si ) {
  emit_common( si );
  T_OUT << " )";
}

void cpp_initializer::visit( transition_info const& ) {
  // nothing to do
}

void cpp_initializer::visit( user_event_info const &si ) {
  emit_common( si );

  if ( si.sy_base_event_ != nullptr )
    T_OUT << '&' << si.sy_base_event_->name();
  else
    T_OUT << "nullptr";

  T_OUT << " )";
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
