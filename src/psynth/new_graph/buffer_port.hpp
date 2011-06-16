/**
 *  Time-stamp:  <2011-06-16 17:43:51 raskolnikov>
 *
 *  @file        buffer_port.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 13 20:15:25 2011
 *
 *  @brief Ports for audio buffers.
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

#ifndef PSYNTH_GRAPH_BUFFER_PORT_HPP_
#define PSYNTH_GRAPH_BUFFER_PORT_HPP_

#include <psynth/new_graph/processor.hpp>
#include <psynth/new_graph/buffers.hpp>
#include <psynth/new_graph/port.hpp>

namespace psynth
{
namespace graph
{

template <typename T>
class buffer_out_port : public out_port<T>
{
public:
    buffer_out_port (std::string name, node* owner)
        : out_port<T> (name, owner) {}

    typename T::const_range rt_range () const
    { return const_range (this->rt_get ()); }
    typename T::range rt_range ()
    { return range (this->rt_get ()); }
    
    void rt_context_update (const rt_process_context& ctx)
    { this->rt_get ().recreate (ctx.block_size ()); }
};

template <typename T>
class buffer_in_port : public in_port<T>
{
public:
    typename T::const_range rt_range () const
    { return const_range (this->rt_get ()); }
    
    buffer_in_port (std::string name, node* owner)
        : in_port<T> (name, owner) {}
};

typedef buffer_out_port<audio_buffer>  audio_out_port;
typedef buffer_in_port<audio_buffer>   audio_in_port;
typedef buffer_out_port<sample_buffer> sample_out_port;
typedef buffer_in_port<sample_buffer>  sample_in_port;

extern template class buffer_out_port<audio_buffer>;
extern template class buffer_in_port<audio_buffer>;
extern template class buffer_out_port<sample_buffer>;
extern template class buffer_in_port<sample_buffer>;

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_BUFFER_PORT_HPP_ */
