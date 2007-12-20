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

#ifndef OSCCLIENT_H
#define OSCCLIENT_H

#include "common/Logger.h"
#include "net/OSCController.h"

class OSCClient;

enum OSCClientError {
    CE_NONE = 0,
    CE_PORT_BINDING,
    CE_SERVER_TIMEOUT,
    CE_SERVER_DROP
};

class OSCClientListener
{
public:
    virtual ~OSCClientListener() {}
    
    virtual bool handleClientConnect(OSCClient* client) = 0;
    virtual bool handleClientDisconnect(OSCClient* client, OSCClientError err) = 0;
    virtual bool handleClientAccept(OSCClient* client) = 0;
};

class OSCClientSubject
{
    std::list<OSCClientListener*> m_list;

protected:
    void notifyClientConnect(OSCClient* param);
    void notifyClientDisconnect(OSCClient* param, OSCClientError err);
    void notifyClientAccept(OSCClient* param);
    
public:
    void addListener(OSCClientListener* l) {
	m_list.push_back(l);
    };
    
    void deleteListener(OSCClientListener* l) {
	m_list.remove(l);
    };
};

class OSCClient : public OSCController,
		  public OSCClientSubject
{
    lo_address m_target;
    lo_server m_server;
    int m_id;
    int m_state;
    int m_last_alive_recv;
    int m_last_alive_sent;
    
    LO_HANDLER(OSCClient, alive);
    LO_HANDLER(OSCClient, drop);
    LO_HANDLER(OSCClient, accept);

    void addMethods();
    void close();
    
public:
    enum State {
	IDLE,
	PENDING,
	CONNECTED,
	CLOSING
    };
    
    OSCClient();
    ~OSCClient();

    int getState() {
	return m_state;
    }
    
    void connect(lo_address target, const char* src_port);
    void disconnect();
    int update(int msec);
};

#endif /* OSCCLIENT_H */
