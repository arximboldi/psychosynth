/**
 *  Time-stamp:  <2011-06-18 23:48:45 raskolnikov>
 *
 *  @file        soft_buffer_port.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 18 20:26:00 2011
 *
 *  @brief Soft buffer port implementation.
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

#include "soft_buffer_port.hpp"

namespace psynth
{
namespace graph
{

template class soft_buffer_in_port<audio_buffer>;
template class soft_buffer_in_port<sample_buffer>;
template class soft_buffer_forward_port<audio_buffer>;
template class soft_buffer_forward_port<sample_buffer>;

template <class B>
void soft_buffer_in_port<B>::rt_context_update (rt_process_context& ctx)
{
    base_type::rt_context_update (ctx);
    auto delta = 1.0f / (_duration * ctx.frame_rate ());
    _envelope.set_deltas (delta, -delta);
    _local_buffer.recreate (ctx.block_size ());
}

template <class B>
void soft_buffer_in_port<B>::rt_process (rt_process_context& ctx)
{
    typedef typename B::value_type frame_type;

    if (_requested)
        _envelope.release ();
    _envelope.update (); // Hack, skips one frame
    
    if (_requested && (!this->rt_connected () ||
                       _envelope.finished ()))
    {
        this->_rt_connect (_request_source);
        if (_request_source) 
            _envelope.press ();
        _requested = false;
    }

    base_type::rt_process (ctx);    
    if (this->rt_in_available ())
    {
        _envelope.update (range (_local_buffer));
        
        synth::blend (const_range (base_type::rt_get_in ()),
                      range (_local_buffer),
                      _stable_value,
                      range (_local_buffer));
        
    }
    else
    {
        _envelope.update (ctx.block_size ());
        sound::fill_frames (range (_local_buffer),
                            frame_type (_stable_value));
    }
}

template <class B>
void soft_buffer_in_port<B>::disconnect ()
{
    this->_user_connect (0);
    _request_source = 0;
    _requested = true;
}

template <class B>
void soft_buffer_in_port<B>::connect (out_port_base& out)
{
    this->_user_connect (&out);
    _request_source = &out;
    _requested = true;
}

} /* namespace graph */
} /* namespace psynth */
