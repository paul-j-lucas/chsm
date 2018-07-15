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

ARGLIST_TESTS =	tests/_ai-post.arglist \
		tests/_ai-pre.arglist \
		tests/_i.arglist \
		tests/_ii.arglist \
		tests/ai-post.arglist \
		tests/ai-pre.arglist \
		tests/i.arglist \
		tests/ii.arglist

CHSMJ_TESTS =	tests/derived.class \
		tests/dominance1.class \
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

TESTS =		$(ARGLIST_TESTS) \
		$(CHSMJ_TESTS)

###############################################################################

AM_TESTS_ENVIRONMENT = \
	BUILD_SRC=$(top_builddir)/src; \
	CLASSPATH=$(CLASSPATH):.; \
	export BUILD_SRC CLASSPATH ;

TEST_EXTENSIONS = .arglist .class
ARGLIST_LOG_DRIVER = $(top_srcdir)/test/arglist_test.sh
CLASS_LOG_DRIVER = $(srcdir)/java_test.sh

clean-local:
	rm -f tests/*.class

EXTRA_DIST = arglist_test.ch tests
dist-hook:
	cd $(distdir)/tests && rm -fr *.log *.trs

###############################################################################
# vim:set noet sw=8 ts=8:
