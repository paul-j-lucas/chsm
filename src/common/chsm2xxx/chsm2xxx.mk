##
#	CHSM Language System
#	src/common/chsm2xxx/chsm2xxx.mk
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

##
# DEFS
#
#	-DPJL_STACK_DEBUG	Turn on stack debugging.
#	-DYYDEBUG		Turn on YACC debugging.
##
DEFS:=		#-DPJL_STACK_DEBUG -DYYDEBUG

########## You shouldn't have to change anything below this line. #############

ROOT:=		../../..
include 	$(ROOT)/config/config.mk

CFLAGS:= 	$(CCFLAGS) $(DEFS)
LPATHS:=	$(LEX_L)
LINK:=		$(YACC_l) $(LEX_l)

Y_SOURCES:=	grammar.y
L_SOURCES:=	lex.l
YL_C_FILES:=	$(Y_SOURCES:.y=.c) $(L_SOURCES:.l=.c)
C_SOURCES:=	$(filter-out $(YL_C_FILES),$(wildcard *.c))
OBJECTS:=	$(C_SOURCES:.c=.o) $(YL_C_FILES:.c=.o)

##
# Build rules
##

.PHONY: all
all: $(TARGET)

# TARGET is defined by the makefile calling this one.
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LPATHS) -o $@ $(OBJECTS) $(LINK)

grammar.o: grammar.y
	$(YACC) $(YFLAGS) $<
	sed 's/^extern char \*malloc()..*/#include <cstdlib>/' y.tab.c > grammar.c
	$(RM) y.tab.c
	$(CC) $(CFLAGS) $(YCFLAGS) -c grammar.c || { $(RM) grammar.c; exit 1; }
	$(RM) grammar.c

lex.o: lex.l
	$(LEX) $(LFLAGS) $< > lex.c
	$(CC) $(CFLAGS) $(LCFLAGS) -c lex.c || { $(RM) lex.c; exit 1; }
	$(RM) lex.c

y.tab.h: grammar.o

# Don't do the "include" if the goal contains the word "clean" or "dist"
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
ifneq ($(findstring dist,$(MAKECMDGOALS)),dist)
-include $(C_SOURCES:%.c=.%.d) $(Y_SOURCES:%.y=.%.d) $(L_SOURCES:%.l=.%.d)
endif
endif

##
# Install rules
##

install: $(TARGET) $(I_BIN)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) $(TARGET) $(I_BIN)

$(I_BIN):
	$(MKDIR) $@

##
# Utility rules
##

# COMMON_DIR is defined by the makefile calling this one.
include		$(COMMON_DIR)/common_files.mk

clean:
	$(RM) *.o $(COMMON_FILES) $(YL_C_FILES) tokens.h y.tab.c *.out core

distclean: clean
	$(RM) $(TARGET) .*.d

# vim:set noet sw=8 ts=8:
