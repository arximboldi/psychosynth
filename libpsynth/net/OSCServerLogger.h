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

#ifndef OSCSERVERLOGGER_H
#define OSCSERVERLOGGER_H

#include <libpsynth/net/OSCServer.h>

class OSCServerLogger : public OSCServerListener
{
public:
    virtual bool handleServerStartListening(OSCServer* server) {
	Logger::instance().log("oscserver", Log::INFO, "Server listening.");
	return false;
    }
    
    virtual bool handleServerStopListening(OSCServer* server, OSCServerError err) {
	switch(err) {
	case SE_NONE:
	    Logger::instance().log("oscserver", Log::INFO, "Server no longer listening.");
	    break;
	    
	case SE_PORT_BINDING:
	    Logger::instance().log("oscserver", Log::INFO, "Could not bind port.");
	    break;

	default:
	    break;
	}

	return false;
    }
    
    virtual bool handleServerClientConnect(OSCServer* server, int client_id) {
	Logger::instance().log("oscserver", Log::INFO, "Client connected.");
	return false;
    }
    
    virtual bool handleServerClientDisconnect(OSCServer* server, int client_id,
					OSCServerClientError cause) {
	switch(cause) {
	case SCE_NONE:
	    Logger::instance().log("oscserver", Log::INFO, "Client disconnected.");
	    break;

	case SCE_CLIENT_TIMEOUT:
	    Logger::instance().log("oscserver", Log::INFO, "Client timeout.");
	    break;
	    
	default:
	    break;
	}
	
	return false;
    }
};

#endif /* OSCSERVERLOGGER_H */
