/*
**      CHSM Language System
**      src/c++/chsmc/info_visitor.h
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

#ifndef chsmc_info_visitor_H
#define chsmc_info_visitor_H

struct child_info;
struct chsm_info;
struct cluster_info;
struct event_info;
struct global_info;
struct set_info;
struct state_info;
struct transition_info;
struct user_event_info;

///////////////////////////////////////////////////////////////////////////////

/**
 * An %info_visitor is used to visit an `*_info` object and all its descendant
 * `*_info` objects (if any).
 */
class info_visitor {
public:
  virtual ~info_visitor();

  virtual void visit( child_info const &si ) = 0;
  virtual void visit( chsm_info const &si ) = 0;
  virtual void visit( cluster_info const &si ) = 0;
  virtual void visit( event_info const &si ) = 0;
  virtual void visit( global_info const &si ) = 0;
  virtual void visit( set_info const &si ) = 0;
  virtual void visit( state_info const &si ) = 0;
  virtual void visit( transition_info const &si ) = 0;
  virtual void visit( user_event_info const &si ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_info_visitor_H */
/* vim:set et sw=2 ts=2: */
