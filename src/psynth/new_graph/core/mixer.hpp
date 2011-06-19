/**
 *  Time-stamp:  <2011-06-18 21:29:01 raskolnikov>
 *
 *  @file        mixer.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 17 18:44:32 2011
 *
 *  @brief Mixer node.
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

#ifndef PSYNTH_GRAPH_CORE_MIXER_HPP_
#define PSYNTH_GRAPH_CORE_MIXER_HPP_

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/control.hpp>
#include <psynth/new_graph/soft_buffer_port.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

template <class Buffer>
class mixer : public node
{
public:
    mixer ();
    
protected:
    void rt_do_process (rt_process_context& ctx);

    typedef soft_buffer_in_port<Buffer> in_port_type;
    typedef std::shared_ptr<in_port_type> in_port_ptr;
    typedef buffer_out_port<Buffer> out_port_type;
    typedef std::vector <in_port_ptr> in_port_vector;

    soft_sample_in_port _in_modulator;
    in_port_vector _in_inputs; 
    out_port_type _out_output;

    in_control<float> _ctl_gain;
    in_control<int>   _ctl_inputs;
};

typedef mixer<audio_buffer> audio_mixer;
typedef mixer<sample_buffer> sample_mixer;

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_MIXER_HPP_ */

