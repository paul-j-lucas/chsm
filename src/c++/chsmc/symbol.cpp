/*
**      PJL C++ Library
**      symbol.cpp
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

// local
#include "symbol.h"

// standard
#include <cassert>

using namespace std;

namespace PJL {

synfo::scope_type synfo::current_scope_;

///////////////////////////////////////////////////////////////////////////////

synfo::synfo( synfo const &from, bool ) :
  symbol_( from.symbol_ ),
  scope_( from.scope_ )
{
}

synfo::~synfo() {
  // out-of-line since it's virtual
}

ostream& synfo::emit( ostream &o ) const {
  if ( symbol_ != nullptr )
    o << '"' << symbol_->name() << '"';
  else
    o << "(no name)";
  o << " <";
  switch ( scope_ ) {
    case synfo::SCOPE_NONE:   o << "none";    break;
    case synfo::SCOPE_GLOBAL: o << "global";  break;
    default:                  o << scope_;
  } // switch
  return o << '>';
}

///////////////////////////////////////////////////////////////////////////////

symbol::~symbol() {
  // out-of-line since it's non-trivial
}

synfo* symbol::insert_info( synfo *new_si ) {
  assert( new_si != nullptr );
  new_si->symbol_ = this;
  if ( info_list_.empty() ) {
    //
    // We have to do this weirdness with the nullptr to prevent clone_ptr from
    // needlessly cloning itself via a copy construction.
    //
    info_list_.push_front( nullptr );
    info_list_.front() = new_si;
  }
  else {
    //
    // Walk along the list of synfo objects looking for the proper place to
    // insert it based on its scope.
    //
    info_list::iterator i;
    for ( i = info_list_.begin(); i != info_list_.end(); ++i ) {
      if ( new_si->scope() >= (*i)->scope() )
        break;
    } // for
    //
    // We have to do this weirdness with the nullptr to prevent clone_ptr from
    // needlessly cloning itself via a copy construction.
    //
    *info_list_.insert( i, nullptr ) = new_si;
  }
  return new_si;
}

void symbol::init( synfo *si ) {
  if ( si != nullptr ) {
    si->symbol_ = this;
    //
    // We have to do this weirdness with the nullptr to prevent clone_ptr from
    // needlessly cloning itself via a copy construction.
    //
    info_list_.push_front( nullptr );
    info_list_.front() = si;
  }
}

synfo* symbol::ith_info( scope_type i ) const {
  if ( i < info_list_.size() ) {
    info_list::const_iterator it = info_list_.begin();
    scope_type j = 0;
    while ( it != info_list_.end() ) {
      if ( i == j )
        return it->get();
      ++it, ++j;
    } // while
  }
  return nullptr;
}

void symbol_table::close_scope() {
  if ( scope() <= 1 )
    return;

  for ( iterator i = begin(); i != end(); ) {
    auto &sy = i->second;
    auto &info_list = sy.info_list_;

    while ( !info_list.empty() ) {
      if ( (*info_list.front()).in_current_scope() )
        sy.delete_info();
      else
        break;
    } // while

    if ( info_list.empty() ) {
      iterator j( i++ );
      erase( j );
    } else {
      ++i;
    }
  } // for

  --synfo::current_scope_;
}

ostream& operator<<( ostream &o, symbol const &sy ) {
  if ( sy.info_list_.empty() )
    return o << '"' << sy.name() << "\" (no info)";

  for ( auto const &si : sy.info_list_ )
    o << *si;
  return o;
}

ostream& operator<<( ostream &o, symbol_table const &table ) {
  for ( auto const &sti : table )
    o << sti.second << '\n';
  return o;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace

/* vim:set et ts=2 sw=2: */
