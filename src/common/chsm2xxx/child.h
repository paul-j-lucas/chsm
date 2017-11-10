/*
**      CHSM Language System
**      src/common/chsm2xxx/child.h
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

#ifndef CHSM_child_info_H
#define CHSM_child_info_H

// local
#include "base.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct child_info : base_info
//
// DESCRIPTION
//
//      A child_info is-a base_info for identifiers in child declaration lists:
//
//          cluster c(x,y) is { // ...
//                    ^ ^
//
//      Being derived from base_info and not global_info, they are destroyed
//      when the scope closes.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    PJL::symbol const *const parent_;
    bool defined_;

    child_info( PJL::symbol const *parent );
};

#endif  /* CHSM_child_info_H */
/* vim:set et ts=4 sw=4: */
