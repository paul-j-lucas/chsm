/*
**      CHSM Language System
**      Machine.java -- Run-Time library implementation
**
**      Copyright (C) 2004-2018  Paul J. Lucas & Fabio Riccardi
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

package CHSM;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;

///////////////////////////////////////////////////////////////////////////////

/**
 * A {@code Machine} contains an entire Concurrent, Hierarchical, Finite State
 * machine.  Every machine is self-contained: there may be multiple instances
 * of the same machine (or different machines) in different states in the same
 * program.
 *
 * @author Paul J. Lucas
 */
public abstract class Machine implements Iterable<State> {

    ////////// public /////////////////////////////////////////////////////////

    /** Do not print any debugging information. */
    public static final int DEBUG_NONE          = 0x00;

    /** Prints entrances to and exits from states. */
    public static final int DEBUG_ENTER_EXIT  = 0x01;

    /** Prints events queueing and dequeuing. */
    public static final int DEBUG_EVENTS    = 0x02;

    /** Reports progress during the broadcast algorithm. */
    public static final int DEBUG_ALGORITHM = 0x04;

    /** Reports all debugging information. */
    public static final int DEBUG_ALL =
        DEBUG_ENTER_EXIT | DEBUG_EVENTS | DEBUG_ALGORITHM;

    /**
     * A machine as a whole is active only if its root {@link Cluster} is.
     *
     * @return Returns {@code true} only if this {@code Machine} is active.
     */
    public final boolean active() {
        return root_.active();
    }

    /**
     * Gets the current debugging state.
     *
     * @return Returns said state.
     * @see #debug(int)
     */
    public final int debug() {
        return debugState_;
    }

    /**
     * Sets the debugging state.  Debugging information is printed to system
     * error.
     *
     * @param state The state to set that is the bitwise &quot;or&quot; of the
     * debugging states {@link #DEBUG_ENTER_EXIT}, {@link #DEBUG_EVENTS},
     * {@link #DEBUG_ALGORITHM}, or {@link #DEBUG_ALL}.
     * @return Returns the previous debugging state.
     * @see #debug()
     */
    public final int debug( int state ) {
        final int temp = debugState_;
        debugState_ = state;
        return temp;
    }

    /**
     * Dumps a printout of the current state to standard error.  The dump
     * consists of each state's name, one per line, preceded by an asterisk
     * only if it is active; a space otherwise.
     */
    public final synchronized void dumpState() {
        echo( "current state:" ).endl();
        for ( State s : this )
            echo( " " + (s.active() ? '*' : ' ') + s.name() ).endl();
    }

    /**
     * Enters this {@code Machine} by entering its root {@link Cluster}.
     *
     * @return Returns {@code true} only if the machine was actually entered,
     * i.e., it wasn't already active.
     * @see #exit()
     */
    public boolean enter() {
        return root_.enter( prime_, null );
    }

    /**
     * Exits this {@code Machine} by exiting its root {@link Cluster}.
     *
     * @return Returns {@code true} only if the machine was actually exited,
     * i.e., it wasn't already inactive.
     * @see #enter()
     */
    public boolean exit() {
        return root_.exit( prime_, null );
    }

    /**
     * Constructs a new iterator over this {@code Machine}'s {@link State}s.
     *
     * @return Returns said iterator.
     */
    public final Iterator<State> iterator() {
        return new MachineIterator();
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * The value used to indicate no state ID.  It's used for the ID of the
     * target state for an internal transition.
     */
    static final int NO_STATE_ID = -1;

    /**
     * The current debugging state.
     */
    int debugState_;

    /**
     * The current debugging indentation level.
     */
    int debugIndent_;

    /**
     * The {@link Event} queue during a broadcast.
     */
    final List<Event> eventQueue_ = new ArrayList<Event>();

    /**
     * The entire set of {@link State}s in the machine.
     */
    State[] state_;

    /**
     * An array of {@link Event}s indexed by transition IDs.  It's used to mark
     * a {@link Transition} as &quot;taken&quot; so it's taken at most once per
     * transition algorithm execution for a given event.
     */
    Event[] taken_;

    /**
     * The target function to be evaluated prior to a {@link Transition} being
     * performed to determine the {@link State} to transition to, if any.
     */
    State[] target_;

    /**
     * The entire set of {@link Transition}s in the machine.
     */
    Transition[] transition_;

    /**
     * Performs all the {@link Transition}s for all {@link Event}s queued.
     */
    final void algorithm() {
        if ( inProgress_ )
            return;
        inProgress_ = true;

        if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
            echo( "ALGORITHM BEGINNING" ).endl();
            ++debugIndent_;
        }

        while ( !eventQueue_.isEmpty() ) {
            final int eventsInStep = eventQueue_.size();
            int i;
            Iterator<Event> ei;

            if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                echo( "events in micro-step: " + eventsInStep ).endl();

            //
            // Phase I: Exit the "from" states
            //
            // Iterate through all the events and their transitions and exit
            // the "from" states.  Also perform the transitions' actions, if
            // any.  Note: We needn't worry about ensuring parent transitions
            // dominate those of children since the CHSM-to-Java compiler
            // defines transitions in just the right order.
            //
            if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                echo( "ALGORITHM PHASE I: Exit \"from\" states" ).endl();
                ++debugIndent_;
            }

            for ( i = 0, ei = eventQueue_.iterator(); i < eventsInStep; ++i ) {
                final Event e = ei.next();
                //
                // Set our base events' ParamBlocks to share ours.
                //
                for ( Event e2 = e.baseEvent_; e2 != null; e2 = e2.baseEvent_ )
                    e2.paramBlock_ = e.paramBlock_;

                if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                    echo( "iterating transitions of: " + e.name() ).endl();
                    ++debugIndent_;
                }

                //noinspection ForLoopReplaceableByForEach
                for ( Event.Iterator ti = e.iterator(); ti.hasNext(); ) {
                    final Transition t = ti.next();
                    if ( taken_[ ti.id() ] != e )
                        continue;
                    final State from = state_[ t.fromID_ ];
                    if ( !from.active() ) {
                        //
                        // If the "from" state isn't active, it means a
                        // previous (outer) transition dominated it: unmark
                        // this transition as taken.
                        //
                        taken_[ ti.id() ] = null;
                        continue;
                    }

                    if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                        if ( !t.isInternal() ) {
                            echo(
                                "performing : " + from.name() +
                                " -> " + target_[ ti.id() ].name()
                            ).endl();
                        }
                        ++debugIndent_;
                    }

                    //
                    // If the transition isn't internal, exit the "from" state.
                    // If it actually exited and there's an action to perform,
                    // perform it.
                    //
                    if ( (t.isInternal() || from.exit( e, target_[ ti.id() ] ))
                        && t.action_ != null )
                    {
                        if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                            echo( "performing action" ).endl();
                            ++debugIndent_;
                        }

                        t.action_.exec( e );

                        if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                            --debugIndent_;
                    }
                    if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                        --debugIndent_;
                }
                if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                    --debugIndent_;
            }

            //
            // Phase II: Enter the "to" states
            //
            // Iterate through all the events and their transitions again to
            // enter the inactive "to" states.  Also unmark the transitions.
            //
            if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                --debugIndent_;
                echo( "ALGORITHM PHASE II: Enter \"to\" states" ).endl();
                ++debugIndent_;
            }

            for ( i = 0, ei = eventQueue_.iterator(); i < eventsInStep; ++i ) {
                final Event e = ei.next();

                if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                    echo( "iterating transitions of: " + e.name() ).endl();
                    ++debugIndent_;
                }

                //noinspection ForLoopReplaceableByForEach
                for ( Event.Iterator ti = e.iterator(); ti.hasNext(); ) {
                    final Transition t = ti.next();
                    if ( taken_[ ti.id() ] != e )
                        continue;

                    if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                        if ( !t.isInternal() ) {
                            echo(
                                "performing : " + state_[ t.fromID_ ].name() +
                                " -> " + target_[ ti.id() ].name()
                            ).endl();
                        }
                        ++debugIndent_;
                    }

                    if ( !t.isInternal() ) {
                        //
                        // This is a real transition as opposed to an internal
                        // one -- enter the "to" state.
                        //
                        target_[ ti.id() ].enter( e, null );
                    }
                    taken_[ ti.id() ] = null;

                    if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                        --debugIndent_;
                }
                if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                    --debugIndent_;
            }

            //
            // Phase III: Dequeue events
            //
            // All the events in the current micro-step have now been processed
            // -- remove them from the queue.
            //
            if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
                --debugIndent_;
                echo( "ALGORITHM PHASE III: dequeue events" ).endl();
                ++debugIndent_;
            }

            for ( i = 0; i < eventsInStep; ++i ) {
                final Event e = eventQueue_.get( 0 );
                eventQueue_.remove( 0 );
                e.broadcasted();

                if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                    echo( "dequeued: " + e.name() ).endl();
            }
            if ( (debugState_ & DEBUG_ALGORITHM) != 0 )
                --debugIndent_;
        }

        inProgress_ = false;

        if ( (debugState_ & DEBUG_ALGORITHM) != 0 ) {
            --debugIndent_;
            echo( "ALGORITHM COMPLETE" ).endl();
        }
    }

    /**
     * Prints debugging information.
     *
     * @return Returns {@code this}.
     */
    final Machine echo() {
        System.err.print( "|" );
        for ( int i = debugIndent_ * debugIndentSize_; i > 0; --i )
            System.err.print( " " );
        return this;
    }

    /**
     * Prints debugging information.
     *
     * @param s The string to print.
     * @return Returns {@code this}.
     */
    final Machine echo( String s ) {
        echo();
        System.err.print( s );
        return this;
    }

    /**
     * Prints a carriage return.
     *
     * @return Returns {@code this}.
     */
    final Machine endl() {
        System.err.println();
        return this;
    }

    /**
     * Prints debugging information.
     *
     * @param s The string to print.
     * @return Returns {@code this}.
     */
    final Machine more( String s ) {
        System.err.print( s );
        return this;
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Constructs a {@code Machine}.
     */
    protected Machine() {
        // do nothing
    }

    /**
     * Initializes this machine.  You should never need to call this explicitly.
     * It is called via code generated by the CHSM-to-Java compiler.
     *
     * @param root The {@link Cluster} to use as the root.
     * @param states All the {@link State}s in the machine.
     * @param transitions All the {@link Transition}s in the machine.
     */
    protected final void init( Cluster root, State[] states,
                               Transition[] transitions ) {
        final int tMinSize = transitions.length > 0 ? transitions.length : 1;
        root_       = root;
        state_      = states;
        taken_      = new Event[ tMinSize ];
        target_     = new State[ tMinSize ];
        transition_ = transitions;
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * This iterator is used to iterate over all the {@link State}s of a
     * {@link Machine}.
     */
    private final class MachineIterator implements Iterator<State> {

        ////////// public /////////////////////////////////////////////////////

        /**
         * Determines whether there is a next {@link State} to be iterated over.
         *
         * @return Returns {@code true} only if there is another {@link State}
         * to be iterated over.
         */
        public boolean hasNext() {
            return i_ < state_.length;
        }

        /**
         * Returns the next child {@link State} and advances the iterator.
         *
         * @return Returns said state.
         * @throws NoSuchElementException Thrown only if there are no more
         * states.
         */
        public State next() {
            if ( !hasNext() )
                throw new NoSuchElementException();
            return state_[ i_++ ];
        }

        /**
         * Attempts to remove the current child {@link State} from the
         * {@link Machine}.
         *
         * @throws UnsupportedOperationException Always thrown since removal of
         * states is not supported.
         */
        public void remove() {
            throw new UnsupportedOperationException(
                "states can not be removed from machines"
            );
        }

        ////////// package ////////////////////////////////////////////////////

        /**
         * Constructs a {@code MachineIterator}.
         */
        MachineIterator() {
            i_ = 0;
        }

        ////////// private ////////////////////////////////////////////////////

        /**
         * The index into the {@link Machine#state_} array.
         */
        private int i_;
    }

    private static final int debugIndentSize_ = 2;  // spaces per indent

    /** A flag to indicate when the transition algorithm is in progress. */
    private boolean inProgress_;

    /** Used to start the machine. */
    private static final Event prime_ =
        new Event( null, null, "<prime>", null );

    /** The outermost {@link Cluster} encompassing the entire machine. */
    private Cluster root_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=4 ts=4: */
