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

#include "OSCServer.h"

const int MAX_ALIVE_DELAY = 10000;
const int MIN_ALIVE_DELAY = 1000;

using namespace std;

void OSCServerSubject::notifyServerClientDisconnect(OSCServer* server, int c_id,
						    OSCServerClientError cause)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleServerClientDisconnect(server, c_id, cause);
}

void OSCServerSubject::notifyServerClientConnect(OSCServer* server, int c_id)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleServerClientConnect(server, c_id);
}

void OSCServerSubject::notifyServerStartListening(OSCServer* server)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleServerStartListening(server);
}

void OSCServerSubject::notifyServerStopListening(OSCServer* server, OSCServerError err)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleServerStopListening(server, err);
}

OSCServer::OSCServer() :
    OSCController(false),
    m_server(NULL),
    m_state(IDLE)
{
}

OSCServer::~OSCServer()
{
    if (m_state != IDLE)
	stop();
}

void OSCServer::addMethods()
{
    /* DEBUG */
    lo_server_add_method(m_server, NULL, NULL, &lo_generic_handler, NULL);

    lo_server_add_method(m_server, "/ps/alive", "", &alive_cb, this);
    lo_server_add_method(m_server, "/ps/connect", "", &connect_cb, this);
    lo_server_add_method(m_server, "/ps/get_state", "", &get_state_cb, this);
}

void OSCServer::listen(const char* port)
{
    if (m_state == IDLE) {
	notifyServerStartListening(this);
	m_server = lo_server_new_with_proto(port, LO_UDP, NULL);

	if (!m_server) {
	    notifyServerStopListening(this, SE_PORT_BINDING);
	    return;
	}

	setID(SERVER_ID);
	m_nextid = SERVER_ID + 1;

	addMethods();
	OSCController::addMethods(m_server);
	setSender(m_server);

	activate();
	
	m_state = LISTENING;
    }
}

void OSCServer::stop()
{
    if (m_state != IDLE) {
	lo_message msg = lo_message_new();
	broadcastMessage("/ps/drop", msg);
	lo_message_free(msg);

	close();
	notifyServerStopListening(this, SE_NONE);
    }
}

void OSCServer::close()
{
    lo_server_free(m_server);
    clear();
    m_slots.clear();
    m_state = IDLE;
}

int OSCServer::update(int msec)
{
    if (m_state != IDLE) {
	lo_server_recv_noblock(m_server, 0);

	for (SlotMap::iterator it = m_slots.begin();
	     it != m_slots.end();) {
	    Slot& cl = it->second;
	    lo_address addr = it->first;

	    cl.last_alive_recv += msec;
	    cl.last_alive_sent += msec;
	
	    if (cl.last_alive_recv > MAX_ALIVE_DELAY) {
		lo_message msg = lo_message_new();
		lo_send_message_from(it->first, m_server,
				     "/ps/drop", msg);
		lo_message_free(msg);
		
		notifyServerClientDisconnect(this, cl.id, SCE_CLIENT_TIMEOUT);
		m_slots.erase(it++);
		deleteDestiny(addr);
	    } else if (cl.last_alive_sent > MIN_ALIVE_DELAY) {
		lo_message msg = lo_message_new();
		lo_send_message_from(it->first, m_server,
				     "/ps/alive", msg);
		lo_message_free(msg);
		
		cl.last_alive_sent = 0;
	    }
	    
	    ++it;
	}
    }
    
    return m_state;
}

int OSCServer::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    SlotMap::iterator iter;

    iter = m_slots.find(lo_message_get_source(msg));
    if (iter != m_slots.end())
	iter->second.last_alive_recv = 0;
    
    return 0;
}

int OSCServer::_connect_cb(const char* path, const char* types,
			   lo_arg** argv, int argc, lo_message msg)
{
    lo_address add = lo_message_get_source(msg);

    if (!isDestiny(add)) {
	int id = m_nextid++;
	
	lo_address addcpy = lo_address_new(lo_address_get_hostname(add),
					   lo_address_get_port(add));
	
	lo_message resp = lo_message_new();
	lo_message_add_int32(resp, id);
	lo_send_message_from(add, m_server, "/ps/accept", resp);

	addDestiny(addcpy);
	m_slots[addcpy] = Slot(id);
    }
    
    return 0;
}

int OSCServer::_get_state_cb(const char* path, const char* types,
			     lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCServer::_disconnect_cb(const char* path, const char* types,
			      lo_arg** argv, int argc, lo_message msg)
{
    
    lo_address add = lo_message_get_source(msg);

    if (isDestiny(add)) {
	notifyServerClientDisconnect(this, m_slots[add].id, SCE_NONE);
	
	m_slots.erase(add);
	deleteDestiny(add);
    }
    
    return 0;
}
