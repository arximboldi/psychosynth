/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef OUTPUTJACK_H
#define OUTPUTJACK_H

#include <vector>
#include <jack/jack.h>

#include <libpsynth/output/Output.h>

class OutputJack : public Output
{   
    static int jack_process_cb(jack_nframes_t nframes, void* jack_client) {
	static_cast<OutputJack*>(jack_client)->jackProcess(nframes);
	return 0;
    }

    static int jack_sample_rate_cb(jack_nframes_t nframes, void* jack_client) {
	static_cast<OutputJack*>(jack_client)->jackSampleRate(nframes);
	return 0;
    }

    static void jack_shutdown_cb(void* jack_client) {
	static_cast<OutputJack*>(jack_client)->jackShutDown();
    }

    void jackProcess(jack_nframes_t nframes);
    void jackSampleRate(jack_nframes_t nframes);
    void jackShutDown();

    void connectPorts();
    
    std::vector<jack_port_t*> m_out_ports;
    jack_client_t* m_client;
    std::string m_serv_name;
    size_t m_actual_rate;
    
public:
    OutputJack();
    OutputJack(const AudioInfo& info);
    OutputJack(const AudioInfo& info, const std::string& server_name);
    ~OutputJack();

    bool setServer(const std::string& server) {
	if (getState() == NOTINIT) {
	    m_serv_name = server;
	    return true;
	}
	
	return false;
    }

    const std::string& getServer() const {
	return m_serv_name;
    }
    
    bool open();
    bool close();
    bool put(const AudioBuffer& buf, size_t nframes);
    bool start();
    bool stop();
};

#endif /* OUTPUTJACK_H */
