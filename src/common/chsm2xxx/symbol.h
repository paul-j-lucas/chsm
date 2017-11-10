/*
**      PJL C++ Library
**      symbol.h
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

#ifndef symbol_table_H
#define symbol_table_H

// standard
#include <iostream>
#include <list>
#include <string>

// local
#include "hash_table.h"
#include "pointer.h"
#include "progenitor.h"
#include "rtti.h"

namespace PJL {

class symbol;

//*****************************************************************************
//
// SYNOPSIS
//
        class synfo : public progenitor<synfo>, public RTTI
//
// DESCRIPTION
//
//      A synfo is a SYmbol iNFO object that contains information for a given
//      symbol for a particular scope.  One symbol can have several synfo
//      objects (one for every nested scope the symbol is declared in).
//
//*****************************************************************************
{
    static int current_scope_;
public:
    DECLARE_RTTI
    PROCREATE( synfo )

    enum {
        no_scope        = 0,
        global_scope    = 1
        // local scope  = n, n > 1
    };

    synfo( int s = current_scope_ ) : scope_( s ) { }
    synfo( synfo const&, bool = false );
    virtual ~synfo();

    int         scope() const           { return scope_; }
    static int  current_scope()         { return current_scope_; }
    bool        in_current_scope() const {
                    return scope_ == current_scope_;
                }

#   define PJL_RELOP(OP)                                            \
    friend bool operator OP( synfo const&, synfo const& );          \
    friend bool operator OP( synfo const&, char const* );           \
    friend bool operator OP( char const*, synfo const& );           \
    friend bool operator OP( synfo const&, std::string const& );    \
    friend bool operator OP( std::string const&, synfo const& );

    PJL_RELOP(==)
    PJL_RELOP(!=)
    PJL_RELOP(<)
    PJL_RELOP(>)
    PJL_RELOP(<=)
    PJL_RELOP(>=)

#   undef PJL_RELOP
protected:
    symbol const*   symbol_;            // our owning symbol
    int const       scope_;

    virtual std::ostream&   emit( std::ostream& ) const;
    friend std::ostream&    operator<<( std::ostream&, symbol const& );

    friend class symbol;                // so it can get at symbol_
    friend class symbol_table;          // so it can get at current_scope_
};

//*****************************************************************************
//
// SYNOPSIS
//
        class symbol
//
// DESCRIPTION
//
//      A symbol is an object representing a named identifier in a symbol
//      table.
//
//*****************************************************************************
{
public:
    symbol()                                    { }
    symbol( char const *name, synfo* = 0 );
    ~symbol();

    /**
     * Returns the given synfo.
     */
    synfo*          info( int in = 0 )          { return ith_info( in ); }
    synfo const*    info( int in = 0 ) const    { return ith_info( in ); }

    /**
     * Add a new synfo object to a symbol.  If there are existing synfo objects
     * for the symbol, the new one is inserted into the proper place according
     * to its scope.
     */
    synfo* info( synfo *new_synfo );

    /**
     * Deletes the first synfo.
     */
    void delete_info() {
        info_list_.pop_front();
    }

    /**
     * Checks whether a symbol at a given scope is of a particular type.
     */
    bool is_a( char const *type, int in = 0 ) const;

    /**
     * Returns the name of this symbol.
     */
    char const* name() const {
        return name_.c_str();
    }

    /**
     * Returns the scope of this symbol.
     */
    int scope() const {
        synfo const *const p = info();
        return p ? p->scope() : synfo::no_scope;
    }

    /**
     * Returns true only if this symbol is in the current scope.
     */
    bool in_current_scope() const {
        return scope() == synfo::current_scope();
    }

    /**
     * Converts this symbol to a C string of the symbol's name.
     */
    operator char const*() const {
        return name_.c_str();
    }

    // relational operators

#   define PJL_RELOP(OP)                                                \
    friend bool operator OP( symbol const &s, symbol const &t ) {       \
        return s.name_ OP t.name_;                                      \
    }                                                                   \
    friend bool operator OP( symbol const &s, char const *t ) {         \
        return s.name_ OP t;                                            \
    }                                                                   \
    friend bool operator OP( char const *s, symbol const &t ) {         \
        return s OP t.name_;                                            \
    }                                                                   \
    friend bool operator OP( symbol const &s, std::string const &t ) {  \
        return s.name_ OP t;                                            \
    }                                                                   \
    friend bool operator OP( std::string const &s, symbol const &t ) {  \
        return s OP t.name_;                                            \
    }

    PJL_RELOP(==)
    PJL_RELOP(!=)
    PJL_RELOP(<)
    PJL_RELOP(>)
    PJL_RELOP(<=)
    PJL_RELOP(>=)

#   undef   PJL_RELOP
private:
    typedef std::list< clone_ptr<synfo> > info_list;

    std::string name_;
    info_list   info_list_;

    /**
     * Accesses the ith scope synfo object of a symbol.
     */
    synfo* ith_info( int i ) const;

    friend class symbol_table;
    friend std::ostream& operator<<( std::ostream&, symbol const& );
};

//*****************************************************************************
//
// SYNOPSIS
//
        class symbol_table : public hash_table<symbol>
//
// DESCRIPTION
//
//      A symbol_table is-a hash_table<symbol> for maintaining a collection of
//      symbols during a compilation.  A symbol_table has a current scope.
//
//*****************************************************************************
{
public:
    symbol_table()                      { }

    static int      scope()             { return synfo::current_scope(); }
    void            open_scope ()       { ++synfo::current_scope_; }

    /**
     * Close a scope: desstroy all synfos at the scope being closed.
     */
    void close_scope();

    // inherited members

    symbol&         operator[]( char const *key );
    symbol&         operator[]( std::string const &key ) {
                        return operator[]( key.c_str() );
                    }
    symbol const&   operator[]( char const *key ) const {
                        return hash_table<symbol>::operator[]( key );
                    }
    symbol const&   operator[]( std::string const &key ) const {
                        return operator[]( key.c_str() );
                    }

    typedef hash_table<symbol>::iterator base_iterator;
    class iterator;
    friend class iterator;
    class iterator : public base_iterator {
    public:
        iterator() { }

        symbol& operator*() const {
            return base_iterator::operator*().value;
        }
        symbol* operator->() const {
            return &operator*();
        }

        iterator& operator++() {
            base_iterator::operator++();
            return *this;
        }
        iterator operator++(int) {
            iterator const temp = *this;
            ++*this;
            return temp;
        }
    protected:
        iterator( symbol_table *t, int in, bucket::iterator const &it ) :
            base_iterator( t, in, it ) { }

        friend class symbol_table;
    };

    iterator begin();
    iterator end();
    iterator find( key_type );

    typedef hash_table<symbol>::const_iterator base_const_iterator;
    class const_iterator;
    friend class const_iterator;
    class const_iterator : public base_const_iterator {
    public:
        const_iterator() { }

        symbol const& operator*() const {
            return base_const_iterator::operator*().value;
        }
        symbol const* operator->() const {
            return &operator*();
        }

        const_iterator& operator++() {
            base_const_iterator::operator++();
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator const temp = *this;
            ++*this;
            return temp;
        }
    protected:
        const_iterator( symbol_table *t, int in, bucket::iterator const &it ):
            base_const_iterator( t, in, it ) { }

        friend class symbol_table;
    };

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator find( key_type ) const;
};

std::ostream& operator<<( std::ostream&, symbol_table const& );

////////// Inlines ////////////////////////////////////////////////////////////

inline symbol_table::iterator symbol_table::begin() {
    return iterator( this, 0, buckets_[0].begin() );
}

inline symbol_table::const_iterator symbol_table::begin() const {
    symbol_table *const that = const_cast<symbol_table*>( this );
    return const_iterator( that, 0, that->buckets_[0].begin() );
}

inline symbol_table::iterator symbol_table::end() {
    return iterator(
        this, buckets_.size() - 1, buckets_[ buckets_.size() - 1 ].end()
    );
}

inline symbol_table::const_iterator symbol_table::end() const {
    symbol_table *const that = const_cast<symbol_table*>( this );
    return const_iterator(
        that, buckets_.size() - 1,
        that->buckets_[ buckets_.size() - 1 ].end()
    );
}

inline symbol_table::iterator symbol_table::find( key_type key ) {
    bucket::iterator it;
    return  hash_table_base::find( key, it ) ?
            iterator( this, 0, it ) : end();
}

inline symbol_table::const_iterator symbol_table::find( key_type key ) const {
    bucket::iterator it;
    symbol_table *const that = const_cast<symbol_table*>( this );
    return  that->hash_table_base::find( key, it ) ?
            const_iterator( that, 0, it ) : end();
}

#define PJL_RELOP(OP)                                               \
inline bool operator OP( synfo const &s, synfo const &t ) {         \
    return *s.symbol_ OP *t.symbol_;                                \
}                                                                   \
inline bool operator OP( synfo const &s, char const *t ) {          \
    return *s.symbol_ OP t;                                         \
}                                                                   \
inline bool operator OP( char const *s, synfo const &t ) {          \
    return s OP *t.symbol_;                                         \
}                                                                   \
inline bool operator OP( synfo const &s, std::string const &t ) {   \
    return *s.symbol_ OP t;                                         \
}                                                                   \
inline bool operator OP( std::string const &s, synfo const &t ) {   \
    return s OP *t.symbol_;                                         \
}

PJL_RELOP(==)
PJL_RELOP(!=)
PJL_RELOP(<)
PJL_RELOP(>)
PJL_RELOP(<=)
PJL_RELOP(>=)

#undef  PJL_RELOP

} // namespace

#endif  /* symbol_table_H */
/* vim:set et ts=4 sw=4: */
