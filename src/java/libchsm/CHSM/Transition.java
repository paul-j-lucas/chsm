/*
**      CHSM Language System
**      Transition.java -- Run-Time library implementation
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

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

/**
 * A {@code Transition} is a simple class containing information for a
 * transition from one {@link State} to another in a {@link Machine}.  The
 * information contains whether the transition has an associated condition to
 * test prior to taking it, the &quot;from&quot; and &quot;to&quot; states, and
 * an optional action to be performed.
 *
 * @author Paul J. Lucas
 */
public final class Transition {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * An {@code Action} is a function to be executed upon performing a
     * transition from one {@link State} to another.
     */
    public interface Action {
        /**
         * Executes the {@code Action}.  You should never need to call this
         * explicitly.  It is called via code generated by the CHSM-to-Java
         * compiler.
         *
         * @param trigger The {@link Event} that caused the {@link Transition}.
         */
        void exec( Event trigger );
    }

    /**
     * A {@code Condition} is a function to be evaluated prior to a
     * {@link Transition} being performed.  The {@link Transition} is performed
     * only if the function returns {@code true}.
     */
    public interface Condition {
        /**
         * Evalulates the {@code Condition}.  You should never need to call
         * this explicitly.  It is called via code generated by the
         * CHSM-to-Java compiler.
         *
         * @param trigger The {@link Event} that is casuing the
         * {@link Transition}.
         * @return Returns {@code true} only if the condition evaluates to
         * {@code true}.
         */
        boolean eval( Event trigger );
    }

    /**
     * A {@code Target} is a function to be evaluated prior to a
     * {@link Transition} being performed to determine the {@link State} to
     * transition to.
     */
    public interface Target {
        /**
         * Computes the target {@link State}.  You should never need to call
         * this explicitly.  It is called via code generated by the
         * CHSM-to-Java compiler.
         *
         * @param trigger The {@link Event} that is causing the
         * {@link Transition}.
         * @return Returns the {@link State} that the {@link Transition} should
         * to go or {@code null} to abort the transition.
         */
        State eval( Event trigger );
    }

    /**
     * Constructs a {@code Transition}.  You should never need to call this
     * explicitly.  It is called via code generated by the CHSM-to-Java
     * compiler.
     *
     * @param c The {@link Condition} to evaluate, if any.
     * @param fromID The ID of the {@link State} being transitioned from.
     * @param toID The ID of the {@link State} being transitioned to.
     * @param t The {@link Target} to evaluate, if any.
     * @param a The {@link Action} to perform, if any.
     */
    public Transition( Condition c, int fromID, int toID, Target t, Action a ) {
        condition_ = c;
        fromID_    = fromID;
        toID_      = toID;
        target_    = t;
        action_    = a;
    }

    /**
     * Checks whether a transition between the two given {@link State}s would
     * be legal.  A transition is illegal only if the from and to states'
     * nearest common ancestor is a {@link Set}.
     *
     * @param s1 One of the {@link State}s.
     * @param s2 The other {@link State}.
     * @return Returns {@code true} only if the transition is legal.
     */
    public static boolean isLegal( State s1, State s2 ) {
        final List<State> s1_list = new LinkedList<State>();
        final List<State> s2_list = new LinkedList<State>();

        for ( State s = s1; s != null; s = s.parent() )
            s1_list.add( 0, s );
        for ( State s = s2; s != null; s = s.parent() )
            s2_list.add( 0, s );

        final Iterator<State> i = s1_list.iterator();
        final Iterator<State> j = s2_list.iterator();

        State nearestCommonAncestor = null;
        while ( i.hasNext() && j.hasNext() ) {
            final State s = i.next();
            final State t = j.next();
            if ( s == t )
                nearestCommonAncestor = s;
            else
                break;
        }
        return !( nearestCommonAncestor instanceof Set );
    }

    ////////// package ////////////////////////////////////////////////////////

    final Condition condition_;
    final int       fromID_, toID_;
    final Target    target_;
    final Action    action_;

    /**
     * Checks whether this {@code Transition} is an internal transition.
     *
     * @return Returns {@code true} only if this {@code Transition} is
     * internal.
     */
    boolean isInternal() {
        return toID_ == Machine.NO_STATE_ID && target_ == null;
    }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=4 ts=4: */
