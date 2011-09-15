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

#include <cstring>
#include <algorithm>
#include <cstdio>

#include "net/osc_server.hpp"
#include "net/osc_protocol.hpp"

static const int MAX_ALIVE_DELAY = 60000;
static const int MIN_ALIVE_DELAY = 1000;

using namespace std;

namespace psynth
{

void osc_server_subject::notify_server_client_disconnect (osc_server* server, int c_id,
							  osc_server_client_error cause)
{
    for (list<osc_server_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_server_client_disconnect (server, c_id, cause);
}

void osc_server_subject::notify_server_client_connect (osc_server* server, int c_id)
{
    for (list<osc_server_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_server_client_connect (server, c_id);
}

void osc_server_subject::notify_server_start_listening (osc_server* server)
{
    for (list<osc_server_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_server_start_listening (server);
}

void osc_server_subject::notify_server_stop_listening (osc_server* server,
						       osc_server_error err)
{
    for (list<osc_server_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_server_stop_listening(server, err);
}

osc_server::osc_server() :
    osc_controller(true),
    m_server(NULL),
    m_state(IDLE)
{
}

osc_server::~osc_server()
{
    if (m_state != IDLE)
	stop();
}

void osc_server::add_methods ()
{
    /*
      lo_server_add_method(m_server, NULL, NULL, &lo_generic_handler, NULL);
    */
    
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_ALIVE, "", &alive_cb, this);
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_CONNECT, "", &connect_cb, this);
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_GET_STATE, "", &get_state_cb, this);
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_DISCONNECT, "", &disconnect_cb, this);
}

void osc_server::listen(const char* port)
{
    if (m_state == IDLE) {
	notify_server_start_listening (this);

	m_server = lo_server_new_with_proto(port, LO_UDP, NULL);
	
	if (!m_server) {
	    notify_server_stop_listening (this, SE_PORT_BINDING);
	    return;
	}

	set_id (SERVER_ID);
	m_nextid = SERVER_ID + 1;

	add_methods ();
	osc_controller::add_methods (m_server);
	set_sender (m_server);

	activate();
	
	m_state = LISTENING;
    }
}

void osc_server::stop()
{
    if (m_state != IDLE) {
	lo_message msg = lo_message_new();
	broadcast_message (PSYNTH_OSC_MSG_DROP, msg);
	lo_message_free(msg);

	close();
	notify_server_stop_listening(this, SE_NONE);
    }
}

void osc_server::close()
{
    lo_server_free(m_server);
    clear();
    m_slots.clear();
    m_state = IDLE;
}

int osc_server::receive(int time_out)
{
    int n_recv = 0;
    
    if (m_state != IDLE) {
	if (time_out >= 0)
	    n_recv = lo_server_recv_noblock(m_server, time_out);
	else
	    n_recv = lo_server_recv(m_server);
    }

    return n_recv;
}

int osc_server::update(int msec)
{    
    if (m_state != IDLE) {
	for (slot_map::iterator it = m_slots.begin();
	     it != m_slots.end();) {
	    slot& cl = it->second;
	    lo_address addr = it->first;

	    cl.last_alive_recv += msec;
	    cl.last_alive_sent += msec;
	
	    if (cl.last_alive_recv > MAX_ALIVE_DELAY) {
		lo_message msg = lo_message_new();
		lo_send_message_from(it->first, m_server,
				     PSYNTH_OSC_MSG_DROP, msg);
		lo_message_free(msg);
	       
		notify_server_client_disconnect(this, cl.id, SCE_CLIENT_TIMEOUT);
		
		m_slots.erase(it++);
		delete_target(addr);
	    } else {
		if (cl.last_alive_sent > MIN_ALIVE_DELAY) {
		    lo_message msg = lo_message_new();
		    lo_send_message_from(it->first, m_server,
					 PSYNTH_OSC_MSG_ALIVE, msg);
		    lo_message_free(msg);
		
		    cl.last_alive_sent = 0;
		}
		++it;
	    }
	}
    }
    
    return m_state;
}

int osc_server::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    slot_map::iterator iter;

    iter = m_slots.find(lo_message_get_source(msg));
    if (iter != m_slots.end())
	iter->second.last_alive_recv = 0;
    
    return 0;
}

int osc_server::_connect_cb(const char* path, const char* types,
			   lo_arg** argv, int argc, lo_message msg)
{
    lo_address add = lo_message_get_source(msg);
    int id;
    
    if (!is_target(add)) {
	id = m_nextid++;
	
	lo_address addcpy = lo_address_new(lo_address_get_hostname(add),
					   lo_address_get_port(add));
	
	add_target(addcpy);
	m_slots[addcpy] = slot (id);
    } else {
	id = m_slots[add].id;
	m_slots[add].last_alive_recv = 0;
	m_slots[add].last_alive_sent = 0;
    }
    
    lo_message resp = lo_message_new();
    lo_message_add_int32(resp, id);
    lo_send_message_from(add, m_server, PSYNTH_OSC_MSG_ACCEPT, resp);
    
    return 0;
}

int osc_server::_get_state_cb(const char* path, const char* types,
			     lo_arg** argv, int argc, lo_message msg)
{
    lo_address add = lo_message_get_source(msg);
    if (is_target(add)) {
	/* TODO */
    }
    
    return 0;
}

int osc_server::_disconnect_cb(const char* path, const char* types,
			      lo_arg** argv, int argc, lo_message msg)
{
    
    lo_address add = lo_message_get_source(msg);

    if (is_target(add)) {
	notify_server_client_disconnect(this, m_slots[add].id, SCE_NONE);
	
	m_slots.erase(add);
	delete_target(add);
    }
    
    return 0;
}

} /* namespace psynth */
