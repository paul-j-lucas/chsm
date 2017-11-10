/*
**      PJL C++ Library
**      serial_number.h
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

#ifndef serial_number_H
#define serial_number_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        class serial_number_base
//
// DESCRIPTION
//
//      The base class for serial_number<Derived> to provide access to the
//      serial number type independent of the template parameter.
//
//*****************************************************************************
{
public:
    typedef long value_type;
};

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename Derived>
        class serial_number : public serial_number_base
//
// DESCRIPTION
//
//      An object of a class derived from serial_number will be assigned a
//      unique serial-number.  Numbers start at zero.  
//
//      This class has to be a template so that a distinct static member is
//      generated for each derived type.
//
//*****************************************************************************
{
public:
    serial_number( serial_number<Derived> const& ) :
        serial_no_( next_no_++ ) { }

    value_type          serial() const  { return serial_no_; }
    static value_type   issued()        { return next_no_; }
protected:
    serial_number() : serial_no_( next_no_++ ) { }
private:
    static value_type   next_no_;
    value_type const    serial_no_;
};

} // namespace PJL

#endif  /* serial_number_H */
/* vim:set et ts=4 sw=4: */
