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

#include "net/OSCClient.h"

const int MAX_ALIVE_DELAY = 10000;
const int MIN_ALIVE_DELAY = 1000;

using namespace std;

void OSCClientSubject::notifyServerAccept(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientAccept(param);
}

void OSCClientSubject::notifyServerTimeout(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientTimeout(param);
}

void OSCClientSubject::notifyServerDrop(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientDrop(param);
}

OSCClient::OSCClient() :
    OSCController(false),
    m_server(NULL)
{
}

OSCClient::~OSCClient()
{
}

void OSCClient::connect(lo_address target, const char* local_port)
{
    if (m_state == IDLE) {
	m_server = lo_server_new(local_port, NULL);
	addMethods();
	addDestiny(target);

	lo_message msg = lo_message_new();
	broadcastMessage("/ps/connect", msg);
	lo_message_free(msg);
    }
}

void OSCClient::close()
{
    lo_server_free(m_server);
    clear();
    m_state = IDLE;
}

void OSCClient::disconnect()
{
    lo_message msg = lo_message_new();
    broadcastMessage("/ps/disconnect", msg);
    lo_message_free(msg);
    
    close();
}

int OSCClient::update(int msec)
{
    if (m_state != IDLE) {
	lo_server_recv_noblock(m_server, 0);

	m_last_alive_recv += msec;
	m_last_alive_sent += msec;
	
	if (m_last_alive_recv > MAX_ALIVE_DELAY) {
	    notifyServerTimeout(this);
	    m_state = CLOSING;
	}

	if (m_last_alive_sent > MIN_ALIVE_DELAY) {
	    lo_message msg = lo_message_new();
	    broadcastMessage("/ps/alive", msg);
	    lo_message_free(msg);
	    m_last_alive_sent = 0;
	}
	
	if (m_state == CLOSING)
	    close();
    }

    return m_state;
}

void OSCClient::addMethods()
{
    lo_server_add_method(m_server, "/ps/drop", "", &drop_cb, this);
    lo_server_add_method(m_server, "/ps/accept", "i", &accept_cb, this);
    lo_server_add_method(m_server, "/ps/alive", "", &alive_cb, this);
}

int OSCClient::_drop_cb(const char* path, const char* types,
			lo_arg** argv, int argc, lo_message msg)
{
    m_state = CLOSING;

    notifyServerDrop(this);
    return 0;
}

int OSCClient::_accept_cb(const char* path, const char* types,
			  lo_arg** argv, int argc, lo_message msg)
{
    m_id = argv[0]->i;
    setID(m_id);
    OSCController::addMethods(m_server);
    activate();
    notifyServerAccept(this);

    return 0;
}

int OSCClient::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    m_last_alive_recv = 0;
    
    return 0;
}
