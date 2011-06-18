/**
 *  Time-stamp:  <2011-06-17 15:42:23 raskolnikov>
 *
 *  @file        oscillator.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun 16 20:56:21 2011
 *
 *  @brief Oscillator node kinds.
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

#ifndef PSYNTH_GRAPH_CORE_OSCILLATOR_HPP_
#define PSYNTH_GRAPH_CORE_OSCILLATOR_HPP_

#include <psynth/synth/oscillator.hpp>
#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/buffer_port.hpp>
#include <psynth/new_graph/control.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

/**
 *  Oscillator node.
 *
 *  Output:
 *    "output" : Output
 *
 *  Input:
 *    "modulator" : sample_buffer
 *
 *  Params:
 *    "frequency" : float
 *    "amplitude" : float
 *    "modulator" : int (0 : am, 1 : fm, 2 : pm)
 *
 *  @todo The audio/sample kind of input difference should finish with
 *  automagic buffer conversion.
 */
template <class Generator, class Output>
class oscillator : public node
{
public:
    oscillator ();
    
protected:
    void rt_on_context_update (rt_process_context& ctx);
    void rt_do_process (rt_process_context& ctx);
    
    Output _out_output;
    sample_in_port _in_modulator;
    
    in_control<float> _ctl_frequency;
    in_control<float> _ctl_amplitude;
    in_control<int>   _ctl_modulator;
    
    synth::oscillator<Generator> _osc;
};

typedef oscillator<synth::sine_generator, audio_out_port>
audio_sine_oscillator;
typedef oscillator<synth::square_generator, audio_out_port>
audio_square_oscillator;
typedef oscillator<synth::triangle_generator, audio_out_port>
audio_triangle_oscillator;
typedef oscillator<synth::sawtooth_generator, audio_out_port>
audio_sawtooth_oscillator;
typedef oscillator<synth::moogsaw_generator, audio_out_port>
audio_moogsaw_oscillator;
typedef oscillator<synth::exp_generator, audio_out_port>
audio_exp_oscillator;

typedef oscillator<synth::sine_generator, sample_out_port>
sample_sine_oscillator;
typedef oscillator<synth::square_generator, sample_out_port>
sample_square_oscillator;
typedef oscillator<synth::triangle_generator, sample_out_port>
sample_triangle_oscillator;
typedef oscillator<synth::sawtooth_generator, sample_out_port>
sample_sawtooth_oscillator;
typedef oscillator<synth::moogsaw_generator, sample_out_port>
sample_moogsaw_oscillator;
typedef oscillator<synth::exp_generator, sample_out_port>
sample_exp_oscillator;

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_OSCILLATOR_HPP_ */

