/**
 *  Time-stamp:  <2011-06-29 13:28:09 raskolnikov>
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

#define PSYNTH_MODULE_NAME "example_graph_soft"

#include <random>
#include <cstdlib>

#include <psynth/base/logger.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/io/file_output.hpp>
#include <psynth/io/buffered_output.hpp>
#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/core/patch.hpp>
#include <psynth/new_graph/core/passive_output.hpp>

const char* test_alsa_device = "default";

using namespace psynth;

graph::node_ptr make_file_output (const char* fname)
{
    return graph::core::new_passive_output (
        io::new_buffered_output<
            graph::audio_range,
            io::file_output<sound::stereo16sc_range> >(
                fname, io::file_fmt::wav, 44100));
}

graph::node_ptr make_output_patch ()
{
    auto& factory = graph::node_factory::self ();
    
    auto patch = graph::core::new_patch ();

    auto out1 = patch->add (make_file_output ("graph-soft-1.wav"));
    auto out2 = patch->add (make_file_output ("graph-soft-2.wav"));
    auto out3 = patch->add (make_file_output ("graph-soft-3.wav"));

    auto mixer = patch->add (factory.create ("audio_mixer"));
    mixer->param ("gain").set (1.0f);
    
    auto in0 = patch->add (factory.create ("audio_patch_soft_in_port"));
    in0->param ("port-name").str ("input-0");
    auto in1 = patch->add (factory.create ("audio_patch_soft_in_port"));
    in1->param ("port-name").str ("input-1");
        
    graph::connect (mixer, "output", out2, "input");
    graph::connect (in0, "output", out1, "input");
    graph::connect (in1, "output", out3, "input");
    
    graph::connect (in0, "output", mixer, "input-0");
    graph::connect (in1, "output", mixer, "input-1");


    return patch;
}
    
int main ()
{
    base::logger::self ().add_sink (base::new_log_std_sink ());
    auto& factory = graph::node_factory::self ();
    
    graph::processor p;
    auto root     = p.root ();
    auto out = root->add (make_output_patch ());
    auto osc = root->add (factory.create ("audio_sine_oscillator"));
    osc->param ("frequency").set (880.0f);
    
    graph::connect (osc, "output", out, "input-0");

    p.rt_request_process (16);

    osc->out ("output").disconnect ();
    graph::connect (osc, "output", out, "input-1");

    p.rt_request_process (16);

    return 0;
}
