##
#	CHSM Language System
#	src/c++/lib/GNUmakefile
#
#	Copyright (C) 1996-2013  Paul J. Lucas & Fabio Riccardi
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

ROOT:=		../../..
include 	$(ROOT)/config/config.mk

TARGET:=	libchsm.a
SOURCES:=	$(wildcard *.c)
CFLAGS:=	$(CCFLAGS)

##
# Build rules
##

.PHONY: all doc docs lib libs
all lib libs: $(TARGET)

$(TARGET): $(SOURCES:.c=.o)
	$(AR) $@ $^
	-$(RANLIB) $@

# Don't do the "include" if the goal contains the word "clean" or "dist".
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
ifneq ($(findstring dist,$(MAKECMDGOALS)),dist)
-include $(SOURCES:%.c=.%.d)
endif
endif

doc docs:
	doxygen

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

clean:
	$(RM) *.o

dist distclean: clean
	$(RM) $(TARGET) .*.d

# vim:set noet sw=8 ts=8:
