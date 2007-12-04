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

#ifndef OSCCLIENT_H
#define OSCCLIENT_H

#include "net/OSCController.h"

class OSCClient;

class OSCClientListener
{
public:
    virtual bool handleClientAccept(OSCClient*) = 0;
    virtual bool handleClientTimeout(OSCClient*) = 0;
    virtual bool handleClientDrop(OSCClient*) = 0;
};

class OSCClientSubject
{
    std::list<OSCClientListener*> m_list;
    
public:
    void addListener(OSCClientListener* l) {
	m_list.push_back(l);
    };
    
    void deleteListener(OSCClientListener* l) {
	m_list.remove(l);
    };
    
    void notifyClientAccept(OSCClient* param);
    void notifyClientTimeout(OSCClient* param);
    void notifyClientDrop(OSCClient* param);
};

class OSCClient : public OSCController,
		  private OSCClientSubject
{
    lo_server m_server;

    LO_HANDLER(OSCClient, alive);
    LO_HANDLER(OSCClient, drop);
    LO_HANDLER(OSCClient, accepted);

public:
    OSCClient();
    ~OSCClient();
    
    void connect(lo_address);
    void disconnect();
    bool isConnected();
    int update(int msec);
};

#endif /* OSCCLIENT_H */
