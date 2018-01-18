/*
**      PJL C++ Library
**      indent.cpp
**
**      Copyright (C) 2018  Paul J. Lucas
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

// local
#include "indent.h"

using namespace std;

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

void ostream_indent::callback( ios_base::event event, ios_base &os,
                               int index ) {
  void *&p = os.pword( index );
  switch ( event ) {
    case ios_base::copyfmt_event:
      p = new ostream_indent( *static_cast<ostream_indent*>( p ) );
      break;
    case ios_base::erase_event:
      delete static_cast<ostream_indent*>( p );
      p = nullptr;
      break;
    case ios_base::imbue_event:
      break;
  } // switch
}

ostream_indent* ostream_indent::instance( std::ios_base &os ) {
  static int const index = ios_base::xalloc();
  void *&p = os.pword( index );
  if ( p == nullptr ) {
    p = new ostream_indent;
    os.register_callback( callback, index );
  }
  return static_cast<ostream_indent*>( p );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace

/* vim:set et sw=2 ts=2: */
