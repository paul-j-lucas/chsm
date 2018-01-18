/*
**      CHSM Language System
**      Parent.java -- Run-Time library implementation
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

import java.util.Iterator;
import java.util.NoSuchElementException;

///////////////////////////////////////////////////////////////////////////////

/**
 * A {@code Parent} is an abstract {@link State} that has child states.
 *
 * @author Paul J. Lucas
 */
public abstract class Parent extends State implements Iterable<State> {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Clears this parent's history.  However, since parents don't have a
     * history, there is nothing to do for them; however, call
     * {@code deepClear()} for all child states.
     */
    public void deepClear() {
        for ( State child : this )
            child.deepClear();
    }

    /**
     * Gets whether this parent is empty, i.e., has no child states.
     *
     * @return Returns {@code true} only if this parent is empty.
     */
    public final boolean empty() {
        return children_ == null || children_.length == 0;
    }

    /**
     * Gets the first child state of this parent.
     *
     * @return Returns said state.
     */
    public final State firstChild() {
        return machine().state_[ children_[0] ];
    }

    /**
     * Constructs a new iterator over this parent's child states.
     *
     * @return Returns said iterator.
     */
    public final Iterator<State> iterator() {
        return new ParentIterator();
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Constructs a {@code Parent}.
     *
     * @param m The {@link Machine} this parent belongs to.
     * @param name The name of this parent.
     * @param p The parent of this parent, or {@code null} if none.
     * @param enterAction The action to be performed upon entrance to this
     * parent, or {@code null} if none.
     * @param exitAction The action to be performed upon exit from this parent,
     * or {@code null} if none.
     * @param enterEvent The {@link Event} to be broadcast upon entrance to
     * this parent.
     * @param exitEvent The {@link Event} to be broadcast upon exit from this
     * parent.
     * @param children An array of indicies into the {@link Machine}'s
     * {@link State} array that are the child states of this parent.
     */
    protected Parent( Machine m, String name, Parent p, Action enterAction,
                      Action exitAction, Event enterEvent, Event exitEvent,
                      int[] children ) {
        super( m, name, p, enterAction, exitAction, enterEvent, exitEvent );
        children_ = children;
    }

    /**
     * Allows a child state to ask permission of its parent state to be entered
     * and become the active state.  The default behavior simply returns
     * {@code true}; {@link Cluster} overrides it -- see its comment for
     * further details.
     *
     * @param notUsed Not used here.
     * @return The default always returns {@code true}.
     * @see Cluster#switchChild(State)
     */
    protected boolean switchChild( State notUsed ) {
        return true;
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * Iterates over all the child states of a parent.
     */
    private final class ParentIterator implements Iterator<State> {

        ////////// public /////////////////////////////////////////////////////

        /**
         * Determines whether there is a next child {@link State} to be
         * iterated over.
         *
         * @return Returns {@code true} only if there is another child state to
         * be iterated over.
         */
        public boolean hasNext() {
            return i_ < children_.length;
        }

        /**
         * Gets the next child {@link State} and advances the iterator.
         *
         * @return Returns said {@link State}.
         * @throws NoSuchElementException Thrown only if there are no more
         * child states.
         */
        public State next() {
            if ( !hasNext() )
                throw new NoSuchElementException();
            return machine().state_[ children_[ i_++ ] ];
        }

        /**
         * Attempts to remove the current child {@link State}.
         *
         * @throws UnsupportedOperationException Always thrown since removal of
         * child states is not permitted.
         */
        public void remove() {
            throw new UnsupportedOperationException(
                "child states can not be removed from states"
            );
        }

        ////////// package ////////////////////////////////////////////////////

        /**
         * Constructs a {@code ParentIterator}.
         */
        ParentIterator() {
            i_ = 0;
        }

        ////////// private ////////////////////////////////////////////////////

        /** The index into the {@link Parent#children_} array. */
        private int i_;
    }

    /**
     * The indicies of the child states.
     */
    private final int[] children_;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=4 ts=4: */
