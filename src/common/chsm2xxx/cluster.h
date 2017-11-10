/*
**      CHSM Language System
**      src/common/chsm2xxx/cluster.h
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

#ifndef CHSM_cluster_info_H
#define CHSM_cluster_info_H

// local
#include "parent.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct cluster_info : parent_info
// 
// DESCRIPTION
//
//  A cluster_info is-a parent_info for a cluster.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    bool const history_;

    cluster_info( PJL::symbol const *parent = 0, char const *derived = 0,
                  bool history = false );

    virtual std::ostream& emit_declaration( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_definition ( std::ostream&, unsigned = 0 ) const;
    virtual std::ostream& emit_initializer( std::ostream&, unsigned = 0 ) const;
};

#endif  /* CHSM_cluster_info_H */
/* vim:set et ts=4 sw=4: */
