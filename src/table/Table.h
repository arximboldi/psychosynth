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

#ifndef CORE_H
#define CORE_H

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

class TableObject
{
    friend class Table;
    
    Object* m_obj;
    Table* m_table;

    TableObject(Object* obj, Table* table) :
	m_obj(obj), m_table(table) {};

public:    
    TableObject(const TableObject& o) :
	m_obj(o.m_obj), m_table(o.m_table) {};

    bool isNull() const {
	return m_obj == NULL;
    };

    int getID() const {
	return m_obj->getID();
    };
    
    int getType() const {
	return m_obj->getID();
    };
    
    Real getX() const {
	return m_obj->getX();
    };
    
    Real getY() const {
	return m_obj->getY();
    };
    
    std::pair<Real, Real> getPosition() const {
	return std::pair<Real, Real>(m_obj->getX(), m_obj->getY());
    };

    inline void move(Real x, Real y);
    
    template <typename T>
    inline void setParam(int id, const T& data);

    void getParamType(int id) {
	m_obj->getParamType(id);
    };

    template <typename T>
    void getParam(int id, T& data) {
	m_obj->getParam(id, data);
    };
};

class TableListener {
public:
    virtual ~TableListener() {};
    virtual void handleAddObject(TableObject& obj) = 0;
    virtual void handleDeleteObject(TableObject& obj) = 0;
    virtual void handleMoveObject(TableObject& obj) = 0;
    virtual void handleActivateObject(TableObject& obj) = 0;
    virtual void handleDeactivateObject(TableObject& obj) = 0;
    virtual void handleSetParamObject(TableObject& ob, int param_id) = 0;
};

class TableSubject {
    typedef std::list<TableListener*>::iterator ListenerIter;
    std::list<TableListener*> m_listeners;
	
public:
    void addListener(TableListener* cl) {
	m_listeners.push_back(cl);
    };
	
    void delListener(TableListener* cl) {
	m_listeners.remove(cl);
    };
	
    void notifyAddObject(TableObject& obj) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleAddObject(obj);
    }

    void notifyMoveObject(TableObject& obj) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleMoveObject(obj);
    }

    void notifyDeleteObject(TableObject& obj) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleDeleteObject(obj);
    }

    void notifyActivateObject(TableObject& obj) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleDeleteObject(obj);
    }

    void notifyDeactivateObject(TableObject& obj) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleDeactivateObject(obj);
    }

    void notifySetParamObject(TableObject& obj, int param_id) {
	for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	    (*i)->handleSetParamObject(obj, param_id);	
    }
};

class Table: public TableSubject {
    AudioInfo m_info;
    ObjectManager m_objmgr;
    Patcher m_patcher;
    ObjectOutput* m_output;
    ObjectMixer* m_mixer;

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

    TableObject addObject(int type, int id);

    void moveObject(TableObject& obj, Real x, Real y);

    template <typename T>
    void setParamObject(TableObject& obj, int id, const T& data);

    void deleteObject(TableObject& obj);

    void activateObject(TableObject& obj);

    void deactivateObject(TableObject& obj);

    void attachOutput(Output* out) {
	m_output->attachOutput(out);
    };

    void dattachOutput(Output* out) {
	m_output->detachOutput(out);
    };
    
    void update() {
	m_patcher.update();
    }
};

void TableObject::move(Real x, Real y) {
    m_table->moveObject(*this, x, y);
};
    
template <typename T>
void TableObject::setParam(int id, const T& data) {
    m_table->setParamObject(*this, id, data);
};

template <typename T>
void Table::setParamObject(TableObject& obj, int id, const T& data)
{
    obj.m_obj->setParam(id, data);
    notifySetParamObject(obj, id);
}

#endif
