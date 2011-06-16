/**
 *  Time-stamp:  <2011-06-16 17:33:16 raskolnikov>
 *
 *  @file        output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 11 23:21:42 2011
 *
 *  @brief Output node.
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

#include "io/output.hpp"
#include "async_output.hpp"

#define PSYNTH_MODULE_NAME "psynth.graph.core.async_output"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (async_output);

PSYNTH_DEFINE_ERROR_WHAT (async_output_not_bound_error,
                          "Output node not bound to device");


constexpr float default_buffer_factor = 4.0f;

async_output::async_output (device_ptr out)
    : _in_input ("input", this)
    , _output (out)
    , _buffer (out ? out->buffer_size () * default_buffer_factor : 0)
{
    using namespace std::placeholders;
    
    if (out)
    {
        out->check_idle ();
        out->set_callback (
            std::bind (&async_output::_output_callback, this, _1));
    }
}

void async_output::start ()
{
    if (!_output)
        throw async_output_not_bound_error ();
    _output->start ();
}

void async_output::stop ()
{
    if (!_output)
        throw async_output_not_bound_error ();
    _output->stop ();
}

void async_output::set_output (device_ptr out)
{
    bool started = false;
    
    if (_output)
    {
        started = _output->state () != io::async_state::idle;
        if (started)
            _output->stop ();
    }
    _output = out;

    if (out)
    {
        _buffer.recreate (out->buffer_size () * default_buffer_factor);
        if (started)
            _output->start ();
    }
}

void async_output::rt_do_process (rt_process_context& ctx)
{
    // This request might come from another thread. Thus, we store it
    // in the ring buffer. If it was us who made the request and the
    // buffer size matches the block size, then we'll directly send it
    // to the device, otherwise we accumulate the results until the
    // device requests the information.
    
    range (_buffer).write (_in_input.rt_range ());
}

void async_output::_output_callback (std::size_t nframes)
{
    // FIXME: This needs locking or fixing the ring buffer
    // implementation to be lock free.

    auto& rng = range (_buffer);
    while (rng.available () < (std::ptrdiff_t) nframes)
        rt_request_process ();
    
    _output->put (rng.sub_buffer_one (_pos, nframes));
    _output->put (rng.sub_buffer_two (_pos, nframes));

    rng.advance (_pos, nframes);
}

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
