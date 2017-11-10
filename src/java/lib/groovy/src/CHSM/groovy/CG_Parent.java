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

import CHSM.State;
import groovy.lang.Closure;

/**
 * A {@code CG_Parent} is a {@link CG_State} that is the base class for
 * {@link CG_Cluster} and {@link CG_Set}.  It is used both during CHSM
 * construction by a {@link CHSM_Builder} and during run-time to get access to
 * child states by using Groovy property notation.
 *
 * @author Paul J. Lucas
 */
public abstract class CG_Parent extends CG_State implements Iterable<CG_State> {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Gets the child state having the given name.
     *
     * @param uqChildName The unqualified name of the child state.
     * @return Returns the child {@link CG_State} having the given name or
     * {@code null} if no such child exists.
     */
    public CG_State getChild( String uqChildName ) {
        final String fqChildName = qualifyChildName( uqChildName );
        return m_fqChildNameMap.get( fqChildName );
    }

    /**
     * Gets the property having the given name.  For a {@code CG_Parent}, the
     * names of child states are properties.  This allows the states to be
     * referred to, e.g.:
     *  <pre>
     *  CG_Machine chsm = new CHSM_Builder().chsm() {
     *      cluster( name:"c", children:["s"] ) {
     *          state("s");
     *      }
     *  }
     *  println chsm.c.s.toString();</pre>
     *
     * @param property The name of the property (state).
     * @return Returns the {@link CG_State} having the given name or
     * whatever the {@code super} method returns if none.
     * @see #getChild(String)
     * @see CG_Machine#getProperty(String)
     */
    public Object getProperty( String property ) {
        final CG_State cgChild = getChild( property );
        return cgChild != null ? cgChild : super.getProperty( property );
    }

    /**
     * Creates a new {@link Iterator} over this parent's child states.  The
     * child states are iterated over in the same order as they were declared.
     *
     * @return Returns said {@link Iterator}.
     */
    public Iterator<CG_State> iterator() {
        return m_fqChildNameMap.values().iterator();
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Adds a child state to this parent's list of children.
     *
     * @param cgChild The {@link CG_State} to add.  If this state is already a
     * child state of this parent, it is not added again.
     * @return Returns {@code true} only if the child was added.
     */
    boolean addChild( CG_State cgChild ) {
        final String fqChildName = cgChild.getName();
        if ( m_fqChildNameMap.containsKey( fqChildName ) )
            return false;
        m_fqChildNameMap.put( fqChildName, cgChild );
        return true;
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Constructs a {@code CG_Parent}.
     *
     * @param stateClass The {@link Class} of the set to create or {@code null}
     * for the default.
     * @param fqName The fully qualified name of the state.
     * @param enterAction The &quot;enter&quot; action or {@code null} if none.
     * @param exitAction The &quot;exit&quot; action or {@code null} if none.
     */
    protected CG_Parent( Class<? extends State> stateClass, String fqName,
                         Closure enterAction, Closure exitAction ) {
        super( stateClass, fqName, enterAction, exitAction );
    }

    /**
     * Converts the set of child states to an array of their IDs.
     *
     * @return Returns said IDs.
     */
    protected final int[] convertChildren() {
        int i = 0;
        final int[] sid = new int[ m_fqChildNameMap.size() ];
        for ( CG_State cgChild : this )
            sid[ i++ ] = cgChild.getID();
        return sid;
    }

    /**
     * Qualifies an unqualified child state name.
     *
     * @param uqName The unqualified child state name.
     * @return Returns the fully qualified child state name.
     */
    protected String qualifyChildName( String uqName ) {
        return CHSM_Builder.qualifyStateName( uqName, this );
    }

    /**
     * {@inheritDoc}
     */
    protected String toString( int stops ) {
        if ( m_fqChildNameMap.isEmpty() )
            return super.toString( stops );
        final StringBuilder sb = new StringBuilder();
        sb.append( super.toString( stops ) );
        sb.append( " {\n" );
        ++stops;
        for ( CG_State cgChild : this ) {
            sb.append( cgChild.toString( stops ) );
            sb.append( '\n' );
        }
        --stops;
        sb.append( indent( stops ) );
        sb.append( '}' );
        return sb.toString();
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * A map of the fully qualified names of child states to the
     * {@link CG_State} objects themselves.  A {@link LinkedHashSet} is used in
     * order to get both uniqueness and order preservation.
     */
    private final Map<String,CG_State> m_fqChildNameMap =
        new LinkedHashMap<String,CG_State>();
}
/* vim:set et sw=4 ts=4: */
