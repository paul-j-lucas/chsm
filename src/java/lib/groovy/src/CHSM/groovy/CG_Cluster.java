/*
** CHSM/Groovy
**
** Copyright (C) 2007-2013  Paul J. Lucas & Fabio Riccardi
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

import java.lang.reflect.Constructor;

import CHSM.*;
import groovy.lang.Closure;

/**
 * A {@code CG_Cluster} is-a {@link CG_Parent} that is used during CHSM
 * construction by a {@link CHSM_Builder} to build a {@link Cluster}.
 *
 * @author Paul J. Lucas
 */
public final class CG_Cluster extends CG_Parent {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Gets whether this {@code CG_Cluster} has a history.
     *
     * @return Returns {@code true} only if this {@code CG_Cluster} has a
     * history.
     */
    public boolean getHistory() {
        return m_hasHistory;
    }

    /**
     * {@inheritDoc}
     */
    public String getTypeName() {
        return "cluster";
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Constructs a {@code CG_Cluster}.
     *
     * @param stateClass The {@link Class} of the cluster to create or
     * {@code null} for the default.
     * @param fqName The fully qualified name of the cluster.
     * @param enterAction The &quot;enter&quot; action or {@code null} if none.
     * @param exitAction The &quot;exit&quot; action or {@code null} if none.
     */
    CG_Cluster( Class<? extends State> stateClass, String fqName,
                Closure enterAction, Closure exitAction ) {
        super(
            checkClass( stateClass, Cluster.class ), fqName,
            enterAction, exitAction
        );
    }

    /**
     * Sets whether this {@code CG_Cluster} has a history.
     *
     * @param hasHistory If {@code true}, this {@code CG_Cluster} shall have a
     * history.
     */
    void setHistory( boolean hasHistory ) {
        m_hasHistory = hasHistory;
    }

    /**
     * Converts this {@code CG_Cluster} to its corresponding {@link Cluster}.
     *
     * @return Returns said {@link Cluster}.
     */
    Cluster toCHSM() {
        try {
            final Constructor<? extends State> c = m_stateClass.getConstructor(
                Machine.class, String.class, Parent.class,
                State.Action.class, State.Action.class,
                Event.class, Event.class, int[].class, boolean.class
            );
            return (Cluster)c.newInstance(
                m_cgMachine.getMachine(), getName(),
                (Parent)m_cgMachine.getStateFor( getParent() ),
                convertAction( m_cgEnterAction ),
                convertAction( m_cgExitAction ),
                m_cgMachine.getEventFor( m_cgEnterEvent ),
                m_cgMachine.getEventFor( m_cgExitEvent ),
                convertChildren(), m_hasHistory
            );
        }
        catch ( Exception e ) {
            throw new IllegalStateException( e );
        }
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * Whether this {@code CG_Cluster} has a history.
     */
    private boolean m_hasHistory;
}
/* vim:set et sw=4 ts=4: */
