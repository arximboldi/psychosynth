/**
 *  Time-stamp:  <2012-02-05 08:27:47 raskolnikov>
 *
 *  @file        noise.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Feb  4 22:53:52 2012
 *
 *  @brief Noise generator.
 */

/*
 *  Copyright (C) 2012 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_GRAPH_CORE_NOISE_HPP_
#define PSYNTH_GRAPH_CORE_NOISE_HPP_

#include <psynth/synth/noise.hpp>

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/soft_buffer_port.hpp>
#include <psynth/new_graph/control.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

template <template<class> class Distribution, class Output>
class noise : public node
{
public:
    noise ();
    
protected:
    void rt_do_process (rt_process_context& ctx);
    
    Output               _out_output;
    soft_sample_in_port  _in_modulator;
    in_control<float>    _ctl_amplitude;

    typedef typename sound::sample_type<
        typename Output::port_type>::type sample_type;
    synth::noise<Distribution<sample_type> > _noise;
};

typedef noise<synth::white_noise_distribution, audio_out_port>  audio_white_noise;
typedef noise<synth::pink_noise_distribution,  audio_out_port>  audio_pink_noise;
typedef noise<synth::white_noise_distribution, sample_out_port> sample_white_noise;
typedef noise<synth::pink_noise_distribution,  sample_out_port> sample_pink_noise;


} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_NOISE_HPP_ */

