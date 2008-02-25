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

#ifndef PSYNTH_OSCSERVER_H
#define PSYNTH_OSCSERVER_H

#include <list>

#include <libpsynth/net/OSCMisc.h>
#include <libpsynth/net/OSCController.h>

namespace psynth
{

class OSCServer;

enum OSCServerClientError {
    SCE_NONE = 0,
    SCE_CLIENT_TIMEOUT
};

enum OSCServerError {
    SE_NONE = 0,
    SE_PORT_BINDING
};

class OSCServerListener
{
    
public:
    virtual ~OSCServerListener() {}
    virtual bool handleServerStartListening(OSCServer* server) = 0;
    virtual bool handleServerStopListening(OSCServer* server, OSCServerError err) = 0;
    virtual bool handleServerClientConnect(OSCServer* server, int client_id) = 0;
    virtual bool handleServerClientDisconnect(OSCServer* server, int client_id,
					OSCServerClientError cause) = 0;
};

class OSCServerSubject
{
    std::list<OSCServerListener*> m_list;

protected:
    void notifyServerStartListening(OSCServer* server);
    void notifyServerStopListening(OSCServer* server, OSCServerError err);
    void notifyServerClientDisconnect(OSCServer* server, int client_id,
				OSCServerClientError cause);
    void notifyServerClientConnect(OSCServer* server, int client_id);
    
public:
    void addListener(OSCServerListener* l) {
	m_list.push_back(l);
    }

    void deleteListener(OSCServerListener* l) {
	m_list.remove(l);
    }
};

class OSCServer : public OSCController,
		  public OSCServerSubject
{
public:
    enum State {
	IDLE,
	LISTENING
    };
    
private:
    const static int SERVER_ID = 0;

    struct Slot {
	int id;
	int last_alive_recv;
	int last_alive_sent;
	
	Slot(int id = 0) :
	    id(id), last_alive_recv(0), last_alive_sent(0) {};
    };

    struct lo_address_lt_func {
	bool operator() (lo_address a, lo_address b) {
	    return lo_address_cmp(a, b) < 0;
	}
    };

    typedef std::map<lo_address, Slot, lo_address_lt_func> SlotMap; 
    SlotMap m_slots;
    
    lo_server m_server;
    bool m_listening;
    int m_nextid;
    State m_state;
    
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

    void listen(const char* port);
    void stop();
    void close();

    int update(int msec);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCSERVER_H */

