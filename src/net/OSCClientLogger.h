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

#ifndef OSCCLIENTLOGGER_H
#define OSCCLIENTLOGGER_H

#include "net/OSCClient.h"

class OSCClientLogger : public OSCClientListener
{
public:
    virtual bool handleClientConnect(OSCClient* client) {
	Logger::instance().log("oscclient", Log::INFO, "Connecting...");
	return false;
    }
    
    virtual bool handleClientDisconnect(OSCClient* client) {
	Logger::instance().log("oscclient", Log::INFO, "Disconnected.");
	return false;
    }
    
    virtual bool handleClientAccept(OSCClient* client) {
	Logger::instance().log("oscclient", Log::INFO, "Accepted.");
	return false;
    }
    
    virtual bool handleClientTimeout(OSCClient* client) {
	Logger::instance().log("oscclient", Log::INFO, "Connection timeout.");
	return false;
    }
    
    virtual bool handleClientDrop(OSCClient* client) {
	Logger::instance().log("oscclient", Log::INFO, "Client dropped by server.");
	return false;
    }
};

#endif /* OSCCLIENTLOGGER_H */
