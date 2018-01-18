/*
** CHSM/Groovy
**
** Copyright (C) 2007-2018  Paul J. Lucas & Fabio Riccardi
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

import CHSM.Machine;
import CHSM.groovy.*

/**
 * An item sold by a vending machine.
 */
class VM_Item {
    final String    name;   // name of yummy item
    final int       price;  // cost in cents

    public VM_Item( String s, int i ) {
        name = s;
        price = i;
    }
}

class VendingMachine extends GroovyMachineSupport {

    public VendingMachine( List<VM_Item> items ) {
        m_items = items;
        setMachine( buildMachine() );
    }

    public int numItems() {
        return m_items.size();
    }

    private final List<VM_Item> m_items;
    private int m_credit;   // running total of amount deposited

    private CG_Machine buildMachine() {
        return new CHSM_Builder().chsm() {

            /********** EVENT SPECIFICATIONS **********/
            
            event(
                name:"coin",
                precondition:{ param ->

                    // must be valid denomination
                    switch ( param["value"] ) {
                        case [5,10,25]:
                            return true;
                        case 1:
                            println "No pennies, please.";
                            return false;
                        default:
                            println "No foreign coins!";
                            return false;
                    }
                }
            );

            event(
                name:"select",
                precondition:{ param ->

                    // selection must be within range
                    final int id = param["id"];
                    if ( id < 0 || id >= numItems() ) {
                        println "Invalid selection";
                        return false;
                    }

                    // must have deposited sufficient amount
                    final int price = m_items[ id ].price;
                    if ( m_credit >= price )
                        return true;

                    println "Please deposit another " + (price - m_credit) +
                            " cents.";
                    return false;
                }
            );

            /********** STATE SPECIFICATIONS **********/

            //
            // idle state: We sit here waiting for somebody to come along and
            //      insert the first coin.
            //
            state(
                name:"idle",
                enter:{
                    println "\nVending Machine:\n---------------";
            		int i = 0;
                    while ( i < numItems() ) {
                        final VM_Item item = m_items[ i ];
                        final char c = (char)(65 + i);
                        println "" + c + ". " + item.name + " (" + item.price + ')';
                        ++i;
                    }
                    m_credit = 0;
                }
            ) {
                transition( on:"coin", to:"collect" );
            }

            //
            // collect state: We enter here upon a coin insertion and reenter
            //      for every coin insertion thereafter until a selection is
            //      made.
            //
            state(
                name:"collect",
                enter:{ event ->
                    m_credit += event["value"];
                    println "Credit: " + m_credit + " cents";
                }
            ) {
                transition( on:"coin", to:"collect" );
                transition(
                    on:"select",
                    to:"idle",
                    action:{ event ->
                        final VM_Item item = m_items[ event[ "id" ] ];
                        println "Enjoy your " + item.name + '.';
                        final int change = m_credit - item.price;
                        if ( change > 0 )
                            println "Change: " + change + " cents.";
                    }
                );
            }
        };
    }
}

public class VendingMachineUI {
    private static final List<VM_Item> items = [
        new VM_Item( "Cheeze Puffs",    65 ),
        new VM_Item( "Chocolate Bar",   60 ),
        new VM_Item( "Corn Chips",      80 ),
        new VM_Item( "Popcorn",         90 ),
        new VM_Item( "Potato Chips",    80 ),
        new VM_Item( "Pretzels",        60 )
    ];

    public static void main( String[] args ) {
        VendingMachine vm = new VendingMachine( items );
        vm.enter();
        //m.debug( Machine.D_ALL );

        final BufferedReader input =
            new BufferedReader( new InputStreamReader( System.in ) );
        while ( true ) {
            //
            // Simple user-interface just to get events into the machine.
            //
            final char A = 'A'.charAt(0);
            final char c = (char)(A + vm.numItems() - 1);
            print "\nEnter coins (5,10,25) or selection (A-" + c + "): ";

            String buf;
            try {
                buf = input.readLine();
            }
            catch ( IOException e ) {
                System.exit(0);
            }
            if ( buf == null )
                System.exit(0);
            final char ch = buf.charAt(0);

            if ( Character.isDigit( ch ) )
                vm.coin( [value:Integer.parseInt( buf )] );
            else if ( Character.isLetter( ch ) )
                vm.select( [id:Character.toUpperCase( ch ) - A] );
            else
                println "Invalid input";
        }
    }
}

VendingMachineUI.main( null );
/* vim:set et sw=4 ts=4: */
