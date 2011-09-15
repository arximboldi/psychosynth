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

#include <algorithm>
#include "net/osc_client.hpp"
#include "net/osc_protocol.hpp"

static const int MAX_ALIVE_DELAY = 60000;
static const int MIN_ALIVE_DELAY = 1000;

using namespace std;

namespace psynth
{

void osc_client_subject::notify_client_connect (osc_client* param)
{
    for (list<osc_client_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_client_connect (param);
}

void osc_client_subject::notify_client_accept (osc_client* param)
{
    for (list<osc_client_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_client_accept (param);
}

void osc_client_subject::notify_client_disconnect (osc_client* param, osc_client_error err)
{
    for (list<osc_client_listener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handle_client_disconnect(param, err);
}

osc_client::osc_client() :
    osc_controller(false),
    m_server(NULL),
    m_state(IDLE),
    m_count_next(0)
{
}

osc_client::~osc_client()
{
    if (m_state != IDLE)
	disconnect();
}

void osc_client::connect(lo_address target, const char* local_port)
{
    if (m_state == IDLE) {
	notify_client_connect(this);
	
	m_server = lo_server_new_with_proto(local_port, LO_UDP, NULL);

	if (!m_server) {
	    notify_client_disconnect(this, CE_PORT_BINDING);
	    return;
	}
	
	m_last_alive_recv = 0;
	m_last_alive_sent = 0;
	m_count_next = 1;
	
	add_methods ();
	set_sender (m_server);
	
	lo_message msg = lo_message_new();
	lo_send_message_from(target, m_server, PSYNTH_OSC_MSG_CONNECT, msg);
	lo_message_free(msg);

	lo_address_free(target);
	
	m_state = PENDING;
    }
}

void osc_client::close()
{
    lo_server_free(m_server);
    clear();
    m_state = IDLE;
}

void osc_client::disconnect()
{
    if (m_state != IDLE) {
	lo_message msg = lo_message_new();
	broadcast_message (PSYNTH_OSC_MSG_DISCONNECT, msg);
	lo_message_free(msg);
	
	close ();
	notify_client_disconnect(this, CE_NONE);
    }
}

int osc_client::receive(int time_out)
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

int osc_client::update(int msec)
{    
    if (m_state != IDLE) {	
	if (!m_count_next) {
	    m_last_alive_recv += msec;
	    m_last_alive_sent += msec;
	} else
	    m_count_next = 0;

	if (m_last_alive_sent > MIN_ALIVE_DELAY) {
	    lo_message msg = lo_message_new();
	    broadcast_message(PSYNTH_OSC_MSG_ALIVE, msg);
	    lo_message_free(msg);
	    m_last_alive_sent = 0;
	}

	if (m_last_alive_recv > MAX_ALIVE_DELAY) {
	    notify_client_disconnect(this, CE_SERVER_TIMEOUT);
	    close();
	}
	
	if (m_state == CLOSING) {
	    close();
	    notify_client_disconnect(this, CE_SERVER_DROP);
	}
    }

    return m_state;
}

void osc_client::add_methods()
{
    /*
      lo_server_add_method(m_server, NULL, NULL, &lo_generic_handler, NULL);
    */
    
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_DROP, "", &drop_cb, this);
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_ACCEPT, "i", &accept_cb, this);
    lo_server_add_method(m_server, PSYNTH_OSC_MSG_ALIVE, "", &alive_cb, this);
}

int osc_client::_drop_cb(const char* path, const char* types,
			lo_arg** argv, int argc, lo_message msg)
{
    m_state = CLOSING;

    return 0;
}

int osc_client::_accept_cb(const char* path, const char* types,
			  lo_arg** argv, int argc, lo_message msg)
{
    if (m_state == PENDING) {
	m_id = argv[0]->i;
	set_id (m_id);

	get_world()->clear();
	
	osc_controller::add_methods(m_server);
	activate();

	/* FIXME: Make sure that the accept message came from our desired target. */
	lo_address add = lo_message_get_source(msg);
	lo_address addcpy = lo_address_new(lo_address_get_hostname(add),
					   lo_address_get_port(add));
    
	add_target(addcpy);

	lo_message msg = lo_message_new();
	broadcast_message (PSYNTH_OSC_MSG_GET_STATE, msg);
	lo_message_free(msg);
	
	m_state = CONNECTED;
	notify_client_accept(this);
    }
    
    return 0;
}

int osc_client::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    m_last_alive_recv = 0;    
    return 0;
}

} /* namespace psynth */
