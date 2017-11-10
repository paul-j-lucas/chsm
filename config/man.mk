##
#	CHSM Language System
#	config/man.mk
#
#	Copyright (C) 1996-2013  Paul J. Lucas
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

NROFF=		nroff -man
#		Command for formatting Unix manual pages for a terminal screen;
#		usually "nroff -man".

TBL=		tbl
#		Command for formatting tables; usually "tbl".

TROFF=		troff -man
#		Command for formatting Unix manual pages for a phototypesetter;
#		usually "troff -man".

DPOST=		/usr/lib/lp/postscript/dpost
ifneq ($(wildcard $(DPOST)),)
TO_PS=		$(DPOST)
else
TO_PS=		grops
endif
#		Command to convert troff output to PostScript; usually
#		"/usr/lib/lp/postscript/dpost" for Solaris or "grops" for
#		systems using groff.

TO_TXT=		col -b
#		Command to strip all non-text characters from nroff output to
#		generate plain text versions of manual pages; usually
#		"col -b".

########## You shouldn't have to change anything below this line. #############

include	$(ROOT)/config/config.mk

# $(SECT) is defined by the makefile including this
PAGES:=		$(wildcard *.$(SECT))
TARGET_TXT:=	$(PAGES:.$(SECT)=.txt)
TARGET_PDF:=	$(PAGES:.$(SECT)=.pdf)
TARGET_PS:=	$(PAGES:.$(SECT)=.ps)

##
# Build rules
##

.SUFFIXES:
.SUFFIXES: .$(SECT) .pdf .ps .txt

%.txt : %.$(SECT)
	$(TBL) $< | $(NROFF) | $(TO_TXT) > $@

%.ps : %.$(SECT)
	$(TBL) $< | $(TROFF) | $(TO_PS) > $@

# Use this function to try to locate Acrobat Distiller since it produces
# better PDF than Ghostscript.
pathsearch = $(firstword $(wildcard $(addsuffix /$(1),$(subst :, ,$(PATH)))))

%.pdf : %.ps
ifneq ($(call pathsearch,distill),)
	distill $<
else
	gs -q -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=$@ $< -c quit
endif

all:

text txt: $(TARGET_TXT)
pdf: $(TARGET_PDF)
ps : $(TARGET_PS)
all: text pdf ps

##
# Install rules
##

install: $(I_MAN)/man$(SECT)
	$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_MODE) $(PAGES) $?

$(I_MAN)/man$(SECT):
	$(MKDIR) $@

uninstall:
	cd $(I_MAN)/man$(SECT) && $(RM) $(PAGES)

##
# Utility rules
##

clean dist distclean:
	$(RM) $(TARGET_TXT) $(TARGET_PDF) $(TARGET_PS)

# vim:set noet sw=8 ts=8:
