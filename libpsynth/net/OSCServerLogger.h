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

#include <libpsynth/net/OSCServer.h>

namespace psynth
{

class OSCServerLogger : public OSCServerListener
{
public:
    virtual bool handleServerStartListening(OSCServer* server) {
	logger::instance() ("oscserver", log::INFO, "Server listening.");
	return false;
    }
    
    virtual bool handleServerStopListening(OSCServer* server, OSCServerError err) {
	switch(err) {
	case SE_NONE:
	    logger::instance() ("oscserver", log::INFO, "Server no longer listening.");
	    break;
	    
	case SE_PORT_BINDING:
	    logger::instance() ("oscserver", log::INFO, "Could not bind port.");
	    break;

	default:
	    break;
	}

	return false;
    }
    
    virtual bool handleServerClientConnect(OSCServer* server, int client_id) {
	logger::instance() ("oscserver", log::INFO, "Client connected.");
	return false;
    }
    
    virtual bool handleServerClientDisconnect(OSCServer* server, int client_id,
					      OSCServerClientError cause) {
	switch(cause) {
	case SCE_NONE:
	    logger::instance() ("oscserver", log::INFO, "Client disconnected.");
	    break;

	case SCE_CLIENT_TIMEOUT:
	    logger::instance() ("oscserver", log::INFO, "Client timeout.");
	    break;
	    
	default:
	    break;
	}
	
	return false;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OSCSERVERLOGGER_H */
