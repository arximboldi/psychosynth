/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef PSYNTH_TABLE_H
#define PSYNTH_TABLE_H

#include <list>
#include <map>
#include <iostream>

#include <libpsynth/common/audio_info.h>
#include <libpsynth/table/Patcher.h>
#include <libpsynth/node/node.h>
#include <libpsynth/node/node_output.h>
#include <libpsynth/node/node_audio_mixer.h>
#include <libpsynth/node/node_manager.h>
#include <libpsynth/node/node_factory_manager.h>

namespace psynth
{

class Table;
class TableObjectListener;

class TableObject
{
    friend class Table;
    
    node* m_obj; /* Use node_manager::Iterator instead? */
    Table* m_table;

    TableObject(node* obj, Table* table) :
	m_obj(obj), m_table(table) {};

public:
    TableObject() :
	m_obj(NULL), m_table(NULL) {}
    
    TableObject(const TableObject& o) :
	m_obj(o.m_obj), m_table(o.m_table) {};

    bool isNull() const {
	return m_obj == NULL;
    };

    int getID() const {
	return m_obj->get_id ();
    };
    
    int getType() const {
	return m_obj->get_type ();
    };

    const std::string& getName() {
	return m_obj->get_name ();
    };
    
    Table* getTable() {
	return m_table;
    }
    
    inline void addListener(TableObjectListener* cl);

    inline void deleteListener(TableObjectListener* cl);

    inline void deleteMe();
    
    inline void activate();

    inline void deactivate();

    template <typename T>
    inline void setParam(int id, const T& data);

    template <typename T>
    inline void setParam(const std::string& name, const T& data);

    void attachWatch(int type, int in_sock, watch* watch) {
	m_obj->attach_watch (type, in_sock, watch);
    }

    void detachWatch(int type, int in_sock, watch* watch) {
	m_obj->detach_watch (type, in_sock, watch);
    }
    
    int getParamID(const std::string& name) const {
	m_obj->param(name).get_id ();
    }

    const std::string& getParamName(int id) const {
	m_obj->param(id).get_name ();
    }
    
    int getParamType(int id) const {
	return m_obj->param(id).type();
    }

    int getParamType(const std::string& name) const {
	return m_obj->param(name).type();
    }
    
    template <typename T>
    void getParam(int id, T& data) const {
	m_obj->param(id).get(data);
    }

    template <typename T>
    void getParam(const std::string& name, T& data) const {
	m_obj->param(name).get(data);
    }

    bool operator==(const TableObject& o) const {
	return m_obj == o.m_obj;
    }

    bool operator!=(const TableObject& o) const {
	return m_obj != o.m_obj;
    }
};

class TableListener
{
public:
    virtual ~TableListener() {};
    virtual void handleAddObject(TableObject& obj) = 0;
    virtual void handleDeleteObject(TableObject& obj) = 0;
};

class TableObjectListener
{
public:
    virtual ~TableObjectListener() {};
    virtual void handleActivateObject(TableObject& obj) = 0;
    virtual void handleDeactivateObject(TableObject& obj) = 0;
    virtual void handleSetParamObject(TableObject& ob, int id) = 0;
};

struct TablePatcherEvent
{
    TableObject src;
    TableObject dest;
    int src_socket;
    int dest_socket;
    int socket_type;
    
    TablePatcherEvent(const TableObject& s, const TableObject& d,
	      int ss, int ds, int st) :
	src(s), dest(d), src_socket(ss), dest_socket(ds), socket_type(st) {}
};

class TablePatcherListener
{
public:
    virtual ~TablePatcherListener() {};
    virtual void handleLinkAdded(const TablePatcherEvent& ev) = 0;
    virtual void handleLinkDeleted(const TablePatcherEvent& ev) = 0;
};

class TableSubject
{
    typedef std::list<TableObjectListener*>::iterator ObjectListenerIter;
    typedef std::list<TableListener*>::iterator ListenerIter;
    
    std::list<TableListener*> m_listeners;
    std::list<TablePatcherListener*> m_patch_list;
    std::map<int, std::list<TableObjectListener*> > m_obj_listeners; 
    std::list<TableObjectListener*> m_all_obj_list;
    
protected:
    void notifyAddObject(TableObject& obj);
    void notifyDeleteObject(TableObject& obj);
    void notifyActivateObject(TableObject& obj);
    void notifyDeactivateObject(TableObject& obj);
    void notifySetParamObject(TableObject& obj, int param_id);
    void notifyLinkAdded(const TablePatcherEvent& ev);
    void notifyLinkDeleted(const TablePatcherEvent& ev);
    
public:
    void addTableListener(TableListener* cl) {
	m_listeners.push_back(cl);
    };

    void addTablePatcherListener(TablePatcherListener* cl) {
	m_patch_list.push_back(cl);
    };

    void addTableObjectListener(TableObjectListener* cl) {
	m_all_obj_list.push_back(cl);
    };
    
    void addTableObjectListener(TableObject& obj, TableObjectListener* cl) {
	std::map<int, std::list<TableObjectListener*> >::iterator it;
	
	if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end())
	    it->second.push_back(cl);
    };
    
    void deleteTableListener(TableListener* cl) {
	m_listeners.remove(cl);
    };

    void deleteTablePatcherListener(TablePatcherListener* cl) {
	m_patch_list.remove(cl);
    };

    void deleteTableObjectListener(TableObjectListener* cl) {
	m_all_obj_list.remove(cl);
    };
    
    void deleteTableObjectListener(TableObject& obj, TableObjectListener* cl) {
	std::map<int, std::list<TableObjectListener*> >::iterator it;
	
	if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end())
	    it->second.remove(cl);
    };
};

class Table : public TableSubject,
	      public PatcherListener
{    
    audio_info m_info;
    node_manager m_node_mgr;
    Patcher* m_patcher;
    node_output* m_output;
    node_audio_mixer* m_mixer;
    int m_last_id;
    node_factory_manager m_objfact;
    
    static const int MIXER_CHANNELS = 16;

    void registerDefaultObjectFactory();
public:
    enum {
	OUTPUT_ID = 0,
	MIXER_ID,
	MIN_USER_ID = 1024
    };
    
    Table(const audio_info& info);
    ~Table();

    void clear();
    
    const audio_info& getInfo() const {
	return m_info;
    };

    void setInfo(const audio_info& info) {
	if (m_info != info)
	    m_node_mgr.set_info (info);
    }

    void registerObjectFactory(node_factory& f) {
	m_objfact.register_factory (f);
    }
    
    TableObject findObject(int id);

    TableObject addObject(int type);
    TableObject addObject(const std::string& type_name);
    
    template <typename T>
    void setParamObject(TableObject& obj, int id, const T& data) {
	obj.m_obj->param(id).set(data);
	notifySetParamObject(obj, id);
	if (m_patcher)
	    m_patcher->setParamNode(obj.m_obj, id);
    }

    template <typename T>
    void setParamObject(TableObject& obj, const std::string& name, const T& data) {
	node_param& param = obj.m_obj->param(name);
	param.set(data);
	notifySetParamObject(obj, param.get_id());
	if (m_patcher)
	    m_patcher->setParamNode(obj.m_obj, param.get_id());
    }
    
    void deleteObject(TableObject& obj);

    void activateObject(TableObject& obj);

    void deactivateObject(TableObject& obj);

    void attachOutput(output* out) {
	m_output->attach_output (out);
    };

    void attachPassiveOutput(output* out) {
	m_output->attach_passive_output (out);
    };
    
    void detachOutput(output* out) {
	m_output->detach_output (out);
    };

    void detachPassiveOutput(output* out) {
	m_output->detach_passive_output (out);
    };

    void attachPatcher(Patcher* pat);
    
    void detachPatcher();
    
    void update() {
	if (m_patcher)
	    m_patcher->update();
    }

    void handleLinkAdded(const PatcherEvent& ev) {
	notifyLinkAdded(TablePatcherEvent(TableObject(ev.src,this),
					  TableObject(ev.dest,this),
					  ev.src_socket,
					  ev.dest_socket,
					  ev.socket_type));
    };
    
    void handleLinkDeleted(const PatcherEvent& ev) {
	notifyLinkDeleted(TablePatcherEvent(TableObject(ev.src,this),
					  TableObject(ev.dest,this),
					  ev.src_socket,
					  ev.dest_socket,
					  ev.socket_type));
    }
};

template <typename T>
void TableObject::setParam(int id, const T& data) {
    m_table->setParamObject(*this, id, data);
};

template <typename T>
void TableObject::setParam(const std::string& name, const T& data) {
    m_table->setParamObject(*this, name, data);
};

void TableObject::addListener(TableObjectListener* cl) {
    m_table->addTableObjectListener(*this, cl);
}

void TableObject::deleteListener(TableObjectListener* cl) {
    m_table->deleteTableObjectListener(*this, cl);
}

void TableObject::activate() {
    m_table->activateObject(*this);
}

void TableObject::deleteMe() {
    m_table->deleteObject(*this);
}

void TableObject::deactivate() {
    m_table->deactivateObject(*this);
}

} /* namespace psynth */

#endif /* PSYNTH_TABLE_H */
