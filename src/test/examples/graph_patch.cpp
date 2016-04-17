/**
 *  Time-stamp:  <2012-02-01 23:13:11 raskolnikov>
 *
 *  @file        graph_patch.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 27 19:34:35 2011
 *
 *  @brief Graph patch example program.
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

#include <psynth/io/alsa_output.hpp>
#include <psynth/io/buffered_output.hpp>
#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>
#include <psynth/new_graph/core/async_output.hpp>

const char* test_alsa_device = "default";

int main ()
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
                    test_alsa_device, 2, 128, 44100)));

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
    ::sleep (1);
    mixer->in ("input-1").disconnect ();
    ::sleep (1);
    p.stop ();

    return 0;
}
