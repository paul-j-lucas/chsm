##
#	CHSM Language System
#	src/java/lib/CHSM/GNUmakefile
#
#	Copyright (C) 2004-2008  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
# 
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
# 
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

########## You shouldn't have to change anything below this line. #############

ROOT:=		../../../..
include 	$(ROOT)/config/config.mk

TARGET:=	chsm.jar

##
# Build rules
##

.PHONY: all lib libs
all lib libs:
	$(ANT) -Djar-file=$(TARGET)

##
# Install rules
##

install: $(TARGET) $(I_LIB)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(TARGET) $(I_LIB)

$(I_LIB):
	$(MKDIR) $@

##
# Utility rules
##

clean distclean javadoc:
	$(ANT) -Djar-file=$(TARGET) $@

dist: distclean

# vim:set noet sw=8 ts=8:
