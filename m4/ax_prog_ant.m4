##
# SYNOPSIS
#
#       AX_PROG_ANT([ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#       This macro searches for the ant build tool and sets the variable "ant"
#       to the name of the application or to "no" if not found. If ACTION-IF-
#       NOT-FOUND is not specified, configure will fail when the program is not
#       found.
#
#       Example:
#
#           AX_PROG_ANT()
#           AX_PROG_ANT([ant_avail="no"])
#
# LICENSE
#
#       Copyright (C) 2018  Paul J. Lucas
#
#       This program is free software: you can redistribute it and/or modify
#       it under the terms of the GNU Lesser General Public License as
#       published by the Free Software Foundation, either version 3 of the
#       License, or (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful, but
#       WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
#       General Public License for more details.
#
#       You should have received a copy of the GNU Lesser General Public
#       License along with this program. If not, see
#       <https://www.gnu.org/licenses/>.
##

AC_DEFUN([AX_PROG_ANT],[
  AC_CHECK_PROG([ANT], ant, ant, no)
  if test x$ant = xno;
  then
    ifelse($#, 0, [AC_MSG_ERROR([ant not found])], $1)
  fi
])

# vim:set et sw=2 ts=2:
