/*
**	CHSM Language System
**	src/java/chsm2java/arg_macros.h
**
**	Copyright (C) 1996-2013 Paul J. Lucas & Fabio Riccardi
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
** 
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
**	GNU General Public License for more details.
** 
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef	chsm_arg_macros_H
#define	chsm_arg_macros_H

// macros to aid in argument-lists
#define	CHSM_FORMAL(X) #X " " 
#define	CHSM_ACTUAL(X) /* nothing */

#define	CHSM_STATE_ARG_LIST(A) \
	A(CHSM.Machine) "chsmM, " \
	A(String) "chsmN, " \
	A(CHSM.Parent) "chsmP, " \
	A(CHSM.State.Action) "chsmEA, " \
	A(CHSM.State.Action) "chsmXA, " \
	A(CHSM.Event) "chsmEE, " \
	A(CHSM.Event) "chsmXE" 

#define	CHSM_STATE_ARGS CHSM_STATE_ARG_LIST(CHSM_FORMAL)
#define	CHSM_STATE_INIT CHSM_STATE_ARG_LIST(CHSM_ACTUAL)

#define	CHSM_EVENT_ARG_LIST(A) \
	A(CHSM.Machine) "chsmM, " \
	A(int[]) "chsmTids, " \
	A(String) "chsmN, " \
	A(CHSM.Event) "chsmBE"

#define	CHSM_EVENT_ARGS CHSM_EVENT_ARG_LIST(CHSM_FORMAL)
#define	CHSM_EVENT_INIT CHSM_EVENT_ARG_LIST(CHSM_ACTUAL)

#define	CHSM_ARG_LIST(A) \
	A(CHSM.State) "chsmS" A([])", " \
	A(CHSM.Transition) "chsmT" A([])", " \
	A(CHSM.Event) "chsmTE" A([])

#define	CHSM_ARGS CHSM_ARG_LIST(CHSM_FORMAL)
#define	CHSM_INIT CHSM_ARG_LIST(CHSM_ACTUAL)

#define	CHSM_PARENT_ARG_LIST(A) \
	CHSM_STATE_ARG_LIST(A)", " \
	A(int[]) "chsmCids"

#define	CHSM_PARENT_ARGS CHSM_PARENT_ARG_LIST(CHSM_FORMAL)
#define	CHSM_PARENT_INIT CHSM_PARENT_ARG_LIST(CHSM_ACTUAL)

#define	CHSM_CLUSTER_ARG_LIST(A) \
	CHSM_PARENT_ARG_LIST(A)", " \
	A(boolean) "chsmH"

#define	CHSM_CLUSTER_ARGS CHSM_CLUSTER_ARG_LIST(CHSM_FORMAL)
#define	CHSM_CLUSTER_INIT CHSM_CLUSTER_ARG_LIST(CHSM_ACTUAL)

#define	CHSM_SET_ARG_LIST(A) \
	CHSM_PARENT_ARG_LIST(A)

#define	CHSM_SET_ARGS CHSM_SET_ARG_LIST(CHSM_FORMAL)
#define	CHSM_SET_INIT CHSM_SET_ARG_LIST(CHSM_ACTUAL)
 
#endif /* chsm_arg_macros_H */
/* vim:set noet ts=8 sw=8: */
