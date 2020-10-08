/*
**      PJL C++ Library
**      file.cpp
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
#include "config.h"                     /* must go first */
#include "chsm_compiler.h"
#include "file.h"
#include "indent.h"

// standard
#include <cstdlib>
#include <memory>
#include <sysexits.h>
#include <unistd.h>

using namespace PJL;
using namespace std;
namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////

file_base::file_base( fs::path const &path, ios::openmode mode ) :
  path_{ path },
  fio_{ nullptr }
{
  if ( path_.empty() )
    return;
  fio_ = new fstream( path_, mode );
  if ( fio_ == nullptr || fio_->fail() ) {
    cc_error()
      << "could not open \"" << path_ << "\" for "
      << (mode == ios::in ? "input" : "output") << endl;
    ::exit( EX_NOINPUT );
  }
}

source_file::source_file( fs::path const &path ) :
  file_base{ path, ios::in }
{
  init();
}

source_file::~source_file() {
  if ( errors_ > 0 ) {
    cerr << errors_ << " error";
    if ( errors_ > 1 )
      cerr << 's';
    cerr << endl;
  }
  if ( warnings_ > 0 ) {
    cerr << warnings_ << " warning";
    if ( warnings_ > 1 )
      cerr << 's';
    cerr << endl;
  }
}

ostream& source_file::complain_at( char const *error_type,
                                   unsigned alt_line_no ) {
  if ( !path_.empty() )
    cerr << '\"' << path_ << "\", ";
  cerr
    << "line " << (alt_line_no > 0 ? alt_line_no : line_no_) << ": "
    << error_type << ": ";
  return cerr;
}

void source_file::init() {
  line_no_ = 1;
  errors_ = warnings_ = 0;
  check_only_ = false;
}

///////////////////////////////////////////////////////////////////////////////

user_code::user_code() :
  file_base{ temp_path( "chsm.XXXXXX" ), std::ios::out }
{
  out() << inc_indent;
}

user_code::~user_code() {
  std::error_code ec;
  if ( !fs::remove( path_, ec ) )
    cc_error() << path_ << ": could not remove: " << STRERROR;
}

void user_code::copy_to( ostream &o ) {
  file().close();
  file().open( path_.c_str(), ios::in );

  if ( !file() ) {
    cc_fatal() << "cannot re-open temporary file \"" << path_ << '"' << endl;
    ::exit( EX_NOINPUT );
  }

  char buf[ 4096 ];
  streamsize count;
  do {
    if ( file().read( buf, sizeof buf ).bad() ) {
      cc_fatal() << "error reading temporary file \"" << path_ << '"' << endl;
      ::exit( EX_NOINPUT );
    }
    file().clear();
    count = file().gcount();
    o.write( buf, count );
  } while ( count == sizeof buf );
  o T_ENDL;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et ts=2 sw=2: */
