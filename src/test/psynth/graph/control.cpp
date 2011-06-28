/**
 *  Time-stamp:  <2011-06-29 01:05:12 raskolnikov>
 *
 *  @file        control.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 27 19:30:40 2011
 *
 *  @brief Control unit tests.
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

#include <complex>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/sink_node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/control.hpp>
#include <psynth/new_graph/core/patch.hpp>

using namespace psynth::graph;

BOOST_AUTO_TEST_SUITE(graph_control_test_suite);

template <class T>
struct control_node : public sink_node
{
    in_control<T>  in;
    out_control<T> out;

    control_node (T val)
        : in ("input", this, val)
        , out ("output", this, val)
    {}
};


BOOST_AUTO_TEST_CASE(test_in_control_fundamental_noattach)
{
    in_control<int> ctl ("test", 0, 0);
    ctl.set (1);
    BOOST_CHECK_EQUAL (ctl.get (), 1);
    BOOST_CHECK_EQUAL (ctl.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_in_control_non_fundamental_noattach)
{
    typedef std::complex<int> ivec2;
    in_control<ivec2> ctl ("test", 0, ivec2 (0, 0));
    ctl.set (ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.str (), std::string ("(1,2)"));
}

BOOST_AUTO_TEST_CASE(test_out_control_fundamental_noattach)
{
    full_process_context ctx;
    out_control<int> ctl ("test", 0, 0);
    ctl.rt_set (1, ctx);
    BOOST_CHECK_EQUAL (ctl.get (), 1);
    BOOST_CHECK_EQUAL (ctl.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_out_control_non_fundamental_noattach)
{
    full_process_context ctx;
    typedef std::complex<int> ivec2;
    out_control<ivec2> ctl ("test", 0, ivec2 (0, 0));
    ctl.rt_set (ivec2 (1,2), ctx);
    BOOST_CHECK_EQUAL (ctl.get (), ivec2 (0,0));
    BOOST_CHECK_EQUAL (ctl.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.str (), std::string ("(0,0)"));
}



BOOST_AUTO_TEST_CASE(test_in_control_fundamental_semiattach)
{
    control_node<int> ctl (0);
    ctl.in.set (1);
    BOOST_CHECK_EQUAL (ctl.in.get (), 1);
    BOOST_CHECK_EQUAL (ctl.in.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl.in.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_in_control_non_fundamental_semiattach)
{
    typedef std::complex<int> ivec2;
    control_node<ivec2> ctl (ivec2 (0, 0));
    ctl.in.set (ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.in.get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.in.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.in.str (), std::string ("(1,2)"));
}

BOOST_AUTO_TEST_CASE(test_out_control_fundamental_semiattach)
{
    full_process_context ctx;
    control_node<int> ctl (0);
    ctl.out.rt_set (1, ctx);
    BOOST_CHECK_EQUAL (ctl.out.get (), 1);
    BOOST_CHECK_EQUAL (ctl.out.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl.out.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_out_control_non_fundamental_semiattach)
{
    full_process_context ctx;
    typedef std::complex<int> ivec2;
    control_node<ivec2> ctl (ivec2 (0, 0));
    ctl.out.rt_set (ivec2 (1,2), ctx);
    BOOST_CHECK_EQUAL (ctl.out.get (), ivec2 (0,0));
    BOOST_CHECK_EQUAL (ctl.out.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl.out.str (), std::string ("(0,0)"));
}



BOOST_AUTO_TEST_CASE(test_in_control_fundamental_attach)
{
    auto ctl = std::make_shared<control_node<int>> (0);

    processor p;
    p.root ()->add (ctl);
    p.start ();
    p.rt_request_process ();
    ::usleep (1 << 10);

    ctl->in.set (1);

    p.rt_request_process ();
    ::usleep (1 << 10);

    BOOST_CHECK_EQUAL (ctl->in.get (), 1);
    BOOST_CHECK_EQUAL (ctl->in.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl->in.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_in_control_non_fundamental_attach)
{
    typedef std::complex<int> ivec2;
    auto ctl = std::make_shared<control_node<ivec2>> (ivec2 (0, 0));

    processor p;
    p.root ()->add (ctl);
    p.start ();
    p.rt_request_process ();
    ::usleep (1 << 10);
    
    ctl->in.set (ivec2 (1,2));

    p.rt_request_process ();
    ::usleep (1 << 10);

    BOOST_CHECK_EQUAL (ctl->in.get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl->in.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl->in.str (), std::string ("(1,2)"));
}

BOOST_AUTO_TEST_CASE(test_out_control_fundamental_attach)
{
    full_process_context ctx;
    auto ctl = std::make_shared<control_node<int>> (0);

    processor p;
    p.root ()->add (ctl);
    p.start ();
    p.rt_request_process ();
    ::usleep (1 << 10);

    ctl->out.rt_set (1, ctx);

    p.rt_request_process ();
    ::usleep (1 << 10);

    BOOST_CHECK_EQUAL (ctl->out.get (), 1);
    BOOST_CHECK_EQUAL (ctl->out.rt_get (), 1);
    BOOST_CHECK_EQUAL (ctl->out.str (), std::string ("1"));
}

BOOST_AUTO_TEST_CASE(test_out_control_non_fundamental_attach)
{
    full_process_context ctx;
    typedef std::complex<int> ivec2;
    auto ctl = std::make_shared<control_node<ivec2>> (ivec2 (0, 0));

    processor p;
    p.root ()->add (ctl);
    p.start ();
    p.rt_request_process ();
    ::usleep (1 << 10);

    ctl->out.rt_set (ivec2 (1,2), ctx);

    p.rt_request_process ();
    ::usleep (1 << 10);

    BOOST_CHECK_EQUAL (ctl->out.get (), ivec2 (0,0));
    BOOST_CHECK_EQUAL (ctl->out.rt_get (), ivec2 (1,2));
    BOOST_CHECK_EQUAL (ctl->out.str (), std::string ("(0,0)"));
}

BOOST_AUTO_TEST_SUITE_END ();

