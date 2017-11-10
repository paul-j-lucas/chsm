/*
**      PJL C++ Library
**      symbol.c
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

// local
#include "symbol.h"

using namespace std;

namespace PJL {

int synfo::current_scope_;

DEFINE_RTTI(synfo,RTTI)

///////////////////////////////////////////////////////////////////////////////

synfo::synfo( synfo const &other, bool ) :
    scope_( other.scope_ ), symbol_( other.symbol_ )
{
}

///////////////////////////////////////////////////////////////////////////////

synfo::~synfo() {
}

///////////////////////////////////////////////////////////////////////////////

ostream& synfo::emit( ostream &out ) const {
    if ( symbol_ )
        out << '"' << symbol_->name() << '"';
    else
        out << "(no name)";
    out << " <";
    switch ( scope_ ) {
        case synfo::no_scope:       out << "no scope";  break;
        case synfo::global_scope:   out << "global";    break;
        default:                    out << scope_;
    }
    return out << '>';
}

///////////////////////////////////////////////////////////////////////////////

symbol::symbol( char const *name, synfo *p ) : name_( name ) {
    if ( p ) {
        p->symbol_ = this;
        //
        // We have to do this weirdness with the 0 to prevent clone_ptr from
        // needlessly cloning itself via a copy construction.
        //
        info_list_.push_front( 0 );
        info_list_.front() = p;
    }
}

///////////////////////////////////////////////////////////////////////////////

symbol::~symbol() {
}

///////////////////////////////////////////////////////////////////////////////

synfo* symbol::info( register synfo *new_synfo ) {
    new_synfo->symbol_ = this;
    if ( info_list_.empty() ) {
        //
        // We have to do this weirdness with the 0 to prevent clone_ptr from
        // needlessly cloning itself via a copy construction.
        //
        info_list_.push_front( 0 );
        info_list_.front() = new_synfo;
    } else {
        //
        // Walk along the list of synfo objects looking for the proper place to
        // insert it based on its scope.
        //
        info_list::iterator i;
        for ( i = info_list_.begin(); i != info_list_.end(); ++i )
            if ( new_synfo->scope() >= (**i).scope() )
                break;
        //
        // We have to do this weirdness with the 0 to prevent clone_ptr from
        // needlessly cloning itself via a copy construction.
        //
        *info_list_.insert( i, 0 ) = new_synfo;
    }
    return new_synfo;
}

///////////////////////////////////////////////////////////////////////////////

bool symbol::is_a( char const *type, int in ) const {
    synfo const *const p = info( in );
    return p ? p->is_a( type ) : false;
}

///////////////////////////////////////////////////////////////////////////////

synfo* symbol::ith_info( int i ) const {
    if ( i >= 0 && i < info_list_.size() ) {
        info_list::const_iterator it = info_list_.begin();
        int j = 0;
        while ( it != info_list_.end() )
            if ( i == j )
                return *it;
            else
                ++it, ++j;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

symbol& symbol_table::operator[]( char const *key ) {
    symbol &sym = hash_table<symbol>::operator[]( key );
    if ( sym.name_.empty() )
        sym.name_ = key;
    return sym;
}

///////////////////////////////////////////////////////////////////////////////

void symbol_table::close_scope() {
    if ( scope() <= 1 )
        return;

    iterator i = begin();
    while ( i != end() ) {
        symbol::info_list &info_list = i->info_list_;

        while ( !info_list.empty() )
            if ( (*info_list.front()).in_current_scope() )
                i->delete_info();
            else
                break;

        if ( info_list.empty() ) {
            iterator j = i++;
            erase( j );
        } else
            ++i;
    }

    --synfo::current_scope_;
}

///////////////////////////////////////////////////////////////////////////////

ostream& operator<<( ostream &out, symbol const &sym ) {
    if ( sym.info_list_.empty() )
        return out << '"' << sym.name() << "\" (no info)";

    int n = 0;
    for ( symbol::info_list::const_iterator
        i = sym.info_list_.begin(); i != sym.info_list_.end(); ++i )
    {
        out << n << ": ";
        (*i)->emit( out );
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////////

ostream& operator<<( ostream &out, symbol_table const &table ) {
    for ( symbol_table::const_iterator
            i = table.begin(); i != table.end(); ++i )
        out << *i << '\n';
    return out;
}

} // namespace
/* vim:set et ts=4 sw=4: */
