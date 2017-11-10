/*
**      CHSM Language System
**      java/lib/CHSM/Cluster.java -- Run-Time library implementation
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

/**
 * A {@code Cluster} <i>is-a</i> {@link Parent} that can have at most one
 * child-state active at any given time.
 * <p>
 * A cluster may have a history.  If a cluster does not have a history, or it
 * does have a history but has not previously been active, then the child-state
 * entered after it itself is entered is the <i>default</i> child-state; if a
 * cluster does have a history and it has been visited before, then the
 * child-state entered after it itself is entered is the one that was last
 * active.
 * <p>
 * A cluster may alternatively have a <i>deep</i> history.  Such a cluster
 * behaves exactly as one with an ordinary history; the difference is that all
 * clusters lexically-enclosed by it in the CHSM description also have a
 * history.
 * <p>
 * The {@code Cluster} class can be extended either to add additional data
 * members and methods, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
public class Cluster extends Parent {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Constructs a {@code Cluster}.
     * 
     * When deriving a class from {@code Cluster} in the declarations section
     * in a CHSM description (<tt>.chsmj</tt>) file, the &quot;macros&quot;
     * {@code CHSM_CLUSTER_ARGS} and {@code CHSM_CLUSTER_INIT} can be used to
     * avoid having to deal with the many constructor arguments.  See
     * {@link State} for an example.
     *
     * @param m The {@link Machine} this cluster belongs to.
     * @param name The name of this cluster.
     * @param p The {@link Parent} of this cluster, or {@code null} if none.
     * @param enterAction The action to be performed upon entrance to this
     * cluster, or {@code null} if none.
     * @param exitAction The action to be performed upon exit from this
     * cluster, or {@code null} if none.
     * @param enterEvent The {@link Event} to be broadcast upon entrance to
     * this cluster.
     * @param exitEvent The {@link Event} to be broadcast upon exit from this
     * cluster.
     * @param children An array of indicies into the {@link Machine}'s
     * {@link State} array that are the child states of this cluster.
     * @param history True only if this cluster has a history.
     */
    public Cluster( Machine m, String name, Parent p, Action enterAction,
                    Action exitAction, Event enterEvent, Event exitEvent,
                    int[] children, boolean history ) {
        super(
            m, name, p,
            enterAction, exitAction, enterEvent, exitEvent,
            children
        );
        activeChild_ = lastChild_ = null;
        history_ = history;
    }

    /**
     * Clears the history.
     */
    public final void clear() {
        lastChild_ = firstChild();
    }

    /**
     * Clears the history, if any; then clear all child cluster's history, if
     * any.
     */
    public final void deepClear() {
        clear();
        super.deepClear();
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Enters a cluster and also enter one of its child states.
     *
     * @param trigger The {@link Event} that triggered the {@link Transition}.
     * @param fromChild If not {@code null}, this child state of the cluster is
     * being entered directly.
     * @return Returns {@code true} only if the cluster was actually entered,
     * i.e., it wasn't already active.
     * @see #exit(Event,State)
     */
    protected boolean enter( Event trigger, State fromChild ) {
        if ( !super.enter( trigger, null ) )
            return false;

        if ( empty() )                  // guard against degenerate case
            return true;

        if ( fromChild != null ) {
            //
            // One of our child states is being entered directly: therefore, we
            // must merely make a note of that fact; it will take care of
            // entering itself.
            //
            activeChild_ = lastChild_ = fromChild;
        } else {
            if ( lastChild_ == null || !history_ ) {
                //
                // We were never active before (and thus have no lastChild_) or
                // we have no history: in either case, we must enter our
                // default child state.
                //
                lastChild_ = firstChild();
            }
            (activeChild_ = lastChild_).enter( trigger, null );
        }

        return true;
    }

    /**
     * Exits a cluster, but first exit its active child state.
     *
     * @param trigger The {@link Event} that triggered the {@link Transition}.
     * @param to The {@link State} that is being transitioned to.
     * @return Returns {@code true} only if the cluster actually exited, i.e.,
     * it was active.
     * @see #enter(Event,State)
     */
    protected boolean exit( Event trigger, State to ) {
        if ( !active() )
            return false;
        activeChild_.exit( trigger, null );     // exit our active child 1st
        return super.exit( trigger, to );
    }

    /**
     * Switches which child state of this cluster is the active child state.
     * This is used only in {@link State#enter(Event,State)}.
     *
     * @param child The {@link State} to switch this cluster's active child
     * state to.
     * @return Returns {@code true} only if the active child was switched.
     */
    protected final boolean switchChild( State child ) {
        if ( activeChild_.active() ) {
            //
            // We're being asked by a currently-inactive child state to become
            // our active child state, but we already have an active child
            // state -- permission denied.
            //
            return false;
        }
        activeChild_ = lastChild_ = child;
        return true;
    }

    ////////// private ////////////////////////////////////////////////////////

    private final boolean history_;
    private State activeChild_;
    private State lastChild_;
}
/* vim:set et sw=4 ts=4: */
