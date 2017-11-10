/*
**      CHSM Language System
**      src/java/tests/Test.java
**
**      Copyright (C) 2004-2010  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/**
 * <code>Test</code> is a small class useful for writing test cases.
 * @author Paul J. Lucas
 */
class Test extends CHSM.Machine {
    static void printResult() {
        System.out.println( exitCode == 0 ? "correct" : "INCORRECT" );
        System.exit( exitCode );
    }

    static int exitCode;
}
// vim:set et sw=4 ts=4 syntax=java:
