/*
**      CHSM Language System
**      java/lib/CHSM/Event.java -- Run-Time library implementation
**
**      Copyright (C) 2004-2013  Paul J. Lucas & Fabio Riccardi
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

package CHSM;

import java.util.NoSuchElementException;

/**
 * The occurrence of an &quot;event&quot; (&quot;broadcast&quot;) is that which
 * causes transitions in a {@link Machine}.  An {@code Event} has a name, may
 * optionally be derived from another, and may optionally have parameters
 * during a broadcast.
 *
 * @author Paul J. Lucas
 */
public class Event implements Iterable<Transition> {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Iterates over all the {@link Transition}s that can possibly be taken
     * when an {@link Event} is broadcast.
     */
    public static final class Iterator
        implements java.util.Iterator<Transition> {

        ////////// public /////////////////////////////////////////////////////

        /**
         * Tests whether there is another {@link Transition} to iterate on.
         *
         * @return Returns {@code true} only if there is another
         * {@link Transition} to iterate on.
         */
        public boolean hasNext() {
            if ( next_ == null && i_ >= 0 ) {
                ++i_;
                setNext();
            }
            return next_ != null;
        }

        /**
         * Returns the {@link Transition}'s ID assigned by the CHSM-to-Java
         * compiler.
         *
         * @return Returns said ID or {@code -1} for none.
         */
        public int id() {
            return i_ < tID_.length ? tID_[ i_ ] : NO_TRANSITION_ID;
        }

        /**
         * Returns the next {@link Transition} and advances the iterator.
         *
         * @return Returns said {@link Transition}.
         * @throws NoSuchElementException Thrown only if there are no more
         * transitions.
         */
        public Transition next() {
            if ( next_ == null && !hasNext() )
                throw new NoSuchElementException();
            final Transition temp = next_;
            next_ = null;
            return temp;
        }

        /**
         * Attempts to remove a {@link Transition} from the {@link Event}.
         *
         * @throws UnsupportedOperationException Always thrown since
         * removal of transitions is not permitted.
         */
        public void remove() {
            throw new UnsupportedOperationException(
                "transitions can not be removed from events"
            );
        }

        ////////// package ////////////////////////////////////////////////////

        /**
         * Constructs a new {@code Iterator}.
         *
         * @param allTransitions The entire set of all the {@link Transition}s
         * in the {@link Machine}.
         * @param tID The array of transition IDs to iterate over.
         * @param baseEvent The event's base event, if any.
         */
        Iterator( Transition[] allTransitions, int[] tID, Event baseEvent ) {
            baseEvent_ = baseEvent;
            i_         = 0;
            t_         = allTransitions;
            tID_       = tID;
            setNext();
        }

        ////////// private ////////////////////////////////////////////////////

        /**
         * Sets the {@code next_} pointer.
         */
        private void setNext() {
            while ( id() == NO_TRANSITION_ID && baseEvent_ != null ) {
                //
                // We've exhausted our transitions; proceed to those of our
                // base event's, if any.
                //
                tID_ = baseEvent_.tIDs_;
                baseEvent_ = baseEvent_.baseEvent_;
                i_ = 0;
            }
            next_ = id() == NO_TRANSITION_ID ? null : t_[ id() ];
        }

        /**
         * The base event (if any) of the event whose transitions are being
         * iterated over.
         */
        private Event baseEvent_;

        /**
         * The index into the {@link #tID_} array.
         */
        private int i_;

        /**
         * The entire set of all the {@link Transition}s in the {@link Machine}.
         */
        private final Transition[] t_;

        /**
         * The IDs of the transitions of the event whose transitions are being
         * iterated over.
         */
        private int[] tID_;

        /**
         * The next {@link Transition} to return via {@link #next()}.
         */
        private Transition next_;
    }

    /**
     * A {@code ParamBlock} holds the parameters for an {@link Event}.
     */
    public static class ParamBlock {

        ////////// public /////////////////////////////////////////////////////

        /**
         * Constructs a {@code ParamBlock}.  You should never need to call this
         * explicitly.  It is called via code generated by the CHSM-to-Java
         * compiler.
         *
         * @param e The {@link Event} to hold parameters for.
         */
        public ParamBlock( Event e ) {
            event_ = e;
        }

        ////////// protected //////////////////////////////////////////////////

        /**
         * Evaluates the precondition for a transition on an {@link Event}.
         * You should never need to call this explicitly.  It is called via
         * code generated by the CHSM-to-Java compiler.
         *
         * @return The default always returns {@code true}.
         */
        protected boolean precondition() {
            return true;
        }

        /**
         * The {@link Event} this {@code ParamBlock} is for.
         */
        protected Event event_;
    }

    /**
     * Constructs an {@code Event}.  You should never need to call this
     * explicitly.  It is called via code generated by the CHSM-to-Java
     * compiler.
     *
     * @param m The {@link Machine} to belong to.
     * @param tIDs The IDs of the {@link Transition}s this event transitions on.
     * @param name The name of this event.
     * @param b The base event of this event or {@code null}.
     */
    public Event( Machine m, int[] tIDs, String name, Event b ) {
        baseEvent_  = b;
        machine_    = m;
        name_       = name;
        tIDs_       = tIDs;
    }

    /**
     * Broadcasts an event to a {@link Machine}.  If the event has a
     * precondition, it will be evaluated first.  Actions may be performed and
     * transitions may occur.
     *
     * @param pb The {@link ParamBlock} containing the parameters for the event,
     * if any.
     */
    public void broadcast( ParamBlock pb ) {
        synchronized ( machine_ ) {
            if ( inProgress_ != 0 )
                return;
            //
            // Mark ourselves and our base event(s), if any, as being in
            // progress.
            //
            for ( Event e = this; e != null; e = e.baseEvent_ )
                ++e.inProgress_;

            if ( (machine_.debugState_ & Machine.D_EVENT) != 0 )
                machine_.echo( "broadcast: " + name() ).endl();

            cancel_broadcast: {
                if ( (paramBlock_ = pb) != null ) {
                    //
                    // Evaluate the event's precondition.  Note that since it's
                    // a virtual function, the most-derived precondition gets
                    // executed.  However, the code emitted by the CHSM-to-Java
                    // compiler is of the form:
                    //
                    //      return  base_event.precondition() &&
                    //              user_precondition();
                    //
                    // so the most-base precondition really gets evaluated
                    // first.  If a base event's precondition is false, the
                    // short-circuit semantics of && means no more derived
                    // preconditions get evaluated and the call-stack simply
                    // unwinds.  The final result is that an event's
                    // precondition is the logical and of all base
                    // preconditions executed in base-to-derived order.
                    //
                    final boolean result = pb.precondition();

                    if ( (machine_.debugState_ & Machine.D_EVENT) != 0 )
                        machine_.echo( "+ precondition: " + result ).endl();

                    if ( !result ) {
                        //
                        // This event's precondition evaluated to false --
                        // cancel its broadcast.
                        //
                        break cancel_broadcast;
                    }
                }

                boolean foundTransition = false;

                if ( (machine_.debugState_ & Machine.D_EVENT) != 0 )
                    machine_.echo( "+ checking transitions" ).endl();

                for ( Iterator i = iterator(); i.hasNext(); ) {
                    final Transition t = i.next();
                    final State from = machine_.state_[ t.fromID_ ];
                    //
                    // We must check specifically for "plain active" so we
                    // don't make more than one nondeterministic transition
                    // from the same state.  This is the only place in the code
                    // that makes a distinction between "plain active" and
                    // "active disabled."
                    //
                    if ( from.state_ != State.S_ACTIVE || (
                        //
                        // If the transition has a condition, execute it to see
                        // whether we should continue.
                        //
                        t.condition_ != null && !t.condition_.eval( this )
                    ) )
                        continue;

                    //
                    // Mark this transition as taken using the event that
                    // triggered it.
                    //
                    if ( machine_.taken_[ i.id() ] == null )
                        machine_.taken_[ i.id() ] = this;

                    if ( !t.isInternal() ) {
                        //
                        // This is not an internal transition -- leave "from"
                        // state active, but mark as disabled to prevent making
                        // more than one nondeterministic transition from the
                        // same state.
                        //
                        from.state_ = State.S_ACTIVE_DISABLED;

                        if ( t.target_ != null ) {
                            //
                            // The transition has a target function: execute it
                            // to compute the target state.
                            //
                            final State to = t.target_.eval( this );
                            if ( to != null ) {
                                assert Transition.isLegal( from, to );
                                machine_.target_[ i.id() ] = to;
                            } else {
                                //
                                // The target function returned null: skip this
                                // transition.
                                //
                                continue;
                            }
                        } else {
                            //
                            // The transition goes to a hard-coded target state.
                            //
                            machine_.target_[ i.id() ] =
                                machine_.state_[ t.toID_ ];
                        }
                    }

                    foundTransition = true;

                    if ( (machine_.debugState_ & Machine.D_EVENT) != 0 ) {
                        machine_.echo( "+ found  : " + from.name() );
                        if ( !t.isInternal() )
                            machine_.more(
                                " -> " + machine_.target_[ i.id() ].name()
                            );
                        else
                            machine_.more( " (internal)" );
                        machine_.endl();
                    }
                }

                if ( !foundTransition ) {
                    //
                    // This event has no possible transitions to take -- cancel
                    // its broadcast.
                    //
                    break cancel_broadcast;
                }

                //
                // Queue ourselves and run algorithm.
                //
                machine_.eventQueue_.add( this );

                if ( (machine_.debugState_ & Machine.D_EVENT) != 0 )
                    machine_.echo( "queued   : " + name() ).endl();

                machine_.algorithm();
                return;
            }

            //
            // This event is not to be queued: return.
            //
            if ( (machine_.debugState_ & Machine.D_EVENT) != 0 )
                machine_.echo(
                    "broadcast: " + name() + " -- cancelled"
                ).endl();
            broadcasted();
        }
    }

    /**
     * Constructs an {@link Iterator} for this event's {@link Transition}s.
     *
     * @return Returns said {@link Iterator}.
     */
    public final Iterator iterator() {
        return new Iterator( machine_.transition_, tIDs_, baseEvent_ );
    }

    /**
     * Returns the {@link Machine} this event belongs to.
     *
     * @return Returns said {@link Machine}.
     */
    public final Machine machine() {
        return machine_;
    }

    /**
     * Returns the name of this event.
     *
     * @return Returns said name.
     */
    public final String name() {
        return name_;
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * This event's base event, if any; or {@code null} if none.
     */
    protected final Event baseEvent_;

    /**
     * If this event has parameters, this will point to its {@link ParamBlock};
     * {@code null} otherwise.
     */
    protected ParamBlock paramBlock_;

    ////////// package ////////////////////////////////////////////////////////

    /**
     * The value used to indicate no transition ID.
     */
    static final int NO_TRANSITION_ID = -1;

    /**
     * Do post-broadcast clean-up for a broadcasted event.
     */
    final void broadcasted() {
        //
        // This check isn't strictly necessary...but I feel better having it
        // here.
        //
        if ( inProgress_ == 0 )
            return;
        //
        // Unmark ourselves and our base events, if any, as being in progress.
        //
        for ( Event e = this; e != null; e = e.baseEvent_ )
            if ( e.inProgress_ != 0 )
                --e.inProgress_;

        paramBlock_ = null;
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * The {@code inProgress_} &quot;flag&quot; is actually a counter because,
     * during a transition, the same event (or an event derived from it) can be
     * broadcast more than once; however, nothing must be done for
     * &quot;nested&quot; broadcasts, i.e., when it's > 0.
     */
    private int inProgress_;

    /**
     * The {@link Machine} this event belongs to.
     */
    private final Machine machine_;

    /**
     * The name of this event.
     */
    private final String name_;

    /**
     * The IDs of all the {@link Transition}s that can possibly be taken when
     * this event is broadcast.
     */
    private final int[] tIDs_;
}
/* vim:set et sw=4 ts=4: */
