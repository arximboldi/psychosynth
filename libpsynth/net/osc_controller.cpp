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
#include "net/osc_controller.h"
#include "net/osc_protocol.h"

using namespace std;

namespace psynth
{

osc_controller::osc_controller(bool broadcast) :
    m_table(NULL),
    m_skip(0),
    m_id(0),
    m_activated(false),
    m_broadcast(broadcast)
{
}

osc_controller::~osc_controller()
{
}
    
void osc_controller::handleAddObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id(m_id, local_id);

	m_local_id[net_id] = local_id;
	m_net_id[local_id] = net_id;
	
	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_string(msg, obj.getName().c_str());

	broadcast_message(PSYNTH_OSC_MSG_ADD, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handleDeleteObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
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

void osc_controller::handleActivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcast_message(PSYNTH_OSC_MSG_ACTIVATE, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handleDeactivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcast_message(PSYNTH_OSC_MSG_DEACTIVATE, msg);

	lo_message_free(msg);
    }
}

void osc_controller::handleSetParamObject(TableObject& obj, int param_id)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_int32(msg, param_id);

	switch(obj.getParamType(param_id)) {
	case ObjParam::INT: {
	    int val;
	    obj.getParam(param_id, val);
	    lo_message_add_int32(msg, val);
	    break;
	}    
	case ObjParam::FLOAT: {
	    float val;
	    obj.getParam(param_id, val);
	    lo_message_add_float(msg, val);
	    break;
	}  
	case ObjParam::STRING: {
	    string val;
	    obj.getParam(param_id, val);
	    lo_message_add_string(msg, val.c_str());
	    break;
	}
	case ObjParam::VECTOR2F: {
	    vector_2f val;
	    obj.getParam(param_id, val);
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
    if (is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	m_skip++;
	TableObject obj = m_table->addObject(std::string(&argv[2]->s));
	m_skip--;
	
	if (!obj.isNull()) {
	    int local_id = obj.getID();
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
    if (is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	TableObject obj;
    
	if (it != m_local_id.end() &&
	    !(obj = m_table->findObject(it->second)).isNull()) {

	    m_skip++;
	    m_table->deleteObject(obj);
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
    if (is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	TableObject obj;
    
	if (it != m_local_id.end() &&
	    !(obj = m_table->findObject(it->second)).isNull()) {

	    int param_id = argv[2]->i;
	    
	    m_skip++;
	    switch(obj.getParamType(param_id)) {
	    case ObjParam::FLOAT:
		m_table->setParamObject(obj, param_id, argv[3]->f);
		break;
	    case ObjParam::INT:
		m_table->setParamObject(obj, param_id, argv[3]->i);
		break;
	    case ObjParam::STRING:
		m_table->setParamObject(obj, param_id, string(&argv[3]->s));
		break;
	    case ObjParam::VECTOR2F:
		m_table->setParamObject(obj, param_id,
					vector_2f(argv[3]->f, argv[4]->f));
		break;
	    default:
		break;
	    }
	    m_skip--;

	    if (m_broadcast) {   
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		lo_message_add_int32(newmsg, argv[2]->i);
		
		switch(obj.getParamType(param_id)) {
		case ObjParam::FLOAT:
		    lo_message_add_float(newmsg, argv[3]->f);
		    break;
		case ObjParam::INT:
		    lo_message_add_int32(newmsg, argv[3]->i);
		    break;
		case ObjParam::STRING:
		    lo_message_add_string(newmsg, &argv[3]->s);
		    break;
		case ObjParam::VECTOR2F:
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
    if (is_target(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	TableObject obj;
    
	if (it != m_local_id.end() &&
	    !(obj = m_table->findObject(it->second)).isNull()) {

	    m_skip++;
	    m_table->activateObject(obj);
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
    if (is_target (lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	TableObject obj;
    
	if (it != m_local_id.end() &&
	    !(obj = m_table->findObject(it->second)).isNull()) {
	
	    m_skip++;
	    m_table->deactivateObject(obj);
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
