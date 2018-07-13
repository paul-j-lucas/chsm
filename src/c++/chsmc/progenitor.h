/*
**      PJL C++ Library
**      progenitor.h
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

#ifndef pjl_progenitor_H
#define pjl_progenitor_H

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

/**
 * A user-defined base class derived from progenitor will be able to construct
 * objects of further-derived classes, i.e., the capability of having a
 * "virtual constructor."
 *
 * Two things are required of the derived classes:
 *
 *  1. Defines a copy constructor that also takes an additional "deep"
 *     parameter.  It, in turn, calls its base class's copy constructor.
 *
 *  2. Uses the `PROCREATE` macro.
 *
 * @tparam RootClass The root class of the user's class hierarchy.
 */
template<class RootClass>
struct progenitor {
  virtual ~progenitor() { }

  /**
   * Clones this object.
   *
   * @param deep For tree-like objects, specifies whether to do a deep or
   * shallow clone.
   * @return Returns a clone of this object.
   */
  virtual RootClass* clone( bool deep = false ) const = 0;
};

/**
 * Defines a clone() member function for the class this macro is used within
 * the declaration of.
 *
 * @param DERIVED_CLASS The derived class name.
 * @hideinitializer
 */
#define PROCREATE(DERIVED_CLASS)                              \
  DERIVED_CLASS* clone( bool deep = false ) const override {  \
    return new DERIVED_CLASS( *this, deep );                  \
  }                                                           \
  typedef int progenitor_dummy_type_to_eat_semicolon

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* pjl_progenitor_H */
/* vim:set et ts=2 sw=2: */
