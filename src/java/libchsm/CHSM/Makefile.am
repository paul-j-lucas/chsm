##
#	CHSM Language System
#	src/java/libchsm/CHSM/Makefile.am
#
#	Copyright (C) 2018  Paul J. Lucas & Fabio Riccardi
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 3 of the Licence, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

JAR =		chsm.jar

SOURCES =	Cluster.java \
		Event.java \
		Machine.java \
		Parent.java \
		Set.java \
		State.java \
		Transition.java

# Since automake's support for Java is minimal, deprecated, and better support
# may never happen, we use just enough Make rules to call ant to do the actual
# build.

$(JAR): $(SOURCES)
	$(ANT) -Djar-file=$(JAR)

all-local: $(JAR)

clean-local:
	$(ANT) -Djar-file=$(JAR) clean

distclean-local:
	$(ANT) -Djar-file=$(JAR) distclean

install-data-local:
	$(INSTALL_DATA) $(JAR) $(top_builddir)/lib

javadoc:
	$(ANT) -Djar-file=$(JAR) javadoc

# vim:set noet sw=8 ts=8:
