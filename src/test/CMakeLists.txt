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

endif(BUILD_TESTS)
