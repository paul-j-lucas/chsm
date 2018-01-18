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

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import CHSM.*;
import groovy.lang.Closure;

/**
 * A {@code CG_Event} is used both during CHSM construction by a
 * {@link CHSM_Builder} to build an {@link Event} and during run-time to
 * broadcast events into a {@link Machine} from Groovy.
 *
 * @author Paul J. Lucas
 */
public final class CG_Event {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Determines whether this event is equal to another.
     *
     * @param o The other object.
     * @return Returns {@code true} only if the other object is also a
     * {@code CG_Event} and the two events' machines and names are equal.
     */
    public boolean equals( Object o ) {
        if ( !(o instanceof CG_Event) )
            return false;
        final CG_Event e = (CG_Event)o;
        return m_cgMachine.equals( e.m_cgMachine ) && m_name.equals( e.m_name );
    }

    /**
     * Broadcasts this event to its owning machine.
     *
     * @see #broadcast(Map)
     */
    public void broadcast() {
        broadcast( null );
    }

    /**
     * Broadcasts this event to its owning machine.
     *
     * @param parameters The parameters to broadcast with the event or
     * {@code null} if none.
     * @see #broadcast()
     */
    public void broadcast( Map<String,Object> parameters ) {
        final Event chsmEvent = m_cgMachine.getEventFor( this );
        if ( chsmEvent == null )
            throw new IllegalStateException(
                "corresponding event not found for event \"" + m_name + '"'
            );
        final ParamBlockForGroovy pb =
            parameters != null || m_cgPrecondition != null ?
                new ParamBlockForGroovy( chsmEvent, parameters ) : null;
        chsmEvent.broadcast( pb );
    }

    /**
     * Gets this event's name.
     *
     * @return Returns said name.
     */
    public String getName() {
        return m_name;
    }

    /**
     * Gets a hashcode value for this {@code CG_Event}.
     *
     * @return Returns said hashcode.
     */
    public int hashCode() {
        return m_cgMachine.hashCode() ^ m_name.hashCode();
    }

    /**
     * Converts this {@code CG_Event} to a {@link String} representation.
     *
     * @return Returns said {@link String}.
     */
    public String toString() {
        return "event " + m_name;
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Constructs a {@code CG_Event}.
     *
     * @param name The name of the event.
     * @param precondition The precondition of this event or {@code null} if
     * none.
     * @param baseEvent The base event of this event or {@code null} if none.
     * @param machine The {@link CG_Machine} to which this event belongs.
     */
    CG_Event( String name, Closure precondition, CG_Event baseEvent,
              CG_Machine machine ) {
        checkClosure( precondition );
        m_cgBaseEvent = baseEvent;
        m_cgMachine = machine;
        m_cgPrecondition = precondition;
        m_name = name;
    }

    /**
     * Adds a transition that this event transitions on.
     *
     * @param t The {@link CG_Transition} to add.
     */
    void addTransition( CG_Transition t ) {
        m_cgTransitions.add( t );
    }

    /**
     * Converts this {@code CG_Event} to an {@link Event}.
     *
     * @return Returns a new {@link Event} corresponding to this
     * {@code CG_Event}.
     */
    Event toCHSM() {
        final int[] tids = new int[ m_cgTransitions.size() ];
        int i = 0;
        for ( CG_Transition t : m_cgTransitions )
            tids[ i++ ] = t.getID();
        final Event baseEvent = m_cgMachine.getEventFor( m_cgBaseEvent );
        return new EventForGroovy(
            m_cgMachine.getMachine(), tids, m_name, baseEvent
        );
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * An {@code EventForGroovy} is-an {@link Event} that overloads the
     * <code>[]</code> operator in Groovy to access parameter values.
     */
    private static final class EventForGroovy extends Event {

        /**
         * Constructs an {@code EventForGroovy}.
         *
         * @param m The {@link Machine} to belong to.
         * @param transitions The indicies of the transitions this event
         * transitions on.
         * @param name The name of this event.
         * @param baseEvent The base event of this event or {@code null} if
         * none.
         */
        EventForGroovy( Machine m, int[] transitions, String name,
                        Event baseEvent ) {
            super( m, transitions, name, baseEvent );
        }

        /**
         * This overloads the <code>[]</code> operator in Groovy to get the
         * value of the named parameter.
         *
         * @param paramName The name of the parameter whose value to get.
         * @return Returns the value of the parameter having the given name or
         * {@code null} if none.
         */
        Object getAt( String paramName ) {
            final ParamBlockForGroovy pb = (ParamBlockForGroovy)paramBlock_;
            return pb != null ? pb.getParameters().get( paramName ) : null;
        }
    }

    /**
     * A {@code ParamBlockForGroovy} is-an {@link Event.ParamBlock} that adds
     * event parameters and a Groovy closure as a precondition.
     */
    private final class ParamBlockForGroovy extends Event.ParamBlock {

        ////////// package ////////////////////////////////////////////////////

        /**
         * Constructs a {@code ParamBlockForGroovy}.
         *
         * @param event The {@link Event} this param block is for.
         * @param parameters The parameters or {@code null} if none.
         */
        ParamBlockForGroovy( Event event, Map<String,Object> parameters ) {
            super( event );
            m_cgParameters = parameters;
        }

        /**
         * Gets the parameters, if any.
         *
         * @return Returns said parameters or {@code null} if none.
         */
        Map<String,Object> getParameters() {
            return m_cgParameters;
        }

        ////////// protected //////////////////////////////////////////////////

        /**
         * Calls a Groovy closure as the precondition for an event being
         * broadcasted.  The closure must return a boolean result.
         *
         * @return Returns {@code true} only if the closure returns
         * {@code true}.
         */
        protected boolean precondition() {
            return precondition( CG_Event.this );
        }

        ////////// private ////////////////////////////////////////////////////

        /**
         * Calls a Groovy {@link Closure} as the precondition for an event
         * being broadcasted.  The closure must return a boolean result.
         * <p>
         * All preconditions for all base events are evaluated in
         * base-to-derived order and evaluation stops when the first returns
         * {@code false}.
         *
         * @param cgEvent The {@link CG_Event} to evalulate the preconditon of.
         * @return Returns {@code true} only if all base events' (if any)
         * preconditions and this event's precondition return {@code true}.
         */
        private boolean precondition( CG_Event cgEvent ) {
            final CG_Event cgBaseEvent = cgEvent.m_cgBaseEvent;
            if ( cgBaseEvent != null && !precondition( cgBaseEvent ) )
                return false;
            final Closure cgPrecondition = cgEvent.m_cgPrecondition;
            return  cgPrecondition == null ||
                    (Boolean)cgPrecondition.call( m_cgParameters );
        }

        /**
         * The parameters or {@code null} if none.
         */
        private final Map<String,Object> m_cgParameters;
    }

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
                    "event precondition closures must have 1 parameter: " +
                    "params"
                );
        }
    }

    /**
     * The base event of this event or {@code null} if none.
     */
    private final CG_Event m_cgBaseEvent;

    /**
     * The {@link CG_Machine} to which this event belongs.
     */
    private final CG_Machine m_cgMachine;

    /**
     * The precondition of this event or {@code null} if none.
     */
    private final Closure m_cgPrecondition;

    /**
     * The collection of transitions that this event transitions on.
     */
    private final List<CG_Transition> m_cgTransitions =
        new LinkedList<CG_Transition>();

    /**
     * The name of this event.
     */
    private final String m_name;
}
/* vim:set et sw=4 ts=4: */
