#
#   PSYCHOSYNTH
#   ===========
#              
#   Copyright (C) 2007, 2008, 2010 by Juan Pedro Bolivar Puente
#                                                        
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#                                      
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

if HAVE_BOOST_TEST

TESTS_ENVIRONMENT = ./utf_test_driver

TESTS = psynth_unit_test

noinst_PROGRAMS = $(TESTS)

psynth_unit_test_SOURCES = \
	psynth/base/tree_node.cpp \
	psynth/base/arg_parser.cpp \
	psynth/sound/sample.cpp \
	psynth/sound/frame.cpp \
	psynth/sound/sample_buffer.cpp \
	psynth/sound/buffer.cpp \
	psynth/sound/performance.cpp \
	psynth/sound/frame_iterator.cpp \
	psynth/sound/ring.cpp \
	psynth_unit_test.cpp

AM_CPPFLAGS = -Wall $(BOOST_CPPFLAGS) -I..
AM_LDFLAGS  = $(BOOST_LDFLAGS)

psynth_unit_test_LDADD = \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
	$(SIGC_LIBS) \
	../psynth/libpsynth.la

psynth_unit_test_LDFLAGS =

endif # HAVE_BOOST_TEST
