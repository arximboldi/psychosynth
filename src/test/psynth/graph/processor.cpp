/**
 *  Time-stamp:  <2011-06-18 23:51:27 raskolnikov>
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

const char* test_alsa_device = "default";

#if 0

BOOST_AUTO_TEST_CASE (test_processor_sin_mod_example)
{
    using namespace psynth;
    
    graph::processor p;
    auto root     = p.root ();
    auto& factory = graph::node_factory::self ();
    
    auto out = root->add (
        graph::core::new_async_output (
            io::new_buffered_async_output<
                graph::audio_range,
                io::alsa_output<sound::stereo16sc_range> >(
                    test_alsa_device, 1024, 44100)));
    
    auto osc = root->add (factory.create ("audio_sine_oscillator"));
    auto mod = root->add (factory.create ("sample_sine_oscillator"));

    osc->in ("modulator").connect (mod->out ("output"));
    out->in ("input").connect (osc->out ("output"));
    
    BOOST_CHECK (out->in ("input").connected ());
    BOOST_CHECK (osc->out ("output").connected ());
    
    osc->param ("modulator").set<int> (1);
    mod->param ("frequency").set<float> (4.0f);
    mod->param ("amplitude").set<float> (0.3f);
    auto& freq = osc->param ("frequency");
    freq.set<float> (60);
    p.start ();
    for (int i = 0; i < 5; ++i)
    {
        freq.set<float> (freq.get<float> () * 2);
        ::sleep (1);
    }
    
    p.stop ();
}

#endif

BOOST_AUTO_TEST_CASE (test_processor_patch_example)
{
    using namespace psynth;
    
    graph::processor p;
    auto root     = p.root ();
    auto& factory = graph::node_factory::self ();

    auto patch = graph::core::new_patch ();
    auto mixer = patch->add (factory.create ("audio_mixer"));
    auto out   = patch->add (
        graph::core::new_async_output (
            io::new_buffered_async_output<
                graph::audio_range,
                io::alsa_output<sound::stereo16sc_range> >(
                    test_alsa_device, 256, 44100)));

    auto in1 = patch->add (factory.create ("audio_patch_soft_in_port"));
    in1->param ("port-name").set<std::string> ("mix-in-one");
    
    mixer->in ("input-0").connect (in1->out ("output"));
    out->in ("input").connect (mixer->out ("output"));
    
    root->add (patch);
    auto osc = root->add (factory.create ("audio_sine_oscillator"));
    auto osc2 = patch->add (factory.create ("audio_sine_oscillator"));
        
    p.start ();
    ::sleep (1);
    patch->in ("mix-in-one").connect (osc->out ("output"));
    //mixer->in ("input-2").connect (osc2->out ("output"));
    ::sleep (2);
    auto in2 = factory.create ("audio_patch_soft_in_port");
    in2->param ("port-name").set<std::string> ("mix-in-two");
    patch->add (in2);
    patch->in ("mix-in-one").disconnect ();
    patch->in ("mix-in-two").connect (osc->out ("output"));
    mixer->in ("input-1").connect (in2->out ("output"));
    BOOST_CHECK_THROW (in2->param ("port-name").str ("mix-in-one"),
                       graph::node_component_error);
    ::sleep (1);
    mixer->in ("input-1").disconnect ();
    ::sleep (1);
    p.stop ();
}

BOOST_AUTO_TEST_SUITE_END ();
