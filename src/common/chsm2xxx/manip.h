/*
**      CHSM Language System
**      src/common/chsm2xxx/manip.h
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

#ifndef CHSM_info_manip_H
#define CHSM_info_manip_H

// standard
#include <iostream>

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class info_manip
//
// DESCRIPTION
//
//      An info_manip<T> is a manipulator making it convenient to insert
//      functions into an ostream.  The type T is one of the *_info classes.
//      The global functions below end up calling the virtual functions of the
//      similar name for the objects.
//
//*****************************************************************************
{
public:
    typedef std::ostream& (T::*function)( std::ostream&, unsigned ) const;

    info_manip( function f, T const *p, unsigned flags = 0 ) :
        function_( f ), info_( p ), flags_( flags ) { }

    friend
    std::ostream& operator<<( std::ostream &o, info_manip<T> const &im ) {
        //
        // The act of inserting an info_manip into an ostream calls the emitter
        // member function for the info object wrapped by the info_manip
        // object.
        //
        return (im.info_->*im.function_)( o, im.flags_ );
    }
private:
    function const function_;
    T const *info_;
    unsigned const flags_;
};

//*****************************************************************************
//  Manipulator function(s)
//*****************************************************************************

template<typename T> inline info_manip<T> declaration( T const *p ) {
    return info_manip<T>( &T::emit_declaration, p );
}
template<typename T> inline info_manip<T> definition( T const *p ) {
    return info_manip<T>( &T::emit_definition, p );
}
template<typename T> inline info_manip<T> initializer( T const *p ) {
    return info_manip<T>( &T::emit_initializer, p );
}

#endif  /* CHSM_info_manip_H */
/* vim:set et ts=4 sw=4: */
