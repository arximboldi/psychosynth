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

#ifndef PSYNTH_OSCCLIENT_H
#define PSYNTH_OSCCLIENT_H

#include <psynth/net/osc_controller.hpp>

#define PSYNTH_DEFAULT_CLIENT_PORT      8192
#define PSYNTH_DEFAULT_CLIENT_PORT_STR "8192"

namespace psynth
{

class osc_client;

enum osc_client_error {
    CE_NONE = 0,
    CE_PORT_BINDING,
    CE_SERVER_TIMEOUT,
    CE_SERVER_DROP
};

class osc_client_listener
{
public:
    virtual ~osc_client_listener() {}

    virtual bool handle_client_connect(osc_client* client) = 0;
    virtual bool handle_client_disconnect(osc_client* client, osc_client_error err) = 0;
    virtual bool handle_client_accept(osc_client* client) = 0;
};

class osc_client_subject
{
    std::list<osc_client_listener*> m_list;

protected:
    void notify_client_connect(osc_client* param);
    void notify_client_disconnect(osc_client* param, osc_client_error err);
    void notify_client_accept(osc_client* param);

public:
    void add_listener(osc_client_listener* l) {
	m_list.push_back(l);
    };

    void delete_listener(osc_client_listener* l) {
	m_list.remove(l);
    };
};

class osc_client : public osc_controller,
                   public osc_client_subject
{
public:
    enum state {
	IDLE,
	PENDING,
	CONNECTED,
	CLOSING
    };

private:
    lo_address m_target;
    lo_server m_server;
    int m_id;
    state m_state;
    int m_last_alive_recv;
    int m_last_alive_sent;
    int m_count_next;

    LO_HANDLER(osc_client, alive);
    LO_HANDLER(osc_client, drop);
    LO_HANDLER(osc_client, accept);

    void add_methods();
    void close();

public:

    osc_client();
    ~osc_client();

    state get_state() const {
	return m_state;
    }

    void connect(lo_address target, const char* src_port);
    void disconnect();

    /* Timeout < 0 for blocking operation. */
    int receive(int time_out = 0);
    int update(int msec);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCCLIENT_H */
