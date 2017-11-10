##
#	CHSM Language System
#	src/common/chsm2xxx/common_files.mk
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

# COMMON_DIR is defined by the makefile including this.
COMMON_FILES:=	$(notdir $(wildcard $(COMMON_DIR)/*.[ch]))

common_files: $(COMMON_FILES)

$(COMMON_FILES):
	ln -s $(COMMON_DIR)/$@ $@

# vim:set noet sw=8 ts=8:
