##
#       CHSM Language System
#       test/java/Makefile.am
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

CHSMC =		$(top_builddir)/src/c++/chsmc/chsmc
CHSMC_FLAGS =	-x java
CLASSPATH =	$(top_builddir)/src/java/libchsm/CHSM/chsm.jar
JAVAC_FLAGS =	-classpath $(CLASSPATH):tests

.chsmj.java:
	$(CHSMC) $(CHSMC_FLAGS) -E $< > $@

.java.class:
	$(JAVAC) $(JAVAC_FLAGS) $<

###############################################################################

#
# Command-line option tests
# =========================
#
TESTS =	tests/dominance1.class \
	tests/dominance2.class \
	tests/dominance3.class \
	tests/enter_deep.class \
	tests/enter_exit.class \
	tests/enter_once.class \
	tests/erroneous1.class \
	tests/erroneous2.class \
	tests/events1.class \
	tests/events2.class \
	tests/events3.class \
	tests/events4.class \
	tests/events5.class \
	tests/finite.class \
	tests/history1.class \
	tests/history2.class \
	tests/internal.class \
	tests/microstep1.class \
	tests/microstep2.class \
	tests/nondeterminism.class \
	tests/precondition.class \
	tests/target1.class \
	tests/target2.class

###############################################################################

AM_TESTS_ENVIRONMENT =	CLASSPATH=$(CLASSPATH):.; export CLASSPATH ;

TEST_EXTENSIONS = .class
CLASS_LOG_DRIVER = $(srcdir)/java_test.sh

clean-local:
	rm -f tests/*.class

EXTRA_DIST = tests
dist-hook:
	cd $(distdir)/tests && rm -fr *.log *.trs

###############################################################################
# vim:set noet sw=8 ts=8:
