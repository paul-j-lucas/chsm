/*
** CHSM/Groovy
**
** Copyright (C) 2007-2017  Paul J. Lucas & Fabio Riccardi
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
**      1. Redistributions of source code must retain the above copyright
**         notice, this list of conditions and the following disclaimer.
**
**      2. Redistributions in binary form must reproduce the above copyright
**         notice, this list of conditions and the following disclaimer in the
**         documentation and/or other materials provided with the distribution.
**
**      3. The names of its contributors may not be used to endorse or promote
**         products derived from this software without specific prior written
**         permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

package CHSM.groovy;

import CHSM.*;
import groovy.lang.Closure;

/**
 * A {@code CG_Transition} is a CHSM/Groovy transition that is used during CHSM
 * construction by a {@link CHSM_Builder}.
 *
 * @author Paul J. Lucas
 */
final class CG_Transition {

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Constructs a new {@code CG_Transition}.
     *
     * @param id The ID of this transition.
     * @param from The &quot;from&quot; {@link CG_State}.
     * @param to The &quot;to&quot; {@link CG_State}.
     * @param condition The condition or {@code null} if none.
     * @param action The action or {@code null} if none.
     */
    CG_Transition( int id, CG_State from, CG_State to, Closure condition,
                   Closure action ) {
        assert id >= 0;
        assert from != null;
        checkClosure( action );
        checkClosure( condition );
        m_cgAction = action;
        m_cgCondition = condition;
        m_cgFrom = from;
        m_cgTo = to;
        m_id = id;
    }

    /**
     * Gets the ID of this transition.
     *
     * @return Returns said ID.
     * @see #setID(int)
     */
    int getID() {
        return m_id;
    }

    /**
     * Sets the ID of this transition.
     *
     * @param id The ID.
     * @see #getID()
     */
    void setID( int id ) {
        assert id >= 0;
        m_id = id;
    }

    /**
     * Converts this {@code CG_Transition} to a {@link Transition}.
     *
     * @return Returns a new {@link Transition} corresponding to this
     * {@code CG_Transition}.
     */
    Transition toCHSM() {
        final Transition.Condition condition;
        if ( m_cgCondition != null )
            condition = new Transition.Condition() {
                public boolean eval( Event event ) {
                    return (Boolean)m_cgCondition.call( event );
                }
            };
        else
            condition = null;

        final Transition.Action action;
        if ( m_cgAction != null )
            action = new Transition.Action() {
                public void exec( Event event ) {
                    m_cgAction.call( event );
                }
            };
        else
            action = null;

        return new Transition(
            condition,
            m_cgFrom.getID(),
            m_cgTo != null ? m_cgTo.getID() : -1,
            null,                       // TODO: target functions
            action
        );
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * Checks that the given {@link Closure} has the right number of parameters.
     *
     * @param closure The {@link Closure} to check.
     */
    private static void checkClosure( Closure closure ) {
        if ( closure != null ) {
            final int numParams = closure.getMaximumNumberOfParameters();
            if ( numParams != 1 )
                throw new IllegalArgumentException(
                    "transition closures must have 1 parameter: event"
                );
        }
    }

    private final Closure m_cgAction;
    private final Closure m_cgCondition;
    private final CG_State m_cgFrom;
    private final CG_State m_cgTo;
    private int m_id;
}
/* vim:set et sw=4 ts=4: */
