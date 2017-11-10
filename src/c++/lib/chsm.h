/*
**      CHSM Language System
**      chsm.h -- Run-Time library declarations
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

#ifndef chsm_H
#define chsm_H

/**
 * \mainpage CHSM Language System for C++ API Specification
 *
 * Provides classes for implementing the run-time of the
 * <a href="http://chsm.sourceforge.net/" target="_top">
 * Concurrent Hierarchical State Machine Language System</a>
 * for C++.
 *
 * \section rel_doc Related Documentation
 * <ul>
 *   <li><a href="http://chsm.sourceforge.net/documentation.html" target="_top">Unix manual pages</a>.</li>
 *   <li><a href="http://chsm.sourceforge.net/examples/" target="_top">Examples</a>.</li>
 * </ul>
 *
 * @authors Paul J. Lucas
 *      and Fabio Riccardi
 */

/**
 * \file chsm.h
 *
 * Provides classes for implementing the run-time of the
 * <a href="http://chsm.sourceforge.net/" target="_top">
 * Concurrent Hierarchical State Machine Language System</a>
 * for C++.
 *
 * @authors Paul J. Lucas
 *      and Fabio Riccardi
 */

// standard
#include <list>
#ifdef  CHSM_MULTITHREADED
#include <pthread.h>
#endif  /* CHSM_MULTITHREADED */

/**
 * Define the long CHSM namespace name.  This shouldn't ever conflict with
 * anything.  See the end of this file for more on namespaces.
 */
#define CHSM_NS Concurrent_Hierarchical_State_Machine

namespace CHSM_NS {
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

#ifdef  CHSM_MULTITHREADED
/**
 * \internal
 *
 * Use a %mutex_lock to lock/unlock a mutex within a scope to guarantee that it
 * will be unlocked.
 *
 * @author Paul J. Lucas
 */
class mutex_lock {
public:
    /**
     * Constructs a %mutex_lock and lock the given mutex.
     * In the example below, the mutex \c some_mutex is unlocked when the \c
     * lock variable goes out of scope.
     * \code
     * void f() {
     *     mutex_lock const lock( some_mutex );
     *     // ...
     * }
     * \endcode
     *
     * @param m The mutex to lock.
     */
    mutex_lock( pthread_mutex_t &m );

    /**
     * Destroys a %mutex_lock and unlocks the mutex given to the constructor.
     */
    ~mutex_lock();

private:
    int prev_cancel_state_;
    pthread_mutex_t &mutex_;

    mutex_lock( mutex_lock& );                  // forbid copy
    mutex_lock& operator=( mutex_lock const& ); // forbid assignment
};

////////// Inlines ////////////////////////////////////////////////////////////

inline mutex_lock::mutex_lock( pthread_mutex_t &m ) : mutex_( m ) {
    pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, &prev_cancel_state_ );
    pthread_mutex_lock( &mutex_ );
}

inline mutex_lock::~mutex_lock() {
    pthread_mutex_unlock( &mutex_ );
    pthread_setcancelstate( prev_cancel_state_, 0 );
    pthread_testcancel();
}
#endif  /* CHSM_MULTITHREADED */

//=============================================================================

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
#   define  CHSM_STATE_ARG_LIST(A) \
            A(CHSM_NS::machine&) chsm_machine_, \
            A(char const*) chsm_name_, \
            A(CHSM_NS::parent*) chsm_parent_, \
            A(CHSM_NS::state::action) chsm_enter_action_, \
            A(CHSM_NS::state::action) chsm_exit_action_, \
            A(CHSM_NS::event*) chsm_enter_event_, \
            A(CHSM_NS::event*) chsm_exit_event_

    /**
     * Defines the constructor arguments for the CHSM::state class.
     *
     * \hideinitializer
     */
#   define  CHSM_STATE_ARGS CHSM_STATE_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::state class.
     *
     * \hideinitializer
     */
#   define  CHSM_STATE_INIT CHSM_STATE_ARG_LIST(CHSM_ACTUAL)

    /**
     * The identification number of a state.
     */
    typedef int id;

    /**
     * An action is a function (machine member function) that is optionally
     * called upon enter/exit of a state.
     *
     * @param s The state that is being entered or exited.
     * @param trigger The event that triggered the transition.
     */
    typedef void (machine::*action)( state const &s, event const &trigger );

    /**
     * Construct a state.
     * When deriving a class from \c state, the macros \c CHSM_STATE_ARGS and
     * \c CHSM_STATE_INIT can be used to avoid having to deal with the many
     * constructor arguments, e.g.:
     * \code
     * class place : public CHSM::state {
     * public:
     *     place( CHSM_STATE_ARGS ) : CHSM::state( CHSM_STATE_INIT ) {
     *         // ...
     *     }
     *     // ...
     * };
     * \endcode
     */
    state( CHSM_STATE_ARGS );

    /**
     * Destroy a state.
     */
    ~state();

    /**
     * Gets whether this state is active.
     *
     * @return Returns \c true only if the state is active.
     */
    bool active() const {
        return state_ & S_active;
    }

    /**
     * Gets the machine that this state belongs to.
     *
     * @return Returns said machine.
     */
    machine& chsm() const {
        return machine_;
    }

    /**
     * Clears the history for all child clusters of a cluster, recursively.
     * Plain states have neither children nor a history.  Placing this function
     * here is a wart on the design; by having it here, however, run-time
     * type-identification can be avoided.
     */
    virtual void deep_clear();

    /**
     * Return the name of the state.
     *
     * @return Returns said name.
     */
    char const* name() const {
        return name_;
    }

    /**
     * Return the parent cluster or set.
     *
     * @return Returns the parent or \c null if none.
     */
    parent* parent_of() const {
        return parent_;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
    /**
     * \internal
     *
     * The CHSM::machine the state belongs to.
     */
    machine &machine_;

    /**
     * Enter a state.
     * If this state's parent state (if any) isn't active, enter it first.
     * The event <code>$enter(</code><i>state</i><code>)</code> is broadcast
     * and the state's enter-action block is executed, if any.
     *
     * When overriding in a derived class, the form \e must be:
     * \code
     * bool enter( CHSM::event const &amp;trigger, CHSM::state *from_child ) {
     *     if ( !CHSM::state::enter( trigger, from_child ) )
     *         return false;
     *     // ... new functionality here ...
     *     return true;
     * }
     * \endcode
     *
     * @param trigger The event that triggered the transition.
     * @param from_child Not used here.
     * @return Returns \c true only if the state was actually
     * entered, i.e., it wasn't already active.
     */
    virtual bool enter( event const &trigger, state *from_child = 0 );

    /**
     * Exit a state.
     * If the "to" state doesn't have this state's parent state (if any) as an
     * ancestor, exit this state's parent state first.  The event
     * <code>$exit(</code><i>state</i><code>)</code> is broadcast and the
     * state's exit-action block is executed, if any.
     *
     * When overriding in a derived class, the form \e must be:
     * \code
     * boolean exit( CHSM::event const &amp;trigger, CHSM::state *to ) {
     *     if ( !CHSM::state::exit( trigger, to ) )
     *         return false;
     *     // ... new functionality here ...
     *     return true;
     * }
     * \endcode
     *
     * @param trigger The event that triggered the transition.
     * @param to If not \c null, the state to transition to.
     * @return Returns \c true only if the state was actually exited,
     * i.e., it wasn't already active.
     */
    virtual bool exit( event const &trigger, state *to = 0 );

private:
    char const  *const name_;       //!< State name.
    parent      *const parent_;     //!< Parent, if any.

    enum {
        S_inactive          = 0x00,
        S_active            = 0x01,
        //
        // S_active_disabled is used to prevent making more than one
        // nondeterministic transition from the same state.  See the comments
        // in event.c for more information.
        //
        S_active_disabled   = 0x02 | S_active
    };
    unsigned state_;                //!< The state of the state.

    /**
     * The enter/exit events for the state are non-null only if they are
     * actually used in the machine.  Their values are determined by the
     * CHSM-to-C++ compiler.
     */
    event *const enter_event_, *const exit_event_;

    /**
     * See the comment for the "action" declaration.
     */
    action const enter_action_, exit_action_;

    friend class cluster;
    friend class event;
    friend class machine;
    friend class parent;
    friend class set;
};

//=============================================================================

/**
 * A %transition is a simple \c struct containing information for a transition
 * from one state to another in a machine.  The information contains whether
 * the transition has an associated condition to test prior to taking it, the
 * "from" and "to" states, and an optional action to be performed.
 *
 * All the data members ideally should be declared \c const.  In the code
 * emitted by the CHSM-to-C++ compiler, we want to be able to use aggregate
 * initialization, but C++ does not permit aggregates to have \c const data
 * members (see section 8.5.1 of the ANSI/ISO C++ reference manual), so we
 * leave the \c const out.  However, the code emitted declares the aggregate as
 * a whole to be \c const, so it amounts to the same thing.
 *
 * @author Paul J. Lucas
 */
struct transition {
    /**
     * A condition is a function to be evaluated prior to a transition being
     * performed.  The transition is performed only if the function returns
     * \c true.
     *
     * @param trigger The event that is causing the transition.
     * @return Returns \c true only if the condition evaluates to \c true.
     */
    typedef bool (machine::*condition)( event const &trigger );

    /**
     * An action is a function to be executed upon performing a transition
     * from one state to another.
     *
     * @param trigger The event that caused the transition.
     */
    typedef void (machine::*action)( event const &trigger );

    /**
     * A target is a function to be evaluated prior to a transition being
     * performed to determine the state to transition to.
     *
     * @param trigger The event that caused the transition.
     * @return Returns the state that the transition should go to or \c null to
     * abort the transition.
     */
    typedef state* (machine::*target)( event const &trigger );

    /**
     * The condition to be evaluated prior to a transition being performed, if
     * any.
     */
    condition   condition_;

    /**
     * The state this transition is transitioning from.
     */
    state::id   from_id_;

    /**
     * The state this transition is transitioning to.
     */
    state::id   to_id_;

    /**
     * The target function to be evaluated prior to a transition being
     * performed to determine the state to transition to, if any.
     */
    target      target_;

    /**
     * The action to be performed, if any.
     */
    action      action_;

    /**
     * Returns whether this transition is internal.
     *
     * @return Returns \c true only if the transition is internal.
     */
    bool is_internal() const;

    /**
     * Checks whether a transition between the two given statess would be
     * legal.  A transition is illegal only if the two states' nearest common
     * ancestor is a CHSM::set.
     *
     * @param s1 One of the states.
     * @param s2 The other state.
     * @return Returns \c true only if the transition is legal.
     */
    static bool is_legal( state const *s1, state const *s2 );
};

//=============================================================================

/**
 * The occurrence of an event ("broadcast") is that which causes transitions in
 * a machine.  An event has a name, may optionally be derived from another, and
 * may optionally have parameters during a broadcast.
 *
 * @author Paul J. Lucas
 */
class event {
protected:
    /*
    ** Note: Even though this section is protected (as opposed to private), it
    ** is so that derived classes emitted by the CHSM-to-C++ compiler will have
    ** access and it is not the intent that end users will.
    */

    //
    // These typedefs are placed up here because they are used in the
    // CHSM_EVENT_ARGS macro used in event's constructor.
    //
    typedef int transition_id;
    typedef transition_id const *transition_list;

    /**
     * \internal
     *
     * The CHSM::machine this event belongs to.
     *
     * This data member is declared up here since it is used inline by
     * param_block::chsm().  Some compilers are broken and need it declared
     * before use as opposed to obeying the "rewriting rule."
     */
    machine &machine_;

public:
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#   define  CHSM_EVENT_ARG_LIST(A) \
            A(CHSM_NS::machine*) chsm_machine_, \
            A(transition_list) chsm_transition_list_, \
            A(char const*) chsm_name_, \
            A(CHSM_NS::event*) chsm_base_event_

    /**
     * Defines the constructor arguments for the CHSM::event class.
     *
     * \hideinitializer
     */
#   define  CHSM_EVENT_ARGS CHSM_EVENT_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::event class.
     *
     * \hideinitializer
     */
#   define  CHSM_EVENT_INIT CHSM_EVENT_ARG_LIST(CHSM_ACTUAL)

    /**
     * Construct an event.
     */
    event( CHSM_EVENT_ARGS );

    /**
     * Destroy an event.
     */
    virtual ~event();

    /**
     * Broadcast an event to a machine.  If it has a precondition, it will be
     * evaluated first.  Actions may be performed and transitions may occur.
     */
    void operator()() {
        lock_broadcast();
    }

    /**
     * Gets the machine that this event belongs to.
     *
     * @return Returns said machine.
     */
    machine& chsm() const {
        return machine_;
    }

    /**
     * Checks whether this event is of a particular event type.  This function
     * is a convenient shorthand.
     *
     * @tparam EventClass The event class's type.
     * @return Returns \c true only if this event is of the given type.
     */
    template<typename EventClass> bool is_type() const {
        return dynamic_cast<EventClass const*>( this );
    }

    /**
     * Returns the name of this event.
     *
     * @return Returns said name.
     */
    char const* name() const {
        return name_;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

public:
    /*
    ** Note: Even though this section is public (as opposed to private), it
    ** is so that code emitted by the CHSM-to-C++ compiler will have access and
    ** it is not the intent that end users will.
    */

    struct param_block;
    friend struct param_block;

    /**
     * \internal
     *
     * A param_block holds the parameters for an event.  It is an (almost)
     * abstract base class for parameter blocks for events (that have
     * parameters).
     */
    struct param_block {
        /**
         * Construct a param_block.
         *
         * @param e The event to be a parameter block for.
         */
        param_block( event const &e ) : chsm_event_( e ) { }

        /**
         * Destroy a param_block.
         */
        virtual ~param_block();

    protected:
        /*
        ** Note: Even though this section is protected (as opposed to private),
        ** it is so that derived classes emitted by the CHSM-to-C++ compiler
        ** will have access and it is not the intent that end users will.
        */

        /**
         * The event this param_block is for.
         */
        event const &chsm_event_;

        /**
         * The reason for having \c chsm() returning the owning event's machine
         * is that only this param_block is a friend of the event class: by
         * having a protected function return it, all derived classes can
         * obtain it.
         */
        machine& chsm() const {
            return chsm_event_.machine_;
        }

        /**
         * Evaluate the precondition for a transition on an event.  You should
         * never need to call this explicitly.  It is called via code generated
         * by the CHSM-to-C++ compiler.
         *
         * @return The default always returns \c true.
         */
        virtual bool precondition() const;

        friend class event;
    };

protected:
    /*
    ** Note: Even though this section is protected (as opposed to private),
    ** it is so that derived classes emitted by the CHSM-to-C++ compiler will
    ** have access and it is not the intent that end users will.
    */

    /**
     * \internal
     *
     * The in_progress_ "flag" is actually a counter because, during a
     * transition, the same event (or an event derived from it) can be
     * broadcast more than once; however, nothing must be done for "nested"
     * broadcasts, i.e., when it's &gt; 0.
     */
    unsigned in_progress_;

    /**
     * \internal
     *
     * The current param_block, if any.
     */
    void *param_block_;

    /**
     * \internal
     *
     * Broadcast an event to a machine.  If it has a precondition, it will be
     * evaluated first.  Find transitions that are to be taken in response to
     * this event: if one is found, queue it and run the CHSM transition
     * algorithm.
     *
     * @param param_block A pointer to a param_block, if any.
     */
    void broadcast( void *param_block );

#ifdef  CHSM_MULTITHREADED
    /**
     * \internal
     *
     * This extra class is used so the \c operator() code generated by the
     * CHSM-to-C++ compiler doesn't need direct access to
     * <code>machine::machine_lock_</code>.
     */
    class machine_lock : public mutex_lock {
    public:
        machine_lock( machine& );
    };
#endif  /* CHSM_MULTITHREADED */

private:
    /**
     * This is the set of transitions an event possibly triggers.  It has to be
     * a "native" C++ array of int rather than, say, an STL vector because the
     * CHSM-to-C++ compiler emits native arrays that are aggregate initialized.
     */
    transition_list const   transitions_;

    char const              *const name_;       //!< Event name.
    event                   *const base_event_; //!< Base event, if any.
    static int const        no_transition_id_;  //!< Sentinel for end().

    /**
     * Returns whether this event has no transitions.
     * @return Returns \c true only if this event has no transitions.
     */
    bool empty() const;

    /**
     * Do a broadcast, but lock first.
     */
    void lock_broadcast();

    /** The type of the value returned by const_iterator. */
    typedef transition value_type;

    /** A pointer to a <code>const value_type</code>. */
    typedef value_type const* const_pointer;

    /** A reference to a <code>const value_type</code>. */
    typedef value_type const& const_reference;

    class const_iterator;
    friend class const_iterator;

    /**
     * This is an iterator in "STL style" to iterate over the transitions an
     * event possibly triggers.
     */
    class const_iterator {
    public:
        /**
         * Construct a const_iterator.
         */
        const_iterator() { }

        /**
         * Returns the ID of the transition that the iterator is positioned at.
         * @return Returns said ID.
         */
        transition_id id() const {
            return *t_id_;
        }

        /**
         * Returns an event::const_reference to the transition the iterator is
         * positioned at.
         * @return Returns said reference.
         */
        const_reference operator*() const {
            return t_[ *t_id_ ];
        }

        /**
         * Returns an event::const_pointer to the transition the iterator is
         * positioned at.
         * @return Returns said pointer.
         */
        const_pointer operator->() const {
            return &operator*();
        }

        /**
         * Pre-increment the iterator.
         * @return Returns the iterator positioned at the next transition, if
         * any.
         */
        const_iterator& operator++() {
            ++t_id_;
            bump();
            return *this;
        }

        /**
         * Post-increment the iterator.
         * @return Returns a new event::const_iterator positioned at the
         * transition the original iterator was positioned at prior to the
         * increment.
         */
        const_iterator operator++(int) {
            const_iterator const temp = *this;
            ++*this;
            return temp;
        }

        /**
         * Compare two iterators for equality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are positioned at
         * the same transition.
         */
        friend bool operator==( const_iterator const &i,
                                const_iterator const &j ) {
            return *i.t_id_ == *j.t_id_;
        }

        /**
         * Compare two iterators for inequality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are not positioned
         * at the same transition.
         */
        friend bool operator!=( const_iterator const &i,
                                const_iterator const &j ) {
            return !( i == j );
        }

    protected:
        const_pointer   t_;             // machine's transitions
        transition_list t_id_;
        event const     *base_event_;

        const_iterator( const_pointer, transition_list, event const* );

    private:
        void bump();

        friend class event;
    };

    /**
     * Returns a new const_iterator positioned at the first transition in an
     * event.
     * @return Returns said iterator.
     */
    const_iterator begin() const;

    /**
     * Returns a new const_iterator positioned one past the last transition in
     * an event.
     * @return Returns said iterator.
     */
    const_iterator end() const {
        return const_iterator( 0, &no_transition_id_, 0 );
    }

    /**
     * Do post-broadcast clean-up for a broadcasted event.
     * @see Bjarne Stroustrup.  "The C++ Programming Language, 3rd ed."
     *      Addison-Wesley, Reading, MA, 1997.  pp. 116-118.
     */
    void broadcasted();

    friend class    machine;
    friend bool     state::enter( event const&, state* );
    friend bool     state::exit ( event const&, state* );
};

////////// Inlines ////////////////////////////////////////////////////////////

inline bool event::empty() const {
    return *transitions_ == no_transition_id_;
}

/**
 * Compare two events for equality.
 * Since there is only one instance of each event per machine, it is sufficient
 * just to compare event addresses.
 * The only caveat is that the same event for different instances of a
 * machine will not compare equal.
 *
 * @param a The first event.
 * @param b The second event.
 * @return Returns \c true only if the events are equal.
 */
inline bool operator==( event const &a, event const &b ) {
    return &a == &b;
}

/**
 * Compare two events for inequality.
 * Since there is only one instance of each event per machine, it is sufficient
 * just to compare event addresses.
 * The only caveat is that the same event for different instances of a
 * machine will compare not equal.
 *
 * @param a The first event.
 * @param b The second event.
 * @return Returns \c true only if the events are not equal.
 */
inline bool operator!=( event const &a, event const &b ) {
    return !( a == b );
}

//=============================================================================

/**
 * A machine contains an entire Concurrent, Hierarchical, Finite State machine.
 * Every machine is self-contained: there may be multiple instances of the same
 * machine (or different machines) in different states in the same program.
 *
 * @author Paul J. Lucas
 */
class machine {
    /**
     * This is the event used to start the machine.
     *
     * This data member is placed up here due to a bug in some C++ compilers
     * where an as-of-yet unseen static data member can not be used as a
     * default argument for a member function.
     */
    static event prime_;

public:
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    //
    // The constructor arguments must have horribly long names to avoid a
    // name collision with user-defined constructor arguments in derived
    // classes.  We use macros to make life easier.
    //
#   define  CHSM_MACHINE_ARG_LIST(A) \
            A(CHSM_NS::state*) chsm_state_ A([]), \
            A(CHSM_NS::cluster&) chsm_root_, \
            A(CHSM_NS::transition const) chsm_transition_ A([]), \
            A(CHSM_NS::event const*) chsm_taken_ A([]), \
            A(CHSM_NS::state*) chsm_target_ A([]), \
            A(int) chsm_transitions_in_machine_

    /**
     * Defines the constructor arguments for the CHSM::machine class.
     *
     * \hideinitializer
     */
#   define CHSM_MACHINE_ARGS    CHSM_MACHINE_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::machine class.
     *
     * \hideinitializer
     */
#   define CHSM_MACHINE_INIT    CHSM_MACHINE_ARG_LIST(CHSM_ACTUAL)

    /**
     * Destroy a machine.
     */
    virtual ~machine();

    /**
     * A machine as a whole is active only if its root cluster is.
     *
     * @return Returns \c true only if this machine is active.
     */
    bool active() const;

    /**
     * Enter a machine via a transition by entering its root cluster.
     *
     * @param trigger The event to start the machine.
     * @return Returns \c true only if the machine was actually entered, i.e.,
     * it wasn't already active.
     */
    virtual bool enter( event const &trigger = prime_ );

    /**
     * Exit a machine via a transition by exiting its root cluster.
     *
     * @param trigger The event to exit the machine.
     * @return Returns \c true only if the machine was actually exited, i.e.,
     * it wasn't already inactive.
     */
    virtual bool exit( event const &trigger = prime_ );

    /** The type of the value returned by const_iterator. */
    typedef state value_type;

    /** A pointer to a <code>const value_type</code>. */
    typedef value_type const* const_pointer;

    /** A reference to a <code>const value_type</code>. */
    typedef value_type const& const_reference;

    /**
     * This is an iterator in "STL style" to iterate over the (direct child)
     * states a machine has.
     */
    class const_iterator {
    public:
        /**
         * Construct a const_iterator.
         */
        const_iterator() { }

        /**
         * Returns a machine::const_reference to the state the iterator is
         * positioned at.
         * @return Returns said reference.
         */
        const_reference operator*() const {
            return **p_;
        }

        /**
         * Returns a machine::const_pointer to the state the iterator is
         * positioned at.
         * @return Returns said pointer.
         */
        const_pointer operator->() const {
            return *p_;
        }

        /**
         * Pre-increment the iterator.
         * @return Returns the iterator positioned at the next state, if any.
         */
        const_iterator& operator++() {
            return ++p_, *this;
        }

        /**
         * Post-increment the iterator.
         * @return Returns a new machine::const_iterator positioned at the
         * state the original iterator was positioned at prior to the
         * increment.
         */
        const_iterator operator++(int) {
            return const_iterator( p_++ );
        }

        /**
         * Compare two iterators for equality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are positioned at
         * the same state.
         */
        friend bool operator==( const_iterator const &i,
                                const_iterator const &j ) {
            return *i.p_ == *j.p_;
        }

        /**
         * Compare two iterators for inequality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are not positioned
         * at the same state.
         */
        friend bool operator!=( const_iterator const &i,
                                const_iterator const &j ) {
            return !( i == j );
        }

    private:
        typedef const_pointer const *iter_type;
        iter_type p_;
        const_iterator( iter_type p ) : p_( p ) { }
        friend class machine;
    };

    /**
     * Returns a new const_iterator positioned at the first state in a machine.
     * @return Returns said iterator.
     */
    const_iterator begin() const {
        return const_iterator( (const_iterator::iter_type)state_ );
    }

    /**
     * Returns a new const_iterator positioned one past the last state in a
     * machine.
     * @return Returns said iterator.
     */
    const_iterator end() const {
        return const_iterator( (const_iterator::iter_type)&nil_ );
    }

    enum {
        /**
         * Do not print any debugging information.
         * \hideinitializer
         */
        D_none  = 0x00,

        /**
         * Prints entrances to and exits from states.
         * \hideinitializer
         */
        D_enex  = 0x01,

        /**
         * Prints events queueing and dequeuing. 
         * \hideinitializer
         */
        D_event = 0x02,

        /**
         * Reports progress during the broadcast algorithm.
         * \hideinitializer
         */
        D_alg   = 0x04,

        /**
         * Reports all debugging information.
         * \hideinitializer
         */
        D_all   = D_enex | D_event | D_alg
    };

    /**
     * Gets the current debugging state.
     *
     * @return Returns said state.
     */
    unsigned debug() const {
        return debug_;
    }

    /**
     * Sets the debugging state.  Debugging information is printed to system
     * error.
     *
     * @param state The state to set that is the bitwise "or" of the debugging
     * states #D_enex, #D_event, #D_alg, or #D_all.
     * @return Returns the previous debugging state.
     */
    unsigned debug( unsigned state ) {
        unsigned const temp = debug_;
        debug_ = state;
        return temp;
    }

    /**
     * Dumps a printout of the current state to standard error.  The dump
     * consists of each state's name, one per line, preceded by an asterisk
     * only if it is active; a space otherwise.
     */
    void dump_state() const;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
    /**
     * Construct a %machine.
     * When deriving a class from %machine, the macros \c CHSM_MACHINE_ARGS and
     * \c CHSM_MACHINE_INIT can be used to avoid having to deal with the many
     * constructor arguments, e.g.:
     * \code
     * class my_machine : public CHSM::machine {
     * public:
     *     my_machine( CHSM_MACHINE_ARGS ) : 
     *         CHSM::machine( CHSM_MACHINE_INIT ) {
     *         // ...
     *     }
     *     // ...
     * };
     * \endcode
     */
    machine( CHSM_MACHINE_ARGS );

private:
    /**
     * The type of the queue of events that have been broadcasted.
     */
    typedef std::list<event*> event_queue;

    /**
     * The entire set of states in the machine.  This is built and set by
     * the CHSM-to-C++ compiler.
     */
    state *const *const state_;

    /**
     * The outermost cluster encompassing the entire machine.
     */
    cluster &root_;

    /**
     * The entire set of transitions in the machine.  This is built and set
     * by the CHSM-to-C++ compiler.
     */
    transition const *const transition_;

    /**
     * The number of transitions in the machine.  This is set by the
     * CHSM-to-C++ compiler.
     */
    int const transitions_in_machine_;

    /**
     * This is an array to pointers to events indexed by transition IDs.  It's
     * used to mark a transition as "taken" so it's taken at most once per
     * transition algorithm execution for a given event.
     */
    event const **const taken_;

    /**
     * This is an array of pointers to states indexed by transition IDs.  It's
     * used store the states to transition to for a given transition.
     */
    state **const target_;

    /**
     * This is \c true only when the transition algorithm is in progress and is
     * used to prevent recursive calls.
     */
    bool in_progress_;

    event_queue         event_queue_;   //!< Events that have been broadcasted.
    unsigned            debug_;         //!< The current debugging state.
    static void         *const nil_;    //!< Sentinel for end().
    static int const    no_state_id_;   //!< Used by internal transitions.

#ifdef  CHSM_MULTITHREADED
    mutable pthread_mutex_t machine_lock_;
#endif  /* CHSM_MULTITHREADED */

    /**
     * Perform all the transitions for all events queued.
     */
    void algorithm();

    friend class event;
    friend class event::machine_lock;
    friend class parent;
    friend struct transition;
};

//=============================================================================

/**
 * A %parent \e is-an abstract state that has child states.
 * It is an implementation detail to factor common data members and member
 * functions for the CHSM::cluster and CHSM::set classes.
 *
 * @author Paul J. Lucas
 */
class parent : public state {
protected:
    /**
     * \internal
     *
     * The array of state IDs that are the child states.
     */
    typedef int const* child_list;

public:
#   define  CHSM_PARENT_ARG_LIST(A) \
            CHSM_STATE_ARG_LIST(A), \
            A(child_list) chsm_children_

    /**
     * Defines the constructor arguments for the CHSM::parent class.
     *
     * \hideinitializer
     */
#   define  CHSM_PARENT_ARGS    CHSM_PARENT_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::parent class.
     *
     * \hideinitializer
     */
#   define  CHSM_PARENT_INIT    CHSM_PARENT_ARG_LIST(CHSM_ACTUAL)

    /** The type of the value returned by iterator and const_iterator. */
    typedef state value_type;

    /** A pointer to a <code>value_type</code>. */
    typedef value_type* pointer;

    /** A pointer to a <code>const value_type</code>. */
    typedef value_type const* const_pointer;

    /** A reference to a <code>value_type</code>. */
    typedef value_type& reference;

    /** A reference to a <code>const value_type</code>. */
    typedef value_type const& const_reference;

    /**
     * Clears this parent's history.  However, since parents don't have a
     * history, there is nothing to do for them; however, call \c deep_clear()
     * for all child states.
     */
    virtual void deep_clear();

    /**
     * Returns whether this parent has any child states.
     * @return Returns \c true only if this parent has any child states.
     */
    bool empty() const {
        return *children_ == no_child_id_;
    }

    /**
     * Returns a parent::reference to the first child state.
     * @return Returns said reference.
     */
    reference front() {
        return *(machine_.state_)[*children_];
    }

    /**
     * Returns a parent::const_reference to the first child state.
     * @return Returns said reference.
     */
    const_reference front() const {
        return *(machine_.state_)[*children_];
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
         * Construct an iterator.
         */
        iterator() { }

        /**
         * Returns a parent::reference to the state the iterator is positioned
         * at.
         * @return Returns said reference.
         */
        reference operator* () const {
            return *p_[ *c_ ];
        }

        /**
         * Returns a parent::pointer to the state the iterator is positioned
         * at.
         * @return Returns said pointer.
         */
        pointer operator->() const {
            return p_[ *c_ ];
        }

        /**
         * Pre-increment the iterator.
         * @return Returns the iterator positioned at the next state, if any.
         */
        iterator& operator++() {
            return ++c_, *this;
        }

        /**
         * Post-increment the iterator.
         * @return Returns a new iterator positioned at the state the original
         * iterator was positioned at prior to the increment.
         */
        iterator operator++(int) {
            return iterator( p_, c_++ );
        }

        /**
         * Compare two iterators for equality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are positioned at
         * the same state.
         */
        friend bool operator==( iterator const &i, iterator const &j ) {
            return *i.c_ == *j.c_;
        }

        /**
         * Compare two iterators for inequality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are not positioned
         * at the same state.
         */
        friend bool operator!=( iterator const &i, iterator const &j ) {
            return !( i == j );
        }

    protected:
        pointer const   *p_;
        child_list      c_;
        iterator( pointer const *p, child_list c ) : p_( p ), c_( c ) { }
        friend class    parent;
    };

    /**
     * Returns a parent::iterator positioned at the first child state of a
     * parent.
     * @return Returns said iterator.
     */
    iterator begin() {
        return iterator( machine_.state_, children_ );
    }

    /**
     * Returns a parent::iterator positioned at one past the last child state
     * of a parent.
     * @return Returns said iterator.
     */
    iterator end() {
        return iterator( 0, &no_child_id_ );
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
         * Construct a const_iterator.
         */
        const_iterator() { }

        /**
         * Returns a parent::const_reference to the state the iterator is
         * positioned at.
         * @return Returns said reference.
         */
        const_reference operator*() const {
            return *p_[ *c_ ];
        }

        /**
         * Returns a parent::const_pointer to the state the iterator is
         * positioned at.
         * @return Returns said reference.
         */
        const_pointer operator->() const {
            return p_[ *c_ ];
        }

        /**
         * Pre-increment the iterator.
         * @return Returns the iterator positioned at the next state, if any.
         */
        const_iterator& operator++() {
            return ++c_, *this;
        }

        /**
         * Post-increment the iterator.
         * @return Returns a new iterator positioned at the state the original
         * iterator was positioned at prior to the increment.
         */
        const_iterator operator++(int) {
            return const_iterator( p_, c_++ );
        }

        /**
         * Compare two iterators for equality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are positioned at
         * the same state.
         */
        friend bool operator==( const_iterator const &i,
                                const_iterator const &j ) {
            return *i.c_ == *j.c_;
        }

        /**
         * Compare two iterators for inequality.
         * @param i The first iterator.
         * @param j The second iterator.
         * @return Returns \c true only if the two iterators are not positioned
         * at the same state.
         */
        friend bool operator!=( const_iterator const &i,
                                const_iterator const &j ) {
            return !( i == j );
        }

    protected:
        pointer const   *p_;
        child_list      c_;
        const_iterator( pointer const *p, child_list c ) : p_( p ), c_( c ) { }
        friend class    parent;
    };

    /**
     * Returns a parent::const_iterator positioned at the first child state of
     * a parent.
     * @return Returns said iterator.
     */
    const_iterator begin() const {
        return const_iterator( machine_.state_, children_ );
    }

    /**
     * Returns a parent::const_iterator positioned at one past the last child
     * state of a parent.
     * @return Returns said iterator.
     */
    const_iterator end() const {
        return const_iterator( 0, &no_child_id_ );
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
    /**
     * \internal
     *
     * Construct a parent.
     */
    parent( CHSM_PARENT_ARGS );

    /**
     * \internal
     *
     * The purpose of switch_child() is to allow a child state to ask
     * permission of its parent state to be entered and become the active
     * state.  The default behavior simply returns true; cluster overrides
     * it -- see its comment for further details.
     *
     * @param child The child to switch to.
     */
    virtual bool switch_child( state *child );

    // for access to switch_child()
    friend bool state::enter( event const&, state* );

private:
    child_list const    children_;      //!< The child states.
    static int const    no_child_id_;   //!< Sentinel for end().
};

//=============================================================================

/**
 * A %cluster \e is-a CHSM::parent that can have at most one child-state
 * active at any given time.
 *
 * A cluster may have a history.  If a cluster does not have a history, or it
 * does have a history but has not previously been active, then the child-state
 * entered after it itself is entered is the \e default child-state; if a
 * cluster does have a history and it has been visited before, then the
 * child-state entered after it itself is entered is the one that was last
 * active.
 *
 * A cluster may alternatively have a \e deep history.  Such a cluster behaves
 * exactly as one with an ordinary history; the difference is that all clusters
 * lexically-enclosed by it in the CHSM description also have a history.
 *
 * The %cluster class can be extended either to add additional data members and
 * member functions, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
class cluster : public parent {
public:
#   define  CHSM_CLUSTER_ARG_LIST(A) \
            CHSM_PARENT_ARG_LIST(A), \
            A(bool) chsm_history_

    /**
     * Defines the constructor arguments for the CHSM::cluster class.
     *
     * \hideinitializer
     */
#   define  CHSM_CLUSTER_ARGS   CHSM_CLUSTER_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::cluster class.
     *
     * \hideinitializer
     */
#   define  CHSM_CLUSTER_INIT   CHSM_CLUSTER_ARG_LIST(CHSM_ACTUAL)

    /**
     * Constructs a cluster.
     * When deriving a class from %cluster, the macros \c CHSM_CLUSTER_ARGS
     * \c CHSM_CLUSTER_INIT can be used to avoid having to deal with the many
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
     * Clear the history, if any; then clear all child cluster's history, if
     * any.
     */
    virtual void deep_clear();

    /**
     * Enters a %cluster via a transition and also enter one of its child
     * states.
     *
     * @param trigger The event that triggered the transition.
     * @param from_child If not \c null, this child state of the cluster is
     * being entered directly.
     * @return Returns \c true only if the %cluster was actually entered, i.e.,
     * it wasn't already active.
     */
    virtual bool enter( event const &trigger, state *from_child = 0 );

    /**
     * Exits a %cluster via a transition, but first exit its active child
     * state.
     *
     * @param trigger The event that triggered the transition.
     * @param to The state that is being transitioned to.
     * @return Returns \c true only if the %cluster actually exited, i.e., it
     * was active.
     */
    virtual bool exit( event const &trigger, state *to = 0 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

private:
    bool const  history_;               // do we have a history?
    state       *active_child_, *last_child_;

    // inherited
    virtual bool switch_child( state* );
};

//=============================================================================

/**
 * A %set \e is-a CHSM::parent that has either all or no child-states active.
 * All child-states are entered after and exited before their parent-state.
 * The order that child-states are entered and exited is undefined.
 *
 * The % set class can be extended either to add additional data members and
 * member functions, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
class set : public parent {
public:
#   define  CHSM_SET_ARG_LIST(A) \
            CHSM_PARENT_ARG_LIST(A)

    /**
     * Defines the constructor arguments for the CHSM::set class.
     *
     * \hideinitializer
     */
#   define  CHSM_SET_ARGS   CHSM_SET_ARG_LIST(CHSM_FORMAL)

    /**
     * Defines the base-class constructor argument \e mem-initializers for the
     * CHSM::set class.
     *
     * \hideinitializer
     */
#   define  CHSM_SET_INIT   CHSM_SET_ARG_LIST(CHSM_ACTUAL)

    /**
     * Constructs a set.
     * When deriving a class from \c set, the macros \c CHSM_SET_ARGS and
     * \c CHSM_SET_INIT can be used to avoid having to deal with the many
     * constructor arguments.  See CHSM::state for an example.
     */
    set( CHSM_SET_ARGS ) : parent( CHSM_PARENT_INIT ) { }

    /**
     * Enters a %set via a transition and also enter all of its child states.
     *
     * @param trigger The event that triggered the transition.
     * @param from_child Not used here.
     * @return Returns \c true only if the %set was actually entered, i.e., it
     * wasn't already active.
     */
    virtual bool enter( event const &trigger, state *from_child = 0 );

    /**
     * Exits a set via a transition, but first exit all of its child states.
     *
     * @param trigger The event that triggered the transition.
     * @param to The state we are transitioning to.
     * @return Returns \c true only if the %set actually exited, i.e., it was
     * active.
     */
    virtual bool exit( event const &trigger, state *to = 0 );
};

////////// Inlines ////////////////////////////////////////////////////////////

inline event::const_iterator event::begin() const {
    //
    // This has to be defined down here so that the declaration for the machine
    // class has been seen by the C++ compiler.
    //
    return const_iterator( machine_.transition_, transitions_, base_event_ );
}

#ifdef  CHSM_MULTITHREADED
inline event::machine_lock::machine_lock( machine &m ) :
    mutex_lock( m.machine_lock_ )
{
}
#endif  /* CHSM_MULTITHREADED */

inline bool machine::active() const {
    //
    // A machine as a whole is active only if its root cluster is.
    //
    return root_.active();
}

inline bool transition::is_internal() const {
    return to_id_ == machine::no_state_id_ && !target_;
}

} // namespace

////////// Namespace stuff ////////////////////////////////////////////////////

/**
 * If, for whatever reason, you don't want the long namespace name aliased to a
 * shorter one, define \c CHSM_NO_ALIAS_NS.
 */
#ifndef CHSM_NO_ALIAS_NS
    /**
     * If you want the short namespace alias name to be something other
     * than "CHSM", define \c CHSM_NS_ALIAS to be what you want.
     */
#   ifndef  CHSM_NS_ALIAS
#   define  CHSM_NS_ALIAS CHSM
#   endif

namespace CHSM_NS_ALIAS = CHSM_NS;

#endif  /* CHSM_NO_ALIAS_NS */

#endif  /* chsm_H */
/* vim:set et sw=4 ts=4: */
