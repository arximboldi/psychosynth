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

using namespace std;

OSCController::OSCController(bool is_server) :
    m_table(NULL),
    m_skip(0),
    m_id(0),
    m_is_server(is_server)
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

	broadcastMessage("ps/add", msg);

	lo_message_free(msg);
    } else
	m_skip--;
}

void OSCController::handleDeleteObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage("ps/delete", msg);

	lo_message_free(msg);

	m_local_id.erase(net_id);
	m_net_id.erase(local_id);
    } else
	m_skip--;
}

void OSCController::handleMoveObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_float(msg, obj.getX());
	lo_message_add_float(msg, obj.getY());
	    
	broadcastMessage("ps/move", msg);

	lo_message_free(msg);		
    } else
	m_skip--;
}

void OSCController::handleActivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage("ps/activate", msg);

	lo_message_free(msg);
    } else
	m_skip--;
}

void OSCController::handleDeactivateObject(TableObject& obj)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);

	broadcastMessage("ps/deactivate", msg);

	lo_message_free(msg);
    } else
	m_skip--;
}

void OSCController::handleSetParamObject(TableObject& obj, int param_id)
{
    if (!m_skip) {
	int local_id(obj.getID());
	pair<int,int> net_id = m_net_id[local_id];

	lo_message msg = lo_message_new();
	
	lo_message_add_int32(msg, net_id.first);
	lo_message_add_int32(msg, net_id.second);
	lo_message_add_int32(msg, param_id);

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
	default:
	    break;
	}
	
	broadcastMessage("ps/param", msg);

	lo_message_free(msg);	
    } else
	m_skip--;
}

void OSCController::addToServer(lo_server s)
{
    lo_server_add_method (s, "/ps/add", "iii", &add_cb, this);
    lo_server_add_method (s, "/ps/delete", "ii", &delete_cb, this);
    lo_server_add_method (s, "/ps/move", "iiff", &move_cb, this);
    lo_server_add_method (s, "/ps/param", "iiif", &param_cb, this);
    lo_server_add_method (s, "/ps/param", "iiii", &param_cb, this);
    lo_server_add_method (s, "/ps/param", "iiis", &param_cb, this);
    lo_server_add_method (s, "/ps/activate", "ii", &activate_cb, this);
    lo_server_add_method (s, "/ps/deactivate", "ii", &deactivate_cb, this);
}

int OSCController::_add_cb(const char* path, const char* types,
			   lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;
    
    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    TableObject obj = m_table->addObject(argv[2]->i);

    if (!obj.isNull()) {
	int local_id = obj.getID();
	m_net_id[local_id] = net_id;
	m_local_id[net_id] = local_id;

	if (m_is_server)
	    broadcastMessageFrom("/ps/add", msg, lo_message_get_source(msg));
    }

    return 0;
}

int OSCController::_delete_cb(const char* path, const char* types,
			      lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;

    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
    TableObject obj;
    
    if (it != m_local_id.end() &&
	!(obj = m_table->findObject(it->second)).isNull()) {
	
	m_table->deleteObject(obj);

	m_local_id.erase(net_id);
	m_net_id.erase(it->second);

	if (m_is_server)
	    broadcastMessageFrom("/ps/delete", msg, lo_message_get_source(msg));
    }

    return 0;
}

int OSCController::_move_cb(const char* path, const char* types,
			    lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;

    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
    TableObject obj;
    
    if (it != m_local_id.end() &&
	!(obj = m_table->findObject(it->second)).isNull()) {

	m_table->moveObject(obj, argv[2]->f, argv[3]->f);

	if (m_is_server)
	    broadcastMessageFrom("/ps/move", msg, lo_message_get_source(msg));
    }

    return 0;
}

int OSCController::_param_cb(const char* path, const char* types,
			     lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;

    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
    TableObject obj;
    
    if (it != m_local_id.end() &&
	!(obj = m_table->findObject(it->second)).isNull()) {
	
	switch(types[2]) {
	case LO_FLOAT:
	    m_table->setParamObject(obj, argv[2]->i, argv[3]->f);
	    break;
	case LO_INT32:
	    m_table->setParamObject(obj, argv[2]->i, argv[3]->i);
	    break;
	case LO_STRING:
	    m_table->setParamObject(obj, argv[2]->i, string(&argv[3]->s));
	    break;
	default:
	    break;
	}
	
	if (m_is_server)
	    broadcastMessageFrom("/ps/param", msg, lo_message_get_source(msg));
    }

    return 0;
}

int OSCController::_activate_cb(const char* path, const char* types,
				lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;

    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
    TableObject obj;
    
    if (it != m_local_id.end() &&
	!(obj = m_table->findObject(it->second)).isNull()) {

	m_table->activateObject(obj);

	if (m_is_server)
	    broadcastMessageFrom("/ps/activate", msg, lo_message_get_source(msg));
    }

    return 0;
}

int OSCController::_deactivate_cb(const char* path, const char* types,
				  lo_arg** argv, int argc, lo_message msg)
{
    m_skip++;

    pair<int,int> net_id(argv[0]->i, argv[1]->i);

    map<pair<int,int>, int>::iterator it = m_local_id.find(net_id);
    TableObject obj;
    
    if (it != m_local_id.end() &&
	!(obj = m_table->findObject(it->second)).isNull()) {
	
	m_table->deactivateObject(obj);

	if (m_is_server)
	    broadcastMessageFrom("/ps/deactivate", msg, lo_message_get_source(msg));
    }
    
    return 0;
}
