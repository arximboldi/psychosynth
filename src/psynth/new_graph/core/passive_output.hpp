/**
 *  Time-stamp:  <2011-06-16 17:29:34 raskolnikov>
 *
 *  @file        passive_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun 15 17:26:31 2011
 *
 *  @brief Pasive output header.
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

#ifndef PSYNTH_GRAPH_CORE_PASSIVE_OUTPUT_HPP_
#define PSYNTH_GRAPH_CORE_PASSIVE_OUTPUT_HPP_

#include <psynth/io/output_fwd.hpp>

#include <psynth/new_graph/control.hpp>
#include <psynth/new_graph/buffer_port.hpp>
#include <psynth/new_graph/sink_node.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

/**
 *  A node for outputing using a given output system passively -
 *  i.e. it directly dumps all the data it receives on every updated,
 *  but will not request processing by itself.
 *
 *  @note Once the device is bound to the output node, one should not
 *  use it directly, as that can lead to inconsistent state.
 */
class passive_output : public sink_node
{
public:
    typedef io::output_ptr<audio_range> device_ptr;
    
    passive_output (device_ptr out = device_ptr ());
    void set_output (device_ptr out = device_ptr ());
    void rt_do_process (rt_process_context& ctx);
    
private:
    audio_in_port      _in_input;
    device_ptr         _output;
};

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_PASSIVE_OUTPUT_HPP_ */

