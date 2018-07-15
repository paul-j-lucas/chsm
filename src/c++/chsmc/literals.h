/*
**      CHSM Language System
**      src/c++/chsmc/literals.h
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

#ifndef chsmc_literals_H
#define chsmc_literals_H

/**
 * @file
 * Declares constants for CHSM, C++, and Java language literals.
 */

#ifdef L_SET
// Some platforms #define L_SET as a synonym for SEEK_SET.
#undef L_SET
#endif /* L_SET */

///////////////////////////////////////////////////////////////////////////////

// CHSM
extern char const L_CHSM[];
extern char const L_CLUSTER[];
extern char const L_DEEP[];
extern char const L_ENTER[];
extern char const L_EVENT[];
extern char const L_EXIT[];
extern char const L_FINAL[];
extern char const L_HISTORY[];
extern char const L_IN[];
extern char const L_IS[];
extern char const L_PARAM[];
extern char const L_SET[];
extern char const L_STATE[];
extern char const L_UPON[];

// Java
extern char const L_PUBLIC[];

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_literals_H */
/* vim:set et sw=2 ts=2: */
