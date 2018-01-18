/*
**      PJL C++ Library
**      serial_number.h
**
**      Copyright (C) 1996-2018  Paul J. Lucas
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

#ifndef pjl_serial_number_H
#define pjl_serial_number_H

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * The base class for serial_number<DerivedClass> to provide access to the
 * serial number type independent of the template parameter.
 */
struct serial_number_base {
  typedef long value_type;
};

/**
 * An object of a class derived from serial_number will be assigned a unique
 * serial-number.  Numbers start at zero.
 *
 * This class has to be a template so that a distinct static member is
 * generated for each derived type.
 *
 * @tparam DerivedClass The derived type that is to have a serial number.
 */
template<class DerivedClass>
class serial_number : public serial_number_base {
public:
  serial_number( serial_number<DerivedClass> const& ) :
    serial_no_( next_no_++ )
  {
  }

  value_type serial() const {
    return serial_no_;
  }

protected:
  serial_number() : serial_no_( next_no_++ ) { }

private:
  static value_type next_no_;
  value_type const serial_no_;
};

template<typename DerivedClass>
serial_number_base::value_type serial_number<DerivedClass>::next_no_ = 0;

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* pjl_serial_number_H */
/* vim:set et ts=2 sw=2: */
