##
#	CHSM Language System
#	GNUmakefile
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

ROOT:=		.
include		$(ROOT)/config/config.mk

SUBDIRS:=	bin src man

##
# Build rules
##

all %:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

dist: distclean
	@if [ -d .svn ]; then \
	echo; echo "Won't 'make $@' in source tree!"; echo; exit 1; \
	fi
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

# vim:set noet sw=8 ts=8:
