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
#include "net/osc_controller.hpp"
#include "net/osc_protocol.hpp"

using namespace std;

namespace psynth
{

osc_controller::osc_controller(bool broadcast, bool restricted) :
    m_world(NULL),
    m_skip(0),
    m_id(0),
    m_activated(false),
    m_broadcast(broadcast),
    m_restricted(restricted)
{
}

osc_controller::~osc_controller()
{
}

void osc_controller::handle_add_node(world_node& obj)
{
    if (!m_skip) {
	int local_id(obj.get_id ());
	pair<int,int> net_id(m_id, local_id);

	m_local_id[net_id] = local_id;
	m_net_id[local_id] = net_id;

	lo_message msg = lo_message_new();

	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_string(msg, obj.get_name().c_str());

	broadcast_message(PSYNTH_OSC_MSG_ADD, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handle_delete_node(world_node& obj)
{
    if (!m_skip) {
	int local_id(obj.get_id ());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();

	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcast_message(PSYNTH_OSC_MSG_DELETE, msg);

	lo_message_free(msg);

	m_local_id.erase(net_id);
	m_net_id.erase(local_id);
    }
}

void osc_controller::handle_activate_node(world_node& obj)
{
    if (!m_skip) {
	int local_id(obj.get_id ());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();

	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcast_message(PSYNTH_OSC_MSG_ACTIVATE, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handle_deactivate_node(world_node& obj)
{
    if (!m_skip) {
	int local_id(obj.get_id ());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();

	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcast_message(PSYNTH_OSC_MSG_DEACTIVATE, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handle_set_param_node(world_node& obj, int param_id)
{
    if (!m_skip) {
	int local_id(obj.get_id ());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();

	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_int32(msg, param_id);

	switch(obj.get_param_type(param_id)) {
	case graph::node_param::INT: {
	    int val;
	    obj.get_param(param_id, val);
	    lo_message_add_int32(msg, val);
	    break;
	}
	case graph::node_param::FLOAT: {
	    float val;
	    obj.get_param(param_id, val);
	    lo_message_add_float(msg, val);
	    break;
	}
	case graph::node_param::STRING: {
	    string val;
	    obj.get_param(param_id, val);
	    lo_message_add_string(msg, val.c_str());
	    break;
	}
	case graph::node_param::VECTOR2F: {
	    base::vector_2f val;
	    obj.get_param(param_id, val);
	    lo_message_add_float(msg, val.x);
	    lo_message_add_float(msg, val.y);
	    break;
	}
	default:
	    break;
	}

	broadcast_message(PSYNTH_OSC_MSG_PARAM, msg);

	lo_message_free(msg);
    }
}

void osc_controller::add_methods (lo_server s)
{
    lo_server_add_method (s, PSYNTH_OSC_MSG_ADD, "iis", &add_cb, this);
    lo_server_add_method (s, PSYNTH_OSC_MSG_DELETE, "ii", &delete_cb, this);
    lo_server_add_method (s, PSYNTH_OSC_MSG_PARAM, NULL, &param_cb, this);
    lo_server_add_method (s, PSYNTH_OSC_MSG_ACTIVATE, "ii", &activate_cb, this);
    lo_server_add_method (s, PSYNTH_OSC_MSG_DEACTIVATE, "ii", &deactivate_cb, this);
}

int osc_controller::_add_cb(const char* path, const char* types,
                            lo_arg** argv, int argc, lo_message msg)
{
    if (!m_restricted || is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	m_skip++;
	world_node obj = m_world->add_node(std::string(&argv[2]->s));
	m_skip--;

	if (!obj.is_null()) {
	    int local_id = obj.get_id ();
	    m_net_id[local_id] = net_id;
	    m_local_id[net_id] = local_id;

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		lo_message_add_string(newmsg, &argv[2]->s);
		broadcast_message_from(PSYNTH_OSC_MSG_ADD, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }

    return 0;
}

int osc_controller::_delete_cb(const char* path, const char* types,
                               lo_arg** argv, int argc, lo_message msg)
{
    if (!m_restricted || is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	world_node obj;

	if (it != m_local_id.end() &&
	    !(obj = m_world->find_node(it->second)).is_null()) {

	    m_skip++;
	    m_world->delete_node(obj);
	    m_skip--;

	    m_local_id.erase(net_id);
	    m_net_id.erase(it->second);

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		broadcast_message_from(PSYNTH_OSC_MSG_DELETE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }

    return 0;
}

int osc_controller::_param_cb(const char* path, const char* types,
                              lo_arg** argv, int argc, lo_message msg)
{
    if (argc < 4)
        return 0;

    if (!m_restricted || is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	world_node obj;

	if (it != m_local_id.end() &&
	    !(obj = m_world->find_node(it->second)).is_null()) {



	    m_skip++;
            int param_type = 0;

            // TODO: CLEAN this shit!
            switch (types[2])
            {
            case 'i':
            {
                int param_id = argv[2]->i;
                switch(param_type = obj.get_param_type(param_id)) {
                case graph::node_param::FLOAT:
                    m_world->set_param_node(obj, param_id, argv[3]->f);
                    break;
                case graph::node_param::INT:
                    m_world->set_param_node(obj, param_id, argv[3]->i);
                    break;
                case graph::node_param::STRING:
                    m_world->set_param_node(obj, param_id, string(&argv[3]->s));
                    break;
                case graph::node_param::VECTOR2F:
                    if (argc < 5)
                        return 0;
                    m_world->set_param_node(
                        obj, param_id, base::vector_2f(argv[3]->f, argv[4]->f));
                    break;
                default:
                    return 0;
                }
                break;
            }

            case 's':
            {
                std::string param_id = &argv[2]->s;
                switch(param_type = obj.get_param_type(param_id)) {
                case graph::node_param::FLOAT:
                    m_world->set_param_node(obj, param_id, argv[3]->f);
                    break;
                case graph::node_param::INT:
                    m_world->set_param_node(obj, param_id, argv[3]->i);
                    break;
                case graph::node_param::STRING:
                    m_world->set_param_node(obj, param_id, string(&argv[3]->s));
                    break;
                case graph::node_param::VECTOR2F:
                    if (argc < 5)
                        return 0;
                    m_world->set_param_node(
                        obj, param_id, base::vector_2f(argv[3]->f, argv[4]->f));
                    break;
                default:
                    return 0;
                }
                break;
            }

            default:
                return 0;
            }


	    m_skip--;

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		lo_message_add_int32(newmsg, argv[2]->i);

		switch(param_type) {
		case graph::node_param::FLOAT:
		    lo_message_add_float(newmsg, argv[3]->f);
		    break;
		case graph::node_param::INT:
		    lo_message_add_int32(newmsg, argv[3]->i);
		    break;
		case graph::node_param::STRING:
		    lo_message_add_string(newmsg, &argv[3]->s);
		    break;
		case graph::node_param::VECTOR2F:
		    lo_message_add_float(newmsg, argv[3]->f);
		    lo_message_add_float(newmsg, argv[4]->f);
		    break;
		default:
		    break;
		}

		broadcast_message_from(PSYNTH_OSC_MSG_PARAM, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }

    return 0;
}

int osc_controller::_activate_cb(const char* path, const char* types,
                                 lo_arg** argv, int argc, lo_message msg)
{
    if (!m_restricted || is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	world_node obj;

	if (it != m_local_id.end() &&
	    !(obj = m_world->find_node(it->second)).is_null()) {

	    m_skip++;
	    m_world->activate_node(obj);
	    m_skip--;

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		broadcast_message_from(PSYNTH_OSC_MSG_ACTIVATE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }

    return 0;
}

int osc_controller::_deactivate_cb(const char* path, const char* types,
                                   lo_arg** argv, int argc, lo_message msg)
{
    if (!m_restricted || is_target (lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	world_node obj;

	if (it != m_local_id.end() &&
	    !(obj = m_world->find_node(it->second)).is_null()) {

	    m_skip++;
	    m_world->deactivate_node(obj);
	    m_skip--;

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		broadcast_message_from(PSYNTH_OSC_MSG_DEACTIVATE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }

    return 0;
}

} /* namespace psynth */
