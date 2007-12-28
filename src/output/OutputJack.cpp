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

#include "common/Misc.h"
#include "common/Logger.h"
#include "output/OutputJack.h"

using namespace std;

OutputJack::OutputJack(const AudioInfo& info) :
    Output(info),
    m_out_ports(info.num_channels)
{
}

OutputJack::OutputJack(const AudioInfo& info, const std::string& server_name) :
    Output(info),
    m_out_ports(info.num_channels),
    m_serv_name(server_name)
{
}

OutputJack::~OutputJack()
{
    if (getState() != NOTINIT)
	close();
}

bool OutputJack::open()
{
    if (getState() == NOTINIT) {
	if (m_serv_name.empty())
	    m_serv_name = string();

	m_client = jack_client_new(m_serv_name.c_str());
	if (!m_client) {
	    Logger::instance().log("jack", Log::ERROR,
				   "Could not connect to jackd, maybe it's not running"
				   "or the client name is duplicated.");
	    return false;
	}

	jack_set_process_callback(m_client, &OutputJack::jack_process_cb, this);
	jack_set_sample_rate_callback(m_client, &OutputJack::jack_sample_rate_cb, this);
	jack_on_shutdown(m_client, &OutputJack::jack_shutdown_cb, this);
	
	m_actual_rate = jack_get_sample_rate(m_client);
	if (m_actual_rate != (size_t)getInfo().sample_rate) {
	    Logger::instance().log("jack", Log::WARNING,
				   "Jackd sample rate and application sample rate mismatch."
				   "Better sound quality is achieved if both are the same.");
	}

	for (size_t i = 0; i < m_out_ports.size(); ++i)
	    m_out_ports[i] = jack_port_register(m_client,
						(string("out_") + itoa(i, 10)).c_str(),
						JACK_DEFAULT_AUDIO_TYPE,
						JackPortIsOutput,
						0);
						

	setState(IDLE);

	return true;
    }

    return false;
}

bool OutputJack::close()
{
    if (getState() == RUNNING)
	stop();
    
    if (getState() != NOTINIT) {
	jack_client_close(m_client);
	setState(NOTINIT);
    }

    return true;
}

bool OutputJack::put(const AudioBuffer& in_buf, size_t nframes)
{
    if (in_buf.getInfo().num_channels != getInfo().num_channels 
	|| in_buf.getInfo().sample_rate != getInfo().sample_rate) {
	Logger::instance().log("jack", Log::WARNING,
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

void OutputJack::connectPorts()
{
    const char** ports;

    ports = jack_get_ports(m_client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);

    if (!ports) {
	Logger::instance().log("jack", Log::WARNING, "There are no phisical output ports.");
	return;
    }

    for (size_t i = 0; i < m_out_ports.size() && ports[i]; ++i)
	jack_connect(m_client, jack_port_name(m_out_ports[i]), ports[i]);

    free(ports);
}

void OutputJack::start()
{
    if (getState() == IDLE) {
	jack_activate(m_client);
	connectPorts();
    }
}

void OutputJack::stop()
{
    if (getState() == RUNNING) {
	jack_deactivate(m_client);
    }
}

void OutputJack::jackProcess(jack_nframes_t nframes)
{
    process(nframes);
}

void OutputJack::jackSampleRate(jack_nframes_t new_rate)
{
    m_actual_rate = new_rate;
}

void OutputJack::jackShutDown()
{
    setState(NOTINIT);
}


