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

import CHSM.Machine;
import CHSM.groovy.*;
import groovy.util.GroovyTestCase;

/**
 * Tests preconditions.
 */
class precondition_TestCase extends GroovyTestCase {
    int exitCode = 0;
    int g;

    void test() {
        CG_Machine m = new CHSM_Builder().chsm() {
            event( name:"A" );
            event( name:"B", precondition:{ g > 0 } );

            event( name:"C", base:"A", precondition:{ g > 0 } );
            event( name:"D", base:"A", precondition:{ it["w"] > 0 } );

            event( name:"E", base:"B" );
            event( name:"F", base:"B", precondition:{ it["x"] > 0 } );

            event( name:"G", base:"E" );
            event( name:"H", base:"E", precondition:{ it["y"] > 0 } );

            event( name:"I", base:"F" );
            event( name:"J", base:"F", precondition:{ it["z"] > 0 } );

            state("a") {
                transition( on:"B", to:"b" );
            }
            state("b") {
                transition( on:["I","J"], to:"c" );
            }
            state("c");
        };

        m.enter();
        m.debug( Machine.D_ALL );

        while ( true ) {
            g = 0; m.B();
            if ( m.b.active() )
                break;

            g = 1; m.B();
            if ( !m.b.active() )
                break;

            m.I( ["x":0] );
            if ( m.c.active() )
                break;

            m.J( ["x":1, "z":1] );
            if ( !m.c.active() )
                break;

            exitCode = 0;
            break;
        };

        assert exitCode == 0;
    }
}
/* vim:set et sw=4 ts=4: */
