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
import groovy.lang.GroovyObjectSupport;

/**
 * A {@code CG_State} is used during CHSM construction by a
 * {@link CHSM_Builder} to build a {@link State}.
 *
 * @author Paul J. Lucas
 */
public class CG_State extends GroovyObjectSupport {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Gets whether this state is active.
     *
     * @return Returns {@code true} only if the state is active.
     */
    public boolean active() {
        return m_cgMachine.getStateFor( this ).active();
    }

    /**
     * Determines whether this state is equal to another.
     *
     * @param o The other object.
     * @return Returns {@code true} only if the other object is also a
     * {@code CG_State} and the two states' machines and names are equal.
     */
    public boolean equals( Object o ) {
        if ( !(o instanceof CG_State) )
            return false;
        final CG_State s = (CG_State)o;
        return  m_cgMachine.equals( s.m_cgMachine ) &&
                m_fqName.equals( s.m_fqName );
    }

    /**
     * Gets the {@link CG_Machine} that this state belongs to.
     *
     * @return Returns said {@link CG_Machine}.
     */
    public CG_Machine getMachine() {
        return m_cgMachine;
    }

    /**
     * Gets the fully qualified name of this state.
     *
     * @return Returns said name.
     * @see #getShortName()
     */
    public String getName() {
        return m_fqName;
    }

    /**
     * Gets the parent state of this state, if any.
     *
     * @return Returns said state or {@code null} if none.
     */
    public CG_Parent getParent() {
        return m_cgParent;
    }

    /**
     * Gets the unqualified (short) name of this state.
     *
     * @return Returns said name.
     * @see #getName()
     */
    public String getShortName() {
        final int dot = m_fqName.lastIndexOf( '.' );
        return dot > -1 ? m_fqName.substring( dot + 1 ) : m_fqName;
    }

    /**
     * Gets the type name of this state.
     *
     * @return Returns said type name.
     */
    public String getTypeName() {
        return "state";
    }

    /**
     * Gets a hashcode value for this {@code CG_State}.
     *
     * @return Returns said hashcode.
     */
    public int hashCode() {
        return m_cgMachine.hashCode() ^ m_fqName.hashCode();
    }

    /**
     * Converts this state to a string representation (mostly for debugging).
     *
     * @return Returns said string.
     */
    public final String toString() {
        return toString( 0 );
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Constructs a {@code CG_State}.
     *
     * @param stateClass The {@link Class} of the state to create or
     * {@code null} for the default.
     * @param fqName The fully qualified name of the state.
     * @param enterAction The &quot;enter&quot; action or {@code null} if none.
     * @param exitAction The &quot;exit&quot; action or {@code null} if none.
     */
    CG_State( Class<? extends State> stateClass, String fqName,
              Closure enterAction, Closure exitAction ) {
        checkClosure( enterAction );
        checkClosure( exitAction );
        m_stateClass = checkClass( stateClass, State.class );
        m_fqName = fqName;
        m_cgEnterAction = enterAction;
        m_cgExitAction = exitAction;
    }

    /**
     * Gets the ID of this state.
     *
     * @return Returns said ID.
     */
    int getID() {
        return m_id;
    }

    /**
     * Sets the &quot;enter event&quot; of this state, i.e., the event that is
     * to be broadcasted whenever this state is entered.
     *
     * @param enterEvent The enter event.
     */
    void setEnterEvent( CG_Event enterEvent ) {
        assert m_cgEnterEvent == null || enterEvent == m_cgEnterEvent;
        m_cgEnterEvent = enterEvent;
    }

    /**
     * Sets the &quot;exit event&quot; of this state, i.e., the event that is
     * to be broadcasted whenever this state is entered.
     *
     * @param exitEvent The exit event.
     */
    void setExitEvent( CG_Event exitEvent ) {
        assert m_cgExitEvent == null || exitEvent == m_cgExitEvent;
        m_cgExitEvent = exitEvent;
    }

    /**
     * Sets the ID of this state.
     *
     * @param id The ID.
     */
    void setID( int id ) {
        assert id > -1;
        m_id = id;
    }

    /**
     * Sets the {@link CG_Machine} to which this state belongs.
     *
     * @param cgMachine The owning {@link CG_Machine}.
     */
    void setMachine( CG_Machine cgMachine ) {
        assert m_cgMachine == null || cgMachine == m_cgMachine;
        m_cgMachine = cgMachine;
    }

    /**
     * Sets the parent state of this state.
     *
     * @param cgParent The parent state.
     */
    void setParent( CG_Parent cgParent ) {
        m_cgParent = cgParent;
    }

    /**
     * Converts this {@code CG_State} to its corresponding {@link State}.
     *
     * @return Returns said {@link State}.
     */
    State toCHSM() {
        try {
            final Constructor<? extends State> c = m_stateClass.getConstructor(
                Machine.class, String.class, Parent.class,
                State.Action.class, State.Action.class,
                Event.class, Event.class
            );
            return c.newInstance(
                m_cgMachine.getMachine(), m_fqName,
                m_cgMachine.getStateFor( m_cgParent ),
                convertAction( m_cgEnterAction ),
                convertAction( m_cgExitAction ),
                m_cgMachine.getEventFor( m_cgEnterEvent ),
                m_cgMachine.getEventFor( m_cgExitEvent )
            );
        }
        catch ( Exception e ) {
            throw new IllegalStateException( e );
        }
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Checks that the given class is derived from a required class.
     *
     * @param c The {@link Class} to check; may be {@code null}.
     * @param requiredClass The {@link Class} that the given class is required
     * to be derived from.
     * @return If the given class is {@code null}, returns the required class;
     * otherwise returns the given class.
     * @throws IllegalArgumentException if the given class is not {@code null}
     * and not derived from the required class.
     */
    protected static Class<? extends State> checkClass(
        Class<? extends State> c, Class<? extends State> requiredClass
    ) {
        if ( c == null )
            return requiredClass;
        if ( !requiredClass.isAssignableFrom( c ) )
            throw new IllegalArgumentException(
                "\"" + c.getSimpleName() + "\" must be derived from " +
                requiredClass.getSimpleName()
            );
        return c;
    }

    /**
     * Converts a {@link Closure} to a {@link State.Action}.
     *
     * @param cgAction The {@link Closure} to convert.
     * @return Returns said {@link State.Action}.
     */
    protected static State.Action convertAction( final Closure cgAction ) {
        if ( cgAction == null )
            return null;
        return new State.Action() {
            public void exec( State state, Event event ) {
                final int numParams = cgAction.getMaximumNumberOfParameters();
                final Object[] params = new Object[ numParams ];
                params[0] = event;
                if ( numParams > 1 )
                    params[1] = state;
                cgAction.call( params );
            }
        };
    }

    /**
     * Creates a {@link String} of spaces that is <code>stops * </code>
     * {@link #INDENT_WIDTH} in length.  This is used by {@link #toString(int)}.
     *
     * @param stops The number of stops to indent.
     * @return Returns said {@link String}.
     */
    protected static String indent( int stops ) {
        int width = stops * INDENT_WIDTH;
        final StringBuilder sb = new StringBuilder( width );
        while ( width-- > 0 )
            sb.append( ' ' );
        return sb.toString();
    }

    /**
     * Converts this state to a string representation (mostly for debugging).
     *
     * @param stops The number of stops to indent.
     * @return Returns said string.
     */
    protected String toString( int stops ) {
        return  indent( stops ) + getTypeName() + ' ' + m_fqName
                + " (" + m_id + ')';
    }

    /**
     * The enter action, if any.
     */
    protected final Closure m_cgEnterAction;

    /**
     * The exit action, if any.
     */
    protected final Closure m_cgExitAction;

    /**
     * The {@link CG_Event}, if any, that is to be broadcasted whenever this
     * state is entered.
     */
    protected CG_Event m_cgEnterEvent;

    /**
     * The {@link CG_Event}, if any, that is to be broadcasted whenever this
     * state is exited.
     */
    protected CG_Event m_cgExitEvent;

    /**
     * The {@link CG_Machine} to which this state belongs.
     */
    protected CG_Machine m_cgMachine;

    /**
     * The {@link Class} of the state to construct.
     * @see #toCHSM()
     */
    protected Class<? extends State> m_stateClass;

    ////////// private ////////////////////////////////////////////////////////

    /**
     * Checks that the given {@link Closure} has the right number of parameters.
     *
     * @param cgAction The {@link Closure} to check.
     */
    private static void checkClosure( Closure cgAction ) {
        if ( cgAction != null ) {
            final int numParams = cgAction.getMaximumNumberOfParameters();
            if ( numParams < 1 || numParams > 2 )
                throw new IllegalArgumentException(
                    "state action closures must have 1 or 2 parameters: " +
                    "event [, state]"
                );
        }
    }

    /**
     * The number of spaces per indent unit.
     * This is used by {@link #toString(int)}.
     */
    private static final int INDENT_WIDTH = 2;

    /**
     * The {@link CG_Parent} state of this state, if any.
     */
    private CG_Parent m_cgParent;

    /**
     * The fully qualified name of this state.
     */
    private final String m_fqName;

    /**
     * The unique ID of this state.
     */
    private int m_id = -1;
}
/* vim:set et sw=4 ts=4: */
