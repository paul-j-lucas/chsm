/*
**      PJL C++ Library
**      hash_table.c
**
**      Copyright (C) 2002  Paul J. Lucas
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

// standard
#include <cstdlib>
#include <cstring>

// local
#include "hash_pjw.h"
#include "hash_table.h"

using namespace std;

namespace PJL {

static int const Initial_Buckets = 7;       // should be a prime number

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ hash_node_base::~hash_node_base()
//
// DESCRIPTION
//
//      Destructor: destroy a hash_node_base.  This is out of line because it's
//      virtual.
//
//*****************************************************************************
{
    // do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
        hash_table_base::hash_table_base( bool manage_strings ) :
//
// DESCRIPTION
//
//      Construct a hast_table_base.
//
// PARAMETERS
//
//      manage_strings  True if the hash_table should manage the strings for
//                      the keys by deleting them.
//
//*****************************************************************************
    buckets_( Initial_Buckets ), manage_strings_( manage_strings ),
    deletes_pending_( false ), node_count_( 0 ), num_iterators_( 0 )
{
    // do nothing else
}

//*****************************************************************************
//
// SYNOPSIS
//
        hash_table_base::hash_table_base( hash_table_base const &other ) :
//
// DESCRIPTION
//
//      Copy-contruct a hash_table_base.
//
//*****************************************************************************
    manage_strings_( other.manage_strings_ )
{
    copy( other );
}

//*****************************************************************************
//
// SYNOPSIS
//
        hash_table_base::~hash_table_base()
//
// DESCRIPTION
//
//      Destructor: destroy a hash_table_base.
//
//*****************************************************************************
{
    clear();
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::bump_iterator(
            int *bucket_index, bucket::iterator &it, bool prime
        )
//
// DESCRIPTION
//
//      Advance the position of the given iterator.
//
// PARAMETERS
//
//      bucket_index    A pointer to the bucket index.
//
//      it              The iterator to bump.
//
//      prime           If true, "prime" the iterator by simply finding the
//                      first non-empty bucket rather than advancing the
//                      iterator.
//
//*****************************************************************************
{
    if ( !prime && it != buckets_[ *bucket_index ].end() )
        ++it;
    while ( true ) {
        while ( it == buckets_[ *bucket_index ].end() ) {
            if ( ++*bucket_index >= buckets_.size() )
                return;
            it = buckets_[ *bucket_index ].begin();
        }
        if ( !(*it)->delete_mark_ )
            return;
        ++it;
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::check_add_node( hash_node_base *p )
//
// DESCRIPTION
//
//      Add a new node to the hash table growing the table if necessary.
//
// PARAMETERS
//
//      p   A pointer to a new hash_node_base to be added.
//
//*****************************************************************************
{
    buckets_[ last_hash_ ].push_front( p );
    if ( ++node_count_ > capacity() )
        grow_table();
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::check_delete_node(
            bucket &b, bucket::iterator const &n
        )
//
// DESCRIPTION
//
//      Delete a node from the hash table, but only if there are no active
//      iterators on it.  If there are, merely mark the node as deleted: it
//      will be deleted later once the number of active iterators goes to zero.
//
// PARAMETERS
//
//      b   The bucket the node to be deleted is in.
//
//      n   An iterator positioned at the node to be deleted.
//
//*****************************************************************************
{
    if ( num_iterators_ ) {
        // we have active iterators; must defer deletion
        (*n)->delete_mark_ = deletes_pending_ = true;
    } else
        delete_node( b, n );
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::clear()
//
// DESCRIPTION
//
//      Delete all the nodes from the hash table.
//
//*****************************************************************************
{
    buckets::iterator b;
    for ( b = buckets_.begin(); b != buckets_.end(); ++b ) {
        for ( bucket::iterator n = b->begin(); n != b->end(); ++n )
            delete_string( *n );
        b->clear();
    }
    deletes_pending_ = false;
    node_count_ = 0;
    num_iterators_ = 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::copy( hash_table_base const &other )
//
// DESCRIPTION
//
//      Copy an entire hash table.
//
// PARAMETERS
//
//      other   The table to copy.
//
//*****************************************************************************
{
    clear();
    buckets new_buckets( other.buckets_.size() );
    buckets::const_iterator i;
    int new_bucket_index = 0;

    for ( i = other.buckets_.begin(); i != other.buckets_.end(); ++i ) {
        bucket &new_bucket = new_buckets[ new_bucket_index++ ];
        for ( bucket::const_iterator j = i->begin(); j != i->end(); ++j )
            if ( !(*j)->delete_mark_ )
                new_bucket.push_front( (*j)->clone( do_string( (*j)->key ) ) );
    }

    buckets_.swap( new_buckets );
    node_count_ = other.node_count_;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::delete_node(
            bucket &b, bucket::iterator const &n
        )
//
// DESCRIPTION
//
//      Delete a node from the hash table.
//
// PARAMETERS
//
//      b   The bucket the node to be deleted is in.
//
//      n   An iterator positioned at the node to be deleted.
//
//*****************************************************************************
{
    delete_string( *n );
    b.erase( n );
}

//*****************************************************************************
//
// SYNOPSIS
//
        char const* hash_table_base::do_string( char const *key ) const
//
// DESCRIPTION
//
//      Copy the string only if managed; otherwise just return the string.
//
// PARAMETERS
//
//      key     The string to use.
//
// RETURN VALUE
//
//      If managed, returns a copy of the string; otherwise returns the
//      original string.
//
//*****************************************************************************
{
    return manage_strings_ ?
        ::strcpy( new char[ ::strlen( key ) + 1 ], key ) : key;
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool hash_table_base::erase( bucket &b, bucket::iterator const &n )
//
// DESCRIPTION
//
//      Erase a node from the hash table.
//
// PARAMETERS
//
//      b   The bucket the node to be deleted is in.
//
//      n   An iterator positioned at the node to be deleted.
//
//*****************************************************************************
{
    check_delete_node( b, n );
    --node_count_;
    return true;
}

//*****************************************************************************
//
// SYNOPSIS
//
        bool hash_table_base::find(
            key_type key, hash_table_base::bucket::iterator &i,
            bool include_deleted
        )
//
// DESCRIPTION
//
//      Find the node having the given key, if any.
//
// PARAMETERS
//
//      key                 The key to look for.
//
//      i                   The iterator to use.  If the key is found, this
//                          iterator is positioned at i.
//
//      include_deleted     Include deleted keys during search?
//
// RETURN VALUE
//
//      Returns true only if the key is found.
//
//*****************************************************************************
{
    last_hash_ = hash_pjw( key, buckets_.size() );
    bucket &b = buckets_[ last_hash_ ];

    for ( i = b.begin(); i != b.end(); ++i )
        if ( ::strcmp( (*i)->key, key ) == 0 ) {
            if ( (*i)->delete_mark_ )
                if ( include_deleted )
                    (*i)->delete_mark_ = false;     // undelete node
                else
                    continue;
            return true;
        }

    return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::grow_table()
//
// DESCRIPTION
//
//      Grow the hash table by nearly doubling the number of buckets.
//
//*****************************************************************************
{
    buckets new_buckets( (buckets_.size() + 1) * 2 + 1 );

    buckets::const_iterator i;

    for ( i = buckets_.begin(); i != buckets_.end(); ++i )
        for ( bucket::const_iterator j = i->begin(); j != i->end(); ++j ) {
            hash_value const v = hash_pjw( (*j)->key, new_buckets.size() );
            new_buckets[ v ].push_back( *j );
        }
    buckets_.swap( new_buckets );
}

//*****************************************************************************
//
// SYNOPSIS
//
        void hash_table_base::process_deletes()
//
// DESCRIPTION
//
//      Go through the entire table and delete all nodes marked for deletion.
//
//*****************************************************************************
{
    if ( !deletes_pending_ )
        return;
    for ( buckets::iterator i = buckets_.begin(); i != buckets_.end(); ++i )
        for ( bucket::iterator j = i->begin(); j != i->end(); ++j )
            if ( (*j)->delete_mark_ )
                delete_node( *i, j );
    deletes_pending_ = false;
}

} // namespace

//*****************************************************************************

/*#define TEST_HASH_TABLE /**/
#ifdef  TEST_HASH_TABLE

#include <iostream>

using namespace PJL;
using namespace std;

int const allspice  = 20;
int const basil     =  1;
int const butter    = 22;
int const cinnamon  = 18;
int const clove     = 16;
int const cumin     =  2;
int const dill      =  3;
int const fennel    = 17;
int const garlic    = 19;
int const ginger    =  4;
int const mustard   =  5;
int const nutmeg    =  6;
int const onion     = 21;
int const oregano   =  7;
int const paprika   =  8;
int const parsley   =  9;
int const pepper    = 10;
int const rosemary  = 11;
int const saffron   = 12;
int const sage      = 13;
int const salt      = 14;
int const thyme     = 15;

int main( int argc, char *argv[] ) {
    hash_table<int> h( false );

    h[ "allspice" ] = allspice;
    h[ "basil"    ] = basil;
    h[ "butter"   ] = butter;
    h[ "cinnamon" ] = cinnamon;
    h[ "clove"    ] = clove;
    h[ "cumin"    ] = cumin;
    h[ "dill"     ] = dill;
    h[ "fennel"   ] = fennel;
    h[ "garlic"   ] = garlic;
    h[ "ginger"   ] = ginger;
    h[ "mustard"  ] = mustard;
    h[ "nutmeg"   ] = nutmeg;
    h[ "onion"    ] = onion;
    h[ "oregano"  ] = oregano;
    h[ "paprika"  ] = paprika;
    h[ "parsley"  ] = parsley;
    h[ "pepper"   ] = pepper;
    h[ "rosemary" ] = rosemary;
    h[ "saffron"  ] = saffron;
    h[ "sage"     ] = sage;
    h[ "salt"     ] = salt;
    h[ "thyme"    ] = thyme;

    cout << h[ "basil" ] << endl;
    cout << h[ "cumin" ] << endl;
    cout << h[ "dill"  ] << endl;

    cout << "----- iterator -----" << endl;
    for ( hash_table<int>::iterator i = h.begin(); i != h.end(); ++i ) {
        cout << i->key << '=' << flush;
        cout << i->value << '\n';
    }

    cout << "----- const_iterator -----" << endl;
    hash_table<int> const &hc = h;
    hash_table<int>::const_iterator i;
    for ( i = hc.begin(); i != hc.end(); ++i ) {
        cout << i->key << '=' << flush;
        cout << i->value << '\n';
    }

    hash_table<int>::iterator it = h.find( "dill" );

    h.clear();
    cout << "----- DONE -----" << endl;
}

#endif  /* TEST_HASH_TABLE */
/* vim:set et ts=4 sw=4: */
