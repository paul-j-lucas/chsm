##
#	CHSM Language System
#	config/config.mk
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
#	Note: If you later redefine any of these variables, you *MUST* first
#	do a "make distclean" before you do another "make".
##

###############################################################################
#
#       OS selection
#
#	FreeBSD is a special-case: uncomment only if you are using FreeBSD.
#
###############################################################################

#FREE_BSD:=1

###############################################################################
#
#	General stuff
#
###############################################################################

ANT:=		ant
#		The command (plus arguments) to run ant.

AR:=		ar rv
#		The command (plus arguments) to create archive libraries;
#		usually "ar rv".

JAR:=		jar cf
#		The command (plus arguments) to make Java archive libraries;
#		usually "jar cf".

RANLIB:=	ranlib
#		The command to generate library tables-of-contents; usually
#		"ranlib".  If your OS doesn't need this done to libraries, you
#		can still leave this here since errors from this command are
#		ignored in the makefiles.

RM:=		rm -fr
#		The command to remove files recursively and ignore errors;
#		usually "rm -fr".

PERL:=		perl
#		Perl 5 needed to make dependencies; usually "perl".

SHELL:=		/bin/sh
#		The shell to spawn for subshells; usually "/bin/sh".

STRIP:=		strip
#		The command to strip symbolic information from executables;
#		usually "strip".  You can leave this defined even if your OS
#		doesn't have it or any equivalent since any errors from this
#		command are ignored in the Makefiles.

###############################################################################
#
#	C++ compiler
#
###############################################################################

CC:=		g++
#		The C++ compiler you are using; usually "CC" or "g++".

CCFLAGS:=	# nothing to start
#		Flags for the C++ compiler.

#DEBUG:=		true
ifdef DEBUG
OPTIM:=		-g
else
CCFLAGS+=	-DNDEBUG
#		Turn of assertions.

OPTIM:=		-O2
#		The optimization level.  Many compilers allow a digit after the
#		O to specify the level of optimization; if so, set yours to the
#		highest number your compiler allows without eliciting problems
#		in the optimizer.
#
#		Using g++, -O3 under Cynwin under Windows produces bad code;
#		-O3 with 3.0 causes the optimizer to take ridiculously long and
#		use most of the CPU and memory.
#
#		If CHSM++ doesn't work correctly with optimization on, but it
#		works just fine with it off, then there is a bug in your
#		compiler's optimizer.

ifeq ($(findstring g++,$(CC)),g++)
OPTIM+=		-felide-constructors -fomit-frame-pointer
endif
endif # DEBUG

MULTITHREADED:=	-DCHSM_MULTITHREADED -D_REENTRANT
#		With this defined, the CHSM/C++ library will use POSIX threads
#		to be thread-safe.  If you have no intention of using CHSM/C++
#		to write multi-threaded applications, then you can comment this
#		out so as not to waste time locking and unlocking mutexes.  If
#		you do comment this out, you also MUST comment it out in
#		bin/chsmc.

ifdef FREE_BSD
PTHREAD_LINK:=	-pthread
else
PTHREAD_LINK:=	-lpthread
endif
#		Library to link against for POSIX threads if building
#		multithreaded.

CCFLAGS+=	$(MULTITHREADED) $(OPTIM)
#		Flags for the C++ compiler.

ifeq ($(findstring g++,$(CC)),g++)
CCFLAGS+=	-fno-exceptions
#		Since CHSM doesn't use exceptions or RTTI, turn off code
#		generation for them to save space in the executables.
endif

ifeq ($(findstring g++,$(CC)),g++)
#CCFLAGS+=      -fmessage-length=0 -W -Wcast-align -Wcast-qual -Wnon-virtual-dtor -Wpointer-arith -Wreorder -Wswitch -Wuninitialized -Wunreachable-code -Wunused #-Winline -Wshadow
endif
#		Warning flags specific to g++.  Unless you are modifying the
#		source code, you should leave this commented out.

###############################################################################
#
#	lex
#
###############################################################################

LEX:=		flex
#		Lexical analyzer generator; usually "lex" or "flex".

LFLAGS:=	-t
#		Flags for lexical analyzer:
#		-d	Turn on debug mode -- flex only!  Also add -DLEX_DEBUG
#			to LCFLAGS below.
#		-t	Send result to standard output.

LCFLAGS:=	-D__EXTERN_C__
#		Additional flags for C++ compiler when compiling $LEX output:
#		-D__EXTERN_C__
#			Needed at least on Solaris when using lex.
#		-DLEX_DEBUG
#			Turn on flex debugging.

LEX_L:=		-L/usr/lib
#		-L directive pointing to where the [f]lex library is.

LEX_l:=		-ll
#		Name of lex library; usually either "l" or "fl".

###############################################################################
#
#	yacc
#
###############################################################################

YACC:=		yacc
#		Parser generator; usually "yacc" or "bison".

YFLAGS:=	-d
#		Flags for parser generator:
#		-by	File prefix -- needed for bison (see below).
#		-d	Generate definitions header file.
#		-t	Turn on debug mode; also add -DYYDEBUG to DEFS in
#			src/common/chsm2xxx/chsm2xxx.mk.

ifeq ($(findstring bison,$(YACC)),bison)
YFLAGS+=	-by
endif

#YCFLAGS=	
#		Additional flags for C++ compiler when compiling yacc output.

YACC_LIB:=	y
#		Name of yacc library:
#		y	For yacc.

#YACC_l:=		-l$(YACC_LIB)
#		-l directive for yacc library.  Some implementations need this;
#		uncomment if yours does.

###############################################################################
#
#	Java
#
###############################################################################

JAVA:=		java
#		The Java interpreter you are using; usually "java".

JAVA_CLASSPATH_OPT:= -classpath
#		The classpath argument for $(JAVA); usually "-classpath".

###############################################################################
#
#	Installation
#
###############################################################################

INSTALL=	$(CONFIG)/install-sh
#		Install command; usually "$(BIN)/install-sh".

I_ROOT:=	/usr/local
#		The top-level directory of where CHSM will be installed.

I_BIN:=		$(I_ROOT)/bin
#		Where executables are installed; usually "$(I_ROOT)/bin".

I_INC:=		$(I_ROOT)/include
#		Where include files are installed; usually "$(I_ROOT)/include".

I_LIB:=		$(I_ROOT)/lib
#		Where libraries are installed; usually "$(I_ROOT)/lib".

I_MAN:=		$(I_ROOT)/man
#		Where manual pages are installed; usually "$(I_ROOT)/man".

I_OWNER:=	-o bin
#		The owner of the installed files.

I_GROUP:=	-g bin
#		The group of the installed files.

I_MODE:=	-m 644
#		File permissions for regular files (non executables).

I_XMODE:=	-m 755
#		File permissions for eXecutables and directories.

MKDIR=		$(INSTALL) $(I_OWNER) $(I_GROUP) $(I_XMODE) -d
#		Command used to create a directory.

########## You shouldn't have to change anything below this line. #############

# ROOT is defined by the Makefile including this.

BIN:=		$(ROOT)/bin
CONFIG:=	$(ROOT)/config
MAN:=		$(ROOT)/man
SRC:=		$(ROOT)/src

CHSM2CXX:=	$(SRC)/c++/chsm2c++/chsm2c++
CHSM_LIB:=	$(SRC)/c++/lib/libchsm.a

CHSM2JAVA:=	$(SRC)/java/chsm2java/chsm2java
CHSM_JAR:=	$(SRC)/java/lib/CHSM/chsm.jar

MAKEDEPEND:=	$(PERL) $(CONFIG)/makedepend.pl $(CFLAGS)

.%.d : %.c
	$(SHELL) -ec '$(MAKEDEPEND) $< | sed "s!\([^:]*\):!\1 $@ : !g" > $@; [ -s $@ ] || $(RM) $@'

.%.d : %.l
	$(SHELL) -ec '$(MAKEDEPEND) $< | sed "s!\([^:]*\):!\1 $@ : !g" > $@; [ -s $@ ] || $(RM) $@'

.%.d : %.y
	$(SHELL) -ec '$(MAKEDEPEND) $< | sed "s!\([^:]*\):!\1 $@ : !g" > $@; [ -s $@ ] || $(RM) $@'

%.c :: %.chsmc
	$(CHSM2CXX) $^ $(basename $@).h $@

% :: %.chsmc
	CC=$(CC) CHSM2CXX=$(CHSM2CXX) \
	$(BIN)/chsmc -I$(SRC)/c++/lib -L$(SRC)/c++/lib -o $@ $^

%.class :: %.java
	$(JAVAC) $(JAVAC_FLAGS) $^

%.java :: %.chsmj
	$(CHSM2JAVA) $^ $@

%.class :: %.chsmj
	JAVAC=$(JAVAC) JAVA_CLASSPATH_OPT=$(JAVA_CLASSPATH_OPT) \
	CHSM2JAVA=$(CHSM2JAVA) \
	$(BIN)/chsmj -k -c $(CHSM_JAR):. $^

# vim:set noet sw=8 ts=8:
