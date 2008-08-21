/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include "common/misc.h"
#include "common/logger.h"
#include "output/output_jack.h"

using namespace std;

namespace psynth
{

output_jack::output_jack()
{
}

output_jack::output_jack(const audio_info& info)
    : output(info)
    , m_out_ports(info.num_channels)
{
}

output_jack::output_jack(const audio_info& info, const std::string& server_name)
    : output(info)
    , m_out_ports(info.num_channels)
    , m_serv_name(server_name)
{
}

output_jack::~output_jack()
{
    if (get_state() != NOTINIT)
	close();
}

bool output_jack::open()
{
    if (get_state() == NOTINIT) {
	if (m_serv_name.empty())
	    m_serv_name = string();

	m_client = jack_client_new(m_serv_name.c_str());
	if (!m_client) {
	    logger::instance() ("jack", log::ERROR,
				"Could not connect to jackd, maybe it's not running"
				"or the client name is duplicated.");
	    return false;
	}

	jack_set_process_callback(m_client, &output_jack::jack_process_cb, this);
	jack_set_sample_rate_callback(m_client, &output_jack::jack_sample_rate_cb, this);
	jack_on_shutdown(m_client, &output_jack::jack_shutdown_cb, this);
	
	m_actual_rate = jack_get_sample_rate(m_client);
	if (m_actual_rate != (size_t)get_info ().sample_rate) {
	    logger::instance() ("jack", log::WARNING,
				"Jackd sample rate and application sample rate mismatch."
				"Better sound quality is achieved if both are the same.");
	}

	if (get_info ().num_channels != m_out_ports.size())
	    m_out_ports.resize(get_info ().num_channels);
	
	for (size_t i = 0; i < m_out_ports.size(); ++i)
	    m_out_ports[i] = jack_port_register(m_client,
						(string("out_") + itoa(i, 10)).c_str(),
						JACK_DEFAULT_AUDIO_TYPE,
						JackPortIsOutput,
						0);

	set_state(IDLE);

	return true;
    }

    return false;
}

bool output_jack::close()
{
    if (get_state() == RUNNING)
	stop();
    
    if (get_state() != NOTINIT) {
	jack_client_close(m_client);
	set_state(NOTINIT);
    }

    return true;
}

bool output_jack::put(const audio_buffer& in_buf, size_t nframes)
{
    if (in_buf.get_info().num_channels != get_info ().num_channels ||
	in_buf.get_info().sample_rate != get_info ().sample_rate) {
	logger::instance() ("jack", log::WARNING,
			    "Cant send data to the device:"
			    "data and output system properties missmatch.");
	
	return false;
    }
	
    for (size_t i = 0; i < m_out_ports.size(); ++i) {
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t*)
	    jack_port_get_buffer(m_out_ports[i], nframes);
	memcpy(out, in_buf[i], sizeof(jack_default_audio_sample_t) *  nframes);
    }

    return true;
}

void output_jack::connect_ports()
{
    const char** ports;

    ports = jack_get_ports (m_client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);

    if (!ports) {
	logger::instance () ("jack", log::WARNING, "There are no phisical output ports.");
	return;
    }

    for (size_t i = 0; i < m_out_ports.size() && ports[i]; ++i)
	jack_connect (m_client, jack_port_name(m_out_ports[i]), ports[i]);

    free(ports);
}

bool output_jack::start()
{
    if (get_state() == IDLE) {
	jack_activate (m_client);
	connect_ports ();
	return true;
    }
    
    return false;
}

bool output_jack::stop()
{
    if (get_state() == RUNNING) {
	jack_deactivate (m_client);
	return true;
    }

    return false;
}

void output_jack::jack_process (jack_nframes_t nframes)
{
    process (nframes);
}

void output_jack::jack_sample_rate (jack_nframes_t new_rate)
{
    m_actual_rate = new_rate;
}

void output_jack::jack_shutdown ()
{
    set_state (NOTINIT);
}

} /* namespace psynth */

