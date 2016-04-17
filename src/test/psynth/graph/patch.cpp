/**
 *  Time-stamp:  <2011-06-29 01:05:37 raskolnikov>
 *
 *  @file        patch.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 27 19:31:02 2011
 *
 *  @brief Patch unit tests.
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
#include <psynth/new_graph/core/passive_output.hpp>

using namespace psynth::graph;

BOOST_AUTO_TEST_SUITE(graph_patch_test_suite);

BOOST_AUTO_TEST_CASE (patch_in_port_noattach)
{
    using namespace psynth;

    auto& factory = node_factory::self ();
    auto root = graph::core::new_patch ();
    auto patch = graph::core::new_patch ();

    auto mixer = patch->add (factory.create ("audio_mixer"));
    auto in1 = patch->add (factory.create ("audio_patch_soft_in_port"));
    BOOST_CHECK_NO_THROW (patch->in ("input"));

    in1->param ("port-name").set<std::string> ("mix-in-one");
    BOOST_CHECK_THROW (patch->in ("input"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->in ("mix-in-one"));
    mixer->in ("input-0").connect (in1->out ("output"));

    root->add (patch);
    auto osc = root->add (factory.create ("audio_sine_oscillator"));

    BOOST_CHECK_NO_THROW (
        patch->in ("mix-in-one").connect (osc->out ("output")));
    in1->param("port-name").str ("mix-in-two");
    BOOST_CHECK_THROW (patch->in ("mix-in-one"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->in ("mix-in-two"));
}

BOOST_AUTO_TEST_CASE (patch_out_port_noattach)
{
    using namespace psynth;

    auto& factory = node_factory::self ();
    auto root = graph::core::new_patch ();
    auto patch = graph::core::new_patch ();

    auto mixer = patch->add (factory.create ("audio_mixer"));
    auto in1 = patch->add (factory.create ("audio_patch_soft_out_port"));
    BOOST_CHECK_NO_THROW (patch->out ("output"));

    in1->param ("port-name").set<std::string> ("mix-out");
    BOOST_CHECK_THROW (patch->out ("output"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->out ("mix-out"));
    in1->in ("input").connect (mixer->out ("output"));

    root->add (patch);
    auto osc = root->add (core::new_passive_output ());

    BOOST_CHECK_NO_THROW (osc->in ("input").connect (patch->out ("mix-out")));
    in1->param("port-name").str ("mix-out-wtf");
    BOOST_CHECK_THROW (patch->out ("mix-out"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->out ("mix-out-wtf"));
}


BOOST_AUTO_TEST_CASE (patch_in_port_attach)
{
    using namespace psynth;

    processor p;
    auto root = p.root ();
    auto& factory = node_factory::self ();
    auto patch = graph::core::new_patch ();

    p.start ();

    auto mixer = patch->add (factory.create ("audio_mixer"));
    auto in1 = patch->add (factory.create ("audio_patch_soft_in_port"));
    BOOST_CHECK_NO_THROW (patch->in ("input"));

    p.rt_request_process ();

    in1->param ("port-name").set<std::string> ("mix-in-one");
    BOOST_CHECK_THROW (patch->in ("input"), node_component_error);
    p.rt_request_process ();
    BOOST_CHECK_NO_THROW (patch->in ("mix-in-one"));
    mixer->in ("input-0").connect (in1->out ("output"));

    p.rt_request_process ();

    root->add (patch);
    auto osc = root->add (factory.create ("audio_sine_oscillator"));

    p.rt_request_process ();

    BOOST_CHECK_NO_THROW (
        patch->in ("mix-in-one").connect (osc->out ("output")));
    in1->param("port-name").str ("mix-in-two");
    BOOST_CHECK_THROW (patch->in ("mix-in-one"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->in ("mix-in-two"));
}

BOOST_AUTO_TEST_CASE (patch_out_port_attach)
{
    using namespace psynth;

    processor p;
    auto root = p.root ();
    auto& factory = node_factory::self ();
    auto patch = graph::core::new_patch ();

    p.start ();

    auto mixer = patch->add (factory.create ("audio_mixer"));
    auto in1 = patch->add (factory.create ("audio_patch_soft_out_port"));
    BOOST_CHECK_NO_THROW (patch->out ("output"));

    p.rt_request_process ();

    in1->param ("port-name").set<std::string> ("mix-out");
    BOOST_CHECK_THROW (patch->out ("output"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->out ("mix-out"));
    in1->in ("input").connect (mixer->out ("output"));

    p.rt_request_process ();

    root->add (patch);
    auto osc = root->add (core::new_passive_output ());

    BOOST_CHECK_NO_THROW (osc->in ("input").connect (patch->out ("mix-out")));
    in1->param("port-name").str ("mix-out-wtf");

    p.rt_request_process ();

    BOOST_CHECK_THROW (patch->out ("mix-out"), node_component_error);
    BOOST_CHECK_NO_THROW (patch->out ("mix-out-wtf"));
}

BOOST_AUTO_TEST_SUITE_END ();
