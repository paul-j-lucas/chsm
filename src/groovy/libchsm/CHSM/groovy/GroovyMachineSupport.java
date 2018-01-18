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

import java.util.Map;

import CHSM.*;
import groovy.lang.GroovyObjectSupport;

/**
 * {@code GroovyMachineSupport} is a useful base class for Groovy classes
 * wishing to be {@link GroovyMachine}s.
 * <p>
 * {@code GroovyMachineSupport} need only be used when additional data or
 * methods need to be added to a {@link Machine}.  For example, in Groovy:
 *  <pre>
 *  class TimeMachine extends GroovyMachineSupport {
 *
 *      public TimeMachine() {
 *          setMachine( buildMachine() );
 *      }
 *
 *      public void dematerialize() {
 *          // ...
 *      }
 *
 *      public void materialize() {
 *          // ...
 *      }
 *
 *      private Date visitDate;
 *
 *      private CG_Machine buildMachine() {
 *          return new CHSM_Builder().chsm() {
 *              // ... machine description ...
 *          };
 *      }
 *  }</pre>
 * Within <i>machine description</i>, all data members
 * (such as {@code visitDate}) and methods (such as {@code dematerialize} and
 * {@code materialize}) are accessible.
 *
 * @author Paul J. Lucas
 */
public abstract class GroovyMachineSupport extends GroovyObjectSupport
    implements GroovyMachine {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * {@inheritDoc}
     */
    public boolean active() {
        return m_cgMachine.active();
    }

    /**
     * {@inheritDoc}
     */
    public boolean broadcast( String eventName,
                              Map<String,Object> parameters ) {
        return m_cgMachine.broadcast( eventName, parameters );
    }

    /**
     * {@inheritDoc}
     */
    public int debug( int state ) {
        return m_cgMachine.debug( state );
    }

    /**
     * {@inheritDoc}
     */
    public void dumpState() {
        m_cgMachine.dumpState();
    }

    /**
     * {@inheritDoc}
     */
    public boolean enter() {
        return m_cgMachine.enter();
    }

    /**
     * {@inheritDoc}
     */
    public boolean exit() {
        return m_cgMachine.exit();
    }

    /**
     * {@inheritDoc}
     */
    public CG_Event getEvent( String name ) {
        return m_cgMachine.getEvent( name );
    }

    /**
     * Gets the {@link CG_Machine} in use.
     *
     * @return Returns said {@link CG_Machine}.
     * @see #setMachine(CG_Machine)
     */
    public CG_Machine getMachine() {
        return m_cgMachine;
    }

    /**
     * Calls {@link CG_Machine#getProperty(String)} for the {@link CG_Machine}
     * in use.
     *
     * @param property The property to get.
     * @return If the {@link CG_Machine} in use has a top-level
     * {@link CG_State} having a name equal to the given property, returns
     * said {@link CG_State}; otherwise returns whatever the {@code super}
     * method returns.
     * @see #setMachine(CG_Machine)
     * @see CG_Machine#getProperty(String)
     * @see CG_Machine#getState(String)
     */
    public Object getProperty( String property ) {
        if ( m_cgMachine != null ) {
            final CG_State cgState = m_cgMachine.getState( property );
            if ( cgState != null )
                return cgState;
        }
        return super.getProperty( property );
    }

    /**
     * Calls {@link CG_Machine#invokeMethod(String,Object)} for the
     * {@link CG_Machine} in use.
     *
     * @param name The method's name.
     * @param args The method's arguments.
     * @return If the {@link CG_Machine} in use has a {@link CG_Event} having
     * the given name, returns {@code null}; otherwise returns whatever the
     * {@code super} method returns.
     * @see #getEvent(String)
     * @see #setMachine(CG_Machine)
     * @see CG_Machine#invokeMethod(String,Object)
     */
    public Object invokeMethod( String name, Object args ) {
        if ( m_cgMachine != null ) {
            final CG_Event cgEvent = getEvent( name );
            if ( cgEvent != null )
                return m_cgMachine.invokeMethod( name, args );
        }
        return super.invokeMethod( name, args );
    }

    /**
     * Sets the {@link CG_Machine} to forward method calls to.
     *
     * @param cgMachine The {@link CG_Machine} to forward method calls to.
     * @see #getMachine()
     */
    public void setMachine( CG_Machine cgMachine ) {
        m_cgMachine = cgMachine;
    }

    /**
     * Calls {@link CG_Machine#toString()} for the {@link CG_Machine} in use.
     *
     * @return Returns what {@link CG_Machine#toString()} returns.
     */
    public String toString() {
        return m_cgMachine.toString();
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Constructs a {@code GroovyMachineSupport}.
     */
    protected GroovyMachineSupport() {
        // do nothing
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * The {@link CG_Machine} to forward method calls to.
     */
    private CG_Machine m_cgMachine;
}
/* vim:set et sw=4 ts=4: */
