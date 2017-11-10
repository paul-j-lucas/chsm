/*
**      CHSM Language System
**      src/common/chsm2xxx/global.h
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

#ifndef CHSM_global_info_H
#define CHSM_global_info_H

// local
#include "base.h"

//*****************************************************************************
//
// SYNOPSIS
//
        struct global_info : base_info
//
// DESCRIPTION
//
//      A global_info is-a base_info such that a symbol having an info of
//      global_info (or an info of a class derived from global_info) will be
//      retained for the life of the compilation, i.e., is not subject to
//      scope.
//
//*****************************************************************************
{
    CHSM_DECLARE_RTTI

    global_info() : base_info( global_scope ) { }
};

#endif  /* CHSM_global_info_H */
/* vim:set et ts=4 sw=4: */
