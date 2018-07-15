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

ARGLIST_TESTS =	tests/_cpmf_v.arglist \
		tests/_i.arglist \
		tests/_ii.arglist \
		tests/_pfii_v.arglist \
		tests/_r_map_ii.arglist \
		tests/ai.arglist \
		tests/ai012.arglist \
		tests/ai0xA.arglist \
		tests/ai9.arglist \
		tests/apcc.arglist \
		tests/cpi.arglist \
		tests/cpmf_v.arglist \
		tests/i.arglist \
		tests/ii.arglist \
		tests/pf_v.arglist \
		tests/pfcpce_v.arglist \
		tests/pfii_v.arglist \
		tests/pfipfi_v_pfi_v.arglist \
		tests/pi.arglist \
		tests/pmf_v.arglist \
		tests/r_map_ii.arglist \
		tests/ri.arglist \
		tests/rri.arglist \
		tests/rtii.arglist \
		tests/tii.arglist

CHSMC_TESTS =	tests/derived \
		tests/dominance1 \
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

TESTS =		$(ARGLIST_TESTS) \
		$(CHSMC_TESTS)

###############################################################################

AM_TESTS_ENVIRONMENT = BUILD_SRC=$(top_builddir)/src; export BUILD_SRC ;

TEST_EXTENSIONS = .arglist
ARGLIST_LOG_DRIVER = $(top_srcdir)/test/arglist_test.sh

clean-local:
	rm -fr tests/*.dSYM

clean-tests:
	rm -f $(CHSMC_TESTS)

EXTRA_DIST = arglist_test.sh tests
dist-hook:
	cd $(distdir)/tests && rm -fr *.log *.trs

###############################################################################
# vim:set noet sw=8 ts=8:
