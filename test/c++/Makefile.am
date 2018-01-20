##
#       CHSM Language System
#       test/c++/Makefile.am
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

AUTOMAKE_OPTIONS = 1.12			# needed for TEST_LOG_DRIVER

CXXFLAGS +=	-I$(top_srcdir)/src/c++/libchsm
LDADD =		$(top_builddir)/src/c++/libchsm/libchsm.a

CHSMC =		$(top_builddir)/src/c++/chsmc/chsmc

.chsmc.cpp:
	$(CHSMC) -E $< > $@

.cpp:
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDADD)

###############################################################################

TESTS =	tests/dominance1 \
	tests/dominance2 \
	tests/dominance3 \
	tests/enter_deep \
	tests/enter_exit \
	tests/enter_once \
	tests/erroneous1 \
	tests/erroneous2 \
	tests/events1 \
	tests/events2 \
	tests/events3 \
	tests/events4 \
	tests/events5 \
	tests/finite \
	tests/history1 \
	tests/history2 \
	tests/internal \
	tests/microstep1 \
	tests/microstep2 \
	tests/nondeterminism \
	tests/precondition \
	tests/target1 \
	tests/target2

###############################################################################

clean-local:
	rm -fr tests/*.dSYM

clean-tests:
	rm -f $(TESTS)

EXTRA_DIST = tests
dist-hook:
	cd $(distdir)/tests && rm -fr *.log *.trs

###############################################################################
# vim:set noet sw=8 ts=8:
