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

/**
 * @file
 * Tests that erroneous transitions do not violate CHSM outer scope semantics
 * (which are the same as a cluster's).
 */

import CHSM.Machine;
import CHSM.groovy.*;
import groovy.util.GroovyTestCase;

///////////////////////////////////////////////////////////////////////////////

class erroneous2_TestCase extends GroovyTestCase {
  int exitCode = 0;

  void test_erroneous2() {
    CG_Machine m = new CHSM_Builder().chsm() {
      set( name:"s", children:["a","b"] ) {
        state("a") {
          transition( on:"alpha", to:"x" );
        }
        state("b") {
          transition(
            on:"alpha",
            to:"y",
            action:{ exitCode = 2; }
          );
        }
      }
      state("x");
      state("y");
    };

    m.enter();
    m.alpha();

    if ( !( m.active() &&
            !m.s.active() && !m.s.a.active() && !m.s.b.active() &&
            m.x.active() && !m.y.active()
    ) )
      exitCode = 1;
    assert exitCode == 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
