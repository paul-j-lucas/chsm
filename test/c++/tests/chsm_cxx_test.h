/*
**      CHSM Language System
**      test/c++/tests/chsm_cxx_test.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas & Fabio Riccardi
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 3 of the License, or
**      (at your option) any later version.
** 
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
** 
**      You should have received a copy of the GNU General Public License
**      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef chsm_cxx_test_H
#define chsm_cxx_test_H

///////////////////////////////////////////////////////////////////////////////

#define BLOCK(...)                do { __VA_ARGS__ } while (0)
#define CHSM_TEST(EXPR)           BLOCK( if ( !(EXPR) ) exit_code = 1; );

#define PRINT_RESULT() BLOCK(                         \
  cout << (exit_code == 0 ? "PASS" : "FAIL") << endl; \
  return exit_code; )

///////////////////////////////////////////////////////////////////////////////

#endif /* chsm_cxx_test_H */
// vim:set et sw=2 ts=2:
