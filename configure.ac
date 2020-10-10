# Process this file with autoconf to produce a configure script.

##
#       CHSM Language System
#       configure.ac
#
#       Copyright (C) 1996-2018  Paul J. Lucas & Fabio Riccardi
#
#       This program is free software: you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation, either version 3 of the License, or
#       (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License
#       along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

AC_INIT([CHSM], [5.0], [paul@lucasmail.org],,[https://github.com/paul-j-lucas/chsm])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
gl_EARLY
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_LEX
AC_PROG_MKDIR_P
AC_PROG_RANLIB
AC_PROG_YACC
AC_ARG_VAR([GROOVY_HOME],[Groovy home directory])
AC_ARG_VAR([JAVAC],[Java compiler command])

test "x$LEX" = xflex ||
  AC_MSG_ERROR([required program "flex" not found])
test "x$YACC" = xyacc &&
  AC_MSG_ERROR([required program "bison" or "byacc" not found])

# Checks for libraries.

# Checks for header files.
AC_HEADER_ASSERT
AC_CHECK_HEADERS([getopt.h])
AC_CHECK_HEADERS([sysexits.h])
gl_INIT

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Program feature: stack-debug (enabled by default)
AC_ARG_ENABLE([stack-debug],
  AS_HELP_STRING([--disable-stack-debug], [disable support for stack debugging]),
  [],
  [enable_stack_debug=yes]
)
AS_IF([test x$enable_stack_debug = xyes],
  [AC_DEFINE([ENABLE_STACK_DEBUG], [1],
    [Define to 1 if CHSM debugging is enabled.])]
)

# Program feature: yacc debug (disabled by default)
AC_ARG_ENABLE([yydebug],
  AS_HELP_STRING([--enable-yydebug], [enable support for yacc debugging]),
  [],
  [enable_yydebug=no]
)
AS_IF([test x$enable_yydebug = xyes],
  [AC_DEFINE([YYDEBUG], [1],
    [Define to 1 if yacc debugging is enabled.])]
)

# Program feature: Groovy version (disabled by default)
AC_ARG_ENABLE([groovy],
  AS_HELP_STRING([--enable-groovy], [enable Groovy version (implies --enable-java)]),
  [],
  [enable_groovy=no]
)
if test "x$enable_groovy" = xyes
then
  test "x$GROOVY_HOME" != x ||
    AC_MSG_ERROR([required GROOVY_HOME environment variable for --enable-groovy not set])
  AC_SUBST([GROOVY_HOME],[$GROOVY_HOME])
fi

# Program feature: Java version (disabled by default)
AC_ARG_ENABLE([java],
  AS_HELP_STRING([--enable-java], [enable Java version]),
  [],
  [enable_java=no]
)
if test "x$enable_groovy" = xyes
then enable_java=yes                    # --enable-groovy implies --enable-java
fi
if test "x$enable_java" = xyes
then
  AX_PROG_ANT
  AX_PROG_JAVAC
  AX_PROG_JAVA
  AC_DEFINE([ENABLE_JAVA], [1],
    [Define to 1 if Java version is enabled.])
fi

# Makefile conditionals.
AM_CONDITIONAL([ENABLE_GROOVY],         [test x$enable_groovy         = xyes])
AM_CONDITIONAL([ENABLE_JAVA],           [test x$enable_java           = xyes])
AM_CONDITIONAL([ENABLE_STACK_DEBUG],    [test x$enable_stack_debug    = xyes])
AM_CONDITIONAL([ENABLE_YYDEBUG],        [test x$enable_yydebug        = xyes])

# Miscellaneous.
AC_LANG(C++)
AX_CXXFLAGS_WARN_ALL
AX_CXXFLAGS_GCC_OPTION([-Wcast-align])
AX_CXXFLAGS_GCC_OPTION([-Wextra])
AX_CXXFLAGS_GCC_OPTION([-Wredundant-decls])
AX_CXXFLAGS_GCC_OPTION([-Wwrite-strings])

# Generate files.
AH_TOP([#ifndef CHSM_config_H
#define CHSM_config_H])
AH_BOTTOM([#endif /* CHSM_config_H */])
AC_CONFIG_HEADERS([src/c++/config.h])
AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  src/Makefile
  src/c++/Makefile
  src/c++/chsmc/Makefile
  src/c++/libchsm/Makefile
  src/java/Makefile
  src/java/libchsm/Makefile
  src/java/libchsm/CHSM/Makefile
  src/groovy/Makefile
  src/groovy/libchsm/Makefile
  src/groovy/libchsm/CHSM/Makefile
  src/groovy/libchsm/CHSM/groovy/Makefile
  man/Makefile
  man/man1/Makefile
  man/man4/Makefile
  test/Makefile
  test/c++/Makefile
  test/java/Makefile
  test/groovy/build.xml
  test/groovy/Makefile
  examples/Makefile
  examples/c++/Makefile
  examples/java/Makefile
])
AC_OUTPUT

# vim:set et sw=2 ts=2:
