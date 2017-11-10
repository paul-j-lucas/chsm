/*
**      CHSM Language System
**      src/c++/lib/util.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
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

#ifndef chsm_util_H
#define chsm_util_H

// standard
#include <iostream>

namespace CHSM_NS {

    extern int      debug_indent;

    std::ostream&   echo();

#   define ECHO     echo()
#   define MORE     std::cerr
#   define ENDL     << std::endl

} // namespace

#endif  /* chsm_util_H */
/* vim:set et sw=4 ts=4: */
