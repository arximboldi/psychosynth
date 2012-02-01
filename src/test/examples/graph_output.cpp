/**
 *  Time-stamp:  <2012-02-01 23:14:00 raskolnikov>
 *
 *  @file        graph_soft.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun 29 11:39:45 2011
 *
 *  @brief Example that tests the soft buffers proper behaviour.
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

#define PSYNTH_MODULE_NAME "example_graph_perf"

#include <random>
#include <cstdlib>

#include <psynth/base/logger.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/io/alsa_output.hpp>
#include <psynth/io/buffered_output.hpp>
#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>
#include <psynth/new_graph/core/async_output.hpp>

using namespace psynth;

graph::core::patch_ptr make_output_patch (const char* device_name)
{
    auto& factory = graph::node_factory::self ();
    auto p = std::dynamic_pointer_cast<graph::core::patch> (
        factory.create ("patch"));

    auto out = p->add (
        graph::core::new_async_output (
            io::new_buffered_async_output<
                graph::audio_range,
                io::alsa_output<sound::stereo16sc_range> >(
                    device_name, 1, 128, 44100)));
    auto mixer = p->add (factory.create ("audio_mixer"));

    auto in1 = p->add (factory.create ("audio_patch_soft_in_port"));
    in1->param ("port-name").set<std::string> ("in-one");

    auto in2 = p->add (factory.create ("audio_patch_soft_in_port"));
    in2->param ("port-name").set<std::string> ("in-two");
    mixer->in ("input-0").connect (in1->out ("output"));
    mixer->in ("input-1").connect (in2->out ("output"));
    
    out->in ("input").connect (mixer->out ("output"));
    
    return p;
}

std::mt19937 random_engine;
std::normal_distribution<float> freq_dist (440.0f, 200.0f);
std::normal_distribution<float> lfo_dist (2.0f, 1); 

auto freq_gen = std::bind (
    base::clamp<float>,
    std::bind (freq_dist, random_engine),
    60.0f, 60.0f * (1 << 7));

auto lfo_gen = std::bind (
    base::clamp<float>,
    std::bind (lfo_dist, random_engine),
    .1f, 30.0f);

struct synth_patch
{
    graph::core::patch_ptr _patch;
    graph::node_ptr  _osc;
    graph::node_ptr  _mod;

    synth_patch ()
    {
        auto& factory = graph::node_factory::self ();
        _patch = std::dynamic_pointer_cast<graph::core::patch> (
            factory.create ("patch"));
        _osc = _patch->add (factory.create ("audio_sine_oscillator"));
        _mod = _patch->add (factory.create ("sample_sine_oscillator"));

        auto out = _patch->add (factory.create ("audio_patch_out_port"));
        out->in ("input").connect (_osc->out ("output"));

        update_params ();
        // update_mod ();
        graph::connect (_mod, "output", _osc, "modulator");
    }

    void update_mod ()
    {
        if (rand () & 1)
        {
            auto& p = _osc->in ("modulator");
            if (p.connected ())
            {
                PSYNTH_LOG << "Modulator OFF";
                p.disconnect ();
            }
            else
            {
                PSYNTH_LOG << "Modulator ON";
                p.connect (_mod->out ("output"));
            }
        }
    }
    
    void update_params ()
    {
        float new_freq = freq_gen ();
        float new_lfo = lfo_gen ();
        PSYNTH_LOG << "New params: " << new_freq << " " << new_lfo;
        _osc->param ("frequency").set (new_freq);
        _mod->param ("frequency").set (new_lfo);
    }
};

int main ()
{
    base::logger::self ().add_sink (base::new_log_std_sink ());
        
    graph::processor p;
    auto root     = p.root ();

    synth_patch synth_one;
    synth_patch synth_two;
    
    auto out1 = root->add (make_output_patch ("hw:0,0"));
    auto out2 = root->add (make_output_patch ("hw:1,0"));

    root->add (synth_one._patch);
    root->add (synth_two._patch);

    graph::connect (synth_one._patch, "output", out1, "in-one");
    graph::connect (synth_two._patch, "output", out1, "in-two");
    graph::connect (synth_one._patch, "output", out2, "in-one");
    
    p.start ();

    ::sleep (3600);
    
    p.stop ();

    return 0;
}

