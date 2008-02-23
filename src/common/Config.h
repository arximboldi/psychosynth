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

#ifndef CONFIG_H
#define CONFIG_H

#include <list>
#include <map>
#include <iostream>

#include "common/Misc.h"
#include "common/Singleton.h"
#include "common/MapIterator.h"
#include "common/FastDelegate.h"


enum ConfType
{
    CONF_NONE,
    CONF_FLOAT,
    CONF_INT,
    CONF_STRING,
    N_CONF_TYPE
};

class ConfElement {
    union ConfData {
	int* intptr;
	float* floatptr;
	std::string* stringptr;
    };

    ConfType m_type;
    ConfData m_data;

    void toType(ConfType type) {
	if (m_type != type) {
	    destroy();

	    m_type = type;
	    switch(m_type) {
	    case CONF_INT:
		m_data.intptr = new int;
		break;
		
	    case CONF_FLOAT:
		m_data.floatptr = new float;
		break;
		
	    case CONF_STRING:
		m_data.stringptr = new std::string;
		break;
		
	    default:
		m_data.intptr = NULL;
	    }
	}   
    }

    void destroy() {
	switch(m_type) {
	case CONF_INT:
	    delete m_data.intptr;
	    break;
	    
	case CONF_FLOAT:
	    delete m_data.floatptr;
	    break;
	    
	case CONF_STRING:
	    delete m_data.stringptr;
	    break;
	    
	default:
	    break;
	}
    }
    

public:
    ConfElement() :
	m_type(CONF_NONE)
	{}

    ~ConfElement() {
	destroy();
    }

    ConfType type() const {
	return m_type;
    }
    
    void set(const float& val) {
	toType(CONF_FLOAT);
	*m_data.floatptr = val;
    }

    void set(const int& val) {
	toType(CONF_INT);
	*m_data.intptr = val;
    }

    void set(const std::string& val) {
	toType(CONF_STRING);
	*m_data.stringptr = val;
    }

    void get(float& val) const {
	if (m_type == CONF_FLOAT)
	    val = *m_data.floatptr;
    }

    void get(int& val) const {
	if (m_type == CONF_INT)
	    val = *m_data.intptr;
    }

    void get(std::string& val) const {
	if (m_type == CONF_STRING)
	    val = *m_data.stringptr; 
    }
};

class ConfNode;

class ConfListener {
public:
    virtual ~ConfListener() {};
    
    virtual bool handleConfChange(const ConfNode& node) = 0;
    virtual bool handleNewChild(const ConfNode& child) = 0;
};

//typedef fastdelegate::FastDelegate<bool (const ConfNode&)> ConfEvent;
typedef fastdelegate::FastDelegate1<const ConfNode&, bool> ConfEvent;
#define MakeEvent fastdelegate::MakeDelegate

class ConfSubject
{    
    std::list<ConfListener*> m_list;
    std::list<ConfEvent> m_change_del;
    
public:
    void addChangeEvent(const ConfEvent& sub) {
	m_change_del.push_back(sub);
    }
    
    void addListener(ConfListener* l) {
	m_list.push_back(l);
    }

    void deleteChangeEvent(const ConfEvent& sub) {
	m_change_del.remove(sub);
    }
    
    void deleteListener(ConfListener* l) {
	m_list.remove(l);
    }

    void notifyConfChange(const ConfNode& source);
    void notifyNewChild(const ConfNode& child);
};

class ConfBackend
{
public:
    virtual ~ConfBackend() {}
    virtual void attach(ConfNode& node) = 0;
    virtual void datach(ConfNode& node) = 0;
    virtual void save(ConfNode& node) = 0;
    virtual void load(ConfNode& node) = 0;
};


class ConfNode : public ConfSubject
{
    /* TODO: This is suboptimal. */
    
    std::map<std::string, ConfNode> m_childs;
    ConfNode* m_parent;
    std::string m_name;
    bool m_isinit;

    ConfElement m_element;
    ConfBackend* m_backend;
   
public:
    typedef MapIterator<std::string, ConfNode> ChildIter;
    
    ConfNode() :
	m_parent(NULL), m_isinit(false), m_backend(NULL)
	{};

    ~ConfNode() {
	if ((!m_parent || m_parent->m_backend != m_backend)
	    && m_backend)
	    delete m_backend;
    }
    
    void init(const std::string& name, ConfNode* parent) {
	m_isinit = true;
	m_name = name;
	m_parent = parent;
	if (m_parent)
	    m_backend = m_parent->m_backend;
    }

    bool isInit() const {
	return m_isinit;
    }

    ChildIter begin() {
	return m_childs.begin();
    }

    ChildIter end() {
	return m_childs.end();
    }
    
    template<class T>
    void set(const T& val) {
	m_element.set(val);
	notifyConfChange(*this);
    }

    template<class T>
    void def(const T& val) {
	if (m_element.type() == CONF_NONE) {
	    m_element.set(val);
	    notifyConfChange(*this);
	}
    }
    
    template<class T>
    void get(T& data) const {
	m_element.get(data);
    }

    ConfType type() const {
	return m_element.type();
    }

    ConfNode& getChild(const std::string& name) {
	if (!m_childs[name].isInit()) {
	    m_childs[name].init(name, this);
	}
	
	return m_childs[name];
    }
    
    ConfNode& getPath(std::string name);

    ConfNode* getParent() const {
	return m_parent;
    }
    
    std::string getPathName() const {
	if (m_parent)
	    return m_parent->getPathName() + "/" + m_name; 
	else
	    return m_name;
    }

    const std::string& getName() const {
	return m_name;
    }
    
    void save() {
	if (m_backend)
	    m_backend->save(*this);
    }
    
    void load() {
	if (m_backend)
	    m_backend->load(*this);
    }

    void attachBackend(ConfBackend* backend) {
	m_backend = backend;
	m_backend->attach(*this);
    }

    void datachBackend() {
	if (m_backend) {
	    m_backend->datach(*this);
	    m_backend = NULL;
	}
    }
};

class Config : public ConfNode,
	       public Singleton<Config>
{
};

#endif /* CONFIG_H */
