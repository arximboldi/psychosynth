/**
 *  Time-stamp:  <2011-03-21 23:02:04 raskolnikov>
 *
 *  @file        jack_raw_output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 12:51:39 2011
 *
 *  Jack raw output implementation.
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

#define PSYNTH_MODULE_NAME "psynth.io.jack"

#include <cstring>
#include <boost/lexical_cast.hpp>

#include "base/logger.hpp"
#include "base/scope_guard.hpp"
#include "jack_raw_output.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (jack_error);
PSYNTH_DEFINE_ERROR_WHAT (jack_open_error, "Could not connect to Jack.");
PSYNTH_DEFINE_ERROR_WHAT (jack_param_error, "Invalid jack param.");

namespace
{

void log_jack_info (const char* msg)
{
    PSYNTH_LOG << base::log::info << msg;
}

void log_jack_error (const char* msg)
{
    PSYNTH_LOG << base::log::error << msg;
}

} /* anonymous namespace */


#define PSYNTH_JACK_CHECK(fun, except)                          \
    do {                                                        \
        int err = fun;                                          \
        if (err < 0) throw except ();                           \
    } while (0)

jack_raw_output::jack_raw_output (const char* client,
                                  const char* server,
                                  int         rate,
                                  int         channels,
                                  callback_type cb)
    : detail::async_base_impl (cb)
    , _out_ports (channels)
    , _buffer_size (0)
{
    jack_set_error_function (log_jack_error);
    jack_set_info_function (log_jack_info);
    
    jack_options_t options = !server ? JackNullOption : JackServerName; 
    _client = jack_client_open (client, options, 0, server);
    if (!_client) throw jack_open_error ();

    auto grd_client = base::make_guard ([&] { jack_client_close (_client); });
    
    PSYNTH_JACK_CHECK (jack_set_process_callback (
                           _client, &jack_raw_output::_process_cb, this),
                       jack_param_error);
    PSYNTH_JACK_CHECK (jack_set_sample_rate_callback (
                           _client, &jack_raw_output::_sample_rate_cb, this),
                       jack_param_error);

    jack_on_shutdown (_client, &jack_raw_output::_shutdown_cb, this);
	
    _actual_rate = jack_get_sample_rate (_client);
    if (_actual_rate != rate)
        PSYNTH_LOG
            << base::log::warning
            << "Jackd sample rate and application sample rate mismatch."
            << "Better sound quality is achieved if both are the same.";


    _buffer_size = jack_get_buffer_size (_client);
    PSYNTH_LOG << base::log::info
               << "Jackd buffer size is: " << _buffer_size;
    
    for (size_t i = 0; i < _out_ports.size(); ++i)
    {
        std::string port_name = std::string ("out_") +
            boost::lexical_cast<std::string> (i);
        
        _out_ports [i] = jack_port_register (
            _client, port_name.c_str (), JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput, 0);
        
        if (_out_ports [i] == 0)
            throw jack_param_error ();
    }
    
    grd_client.dismiss ();
}

jack_raw_output::~jack_raw_output ()
{
    soft_stop ();
    if (_client != 0)
        jack_client_close (_client);
}

std::size_t jack_raw_output::put_i (const void* data, std::size_t frames)
{
    assert (false);
    return 0;
}

std::size_t jack_raw_output::put_n (const void* const* data, std::size_t frames)
{
    assert (state () == async_state::running); // Non-asynchronous IO only
    
    for (size_t i = 0; i < _out_ports.size(); ++i)
    {
	const auto out   = jack_port_get_buffer (_out_ports [i], frames);
        const auto bytes = sizeof (jack_default_audio_sample_t) * frames;
        const auto src   = *data++;

        std::memcpy (out, src, bytes);
    }
    return frames;
}

void jack_raw_output::start ()
{
    check_idle ();

    jack_activate (_client);
    auto grd_activate = base::make_guard ([&] { jack_deactivate (_client); });

    connect_ports ();
    
    grd_activate.dismiss ();
    set_state (async_state::running);
}

void jack_raw_output::stop ()
{
    check_running ();
    jack_deactivate (_client);
    set_state (async_state::idle);
}

void jack_raw_output::connect_ports ()
{
    const char** ports;

    ports = jack_get_ports (_client, 0, 0, JackPortIsPhysical | JackPortIsInput);
    if (!ports)
    {
	PSYNTH_LOG << base::log::warning << "There are no phisical output ports.";
	return;
    }

    PSYNTH_ON_BLOCK_EXIT ([&] { ::free (ports); });

    std::size_t i = 0;
    for (; i < _out_ports.size() && ports [i]; ++i)
	jack_connect (_client, jack_port_name (_out_ports [i]), ports [i]);

    if (i < _out_ports.size ())
        PSYNTH_LOG << base::log::warning << "Not enough phisical output ports.";
}

int jack_raw_output::_process_cb (jack_nframes_t nframes,
                                  void* jack_client)
{
    static_cast<jack_raw_output*>(jack_client)->_on_process (nframes);
    return 0;
}

int jack_raw_output::_sample_rate_cb (jack_nframes_t newrate,
                                      void* jack_client)
{
    static_cast<jack_raw_output*>(jack_client)->_on_sample_rate (newrate);
    return 0;
}

int jack_raw_output::_buffer_size_cb (jack_nframes_t newsize,
                                             void* jack_client)
{
    static_cast<jack_raw_output*>(jack_client)->_on_buffer_size (newsize);
    return 0;
}

void jack_raw_output::_shutdown_cb (void* jack_client)
{
    static_cast<jack_raw_output*>(jack_client)->_on_shutdown ();
}

} /* namespace io */
} /* namespace psynth */
