/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef PSYNTH_OSCSERVERLOGGER_H
#define PSYNTH_OSCSERVERLOGGER_H

#include <psynth/net/osc_server.hpp>

namespace psynth
{

class osc_server_logger : public osc_server_listener
{
public:
    virtual bool handle_server_start_listening (osc_server* server)
    {
	base::logger::self () ("oscserver", base::log::info, "Server listening.");
	return false;
    }
    
    virtual bool handle_server_stop_listening (osc_server* server,
					       osc_server_error err)
    {
	switch(err) {
	case SE_NONE:
	    base::logger::self () ("oscserver", base::log::info, "Server no longer listening.");
	    break;
	    
	case SE_PORT_BINDING:
	    base::logger::self () ("oscserver", base::log::info, "Could not bind port.");
	    break;

	default:
	    break;
	}

	return false;
    }
    
    virtual bool handle_server_client_connect(osc_server* server, int client_id)
    {
	base::logger::self () ("oscserver", base::log::info, "Client connected.");
	return false;
    }
    
    virtual bool handle_server_client_disconnect(osc_server* server,
						 int client_id,
						 osc_server_client_error cause)
    {
	switch(cause) {
	case SCE_NONE:
	    base::logger::self () ("oscserver", base::log::info, "Client disconnected.");
	    break;

	case SCE_CLIENT_TIMEOUT:
	    base::logger::self () ("oscserver", base::log::info, "Client timeout.");
	    break;
	    
	default:
	    break;
	}
	
	return false;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OSCSERVERLOGGER_H */
