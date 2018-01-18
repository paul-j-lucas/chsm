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

import CHSM.Machine;
import groovy.lang.GroovyObject;

/**
 * A {@code GroovyMachine} wraps a {@link Machine} for Groovy.
 *
 * @author Paul J. Lucas
 */
public interface GroovyMachine extends GroovyObject {

    /**
     * Gets whether this machine is active.  A machine is not active initially
     * upon construction.  A machne is active only after {@link #enter()} has
     * been called and before {@link #exit()} has been called.
     *
     * @return Returns {@code true} only if this machine is active.
     */
    boolean active();

    /**
     * Broadcasts the event having the given name.  If no such event exists,
     * nothing is done.
     *
     * @param eventName The name of the event.
     * @param parameters The event's parameters or {@code null} if none.
     * @return Returns {@code true} only if an event having the given name
     * exists and was therefore broadcasted.
     */
    boolean broadcast( String eventName, Map<String,Object> parameters );

    /**
     * Sets the debugging state.  Debugging information is printed to system
     * error.
     *
     * @param state The state to set that is the bitwise &quot;or&quot; of the
     * debugging states {@link Machine#DEBUG_ENTER_EXIT},
     * {@link Machine#DEBUG_EVENTS}, {@link Machine#DEBUG_ALGORITHM}, or
     * {@link Machine#DEBUG_ALL}.
     *
     * @return Returns the previous debugging state.
     */
    int debug( int state );

    /**
     * Dumps a printout of the current state to standard error.  The dump
     * consists of each state's name, one per line, preceded by an asterisk
     * only if it is active; a space otherwise.
     */
    void dumpState();

    /**
     * Enters a machine by entering its root cluster.
     *
     * @return Returns {@code true} only if the machine was actually entered,
     * i.e., it wasn't already active.
     */
    boolean enter();

    /**
     * Exits a machine by exiting its root cluster.
     *
     * @return Returns {@code true} only if the machine was actually exited,
     * i.e., it wasn't already inactive.
     */
    boolean exit();

    /**
     * Gets the {@link CG_Event} having the given name.
     *
     * @param name The name of the event.
     * @return Returns said {@link CG_Event} or {@code null} if no event having
     * the given name exists.
     */
    CG_Event getEvent( String name );
}
/* vim:set et sw=4 ts=4: */
