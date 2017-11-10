/*
**      PJL C++ Library
**      pjl_threads.h
**
**      Copyright (C) 1998  Paul J. Lucas
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

#ifndef pjl_threads_H
#define pjl_threads_H

// standard
#ifdef  CHSM_MULTITHREADED
#include <pthread.h>
#endif  /* CHSM_MULTITHREADED */

#ifdef  CHSM_MULTITHREADED
//
// Macros to wrap critical sections.
//
#define DEFER_CANCEL() \
        { int pjl_cancel_type; \
        ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &pjl_cancel_type )

#define RESTORE_CANCEL() \
        ::pthread_setcanceltype( pjl_cancel_type, 0 ); \
        ::pthread_testcancel(); }

#else   /* CHSM_MULTITHREADED */

#define DEFER_CANCEL()                  /* nothing */
#define RESTORE_CANCEL()                /* nothing */

#endif  /* CHSM_MULTITHREADED */

#ifdef  CHSM_MULTITHREADED
//
// Define a more robust mutex locking mechanism by ensuring that the mutex will
// be unlocked even if the thread is cancelled.  Also roll in optional critical
// section protection since we're doing pthread_setcanceltype() anyway.
//
// See also: Bradford Nichols, Dick Buttlar, and Jacqueline Proulx Farrell.
// "Pthreads Programming," O'Reilly & Associates, Sebastopol, CA, 1996.
// pp. 141-142.
//
#define MUTEX_LOCK(M,D) { \
        int pjl_cancel_type; bool const pjl_defer_cancel = (D); \
        ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &pjl_cancel_type ); \
        pthread_cleanup_push( (void (*)(void*))::pthread_mutex_unlock, (M) ); \
        ::pthread_mutex_lock( (M) ); \
        if ( D ) ; else { \
            ::pthread_setcanceltype( pjl_cancel_type, 0 ); \
            ::pthread_testcancel(); \
        }

#define MUTEX_UNLOCK() \
        pthread_cleanup_pop( 1 ); \
        if ( pjl_defer_cancel ) { \
            ::pthread_setcanceltype( pjl_cancel_type, 0 ); \
            ::pthread_testcancel(); \
        } \
    }

#else   /* CHSM_MULTITHREADED */

#define MUTEX_LOCK(M,D)                 /* nothing */
#define MUTEX_UNLOCK()                  /* nothing */

#endif  /* CHSM_MULTITHREADED */

#endif  /* pjl_threads_H */
/* vim:set et sw=4 ts=4: */
