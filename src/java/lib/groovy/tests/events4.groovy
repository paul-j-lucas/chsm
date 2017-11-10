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
 * Tests that derived events dominate in the same scope regardless of
 * specification order.
 */
class events4_TestCase extends GroovyTestCase {
    int exitCode = 0;

    void test() {
        CG_Machine m = new CHSM_Builder().chsm() {
            event( name:"alpha" );
            event( name:"beta", base:"alpha" );

            set( name:"s", children:["c","d"] ) {
                cluster( name:"c", children:["x","a","b"] ) {
                    state("x") {
                        transition( on:"alpha", to:"a" );
                        transition( on:"beta", to:"b" );
                    }
                    state("a");
                    state("b");
                }
                cluster( name:"d", children:["x","a","b"] ) {
                    state("x") {
                        transition( on:"beta", to:"b" );
                        transition( on:"alpha", to:"a" );
                    }
                    state("a");
                    state("b");
                }
            }
        };

        m.enter();
        m.debug( Machine.D_ALL );
        m.beta();
        m.dumpState();

        if ( !( m.active() &&
            m.s.active() &&
            m.s.c.active() && m.s.d.active() &&
            !m.s.c.x.active() && !m.s.c.a.active() && m.s.c.b.active() &&
            !m.s.d.x.active() && !m.s.d.a.active() && m.s.d.b.active()
        ) )
            exitCode = 1;
        assert exitCode == 0;
    }
}
/* vim:set et sw=4 ts=4: */
