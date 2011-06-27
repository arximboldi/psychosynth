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

dist_TESTS = utf_test_driver

TESTS = $(dist_TESTS)

EXTRA_DIST = $(dist_TESTS)

noinst_PROGRAMS = \
	psynth_unit_test \
	example_graph_scale \
	example_graph_patch

examples_cxxflags = \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src
examples_ldadd = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SIGNALS_LIB) \
	-lboost_system-mt \
	../psynth/libpsynth.la	

example_graph_scale_SOURCES = examples/graph_scale.cpp
example_graph_scale_CXXFLAGS = $(examples_cxxflags)
example_graph_scale_LDADD = $(examples_ldadd)

example_graph_patch_SOURCES = examples/graph_patch.cpp
example_graph_patch_CXXFLAGS = $(examples_cxxflags)
example_graph_patch_LDADD = $(examples_ldadd)

psynth_unit_test_SOURCES = \
	psynth_unit_test.cpp

psynth_unit_test_SOURCES += \
	psynth/base/tree_node.cpp \
	psynth/base/arg_parser.cpp \
	psynth/base/c3_class.cpp \
	psynth/base/exception.cpp \
	psynth/base/hetero_deque.cpp \
	psynth/base/factory.cpp \
	psynth/sound/sample.cpp \
	psynth/sound/frame.cpp \
	psynth/sound/sample_buffer.cpp \
	psynth/sound/buffer.cpp \
	psynth/sound/performance.cpp \
	psynth/sound/frame_iterator.cpp \
	psynth/sound/ring.cpp \
	psynth/io/output.cpp \
	psynth/io/input.cpp \
	psynth/graph/processor.cpp \
	psynth/graph/core.cpp \
	psynth/graph/port.cpp \
	psynth/graph/control.cpp \
	psynth/graph/patch.cpp \
	psynth/util.cpp \
	psynth/util.hpp

psynth_unit_test_CXXFLAGS = -Wall $(BOOST_CPPFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src

psynth_unit_test_LDFLAGS  = $(BOOST_LDFLAGS)

psynth_unit_test_LDADD = \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SIGNALS_LIB) \
	-lboost_system-mt \
	../psynth/libpsynth.la


endif # HAVE_BOOST_TEST
