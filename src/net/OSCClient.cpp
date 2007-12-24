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
#include "net/OSCProtocol.h"

const int MAX_ALIVE_DELAY = 20000;
const int MIN_ALIVE_DELAY = 1000;

using namespace std;

void OSCClientSubject::notifyClientConnect(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientConnect(param);
}

void OSCClientSubject::notifyClientAccept(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientAccept(param);
}

void OSCClientSubject::notifyClientDisconnect(OSCClient* param, OSCClientError err)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientDisconnect(param, err);
}

OSCClient::OSCClient() :
    OSCController(false),
    m_server(NULL),
    m_state(IDLE),
    m_count_next(0)
{
}

OSCClient::~OSCClient()
{
    if (m_state != IDLE)
	disconnect();
}

void OSCClient::connect(lo_address target, const char* local_port)
{
    if (m_state == IDLE) {
	notifyClientConnect(this);
	
	m_server = lo_server_new_with_proto(local_port, LO_UDP, NULL);

	if (!m_server) {
	    notifyClientDisconnect(this, CE_PORT_BINDING);
	    return;
	}
	
	m_last_alive_recv = 0;
	m_last_alive_sent = 0;
	m_count_next = 1;
	
	addMethods();
	setSender(m_server);
	
	lo_message msg = lo_message_new();
	lo_send_message_from(target, m_server, MSG_CONNECT, msg);
	lo_message_free(msg);

	lo_address_free(target);
	
	m_state = PENDING;
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
    if (m_state != IDLE) {
	lo_message msg = lo_message_new();
	broadcastMessage(MSG_DISCONNECT, msg);
	lo_message_free(msg);
	
	close();
	notifyClientDisconnect(this, CE_NONE);
    }
}

int OSCClient::update(int msec)
{
    if (m_state != IDLE) {
	lo_server_recv_noblock(m_server, 0);

	if (!m_count_next) {
	    m_last_alive_recv += msec;
	    m_last_alive_sent += msec;
	} else
	    m_count_next = 0;
	
	if (m_last_alive_recv > MAX_ALIVE_DELAY) {
	    notifyClientDisconnect(this, CE_SERVER_TIMEOUT);
	    m_state = CLOSING;
	}

	if (m_last_alive_sent > MIN_ALIVE_DELAY) {
	    lo_message msg = lo_message_new();
	    broadcastMessage(MSG_ALIVE, msg);
	    lo_message_free(msg);
	    m_last_alive_sent = 0;
	}
	
	if (m_state == CLOSING) {
	    close();
	    notifyClientDisconnect(this, CE_SERVER_DROP);
	}
    }

    return m_state;
}

void OSCClient::addMethods()
{
    /* DEBUG */
    // lo_server_add_method(m_server, NULL, NULL, &lo_generic_handler, NULL);

    lo_server_add_method(m_server, MSG_DROP, "", &drop_cb, this);
    lo_server_add_method(m_server, MSG_ACCEPT, "i", &accept_cb, this);
    lo_server_add_method(m_server, MSG_ALIVE, "", &alive_cb, this);
}

int OSCClient::_drop_cb(const char* path, const char* types,
			lo_arg** argv, int argc, lo_message msg)
{
    m_state = CLOSING;

    return 0;
}

int OSCClient::_accept_cb(const char* path, const char* types,
			  lo_arg** argv, int argc, lo_message msg)
{
    if (m_state == PENDING) {
	m_id = argv[0]->i;
	setID(m_id);

	getTable()->clear();
	
	OSCController::addMethods(m_server);
	activate();

	/* FIXME: Make sure that the accept message came from our desired target. */
	lo_address add = lo_message_get_source(msg);
	lo_address addcpy = lo_address_new(lo_address_get_hostname(add),
					   lo_address_get_port(add));
    
	addDestiny(addcpy);

	lo_message msg = lo_message_new();
	broadcastMessage(MSG_GET_STATE, msg);
	lo_message_free(msg);
	
	m_state = CONNECTED;
	notifyClientAccept(this);
    }
    
    return 0;
}

int OSCClient::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    m_last_alive_recv = 0;    
    return 0;
}
