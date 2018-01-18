##
#       CHSM Language System
#       examples/java/Makefile.am
#
#       Copyright (C) 2018  Paul J. Lucas
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

javadir =	.
java_JAVA =	grid.java \
		h2o.java \
		hello.java \
		infinite.java \
		microwave.java \
		vending.java

CHSMC =		$(top_builddir)/src/c++/chsmc/chsmc
CLASSPATH =	$(top_builddir)/src/java/libchsm/CHSM/chsm.jar
BUILD_DIR =	$(top_builddir)/examples/java/build
JAVACFLAGS =	-classpath $(CLASSPATH)
JAVAROOT =	$(BUILD_DIR)

.chsmj.java:
	$(CHSMC) -E $< > $@

$(java_JAVA): $(BUILD_DIR)

$(BUILD_DIR):
	$(MKDIR_P) $@

grid h2o hello infinite microwave vending:
	$(JAVA) -classpath $(CLASSPATH):$(BUILD_DIR) $@

# vim:set noet sw=8 ts=8:
