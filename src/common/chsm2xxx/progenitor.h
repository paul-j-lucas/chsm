/*
**      PJL C++ Library
**      progenitor.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas
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

#ifndef progenitor_H
#define progenitor_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename Root> class progenitor
//
// DESCRIPTION
//
//      A user-defined base class derived from progenitor will be able to
//      construct objects of further-derived classes, i.e., the capability of
//      having a "virtual constructor."  The Root parameter should be the name
//      of the root class of the user's class hierarchy.
//
//      Two things are required of the derived classes:
//
//      (1) Define a copy constructor that also takes an additional "deep"
//          argument.  It, in turn, calls its base class's copy constructor.
//
//      (2) Use the PROCREATE macro without a trailing semicolon.
//
//*****************************************************************************
{
public:
    virtual ~progenitor() { }
    virtual Root* clone( bool deep = false ) const = 0;
};

#define PROCREATE( DERIVED )                        \
        DERIVED* clone( bool deep = false ) const { \
            return new DERIVED( *this, deep );      \
        }

} // namespace PJL

#endif  /* progenitor_H */
/* vim:set et ts=4 sw=4: */
