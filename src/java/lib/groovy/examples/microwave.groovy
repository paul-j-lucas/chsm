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

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

import CHSM.Machine;
import CHSM.groovy.*
import groovy.swing.SwingBuilder;

/**
 * A <code>MicrowaveOvenModel</code> contains the necessary information for a
 * microwave oven.
 */
class MicrowaveOvenModel {
    int cookMinutes, cookSeconds;
    boolean isDoorOpen, isLightOn;
    int powerLevel;                     // 10-100% in 10% increments
    int timeHour = 12, timeMinute = 0;

    public void advanceTime() {
        if ( ++timeMinute > 59 ) {
            timeMinute = 0;
            if ( ++timeHour > 12 )
                timeHour = 1;
        }
    }

    public boolean countdownTimer() {
        if ( --cookSeconds < 0 ) {
            if ( --cookMinutes < 0 ) {
                cookMinutes = cookSeconds = 0;
                return false;
            }
            cookSeconds = 59;
        }
        return true;
    }

    public boolean isTimeOK() {
        return timeHour in 1..12 && timeMinute in 0..59;
    }

}

/**
 * A <code>MicrowaveOvenView</code> displays the user interface for a microwave
 * oven.
 */
class MicrowaveOvenView {

    ////////// public /////////////////////////////////////////////////////////

    public MicrowaveOvenView( MicrowaveOvenModel model ) {
        m_model = model;
        buildUI();
    }

    public void displayDone() {
        m_display.text = "Done";
    }

    public void displayPowerLevelPrompt() {
        m_display.text = "Power?";
    }

    public void displayTime() {
        m_display.text =
            sprintf( "%02d:%02d", m_model.timeHour, m_model.timeMinute );
    }

    public void displayTimeError() {
        m_display.text = "Time: EE";
    }

    public void displayTimer() {
        m_display.text =
            sprintf( "%02d:%02d", m_model.cookMinutes, m_model.cookSeconds );
    }

    public void setController( MicrowaveOven controller ) {
        m_machine = controller;
    }

    public void updateLight() {
        m_door.background = m_model.isLightOn ? Color.YELLOW : Color.BLACK;
    }

    public void updateDoor() {
        m_door.text = m_model.isDoorOpen ? "(door is open)" : "";
    }

    ////////// private ////////////////////////////////////////////////////////

    private void buildUI() {
        def frame = new SwingBuilder().frame(
                title:"Microwave Oven",
                size:[600,300] ) {
            gridLayout( columns:2, rows:1 );
            m_door = textArea(
                background:Color.BLACK,
                editable:false,
                focusable:false,
                foreground:Color.WHITE
            );
            panel {
                gridLayout( columns:3, rows:6 );
                m_display = textField(
                    text:"00:00",
                    horizontalAlignment:JTextField.CENTER,
                    editable:false,
                    focusable:false
                );
                ["Open","Close"].each { name ->
                    button(
                        text:name,
                        actionPerformed:{
                            m_machine.broadcast( name, null );
                        },
                        focusable:false
                    );
                };
                ["7","8","9","4","5","6","1","2","3","0"].each { name ->
                    final int n = Integer.parseInt( name );
                    button(
                        text:name,
                        actionPerformed:{
                            //
                            // The Digit event takes a single parameter "n"
                            // that is the integer value of the digit.
                            //
                            m_machine.broadcast( "Digit", [n:n] );
                        },
                        focusable:false
                    );
                };
                ["Power","Minute","Start","Stop","Clock"].each { name ->
                    button(
                        text:name,
                        actionPerformed:{
                            m_machine.broadcast( name, null );
                        },
                        focusable:false
                    );
                };
            }
        }
        frame.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
        frame.pack();
        frame.show();
    }

    private JTextField m_display;
    private JTextArea m_door;
    private MicrowaveOven m_machine;
    private final MicrowaveOvenModel m_model;
}

/**
 * A <code>MicrowaveOven</code> is the controller for a microwave oven.
 */
class MicrowaveOven extends GroovyMachineSupport {

    ////////// public /////////////////////////////////////////////////////////

    public MicrowaveOven( MicrowaveOvenModel model ) {
        m_model = model;
        setMachine( buildMachine() );
/*
        final ActionListener listener = { Tick(); } as ActionListener;
        new Timer( 1000, { Tick(); } as ActionListener ).start();
*/
    }

    public void setView( MicrowaveOvenView view ) {
        m_view = view;
    }

    private Timer m_cookTimer;

    ////////// private ////////////////////////////////////////////////////////

    private CG_Machine buildMachine() {
        return new CHSM_Builder().chsm() {

            set( name:"components", children:["mode","display","light"] ) {

                /*************************************************************/

                cluster( name:"mode", children:["operational","disabled"] ) {

                    //
                    // operational state: the oven's door is closed
                    //
                    cluster( name:"operational",
                             children:["idle","program","cook","done","set_time"],
                             history:true ) {
                        transition( on:"Open", to:"disabled" );

                        //
                        // idle state: wait for somebody to come along
                        //
                        state( name:"idle",
                               enter:{
                                    m_view.displayTime();
                               } ) {
                            transition( on:"Digit", to:"program" );
                            transition(
                                on:"Minute",
                                to:"cook",
                                action:{
                                    m_model.cookMinutes = 1;
                                }
                            );
                            transition( on:"Clock", to:"set_time" );
                            transition(
                                on:"Tick",
                                action:{
                                    m_model.advanceTime();
                                    m_view.displayTime();
                                }
                            );
                        }

                        //
                        // program state: set the timer and power-level
                        //
                        cluster( name:"program", children:["setting","power"],
                                 history:true,
                                 enter:{ event ->
                                    //
                                    // Initialize the minutes/seconds, but only
                                    // if the event was Digit.  (The event
                                    // could be Close because the user opened
                                    // then closed the door that would have
                                    // exited and then re-entered the program
                                    // state.  In that case, we want to pick up
                                    // from where we left off, hence we don't
                                    // initialize anything.)
                                    //
                                    if ( event.name().equals( "Digit" ) ) {
                                        m_model.cookMinutes = 0;
                                        m_model.cookSeconds = event["n"];
                                        m_model.powerLevel = 100;
                                        m_view.displayTimer();
                                    }
                                 },
                                 exit:{ event, state ->
                                    if ( !event.name().equals( "Open" ) ) {
                                        state.clear();
                                    }
                                 } ) {
                            transition( on:"Stop", to:"idle" );

                            state("setting") {
                                transition(
                                    on:"Digit",
                                    action:{ event ->
                                        m_model.cookMinutes = shiftAdd(
                                            m_model.cookMinutes,
                                            (int)(m_model.cookSeconds / 10)
                                        );
                                        m_model.cookSeconds = shiftAdd(
                                            m_model.cookSeconds, event["n"]
                                        );
                                        m_view.displayTimer();
                                    }
                                );
                                transition( on:"Power", to:"power" );
                                transition( on:"Start", to:"cook" );
                            }

                            state("power") {
                                transition(
                                    on:"Digit",
                                    condition:{ event ->
                                        //
                                        // Insist on a greather-than-zero power
                                        // level.
                                        //
                                        event["n"] > 0
                                    },
                                    to:"setting",
                                    action:{ event ->
                                        m_model.powerLevel = event["n"] * 10;
                                    }
                                );
                            }
                        }

                        //
                        // cook state: cook food
                        //
                        state( name:"cook",
                               enter:{
                                    final Closure c = {
                                        final boolean done = !m_model.countdownTimer();
                                        m_view.displayTimer();
                                        if ( done )
                                            Done();
                                    };
                                    m_cookTimer = new Timer( 1000, c as ActionListener );
                                    m_cookTimer.start();
                               },
                               exit:{
                                    m_cookTimer.stop();
                                    m_cookTimer = null;
                               } ) {
                            transition( on:"Done", to:"done" );
                            transition( on:"Stop", to:"program" );
                            transition(
                                on:"Minute",
                                action:{
                                    ++m_model.cookMinutes;
                                    m_view.displayTimer();
                                }
                            );
                        }

                        state( name:"done",
                               enter:{
                                   m_view.displayDone();
                               },
                               exit:{ event, state ->
                                   state.parent().clear();
                               } ) {
                            transition( on:"Stop", to:"idle" );
                        }

                        //
                        // set_time state: set the time-of-day clock
                        //
                        cluster( name:"set_time", children:["setting","error"],
                                enter:{ event ->
                                    if ( !event.name().equals( "Close" ) ) {
                                        //
                                        // 
                                        //
                                        oldHour = m_model.timeHour;
                                        oldMinute = m_model.timeMinute;
                                        m_model.timeHour = 0;
                                        m_model.timeMinute = 0;
                                    }
                                    m_view.displayTime();
                                },
                                exit:{ event ->
                                    if ( !event.name().equals( "Open" ) ) {
                                        if ( !m_model.isTimeOK() ) {
                                            m_model.timeHour = oldHour;
                                            m_model.timeMinute = oldMinute;
                                        }
                                    }
                                    m_view.displayTime();
                                } ) {
                            transition( on:"Stop", to:"idle" );

                            state("setting") {
                                transition(
                                    on:"Digit",
                                    action:{ event ->
                                        m_model.timeHour = shiftAdd(
                                            m_model.timeHour,
                                            (int)(m_model.timeMinute / 10)
                                        );
                                        m_model.timeMinute = shiftAdd(
                                            m_model.timeMinute, event["n"]
                                        );
                                        m_view.displayTime();
                                    }
                                );
                                transition(
                                    on:"Clock",
                                    condition:{ m_model.isTimeOK() },
                                    to:"idle"
                                );
                                transition( on:"Clock", to:"error" );
                            }

                            state("error");
                        }
                    }

                    //
                    // disabled state: the oven's door is open
                    //
                    state( name:"disabled",
                           enter:{
                               m_model.isDoorOpen = true;
                               m_view.updateDoor();
                           },
                           exit:{
                               m_model.isDoorOpen = false;
                               m_view.updateDoor();
                           }
                        ) {
                        transition( on:"Close", to:"operational" );
                    }
                }

                /*************************************************************/

                cluster( name:"display", children:["time","not_time"] ) {

                    state( name:"time",
                           enter:{
                               m_view.displayTime();
                           } ) {
                        transition(
                            on:[
                                enter("mode.operational.program"),
                                enter("mode.operational.cook")
                            ],
                            to:"not_time.counter"
                        );
                        transition(
                            on:enter("mode.operational.set_time.error"),
                            to:"not_time.error"
                        );
                    }

                    cluster( name:"not_time",
                             children:["counter","power","error"] ) {
                        transition(
                            on:enter("mode.operational.idle"), to:"time"
                        );
                        transition(
                            on:exit("mode.operational.cook"), to:"counter"
                        );

                        state( name:"counter",
                               enter:{
                                   m_view.displayTimer();
                               } ) {
                            transition(
                                on:"Power",
                                condition:{
                                    components.mode.operational.program.active()
                                },
                                to:"power"
                            );
                        }

                        state( name:"power",
                               enter:{
                                   m_view.displayPowerLevelPrompt();
                               } ) {
                            transition(
                                on:"Digit",
                                condition:{ event -> event["n"] > 0 },
                                to:"counter"
                            );
                        }

                        state( name:"error",
                               enter:{
                                   m_view.displayTimeError();
                               } );
                    }
                }

                /*************************************************************/

                cluster( name:"light", children:["off","on"] ) {
                    state( name:"off",
                           enter:{
                               m_model.isLightOn = false;
                               m_view.updateLight();
                           } ) {
                        transition(
                            on:enter("mode.operational.cook"), to:"on"
                        );
                    }
                    state( name:"on",
                           enter:{
                               m_model.isLightOn = true;
                               m_view.updateLight();
                           } ) {
                        transition(
                            on:exit("mode.operational.cook"), to:"off"
                        );
                    }
                }
            }
        }
    }

    private int shiftAdd( int n, int amount ) {
        if ( (n = n * 10 + amount) >= 100 )
            n %= 100;
        return n;
    }

    private final MicrowaveOvenModel m_model;
    private MicrowaveOvenView m_view;
    private int oldHour, oldMinute;
}

MicrowaveOvenModel ovenModel = new MicrowaveOvenModel();
MicrowaveOvenView ovenView = new MicrowaveOvenView( ovenModel );

MicrowaveOven oven = new MicrowaveOven( ovenModel );

ovenView.setController( oven );
oven.setView( ovenView );

oven.enter();
oven.debug( Machine.D_ALL );

/* vim:set et sw=4 ts=4: */
