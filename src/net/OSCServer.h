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

#ifndef OSCSERVER_H
#define OSCSERVER_H

#include <list>

#include "net/OSCMisc.h"
#include "net/OSCController.h"

class OSCServer;

class OSCServerListener
{
public:
    virtual bool handleClientConnect(OSCServer* server, int client_id) = 0;
    virtual bool handleClientDisconnect(OSCServer* server, int client_id) = 0;
    virtual bool handleClientTimeout(OSCServer* server, int client_id) = 0;
};

class OSCServerSubject
{
    std::list<OSCServerListener*> m_list;
public:
    void addListener(OSCServerListener* l) {
	m_list.push_back(l);
    }

    void deleteListener(OSCServerListener* l) {
	m_list.remove(l);
    }
    
    void notifyClientDisconnect(OSCServer* server, int client_id);
    void notifyClientConnect(OSCServer* server, int client_id);
    void notifyClientTimeout(OSCServer* server, int client_id);
};

class OSCServer : public OSCController,
		  public OSCServerSubject
{
    struct Slot {
	int id;
	int last_alive_recv;
	int last_alive_sent;
    };
    
    lo_server m_server;
    bool m_listening;
    
    LO_HANDLER(OSCServer, alive);
    LO_HANDLER(OSCServer, connect);
    LO_HANDLER(OSCServer, get_state);
    LO_HANDLER(OSCServer, disconnect);

    void addMethods();
public:
    OSCServer();
    ~OSCServer();

    bool isListening() {
	return m_listening;
    };

    bool listen(const char* port);
    void stop();
    int update(int msec);
};

#endif

