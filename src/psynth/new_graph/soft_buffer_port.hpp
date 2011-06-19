/**
 *  Time-stamp:  <2011-06-18 23:49:49 raskolnikov>
 *
 *  @file        soft_buffer_port.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 18 18:11:03 2011
 *
 *  @brief Buffer port that makes a soft transition on reconnection.
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

#include <iostream>

#include <psynth/sound/output.hpp>
#include <psynth/synth/simple_envelope.hpp>
#include <psynth/synth/util.hpp>
#include <psynth/new_graph/buffer_port.hpp>

namespace psynth
{
namespace graph
{

constexpr float default_soft_port_duration = 0.001f; // Seconds

template <class Buffer>
class soft_buffer_in_port : public buffer_in_port<Buffer>
{
    typedef buffer_in_port<Buffer> base_type;
    
public:
    soft_buffer_in_port (
        std::string name,
        node* owner,
        audio_sample stable_value = 0.0f, 
        float duration  = default_soft_port_duration);

    const Buffer& rt_get_in () const
    { return _local_buffer; }
    
    void disconnect ();
    void connect (out_port_base& dest);
    void rt_process (rt_process_context& ctx);
    void rt_context_update (rt_process_context& ctx);
    
private:
    typedef synth::simple_envelope<sample_range> envelope_type;

    audio_sample   _stable_value;
    out_port_base* _request_source;
    bool           _requested;
    envelope_type  _envelope;
    float          _duration;
    Buffer         _local_buffer;
};

template <class B>
soft_buffer_in_port<B>::soft_buffer_in_port (std::string name,
                                             node* owner,
                                             audio_sample stable_value, 
                                             float duration)
    : base_type (name, owner)
    , _stable_value (stable_value)
    , _request_source (0)
    , _requested (false)
    , _duration (duration)
    , _local_buffer ()
{
}

template <typename T>
struct soft_buffer_forward_port
    : public forward_port_impl<soft_buffer_in_port<T>,
                               buffer_out_port<T> >
{
    typedef forward_port_impl<soft_buffer_in_port<T>,
                              buffer_out_port<T> > base_type;
    
    soft_buffer_forward_port (std::string in_name,
                              std::string out_name,
                              node* in_owner,
                              node* out_owner)
        : base_type (in_name, out_name,
                     in_owner, out_owner)
    {}
};

typedef soft_buffer_in_port<audio_buffer> soft_audio_in_port;
typedef soft_buffer_in_port<sample_buffer> soft_sample_in_port;
typedef soft_buffer_forward_port<audio_buffer> soft_audio_forward_port;
typedef soft_buffer_forward_port<sample_buffer> soft_sample_forward_port;

extern template class soft_buffer_in_port<audio_buffer>;
extern template class soft_buffer_in_port<sample_buffer>;
extern template class soft_buffer_forward_port<audio_buffer>;
extern template class soft_buffer_forward_port<sample_buffer>;

} /* namespace graph */
} /* namespace psynth */
