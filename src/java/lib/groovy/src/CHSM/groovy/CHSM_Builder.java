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

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import CHSM.*;
import groovy.lang.Closure;
import groovy.util.BuilderSupport;

/**
 * A {@code CHSM_Builder} is used to build
 * <a href="http://chsm.sourceforge.net/">CHSM</a>s in Groovy.
 *
 * There are the following node types:
 * chsm, event, state, cluster, set, transition, end enter/exit event.
 *
 * <h4>CHSM Nodes</h4>
 * CHSM (&quot;<tt>chsm</tt>&quot;) nodes have the following attributes:
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>enter</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          Code executed upon entrance.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>exit</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          Code executed upon exit.
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 * Within the closure for either <tt>enter</tt> or <tt>exit</tt>,
 * there are two variables available:
 *  <ol>
 *    <li>The {@link Event} that triggered the transition.</li>
 *    <li>The {@link State} being either entered or exited.</li>
 *  </ol>
 * 
 * <h4>Event Nodes</h4>
 * Event (&quot;<tt>event</tt>&quot;) nodes have the following attributes
 * (bold are required):
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><b><tt>name</tt></b></td>
 *        <td>{@link String}</td>
 *        <td>
 *          The event's name.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>base</tt></td>
 *        <td>{@link String}</td>
 *        <td>
 *         The base event.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>precondition</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          A boolean function evaluated upon broadcast.
 *          If it returns {@code false}, the broadcast is cancelled.
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 * If only the <tt>name</tt> attribute is given, it can be abbreviated, e.g.:
 *  <blockquote>
 *    <tt>event("alpha")</tt>
 *  </blockquote>
 * However, there is no reason to do that since events are implicitly declared
 * upon first use.
 * <p>
 * Within the closure for <tt>precondition</tt>,
 * there is one variable available:
 *  <ol>
 *    <li>The map of event parameters.</li>
 *  </ol>
 *
 * <h4>State Nodes</h4>
 * State (&quot;<tt>state</tt>&quot;) nodes have the following attributes
 * (bold are required):
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><b><tt>name</tt></b></td>
 *        <td>{@link String}</td>
 *        <td>
 *          The state's name.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>class</tt></td>
 *        <td>{@link Class}</td>
 *        <td>
 *          The {@link Class}
 *          (derived directly or indirectly from {@link State}) of this state.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>enter</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          Code executed upon entrance.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>exit</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          Code executed upon exit.
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 * If only the <tt>name</tt> attribute is given, it can be abbreviated, e.g.:
 *  <pre>
 *    state("a")</pre>
 * Within the closure for either <tt>enter</tt> or <tt>exit</tt>,
 * there are two variables available:
 *  <ol>
 *    <li>The {@link Event} that triggered the transition.</li>
 *    <li>The {@link State} being either entered or exited.</li>
 *  </ol>
 *
 * <h4>Cluster and Set Nodes</h4>
 * Cluster (&quot;<tt>cluster</tt>&quot;) and set (&quot;<tt>set</tt>&quot;)
 * nodes have all the attributes that state nodes have,
 * but also have the following additional attributes:
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>children</tt></td>
 *        <td>{@link String}<tt>[]</tt></td>
 *        <td>
 *          The name(s) of the child state(s).
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 * The order in which child states are declared need not match the order in
 * which they are defined.
 *
 * <h4>Cluster Nodes</h4>
 * Cluster (&quot;<tt>cluster</tt>&quot;) nodes have all the attributes
 * that state and set nodes have, but also have the following additional
 * attributes:
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>history</tt></td>
 *        <td>boolean or {@link String}</td>
 *        <td>
 *          Whether the cluster has a history and what type.
 *          If the type is {@link String}, the legal values are:
 *          <tt>true</tt>, <tt>false</tt>,
 *          <tt>shallow</tt>, and <tt>deep</tt>.
 *          The value of <tt>true</tt>
 *          (either as a boolean or a {@link String})
 *          is the same as <tt>shallow</tt>.
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 *
 * <h4>Transition Nodes</h4>
 * Transition (&quot;<tt>transition</tt>&quot;) nodes
 * have the following attributes (bold are required):
 *  <blockquote>
 *    <table width="100%">
 *      <tr valign="top">
 *        <th width="20%">Name</th>
 *        <th width="20%">Type</th>
 *        <th width="60%">Explanation</th>
 *      </tr>
 *      <tr valign="top">
 *        <td><b><tt>on</tt></b></td>
 *        <td>event, {@link String}, or array thereof</td>
 *        <td>
 *          The event name(s).
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>to</tt></td>
 *        <td>{@link String}</td>
 *        <td>
 *          The target state's name.
 *          If omitted, this is an &quot;internal&quot; transition.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>condition</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          A boolean function evaluated just prior to the transition being
 *          taken.
 *        </td>
 *      </tr>
 *      <tr valign="top">
 *        <td><tt>action</tt></td>
 *        <td>{@link Closure}</td>
 *        <td>
 *          Code executed during transition.
 *        </td>
 *      </tr>
 *    </table>
 *  </blockquote>
 * Within the closure for either <tt>condition</tt> or <tt>action</tt>,
 * there is one variable available:
 *  <ol>
 *    <li>The {@link Event} that triggered the transition.</li>
 *  </ol>
 *
 * <h4>Enter and Exit Event Nodes</h4>
 * Enter (&quot;<tt>enter</tt>&quot;) and exit (&quot;<tt>exit</tt>&quot;)
 * event nodes have only a single value:
 * the name of the state either being entered or exited, e.g.:
 *  <pre>
 *    enter("off")</pre>
 * Enter/exit event nodes may be used only as the value of the
 * <tt>on</tt> attribute for transition nodes, e.g.:
 *  <pre>
 *    transition( on:enter("a"), to:"b" );</pre>
 *
 * <h4>
 * @author Paul J. Lucas
 */
public class CHSM_Builder extends BuilderSupport {

    ////////// public /////////////////////////////////////////////////////////

    /**
     * Invokes the method having the given name.  For a {@code CHSM_Builder},
     * the names of events are methods.  This allows events to be broadcasted
     * from within the machine description, e.g.:
     *  <pre>
     *  CG_Machine chsm = new CHSM_Builder().chsm() {
     *      state("a") {
     *          transition( on:"alpha", to:"b", action:{ beta(); } );
     *      }
     *      state("b");
     *  }</pre>
     *
     * @param name The method's name.
     * @param args The method's arguments.
     * @return Returns {@code null} if the given name refers to an event or, if
     * not, whatever the super method returns.
     * @see CG_Machine#invokeMethod(String,Object)
     */
    public Object invokeMethod( String name, Object args ) {
        final CG_Event cgEvent = m_cgMachine.getEvent( name );
        if ( cgEvent != null )
            return m_cgMachine.invokeMethod( name, args );
        return super.invokeMethod( name, args );
    }

    ////////// package ////////////////////////////////////////////////////////

    /**
     * Gets the {@link CG_State} that is currently being built.
     *
     * @return Returns said {@link CG_State}.
     */
    CG_State getCurrentState() {
        return (CG_State)getCurrent();
    }

    /**
     * Gets the {@link CG_Machine} that is currently being built.
     *
     * @return Returns said {@link CG_Machine}.
     */
    CG_Machine getMachine() {
        return m_cgMachine;
    }

    /**
     * Qualifies an unqualified state name.
     *
     * @param uqStateName The unqualified state name.
     * @param cgParent The {@link CG_Parent} to start from.
     * @return Returns the fully qualified state name.
     * @throws IllegalArgumentException if the unqualified state name contains
     * a {@code '.'}.
     * @see #qualifyStateName(String)
     * @see #resolveStateName(String)
     */
    static String qualifyStateName( String uqStateName, CG_Parent cgParent ) {
        if ( uqStateName.indexOf( '.' ) > -1 )
            throw new IllegalArgumentException(
                "state names may not contain '.' characters"
            );
        if ( cgParent != null ) {
            final String parentName = cgParent.getName();
            if ( !parentName.equals( "root" ) )
                return parentName + '.' + uqStateName;
        }
        return uqStateName;
    }

    ////////// protected //////////////////////////////////////////////////////

    /**
     * Creates a node.  This method is called when code is of the form:
     *  <blockquote>
     *    <i>typeName</i><tt>()</tt>
     *  </blockquote>
     *
     * @param typeName The type name of the node to create.
     * @return Returns said node or {@code null} if none.
     * @see #createNode(Object,Object)
     * @see #createNode(Object,Map)
     * @see #createNode(Object,Map,Object)
     */
    protected Object createNode( Object typeName ) {
        return createNode( typeName, Collections.EMPTY_MAP, null );
    }

    /**
     * Creates a node.  This method is called when code is of the form:
     *  <blockquote>
     *    <i>typeName</i><tt>(</tt><i>value</i><tt>)</tt>
     *  </blockquote>
     *
     * @param typeName The type name of the node to create.
     * @param value The single value.
     * @return Returns said node or {@code null} if none.
     * @see #createNode(Object)
     * @see #createNode(Object,Map)
     * @see #createNode(Object,Map,Object)
     */
    protected Object createNode( Object typeName, Object value ) {
        return createNode( typeName, Collections.EMPTY_MAP, value );
    }

    /**
     * Creates a node.  This method is called when code is of the form:
     *  <blockquote>
     *    <i>typeName</i><tt>(</tt>
     *      <i>key</i><tt>:</tt><i>value</i>
     *      <b>[</b><tt>,</tt>
     *        <i>key</i><tt>:</tt><i>value</i>
     *      <b>]</b>...
     *    <tt>)</tt>
     *  </blockquote>
     *
     * @param typeName The type name of the node to create.
     * @param attributes The node's attributes.
     * @return Returns said node or {@code null} if none.
     * @see #createNode(Object)
     * @see #createNode(Object,Object)
     * @see #createNode(Object,Map,Object)
     */
    protected Object createNode( Object typeName, Map attributes ) {
        return createNode( typeName, attributes, null );
    }

    /**
     * Creates a node.  This method is called by all the other
     * {@code createNode()} methods.
     *
     * @param typeName The type name of the node to create.
     * @param attributes The node's attributes.
     * @param value The single value.
     * @return Returns said node or {@code null} if none.
     * @see #createNode(Object)
     * @see #createNode(Object,Object)
     * @see #createNode(Object,Map)
     */
    protected Object createNode( Object typeName, Map attributes,
                                 Object value ) {
        if ( typeName.equals( "chsm" ) ) {
            if ( m_cgMachine.getRoot() == null )
                return createRootCluster( attributes );
            throw new IllegalArgumentException(
                "\"chsm\" node allowed only once"
            );
        }

        if ( typeName.equals( "enter" ) || typeName.equals( "exit" ) ) {
            if ( !(value instanceof String) )
                throw new IllegalArgumentException(
                    "state name expected for enter/exit event"
                );
            return createEnterOrExitEvent( (String)typeName, (String)value );
        }

        if ( typeName.equals( "event" ) )
            return createEvent( (String)value, attributes );

        if ( typeName.equals( "state" ) ||
             typeName.equals( "cluster" ) ||
             typeName.equals( "set" ) )
            return createState( (String)typeName, attributes, (String)value );

        if ( typeName.equals( "transition" ) ) {
            createTransition( attributes );
            //
            // We return null because createTransition() can actually create
            // more than one transition.
            //
            return null;
        }

        throw new IllegalArgumentException(
            "\"" + typeName + "\": unexpected node type"
        );
    }

    /**
     * This is called for every node after all of its child nodes have been
     * applied to it.
     * 
     * @param parent The parent of the node that was just completed or
     * {@code null} if none.
     * @param node The node that was just completed.
     * @return Returns either the given node or the {@link CG_Machine}.
     */
    protected Object postNodeCompletion( Object parent, Object node ) {
        if ( node == m_outerDeepHistory ) {
            //
            // We've unnested from the outer-most cluster node that had a "deep
            // history" specified for it: clear the pointer.
            //
            m_outerDeepHistory = null;
        }

        if ( parent == null && node == m_cgMachine.getRoot() ) {
            //
            // The top-level state (which obviously has no parent) has just
            // been completed: convert to a real CHSM and return the machine as
            // a whole rather than the root node.
            //
            m_cgMachine.toCHSM();
            return m_cgMachine;
        }

        return node;
    }

    /**
     * Sets the parent node of the given child node.
     *
     * @param parent The parent node.
     * @param node The child node.
     */
    protected void setParent( Object parent, Object node ) {
        if ( parent instanceof CG_Parent && node instanceof CG_State ) {
            final CG_Parent cgParent = (CG_Parent)parent;
            final CG_State cgChild = (CG_State)node;
            cgParent.addChild( cgChild );
            cgChild.setParent( cgParent );
            if ( cgChild instanceof CG_Cluster && m_outerDeepHistory != null ) {
                //
                // The current child node is a cluster nested inside another
                // cluster that had "deep history" specified for it: therefore,
                // set the current cluster to have a history too.
                //
                ((CG_Cluster)cgChild).setHistory( true );
            }
            //System.out.println( "setParent(" + parent + ',' + child + ')' );
        }
    }

    ////////// private ////////////////////////////////////////////////////////

    /**
     * Creates an enter or exit event.
     *
     * @param typeName Either &quot;enter&quot; or &quot;exit&quot;.
     * @param uqStateName The unqualified state name either being entered or
     * exited.
     * @return Returns an event.
     */
    private CG_Event createEnterOrExitEvent( String typeName,
                                             String uqStateName ) {
        final String fqStateName = resolveStateName( uqStateName );
        CG_State cgState = getState( fqStateName );
        final String eventName = typeName + '(' + fqStateName + ')';
        CG_Event cgEvent = m_cgMachine.getEvent( eventName );
        if ( cgEvent == null ) {
            cgEvent = new CG_Event( eventName, null, null, m_cgMachine );
            m_cgMachine.addEvent( cgEvent );
        }
        if ( typeName.equals( "enter" ) )
            cgState.setEnterEvent( cgEvent );
        else
            cgState.setExitEvent( cgEvent );
        return cgEvent;
    }

    /**
     * Creates a {@link CG_Event} having the given attributes.
     *
     * @param name The name of the event or {@code null} if the name is to be
     * taken from the &quot;name&quot; attribute.
     * @param attributes The event's attributes or {@code null} if none.
     * @return Returns a new {@link CG_Event}.
     */
    private CG_Event createEvent( String name, Map attributes ) {

        ////////// Process "name" attribute.

        if ( name == null )
            name = getAttribute( String.class, "name", attributes, true );

        ////////// Process "base" attribute.

        CG_Event cgBaseEvent = null;
        final String baseName =
            getAttribute( String.class, "base", attributes, false );
        if ( baseName != null ) {
            cgBaseEvent = m_cgMachine.getEvent( baseName );
            if ( cgBaseEvent == null )
                throw new IllegalArgumentException(
                    "base event \"" + baseName + "\" undeclared"
                );
        }

        ////////// Process "precondition" attribute.
            
        final Closure cgPrecondition =
            getAttribute( Closure.class, "precondition", attributes, false );

        ////////// Create event.
        
        CG_Event cgEvent = m_cgMachine.getEvent( name );
        if ( cgEvent != null )
            throw new IllegalStateException(
                "event \"" + name + "\" already declared"
            );
        cgEvent = new CG_Event(
            name, cgPrecondition, cgBaseEvent, m_cgMachine
        );
        m_cgMachine.addEvent( cgEvent );

        return cgEvent;
    }

    /**
     * Creates the root {@link CG_Cluster}.
     *
     * @param attributes The cluster's attributes or {@code null} if none.
     * @return Returns said {@link CG_Cluster}.
     */
    private CG_Cluster createRootCluster( Map attributes ) {
        final Closure cgEnterAction =
            getAttribute( Closure.class, "enter", attributes, false );
        final Closure cgExitAction =
            getAttribute( Closure.class, "exit", attributes, false );
        final CG_Cluster root =
            new CG_Cluster( null, "root", cgEnterAction, cgExitAction );
        setHistory( root, attributes );
        m_cgMachine.setRoot( root );
        return root;
    }

    /**
     * Creates a {@link CG_State}.
     *
     * @param typeName The name of the type of state to create.
     * @param attributes The state's attributes if none.
     * @param uqStateName The unqualified name of the state or
     * {@code null} if the name is to be taken from the &quot;name&quot;
     * attribute.
     * @return Returns a new {@link CG_State}.
     */
    private CG_State createState( String typeName, Map attributes,
                                  String uqStateName ) {

        ////////// Process "name" attribute.

        if ( uqStateName == null )
            uqStateName = getAttribute(
                String.class, "name", attributes, true
            );

        ////////// Ensure state has been declared as a child of parent.

        final CG_Parent cgCurrentParent = getCurrentParent();
        if ( cgCurrentParent != m_cgMachine.getRoot() &&
             cgCurrentParent.getChild( uqStateName ) == null )
            throw new IllegalStateException(
                "\"" + uqStateName + "\": undeclared child state"
            );

        ////////// Process "enter" and "exit" attributes.

        final Closure cgEnterAction =
            getAttribute( Closure.class, "enter", attributes, false );
        final Closure cgExitAction =
            getAttribute( Closure.class, "exit", attributes, false );

        ////////// Process "class" attribute.
        
        final Class<? extends State> stateClass =
            getAttribute( Class.class, "class", attributes, false );

        ////////// Create state.

        final String fqStateName = qualifyStateName( uqStateName );
        CG_State cgState = m_fqStateNameMap.get( fqStateName );
        if ( cgState == null ) {
            cgState = createState(
                typeName, stateClass, fqStateName,
                cgEnterAction, cgExitAction
            );
            if ( m_cgMachine.addState( cgState ) )
                m_fqStateNameMap.put( fqStateName, cgState );
        } else if ( cgState instanceof CG_StateRef ) {
            final CG_StateRef ref = (CG_StateRef)cgState;
            if ( ref.getReferent() == null ) {
                //
                // The state already exists as a CG_StateRef and its referrent
                // state is null.  This means that the previously refererenced
                // forward reference to a state is now being declared.  We can
                // now set the referent state of the CG_StateRef.
                //
                final CG_State cgReferentState = createState(
                    typeName, stateClass, fqStateName,
                    cgEnterAction, cgExitAction
                );
                ref.setReferent( cgReferentState );
                cgState = cgReferentState;
                m_fqStateNameMap.put( fqStateName, cgState );
            }
        }

        ////////// Process "children" attribute.

        final Object children = attributes.get( "children" );
        if ( children != null ) {
            if ( !(cgState instanceof CG_Parent) )
                throw new IllegalArgumentException(
                    "\"children\" can be specified only for a cluster or set"
                );
            if ( !(children instanceof Collection) )
                throw new IllegalArgumentException(
                    "array expected for cluster or set \"children\" attribute"
                );
            final CG_Parent cgParent = (CG_Parent)cgState;
            for ( Object child : (Collection)children ) {
                final String uqChildName = (String)child;
                final String fqChildName = fqStateName + '.' + uqChildName;
                final CG_State cgChildState = new CG_StateRef( fqChildName );
                if ( m_cgMachine.addState( cgChildState ) )
                    m_fqStateNameMap.put( fqChildName, cgChildState );
                cgParent.addChild( cgChildState );
                cgChildState.setParent( cgParent );
            }
        }

        ////////// Process "history" attribute.

        setHistory( cgState, attributes );

        return cgState;
    }

    /**
     * Creates a {@link CG_State}.
     *
     * @param typeName The name of the type of state to create.
     * @param stateClass The {@link Class} of the set to create or {@code null}
     * for the default.
     * @param fqStateName The fully qualified name of the state.
     * @param enterAction The &quot;enter&quot; action or {@code null} if none.
     * @param exitAction The &quot;exit&quot; action or {@code null} if none.
     * @return Returns the new {@link CG_State}.
     */
    private CG_State createState( String typeName,
                                  Class<? extends State> stateClass,
                                  String fqStateName,
                                  Closure enterAction, Closure exitAction ) {
        if ( typeName.equals( "state" ) )
            return new CG_State(
                stateClass, fqStateName, enterAction, exitAction
            );
        if ( typeName.equals( "cluster" ) )
            return new CG_Cluster(
                stateClass, fqStateName, enterAction, exitAction
            );
        if ( typeName.equals( "set" ) )
            return new CG_Set(
                stateClass, fqStateName, enterAction, exitAction
            );
        throw new IllegalArgumentException(
            "typeName must be \"state\", \"cluster\", or \"set\""
        );
    }

    /**
     * Creates one or more {@link CG_Transition}s having the given attributes.
     *
     * @param attributes The transition's attributes.
     */
    private void createTransition( Map attributes ) {

        ////////// Process "to" attribute.

        final CG_State cgTo;
        final String uqStateName =
            getAttribute( String.class, "to", attributes, false );
        if ( uqStateName != null ) {
            final String fqStateName = resolveStateName( uqStateName );
            cgTo = getState( fqStateName );
        } else
            cgTo = null;

        ////////// Process "condition" and "action" attributes.

        final Closure cgCondition =
            getAttribute( Closure.class, "condition", attributes, false );
        final Closure cgAction =
            getAttribute( Closure.class, "action", attributes, false );

        ////////// Process "on" attribute.

        final Object on = getAttribute( Object.class, "on", attributes, true );
        if ( on instanceof Collection )
            for ( Object event : (Collection)on )
                createTransition( event, cgTo, cgCondition, cgAction );
        else
            createTransition( on, cgTo, cgCondition, cgAction );
    }

    /**
     * Creates a {@link CG_Transition}.
     *
     * @param event The event that triggers the transition.  It can be either
     * an actual {@link CG_Event} or the {@link String} name of the event.
     * @param cgTo The state transitioned to.
     * @param cgCondition The condition or {@code null} if none.
     * @param cgAction The action or {@code null} if none.
     */
    private void createTransition( Object event, CG_State cgTo,
                                   Closure cgCondition, Closure cgAction ) {
        final CG_Event cgEvent;
        if ( event instanceof CG_Event )
            cgEvent = (CG_Event)event;
        else if ( event instanceof String )
            cgEvent = getEvent( (String)event );
        else
            throw new IllegalArgumentException(
                "event, event name, or array thereof expected " +
                "for transition \"on\" attribute"
            );
        final CG_Transition t = new CG_Transition(
            0, getCurrentState(), cgTo, cgCondition, cgAction
        );
        m_cgMachine.addTransition( t );
        cgEvent.addTransition( t );
    }

    /**
     * Gets the value of an attribute having the given name.
     *
     * @param requiredClass The {@link Class} that the attribute's value must
     * be of.
     * @param attName The name of the attribute.
     * @param attributes The attributes.
     * @param required If {@code true}, the attribute is required.
     * @return Returns said value or {@code null} if none.
     * @throws IllegalArgumentException if an attribute having the given name
     * does not exist and is required, or does exist, but whose value is not of
     * the required class.
     */
    private <T> T getAttribute( Class requiredClass, String attName,
                                Map attributes, boolean required ) {
        final Object attValue =
            attributes != null ? attributes.get( attName ) : null;
        if ( attValue == null && required )
            throw new IllegalArgumentException(
                "\"" + attName + "\" attribute required"
            );
        if ( attValue != null &&
             !(requiredClass.isAssignableFrom( attValue.getClass() ) ) )
            throw new IllegalArgumentException(
                requiredClass.getSimpleName() +
                " expected for \"" + attName + "\" attribute"
            );
        //noinspection unchecked
        return (T)attValue;
    }

    /**
     * Gets the current parent state, i.e., the state to which newly declared
     * states are added to as child states.
     *
     * @return Returns the current {@link CG_Parent} or {@code null} if none.
     */
    private CG_Parent getCurrentParent() {
        CG_State cgState = getCurrentState();
        if ( cgState != null && !(cgState instanceof CG_Parent) )
            cgState = cgState.getParent();
        return (CG_Parent)cgState;
    }

    /**
     * Gets the {@link CG_Event} having the given name creating it if
     * necessary.
     *
     * @param name The name of the event.
     * @return Returns the {@link CG_Event} having the given name.
     */
    private CG_Event getEvent( String name ) {
        CG_Event cgEvent = m_cgMachine.getEvent( name );
        if ( cgEvent == null )
            cgEvent = createEvent( name, null );
        return cgEvent;
    }

    /**
     * Gets the {@link CG_State} having the given name creating a forward
     * reference to it if necessary.
     *
     * @param fqStateName The fully qualified state name.
     * @return Returns the {@link CG_State} having the given name.
     */
    private CG_State getState( String fqStateName ) {
        CG_State cgState = m_fqStateNameMap.get( fqStateName );
        if ( cgState == null ) {
            //
            // A state having the given name hasn't been seen yet, hence this
            // is a forward reference to an as-of-yet undeclared state.
            //
            cgState = new CG_StateRef( fqStateName );
            CG_Parent cgParent = getCurrentParent();
            if ( cgParent == getCurrentState() ) {
                //
                // The current parent is the current state.  This means that
                // this transition is from the current parent to another state.
                // That means that the parent of the target state isn't the
                // current state.
                //
                // TODO: not sure if the following is correct in all cases.
                //
                cgParent = cgParent.getParent();
            }
            cgState.setParent( cgParent );
            if ( m_cgMachine.addState( cgState ) )
                m_fqStateNameMap.put( fqStateName, cgState );
        }
        return cgState;
    }

    /**
     * Qualifies an unqualified state name.
     *
     * @param uqStateName The unqualified state name.
     * @return Returns the fully qualified state name.
     * @see #qualifyStateName(String,CG_Parent)
     * @see #resolveStateName(String)
     */
    private String qualifyStateName( String uqStateName ) {
        return qualifyStateName( uqStateName, getCurrentParent() );
    }

    /**
     * Resolves a state name used in a transition to its fully qualified
     * version.
     *
     * @param stateName The state name.
     * @return Returns the fully qualified state name.
     * @see #qualifyStateName(String)
     */
    private String resolveStateName( String stateName ) {
        if ( stateName.startsWith( "::" ) ) {
            //
            // The user is asserting that the state name is fully qualified
            // as-is.
            //
            return stateName.substring( 2 );
        }

        CG_Parent cgParent = getCurrentParent();
        final CG_Cluster cgRoot = m_cgMachine.getRoot();
        if ( cgParent == null || cgParent == cgRoot ) {
            //
            // We're at the top-level, so assume the name is fully qualified.
            //
            return stateName;
        }

        final String[] partNames = stateName.split( "\\." );
        while ( cgParent != null ) {

            if ( partNames[0].equals( cgParent.getShortName() ) &&
                 partNames.length == 1 )
                return cgParent.getName();

            for ( CG_State cgChild : cgParent ) {
                if ( partNames[0].equals( cgChild.getShortName() ) ) {
                    final StringBuilder sb = new StringBuilder();
                    sb.append( cgChild.getName() );
                    for ( int p = 1; p < partNames.length; ++p ) {
                        sb.append( '.' );
                        sb.append( partNames[ p ] );
                    }
                    return sb.toString();
                }
            }

            cgParent = cgParent.getParent();
        }

        //
        // We weren't able to resolve the state name, so assume the name is
        // fully qualified and that it refers to an as-of-yet unseen top-level
        // state.
        //
        return stateName;
    }

    /**
     * Sets the history of the given {@link CG_Cluster} and also keeps track of
     * deep history.
     *
     * @param cgState The {@link CG_State} to set the history for.
     * @param attributes The attributes.
     * @throws IllegalArgumentException if the state isn't a {@link CG_Cluster}.
     */
    private void setHistory( CG_State cgState, Map attributes ) {
        if ( attributes == null )
            return;
        final Object value = attributes.get( "history" );
        if ( value != null ) {
            if ( !(cgState instanceof CG_Cluster) )
                throw new IllegalArgumentException(
                    "\"history\" can be specified only for a cluster"
                );
            final CG_Cluster cgCluster = (CG_Cluster)cgState;
            if ( value instanceof Boolean )
                cgCluster.setHistory( (Boolean)value );
            else if ( value instanceof String ) {
                final String history = (String)value;
                if ( history.equals( "shallow" ) || history.equals( "true" ) )
                    cgCluster.setHistory( true );
                else if ( history.equals( "deep" ) ) {
                    cgCluster.setHistory( true );
                    if ( m_outerDeepHistory == null )
                        m_outerDeepHistory = cgCluster;
                } else if ( !history.equals( "false" ) )
                    throw new IllegalArgumentException(
                        "\"" + history + "\": illegal value for \"history\" attribute"
                    );
            } else
                throw new IllegalArgumentException(
                    "boolean or string expected for \"history\" attribute"
                );
        }
    }

    /**
     * The {@link CG_Machine} being built.
     */
    private final CG_Machine m_cgMachine = new CG_Machine();

    /**
     * A mapping of fully qualified state names to {@link CG_State} objects.
     */
    private final Map<String,CG_State> m_fqStateNameMap =
        new HashMap<String,CG_State>();

    /**
     * The outermost {@link CG_Cluster} that has a deep history.
     */
    private CG_Cluster m_outerDeepHistory;
}
/* vim:set et sw=4 ts=4: */
