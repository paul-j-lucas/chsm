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

import java.util.*;
import java.util.Set;

import CHSM.*;
import groovy.lang.GroovyObjectSupport;

/**
 * A {@code CG_Machine} wraps a {@link Machine} for Groovy.
 * A new instance is created by {@link CHSM_Builder}.
 *
 * @author Paul J. Lucas
 */
public final class CG_Machine extends GroovyObjectSupport
    implements GroovyMachine {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * {@inheritDoc}
     */
    public boolean active() {
        return m_machine.active();
    }

    /**
     * {@inheritDoc}
     */
    public boolean broadcast( String eventName,
                              Map<String,Object> parameters ) {
        final CG_Event cgEvent = getEvent( eventName );
        if ( cgEvent != null ) {
            cgEvent.broadcast( parameters );
            return true;
        }
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public int debug( int state ) {
        return m_machine.debug( state );
    }

    /**
     * {@inheritDoc}
     */
    public void dumpState() {
        m_machine.dumpState();
    }

    /**
     * {@inheritDoc}
     */
    public boolean enter() {
        return m_machine.enter();
    }

    /**
     * {@inheritDoc}
     */
    public boolean exit() {
        return m_machine.exit();
    }

    /**
     * {@inheritDoc}
     */
    public CG_Event getEvent( String name ) {
        return m_eventNameMap.get( name );
    }

    /**
     * Gets the {@link Machine} corresponding to this {@code CG_Machine}.
     *
     * @return Returns said {@link Machine}.
     */
    public Machine getMachine() {
        return m_machine;
    }

    /**
     * Gets the property having the given name.  For a {@code CG_Machine}, the
     * names of top-level states are properties.  This allows the states to be
     * referred to, e.g.:
     *  <pre>
     *  CG_Machine chsm = new CHSM_Builder().chsm() {
     *      state("s");
     *  }
     *  println chsm.s.toString();</pre>
     *
     * @param property The name of the property (state).
     * @return Returns the {@link CG_State} having the given name or
     * whatever the {@code super} method returns if none.
     * @see #getState(String)
     * @see CG_Parent#getProperty(String)
     */
    public Object getProperty( String property ) {
        final CG_State cgState = getState( property );
        return cgState != null ? cgState : super.getProperty( property );
    }

    /**
     * Gets the top-level {@link CG_State} having the given name.
     *
     * @param name The name of the state.
     * @return Returns the {@link CG_State} having the given name or
     * {@code null} if no such state exists.
     */
    public CG_State getState( String name ) {
        return m_cgRoot != null ? m_cgRoot.getChild( name ) : null;
    }

    /**
     * Invokes the method having the given name.  For a {@code CG_Machine}, the
     * names of events are methods.  This allows events to be broadcasted,
     * e.g.:
     *  <pre>
     *  CG_Machine chsm = new CHSM_Builder().chsm() {
     *      event("alpha");
     *      // ...
     *  }
     *  chsm.alpha();   // broadcasts alpha</pre>
     *
     * @param name The method's name.
     * @param args The method's arguments.
     * @return Returns {@code null} if the given name refers to an event or, if
     * not, whatever the {@code super} method returns.
     * @see #getEvent(String)
     */
    public Object invokeMethod( String name, Object args ) {
        final CG_Event cgEvent = getEvent( name );
        if ( cgEvent != null ) {
            Map parameters = null;
            if ( args != null ) {
                Object arg = null;
                if ( args.getClass().isArray() ) {
                    final Object[] argArray = (Object[])args;
                    if ( argArray.length > 0 )
                        arg = argArray[0];
                } else
                    arg = args;
                if ( arg instanceof Map )
                    parameters = (Map)arg;
            }
            cgEvent.broadcast( parameters );
            return null;
        }
        return super.invokeMethod( name, args );
    }

    /**
     * Converts this {@code CG_Machine} to a {@link String} representation
     * (mostly for debugging).
     *
     * @return Returns said {@link String} or {@code null} if none.
     */
    public String toString() {
        return m_cgRoot != null ? m_cgRoot.toString() : null;
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * {@code MachineForGroovy} exists to have a non-abstract {@link Machine}
     * class and also to be able to call the machine's {@code init()} method.
     */
    static final class MachineForGroovy extends Machine {
        void init2( Cluster root, State[] states, Transition[] transitions ) {
            init( root, states, transitions );
        }
    }

    /**
     * Constructs a {@code CG_Machine}.
     */
    CG_Machine() {
        // do nothing
    }

    /**
     * Adds a {@link CG_Event} to the machine.
     *
     * @param e The {@link CG_Event} to add.
     */
    void addEvent( CG_Event e ) {
        final boolean added = m_eventNameMap.put( e.getName(), e ) == null;
        assert added;
    }

    /**
     * Adds a {@link CG_State} to the machine.
     *
     * @param s The {@link CG_State} to add.
     * @return Returns {@code true} only if the state actually was added.
     */
    boolean addState( CG_State s ) {
        s.setMachine( this );
        if ( m_cgStates.add( s ) ) {
            s.setID( m_nextStateID++ );
            return true;
        }
        return false;
    }

    /**
     * Adds a {@link CG_Transition} to the machine.
     *
     * @param t The {@link CG_Transition} to add.
     */
    void addTransition( CG_Transition t ) {
        m_cgTransitions.add( t );
        t.setID( m_nextTransitionID++ );
    }

    /**
     * Gets the {@link Event} corresponding to the given {@link CG_Event}
     * creating it if necessary.
     *
     * @param cgEvent The {@link CG_Event} to get the {@link Event} for.
     * @return Returns said {@link Event}.
     */
    Event getEventFor( CG_Event cgEvent ) {
        if ( cgEvent == null )
            return null;
        synchronized ( m_cgEventMap ) {
            Event chsmEvent = m_cgEventMap.get( cgEvent );
            if ( chsmEvent == null ) {
                chsmEvent = cgEvent.toCHSM();
                m_cgEventMap.put( cgEvent, chsmEvent );
            }
            return chsmEvent;
        }
    }

    /**
     * Gets the root {@link CG_Cluster} of this machine.
     *
     * @return Returns said {@link CG_Cluster}.
     */
    CG_Cluster getRoot() {
        return m_cgRoot;
    }

    /**
     * Gets the {@link State} corresponding to the given {@link CG_State}.
     *
     * @param cgState The {@link CG_State} to get the {@link State} for.
     * @return Returns said {@link State}.
     */
    State getStateFor( CG_State cgState ) {
        if ( cgState == null )
            return null;
        synchronized ( m_cgStateMap ) {
            State chsmState = m_cgStateMap.get( cgState );
            if ( chsmState == null ) {
                chsmState = cgState.toCHSM();
                m_cgStateMap.put( cgState, chsmState );
            }
            return chsmState;
        }
    }

    /**
     * Sets the root {@link CG_Cluster} of this machine.
     *
     * @param root The new root {@link CG_Cluster}.
     * @see #getRoot()
     */
    void setRoot( CG_Cluster root ) {
        assert m_cgRoot == null;
        root.setMachine( this );
        m_cgRoot = root;
    }

    /**
     * Converts this {@code CG_Machine} to its corresponding {@link Machine}.
     *
     * @return Returns said {@link Machine}.
     */
    Machine toCHSM() {
        for ( CG_Event e : m_cgEventMap.keySet() )
            getEventFor( e );           // no need to store return value

        int i = 0;
        final State[] states = new State[ m_cgStates.size() ];
        for ( CG_State s : m_cgStates )
            states[ i++ ] = getStateFor( s );

        i = 0;
        final Transition[] transitions =
            new Transition[ m_cgTransitions.size() ];
        for ( CG_Transition t : m_cgTransitions )
            transitions[ i++ ] = t.toCHSM();

        final Cluster chsmRoot = (Cluster)getStateFor( m_cgRoot );
        m_machine.init2( chsmRoot, states, transitions );
        return m_machine;
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * A mapping between {@link CG_Event}s and their corresponding
     * {@link Event}s.
     *
     * @see #m_eventNameMap
     */
    private final Map<CG_Event,Event> m_cgEventMap =
        new HashMap<CG_Event,Event>();

    /**
     * The root {@link CG_Cluster} of the machine.
     */
    private CG_Cluster m_cgRoot;

    /**
     * A mapping between {@link CG_State}s and their corresponding
     * {@link State}s.
     */
    private final Map<CG_State,State> m_cgStateMap =
        new HashMap<CG_State,State>();

    /**
     * The list of all the states in the machine.  A {@link LinkedHashSet} is
     * used in order to get both uniqueness and order preservation.
     */
    private final Set<CG_State> m_cgStates = new LinkedHashSet<CG_State>();

    /**
     * A list of all transitions in the machine.
     */
    private final List<CG_Transition> m_cgTransitions =
        new LinkedList<CG_Transition>();

    /**
     * A mapping of event names to {@link CG_Event} objects.
     */
    private final Map<String,CG_Event> m_eventNameMap =
        new HashMap<String,CG_Event>();

    /**
     * The actual CHSM {@link Machine}.
     */
    private final MachineForGroovy m_machine = new MachineForGroovy();

    /**
     * The next ID to use for a {@link CG_State}.
     */
    private int m_nextStateID;

    /**
     * The next ID to use for a {@link CG_Transition}.
     */
    private int m_nextTransitionID;
}
/* vim:set et sw=4 ts=4: */
