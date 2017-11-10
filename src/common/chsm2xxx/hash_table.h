/*
**      PJL C++ Library
**      hash_table.h
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

#ifndef hash_table_H
#define hash_table_H

// standard
#include <climits>
#include <iterator>
#include <iostream>
#include <list>
#include <string>
#include <sys/types.h>
#include <vector>

// local
#include "hash_pjw.h"

namespace PJL {

template<typename V> class hash_table;

//*****************************************************************************
//
// SYNOPSIS
//
        class hash_node_base
//
// DESCRIPTION
//
//      A hash_node_base is the base class for hash_node<V> to have a common
//      base type independent of the type V that can be manipulated by
//      hash_table_base.
//
//*****************************************************************************
{
public:
    typedef char const* key_type;

    key_type const  key;
protected:
    bool            delete_mark_;

    hash_node_base( key_type k ) : key( k ) { delete_mark_ = false; }
    virtual ~hash_node_base();

    virtual hash_node_base* clone( key_type ) const = 0;

    friend class hash_table_base;
};

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename V> class hash_node : public hash_node_base
//
// DESCRIPTION
//
//      A hash_node is-a hash_node_base, but parameterized with the type of
//      the value hashed to.
//
//*****************************************************************************
{
public:
    typedef V value_type;
    typedef V* pointer;
    typedef V const* const_pointer;
    typedef V& reference;
    typedef V const& const_reference;

    value_type value;
protected:
    hash_node( key_type k, const_reference v ) :
        hash_node_base( k ), value( v ) { }
    ~hash_node() { }                    // non-public so user can't delete it

    hash_node_base* clone( key_type k ) const {
        return new hash_node<V>( k, value );
    }

    friend class hash_table_base;
    friend class hash_table<V>;
};

//*****************************************************************************
//
// SYNOPSIS
//
        class hash_table_base
//
// DESCRIPTION
//
//      A hash_table_base is the base class for hash_table<V> in order to have
//      a lot of the code intependent of the type V.
//
//*****************************************************************************
{
public:
    typedef hash_node_base::key_type key_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    size_type   capacity() const;
    void        clear();
    bool        empty() const           { return size() == 0; }
    size_type   max_size() const        { return INT_MAX; }
    size_type   size() const            { return node_count_; }
protected:
    typedef std::list<hash_node_base*> bucket;
    typedef std::vector<bucket> buckets;

    buckets     buckets_;
    bool const  manage_strings_;

    hash_table_base( bool manage_strings );
    ~hash_table_base();

    hash_table_base( hash_table_base const& );

    void bump_iterator( int*, bucket::iterator&, bool = false );
    void check_add_node( hash_node_base* );
    void check_delete_node( bucket&, bucket::iterator const& );
    void copy( hash_table_base const& );
    void delete_node( bucket&, bucket::iterator const& );
    bool erase( bucket&, bucket::iterator const& );
    bool erase( key_type );
    bool find( key_type, bucket::iterator&, bool = false );
private:
    enum { GrowFactor = 3 };

    bool                deletes_pending_;
    mutable hash_value  last_hash_;
    size_type           node_count_;
    int                 num_iterators_;

    void                delete_string( hash_node_base* ) const;
    char const*         do_string( char const* ) const;
    void                grow_table();
    void                process_deletes();

    template<typename V> friend class hash_table;
public:
    // This is for a horrible hack -- don't call these yourself!
    void inc_iterators() {
        ++num_iterators_;
    }
    void dec_iterators() {
        if ( num_iterators_ > 0 && !--num_iterators_ )
            process_deletes();
    }
};

////////// Inlines ////////////////////////////////////////////////////////////

inline hash_table_base::size_type hash_table_base::capacity() const {
    return buckets_.size() * GrowFactor;
}

inline void hash_table_base::delete_string( hash_node_base *node ) const {
    if ( manage_strings_ )
        delete[] const_cast<char*>( node->key );
}

inline bool hash_table_base::erase( key_type key ) {
    bucket::iterator it;
    return find( key, it ) ? erase( buckets_[ last_hash_ ], it ) : false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename V> class hash_table : public hash_table_base
//
// DESCRIPTION
//
//      A hash_table is an O(1) mapping from a string to another object.
//
//*****************************************************************************
{
public:
    typedef typename hash_node<V>::value_type value_type;
    typedef typename hash_node<V>::pointer pointer;
    typedef typename hash_node<V>::const_pointer const_pointer;
    typedef typename hash_node<V>::reference reference;
    typedef typename hash_node<V>::const_reference const_reference;
protected:
    static const_pointer get_default_value() {
        static value_type the_default_value;
        return &the_default_value;
    }
public:
    // constructor, destructor

    hash_table( bool manage_strings = true,
        const_reference default_value = *get_default_value()
    );
    ~hash_table() { }

    // copy, assign

    hash_table( hash_table<V> const& );
    hash_table<V>& operator=( hash_table<V> const &other ) {
        if ( &other != this ) copy( other );
        return *this;
    }

    // element access

    reference       operator[]( key_type );
    reference       operator[]( std::string const &key ) {
                        return operator[]( key.c_str() );
                    }
    const_reference operator[]( key_type ) const;
    const_reference operator[]( std::string const &key ) const {
                        return operator[]( key.c_str() );
                    }

    ///////////////////////////////////////////////////////////////////////////

    class iterator;
    friend class iterator;
    class iterator :
        public std::iterator<std::forward_iterator_tag,value_type> {
    public:
        iterator() : table_( 0 ) { }
        iterator( iterator const &i ) :
            table_( i.table_ ), bucket_index_( i.bucket_index_ ),
            bucket_iter_( i.bucket_iter_ )
        {
            if ( table_ )
                table_->inc_iterators();
        }
        ~iterator() {
            if ( table_ )
                table_->dec_iterators();
        }
        iterator& operator=( iterator const &i ) {
            if ( table_ )
                table_->dec_iterators();
            if ( table_ = i.table_ )
                table_->inc_iterators();
            bucket_index_ = i.bucket_index_;
            bucket_iter_ = i.bucket_iter_;
            return *this;
        }

        hash_node<V>& operator*() const {
            return *static_cast<hash_node<V>*>( *bucket_iter_ );
        }
        hash_node<V>* operator->() const {
            return static_cast<hash_node<V>*>( *bucket_iter_ );
        }

        iterator& operator++() {
            table_->bump_iterator( &bucket_index_, bucket_iter_ );
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool operator==( iterator const &i, iterator const &j ) {
            return i.bucket_iter_ == j.bucket_iter_;
        }
        friend bool operator!=( iterator const &i, iterator const &j ) {
            return i.bucket_iter_ != j.bucket_iter_;
        }
    protected:
        hash_table<V>*      table_;
        int                 bucket_index_;
        bucket::iterator    bucket_iter_;
        iterator( hash_table<V> *t, int in, bucket::iterator const &it ) :
            table_( t ), bucket_index_( in ), bucket_iter_( it )
        {
            if ( table_ ) {
                table_->bump_iterator( &bucket_index_, bucket_iter_, true );
                table_->inc_iterators();
            }
        }
        friend class hash_table<V>;
    };

    iterator    begin();
    iterator    end();
    bool        erase( key_type );
    void        erase( iterator const& );
    iterator    find( key_type );

    ///////////////////////////////////////////////////////////////////////////

    class const_iterator;
    friend class const_iterator;
    class const_iterator :
        public std::iterator<std::forward_iterator_tag,value_type> {
    public:
        const_iterator() : table_( 0 ) { }

        hash_node<V> const& operator*() const {
            return *static_cast<hash_node<V> const*>( *bucket_iter_ );
        }
        hash_node<V> const* operator->() const {
            return static_cast<hash_node<V> const*>( *bucket_iter_ );
        }

        const_iterator& operator++() {
            table_->bump_iterator( &bucket_index_, bucket_iter_ );
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool
        operator==( const_iterator const &i, const_iterator const &j ) {
            return i.bucket_iter_ == j.bucket_iter_;
        }

        friend bool
        operator!=( const_iterator const &i, const_iterator const &j ) {
            return i.bucket_iter_ != j.bucket_iter_;
        }
    protected:
        hash_table<V>       *table_;
        int                 bucket_index_;
        bucket::iterator    bucket_iter_;
        const_iterator( hash_table<V> *t, int in, bucket::iterator const &it ) :
            table_( t ), bucket_index_( in ), bucket_iter_( it )
        {
            if ( table_ )
                table_->bump_iterator( &bucket_index_, bucket_iter_, true );
        }

        friend class hash_table<V>;
    };

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator find( key_type key ) const;
private:
    value_type const local_default_value_;
};

////////// Inlines ////////////////////////////////////////////////////////////

template<typename V>
hash_table<V>::hash_table( bool manage_strings, const_reference default_value )
    : hash_table_base( manage_strings ),
      local_default_value_( default_value )
{
}

template<typename V>
hash_table<V>::hash_table( hash_table<V> const &other )
    : hash_table_base( other ),
      local_default_value_( other.local_default_value_ )
{
}

template<typename V>
typename hash_table<V>::reference hash_table<V>::operator[]( key_type key ) {
    bucket::iterator it;
    if ( hash_table_base::find( key, it, true ) )
        return static_cast<hash_node<V>* >(*it)->value;
    hash_node<V> *const node = new hash_node<V>(
        do_string( key ), local_default_value_
    );
    check_add_node( node );
    return node->value;
}

template<typename V> inline
typename hash_table<V>::const_reference
hash_table<V>::operator[]( key_type key ) const {
    hash_table<V> *const that = const_cast<hash_table<V>* >( this );
    bucket::iterator it;
    return  that->hash_table_base::find( key, it ) ?
        static_cast<hash_node<V> const*>(*it)->value :
        local_default_value_;
}

template<typename V> inline
typename hash_table<V>::iterator hash_table<V>::begin() {
    return iterator( this, 0, buckets_[0].begin() );
}

template<typename V> inline
typename hash_table<V>::const_iterator hash_table<V>::begin() const {
    hash_table<V> *const that = const_cast<hash_table<V>*>( this );
    return const_iterator( that, 0, that->buckets_[0].begin() );
}

template<typename V> inline
typename hash_table<V>::iterator hash_table<V>::end() {
    return iterator(
        this, buckets_.size() - 1, buckets_[ buckets_.size() - 1 ].end()
    );
}

template<typename V> inline
typename hash_table<V>::const_iterator hash_table<V>::end() const {
    hash_table<V> *const that = const_cast<hash_table<V>*>( this );
    return const_iterator(
        that, buckets_.size() - 1,
        that->buckets_[ buckets_.size() - 1 ].end()
    );
}

template<typename V> inline
bool hash_table<V>::erase( key_type key ) {
    bucket::iterator it;
    return  hash_table_base::find( key, it ) ?
        hash_table_base::erase( it ) : false;
}

template<typename V> inline
void hash_table<V>::erase( iterator const &i ) {
    i.table_->buckets_[ i.bucket_index_ ].erase( i.bucket_iter_ );
}

template<typename V> inline
typename hash_table<V>::iterator hash_table<V>::find( key_type key ) {
    bucket::iterator it;
    return  hash_table_base::find( key, it ) ?
        iterator( this, 0, it ) : end();
}

template<typename V> inline
typename hash_table<V>::const_iterator
hash_table<V>::find( key_type key ) const {
    bucket::iterator it;
    return  hash_table_base::find( key, it ) ?
        const_iterator( this, 0, it ) : end();
}

} // namespace

#endif  /* hash_table_H */
/* vim:set et ts=4 sw=4: */
