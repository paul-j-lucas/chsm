/*
**      CHSM Language System
**      chsm.h -- Run-Time library declarations
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

#ifndef chsm_H
#define chsm_H

/**
 * @mainpage CHSM Language System for C++ API Specification
 *
 * Provides classes for implementing the run-time of the
 * <a href="http://chsm.sourceforge.net/" target="_top">
 * Concurrent Hierarchical State Machine Language System</a>
 * for C++.
 *
 * @section rel_doc Related Documentation
 *  + <a href="http://chsm.sourceforge.net/documentation.html" target="_top">Unix manual pages</a>.</li>
 *  + <a href="http://chsm.sourceforge.net/examples/" target="_top">Examples</a>.</li>
 *
 * @authors Paul J. Lucas
 *      and Fabio Riccardi
 */

/**
 * @file
 * Provides classes for implementing the run-time of the
 * <a href="http://chsm.sourceforge.net/" target="_top">
 * Concurrent Hierarchical State Machine Language System</a>
 * for C++.
 *
 * @authors Paul J. Lucas
 *      and Fabio Riccardi
 */

// standard
#include <deque>
#include <iostream>
#include <mutex>

/**
 * Defines the long CHSM namespace name.  This shouldn't ever conflict with
 * anything.  See the end of this file for more on namespaces.
 */
#define CHSM_NS Concurrent_Hierarchical_State_Machine

namespace CHSM_NS {

///////////////////////////////////////////////////////////////////////////////

typedef std::recursive_mutex mutex_type;
typedef std::unique_lock<mutex_type> lock_type;

class   machine;
class   state;
class   parent;
class   cluster;
class   set;
class   event;
struct  transition;

// macros to aid in argument-lists
#define CHSM_FORMAL(X)  X
#define CHSM_ACTUAL(X)  /* nothing */

/**
 * A %state is the simplest kind of state in a machine: it has no
 * child states.  A %state can either be active (in the state) or inactive (not
 * in the state); a state may have a parent, and enter/exit actions.
 *
 * The %state class can be extended either to add additional data members and
 * member functions, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
class state {
public:
  //
  // The constructor arguments must have horribly long names to avoid a name
  // collision with user-defined state names in the mem-initializers in the
  // resultant C++ code.  We use macros to make life easier.
  //
# define  CHSM_STATE_ARG_LIST(A)                        \
          A(CHSM_NS::machine&) chsm_machine_,           \
          A(char const*) chsm_name_,                    \
          A(CHSM_NS::parent*) chsm_parent_,             \
          A(CHSM_NS::state::action) chsm_enter_action_, \
          A(CHSM_NS::state::action) chsm_exit_action_,  \
          A(CHSM_NS::event*) chsm_enter_event_,         \
          A(CHSM_NS::event*) chsm_exit_event_

  /**
   * Defines the constructor arguments for the CHSM::state class.
   *
   * @hideinitializer
   */
# define  CHSM_STATE_ARGS CHSM_STATE_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::state class.
   *
   * @hideinitializer
   */
# define  CHSM_STATE_INIT CHSM_STATE_ARG_LIST(CHSM_ACTUAL)

  /**
   * The identification number of a %state.
   */
  typedef int id;

  /**
   * An \e action is a function (machine member function) that is optionally
   * called upon enter/exit of a %state.
   *
   * @param s The %state that is being entered or exited.
   * @param trigger The event that triggered the transition.
   */
  typedef void (machine::*action)( state const &s, event const &trigger );

  /**
   * Constructs a %state.
   *
   * When deriving a class from %state, the macros `CHSM_STATE_ARGS` and
   * `CHSM_STATE_INIT` can be used to avoid having to deal with the many
   * constructor arguments, e.g.:
   * @code
   *  class place : public CHSM::state {
   *  public:
   *    place( CHSM_STATE_ARGS ) : CHSM::state( CHSM_STATE_INIT ) {
   *      // ...
   *    }
   *    // ...
   *  };
   * @endcode
   */
  state( CHSM_STATE_ARGS );

  /**
   * Destroys a state.
   */
  ~state();

  /**
   * Gets whether this %state is active.
   *
   * @return Returns `true` only if the %state is active.
   */
  bool active() const {
    return (state_ & STATE_ACTIVE) != 0;
  }

  /**
   * Gets the `machine` that this %state belongs to.
   *
   * @return Returns said `machine`.
   */
  machine& chsm() const {
    return machine_;
  }

  /**
   * Clears the history for all child clusters of a cluster, recursively.
   * Plain states have neither children nor a history.
   *
   * @note Placing this function here is a wart on the design; by having it
   * here, however, run-time type-identification can be avoided.
   */
  virtual void deep_clear();

  /**
   * Returns the name of the %state.
   *
   * @return Returns said name.
   */
  char const* name() const {
    return name_;
  }

  /**
   * Returns the parent cluster or set.
   *
   * @return Returns the parent or null if none.
   */
  parent* parent_of() const {
    return parent_;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
  /**
   * @internal
   *
   * The CHSM::machine the %state belongs to.
   */
  machine &machine_;

  std::ostream& dout() const;

  /**
   * Enters a %state.
   *
   * If this %state's parent %state (if any) isn't active, enter it first.  The
   * event <code>$enter(</code><i>state</i><code>)</code> is broadcast and the
   * %state's \e enter-action block is executed, if any.
   *
   * When overriding in a derived class, the form \e must be:
   * @code
   *  bool enter( CHSM::event const &amp;trigger, CHSM::state *from_child * ) {
   *    if ( !CHSM::state::enter( trigger, from_child ) )
   *      return false;
   *    // ... new functionality here ...
   *    return true;
   * }
   * @endcode
   *
   * @param trigger The event that triggered the transition.
   * @param from_child Not used here.
   * @return Returns `true` only if the %state was actually entered, i.e., it
   * wasn't already active.
   */
  virtual bool enter( event const &trigger, state *from_child = nullptr );

  /**
   * Exits a %state.
   *
   * If the \a to %state doesn't have this %%state's parent state (if any) as
   * an ancestor, exits this %state's parent %state first.  The event
   * <code>$exit(</code><i>state</i><code>)</code> is broadcast and the
   * %state's \e exit-action block is executed, if any.
   *
   * When overriding in a derived class, the form \e must be:
   * @code
   *  bool exit( CHSM::event const &amp;trigger, CHSM::state *to ) {
   *    if ( !CHSM::state::exit( trigger, to ) )
   *      return false;
   *    // ... new functionality here ...
   *    return true;
   *  }
   * @endcode
   *
   * @param trigger The event that triggered the transition.
   * @param to If not null, the %state to transition to.
   * @return Returns `true` only if the %state was actually exited, i.e., it
   * wasn't already active.
   */
  virtual bool exit( event const &trigger, state *to = nullptr );

private:
  state( state const& ) = delete;
  state& operator=( state const& ) = delete;

  char const  *const name_;             ///< State name.
  parent      *const parent_;           ///< Parent, if any.

  static unsigned const STATE_INACTIVE        = 0x00;
  static unsigned const STATE_ACTIVE          = 0x01;
  //
  // STATE_ACTIVE_DISABLED is used to prevent making more than one
  // nondeterministic transition from the same state.  See the comments in
  // `event.cpp` for more information.
  //
  static unsigned const STATE_ACTIVE_DISABLED = 0x02 | STATE_ACTIVE;

  unsigned state_;                      ///< The %state of the %state.

  /**
   * See the comment for the "action" declaration.
   */
  action const enter_action_, exit_action_;

  /**
   * The enter/exit events for the %state are non-null only if they are
   * actually used in the machine.  Their values are determined by the CHSM-to-
   * C++ compiler.
   */
  event *const enter_event_, *const exit_event_;

  friend class cluster;
  friend class event;
  friend class machine;
  friend class parent;
  friend class set;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %transition is a simple `struct` containing information for a transition
 * from one state to another in a machine.  The information contains whether
 * the transition has an associated condition to test prior to taking it, the
 * "from" and "to" states, and an optional action to be performed.
 *
 * All the data members ideally should be declared `const`.  In the code
 * emitted by the CHSM-to-C++ compiler, we want to be able to use aggregate
 * initialization, but C++ does not permit aggregates to have `const` data
 * members (see section 8.5.1 of the ANSI/ISO C++ reference manual), so we
 * leave the `const` out.  However, the code emitted declares the aggregate as
 * a whole to be `const`, so it amounts to the same thing.
 *
 * @author Paul J. Lucas
 */
struct transition {
  /**
   * The identification number of a %transition.
   */
  typedef int id;

  /**
   * A condition is a function to be evaluated prior to a transition being
   * performed.  The transition is performed only if the function returns
   * `true`.
   *
   * @param trigger The event that is causing the transition.
   * @return Returns `true` only if the condition evaluates to `true`.
   */
  typedef bool (machine::*condition)( event const &trigger );

  /**
   * An action is a function to be executed upon performing a transition from
   * one state to another.
   *
   * @param trigger The event that caused the %transition.
   */
  typedef void (machine::*action)( event const &trigger );

  /**
   * A target is a function to be evaluated prior to a transition being
   * performed to determine the state to transition to.
   *
   * @param trigger The event that caused the transition.
   * @return Returns the state that the transition should go to or null to
   * abort the transition.
   */
  typedef state* (machine::*target)( event const &trigger );

  /**
   * The condition to be evaluated prior to a transition being performed, if
   * any.
   */
  condition condition_;

  /**
   * The state this transition is transitioning from.
   */
  state::id from_id_;

  /**
   * The state this transition is transitioning to.
   */
  state::id to_id_;

  /**
   * The target function to be evaluated prior to a transition being
   * performed to determine the state to transition to, if any.
   */
  target target_;

  /**
   * The action to be performed, if any.
   */
  action action_;

  /**
   * Checks whether this transition is internal.
   *
   * @return Returns `true` only if the transition is internal.
   */
  bool is_internal() const;

  /**
   * Checks whether a transition between the two given statess would be
   * legal.  A transition is illegal only if the two states' nearest common
   * ancestor is a CHSM::set.
   *
   * @param s1 One of the states.
   * @param s2 The other state.
   * @return Returns `true` only if the transition is legal.
   */
  static bool is_legal( state const *s1, state const *s2 );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * The occurrence of an event ("broadcast") is that which causes transitions in
 * a machine.  An event has a name, may optionally be derived from another, and
 * may optionally have parameters during a broadcast.
 *
 * @author Paul J. Lucas
 */
class event {
protected:
  /////////////////////////////////////////////////////////////////////////////
  // Note: Even though this section is protected (as opposed to private), it //
  // is so that derived classes emitted by the CHSM-to-C++ compiler will     //
  // have access and it is not the intent that end users will.               //
  /////////////////////////////////////////////////////////////////////////////

  //
  // This typedef is placed up here because it us used in the CHSM_EVENT_ARGS
  // macro used in event's constructor.
  //
  typedef transition::id const *transition_list;

  /**
   * @internal
   *
   * The CHSM::machine this %event belongs to.
   *
   * This data member is declared up here since it is used inline by
   * `param_block::chsm()`.  Some compilers are broken and need it declared
   * before use as opposed to obeying the "rewriting rule."
   */
  machine &machine_;

public:
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# define  CHSM_EVENT_ARG_LIST(A)                    \
          A(CHSM_NS::machine*) chsm_machine_,       \
          A(transition_list) chsm_transition_list_, \
          A(char const*) chsm_name_,                \
          A(CHSM_NS::event*) chsm_base_event_

  /**
   * Defines the constructor arguments for the CHSM::event class.
   *
   * \hideinitializer
   */
# define CHSM_EVENT_ARGS          CHSM_EVENT_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::event class.
   *
   * @hideinitializer
   */
# define CHSM_EVENT_INIT          CHSM_EVENT_ARG_LIST(CHSM_ACTUAL)

  /**
   * Constructs an %event.
   */
  event( CHSM_EVENT_ARGS );

  /**
   * Destroys an %event.
   */
  virtual ~event();

  /**
   * Broadcasts an %event to a machine.  If it has a precondition, it will be
   * evaluated first.  Actions may be performed and transitions may occur.
   */
  void operator()() {
    lock_broadcast();
  }

  /**
   * Gets the machine that this %event belongs to.
   *
   * @return Returns said machine.
   */
  machine& chsm() const {
    return machine_;
  }

  /**
   * Checks whether this %event is of a particular event type.  This function
   * is a convenient shorthand.
   *
   * @tparam EventClass The %event class's type.
   * @return Returns `true` only if this %event is of the given type.
   */
  template<class EventClass> bool is_type() const {
    return dynamic_cast<EventClass const*>( this ) != nullptr;
  }

  /**
   * Returns the name of this %event.
   *
   * @return Returns said name.
   */
  char const* name() const {
    return name_;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

public:
  /////////////////////////////////////////////////////////////////////////////
  // Note: Even though this section is protected (as opposed to private), it //
  // is so that derived classes emitted by the CHSM-to-C++ compiler will     //
  // have access and it is not the intent that end users will.               //
  /////////////////////////////////////////////////////////////////////////////

  struct param_block;
  friend struct param_block;

  /**
   * @internal
   *
   * A %param_block holds the parameters for an event.  It is an (almost)
   * abstract base class for parameter blocks for events (that have
   * parameters).
   */
  struct param_block {
    /**
     * Constructs a %param_block.
     *
     * @param e The event to be a parameter block for.
     */
    explicit param_block( event const &e ) : chsm_event_{ e } { }

    /**
     * Destroys a %param_block.
     */
    virtual ~param_block();

  protected:
    ///////////////////////////////////////////////////////////////////////////
    // Note: Even though this section is protected (as opposed to private),  //
    // it is so that derived classes emitted by the CHSM-to-C++ compiler     //
    // will have access and it is not the intent that end users will.        //
    ///////////////////////////////////////////////////////////////////////////

    /**
     * The event this %param_block is for.
     */
    event const &chsm_event_;

    /**
     * The reason for having `chsm()` returning the owning event's machine is
     * that only this %param_block is a friend of the event class: by having a
     * `protected` function return it, all derived classes can obtain it.
     *
     * @return Returns said machine.
     */
    machine& chsm() const {
      return chsm_event_.machine_;
    }

    /**
     * Evaluates the precondition for a transition on an event.  You should
     * never need to call this explicitly.  It is called via code generated by
     * the CHSM-to-C++ compiler.
     *
     * @return The default always returns `true`.
     */
    virtual bool precondition() const;

    friend class event;
  };

protected:
  /////////////////////////////////////////////////////////////////////////////
  // Note: Even though this section is protected (as opposed to private), it //
  // is so that derived classes emitted by the CHSM-to-C++ compiler will     //
  // have access and it is not the intent that end users will.               //
  /////////////////////////////////////////////////////////////////////////////

  /**
   * @internal
   *
   * This extra class is used so the `operator()` code generated by the CHSM-
   * to-C++ compiler doesn't need direct access to `machine::mutex_`.
   */
  struct machine_lock;

  /**
   * @internal
   *
   * The `in_progress_` "flag" is actually a counter because, during a
   * transition, the same %event (or an %event derived from it) can be
   * broadcast more than once; however, nothing must be done for "nested"
   * broadcasts, i.e., when it's &gt; 0.
   */
  unsigned in_progress_;

  /**
   * @internal
   *
   * The current param_block, if any.
   */
  void *param_block_;

  /**
   * @internal
   *
   * Broadcasts an %event to a machine.  If it has a precondition, it will be
   * evaluated first.  Find transitions that are to be taken in response to
   * this %event: if one is found, queue it and run the CHSM transition
   * algorithm.
   *
   * @param param_block A pointer to a param_block, if any.
   */
  void broadcast( void *param_block );

private:
  char const *const           name_;              ///< Event name.
  event      *const           base_event_;        ///< Base event, if any.
  static transition::id const NO_TRANSITION_ID_;  ///< Sentinel for end().

  /**
   * This is the set of transitions an %event possibly triggers.  It has to be
   * a "native" C++ array of int rather than, say, an STL vector because the
   * CHSM-to-C++ compiler emits native arrays that are aggregate initialized.
   */
  transition_list const transitions_;

  bool is_debug_events() const;
  std::ostream& dout() const;

  /**
   * Returns whether this %event has no transitions.
   *
   * @return Returns `true` only if this %event has no transitions.
   */
  bool empty() const;

  /**
   * Does a broadcast, but locks first.
   */
  void lock_broadcast();

  class const_iterator;
  friend class const_iterator;

  /**
   * This is an iterator in "STL style" to iterate over the transitions an
   * %event possibly triggers.
   */
  class const_iterator {
  public:
    typedef transition value_type;
    typedef value_type const* const_pointer;
    typedef value_type const& const_reference;

    /**
     * Constructs a %const_iterator.
     */
    const_iterator() { }

    /**
     * Returns the ID of the transition that the iterator is positioned at.
     *
     * @return Returns said ID.
     */
    transition::id id() const {
      return *t_id_;
    }

    /**
     * Returns an event::const_reference to the transition the iterator is
     * positioned at.
     *
     * @return Returns said reference.
     */
    const_reference operator*() const {
      return t_[ *t_id_ ];
    }

    /**
     * Returns an event::const_pointer to the transition the iterator is
     * positioned at.
     *
     * @return Returns said pointer.
     */
    const_pointer operator->() const {
      return &operator*();
    }

    /**
     * Pre-increments the iterator.
     *
     * @return Returns the iterator positioned at the next transition, if any.
     */
    const_iterator& operator++() {
      ++t_id_;
      bump();
      return *this;
    }

    /**
     * Post-increments the iterator.
     *
     * @return Returns a new event::const_iterator positioned at the transition
     * the original iterator was positioned at prior to the increment.
     */
    const_iterator operator++(int) {
      auto const temp{ *this };
      ++*this;
      return temp;
    }

    /**
     * Compares two iterators for equality.
     *
     * @param i The first %iterator.
     * @param j The second %iterator.
     * @return Returns `true` only if the two iterators are positioned at the
     * same transition.
     */
    friend bool operator==( const_iterator const &i, const_iterator const &j ) {
      return *i.t_id_ == *j.t_id_;
    }

    /**
     * Compares two iterators for inequality.
     *
     * @param i The first %iterator.
     * @param j The second %iterator.
     * @return Returns `true` only if the two iterators are not positioned at
     * the same transition.
     */
    friend bool operator!=( const_iterator const &i, const_iterator const &j ) {
      return !(i == j);
    }

  protected:
    const_pointer     t_;               ///< Machine's transitions.
    transition_list   t_id_;
    event const      *base_event_;      ///< Base event, if any.

    const_iterator( const_pointer t, transition_list id, event const *b ) :
      t_{ t }, t_id_{ id }, base_event_{ b }
    {
      bump();
    }

  private:
    void bump();

    friend class event;
  };

  /**
   * Returns a new `const_iterator` positioned at the first transition in an
   * %event.
   *
   * @return Returns said iterator.
   */
  const_iterator begin() const;

  /**
   * Returns a new `const_iterator` positioned one past the last transition in
   * an %event.
   *
   * @return Returns said iterator.
   */
  const_iterator end() const {
    return const_iterator{ nullptr, &NO_TRANSITION_ID_, nullptr };
  }

  /**
   * Does post-broadcast clean-up for a broadcasted event.
   */
  void broadcasted();

  /**
   * Attempts to find transitions that are to be taken when this %event is
   * broadcast.
   *
   * @return Returns `true` only if at least one such transition is found.
   */
  bool find_transition();

  event( event const& ) = delete;
  event& operator=( event const& ) = delete;

  friend class  machine;
  friend bool   state::enter( event const&, state* );
  friend bool   state::exit ( event const&, state* );
};

////////// inlines ////////////////////////////////////////////////////////////

inline bool event::empty() const {
  return *transitions_ == NO_TRANSITION_ID_;
}

/**
 * Compares two events for equality.
 *
 * @note Since there is only one instance of each event per machine, it is
 * sufficient just to compare event addresses.  The only caveat is that the
 * same event for different instances of a machine will not compare equal.
 *
 * @param i The first event.
 * @param j The second event.
 * @return Returns `true` only if the events are equal.
 */
inline bool operator==( event const &i, event const &j ) {
  return &i == &j;
}

/**
 * Compares two events for inequality.
 *
 * @note Since there is only one instance of each event per machine, it is
 * sufficient just to compare event addresses.  The only caveat is that the
 * same event for different instances of a machine will compare not equal.
 *
 * @param i The first event.
 * @param j The second event.
 * @return Returns `true` only if the events are not equal.
 */
inline bool operator!=( event const &i, event const &j ) {
  return !(i == j);
}

///////////////////////////////////////////////////////////////////////////////

/**
 * A %machine contains an entire Concurrent, Hierarchical, Finite State
 * machine.  Every %machine is self-contained: there may be multiple instances
 * of the same %machine (or different machines) in different states in the same
 * program.
 *
 * @author Paul J. Lucas
 */
class machine {
  /**
   * The event used to start the machine.
   *
   * @note: This data member is placed up here due to a bug in some C++
   * compilers where an as-of-yet unseen static data member can not be used as
   * a default argument for a member function.
   */
  static event const PRIME_EVENT_;

public:
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /** Bitmask for debugging flags. */
  typedef unsigned debug_mask;

  /**
   * The constructor arguments must have horribly long names to avoid a name
   * collision with user-defined constructor arguments in derived classes.  We
   * use macros to make life easier.
   *
   * @hideinitializer
   */
# define  CHSM_MACHINE_ARG_LIST(A)                              \
          A(CHSM_NS::state*) chsm_state_ A([]),                 \
          A(CHSM_NS::cluster&) chsm_root_,                      \
          A(CHSM_NS::transition const) chsm_transition_ A([]),  \
          A(CHSM_NS::event const*) chsm_taken_ A([]),           \
          A(CHSM_NS::state*) chsm_target_ A([]),                \
          A(unsigned) chsm_transitions_in_machine_

  /**
   * Defines the constructor arguments for the CHSM::machine class.
   *
   * @hideinitializer
   */
# define CHSM_MACHINE_ARGS        CHSM_MACHINE_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::machine class.
   *
   * @hideinitializer
   */
# define CHSM_MACHINE_INIT        CHSM_MACHINE_ARG_LIST(CHSM_ACTUAL)

  /**
   * Destroys a %machine.
   */
  virtual ~machine();

  /**
   * A %machine as a whole is active only if its root cluster is.
   *
   * @return Returns `true` only if this %machine is active.
   */
  bool active() const;

  /**
   * Enters a %machine via a transition by entering its root cluster.
   *
   * @param trigger The event to start the %machine.
   * @return Returns `true` only if the %machine was actually entered, i.e., it
   * wasn't already active.
   */
  virtual bool enter( event const &trigger = PRIME_EVENT_ );

  /**
   * Exits a %machine via a transition by exiting its root cluster.
   *
   * @param trigger The event to exit the %machine.
   * @return Returns `true` only if the %machine was actually exited, i.e., it
   * wasn't already inactive.
   */
  virtual bool exit( event const &trigger = PRIME_EVENT_ );

  /**
   * This is an iterator in "STL style" to iterate over the (direct child)
   * states a machine has.
   */
  class const_iterator {
  public:
    typedef state value_type;
    typedef value_type const* const_pointer;
    typedef value_type const& const_reference;

    /**
     * Constructs a %const_iterator.
     */
    const_iterator() { }

    /**
     * Returns a machine::const_reference to the state the iterator is
     * positioned at.
     *
     * @return Returns said reference.
     */
    const_reference operator*() const {
      return **p_;
    }

    /**
     * Returns a machine::const_pointer to the state the iterator is positioned
     * at.
     *
     * @return Returns said pointer.
     */
    const_pointer operator->() const {
      return *p_;
    }

    /**
     * Pre-increments the iterator.
     *
     * @return Returns the iterator positioned at the next state, if any.
     */
    const_iterator& operator++() {
      return ++p_, *this;
    }

    /**
     * Post-increments the iterator.
     *
     * @return Returns a new machine::const_iterator positioned at the
     * state the original iterator was positioned at prior to the
     * increment.
     */
    const_iterator operator++(int) {
      return const_iterator( p_++ );
    }

    /**
     * Compares two iterators for equality.
     *
     * @param i The first iterator.
     * @param j The second iterator.
     * @return Returns `true` only if the two iterators are positioned at the
     * same state.
     */
    friend bool operator==( const_iterator const &i, const_iterator const &j ) {
      return *i.p_ == *j.p_;
    }

    /**
     * Compares two iterators for inequality.
     *
     * @param i The first iterator.
     * @param j The second iterator.
     * @return Returns `true` only if the two iterators are not positioned at
     * the same state.
     */
    friend bool operator!=( const_iterator const &i, const_iterator const &j ) {
      return !(i == j);
    }

  private:
    typedef const_pointer const *iter_type;
    iter_type p_;

    /**
     * Constructs a %const_iterator.
     *
     * @param p A pointer to a pointer to the first state in the machine.
     */
    explicit const_iterator( iter_type p ) : p_{ p } { }

    friend class machine;
  };

  /**
   * Returns a new `const_iterator` positioned at the first state in a
   * %machine.
   *
   * @return Returns said iterator.
   */
  const_iterator begin() const {
    return const_iterator( state_ );
  }

  /**
   * Returns a new `const_iterator` positioned one past the last state in a
   * %machine.
   *
   * @return Returns said iterator.
   */
  const_iterator end() const {
    return const_iterator( static_cast<const_iterator::iter_type>( &NIL_ ) );
  }

  /**
   * Do not print any debugging information.
   *
   * @hideinitializer
   */
  static debug_mask const DEBUG_NONE = 0x00u;

  /**
   * Prints entrances to and exits from states.
   *
   * @hideinitializer
   */
  static debug_mask const DEBUG_ENTER_EXIT = 0x01u;

  /**
   * Prints events queueing and dequeuing. 
   *
   * @hideinitializer
   */
  static debug_mask const DEBUG_EVENTS = 0x02u;

  /**
   * Reports progress during the broadcast algorithm.
   *
   * @hideinitializer
   */
  static debug_mask const DEBUG_ALGORITHM = 0x04u;

  /**
   * Reports all debugging information.
   *
   * @hideinitializer
   */
  static debug_mask const DEBUG_ALL = ~0u;

  /**
   * Gets the current debugging state.
   *
   * @return Returns said state.
   */
  debug_mask debug() const {
    return debug_state_;
  }

  /**
   * Sets the debugging state.  Debugging information is printed to standard
   * error.
   *
   * @param debug_state The state to set that is the bitwise "or" of the
   * debugging states #DEBUG_ENTER_EXIT, #DEBUG_EVENTS, #DEBUG_ALGORITHM, or
   * #DEBUG_ALL.
   * @return Returns the previous debugging state.
   */
  debug_mask debug( debug_mask debug_state ) {
    debug_mask const temp = debug_state_;
    debug_state_ = debug_state;
    return temp;
  }

  /**
   * Gets whether the current debug state contains \a debug_state.
   *
   * @param debug_state The debugging state to check.
   * @return Returns `true` only if the current debug state contains \a
   * debug_state.
   */
  bool is_debug( debug_mask debug_state ) const {
    return (debug_state_ & debug_state) != 0;
  }

  /**
   * Dumps a printout of the current state to standard error.  The dump
   * consists of each state's name, one per line, preceded by an asterisk only
   * if it is active; a space otherwise.
   */
  void dump_state() const;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
  /**
   * Constructs a %machine.
   *
   * When deriving a class from %machine, the macros `CHSM_MACHINE_ARGS` and
   * `CHSM_MACHINE_INIT` can be used to avoid having to deal with the many
   * constructor arguments, e.g.:
   * @code
   *  class my_machine : public CHSM::machine {
   *  public:
   *    my_machine( CHSM_MACHINE_ARGS ) : 
   *      CHSM::machine{ CHSM_MACHINE_INIT } {
   *      // ...
   *    }
   *    // ...
   *  };
   * @endcode
   */
  machine( CHSM_MACHINE_ARGS );

private:
  /**
   * The type of the queue of events that have been broadcasted.
   */
  typedef std::deque<event*> event_queue;

  /**
   * The entire set of states in the machine.  This is built and set by the
   * CHSM-to-C++ compiler.
   */
  state *const *const state_;

  /**
   * The outermost cluster encompassing the entire machine.
   */
  cluster &root_;

  /**
   * The entire set of transitions in the machine.  This is built and set by
   * the CHSM-to-C++ compiler.
   */
  transition const *const transition_;

  /**
   * The number of transitions in the machine.  This is set by the CHSM-to-C++
   * compiler.
   */
  unsigned const transitions_in_machine_;

  /**
   * An array to pointers to events indexed by transition::id.  It's used to
   * mark a transition as "taken" so it's taken at most once per transition
   * algorithm execution for a given event.
   */
  event const **const taken_;

  /**
   * An array of pointers to states indexed by state::id.  It's used store the
   * states to transition to for a given transition.
   */
  state **const target_;

  /**
   * This is `true` only when the transition algorithm is in progress and is
   * used to prevent recursive calls.
   */
  bool in_progress_;

  event_queue event_queue_;             ///< Events that have been broadcasted.
  unsigned    debug_indent_;            ///< Current debugging indentation.
  debug_mask  debug_state_;             ///< Current debugging state.

  static state const *const NIL_;       ///< Sentinel for end().
  static state::id const    NO_STATE_ID_; ///< Used by internal transitions.

  /**
   * Performs all the transitions for all events queued.
   */
  void algorithm();

  std::ostream& dout() const;

  friend class event;
  friend class parent;
  friend class state;
  friend struct transition;

  mutable mutex_type mutex_;
  friend struct event::machine_lock;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %parent \e is-an abstract state that has child states.
 *
 * It is an implementation detail to factor common data members and member
 * functions for the CHSM::cluster and CHSM::set classes.
 *
 * @author Paul J. Lucas
 */
class parent : public state {
protected:
  /**
   * @internal
   *
   * The array of state IDs that are the child states.
   */
  typedef id const* child_list;

public:
# define  CHSM_PARENT_ARG_LIST(A)     \
          CHSM_STATE_ARG_LIST(A),     \
          A(child_list) chsm_children_

  /**
   * Defines the constructor arguments for the CHSM::parent class.
   *
   * @hideinitializer
   */
# define CHSM_PARENT_ARGS         CHSM_PARENT_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::parent class.
   *
   * @hideinitializer
   */
# define CHSM_PARENT_INIT         CHSM_PARENT_ARG_LIST(CHSM_ACTUAL)

  /** The type of the value returned by iterator and const_iterator. */
  typedef state value_type;

  /** A pointer to a `value_type`. */
  typedef value_type* pointer;

  /** A pointer to a `const value_type`. */
  typedef value_type const* const_pointer;

  /** A reference to a `value_type`. */
  typedef value_type& reference;

  /** A reference to a `const value_type`. */
  typedef value_type const& const_reference;

  /**
   * Clears this parent's history.  However, since parents don't have a
   * history, there is nothing to do for them; however, calls `deep_clear()`
   * for all child states.
   */
  void deep_clear() override;

  /**
   * Returns whether this parent has any child states.
   *
   * @return Returns `true` only if this parent has any child states.
   */
  bool empty() const {
    return *children_ == NO_CHILD_ID_;
  }

  /**
   * Returns a parent::reference to the first child state.
   *
   * @return Returns said reference.
   */
  reference front() {
    return *(machine_.state_)[ *children_ ];
  }

  /**
   * Returns a parent::const_reference to the first child state.
   *
   * @return Returns said reference.
   */
  const_reference front() const {
    return *(machine_.state_)[ *children_ ];
  }

  class iterator;
  friend class iterator;

  /**
   * This is an iterator in "STL style" to iterate over the child states of a
   * parent state.
   */
  class iterator {
  public:
    /**
     * Constructs an %iterator.
     */
    iterator() { }

    /**
     * Returns a parent::reference to the state the %iterator is positioned at.
     *
     * @return Returns said reference.
     */
    reference operator*() const {
      return *p_[ *c_ ];
    }

    /**
     * Returns a parent::pointer to the state the %iterator is positioned at.
     *
     * @return Returns said pointer.
     */
    pointer operator->() const {
      return p_[ *c_ ];
    }

    /**
     * Pre-increments the %iterator.
     *
     * @return Returns the %iterator positioned at the next state, if any.
     */
    iterator& operator++() {
      return ++c_, *this;
    }

    /**
     * Post-increments the %iterator.
     *
     * @return Returns a new %iterator positioned at the state the original
     * %iterator was positioned at prior to the increment.
     */
    iterator operator++(int) {
      return iterator( p_, c_++ );
    }

    /**
     * Compares two iterators for equality.
     *
     * @param i The first %iterator.
     * @param j The second %iterator.
     * @return Returns `true` only if the two iterators are positioned at
     * the same state.
     */
    friend bool operator==( iterator const &i, iterator const &j ) {
      return *i.c_ == *j.c_;
    }

    /**
     * Compares two iterators for inequality.
     *
     * @param i The first %iterator.
     * @param j The second %iterator.
     * @return Returns `true` only if the two iterators are not positioned at
     * the same state.
     */
    friend bool operator!=( iterator const &i, iterator const &j ) {
      return !(i == j);
    }

  protected:
    pointer const  *p_;
    child_list      c_;

    iterator( pointer const *p, child_list c ) : p_( p ), c_( c ) { }

    friend class parent;
  };

  /**
   * Returns a parent::iterator positioned at the first child state of a
   * parent.
   *
   * @return Returns said iterator.
   */
  iterator begin() {
    return iterator( machine_.state_, children_ );
  }

  /**
   * Returns a parent::iterator positioned at one past the last child state of
   * a parent.
   *
   * @return Returns said iterator.
   */
  iterator end() {
    return iterator( nullptr, &NO_CHILD_ID_ );
  }

  class const_iterator;
  friend class const_iterator;

  /**
   * This is an iterator in "STL style" to iterate over the child states of a
   * parent state.
   */
  class const_iterator {
  public:
    /**
     * Constructs a %const_iterator.
     */
    const_iterator() { }

    /**
     * Returns a parent::const_reference to the state the iterator is
     * positioned at.
     *
     * @return Returns said reference.
     */
    const_reference operator*() const {
      return *p_[ *c_ ];
    }

    /**
     * Returns a parent::const_pointer to the state the iterator is positioned
     * at.
     *
     * @return Returns said reference.
     */
    const_pointer operator->() const {
      return p_[ *c_ ];
    }

    /**
     * Pre-increments the iterator.
     *
     * @return Returns the iterator positioned at the next state, if any.
     */
    const_iterator& operator++() {
      return ++c_, *this;
    }

    /**
     * Post-increments the iterator.
     *
     * @return Returns a new iterator positioned at the state the original
     * iterator was positioned at prior to the increment.
     */
    const_iterator operator++(int) {
      return const_iterator( p_, c_++ );
    }

    /**
     * Compares two iterators for equality.
     *
     * @param i The first iterator.
     * @param j The second iterator.
     * @return Returns `true` only if the two iterators are positioned at the
     * same state.
     */
    friend bool operator==( const_iterator const &i, const_iterator const &j ) {
      return *i.c_ == *j.c_;
    }

    /**
     * Compares two iterators for inequality.
     *
     * @param i The first iterator.
     * @param j The second iterator.
     * @return Returns `true` only if the two iterators are not positioned at
     * the same state.
     */
    friend bool operator!=( const_iterator const &i, const_iterator const &j ) {
      return !(i == j);
    }

  protected:
    pointer const  *p_;
    child_list      c_;

    const_iterator( pointer const *p, child_list c ) : p_( p ), c_( c ) { }

    friend class parent;
  };

  /**
   * Returns a parent::const_iterator positioned at the first child state of a
   * parent.
   *
   * @return Returns said iterator.
   */
  const_iterator begin() const {
    return const_iterator( machine_.state_, children_ );
  }

  /**
   * Returns a parent::const_iterator positioned at one past the last child
   * state of a parent.
   *
   * @return Returns said iterator.
   */
  const_iterator end() const {
    return const_iterator( nullptr, &NO_CHILD_ID_ );
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
  /**
   * @internal
   *
   * Constructs a parent.
   */
  parent( CHSM_PARENT_ARGS ) :
    state( CHSM_STATE_INIT ),
    children_( chsm_children_ )
  {
  }

  /**
   * @internal
   *
   * Requests that an inactive child state become the active child of its
   * parent state.
   *
   * @param child The child state requesting permission to become the active
   * child.
   * @return Returns `true` by default.
   */
  virtual bool switch_active_child_to( state *child );

  // for access to switch_active_child_to()
  friend bool state::enter( event const&, state* );

private:
  parent( parent const& ) = delete;
  parent& operator=( parent const& ) = delete;

  child_list const  children_;          ///< The child states.
  static id const   NO_CHILD_ID_;       ///< Sentinel for end().
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %cluster \e is-a CHSM::parent that can have at most one child-state active
 * at any given time.
 *
 * A %cluster may have a history.  If a %cluster does not have a history, or it
 * does have a history but has not previously been active, then the child-state
 * entered after it itself is entered is the \e default child-state; if a
 * %cluster does have a history and it has been visited before, then the child-
 * state entered after it itself is entered is the one that was last active.
 *
 * A %cluster may alternatively have a \e deep history.  Such a %cluster
 * behaves exactly as one with an ordinary history; the difference is that all
 * clusters lexically-enclosed by it in the CHSM description also have a
 * history.
 *
 * The %cluster class can be extended either to add additional data members and
 * member functions, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
class cluster : public parent {
public:
# define  CHSM_CLUSTER_ARG_LIST(A) \
          CHSM_PARENT_ARG_LIST(A), \
          A(bool) chsm_history_

  /**
   * Defines the constructor arguments for the CHSM::cluster class.
   *
   * @hideinitializer
   */
# define  CHSM_CLUSTER_ARGS       CHSM_CLUSTER_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::cluster class.
   *
   * @hideinitializer
   */
# define  CHSM_CLUSTER_INIT       CHSM_CLUSTER_ARG_LIST(CHSM_ACTUAL)

  /**
   * Constructs a cluster.
   *
   * When deriving a class from %cluster, the macros `CHSM_CLUSTER_ARGS` and
   * `CHSM_CLUSTER_INIT` can be used to avoid having to deal with the many
   * constructor arguments.  See CHSM::state for an example.
   */
  cluster( CHSM_CLUSTER_ARGS );

  /**
   * Clears the history.
   */
  void clear() {
    last_child_ = &front();
  }

  /**
   * Clears the history, if any; then clear all child cluster's history, if
   * any.
   */
  void deep_clear() override;

  /**
   * Enters a %cluster via a transition and also enter one of its child states.
   *
   * @param trigger The event that triggered the transition.
   * @param from_child If not null, this child state of the cluster is being
   * entered directly.
   * @return Returns `true` only if the %cluster was actually entered, i.e., it
   * wasn't already active.
   */
  bool enter( event const &trigger, state *from_child = nullptr ) override;

  /**
   * Exits a %cluster via a transition, but first exit its active child state.
   *
   * @param trigger The event that triggered the transition.
   * @param to The state that is being transitioned to.
   * @return Returns `true` only if the %cluster actually exited, i.e., it was
   * active.
   */
  bool exit( event const &trigger, state *to = nullptr ) override;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

private:
  cluster( cluster const& ) = delete;
  cluster& operator=( cluster const& ) = delete;

  bool switch_active_child_to( state *child ) override;

  bool const  history_;                 ///< Does this cluster have a history?
  state      *active_child_;            ///< Currently active child, if any.
  state      *last_child_;              ///< Last active child, if any.
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %set \e is-a CHSM::parent that has either all or no child-states active.
 * All child-states are entered after and exited before their parent-state.
 * The order that child-states are entered and exited is undefined.
 *
 * The %set class can be extended either to add additional data members and
 * member functions, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
class set : public parent {
public:
# define  CHSM_SET_ARG_LIST(A) \
          CHSM_PARENT_ARG_LIST(A)

  /**
   * Defines the constructor arguments for the CHSM::set class.
   *
   * @hideinitializer
   */
# define  CHSM_SET_ARGS   CHSM_SET_ARG_LIST(CHSM_FORMAL)

  /**
   * Defines the base-class constructor argument \e mem-initializers for the
   * CHSM::set class.
   *
   * @hideinitializer
   */
# define  CHSM_SET_INIT   CHSM_SET_ARG_LIST(CHSM_ACTUAL)

  /**
   * Constructs a %set.
   *
   * When deriving a class from %set, the macros `CHSM_SET_ARGS` and
   * `CHSM_SET_INIT` can be used to avoid having to deal with the many
   * constructor arguments.  See CHSM::state for an example.
   */
  set( CHSM_SET_ARGS ) : parent{ CHSM_PARENT_INIT } { }

  /**
   * Enters a %set via a transition and also enters all of its child states.
   *
   * @param trigger The event that triggered the transition.
   * @param from_child Not used here.
   * @return Returns `true` only if the %set was actually entered, i.e., it
   * wasn't already active.
   */
  bool enter( event const &trigger, state *from_child = nullptr ) override;

  /**
   * Exits a %set via a transition, but first exits all of its child states.
   *
   * @param trigger The event that triggered the transition.
   * @param to The state we are transitioning to.
   * @return Returns `true` only if the %set actually exited, i.e., it was
   * active.
   */
  bool exit( event const &trigger, state *to = nullptr ) override;

  set( set const& ) = delete;
  set& operator=( set const& ) = delete;
};

///////////////////////////////////////////////////////////////////////////////

struct event::machine_lock : lock_type {
  explicit machine_lock( machine &m ) : lock_type{ m.mutex_ } { }
};

////////// inlines ////////////////////////////////////////////////////////////

inline event::const_iterator event::begin() const {
  //
  // This has to be defined down here so that the declaration for the machine
  // class has been seen by the C++ compiler.
  //
  return const_iterator{ machine_.transition_, transitions_, base_event_ };
}

inline bool machine::active() const {
  //
  // A machine as a whole is active only if its root cluster is.
  //
  return root_.active();
}

inline bool transition::is_internal() const {
  return to_id_ == machine::NO_STATE_ID_ && target_ == nullptr;
}

} // namespace

////////// namespace stuff ////////////////////////////////////////////////////

/**
 * If, for whatever reason, you don't want the long namespace name aliased to a
 * shorter one, define `CHSM_NO_ALIAS_NS`.
 */
#ifndef CHSM_NO_ALIAS_NS
  /**
   * If you want the short namespace alias name to be something other than
   * "CHSM", define `CHSM_NS_ALIAS` to be what you want.
   */
# ifndef  CHSM_NS_ALIAS
# define  CHSM_NS_ALIAS CHSM
# endif

namespace CHSM_NS_ALIAS = CHSM_NS;

#endif /* CHSM_NO_ALIAS_NS */

#endif /* chsm_H */
/* vim:set et sw=2 ts=2: */
