/*
**      PJL C++ Library
**      rtti.h
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

#ifndef RTTI_H
#define RTTI_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        struct RTTI
//
//      An RTTI is an object that adds the ability for a derived object to be
//      type-identified at run-time.
//
//      Two things are required of the derived classes:
//
//      (1) In the public section, say DECLARE_RTTI.
//
//      (2) In the definition, say DEFINE_RTTI( Class, BaseClass ) where Class
//          is the name of the class and BaseClass is the base class.
//
//*****************************************************************************
{
    virtual bool is_a( char const* ) const { return false; }
    virtual ~RTTI() { }
};

} // namespace PJL

#define DECLARE_RTTI                                            \
        static char const *const Class;                         \
        bool is_a( char const* ) const;

#define DEFINE_RTTI( CLASS, BASECLASS )                         \
        char const *const CLASS::Class = #CLASS;                \
        bool CLASS::is_a( char const *name ) const {            \
            return Class == name || BASECLASS::is_a( name );    \
        }

#endif  /* RTTI_H */
/* vim:set et ts=4 sw=4: */
