/*
**      CHSM Language System
**      src/c++/chsm2c++/grammar.y
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

%{

// standard
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>                     /* for unlink(2) */

// local
#include "child.h"
#include "chsm.h"
#include "chsm_compiler.h"
#include "cluster.h"
#include "event.h"
#include "global.h"
#include "lex.h"
#include "mangle.h"
#include "manip.h"
#include "parent.h"
#include "set.h"
#include "state.h"
#include "transition.h"
#include "user_event.h"
#include "util.h"

using namespace PJL;
using namespace std;

//
// Although I usually frown on the use of NIL macros, it is necessary in this
// case since the PUSH() macro needs to be able to distinguish between a 0 as
// an integer and a 0 as a nil.
//
#define NIL     ( (void*)0 )

/*****************************************************************************
 *  Code-emission function(s)
 *****************************************************************************/

static void emit_chsm_declaration();
static void emit_chsm_definition();
static void emit_event_definitions();
void        emit_line_directive( std::ostream& );
static void emit_state_definitions();
static void emit_the_end();
static void emit_transitions();
inline void emit_user_line() { emit_line_directive( U_EMIT ); }

%}

/*****************************************************************************
 *  Tokens
 *****************************************************************************/

        /* keywords -- in alphabetical order */

%token  T_CHSM
%token  T_CLUSTER
%token  T_CONST_OR_VOLATILE
%token  T_DEEP
%token  T_ENTER
%token  T_EVENT
%token  T_EXIT
%token  T_HISTORY
%token  T_IN
%token  T_IS
%token  T_SET
%token  T_STATE
%token  T_UPON

        /* punctuation */

%token  ',' ';'
%token  T_ARROW                     /* -> */
%token  T_DOTS                      /* \.+ */
%token  T_LCODE T_RCODE             /* %{ and %} used to delimit C++ code */
%token  T_PERCENT_PERCENT           /* %% used to delimit sections */
%token  '$'                         /* used in embedded C++ code */
%right  T_PTR_OPERATOR              /* * and & used only in argument decl. */
%left   T_COLON_COLON_STAR          /* ::* used only in argument decl. */
%left   '[' ']'
%left   T_LOWER_THAN_COLON_COLON    /* bogus: used to eliminate shift/reduce */
%left   T_COLON_COLON               /* :: */
%token  '(' ')' '{' '}' '<' '>'

        /* other */

%token  T_IDENT
%token  T_SUBSTATES                 /* (\.s)+ */
%token  T_ERROR

%%

/*****************************************************************************
 *  Preamble
 *****************************************************************************/
/*
    There is a notation used at the beginning of some productions, e.g.:

        state_sym -> ---

    The stuff on the left of the -> is what is expected to already exist on the
    semantic stack; the stuff on the right is what is left on the semantic
    stack after the production completes.  What may be on the stack to the left
    of the left-most item is irrelevant with respect to the current production.
    A "---" means "empty stack."

    Additionally:

        Items ending in:    Are of type:
        ---------------     -----------
        _sym                symbol*
        ?                   bool
        (nothing)           int (usually)

    Productions not having any of the above notation don't use the semantic
    stack at all.
*/

/*****************************************************************************
 *  CHSM and descriptions
 *****************************************************************************/

chsm
    :   T_PERCENT_PERCENT
        chsm_description {
            g.check_states_defined();
            g.check_events_used();
            g.check_transitions();
            /*
            ** If there were no errors, emit the C++ code.
            */
            if ( !g.source_->errors_ ) {
                g.backpatch_enter_exit_events();

                /*
                ** emit declaration file
                */
                emit_chsm_declaration();

                /*
                ** Switch from emitting the declaration file to emitting the
                ** definition file only if we're not emitting to stdout.
                */
                if ( !g.options_.to_stdout_ ) {
                    emit_the_end();
                    EMIT << "#endif" ENDL;
                    string const tmp = g.target_->name_;
                    delete g.target_;
                    g.target_ = new compiler::target( g.definition_file_name_ );
                    EMIT    << compiler::ident_stamp ENDL
                            ENDL
                            << "#include \"" << tmp << '"' ENDL
                            ENDL;
                }
                EMIT    << "#include <new>" ENDL
                        ENDL;
                emit_state_definitions();
                emit_event_definitions();
                emit_transitions();
                emit_chsm_definition();
                g.user_code_->emit( EMIT );
                emit_line_directive( EMIT );
                emit_the_end();
            } else {
                /*
                ** There were errors: blow away target file, if any.
                */
                if ( !g.options_.to_stdout_ )
                    ::unlink( g.target_->name_.c_str() );
            }
        }
        opt_the_end
    |   error {
            g.parse_error( "no \"%%\" ever encountered" );
        }
    ;
chsm_description
        /*
        ** derived_class(char*) chsm_sym -> ---
        */
    :   T_CHSM opt_derived_class_spec identifier {
            POP_SYMBOL( ident );
            POP_STRING( derived );
            g.chsm_ = ident;
            g.chsm_->info( new chsm_info(
                /*
                ** Place "root" in the symbol table so the user can't declare
                ** states of the same name.
                */
                &g.sym_table_[ "root" ], derived
            ) );
            PUSH_SYMBOL( g.chsm_ );
            PUSH( static_cast<param_data*>( CHSM ) );
        }
        opt_argument_list {
            POP_PTR( param_data, pd );
            if ( pd->param_list_.size() && CHSM->derived_.empty() ) {
                g.source_->error()
                    << "parameters allowed only for derived CHSMs\n";
            }
            POP_SYMBOL( junk );
            PUSH_SYMBOL( ROOT_SYM );
        }
        opt_history {
            POP_TYPE( bool, history );
            /*
            ** The states in the outer-most scope of a CHSM have cluster
            ** semantics.
            */
            ROOT_SYM->info( new cluster_info( 0, 0, history ) );
        }
        opt_enter_exit_action_spec body
    |   T_CHSM error {
            g.parse_error( "chsm name expected" );
        }
    |   error {
            g.parse_error( "\"chsm\" expected" );
        }
    ;
opt_enter_exit_action_spec
    :   /* empty */
    |   '{' opt_enter_exit_action_list '}' {
            yyerrok;
        }
    ;

body
        /*
        ** new_parent_sym -> ---
        */
    :   T_IS lbrace_expected {
            POP_SYMBOL( new_parent );
            PUSH_SYMBOL( g.parent_ );
            g.parent_ = new_parent;
            g.sym_table_.open_scope();
        }
        opt_description_list
        '}' {
            g.check_child_states_defined();
            g.sym_table_.close_scope();

            if ( g.parent_ == g.outer_deep_ ) {
                /*
                ** This is the outer-most cluster that specified a deep
                ** history; it's going out of scope, so we have to turn off
                ** "deepness."
                */
                g.outer_deep_ = 0;
            }
            POP_SYMBOL( old_parent );
            g.parent_ = old_parent;
        }
    |   error {
            g.parse_error( "\"is\" expected" );
        }
    ;

opt_description_list
    :   /* empty */
    |   opt_description_list description
    ;
description
    :   state_description
    |   cluster_description
    |   set_description
    |   event_description
    |   error {
            g.parse_error( "description expected" );
        }
    ;

opt_the_end
    :   /* empty */
    |   T_PERCENT_PERCENT
    ;

/*****************************************************************************
 *  state, cluster, and set descriptions
 *****************************************************************************/

state_description
        /*
        ** derived_class(char*) state_sym -> ---
        */
    :   T_STATE state_declaration {
            POP_SYMBOL( state );
            POP_STRING( derived );
            if ( g.not_exists( state ) )
                state->info( new state_info( g.parent_, derived ) );
            PUSH_SYMBOL( state );
        }
        state_transition_spec {
            POP_SYMBOL( state );
        }
    ;

cluster_description
        /*
        ** derived_class(char*) cluster_sym history? -> ---
        */
    :   T_CLUSTER state_declaration child_declaration opt_history {
            POP_TYPE( bool, history );
            POP_SYMBOL( cluster );
            POP_STRING( derived );
            if ( g.not_exists( cluster ) )
                cluster->info( new cluster_info(
                    g.parent_, derived,
                    /*
                    ** This cluster has a history either if a history was
                    ** explicity specified for it or it is nested inside a
                    ** cluster that has a deep history.
                    */
                    history || g.outer_deep_
                ) );
            PUSH_SYMBOL( cluster );
        }
        opt_parent_transition_spec body
    ;
opt_history
        /*
        ** -> history?
        */
    :   /* empty */ {
            PUSH( false );
        }
    |   opt_deep T_HISTORY {
            PUSH( true );
        }
    ;
opt_deep
        /*
        ** cluster_sym -> cluster_sym
        */
    :   /* empty */
    |   T_DEEP {
            if ( !g.outer_deep_ ) {
                TOP_SYMBOL( cluster );
                /*
                ** Remember this cluster as being the outermost one that
                ** specified a deep history so that we can turn off the
                ** "deepness" when this cluster goes out of scope.
                */
                g.outer_deep_ = cluster;
            }
        }
    ;

set_description
        /*
        ** derived_class(char*) set_sym -> ---
        */
    :   T_SET state_declaration child_declaration {
            POP_SYMBOL( set );
            POP_STRING( derived );
            if ( g.not_exists( set ) )
                set->info( new set_info( g.parent_, derived ) );
            PUSH_SYMBOL( set );
        }
        opt_parent_transition_spec body
    ;

/*****************************************************************************
 *  state declaration
 *****************************************************************************/

state_declaration
        /*
        ** ident_sym -> state_sym
        */
    :   opt_derived_class_spec identifier {
            POP_SYMBOL( ident );
            string name;
            if ( g.parent_ != ROOT_SYM ) {
                name = g.parent_->name();
                name += string( "." ) + ident->name();
                /*
                ** Make sure the ident was declared as a child and that it's a
                ** child of the current parent.
                */
                if ( type_check(
                        ident, TYPE(child), must_exist, "in state declaration"
                     )
                ) {
                    child_info *const child = INFO( child, ident );
                    if ( child->parent_ == g.parent_ ) {
                        /*
                        ** This child is a child of the current parent: Mark
                        ** this declared child as having been defined.
                        */
                        child->defined_ = true;
                    } else {
                        g.source_->error()
                            << type_string( ident )
                            << " \"" << ident->name()
                            << "\" is not a child of \""
                            << g.parent_->name() << "\"\n";
                    }
                }
            } else {
                /*
                ** We're at scope-level zero.
                */
                name = ident->name();
            }

            symbol &new_state = g.sym_table_[ name ];

            if ( type_of( new_state ) & TYPE(global) ) {
                /*
                ** This state was forward-referenced; since it's about to get
                ** its real info, delete the dummy.
                */
                new_state.delete_info();
            }
            /*
            ** Add the state to the queue of states to remember the
            ** declaration order (if we haven't seen it before).
            */
            if ( !new_state.info() ) {
                CHSM->state_q_.push_back( &new_state );
                if ( type_of( g.parent_ ) & TYPE(parent) ) {
                    /*
                    ** This symbol was already typechecked and, if of the wrong
                    ** type, an error message was issued; but, we still have to
                    ** check here so we don't try to use a symbol of the wrong
                    ** type.
                    **
                    ** If it really is a parent, add this child to its parent's
                    ** list of child states.
                    */
                    INFO( parent, g.parent_ )->
                        children_.push_back( &new_state );
                }
            }
            PUSH_SYMBOL( &new_state );
        }
    |   opt_derived_class_spec error {
            g.parse_error( "name expected in state declaration" );
            // clean up as best as possible
            PUSH_SYMBOL( &g.dummy_state );
        }
    ;

/*****************************************************************************
 *  child declaration
 *****************************************************************************/

child_declaration
    :   '(' child_list ')' {
            yyerrok;
        }
    |   error {
            g.parse_error( "child list expected after cluster/set name" );
        }
    ;
child_list
    :   child
    |   child_list comma_expected child
    ;
child
        /*
        ** parent_sym child_sym -> parent_sym
        */
    :   identifier {
            POP_SYMBOL( child );
            TOP_SYMBOL( parent );
            /*
            ** Check to see that the symbol does not exist as an event; check
            ** for multiply-declared child.
            */
            if ( g.not_exists( child, TYPE(event) )
                 && type_of( child ) & TYPE(child)
                 && INFO_CONST( child, child )->parent_ == parent
            ) {
                g.source_->error()
                    << type_string( child ) << " \"" << child->name()
                    << "\" previously declared\n";
            } else
                child->info( new child_info( parent ) );
        }
    |   error {
            g.parse_error( "child name expected (extra ','?)" );
        }
    ;

/*****************************************************************************
 *  event descriptions
 *****************************************************************************/

event_description
    :   T_EVENT opt_base_event_spec identifier {
            POP_SYMBOL( event );
            POP_SYMBOL( base_event );
            if ( g.not_exists( event ) ) {
                event->info( new user_event_info( base_event ) );
                CHSM->event_q_.push_back( event );
            }
            PUSH( event );
            PUSH( (param_data*)INFO( user_event, event ) );
            /*
            ** Let the lexical analyzer know that we're doing conditions.
            */
            g_lexer.push_state( lexer::condition_processing );
        }
        opt_argument_list opt_event_precondition semi_expected {
            yyerrok;
            g_lexer.pop_state();
            POP_PTR( param_data, pd );
            POP_SYMBOL( event );
        }
    |   T_EVENT error ';' {
            yyerrok;
            g.parse_error( "event name expected" );
        }
    ;

opt_base_event_spec
        /*
        ** -> event_sym
        */
    :   /* empty */ {
            PUSH_SYMBOL( NIL );
        }
    |   '<' base_event '>'
    |   '<' base_event error '>' {
            yyerrok;
            g.parse_error( "error in base event specification" );
        }
    |   '<' error '>' {
            yyerrok;
            g.parse_error( "base event name expected" );
            PUSH_SYMBOL( NIL );
        }
    ;
base_event
        /*
        ** -> event_sym
        */
    :   identifier {
            TOP_SYMBOL( ident );
            /*
            ** Check that the event specified for the base event has been
            ** declared.
            */
            type_check(
                ident, TYPE(user_event), must_exist,
                "in base event specification"
            );
        }
    ;

opt_event_precondition
    :   /* empty */
    |   event_precondition_expr
    |   event_precondition_func
    ;
event_precondition_expr
        /*
        ** event_sym param_data* -> event_sym param_data*
        */
    :   '[' {
            PEEK_SYMBOL( event, 1 );
            user_event_info *const i = INFO( user_event, event );
            i->precondition_ = user_event_info::expr_precondition;
            /*
            ** The C++ function generated for a precondition expression is the
            ** && of the base event's precondition function and the C++
            ** expression the user specified.
            **
            ** Note: The user's expression must be wrapped in ()'s so the
            ** resulting C++ expression has the proper precedence and is
            ** evaluated correctly.
            */
            U_EMIT  << "bool " << g.chsm_->name() << "::"
                    << event->name()
                    << user_event_info::class_suffix_
                    << "::param_block::precondition() const {\n"
                       "\treturn base_param_block::precondition() && (\n";
            emit_user_line();
        }
        opt_embedded_state_name_list ']' {
            U_EMIT  << ");\n"
                    "}\n";
        }
    ;
event_precondition_func
        /*
        ** event_sym -> event_sym
        */
    :   T_LCODE {
            yyerrok;
            PEEK_SYMBOL( event, 1 );
            /*
            ** For a precondition function, the user's code is placed into a
            ** function unto itself.
            */
            U_EMIT  << "bool " << g.chsm_->name() << "::"
                    << event->name() << "_precondition( "
                    << param_list(
                        INFO( user_event, event ), param_data::emit_formal
                       )
                    << " ) const {\n";
            emit_user_line();
        }
        opt_embedded_state_name_list T_RCODE {
            PEEK_SYMBOL( event, 1 );
            user_event_info *const i = INFO( user_event, event );
            i->precondition_ = user_event_info::func_precondition;
            /*
            ** The C++ function generated for a precondition function is the &&
            ** of the base event's precondition function and the user's C++
            ** function we just generated above.
            */
            U_EMIT  << "}\n"
                        "bool " << g.chsm_->name() << "::"
                    << event->name() << user_event_info::class_suffix_
                    << "::param_block::precondition() const {\n"
                        "\treturn base_param_block::precondition() &&\n"
                        "\t\t((" << g.chsm_->name()
                    << "&)chsm())." << event->name() << "_precondition( "
                    << param_list(
                        INFO( user_event, event ), param_data::emit_actual
                       )
                    << " );\n"
                        "}\n";
        }
    ;

/*****************************************************************************
 *  transitions
 *****************************************************************************/

state_transition_spec
    :   transition_spec
    |   semi_expected                   /* for a "sink" state */
    ;
opt_parent_transition_spec
    :   /* empty */
    |   transition_spec
    ;
transition_spec
    :   '{' opt_enter_exit_action_list opt_transition_list '}' {
            yyerrok;
        }
    ;
opt_transition_list
    :   /* empty */
    |   opt_transition_list transition
    ;
transition
    :   /* empty */ {
            PUSH( 0 ); // number of events in event_condition_list
        }
        /*
        ** from_state_sym { event_sym condition_id }... num_events to_state_sym
        **      target_id action_id -> ---
        */
        event_condition_list transition_type semi_expected {
            yyerrok;
            POP_INT( action_id );
            POP_INT( target_id );
            POP_SYMBOL( to_state );
            POP_INT( num_events );
            PEEK_SYMBOL( from_state, 2 * num_events );

            while ( num_events-- ) {
                POP_INT( condition_id );
                CHSM->transition_q_.push_back(
                    new symbol( "",
                        new transition_info(
                            condition_id, from_state, to_state, target_id,
                            action_id
                        )
                    )
                );
                POP_SYMBOL( event );
                if ( !( type_of( event ) & TYPE(event) ) ) {
                    /*
                    ** This symbol was already typechecked and, if of the wrong
                    ** type, an error message was issued; but, we still have to
                    ** check here so we don't try to use a symbol of the wrong
                    ** type.
                    */
                    continue;
                }
                event_info &info = *INFO( event, event );
                info.transition_id_list_.push_back(
                    CHSM->transition_q_.size() - 1
                );
                info.used_ = true;
            }
        }
    ;
transition_type
        /*
        ** -> state_sym target_id action_id
        */
    :   T_ARROW {
            /*
            ** Let the lexical analyzer know that we're doing conditions.
            */
            g_lexer.push_state( lexer::condition_processing );
        }
        transition_target {
            g_lexer.pop_state();
        }
        opt_action_block
    |   action_block {
            POP_INT( action_id );
            PUSH_SYMBOL( NIL );         // no "to" state
            PUSH( 0 );                  // no target function
            PUSH( action_id );
        }
    ;
transition_target
        /*
        ** -> state_sym target_id
        */
    :   state_name {
            PUSH( 0 );                  // no target function
        }
    |   '[' {
            emit_user_line();
            ++CHSM->id_.target_;
            U_EMIT  << chsm_info::chsm_ns_alias_ << "::state* "
                    << g.chsm_->name() << "::" << chsm_info::target_prefix_
                    << CHSM->id_.target_ << "( " << chsm_info::chsm_ns_alias_
                    << "::event const &event ) { return ";
            PUSH_SYMBOL( NIL );         // no "to" state
            PUSH( CHSM->id_.target_ );
        }
        opt_embedded_state_name_list ']' {
            U_EMIT  << "; }\n";
        }
    |   error {
            g.parse_error( "state name or expression expected" );
        }
    ;

event_condition_list
        /*
        ** 0 -> { event_sym condition_id }... num_events
        */
    :   event_condition_spec
    |   event_condition_list comma_expected event_condition_spec {
            yyerrok;
        }
    ;
event_condition_spec
        /*
        ** num_events -> event_sym condition_id num_events+1
        */
    :   event_name {
            /*
            ** Let the lexical analyzer know that we're doing conditions.
            */
            g_lexer.push_state( lexer::condition_processing );
        }
        opt_event_condition {
            g_lexer.pop_state();
            POP_INT( condition_id );
            POP_SYMBOL( event );
            POP_INT( n );
            PUSH_SYMBOL( event );
            PUSH( condition_id );
            PUSH( n+1 );
        }
    |   error {
            g.parse_error( "event expected in transition" );
        }
    ;
opt_event_condition
        /*
        ** -> condition_id
        */
    :   /* empty */ {
            PUSH( 0 );                  // no condition
        }
    |   '[' {
            emit_user_line();
            ++CHSM->id_.condition_;
            U_EMIT  << "bool " << g.chsm_->name() << "::"
                    << chsm_info::condition_prefix_ << CHSM->id_.condition_
                    << "( " << chsm_info::chsm_ns_alias_
                    << "::event const &event ) { return ";
            PUSH( CHSM->id_.condition_ );
        }
        opt_embedded_state_name_list ']' {
            U_EMIT  << "; }\n";
        }
    ;

/*****************************************************************************
 *  enter/exit actions
 *****************************************************************************/

opt_enter_exit_action_list
    :   /* empty */
    |   opt_enter_exit_action_list enter_exit_action
    ;
enter_exit_action
        /*
        ** state_sym prefix(char*) -> state_sym
        */
    :   T_UPON enter_or_exit T_LCODE {
            yyerrok;
            POP_PTR( char, kind );
            TOP_SYMBOL( state );
            bool const is_enter = !::strcmp( kind, chsm_info::enter_prefix_ );
            state_info &info = *INFO( state, state );
            state_info::actions &action = info.action_;
            bool &b = is_enter ? action.enter_ : action.exit_;
            if ( !b ) {
                U_EMIT  << "void " << g.chsm_->name() << "::" << kind
                        << chsm_info::action_prefix_ << mangle( state->name() )
                        << "( " << chsm_info::chsm_ns_alias_
                        << "::state const &chsm_state_, "
                        << chsm_info::chsm_ns_alias_
                        << "::event const &event ) {\n";
                emit_user_line();
                U_EMIT  << '\t' << info.class_name() << " const &state = ("
                        << info.class_name() << " const&)chsm_state_;";
                b = true;
            } else {
                g.source_->error()
                    << ( is_enter ? "enter" : "exit" )
                    << " action already specified for "
                    << state->name() << '\n';
            }
        }
        opt_embedded_state_name_list T_RCODE {
            U_EMIT  << "}\n";
        }
    |   T_UPON error {
            g.parse_error( "\"enter\" or \"exit\" expected after \"upon\"" );
        }
        T_LCODE /* ... */ T_RCODE
    ;

/*****************************************************************************
 *  state names
 *****************************************************************************/

state_name
        /*
        ** depth ident_sym substates(char*) -> state_sym
        */
    :   opt_scope_resolver identifier opt_substates {
            POP_PTR( char, substates );
            POP_SYMBOL( ident );
            POP_INT( dots );
            string name;
            switch ( type_of( ident, dots ) ) {

                case TYPE(child):
                    /*
                    ** Easy case: it's a child of some previously-declared
                    ** state.
                    */
                    name = INFO_CONST( child, ident )->parent_->name();
                    name += ".";
                    name += ident->name();
                    break;

                default:
                    /*
                    ** Something weird...call type_check() to print an error
                    ** message.
                    */
                    type_check( ident, TYPE(state), must_exist, "", dots );
                    // no break;

                case TYPE(state):
                case TYPE(cluster):
                case TYPE(set):
                    /*
                    ** Previously-encountered state.
                    */
                    // no break;

                case TYPE(global):
                case TYPE(unknown):
                    /*
                    ** This is presumeably a forward reference to a state at
                    ** scope-level zero.
                    */
                    name = ident->name();
                    break;
            }
            /*
            ** Tack on the substates, if given.
            */
            name += substates;

            symbol &state = g.sym_table_[ name ];
            if ( !state.info() ) {
                /*
                ** This is presumeably a forward-reference to a state.  Give it
                ** dummy info just so it survives close_scope().
                */
                state.info( new global_info );
            }
            PUSH_SYMBOL( &state );
        }
    ;
opt_scope_resolver
        /*
        ** -> num_dots
        */
    :   /* empty */ {
            PUSH( 0 );
        }
    |   T_DOTS {
            int dots = ::strlen( g_lexer.text );
            if ( dots > symbol_table::scope() ) {
                g.source_->warning() << "dots exceed current scope-depth\n";
                dots = symbol_table::scope();
            }
            PUSH( dots );
        }
    |   T_COLON_COLON {
            PUSH( symbol_table::scope() );
        }
    ;
opt_substates
        /*
        ** -> substates(char*)
        */
    :   /* empty */ {
            PUSH_STRING( "" );
        }
    |   T_SUBSTATES {
            PUSH_STRING( g_lexer.text );
        }
    ;
paren_state_name
        /*
        ** -> state_sym
        */
    :   lparen_expected state_name rparen_expected
    |   '(' state_name_error ')' {
            yyerrok;
            // clean up as best as possible
            PUSH_SYMBOL( &g.dummy_state );
        }
    ;

/*****************************************************************************
 *  event names
 *****************************************************************************/

event_name
        /*
        ** prefix(char*) state_sym -> event_sym
        */
    :   user_event
    |   enter_or_exit paren_state_name {
            POP_SYMBOL( state );
            POP_PTR( char, kind );
            string name = kind;
            name += mangle( state->name() );
            symbol &event = g.sym_table_[ name ];
            if ( !event.info() ) {
                event.info(
                    new event_info(
                        ::strcmp( kind, chsm_info::enter_prefix_ ) ?
                            event_info::exit_state : event_info::enter_state,
                        state
                    )
                );
                CHSM->event_q_.push_back( &event );
            }
            PUSH_SYMBOL( &event );
        }
    ;
user_event
        /*
        ** -> event_sym
        */
    :   identifier {
            TOP_SYMBOL( event );
            if ( type_check( event, TYPE(user_event), no_info ) ) {
                event->info( new user_event_info );
                CHSM->event_q_.push_back( event );
            }
        }
    ;

/*****************************************************************************
 *  Argument lists
 *****************************************************************************/

opt_argument_list
        /*
        ** This part of the grammar to parse parameter declarations had the C++
        ** grammar in the ARM for argument declarations, sections 17.3 and
        ** 17.4, as its base.  There were several modifications and
        ** simplifications made to allow yacc to be able to parse it
        ** unambigously and independent of context.
        **
        ** One thing we impose is that parameter declarations may not be
        ** abstract since we need the variable name:
        **
        **      event alpha( int );     // illegal
        **      event alpha( int x );   // better
        */
    :   /* empty */
    |   '(' ')'
    |   '(' {
            g_lexer.push_state( lexer::arg_list_processing );
        }
        arg_decl_list ')' {
            g_lexer.pop_state();
        }
    ;

arg_cat
        /*
        ** During argument declaration list parsing, we have to reconstruct the
        ** text of the declaration so it can later be emitted into the
        ** resultant C++ code in user-event paramater declarations.  To do
        ** this, we insert a "subroutine" production, arg_cat, after every
        ** terminal to concatenate the text in g_lexer.text to the declaration
        ** we're reconstructing.
        */
    :   /* empty */ {
            g.arg_.cat( g_lexer.text );
            g.arg_.cat( ' ' );
        }
    ;
arg_cat_ident
        /*
        ** However, we want to extract the identifier of the variable name and
        ** substitute a '$' marker in its place.  (See the comment in
        ** user_event_info::definition() for why we need to do this.)  Hence,
        ** when we're all done, for a declaration such as:
        **
        **      T (*const foo)( U )
        **
        ** we'll get:
        **
        **      declaration: T (*const $)( U )
        **      variable   : foo
        **
        ** To do this, we can't call arc_cat immediately upon getting a T_IDENT
        ** because we can't tell at that level if it's the identifier for the
        ** variable or not.  Instead, we store the text for a T_IDENT someplace
        ** used only for T_IDENTs that we can concatenate later further along
        ** in a production.
        */
    :   /* empty */ {
            char *const s = g.arg_.ident_[ g.arg_.got_variable_ ];
            if ( *s ) {
                g.arg_.cat( s );
                g.arg_.cat( ' ' );
                *s = '\0';
            }
        }
    ;

arg_decl_list
    :   arg_decl
    |   arg_decl_list ',' arg_cat arg_decl
    ;
arg_decl
        /*
        ** param_data -> param_data
        */
    :   {
            g.arg_.init( g.source_->line_no_ );
        }
        decl_specifiers declarator {
            TOP_PTR( param_data, pd );
            pd->param_list_.push_back(
                param_data::param(
                    g.arg_.line_no_, g.arg_.decl_, g.arg_.ident_[ 0 ]
                )
            );
        }
    ;

decl_specifiers
    :   decl_specifier
    |   decl_specifiers decl_specifier
    ;
decl_specifier
    :   complete_class_name
    |   T_CONST_OR_VOLATILE arg_cat
    ;

complete_class_name
    :   qualified_class_name arg_cat_ident
    |   T_COLON_COLON arg_cat qualified_class_name arg_cat_ident
    |   T_COLON_COLON error {
            g.parse_error( "class name expected after \"::\"" );
        }
    ;
qualified_class_name
        /*
        ** Set explicit precedence to eliminate a shift/reduce conflict.  This
        ** case parallels the classic if/else conflict.
        **
        ** See: "lex & yacc" 2nd edition, O'Reilly & Associates, Inc., ISBN
        ** 1-56592-000-7, Oct. 1992, p. 234.
        */
    :   class_name %prec T_LOWER_THAN_COLON_COLON
    |   class_name T_COLON_COLON arg_cat_ident {
            /*
            ** Must explicitly append "::" rather than call arg_cat since the
            ** lexer's text is the _next_ token due to lookahead.
            */
            g.arg_.cat( "::" );
        }
        qualified_class_name
    |   class_name T_COLON_COLON error {
            g.parse_error( "class name expected after \"::\"" );
        }
    ;
class_name
    :   decl_ident opt_template_args
    ;
decl_ident
        /*
        ** As mentioned above in the comment for arc_cat_ident, store the text
        ** of a T_IDENT seperately.  Once we have the T_IDENT for the variable
        ** in g.arg_.ident_[ 0 ], store all subsequent text for T_IDENTs in
        ** g.arg_.ident_[ 1 ] so as not to overwrite it.
        */
    :   T_IDENT {
            ::strcpy( g.arg_.ident_[ g.arg_.got_variable_ ], g_lexer.text );
        }
    ;
opt_template_args
        /*
        ** Since the identifier for the variable name can not possibly be
        ** between < and > template arguments, let lex reduce all that down to
        ** a single token thus simplifying the grammar.
        */
    :   /* empty */
    |   '<' arg_cat_ident arg_cat /* anything */ '>' arg_cat
    ;

declarator
        /*
        ** Nested declarators:
        **      T (d)                   // silly, but legal
        **      T (*d)( ... )           // pointer to function
        **
        ** Note: we do not allow:
        **
        **      T d()                   // function declaration
        */
    :   '(' arg_cat declarator ')' arg_cat opt_sub_arg_decl_list

        /*
        ** Vector declarations:
        **      T d[ ... ]
        **
        ** Similarly to < and > for template arguments, the identifier for the
        ** variable name can not possibly be between [ and ] vector dimensions;
        ** therefore, let lex reduce all that down to a single token thus
        ** simplifying the grammar.
        */
    |   declarator '[' arg_cat /* anything */ ']' arg_cat

        /*
        ** Pointers and references:
        **      T *d
        **      T &d
        */
    |   T_PTR_OPERATOR arg_cat opt_cv_qualifier_list declarator

        /*
        ** Pointers to member:
        **      T C::*d
        **
        ** Note: We cheat by using a single token for this ::{white}*\* to
        ** eliminate a shift/reduce conflict.  In C++, :: and * are separate
        ** tokens.  Allowing whitspace between :: and * should cover all cases
        ** (we hope).
        */
    |   complete_class_name arg_cat_ident T_COLON_COLON_STAR arg_cat
            opt_cv_qualifier_list declarator

        /*
        ** Variable name.
        */
    |   decl_ident {
            if ( !g.arg_.got_variable_ ) {
                g.arg_.got_variable_ = true;
                g.arg_.cat( '$' );
            }
        }
    |   error {
            g.parse_error( "variable name expected in argument declaration" );
        }
    ;

opt_sub_arg_decl_list
    :   /* empty */
    |   '(' arg_cat opt_abstract_arg_decl_list ')' arg_cat
            opt_cv_qualifier_list
    ;
opt_abstract_arg_decl_list
    :   /* empty */
    |   abstract_arg_decl_list
    ;
abstract_arg_decl_list
    :   abstract_arg_decl
    |   abstract_arg_decl_list ',' arg_cat abstract_arg_decl
    ;
abstract_arg_decl
    :   decl_specifiers abstract_declarator
    ;

abstract_declarator
        /*
        ** abstract_declarator is basically the same as declarator except that
        ** it allows a variable name to be omitted, hence abstract.
        */
    :   /* empty */
    |   '(' arg_cat abstract_declarator ')' arg_cat
    |   abstract_declarator '[' arg_cat /* anything */ ']' arg_cat
    |   T_PTR_OPERATOR arg_cat
            opt_cv_qualifier_list abstract_declarator
    |   complete_class_name T_COLON_COLON_STAR arg_cat
            opt_cv_qualifier_list abstract_declarator
    ;

opt_cv_qualifier_list
    :   /* empty */
    |   cv_qualifier_list
    ;
cv_qualifier_list
    :   T_CONST_OR_VOLATILE arg_cat
    |   cv_qualifier_list T_CONST_OR_VOLATILE arg_cat
    ;

/*****************************************************************************
 *  Miscellaneous
 *****************************************************************************/

identifier
        /*
        ** -> ident_sym
        */
    :   T_IDENT {
            PUSH_SYMBOL( &g.sym_table_[ g_lexer.text ] );
        }
    ;

opt_derived_class_spec
        /*
        ** -> derived_class(char*)
        */
    :   /* empty */ {
            PUSH_STRING( 0 );
        }
    |   '<' {
            g.arg_.init( g.source_->line_no_ );
            g_lexer.push_state( lexer::arg_list_processing );
        }
        complete_class_name '>' {
            g_lexer.pop_state();
            PUSH_STRING( g.arg_.decl_ );
        }
    |   '<' error '>' {
            yyerrok;
            g.parse_error( "derived class expected" );
            PUSH_STRING( 0 );
        }
    ;

opt_embedded_state_name_list
    :   /* empty */
    |   opt_embedded_state_name_list embedded_state_name
    ;
embedded_state_name
        /*
        ** state_sym -> ---
        */
    :   '$' '{' state_name rbrace_expected {
            POP_SYMBOL( state );
            U_EMIT << state->name();
        }
    |   '$' '{' state_name_error '}' {
            yyerrok;
        }
    |   '$' embedded_keyword
        lparen_expected state_name rparen_expected {
            POP_SYMBOL( state );
            POP_INT( keyword );
            switch ( keyword ) {
                case T_ENTER:
                    POP_PTR( char, kind );
                    U_EMIT << kind;
                    // no break;
                case T_STATE:
                    U_EMIT << mangle( state->name() );
                    break;
                case T_IN:
                    U_EMIT << state->name() << ".active()";
                    break;
            }
        }
    |   '$' embedded_keyword '(' state_name_error ')' {
            yyerrok;
            POP_INT( junk );
        }
    ;
embedded_keyword
        /*
        ** -> keyword
        */
    :   enter_or_exit   { PUSH( T_ENTER ); }
    |   T_IN            { PUSH( T_IN ); }
    |   error {
            g.parse_error( "invalid keyword after '$'" );
            PUSH( 0 );
        }
    ;

opt_action_block
        /*
        ** -> action_id
        */
    :   /* empty */ {
            PUSH( 0 );                  // no block
        }
    |   action_block
    ;
action_block
        /*
        ** -> action_id
        */
    :   T_LCODE {
            emit_user_line();
            ++CHSM->id_.action_;
            U_EMIT  << "void " << g.chsm_->name() << "::"
                    << chsm_info::action_prefix_ << CHSM->id_.action_
                    << "( " << chsm_info::chsm_ns_alias_
                    << "::event const &event ) {";
            PUSH( CHSM->id_.action_ );
        }
        opt_embedded_state_name_list T_RCODE {
            U_EMIT  << "}\n";
        }
    ;

enter_or_exit
        /*
        ** -> prefix(char*)
        */
    :   T_ENTER {
            PUSH_STRING( chsm_info::enter_prefix_ );
        }
    |   T_EXIT {
            PUSH_STRING( chsm_info::exit_prefix_ );
        }
    ;

/*****************************************************************************
 *  Errors
 *****************************************************************************/

comma_expected
    :   ','
    |   error { g.parse_error( "',' expected" ); }
    ;
semi_expected
    :   ';'
    |   error { g.parse_error( "';' expected" ); }
    ;
lparen_expected
    :   '('
    |   error { g.parse_error( "'(' expected" ); }
    ;
rparen_expected
    :   ')'
    |   error { g.parse_error( "')' expected" ); }
    ;
lbrace_expected
    :   '{'
    |   error { g.parse_error( "'{' expected" ); }
    ;
rbrace_expected
    :   '}'
    |   error { g.parse_error( "'}' expected" ); }
    ;
state_name_error
    :   error { g.parse_error( "state name expected" ); }
    ;
%%

/*****************************************************************************
 *  Emitting routine(s)
 *****************************************************************************/

static void emit_chsm_declaration() {
    EMIT    << "/***** CHSM class declaration *****/" ENDL
            ENDL
            << declaration( CHSM ) ENDL;
}

//*****************************************************************************

static void emit_chsm_definition() {
    EMIT    << "/***** CHSM constructor definition *****/" ENDL
            ENDL
            << definition( CHSM ) ENDL;
}

//*****************************************************************************

static void emit_event_definitions() {
    param_data::default_emit_flags_ = param_data::emit_formal;

    EMIT    << "/***** event definitions *****/" ENDL
            ENDL;

    FOR_EACH( chsm_info::event_queue, CHSM->event_q_, event )
        EMIT << definition( INFO_CONST( event, *event ) ) ENDL;
}

//*****************************************************************************

void emit_line_directive( ostream &o ) {
    if ( !g.options_.no_line_ )
        o   << "#line " << g.source_->line_no_
            << " \"" << g.source_->name_ << "\"\n";
}

//*****************************************************************************

static void emit_state_definitions() {
    EMIT    << "/***** state definitions *****/" ENDL
            ENDL
            << definition( INFO_CONST( parent, ROOT_SYM ) ) ENDL;

    FOR_EACH( chsm_info::state_queue, CHSM->state_q_, state )
        if ( type_of( state ) & TYPE(parent) )
            EMIT << definition( INFO_CONST( parent, *state ) ) ENDL;
}

//*****************************************************************************

static void emit_the_end() {
    EMIT    ENDL
            << "/***** THE END *****/" ENDL;
}

//*****************************************************************************

static void emit_transitions() {
    EMIT    << "/***** transitions *****/" ENDL
            ENDL
            << chsm_info::chsm_ns_alias_ << "::transition const "
            << g.chsm_->name() << "::transition_[] = {" ENDL;

    FOR_EACH( chsm_info::transition_queue, CHSM->transition_q_, t )
        EMIT << definition( INFO_CONST( transition, *t ) ) ENDL;

    EMIT    << "\t0" ENDL
            << "};" ENDL
            ENDL;
}
/* vim:set et ts=4 sw=4: */
