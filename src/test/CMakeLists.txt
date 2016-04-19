#
#   PSYCHOSYNTH
#   ===========
#
#   Copyright (C) 2007, 2008, 2010, 2016 by Juan Pedro Bolivar Puente
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

if (BUILD_TESTS)

  #  Examples
  #  ===================================================================

  function(add_example name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PUBLIC psynth)
  endfunction(add_example)

  add_example(example-graph-patch examples/graph_patch.cpp)
  add_example(example-graph-perf examples/graph_perf.cpp)
  add_example(example-graph-soft examples/graph_soft.cpp)
  add_example(example-graph-output examples/graph_output.cpp)

  #  Unit tests
  #  ===================================================================

  add_executable(psynth-unit-tests
    psynth_unit_test.cpp
    psynth/base/tree_node.cpp
    psynth/base/arg_parser.cpp
    psynth/base/c3_class.cpp
    psynth/base/exception.cpp
    psynth/base/hetero_deque.cpp
    psynth/base/factory.cpp
    psynth/sound/sample.cpp
    psynth/sound/frame.cpp
    psynth/sound/sample_buffer.cpp
    psynth/sound/buffer.cpp
    psynth/sound/performance.cpp
    psynth/sound/frame_iterator.cpp
    psynth/sound/ring.cpp
    psynth/io/output.cpp
    psynth/io/input.cpp
    psynth/graph/processor.cpp
    psynth/graph/core.cpp
    psynth/graph/port.cpp
    psynth/graph/control.cpp
    psynth/graph/patch.cpp
    psynth/util.cpp
    psynth/util.hpp)
  target_link_libraries(psynth-unit-tests PUBLIC psynth)
  add_test(psynth-unit-tests psynth-unit-tests)

endif(BUILD_TESTS)
