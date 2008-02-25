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

#include "net/OSCController.h"
#include "net/OSCProtocol.h"

using namespace std;

namespace psynth
{

OSCController::OSCController(bool broadcast) :
    m_table(NULL),
    m_skip(0),
    m_id(0),
    m_activated(false),
    m_broadcast(broadcast)
{
}

OSCController::~OSCController()
{
}
    
void OSCController::handleAddObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id(m_id, local_id);

	m_local_id[net_id] = local_id;
	m_net_id[local_id] = net_id;
	
	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_int32(msg, obj.getType());

	broadcastMessage(MSG_ADD, msg);

	lo_message_free(msg);
    }
}

void OSCController::handleDeleteObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage(MSG_DELETE, msg);

	lo_message_free(msg);

	m_local_id.erase(net_id);
	m_net_id.erase(local_id);
    }
}

void OSCController::handleActivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage(MSG_ACTIVATE, msg);

	lo_message_free(msg);
    }
}

void OSCController::handleDeactivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage(MSG_DEACTIVATE, msg);

	lo_message_free(msg);
    }
}

void OSCController::handleSetParamObject(TableObject& obj, Object::ParamID param_id)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_int32(msg, param_id.scope);
	lo_message_add_int32(msg, param_id.id);

	switch(obj.getParamType(param_id)) {
	case Object::PARAM_INT: {
	    int val;
	    obj.getParam(param_id, val);
	    lo_message_add_int32(msg, val);
	    break;
	}    
	case Object::PARAM_FLOAT: {
	    float val;
	    obj.getParam(param_id, val);
	    lo_message_add_float(msg, val);
	    break;
	}  
	case Object::PARAM_STRING: {
	    string val;
	    obj.getParam(param_id, val);
	    lo_message_add_string(msg, val.c_str());
	    break;
	}
	case Object::PARAM_VECTOR2F: {
	    Vector2f val;
	    obj.getParam(param_id, val);
	    lo_message_add_float(msg, val.x);
	    lo_message_add_float(msg, val.y);
	    break;
	}  
	default:
	    break;
	}
	
	broadcastMessage(MSG_PARAM, msg);

	lo_message_free(msg);
    }
}

void OSCController::addMethods(lo_server s)
{
    lo_server_add_method (s, MSG_ADD, "iii", &add_cb, this);
    lo_server_add_method (s, MSG_DELETE, "ii", &delete_cb, this);
    lo_server_add_method (s, MSG_PARAM, NULL, &param_cb, this);
    lo_server_add_method (s, MSG_ACTIVATE, "ii", &activate_cb, this);
    lo_server_add_method (s, MSG_DEACTIVATE, "ii", &deactivate_cb, this);
}

int OSCController::_add_cb(const char* path, const char* types,
			   lo_arg** argv, int argc, lo_message msg)
{
    if (isDestiny(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	m_skip++;
	TableObject obj = m_table->addObject(argv[2]->i);
	m_skip--;
	
	if (!obj.isNull()) {
	    int local_id = obj.getID();
	    m_net_id[local_id] = net_id;
	    m_local_id[net_id] = local_id;

	    if (m_broadcast) {
		lo_message newmsg = lo_message_new();
		lo_message_add_int32(newmsg, argv[0]->i);
		lo_message_add_int32(newmsg, argv[1]->i);
		lo_message_add_int32(newmsg, argv[2]->i);
		broadcastMessageFrom(MSG_ADD, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }
    
    return 0;
}

int OSCController::_delete_cb(const char* path, const char* types,
			      lo_arg** argv, int argc, lo_message msg)
{
    if (isDestiny(lo_message_get_source(msg))) {
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
		broadcastMessageFrom(MSG_DELETE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }
    
    return 0;
}

int OSCController::_param_cb(const char* path, const char* types,
			     lo_arg** argv, int argc, lo_message msg)
{
    if (isDestiny(lo_message_get_source(msg))) {
	pair<int,int> net_id(argv[0]->i, argv[1]->i);

	map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
	TableObject obj;
    
	if (it != m_local_id.end() &&
	    !(obj = m_table->findObject(it->second)).isNull()) {

	    Object::ParamID param_id((Object::ParamScope)argv[2]->i, argv[3]->i);
	    
	    m_skip++;
	    switch(obj.getParamType(param_id)) {
	    case Object::PARAM_FLOAT:
		m_table->setParamObject(obj, param_id, argv[4]->f);
		break;
	    case Object::PARAM_INT:
		m_table->setParamObject(obj, param_id, argv[4]->i);
		break;
	    case Object::PARAM_STRING:
		m_table->setParamObject(obj, param_id, string(&argv[4]->s));
		break;
	    case Object::PARAM_VECTOR2F:
		m_table->setParamObject(obj, param_id,
					Vector2f(argv[4]->f, argv[5]->f));
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
		lo_message_add_int32(newmsg, argv[3]->i);
		
		switch(obj.getParamType(param_id)) {
		case Object::PARAM_FLOAT:
		    lo_message_add_float(newmsg, argv[4]->f);
		    break;
		case Object::PARAM_INT:
		    lo_message_add_int32(newmsg, argv[4]->i);
		    break;
		case Object::PARAM_STRING:
		    lo_message_add_string(newmsg, &argv[4]->s);
		    break;
		case Object::PARAM_VECTOR2F:
		    lo_message_add_float(newmsg, argv[4]->f);
		    lo_message_add_float(newmsg, argv[5]->f);
		    break;
		default:
		    break;
		}
		
		broadcastMessageFrom(MSG_PARAM, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }
    
    return 0;
}

int OSCController::_activate_cb(const char* path, const char* types,
				lo_arg** argv, int argc, lo_message msg)
{
    if (isDestiny(lo_message_get_source(msg))) {
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
		broadcastMessageFrom(MSG_ACTIVATE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }
    
    return 0;
}

int OSCController::_deactivate_cb(const char* path, const char* types,
				  lo_arg** argv, int argc, lo_message msg)
{
    if (isDestiny(lo_message_get_source(msg))) {
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
		broadcastMessageFrom(MSG_DEACTIVATE, newmsg, lo_message_get_source(msg));
		lo_message_free(newmsg);
	    }
	}
    }
    
    return 0;
}

} /* namespace psynth */
