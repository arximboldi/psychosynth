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

#ifndef TABLE_H
#define TABLE_H

#include <list>
#include <map>
#include <iostream>

#include "common/AudioInfo.h"
#include "table/Patcher.h"
#include "object/Object.h"
#include "object/ObjectOutput.h"
#include "object/ObjectManager.h"
#include "output/Output.h"

class Table;
class TableObjectListener;

class TableObject
{
    friend class Table;
    
    Object* m_obj;
    Table* m_table;

    TableObject(Object* obj, Table* table) :
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
	return m_obj->getID();
    };
    
    int getType() const {
	return m_obj->getType();
    };
    
    Real getX() const {
	return m_obj->getX();
    };
    
    Real getY() const {
	return m_obj->getY();
    };

    Table* getTable() {
	return m_table;
    }
    
    std::pair<Real, Real> getPosition() const {
	return std::pair<Real, Real>(m_obj->getX(), m_obj->getY());
    };

    inline void move(Real x, Real y);
    
    template <typename T>
    inline void setParam(int id, const T& data);

    inline void addListener(TableObjectListener* cl);

    inline void deleteListener(TableObjectListener* cl);

    inline void deleteMe();
    
    inline void activate();

    inline void deactivate();
    
    int getParamType(int id) {
	return m_obj->getParamType(id);
    };
    
    template <typename T>
    void getParam(int id, T& data) {
	m_obj->getParam(id, data);
    };

    bool operator==(const TableObject& o) {
	return m_obj == o.m_obj;
    }

    bool operator!=(const TableObject& o) {
	return m_obj != o.m_obj;
    }
};

class TableListener {
public:
    virtual ~TableListener() {};
    virtual void handleAddObject(TableObject& obj) = 0;
    virtual void handleDeleteObject(TableObject& obj) = 0;
};

class TableObjectListener {
public:
    virtual ~TableObjectListener() {};
    virtual void handleMoveObject(TableObject& obj) = 0;
    virtual void handleActivateObject(TableObject& obj) = 0;
    virtual void handleDeactivateObject(TableObject& obj) = 0;
    virtual void handleSetParamObject(TableObject& ob, int param_id) = 0;
};

struct TablePatcherEvent {
    TableObject src;
    TableObject dest;
    int src_socket;
    int dest_socket;
    int socket_type;
    
    TablePatcherEvent(const TableObject& s, const TableObject& d,
		      int ss, int ds, int st) :
	src(s), dest(d), src_socket(ss), dest_socket(ds), socket_type(st) {}
};

class TablePatcherListener {
public:
    virtual ~TablePatcherListener() {};
    virtual void handleLinkAdded(const TablePatcherEvent& ev) = 0;
    virtual void handleLinkDeleted(const TablePatcherEvent& ev) = 0;
};

class TableSubject {
    typedef std::list<TableListener*>::iterator ListenerIter;
    typedef std::list<TableObjectListener*>::iterator ObjectListenerIter;
    
    std::list<TableListener*> m_listeners;
    std::list<TablePatcherListener*> m_patch_list;
    std::map<int, std::list<TableObjectListener*> > m_obj_listeners; 

protected:
    void notifyAddObject(TableObject& obj);
    void notifyDeleteObject(TableObject& obj);
    void notifyMoveObject(TableObject& obj);
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
    
    void deleteTableObjectListener(TableObject& obj, TableObjectListener* cl) {
	std::map<int, std::list<TableObjectListener*> >::iterator it;
	
	if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end())
	    it->second.remove(cl);
    };
};

class Table: public TableSubject, public PatcherListener {
    AudioInfo m_info;
    ObjectManager m_objmgr;
    Patcher* m_patcher;
    ObjectOutput* m_output;
    ObjectMixer* m_mixer;
    int m_last_id;
    
    static const int MIXER_CHANNELS = 16;

public:
    enum {
	OUTPUT_ID = 0,
	MIXER_ID,
	MIN_USER_ID = 1024
    };
    
    Table(const AudioInfo& info);
    ~Table();

    const AudioInfo& getInfo() const;

    TableObject findObject(int id);

    TableObject addObject(int type);

    void moveObject(TableObject& obj, Real x, Real y);

    template <typename T>
    void setParamObject(TableObject& obj, int id, const T& data) {
	obj.m_obj->setParam(id, data);
	notifySetParamObject(obj, id);
    }
    
    void deleteObject(TableObject& obj);

    void activateObject(TableObject& obj);

    void deactivateObject(TableObject& obj);

    void attachOutput(Output* out) {
	m_output->attachOutput(out);
    };

    void dattachOutput(Output* out) {
	m_output->detachOutput(out);
    };

    void attachPatcher(Patcher* pat);
    
    void dattachPatcher();
    
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

void TableObject::move(Real x, Real y) {
    m_table->moveObject(*this, x, y);
};
    
template <typename T>
void TableObject::setParam(int id, const T& data) {
    m_table->setParamObject(*this, id, data);
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

#endif
