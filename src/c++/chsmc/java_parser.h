/*
**      CHSM Language System
**      src/c++/chsmc/java_parser.h
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

#ifndef chsmc_java_parser_H
#define chsmc_java_parser_H

// local
#include "lang_parser.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %java_parser is used to parse Java code.
 */
class java_parser : public lang_parser {
public:
  java_parser( std::istream &is ) : lang_parser( is ) {
  }

private:
  bool parse_param_( fn_param* ) override;
};

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_java_parser_H */
/* vim:set et sw=2 ts=2: */
