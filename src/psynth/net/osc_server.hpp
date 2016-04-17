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

#include <psynth/net/osc_misc.hpp>
#include <psynth/net/osc_controller.hpp>

#define PSYNTH_DEFAULT_SERVER_PORT     8191
#define PSYNTH_DEFAULT_SERVER_PORT_STR "8191"

namespace psynth
{

class osc_server;

enum osc_server_client_error {
    SCE_NONE = 0,
    SCE_CLIENT_TIMEOUT
};

enum osc_server_error {
    SE_NONE = 0,
    SE_PORT_BINDING
};

class osc_server_listener
{

public:
    virtual ~osc_server_listener() {}
    virtual bool handle_server_start_listening (osc_server* server) = 0;
    virtual bool handle_server_stop_listening (osc_server* server, osc_server_error err) = 0;
    virtual bool handle_server_client_connect (osc_server* server, int client_id) = 0;
    virtual bool handle_server_client_disconnect (osc_server* server, int client_id,
						  osc_server_client_error cause) = 0;
};

class osc_server_subject
{
    std::list<osc_server_listener*> m_list;

protected:
    void notify_server_start_listening (osc_server* server);
    void notify_server_stop_listening (osc_server* server, osc_server_error err);
    void notify_server_client_disconnect (osc_server* server, int client_id,
					  osc_server_client_error cause);
    void notify_server_client_connect (osc_server* server, int client_id);

public:
    void add_listener (osc_server_listener* l) {
	m_list.push_back(l);
    }

    void delete_listener (osc_server_listener* l) {
	m_list.remove(l);
    }
};

class osc_server : public osc_controller,
		   public osc_server_subject
{
public:
    enum state {
	IDLE,
	LISTENING
    };

private:
    const static int SERVER_ID = 0;

    struct slot {
	int id;
	int last_alive_recv;
	int last_alive_sent;

	slot(int id = 0) :
	    id(id), last_alive_recv(0), last_alive_sent(0) {};
    };

    struct lo_address_lt_func {
	bool operator() (lo_address a, lo_address b) {
	    return lo_address_cmp(a, b) < 0;
	}
    };

    typedef std::map<lo_address, slot, lo_address_lt_func> slot_map;
    slot_map m_slots;

    lo_server m_server;
    bool m_listening;
    int m_nextid;
    state m_state;

    LO_HANDLER (osc_server, alive);
    LO_HANDLER (osc_server, connect);
    LO_HANDLER (osc_server, get_state);
    LO_HANDLER (osc_server, disconnect);

    void add_methods ();

public:

    osc_server();
    ~osc_server();

    bool is_listening () {
	return m_listening;
    };

    state get_state () {
	return m_state;
    }

    void listen (const char* port);
    void stop ();
    void close ();

    /* time_out < 0 for blocking operation. */
    int receive (int time_out = 0);
    int update (int msec);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCSERVER_H */
