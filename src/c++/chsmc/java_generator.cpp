/*
**      CHSM Language System
**      src/c++/chsmc/java_generator.cpp
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
#include "chsm_compiler.h"
#include "chsm_info.h"
#include "cluster_info.h"
#include "event_info.h"
#include "indent.h"
#include "java_generator.h"
#include "list_sep.h"
#include "mangle.h"
#include "options.h"
#include "param_data.h"
#include "set_info.h"
#include "transition_info.h"
#include "user_event_info.h"
#include "util.h"

// standard
#include <functional>
#include <map>
#include <string>
#include <typeinfo>

// macros to aid in argument-lists
#define CHSM_FORMAL_ARG(X)              #X " "
#define CHSM_ACTUAL_ARG(X)              /* nothing */

#define CHSM_STATE_ARG_LIST(A) \
        A(CHSM.Machine) "chsmM, " \
        A(String) "chsmN, " \
        A(CHSM.Parent) "chsmP, " \
        A(CHSM.State.Action) "chsmEA, " \
        A(CHSM.State.Action) "chsmXA, " \
        A(CHSM.Event) "chsmEE, " \
        A(CHSM.Event) "chsmXE"

#define CHSM_STATE_ARGS CHSM_STATE_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_STATE_INIT CHSM_STATE_ARG_LIST(CHSM_ACTUAL_ARG)

#define CHSM_EVENT_ARG_LIST(A) \
        A(CHSM.Machine) "chsmM, " \
        A(int[]) "chsmTids, " \
        A(String) "chsmN, " \
        A(CHSM.Event) "chsmBE"

#define CHSM_EVENT_ARGS CHSM_EVENT_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_EVENT_INIT CHSM_EVENT_ARG_LIST(CHSM_ACTUAL_ARG)

#define CHSM_ARG_LIST(A) \
        A(CHSM.State) "chsmS" A([])", " \
        A(CHSM.Transition) "chsmT" A([])", " \
        A(CHSM.Event) "chsmTE" A([])

#define CHSM_ARGS CHSM_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_INIT CHSM_ARG_LIST(CHSM_ACTUAL_ARG)

#define CHSM_PARENT_ARG_LIST(A) \
        CHSM_STATE_ARG_LIST(A)", " \
        A(int[]) "chsmCids"

#define CHSM_PARENT_ARGS CHSM_PARENT_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_PARENT_INIT CHSM_PARENT_ARG_LIST(CHSM_ACTUAL_ARG)

#define CHSM_CLUSTER_ARG_LIST(A) \
        CHSM_PARENT_ARG_LIST(A)", " \
        A(boolean) "chsmH"

#define CHSM_CLUSTER_ARGS CHSM_CLUSTER_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_CLUSTER_INIT CHSM_CLUSTER_ARG_LIST(CHSM_ACTUAL_ARG)

#define CHSM_SET_ARG_LIST(A) \
        CHSM_PARENT_ARG_LIST(A)

#define CHSM_SET_ARGS CHSM_SET_ARG_LIST(CHSM_FORMAL_ARG)
#define CHSM_SET_INIT CHSM_SET_ARG_LIST(CHSM_ACTUAL_ARG)

using namespace std;
using namespace PJL;

static char const CHSM_PACKAGE_NAME[]   = "CHSM";
static char const EVENT_CLASS_SUFFIX[]  = "Event";
static char const STATE_CLASS_PREFIX[]  = "State_";
static char const TRANSITIONS_SUFFIX[]  = "Transitions";

///////////////////////////////////////////////////////////////////////////////

namespace {

class java_definer;

/**
 * A %java_initializer is-a java_generator that generates the initialization
 * portion (the calls to constructors of data members) of Java code.
 */
class java_initializer : public java_generator {
private:
  /**
   * Constructs a %java_initializer.
   */
  java_initializer();

  bool emitting_constructor_;

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

  friend class java_definer;
};

/**
 * A %java_definer is-a java_generator that generates the definitions portion
 * of Java code.
 */
class java_definer : public java_generator {
private:
  java_initializer initializer_;

  void emit_chsm();

  void emit_common( event_info const &si );

  void emit_common( parent_info const &si,
                    char const *actual_params = nullptr );

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

  friend class java_declarer;
};

/**
 * A %java_declarer is-a java_generator that generates the declarations portion
 * of Java code.
 */
class java_declarer : public java_generator {
public:
  java_declarer();

  void emit_actions( PJL::symbol const *sy_state );
  void emit_common( state_info const &si );
  void emit_common( event_info const &si );

  void emit_common( parent_info const &si,
                    char const *formal_params = nullptr );

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

private:
  std::unique_ptr<java_definer> java_definer_;
};

} // namespace

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
          return o << CHSM_PACKAGE_NAME << ".Cluster";
      };
    }
    TYPE_CASE( ei, event_info const ) {
      return []( ostream &o ) -> ostream& {
        return o << CHSM_PACKAGE_NAME << ".Event";
      };
    }
    TYPE_CASE( si, set_info const ) {
      return [si]( ostream &o ) -> ostream& {
        if ( !si->derived_.empty() )
          return o << si->derived_;
        else
          return o << CHSM_PACKAGE_NAME << ".Set";
      };
    }
    TYPE_CASE( si, state_info const ) { // must be after cluster & set
      return [si]( ostream &o ) -> ostream& {
        if ( !si->derived_.empty() )
          return o << si->derived_;
        else
          return o << CHSM_PACKAGE_NAME << ".State";
      };
    }
    TYPE_DEFAULT {
      INTERNAL_ERR( '"' << typeid( bi ).name() << "\": unexpected type\n" );
    }
  }
  return nullptr;
}

static inline ostream_manip base_class_name( user_event_info const &si ) {
  return [&si]( ostream &o ) -> ostream& {
    if ( si.sy_base_event_ != nullptr )
      o << cc.sy_chsm_->name() << '.'
        << si.sy_base_event_->name() << EVENT_CLASS_SUFFIX;
    else
      o << lib_class_name( si );
    return o;
  };
}

static inline ostream_manip class_name( user_event_info const &si ) {
  return [&si]( ostream &o ) -> ostream& {
    o << si.get_symbol()->name() << EVENT_CLASS_SUFFIX;
    return o;
  };
}

/**
 * Performs substitutions of the CHSM_* macros on a line of text for their
 * expansions.  We have to do this ourselves for Java since Java doesn't have a
 * macro preprocessor like C++ does.  :-(
 *
 * @param line The line to do replacements on.
 * @return Returns the line with the replacements having been done.
 */
static string substitute( char const *line ) {
  typedef map<string,char const*> macro_map_type;
  static macro_map_type const macro_map {
    { "CHSM_CLUSTER_ARGS",  CHSM_CLUSTER_ARGS },
    { "CHSM_CLUSTER_INIT",  CHSM_CLUSTER_INIT },
    { "CHSM_EVENT_ARGS",    CHSM_EVENT_ARGS   },
    { "CHSM_EVENT_INIT",    CHSM_EVENT_INIT   },
    { "CHSM_PARENT_ARGS",   CHSM_PARENT_ARGS  },
    { "CHSM_PARENT_INIT",   CHSM_PARENT_INIT  },
    { "CHSM_SET_ARGS",      CHSM_SET_ARGS     },
    { "CHSM_SET_INIT",      CHSM_SET_INIT     },
    { "CHSM_STATE_ARGS",    CHSM_STATE_ARGS   },
    { "CHSM_STATE_INIT",    CHSM_STATE_INIT   },
  };

  string s( line );
  for ( auto const &i : macro_map ) {
    for ( string::size_type pos; (pos = s.find( i.first )) != string::npos; )
      s.replace( pos, i.first.length(), i.second );
  } // for

  return s;
}

///////////////////////////////////////////////////////////////////////////////

unique_ptr<code_generator> java_generator::create() {
  return unique_ptr<code_generator>( new java_declarer );
}

void java_generator::emit_action_block_begin() const {
  U_OUT << indent << "private static final class Thunk_"
        << chsm_info::PREFIX_ACTION << CHSM->id_.action_
        << " implements " << CHSM_PACKAGE_NAME << ".Transition.Action {\n"
        << indent(2) << "public void exec("
        << CHSM_PACKAGE_NAME << ".Event event ) {\n"
        << indent(3) << "((" << cc.sy_chsm_->name()
        << ")event.machine()).thunk_" << chsm_info::PREFIX_ACTION
        << CHSM->id_.action_ << "( event );\n"
        << indent(2) << "}\n"
        << indent << "}\n"
        << indent << "private void thunk_"
        << chsm_info::PREFIX_ACTION << CHSM->id_.action_
        << "( " << CHSM_PACKAGE_NAME << ".Event event ) {\n";
  emit_source_line_no( U_OUT );
}

void java_generator::emit_action_block_end() const {
  U_OUT << "}\n";
}

void java_generator::emit_condition_expr_begin() const {
  U_OUT << indent << "private static final class Thunk_"
        << chsm_info::PREFIX_CONDITION << CHSM->id_.condition_
        << " implements " << CHSM_PACKAGE_NAME << ".Transition.Condition {\n"
        << indent(2) << "public boolean eval( "
        << CHSM_PACKAGE_NAME << ".Event event ) {\n"
        << indent(3) << "return ((" << cc.sy_chsm_->name()
        << ")event.machine()).thunk_"
        << chsm_info::PREFIX_CONDITION << CHSM->id_.condition_
        << "( event );\n"
        << indent(2) << "}\n"
        << indent << "}\n"
        << indent << "private boolean thunk_"
        << chsm_info::PREFIX_CONDITION << CHSM->id_.condition_
        << "( " << CHSM_PACKAGE_NAME << ".Event event ) {\n";
  emit_source_line_no( U_OUT );
  U_OUT << indent(2) << "return ";
}

void java_generator::emit_condition_expr_end() const {
  U_OUT << ";\n"
        << indent << "}\n";
}

void java_generator::emit_enter_exit_begin( char const *kind,
                                            symbol const *sy_state ) const {
  U_OUT << indent << "private static final class Thunk_" << kind
        << chsm_info::PREFIX_ACTION << mangle( sy_state->name() )
        << " implements " << CHSM_PACKAGE_NAME << ".State.Action {\n"
        << indent(2) << "public void exec( "
        << CHSM_PACKAGE_NAME << ".State state, "
        << CHSM_PACKAGE_NAME << ".Event event ) {\n"
        << indent(3) << "((" << cc.sy_chsm_->name()
        << ")state.machine())." << kind
        << chsm_info::PREFIX_ACTION << mangle( sy_state->name() )
        << "( state, event );\n"
        << indent(2) << "}\n"
        << indent << "}\n"
        << indent <<  "private void " << kind
        << chsm_info::PREFIX_ACTION << mangle( sy_state->name() )
        << "( "
        << CHSM_PACKAGE_NAME << ".State state, "
        << CHSM_PACKAGE_NAME << ".Event event ) {\n";
  emit_source_line_no( U_OUT );
}

void java_generator::emit_enter_exit_end() const {
  U_OUT << indent << "}\n";
}

void java_generator::
emit_precondition_expr_begin( symbol const *sy_event ) const {
  //
  // The Java function generated for a precondition expression is the && of the
  // base event's precondition function and the Java expression the user
  // specified.
  //
  // Note: The user's expression must be wrapped in ()'s so the resulting Java
  // expression has the proper precedence and is evaluated correctly.
  //
  U_OUT << indent << "boolean " << sy_event->name() << "_precondition"
        << "( "
        << param_list( INFO( user_event, sy_event ), param_data::EMIT_FORMAL )
        << " ) {\n"
        << indent(2) << "return (\n";
  emit_source_line_no( U_OUT );
}

void java_generator::emit_precondition_expr_end() const {
  U_OUT << '\n'
        << indent(2) << ");\n"
        << indent << "}\n";
}

void java_generator::
emit_precondition_func_begin( symbol const *sy_event ) const {
  //
  // For a precondition function, the user's code is placed into a function
  // unto itself.
  //
  U_OUT << indent << "boolean " << sy_event->name() << "_precondition"
        << "( "
        << param_list( INFO( user_event, sy_event ), param_data::EMIT_FORMAL )
        << " ) {\n";
  emit_source_line_no( U_OUT );
}

void java_generator::
emit_precondition_func_end( symbol const* ) const {
  U_OUT << '\n'
        << indent << "}\n";
}

void java_generator::emit_text( char const *text ) const {
  T_OUT << substitute( text );
}

void java_generator::emit_transition_target_begin() const {
  U_OUT << indent << "private static final class Thunk_"
        << chsm_info::PREFIX_TARGET << CHSM->id_.target_
        << " implements " << CHSM_PACKAGE_NAME << ".Transition.Target {\n"
        << indent(2) << "public " << CHSM_PACKAGE_NAME
        << ".State eval( " << CHSM_PACKAGE_NAME << ".Event event ) {\n"
        << indent(3) << "return ((" << cc.sy_chsm_->name()
        << ")event.machine()).thunk_" << chsm_info::PREFIX_TARGET
        << CHSM->id_.target_ << "( event );\n"
        << indent(2) << "}\n"
        << indent << "}\n"
        << indent << "private "
        << CHSM_PACKAGE_NAME << ".State thunk_"
        << chsm_info::PREFIX_TARGET << CHSM->id_.target_ << "( "
        << CHSM_PACKAGE_NAME << ".Event event ) {\n";
  emit_source_line_no( U_OUT );
  U_OUT << indent(2) << "return ";
}

void java_generator::emit_transition_target_end() const {
  U_OUT << ";\n"
        << indent << "}\n";
}

void java_generator::get_filename_exts( string *declaration_ext,
                                        string *definition_ext ) const {
  assert( declaration_ext != nullptr );
  assert( definition_ext != nullptr );
  *declaration_ext = *definition_ext = "java";
}

///////////////////////////////////////////////////////////////////////////////

java_declarer::java_declarer() : java_definer_( new java_definer ) {
}

void java_declarer::emit() {
  T_OUT << inc_indent;

  T_OUT << section_comment << "<<" << PACKAGE_STRING << ">>" T_ENDL
        T_ENDL
        << section_comment << "user-declarations" T_ENDL
        T_ENDL;
  emit_source_line_no( T_OUT );

  CHSM->accept( *this );
  cc.code_gen_.reset( java_definer_.release() );
  // don't touch *this now
  cc.code_gen_->emit();
}

void java_declarer::emit_actions( symbol const *sy_state ) {
  (void)sy_state;
  // TODO: is this right?
}

void java_declarer::emit_common( event_info const &si ) {
  java_definer_->emit_common( si );
}

void java_declarer::emit_common( parent_info const &si,
                                 char const *formal_params ) {
  symbol const *const sy = si.get_symbol();
  char const *const base_name = state_base_name( sy->name() );

  T_OUT << indent << "final static class "
        << STATE_CLASS_PREFIX << base_name
        << " extends " << lib_class_name( si ) << " { // "
        << ::serial( sy ) T_ENDL
        << indent(2) << "private static final int children_[] = new int[] { ";

  list_sep comma;
  for ( auto const &child : si.children_ )
    T_OUT << comma << ::serial( child );

  T_OUT << " };" T_ENDL;

  //
  // Recursively emit declarations for child states only if this is not the
  // root state.  The root state is "special" in that its children are not
  // lexically enclosed by it.
  //
  if ( &si != INFO_CONST( parent, SY_ROOT ) ) {
    T_OUT << inc_indent;
    for ( auto const &sy_child : si.children_ )
      INFO_CONST( state, sy_child )->accept( *this );
    T_OUT << dec_indent;
  }

  // emit constructor declaration
  T_OUT << indent(2) << STATE_CLASS_PREFIX << base_name
        << "( " CHSM_STATE_ARGS;
  if ( formal_params != nullptr )
    T_OUT << ", " << formal_params;
  T_OUT << " ) {" T_ENDL;

  INFO_CONST( state, sy )->accept( *java_definer_ );

  T_OUT << indent << '}' T_ENDL
        << indent << "public final "
        << STATE_CLASS_PREFIX << base_name << ' '
        << base_name << ';' T_ENDL;
}

void java_declarer::emit_common( state_info const &si ) {
  symbol const *const sy = si.get_symbol();
  T_OUT << indent << "public final " << lib_class_name( si ) << ' '
        << state_base_name( sy->name() )
        << "; // id = " << ::serial( sy ) T_ENDL;
}

void java_declarer::visit( chsm_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << section_comment << "CHSM class declaration" T_ENDL
        T_ENDL;
  if ( si.java_public_ )
    T_OUT << "public ";
  T_OUT << "class " << sy->name() << " extends ";

  if ( !si.derived_.empty() )
    T_OUT << si.derived_;
  else
    T_OUT << CHSM_PACKAGE_NAME << ".Machine";

  T_OUT << " {" T_ENDL;

  // emit state declarations, recursively
  T_OUT T_ENDL
        << indent << "// states" T_ENDL;
  INFO_CONST( state, si.sy_root_ )->accept( *this );

  for ( auto const &child : INFO_CONST( parent, si.sy_root_ )->children_ )
    INFO_CONST( state, child )->accept( *this );

  // emit event declarations
  T_OUT T_ENDL
        << indent << "// events" T_ENDL;
  for ( auto const &event : si.events_ )
    INFO_CONST( event, event )->accept( *this );

  // emit enter/exit action member function declarations
  T_OUT T_ENDL
        << indent << "// enter/exit actions" T_ENDL;
  emit_actions( si.sy_root_ );
  for ( auto const &state : si.states_ )
    emit_actions( state );

  INFO( chsm, sy )->accept( *java_definer_ );

  // emit transition condition member function declarations
  T_OUT T_ENDL
        << indent << "// transition conditions" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.condition_; ++id ) {
    T_OUT << indent << "private static final Thunk_"
          << chsm_info::PREFIX_CONDITION << id << ' '
          << chsm_info::PREFIX_CONDITION << id << " = new Thunk_"
          << chsm_info::PREFIX_CONDITION << id << "();" T_ENDL;
  } // for

  // emit transition target member function declarations
  T_OUT T_ENDL
        << indent << "// transition targets" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.target_; ++id ) {
    T_OUT << indent << "private static final Thunk_"
          << chsm_info::PREFIX_TARGET << id << ' '
          << chsm_info::PREFIX_TARGET << id << " = new Thunk_"
          << chsm_info::PREFIX_TARGET << id << "();" T_ENDL;
  } // for

  // emit transition action member function declarations
  T_OUT T_ENDL
        << indent << "// transition actions" T_ENDL;
  for ( chsm_info::id_type id = 1; id <= si.id_.action_; ++id ) {
    T_OUT << indent << "private static final Thunk_"
          << chsm_info::PREFIX_ACTION << id << ' '
          << chsm_info::PREFIX_ACTION << id << " = new Thunk_"
          << chsm_info::PREFIX_ACTION << id << "();" T_ENDL;
  } // for

  // emit private data members
  T_OUT T_ENDL
        << indent << "// data members" T_ENDL
        << indent << "private final "
        << CHSM_PACKAGE_NAME << ".State state_[] = new "
        << CHSM_PACKAGE_NAME << ".State[" << si.states_.size() << "];" T_ENDL
        << indent << "private static final "
        << CHSM_PACKAGE_NAME << ".Transition transition_[] = { ";

  if ( !CHSM->transitions_.empty() ) {
    list_sep comma;
    for ( auto const &t : CHSM->transitions_ ) {
      T_OUT << comma T_ENDL
            << indent(2);
      INFO_CONST( transition, t )->accept( *this );
    } // for
  } else {
    T_OUT << "null";
  }

  T_OUT T_ENDL
        << indent << "};" T_ENDL;

  T_OUT T_ENDL
        << indent << "// user-code" T_ENDL;
  cc.user_code_->copy_to( T_OUT );
  T_OUT << '}' T_ENDL;
}

void java_declarer::visit( child_info const& ) {
  // nothing to do
}

void java_declarer::visit( cluster_info const &si ) {
  emit_common( si, "boolean chsm_history_" );
}

void java_declarer::visit( event_info const &si ) {
  emit_common( si );
  symbol const *const sy = si.get_symbol();
  T_OUT << indent << "protected final " << lib_class_name( si ) << ' '
        << sy->name() << ';';
}

void java_declarer::visit( global_info const& ) {
  // nothing to do
}

void java_declarer::visit( set_info const &si ) {
  emit_common( si );
}

void java_declarer::visit( state_info const &si ) {
  emit_common( si );
}

void java_declarer::visit( transition_info const &si ) {
  T_OUT << "new CHSM.Transition( ";

  if ( si.condition_id_ > 0 )
    T_OUT << chsm_info::PREFIX_CONDITION << si.condition_id_;
  else
    T_OUT << "null";

  T_OUT << ", " << ::serial( si.sy_from_ )
        << ", " << ::serial( si.sy_to_ )
        << ", ";

  if ( si.target_id_ > 0 )
    T_OUT << chsm_info::PREFIX_TARGET << si.target_id_;
  else
    T_OUT << "null";

  T_OUT << ", ";

  if ( si.action_id_ > 0 )
    T_OUT << chsm_info::PREFIX_ACTION << si.action_id_;
  else
    T_OUT << "null";

  T_OUT << " )";
}

void java_declarer::visit( user_event_info const &si ) {
  symbol const *const sy = si.get_symbol();
  emit_common( si );
  T_OUT << indent << "protected static class " << class_name( si )
        << " extends " << base_class_name( si ) << " {" T_ENDL;

  // emit ParamBlock data member declarations
  T_OUT << indent(2) << "protected static class ParamBlock extends "
        << base_class_name( si ) << ".ParamBlock {" T_ENDL;
  for ( auto const &param : si.param_list_ ) {
    T_OUT << indent(3)
          << si.stuff_decl( param.decl_, "", param.name_ )
          << ';' T_ENDL;
  } // for

  // emit ParamBlock constructor declaration
  T_OUT << indent(3) << "protected ParamBlock( "
        << CHSM_PACKAGE_NAME << ".Event event"
        << param_list( si,
            param_data::EMIT_COMMA |
            param_data::EMIT_PREFIX |
            param_data::EMIT_FORMAL
          )
        << " ) {" T_ENDL;
  INFO( user_event, sy )->accept( *java_definer_ );
  T_OUT << indent(3) << '}' T_ENDL;

  // emit precondition declaration
  if ( si.precondition_ != user_event_info::PRECONDITION_NONE ) {
    T_OUT << indent(3) << "protected boolean precondition() {" T_ENDL
          << indent(4) << "return super.precondition() &&\n"
          << indent(5) << "((" << cc.sy_chsm_->name() << ")event_.machine())."
          << sy->name() << "_precondition( "
          << param_list( si, param_data::EMIT_ACTUAL )
          << " );" T_ENDL
          << indent(3) << '}' T_ENDL;
  }

  T_OUT << indent(2) << '}' T_ENDL;

  // emit event constructor definition
  T_OUT << indent(2) << sy->name() << EVENT_CLASS_SUFFIX
        << "( " CHSM_EVENT_ARGS " ) {" T_ENDL
        << indent(3) << "super( " CHSM_EVENT_INIT " );" T_ENDL
        << indent(2) << '}' T_ENDL;

  if ( si.has_any_parameters() ||
       si.precondition_ != user_event_info::PRECONDITION_NONE ) {
    T_OUT << indent(2) << "public void broadcast( "
          << param_list( si, param_data::EMIT_FORMAL ) << " ) {" T_ENDL
          << indent(3) << "broadcast( new ParamBlock( this"
          << param_list( si, param_data::EMIT_COMMA | param_data::EMIT_ACTUAL )
          << " ) );" T_ENDL
          << indent(2) << '}' T_ENDL
          << indent(2) << "public final ParamBlock " << sy->name()
          << "Param() {" T_ENDL
          << indent(3) << "return (ParamBlock)paramBlock_;" T_ENDL
          << indent(2) << '}' T_ENDL;
  }

  T_OUT << indent << '}' T_ENDL;

  // emit rest of event declaration
  T_OUT << indent << "protected final " << sy->name() << EVENT_CLASS_SUFFIX
        << ' ' << sy->name() << ';' T_ENDL;

  // emit short-hand method for broadcasting an event
  T_OUT << indent << "public final void " << sy->name() << "( "
        << param_list( si , param_data::EMIT_FORMAL ) << " ) {" T_ENDL
        << indent(2) << sy->name() << ".broadcast( ";
  if ( si.has_any_parameters() )
    T_OUT << param_list( si, param_data::EMIT_ACTUAL );
  else
    T_OUT << "null";
  T_OUT << " );" T_ENDL
        << indent << '}' T_ENDL;
}

///////////////////////////////////////////////////////////////////////////////

void java_definer::emit() {
  // nothing to do
}

void java_definer::emit_common( event_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << indent << "private final static int "
        << sy->name() << TRANSITIONS_SUFFIX << "[] = { ";

  list_sep comma;
  for ( auto const &tid : si.transition_ids_ )
    T_OUT << comma << tid;

  T_OUT << " };" T_ENDL;
}

void java_definer::emit_common( parent_info const &si,
                                char const *actual_params ) {
  T_OUT << indent(3) << "super( " CHSM_STATE_INIT ", children_";
  if ( actual_params != nullptr )
    T_OUT << ", " << actual_params;
  T_OUT << " );" T_ENDL;

  if ( &si != INFO_CONST( parent, SY_ROOT ) ) {
    //
    // Emit initializers for child states only if this is not the root state.
    // The root state is "special" in that its children are not lexically
    // enclosed by it.
    //
    for ( auto const &child : si.children_ ) {
      INFO_CONST( state, child )->accept( initializer_ );
    } // for
  }
}

void java_definer::visit( child_info const& ) {
  // nothing to do
}

void java_definer::visit( chsm_info const &si ) {
  symbol const *const sy = si.get_symbol();

  // Set to true to alter how states emit their mem-initializers.
  initializer_.emitting_constructor_ = true;

  T_OUT T_ENDL
        << indent << "// constructor definition" T_ENDL
        << indent << "public " << sy->name() << "( "
        << param_list( si, param_data::EMIT_FORMAL ) << " ) {" T_ENDL
        << indent(2) << "super( " << param_list( si, param_data::EMIT_ACTUAL )
        << " );" T_ENDL;

  // emit initializers for events
  T_OUT T_ENDL
        << indent(2) << "// initialize events" T_ENDL;
  for ( auto const &event : si.events_ )
    INFO_CONST( event, event )->accept( initializer_ );

  // emit initializers for states
  T_OUT T_ENDL
        << indent(2) << "// initialize states" T_ENDL;
  INFO_CONST( state, si.sy_root_ )->accept( initializer_ );

  for ( auto const &child : INFO_CONST( parent, si.sy_root_ )->children_ )
    INFO_CONST( state, child )->accept( initializer_ );

  // emit state array initialization
  T_OUT T_ENDL
        << indent(2) << "// initialize state array" T_ENDL;

  unsigned state_idx = 0;
  for ( auto const &state : si.states_ ) {
    T_OUT << indent(2) << "state_[ " << state_idx++ << " ] = "
          << state->name() << ';' T_ENDL;
  } // for
  T_OUT T_ENDL
        << indent(2)
        << "init( " << si.sy_root_->name() << ", state_, transition_ );" T_ENDL;

#ifdef CHSM_AUTO_ENTER_EXIT
  // emit the statement to enter the CHSM
  T_OUT T_ENDL
        << indent << "enter();" T_ENDL;
#endif

  T_OUT << indent << '}' T_ENDL;
}

void java_definer::visit( cluster_info const &si ) {
  emit_common( si, "chsm_history_" );
  T_OUT << indent(2) << '}' T_ENDL;
}

void java_definer::visit( event_info const &si ) {
  emit_common( si );
}

void java_definer::visit( global_info const& ) {
  // nothing to do
}

void java_definer::visit( set_info const &si ) {
  emit_common( si );
  T_OUT << indent(2) << '}' T_ENDL;
}

void java_definer::visit( state_info const& ) {
  // nothing to do
}

void java_definer::visit( transition_info const& ) {
  // nothing to do
}

void java_definer::visit( user_event_info const &si ) {
  T_OUT << indent(4) << "super( event";
  if ( si.has_any_parameters() ) {
    //
    // emit ParamBlock constructor definition
    //
    // The formal parameter names can not have the same names as the data
    // members, so we prefix each one by a string, i.e.:
    //
    //      ParamBlock( T Pparam ) : param( Pparam ) { }
    //                    ^                 ^
    //
    // emit base event's ParamBlock initializer
    //
    if ( si.base_has_any_parameters() ) {
      T_OUT << param_list(
                INFO_CONST( user_event, si.sy_base_event_ ),
                param_data::EMIT_COMMA |
                param_data::EMIT_PREFIX |
                param_data::EMIT_ACTUAL
              );
    }
    T_OUT << " );" T_ENDL;
  }
  else {
    T_OUT << " );" T_ENDL;
  }
}

///////////////////////////////////////////////////////////////////////////////

java_initializer::java_initializer() : emitting_constructor_( false ) {
}

void java_initializer::emit() {
  // nothing to do
}

void java_initializer::emit_common( event_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << "( this, " << sy->name() << TRANSITIONS_SUFFIX << ", \"";

  if ( si.kind_ == event_info::KIND_USER )
    T_OUT << sy->name();
  else
    T_OUT << (si.kind_ == event_info::KIND_ENTER ? "enter" : "exit")
          << '(' << demangle( sy->name() + 1 ) << ')';

  T_OUT << "\", ";
}

void java_initializer::emit_common( state_info const &si ) {
  symbol const *const sy = si.get_symbol();
  char const *const m_name = mangle( sy->name() );

  //
  // If we're emitting a mem-initializer for the CHSM constructor, the
  // reference to the CHSM is *this; otherwise, we're emitting for a parent
  // class in which case the reference to the CHSM is the chsm_ data member.
  //
  char const *const chsm_ref = emitting_constructor_ ? "this" : "chsmM";

  T_OUT << "( " << chsm_ref << ", \"" << sy->name() << "\", "
        << (si.sy_parent_ ?
            //
            // The root state is "special" in that its children are not
            // lexically enclosed by it; hence, we have to emit "root" rather
            // than "this".
            //
            (si.sy_parent_ == SY_ROOT ? "root" : "this")
            :
            //
            // The root state is the only state with no parent.
            //
            "null"
          )
        << ", ";

  // enter/exit actions
  if ( si.action_.has_enter_ )
    T_OUT << "new Thunk_"
          << chsm_info::PREFIX_ENTER << chsm_info::PREFIX_ACTION
          << m_name << "()";
  else
    T_OUT << "null";
  T_OUT << ", ";
  if ( si.action_.has_exit_ )
    T_OUT << "new Thunk_"
          << chsm_info::PREFIX_EXIT << chsm_info::PREFIX_ACTION
          << m_name << "()";
  else
    T_OUT << "null";
  T_OUT << ", ";

  // enter/exit events
  if ( si.event_.has_enter_ )
    T_OUT << "((" << cc.sy_chsm_->name() << ')' << chsm_ref << ")."
          << chsm_info::PREFIX_ENTER << m_name;
  else
    T_OUT << "null";
  T_OUT << ", ";
  if ( si.event_.has_exit_ )
    T_OUT << "( (" << cc.sy_chsm_->name() << ')' << chsm_ref << " )."
          << chsm_info::PREFIX_EXIT << m_name;
  else
    T_OUT << "null";
}

void java_initializer::visit( child_info const& ) {
  // nothing to do
}

void java_initializer::visit( chsm_info const& ) {
  // nothing to do
}

void java_initializer::visit( cluster_info const &si ) {
  symbol const *const sy = si.get_symbol();
  char const *const base_name = state_base_name( sy->name() );

  T_OUT << indent(2) << base_name << " = new "
        << STATE_CLASS_PREFIX << base_name;
  emit_common( si );
  T_OUT << ", " << (si.history_ ? "true" : "false") << " );" T_ENDL;
}

void java_initializer::visit( event_info const &si ) {
  symbol const *const sy = si.get_symbol();
  T_OUT << indent(2) << sy->name() << " = new " << lib_class_name( si );
  emit_common( si );
  T_OUT << "null );" T_ENDL;
}

void java_initializer::visit( global_info const& ) {
  // nothing to do
}

void java_initializer::visit( set_info const &si ) {
  symbol const *const sy = si.get_symbol();
  char const *const base_name = state_base_name( sy->name() );

  T_OUT << indent(2) << base_name << " = new "
        << STATE_CLASS_PREFIX << base_name;
  emit_common( si );
  T_OUT << " );" T_ENDL;
}

void java_initializer::visit( state_info const &si ) {
  symbol const *const sy = si.get_symbol();
  T_OUT << indent(2) << state_base_name( sy->name() ) << " = new ";
  if ( &si == INFO_CONST(state, SY_ROOT) )
    T_OUT << "State_root";
  else
    T_OUT << lib_class_name( si );
  emit_common( si );
  T_OUT << " );" T_ENDL;
}

void java_initializer::visit( transition_info const& ) {
  // nothing to do
}

void java_initializer::visit( user_event_info const &si ) {
  symbol const *const sy = si.get_symbol();

  T_OUT << indent(2) << sy->name() << " = new "
        << sy->name() << EVENT_CLASS_SUFFIX;
  emit_common( si );

  if ( si.sy_base_event_ != nullptr )
    T_OUT << si.sy_base_event_->name();
  else
    T_OUT << "null";

  T_OUT << " );" T_ENDL;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
