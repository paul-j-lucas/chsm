/*
**      PJL C++ Library
**      file.h
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

#ifndef file_H
#define file_H

// local
#include "util.h"

// standard
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for different types of files.
 */
class file_base {
public:
  std::string const& path() const {
    return path_;
  }

  std::istream& in() {
    return *in_;
  }

  std::ostream& out() {
    return *out_;
  }

  std::fstream& file() {
    return *fio_;
  }

protected:
  std::string const path_;

  file_base() : fio_{ nullptr } {
  }

  file_base( std::istream &i ) : in_{ &i } {
  }

  file_base( std::ostream &o ) : out_{ &o } {
  }

  ~file_base() {
    if ( fio_ != nullptr )
      fio_->close();
  }

  /**
   * Constructs a %file_base.
   *
   * @param path The path to the file.
   * @param mode The open mode.
   */
  file_base( char const *path, std::ios::openmode mode );

  /**
   * Constructs a %file_base.
   *
   * @param path The path to the file.
   * @param mode The open mode.
   */
  file_base( std::string const &path, std::ios::openmode mode ) :
    file_base{ path.c_str(), mode }
  {
  }

  union {
    std::istream *in_;
    std::ostream *out_;
    std::fstream *fio_;
  };
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Source file info.
 */
struct source_file final : file_base {
  unsigned  line_no_;
  unsigned  errors_, warnings_;
  bool      check_only_;    // true if no errors

  //
  // Prints respective message to stderr regarding current line number
  // (unless an alternate number is specified).
  //
  std::ostream& warning( unsigned alt_no = 0 );
  std::ostream& error  ( unsigned alt_no = 0 );
  std::ostream& fatal  ( unsigned alt_no = 0 );
  std::ostream& sorry  ( unsigned alt_no = 0 );

  explicit source_file( std::istream &i ) : file_base{ i } {
    init();
  }

  source_file( char const *path = nullptr );

  /**
   * Destroys a %source_file:
   *
   *  1. Prints a newline if needed.
   *  2. Prints the number of errors and warnings, if any.
   */
  ~source_file();

  void check_only() {
    check_only_ = true;
  }

protected:
  /**
   * Emits an error of a certain type regarding a line the source file.
   *
   * @param error_type The error message type.
   * @param alt_line_no The line number to refer to instead of the default.
   * @return Returns the ostream used to emit to.
   */
  std::ostream& complain_at( char const*, unsigned alt_line_no );

  void init();
};

inline std::ostream& source_file::error( unsigned alt_no ) {
  ++errors_;
  check_only_ = true;
  return complain_at( "error", alt_no );
}

inline std::ostream& source_file::fatal( unsigned alt_no ) {
  check_only_ = true;
  return complain_at( "fatal error", alt_no );
}

inline std::ostream& source_file::sorry( unsigned alt_no ) {
  check_only_ = true;
  return complain_at( "sorry, not implemented", alt_no );
}

inline std::ostream& source_file::warning( unsigned alt_no ) {
  ++warnings_;
  return complain_at( "warning", alt_no );
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Target file info.
 */
struct target_file final : file_base {
  unsigned line_no_ = 1;

  explicit target_file( std::ostream &o ) : file_base{ o } {
  }

  explicit target_file( char const *path = nullptr ) :
    file_base{ path, std::ios::out }
  {
  }

  explicit target_file( std::string const &path ) :
    file_base{ path, std::ios::out }
  {
  }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A temporary file where user-code for conditions and actions are diverted for
 * later emission into the definition file code stream.
 */
struct user_code : file_base {
  /**
   * Constructs the user code file.
   */
  user_code();

  /**
   * Deletes the user code file.
   */
  ~user_code();

  /**
   * Copies the user's code in conditions and actions from the temporary file
   * into the code definition file.
   *
   * @param o The ostream to copy to.
   */
  void copy_to( std::ostream &o );
};

///////////////////////////////////////////////////////////////////////////////

#endif /* file_H */
/* vim:set et ts=2 sw=2: */
