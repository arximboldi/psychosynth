/**
 *  Time-stamp:  <2012-02-01 23:12:35 raskolnikov>
 *
 *  @file        graph_scale.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 27 19:32:54 2011
 *
 *  @brief Graph example program playing an scale.
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

    auto out = root->add (
        graph::core::new_async_output (
            io::new_buffered_async_output<
                graph::audio_range,
                io::alsa_output<sound::stereo16sc_range> >(
                    test_alsa_device, 2, 512, 44100)));

    auto osc = root->add (factory.create ("audio_sine_oscillator"));
    auto mod = root->add (factory.create ("sample_sine_oscillator"));

    osc->in ("modulator").connect (mod->out ("output"));
    out->in ("input").connect (osc->out ("output"));

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


    return 0;
}
