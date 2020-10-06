/*
**      PJL C++ Library
**      symbol.h
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

#ifndef pjl_symbol_table_H
#define pjl_symbol_table_H

// local
#include "clone_ptr.h"
#include "progenitor.h"

// standard
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

class symbol;

/**
 * A %synfo is a SYmbol iNFO object that contains information for a given
 * symbol for a particular scope.  One symbol can have several synfo objects
 * (one for every nested scope the symbol is declared in).
 */
class synfo : public progenitor<synfo> {
public:
  PROCREATE( synfo );

  typedef unsigned scope_type;

  static scope_type const SCOPE_NONE    = 0;
  static scope_type const SCOPE_GLOBAL  = 1;
  // local scopes are > 1

  /**
   * Constructs a %synfo.
   *
   * @param s The scope level.  Defaults to the current scope.
   */
  explicit synfo( scope_type s = current_scope_ ) :
    symbol_( nullptr ),
    scope_( s )
  {
  }

  /**
   * Copy constructs a %synfo.
   *
   * @param from The %synfo to copy.
   * @param deep If `true`, do a deep copy.
   */
  synfo( synfo const &from, bool deep = false );

  synfo( synfo&& ) = default;

  /**
   * Destroys a %synfo.
   */
  virtual ~synfo();

  synfo& operator=( synfo const& ) = delete;

  /**
   * Gets the symbol to which this %synfo belongs.
   *
   * @return Returns said symbol, if any.
   */
  symbol const* get_symbol() const {
    return symbol_;
  }

  /**
   * Gets the name of the symbol to thich this %synfo belongs.
   *
   * @return Returns said name.
   */
  char const* name() const;

  /**
   * Gets the scope level of this %synfo.
   *
   * @return Returns said scope.
   */
  scope_type scope() const {
    return scope_;
  }

  static scope_type current_scope() {
    return current_scope_;
  }

  /**
   * Gets whether this %synfo is in the current scope.
   *
   * @return Returns `true` only if it is.
   */
  bool in_current_scope() const {
    return scope_ == current_scope_;
  }

# define PJL_RELOP(OP)                                          \
  friend bool operator OP( synfo const&, synfo const& );        \
  friend bool operator OP( synfo const&, char const* );         \
  friend bool operator OP( char const*, synfo const& );         \
  friend bool operator OP( synfo const&, std::string const& );  \
  friend bool operator OP( std::string const&, synfo const& );

  PJL_RELOP(==)
  PJL_RELOP(!=)
  PJL_RELOP(< )
  PJL_RELOP(> )
  PJL_RELOP(<=)
  PJL_RELOP(>=)
# undef PJL_RELOP

  friend std::ostream& operator<<( std::ostream &o, synfo const &si ) {
    return si.emit( o );
  }

protected:
  symbol const     *symbol_;            // our owning symbol
  scope_type const  scope_;
  static scope_type current_scope_;

  virtual std::ostream& emit( std::ostream &o ) const;

  friend class symbol;                // so it can get at symbol_
  friend class symbol_table;          // so it can get at current_scope_
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %symbol is an object representing a named identifier in a %symbol table.
 */
class symbol {
public:
  typedef synfo::scope_type scope_type;

  /**
   * Default constructs a %symbol.
   */
  symbol() {
  }

  /**
   * Constructs a %symbol.
   *
   * @param name The %symbol name.
   * @param si An initial synfo, if any.
   */
  symbol( std::string const &name, synfo *si = nullptr ) : name_( name ) {
    init( si );
  }

  /**
   * Constructs a %symbol.
   *
   * @param name The %symbol name.
   * @param si An initial synfo, if any.
   */
  symbol( char const *name, synfo *si = nullptr ) : name_( name ) {
    init( si );
  }

  symbol( symbol&& ) = default;
  symbol& operator=( symbol&& ) = default;

  /**
   * Destroys a %symbol.
   */
  ~symbol();

  /**
   * Returns the given synfo.
   *
   * @param in How many scopes "in" to go.  Defaults to current scope.
   */
  synfo* info( scope_type in = 0 ) {
    return ith_info( in );
  }

  /**
   * Returns the given synfo.
   *
   * @param in How many scopes "in" to go.  Defaults to current scope.
   */
  synfo const* info( scope_type in = 0 ) const {
    return ith_info( in );
  }

  /**
   * Adds a new synfo object to a symbol.  If there are existing synfo objects
   * for the symbol, the new one is inserted into the proper place according to
   * its scope.
   *
   * @param new_synfo The synfo to insert.
   * @return Returns \a new_synfo.
   */
  synfo* insert_info( synfo *new_synfo );

  /**
   * Deletes the first synfo.
   */
  void delete_info() {
    info_list_.pop_front();
  }

  /**
   * Gets the name of this symbol.
   *
   * @return Returns said name.
   */
  char const* name() const {
    return name_.c_str();
  }

  /**
   * Returns the scope of this symbol.
   */
  scope_type scope() const {
    synfo const *const s = info();
    return s != nullptr ? s->scope() : synfo::SCOPE_NONE;
  }

  /**
   * Returns true only if this symbol is in the current scope.
   */
  bool in_current_scope() const {
    return scope() == synfo::current_scope();
  }

  // relational operators

# define PJL_RELOP(OP)                                                \
  friend bool operator OP( symbol const &s, symbol const &t ) {       \
    return s.name_ OP t.name_;                                        \
  }                                                                   \
  friend bool operator OP( symbol const &s, char const *t ) {         \
    return s.name_ OP t;                                              \
  }                                                                   \
  friend bool operator OP( char const *s, symbol const &t ) {         \
    return s OP t.name_;                                              \
  }                                                                   \
  friend bool operator OP( symbol const &s, std::string const &t ) {  \
    return s.name_ OP t;                                              \
  }                                                                   \
  friend bool operator OP( std::string const &s, symbol const &t ) {  \
    return s OP t.name_;                                              \
  }

  PJL_RELOP(==)
  PJL_RELOP(!=)
  PJL_RELOP(< )
  PJL_RELOP(> )
  PJL_RELOP(<=)
  PJL_RELOP(>=)
# undef PJL_RELOP

private:
  typedef std::list<clone_ptr<synfo>> info_list;

  std::string name_;
  info_list   info_list_;

  void init( synfo *si );

  /**
   * Accesses the ith scope synfo object of a symbol.
   *
   * @param i The ith scope to access.
   * @return Returns the synfo at scope \a i or nullptr if none.
   */
  synfo* ith_info( scope_type i ) const;

  friend class symbol_table;
  friend std::ostream& operator<<( std::ostream&, symbol const& );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A %symbol_table maintains a collection of symbols during a compilation.  A
 * %symbol_table has a current scope.
 */
class symbol_table : public std::unordered_map<std::string,symbol> {
  typedef std::unordered_map<std::string,symbol> base_type;
public:
  typedef symbol::scope_type scope_type;

  mapped_type& operator[]( key_type const &key ) {
    auto const &rv = emplace( key, key );
    return rv.first->second;
  }

  /**
   * Opens a new scope.
   */
  void open_scope() {
    ++synfo::current_scope_;
  }

  /**
   * Closes a scope: destroy all synfos at the scope being closed.
   */
  void close_scope();

  /**
   * Gets the current scope.
   */
  static scope_type scope() {
    return synfo::current_scope();
  }
};

////////// friends ////////////////////////////////////////////////////////////

/**
 * Emits a symbol_table to an ostream.
 *
 * @param o The ostream to emit to.
 * @param s The symbol_table to emit.
 * @return Returns \a o.
 */
std::ostream& operator<<( std::ostream &o, symbol_table const &s );

////////// inlines ////////////////////////////////////////////////////////////

inline char const* synfo::name() const {
  return symbol_->name();
}

#define PJL_RELOP(OP)                                               \
inline bool operator OP( synfo const &s1, synfo const &s2 ) {       \
  return *s1.symbol_ OP *s2.symbol_;                                \
}                                                                   \
inline bool operator OP( synfo const &s1, char const *s2 ) {        \
  return *s1.symbol_ OP s2;                                         \
}                                                                   \
inline bool operator OP( char const *s1, synfo const &s2 ) {        \
  return s1 OP *s2.symbol_;                                         \
}                                                                   \
inline bool operator OP( synfo const &s1, std::string const &s2 ) { \
  return *s1.symbol_ OP s2;                                         \
}                                                                   \
inline bool operator OP( std::string const &s1, synfo const &s2 ) { \
  return s1 OP *s2.symbol_;                                         \
}

PJL_RELOP(==)
PJL_RELOP(!=)
PJL_RELOP(< )
PJL_RELOP(> )
PJL_RELOP(<=)
PJL_RELOP(>=)
#undef PJL_RELOP

///////////////////////////////////////////////////////////////////////////////

} // namespace

#endif /* pjl_symbol_table_H */
/* vim:set et ts=2 sw=2: */
