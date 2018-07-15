##
#	CHSM Language System
#	src/c++/chsmc/Makefile.am
#
#	Copyright (C) 1996-2018  Paul J. Lucas & Fabio Riccardi
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 3 of the License, or
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

bin_PROGRAMS =		chsmc
check_PROGRAMS =	lang_parser_test

AM_CPPFLAGS =		-I$(top_srcdir)/lib \
			-I$(top_builddir)/lib \
			-I$(top_builddir)/src/c++

AM_YFLAGS =		-d
LDADD =			$(top_builddir)/lib/libgnu.a

COMMON_SOURCES =	lang_parser.cpp lang_parser.h \
			cpp_parser.cpp cpp_parser.h \
			options.cpp options.h \
			string_builder.h \
			util.cpp util.h

if ENABLE_JAVA
COMMON_SOURCES +=	java_parser.cpp java_parser.h
endif

chsmc_SOURCES =		$(COMMON_SOURCES) \
			base_info.h \
			child_info.h \
			chsm_info.h \
			chsm_compiler.cpp chsm_compiler.h \
			clone_ptr.h \
			cluster_info.h \
			code_generator.cpp code_generator.h \
			compiler_util.cpp compiler_util.h \
			cpp_generator.cpp cpp_generator.h \
			event_info.h \
			file.cpp file.h \
			global_info.h \
			indent.cpp indent.h \
			info_visitor.cpp info_visitor.h \
			lexer.lpp lexer.h \
			list_sep.h \
			literals.cpp literals.h \
			main.cpp \
			mangle.cpp mangle.h \
			param_data.cpp param_data.h \
			parent_info.h \
			parser.ypp \
			progenitor.h \
			semantic.cpp semantic.h \
			serial_number.h \
			set_info.h \
			state_info.h \
			symbol.cpp symbol.h \
			transition_info.h \
			user_event_info.h \
			xxx_info.cpp

if ENABLE_JAVA
chsmc_SOURCES +=	java_generator.cpp java_generator.h
endif

BUILT_SOURCES =		parser.cpp parser.hpp lexer.cpp

lang_parser_test_SOURCES = \
			$(COMMON_SOURCES) \
			lang_parser_test.cpp

# vim:set noet sw=8 ts=8:
