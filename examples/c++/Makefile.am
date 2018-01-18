##
#       CHSM Language System
#       examples/c++/Makefile.am
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

bin_PROGRAMS =	grid h2o hello infinite microwave vending

AM_CPPFLAGS =	-I$(top_srcdir)/src/c++/libchsm
LDADD =		-L$(top_builddir)/src/c++/libchsm -lchsm

CHSMC =		$(top_builddir)/src/c++/chsmc/chsmc

.chsmc.cpp:
	$(CHSMC) -E $< > $@

grid_SOURCES =		grid.chsmc
h2o_SOURCES =		h2o.chsmc
hello_SOURCES =		hello.chsmc
infinite_SOURCES =	infinite.chsmc
microwave_SOURCES =	microwave.chsmc
vending_SOURCES =	vending.chsmc

# vim:set noet sw=8 ts=8:
