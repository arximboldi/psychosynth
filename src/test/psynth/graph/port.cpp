/**
 *  Time-stamp:  <2011-06-27 23:16:10 raskolnikov>
 *
 *  @file        port.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 27 19:30:19 2011
 *
 *  @brief Ports unit tests.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/sink_node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>

using namespace psynth::graph;

BOOST_AUTO_TEST_SUITE(graph_port_test_suite);

BOOST_AUTO_TEST_CASE(test_port_todo)
{
    BOOST_CHECK (1);
}

BOOST_AUTO_TEST_SUITE_END ();
