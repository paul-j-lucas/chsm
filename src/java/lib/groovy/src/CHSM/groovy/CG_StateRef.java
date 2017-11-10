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

import CHSM.State;

/**
 * A {@code CG_StateRef} is a {@link CG_State} that refers to another state.
 * This acts as a place-holder for states that are forward-referenced.
 *
 * @author Paul J. Lucas
 */
final class CG_StateRef extends CG_State {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Gets the parent state of the referent state (if any) or of this state
     * (if none).
     *
     * @return Returns said state or {@code null} if none.
     */
    public CG_Parent getParent() {
        return m_referent != null ? m_referent.getParent() : super.getParent();
    }

    /**
     * Gets the property having the given name for the referent state (if any).
     *
     * @param property The name of the property.
     * @return Returns the property having the given name or {@code null} if no
     * such property exists.
     * @see CG_Parent#getProperty(String)
     */
    public Object getProperty( String property ) {
        if ( m_referent != null )
            return m_referent.getProperty( property );
        return super.getProperty( property );
    }

    /**
     * Gets the type name of the referent state (if any) or of this state (if
     * none).
     *
     * @return Returns said type name.
     */
    public String getTypeName() {
        return m_referent != null ? m_referent.getTypeName() : "ref";
    }

    /**
     * Converts the referent state (if any) or this state (if none) to a string
     * representation (mostly for debugging).
     *
     * @param stops The number of stops to indent.
     * @return Returns said string.
     */
    protected String toString( int stops ) {
        return m_referent != null ?
                m_referent.toString( stops ) : super.toString( stops );
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Constructs a {@code CG_StateRef}.
     *
     * @param fqName The fully qualified name of the state.
     */
    CG_StateRef( String fqName ) {
        super( null, fqName, null, null );
    }

    /**
     * Gets the ID of the referent state (if any) or of this state (if none).
     *
     * @return Returns said ID.
     */
    int getID() {
        return m_referent != null ? m_referent.getID() : super.getID();
    }

    /**
     * Gets the state to which this {@code CG_StateRef} refers.
     *
     * @return Returns said state or {@code null} if none.
     * @see #setReferent(CG_State)
     */
    CG_State getReferent() {
        return m_referent;
    }

    /**
     * Sets the &quot;enter event&quot; of the referent state (if any) or of
     * this state temporarily (if none).
     *
     * @param enterEvent The enter event.
     */
    void setEnterEvent( CG_Event enterEvent ) {
        if ( m_referent != null )
            m_referent.setEnterEvent( enterEvent );
        else
            super.setEnterEvent( enterEvent );
    }

    /**
     * Sets the &quot;exit event&quot; of the referent state (if any) or of
     * this state temporarily (if none).
     *
     * @param exitEvent The exit event.
     */
    void setExitEvent( CG_Event exitEvent ) {
        if ( m_referent != null )
            m_referent.setExitEvent( exitEvent );
        else
            super.setExitEvent( exitEvent );
    }

    /**
     * Sets the ID of the referent state (if any) or of this state temporarily
     * (if none).
     *
     * @param id The ID.
     * @see #setReferent(CG_State)
     */
    void setID( int id ) {
        if ( m_referent != null )
            m_referent.setID( id );
        else
            super.setID( id );
    }

    /**
     * Sets the parent state of the referent state (if any) or of this state
     * temporarily (if none).
     *
     * @param cgParent The parent state.
     * @see #setReferent(CG_State)
     */
    void setParent( CG_Parent cgParent ) {
        if ( m_referent != null )
            m_referent.setParent( cgParent );
        super.setParent( cgParent );
    }

    /**
     * Sets the referent state.  That state's ID, parent, and machine are then
     * set to be those of this state.
     *
     * @param referent The referent state.
     */
    void setReferent( CG_State referent ) {
        assert m_referent == null;
        if ( m_cgEnterEvent != null )
            referent.setEnterEvent( m_cgEnterEvent );
        if ( m_cgExitEvent != null )
            referent.setExitEvent( m_cgExitEvent );
        referent.setID( getID() );
        referent.setMachine( m_cgMachine );
        referent.setParent( getParent() );
        m_referent = referent;
    }

    /**
     * {@inheritDoc}
     */
    State toCHSM() {
        if ( m_referent == null )
            throw new IllegalStateException(
                "reference to state \"" + getName() + "\" unresolved"
            );
        return m_referent.toCHSM();
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * The {@link CG_State} to which this {@code CG_StateRef} refers.
     */
    private CG_State m_referent;
}
/* vim:set et sw=4 ts=4: */
