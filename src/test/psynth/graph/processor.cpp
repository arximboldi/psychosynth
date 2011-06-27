/**
 *  Time-stamp:  <2011-06-27 19:35:10 raskolnikov>
 *
 *  @file        processor.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 11 20:02:04 2011
 *
 *  @brief Proccessor unit tests.
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

#include <psynth/io/alsa_output.hpp>
#include <psynth/io/buffered_output.hpp>
#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>
#include <psynth/new_graph/core/async_output.hpp>

BOOST_AUTO_TEST_SUITE(graph_processor_test_suite);

BOOST_AUTO_TEST_CASE(test_processor)
{
    psynth::graph::processor p;
    p.rt_request_process ();
}

BOOST_AUTO_TEST_CASE (test_processor_errors)
{
    psynth::graph::processor p;
    
    BOOST_CHECK_THROW (
        p.stop (), psynth::graph::processor_not_running_error);
    p.start ();
    
    BOOST_CHECK_THROW (
        p.start (), psynth::graph::processor_not_idle_error);
    
    // Should stop on destroy
}

BOOST_AUTO_TEST_SUITE_END ();
