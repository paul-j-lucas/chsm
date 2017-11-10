/*
**      CHSM Language System
**      src/common/chsm2xxx/list_comma.h
**
**      Copyright (C) 1996  Paul J. Lucas & Fabio Riccardi
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

#ifndef CHSM_list_comma_H
#define CHSM_list_comma_H

// standard
#include <iostream>

//*****************************************************************************
//
// SYNOPSIS
//
        class list_comma
//
// DESCRIPTION
//
//      A list_comma is a class that, when inserted into an ostream, doesn't
//      print a comma the first time around.  This is useful for emitting
//      comma-separated lists, e.g.:
//
//          ( A, B, C )
//
//*****************************************************************************
{
public:
    list_comma() : print_( false ) { }

    friend std::ostream& operator<<( std::ostream &o, list_comma &c ) {
        if ( c.print_ )
            o << ", ";
        else
            c.print();
        return o;
    }

    void reset()                        { print_ = false; }
    void print()                        { print_ = true; }
private:
    bool print_;
};

#endif  /* CHSM_list_comma_H */
/* vim:set et ts=4 sw=4: */
