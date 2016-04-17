/**
 *  Time-stamp:  <2011-06-29 01:01:09 raskolnikov>
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

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/sink_node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>

using namespace psynth::graph;

struct counting_node : public node
{
    int count;
    int init;
    counting_node (int count_ = 0) : count (count_) {}
    void rt_on_context_update (rt_process_context& ctx)
    { ++init; }
    void rt_do_process (rt_process_context& ctx)
    { ++count; }
};

struct counting_sink : public sink_node
{
    int count;
    int init;
    counting_sink (int count_ = 0) : count (count_) {}
    void rt_on_context_update (rt_process_context& ctx)
    { ++init; }
    void rt_do_process (rt_process_context& ctx)
    { ++count; }
};

BOOST_AUTO_TEST_SUITE(graph_processor_test_suite);

BOOST_AUTO_TEST_CASE(test_processor)
{
    processor p;
    p.rt_request_process ();
    BOOST_CHECK (true);
}

BOOST_AUTO_TEST_CASE(test_processor_not_sink)
{
    processor p;

    std::shared_ptr<counting_node> n = std::make_shared<counting_node> ();
    BOOST_CHECK_THROW (n->check_attached_to_patch (),
                       node_attachment_error);
    BOOST_CHECK_THROW (n->check_attached_to_process (),
                       node_attachment_error);

    p.root ()->add (n);
    BOOST_CHECK_NO_THROW (n->check_attached_to_patch ());
    BOOST_CHECK_NO_THROW (n->check_attached_to_process ());
    p.rt_request_process ();

    BOOST_CHECK_EQUAL (n->count, 0);
}

BOOST_AUTO_TEST_CASE(test_processor_sink)
{
    processor p;

    std::shared_ptr<counting_sink> n = std::make_shared<counting_sink> ();
    BOOST_CHECK_THROW (n->check_attached_to_patch (),
                       node_attachment_error);
    BOOST_CHECK_THROW (n->check_attached_to_process (),
                       node_attachment_error);

    p.root ()->add (n);
    BOOST_CHECK_NO_THROW (n->check_attached_to_patch ());
    BOOST_CHECK_NO_THROW (n->check_attached_to_process ());

    p.rt_request_process ();
    p.rt_request_process ();
    p.rt_request_process ();
    BOOST_CHECK_EQUAL (n->count, 3);
}

BOOST_AUTO_TEST_CASE(test_processor_init)
{
    processor p;

    std::shared_ptr<counting_sink> n = std::make_shared<counting_sink> ();
    BOOST_CHECK_EQUAL (n->init, 0);
    p.root ()->add (n);
    BOOST_CHECK_EQUAL (n->init, 1);
}

BOOST_AUTO_TEST_CASE(test_processor_rt_event)
{
    processor p;
    int var = 0;
    p.context ().push_rt_event (
        make_rt_event ([&] (rt_process_context&) {
                var++;
            }));
    BOOST_CHECK_EQUAL (var, 0);
    p.rt_request_process ();
    BOOST_CHECK_EQUAL (var, 1);
}

BOOST_AUTO_TEST_CASE(test_processor_rt_event_two)
{
    processor p;
    int var = 0;

    p.context ().push_rt_event (
        make_rt_event ([&] (rt_process_context&) {
                var++;
            }));

    BOOST_CHECK_EQUAL (var, 0);
    p.start ();
    for (int i = 0; i < 3; ++i) {
        p.context ().push_rt_event (
            make_rt_event ([&] (rt_process_context&) {
                    var++;
                }));
        p.rt_request_process ();
        ::usleep (1 << 10);
    }

    BOOST_CHECK_EQUAL (var, 4);
}

BOOST_AUTO_TEST_CASE(test_processor_async_event)
{
    processor p;
    int var = 0;
    p.context ().push_async_event (
        make_async_event ([&] (async_process_context&) {
                var++;
            }));
    BOOST_CHECK_EQUAL (var, 0);
    p.rt_request_process ();
    BOOST_CHECK_EQUAL (var, 0);
}

BOOST_AUTO_TEST_CASE(test_processor_async_event_two)
{
    processor p;
    int var = 0;

    p.context ().push_async_event (
        make_async_event ([&] (async_process_context&) {
                var++;
            }));

    BOOST_CHECK_EQUAL (var, 0);
    p.start ();
    for (int i = 0; i < 3; ++i) {
        p.context ().push_async_event (
            make_async_event ([&] (async_process_context&) {
                    var++;
                }));
        ::usleep (1 << 10);
    }
    p.stop ();
    BOOST_CHECK_EQUAL (var, 4);
}

BOOST_AUTO_TEST_CASE (test_processor_errors)
{
    processor p;

    BOOST_CHECK_THROW (
        p.stop (), processor_not_running_error);
    p.start ();
    BOOST_CHECK_THROW (
        p.start (), processor_not_idle_error);

    // Should stop on destroy
}

BOOST_AUTO_TEST_SUITE_END ();
