/**
 *  Time-stamp:  <2011-03-09 18:49:58 raskolnikov>
 *
 *  @file        jack_raw_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 12:37:45 2011
 *
 *  Jack raw output interface.
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

#ifndef PSYNTH_IO_JACK_RAW_OUTPUT_H_
#define PSYNTH_IO_JACK_RAW_OUTPUT_H_

#include <vector>
#include <jack/jack.h>

#include <psynth/new_io/async_base.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (error, jack_error);
PSYNTH_DECLARE_ERROR (jack_error, jack_open_error);
PSYNTH_DECLARE_ERROR (jack_error, jack_param_error);

class jack_raw_output : public virtual async_base,
                        public boost::noncopyable
{
public:
    jack_raw_output (const char* client,
                     const char* server,
                     int         rate,
                     int         channels);

    ~jack_raw_output ();

    void start ();
    void stop ();
    
    std::size_t put_i (const void*        data, std::size_t frames);
    std::size_t put_n (const void* const* data, std::size_t frames);

    std::size_t buffer_size () const
    { return _buffer_size; }
    
private:
    void connect_ports ();
    
    void _on_process (jack_nframes_t frames)
    { process (frames); }

    void _on_sample_rate (jack_nframes_t newrate) { /* TODO */ }

    void _on_buffer_size (jack_nframes_t newsize)
    { _buffer_size = newsize; }
    
    void _on_shutdown () { /* TODO */ }
    
    static int _process_cb (jack_nframes_t nframes, void* jack_client);
    static int _sample_rate_cb (jack_nframes_t newrate, void* jack_client);
    static int _buffer_size_cb (jack_nframes_t newsize, void* jack_client);
    static void _shutdown_cb (void* jack_client);
    
    std::vector<jack_port_t*> _out_ports;
    jack_client_t*            _client;
    int                       _actual_rate;
    std::size_t               _buffer_size;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_JACK_RAW_OUTPUT_H_ */

