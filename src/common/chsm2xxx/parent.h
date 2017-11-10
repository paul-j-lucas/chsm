/*
**      CHSM Language System
**      src/common/chsm2xxx/parent.h
**
**      Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
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

#ifndef CHSM_parent_info_H
#define CHSM_parent_info_H

// standard
#include <iostream>

// local
#include "state.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct parent_info : state_info
//
// DESCRIPTION
//
//      A parent_info is-a parent_data and-a state_info for holding the
//      information in common by a cluster and a set namely the children.
//
//      Note: This is an abstract class.  (Therefore, it doesn't need either of
//      the CHSM_DECLARE_RTTI or CHSM_DEFINE_RTTI macros since there will never
//      be an instance of it.)
//
//*****************************************************************************
{
    //
    // Prefix used in compiler-generated names.
    //
    static char const   class_prefix_[];

    typedef std::list<PJL::symbol*> child_list;
    child_list children_;

    virtual std::ostream& emit_definition( std::ostream&,
                                           unsigned = 0 ) const = 0;
protected:
    parent_info( char const *class_name, PJL::symbol const *parent,
                 char const *derived ) :
        state_info( class_name, parent, derived )
    {
    }

    std::ostream& common_declaration( std::ostream&,
                                      char const *formal_args = 0 ) const;
    std::ostream& common_definition ( std::ostream&,
                                      char const *formal_args = 0,
                                      char const *actual_args = 0 ) const;
};

#endif  /* CHSM_parent_info_H */
/* vim:set et ts=4 sw=4: */
