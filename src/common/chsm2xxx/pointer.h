/*
**      PJL C++ Library
**      pointer.h
**
**      Copyright (C) 2002-2013  Paul J. Lucas
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

#ifndef pointer_H
#define pointer_H

namespace PJL {

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class comp_ptr
//
// DESCRIPTION
//
//      A comp_ptr<T> wraps a T*.  The benefits is that relational comparisons
//      involving a comp_ptr<T> compare the object pointed to.
//
//****************************************************************************/
{
public:
    typedef T value_type;

    comp_ptr( T *p = 0 )                    : p_( p ) { }
    comp_ptr( comp_ptr<T> const &r )        : p_( r.p_ ) { }

    comp_ptr<T>& operator=( T *p ) {
        p_ = p;
        return *this;
    }
    comp_ptr<T>& operator=( comp_ptr<T> const &r ) {
        p_ = r.p_;
        return *this;
    }

    operator T*() const                     { return p_; }

    T&  operator* () const                  { return *p_; }
    T*  operator->() const                  { return  p_; }
protected:
    mutable T *p_;
};

#define PJL_RELOP(OP)                                           \
    template<typename T> bool                                   \
    operator OP( comp_ptr<T> const &p, comp_ptr<T> const &q ) { \
        return *p OP *q;                                        \
    }                                                           \
    template<typename T> bool                                   \
    operator OP( comp_ptr<T> const &p, T *q ) {                 \
        return *p OP *q;                                        \
    }                                                           \
    template<typename T> bool                                   \
    operator OP( T *p, comp_ptr<T> const &q ) {                 \
        return *p OP *q;                                        \
    }

    PJL_RELOP(==)
    PJL_RELOP(!=)
    PJL_RELOP(< )
    PJL_RELOP(> )
    PJL_RELOP(<=)
    PJL_RELOP(>=)

#undef  PJL_RELOP

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class token_ptr : public comp_ptr<T>
//
// DESCRIPTION
//
//      A token_ptr<T> is-a comp_ptr<T>.  The additional benefits are:
//      - copying transfers managerial responsibility of the object pointed to
//      - assignment deletes the old object pointed to
//      - deletion deletes the object pointed to
//
//******************************************************************************
{
public:
    token_ptr( T *p = 0 ) : comp_ptr<T>( p ) { }
    token_ptr( token_ptr<T> const &r ) : comp_ptr<T>( r ) {
        r.p_ = 0;
    }
    ~token_ptr() { delete this->p_; }

    token_ptr<T>& operator=( token_ptr<T> const &r ) {
        delete this->p_;
        this->p_ = r.p_;
        r.p_ = 0;
        return *this;
    }
    token_ptr<T>& operator=( T *p ) {
        delete this->p_;
        this->p_ = p;
        return *this;
    }
};

#define PJL_RELOP(OP)                                               \
    template<typename T> bool                                       \
    operator OP( token_ptr<T> const &p, token_ptr<T> const &q ) {   \
        return *p OP *q;                                            \
    }                                                               \
    template<typename T> bool                                       \
    operator OP( token_ptr<T> const &p, T *q ) {                    \
        return *p OP *q;                                            \
    }                                                               \
    template<typename T> bool                                       \
    operator OP( T *p, token_ptr<T> const &q ) {                    \
        return *p OP *q;                                            \
    }

    PJL_RELOP(==)
    PJL_RELOP(!=)
    PJL_RELOP(< )
    PJL_RELOP(> )
    PJL_RELOP(<=)
    PJL_RELOP(>=)

#undef  PJL_RELOP

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class copy_ptr : public comp_ptr<T>
//
// DESCRIPTION
//
//      A copy_ptr<T> is-a comp_ptr<T>.  The additional benefits are:
//      - copying copies the object pointed to
//      - assignment deletes the old object pointed to
//      - deletion deletes the object pointed to
//
//*****************************************************************************
{
public:
    copy_ptr( T *p = 0 ) : comp_ptr<T>( p ) { }
    copy_ptr( copy_ptr<T> const &r ) :
        comp_ptr<T>( r.p_ ? new T( *r.p_ ) : 0 ) { }
    ~copy_ptr() { delete this->p_; }

    copy_ptr<T>& operator=( copy_ptr<T> const &r ) {
        delete this->p_;
        this->p_ = r.p_ ? new T( *r.p_ ) : 0;
        return *this;
    }
    copy_ptr<T>& operator=( T *p ) {
        delete this->p_;
        this->p_ = p;
        return *this;
    }
};

#define PJL_RELOP(OP)                                           \
    template<typename T> bool                                   \
    operator OP( copy_ptr<T> const &p, copy_ptr<T> const &q ) { \
        return *p OP *q;                                        \
    }                                                           \
    template<typename T> bool                                   \
    operator OP( copy_ptr<T> const &p, T *q ) {                 \
        return *p OP *q;                                        \
    }                                                           \
    template<typename T> bool                                   \
    operator OP( T *p, copy_ptr<T> const &q ) {                 \
        return *p OP *q;                                        \
    }

    PJL_RELOP(==)
    PJL_RELOP(!=)
    PJL_RELOP(< )
    PJL_RELOP(> )
    PJL_RELOP(<=)
    PJL_RELOP(>=)

#undef  PJL_RELOP

//*****************************************************************************
//
// SYNOPSIS
//
        template<typename T> class clone_ptr : public comp_ptr<T>
//
// DESCRIPTION
//
//      A clone_ptr<T> is-a comp_ptr<T>.  The additional benefits are:
//      - copying clones the object pointed to
//      - assignment deletes the old object pointed to
//      - deletion deletes the object pointed to
//
//      The type T is presumed to have a clone() member function.
//
//      A clone_ptr is useful in the case where it points to an instance of
//      some class C or some instance of a class D derived from C.
//
//*****************************************************************************
{
public:
    clone_ptr( T *p = 0 ) : comp_ptr<T>( p ) { }
    clone_ptr( clone_ptr<T> const &r ) :
        comp_ptr<T>( r.p_ ? r.p_->clone() : 0 ) { }
    ~clone_ptr() { delete this->p_; }

    clone_ptr<T>& operator=( clone_ptr<T> const &r ) {
        delete this->p_;
        this->p_ = r.p_ ? r.p_->clone() : 0;
        return *this;
    }
    clone_ptr<T>& operator=( T *p ) {
        delete this->p_;
        this->p_ = p;
        return *this;
    }
};

#define PJL_RELOP(OP)                                                   \
        template<typename T> bool                                       \
        operator OP( clone_ptr<T> const &p, clone_ptr<T> const &q ) {   \
            return *p OP *q;                                            \
        }                                                               \
        template<typename T> bool                                       \
        operator OP( clone_ptr<T> const &p, T *q ) {                    \
            return *p OP *q;                                            \
        }                                                               \
        template<typename T> bool                                       \
        operator OP( T *p, clone_ptr<T> const &q ) {                    \
            return *p OP *q;                                            \
        }

        PJL_RELOP(==)
        PJL_RELOP(!=)
        PJL_RELOP(< )
        PJL_RELOP(> )
        PJL_RELOP(<=)
        PJL_RELOP(>=)

#undef  PJL_RELOP

} // namespace PJL

#endif  /* pointer_H */
/* vim:set et ts=4 sw=4: */
