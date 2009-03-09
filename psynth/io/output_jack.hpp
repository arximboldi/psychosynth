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

#ifndef PSYNTH_OUTPUTJACK_H
#define PSYNTH_OUTPUTJACK_H

#include <vector>
#include <jack/jack.h>

#include <psynth/io/output.hpp>

namespace psynth
{

class output_jack : public output
{   
    static int jack_process_cb(jack_nframes_t nframes, void* jack_client) {
	static_cast<output_jack*>(jack_client)->jack_process(nframes);
	return 0;
    }

    static int jack_sample_rate_cb(jack_nframes_t nframes, void* jack_client) {
	static_cast<output_jack*>(jack_client)->jack_sample_rate(nframes);
	return 0;
    }

    static void jack_shutdown_cb(void* jack_client) {
	static_cast<output_jack*>(jack_client)->jack_shutdown();
    }

    void jack_process (jack_nframes_t nframes);
    void jack_sample_rate (jack_nframes_t nframes);
    void jack_shutdown ();

    void connect_ports ();
    
    std::vector<jack_port_t*> m_out_ports;
    jack_client_t* m_client;
    std::string m_serv_name;
    size_t m_actual_rate;
    
public:
    output_jack();
    output_jack(const audio_info& info);
    output_jack(const audio_info& info, const std::string& server_name);
    ~output_jack();

    bool set_server (const std::string& server) {
	if (get_state () == NOTINIT) {
	    m_serv_name = server;
	    return true;
	}
	
	return false;
    }

    const std::string& get_server () const {
	return m_serv_name;
    }
    
    bool open ();
    bool close ();
    bool put (const audio_buffer& buf, size_t nframes);
    bool start ();
    bool stop ();
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUTJACK_H */
