/**
 *  Time-stamp:  <2011-06-16 17:29:17 raskolnikov>
 *
 *  @file        output_node.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 21:36:45 2011
 *
 *  @brief Special output node.
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

#ifndef PSYNTH_GRAPH_CORE_ASYNC_OUTPUT_NODE_HPP_
#define PSYNTH_GRAPH_CORE_ASYNC_OUTPUT_NODE_HPP_

#include <psynth/io/output_fwd.hpp>

#include <psynth/new_graph/control.hpp>
#include <psynth/new_graph/buffer_port.hpp>
#include <psynth/new_graph/sink_node.hpp>
#include <psynth/new_graph/process_node.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_DECLARE_ERROR (error, async_output_not_bound_error);

/**
 *  A node for outputing using a given device. The device is supposed
 *  to actively request new frames when needed using the process_node
 *  interface.
 *
 *  @note Once the device is bound to the output node, one should not
 *  use it directly, as that can lead to inconsistent state.
 */
class async_output : public sink_node
                   , public process_node
{
public:
    typedef io::async_output_ptr<audio_range> device_ptr;
    
    async_output (device_ptr out = device_ptr ());
    void set_output (device_ptr out = device_ptr ());
    
    void start ();
    void stop ();
    void rt_do_process (rt_process_context& ctx);
    
private:
    void _output_callback (std::size_t nframes);

    audio_in_port _in_input;
    
    // FIXME: Actually there is unnecesary buffering here, because we
    // buffer first to accumulate the output, then to convert it.
    
    device_ptr         _output;
    audio_ring_buffer  _buffer;
    audio_ring_buffer::range::position _pos;
};

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_ASYNC_OUTPUT_NODE_HPP_ */

