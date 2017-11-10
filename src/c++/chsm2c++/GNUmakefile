##
#	CHSM Language System
#	src/c++/chsm2c++/GNUmakefile
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

TARGET:=	chsm2c++

all: $(TARGET)

COMMON_DIR:=	../../common/chsm2xxx
include		$(COMMON_DIR)/common_files.mk

MAKE_TARGET=	$(MAKE) -f $(COMMON_DIR)/chsm2xxx.mk \
		COMMON_DIR=$(COMMON_DIR) TARGET=$(TARGET) $1

$(TARGET) install: $(COMMON_FILES) FORCE
	@$(call MAKE_TARGET,$@)

clean distclean:
	@$(call MAKE_TARGET,$@)

dist: distclean
	$(RM) *.xcode*

FORCE:

# vim:set noet ts=8 sw=8:
