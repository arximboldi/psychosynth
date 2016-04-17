/**
 *  Time-stamp:  <2011-07-03 10:42:20 raskolnikov>
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
#include <psynth/new_graph/buffer_port_fwd.hpp>

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

    virtual typename T::const_range rt_out_range () const
    { return const_range (this->rt_get_out ()); }

    virtual typename T::range rt_out_range ()
    { return range (this->rt_get_out ()); }

    void rt_context_update (rt_process_context& ctx)
    { this->rt_get_out ().recreate (ctx.block_size ()); }
};

template <typename T>
class buffer_in_port : public in_port<T>
{
public:
    virtual typename T::const_range rt_in_range () const
    { return const_range (this->rt_get_in ()); }

    buffer_in_port (std::string name, node* owner)
        : in_port<T> (name, owner) {}
};

template <typename T>
class defaulting_buffer_in_port : public buffer_in_port<T>
{
public:
    typedef buffer_in_port<T> base_type;

    const T& rt_get_in () const
    {
        if (this->rt_in_available ())
            return base_type::rt_get_in ();
        else
            return _default;
    }

    defaulting_buffer_in_port (std::string name,
                               node* owner,
                               typename T::value_type defval)
        : base_type (name, owner)
        , _default_value (defval)
    {}

    void rt_context_update (rt_process_context& ctx)
    {
        base_type::rt_context_update (ctx);
        _default.recreate (ctx.block_size (), _default_value, 0);
    }

private:
    T _default;
    typename T::value_type _default_value;
};

template <typename T>
struct buffer_forward_port
    : public forward_port_impl<buffer_in_port<T>,
                               buffer_out_port<T> >
{
    typedef forward_port_impl<buffer_in_port<T>,
                              buffer_out_port<T> > base_type;

    buffer_forward_port (std::string in_name,
                         std::string out_name,
                         node* in_owner,
                         node* out_owner)
        : base_type (in_name, out_name,
                     in_owner, out_owner)
    {}
};

typedef buffer_out_port<audio_buffer>    audio_out_port;
typedef buffer_in_port<audio_buffer>     audio_in_port;
typedef defaulting_buffer_in_port<audio_buffer>
defaulting_audio_in_port;
typedef buffer_forward_port<audio_buffer> audio_forward_port;

typedef buffer_out_port<sample_buffer> sample_out_port;
typedef buffer_in_port<sample_buffer>  sample_in_port;
typedef defaulting_buffer_in_port<sample_buffer>
defaulting_sample_in_port;
typedef buffer_forward_port<sample_buffer> sample_forward_port;

extern template class buffer_out_port<audio_buffer>;
extern template class buffer_in_port<audio_buffer>;
extern template class defaulting_buffer_in_port<audio_buffer>;
extern template class buffer_forward_port<audio_buffer>;
extern template class buffer_out_port<sample_buffer>;
extern template class buffer_in_port<sample_buffer>;
extern template class defaulting_buffer_in_port<sample_buffer>;
extern template class buffer_forward_port<sample_buffer>;

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_BUFFER_PORT_HPP_ */
