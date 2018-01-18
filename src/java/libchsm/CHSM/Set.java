/*
**      CHSM Language System
**      Set.java -- Run-Time library implementation
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

///////////////////////////////////////////////////////////////////////////////

/**
 * A {@code Set} <i>is-a</i> {@link Parent} that has either all or no
 * child-states active at any given time.  All child-states are entered after
 * and exited before their parent-state.  The order that child-states are
 * entered and exited is undefined.
 * <p>
 * The {@code Set} class can be extended either to add additional data members
 * and methods, or to alter its behavior upon being entered or exited.
 *
 * @author Paul J. Lucas
 */
public class Set extends Parent {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Constructs a {@code Set}.
     * 
     * When deriving a class from {@code Set} in the declarations section in a
     * CHSM description (<tt>.chsmj</tt>) file, the &quot;macros&quot;
     * {@code CHSM_SET_ARGS} and {@code CHSM_SET_INIT} can be used to avoid
     * having to deal with the many constructor arguments.  See {@link State}
     * for an example.
     *
     * @param m The {@link Machine} this set belongs to.
     * @param name The name of this set.
     * @param p The {@link Parent} of this set, or {@code null} if none.
     * @param enterAction The action to be performed upon entrance to this set,
     * or {@code null} if none.
     * @param exitAction The action to be performed upon exit from this set, or
     * {@code null} if none.
     * @param enterEvent The {@link Event} to be broadcast upon entrance to
     * this set.
     * @param exitEvent The {@link Event} to be broadcast upon exit from this
     * set.
     * @param children An array of indicies into the {@link Machine}'s
     * {@link State} array that are the child states of this set.
     */
    public Set( Machine m, String name, Parent p, Action enterAction,
                Action exitAction, Event enterEvent, Event exitEvent,
                int[] children ) {
        super(
            m, name, p, enterAction, exitAction, enterEvent, exitEvent, children
        );
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Enters a set and then also enters all of its child states.
     *
     * @param trigger The {@link Event} that triggered the {@link Transition}.
     * @param fromChild Not used here.
     * @return Returns {@code true} only if the set was actually entered, i.e.,
     * it wasn't already active.
     * @see #exit(Event,State)
     */
    protected boolean enter( Event trigger, State fromChild ) {
        if ( !super.enter( trigger, null ) )
            return false;

        // enter all of our children
        for ( State child : this )
            child.enter( trigger, null );

        return true;
    }

    /**
     * Exits a set, but first exits all of its child states.
     *
     * @param trigger The {@link Event} that triggered the {@link Transition}.
     * @param to The {@link State} we are transitioning to.
     * @return Returns {@code true} only if the set actually exited, i.e., it
     * was active.
     * @see #enter(Event,State)
     */
    protected boolean exit( Event trigger, State to ) {
        if ( !active() )
            return false;

        // exit all of our children
        for ( State child : this )
            child.exit( trigger, null );

        return super.exit( trigger, to );
    }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=4 ts=4: */
