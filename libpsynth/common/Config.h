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

#ifndef PSYNTH_CONFIG_H
#define PSYNTH_CONFIG_H

#include <list>
#include <map>
#include <iostream>

#include <libpsynth/common/Tree.h>
#include <libpsynth/common/Misc.h>
#include <libpsynth/common/Singleton.h>
#include <libpsynth/common/MapIterator.h>

namespace psynth
{

/*
 * We wanto to put the delegate into our own namespace.
 */
#include <libpsynth/common/FastDelegate.h>
using namespace fastdelegate;

/**
 * The type of a data in the config.
 */
enum ConfType
{
    CONF_NONE,   /**< No config asigned yet. */
    CONF_FLOAT,  /**< @c float data type. */
    CONF_INT,    /**< @c int data type. */
    CONF_STRING, /**< @c string data type. */
    N_CONF_TYPE  /**< Total number of conf types. */
};

class ConfNode;

/**
 * Listener for events in a ConfNode.
 */
class ConfListener {
public:
    /** Virtual destructor. */
    virtual ~ConfListener() {};

    /**
     * The @c ConfNode has been nudged, probably because there has been some
     * changes in its hierarchy or any other meaning that you want to give to
     * it -this is a bit generic event.
     * @param node The node that has raised the event.
     */
    virtual bool handleConfNudge(ConfNode& node) = 0;

    /**
     * The config node has been changed.
     * @param node The node that has raised the event.
     */
    virtual bool handleConfChange(ConfNode& node) = 0;

    /**
     * A child has been added to this node.
     * @param child The child that has been added.
     */
    virtual bool handleConfNewChild(ConfNode& child) = 0;

    /**
     * A child has been added to this node.
     * @param child The child that has been added.
     */
    virtual bool handleConfRemoveChild(ConfNode& child) = 0;
};


//typedef fastdelegate::FastDelegate<bool (const ConfNode&)> ConfEvent;

/**
 * A config event delegate.
 */
typedef fastdelegate::FastDelegate1<ConfNode&, bool> ConfEvent;

/**
 * A ConfNode subject to handle its events.
 */
class ConfSubject
{    
    std::list<ConfListener*> m_list;
    std::list<ConfEvent> m_change_del;
    std::list<ConfEvent> m_nudge_del;
    
public:

    /**
     * Add an event to listen for changes.
     * @param sub The event to add.
     */
    void addChangeEvent(const ConfEvent& sub) {
	m_change_del.push_back(sub);
    }

    /**
     * Add an event to listen for nudges.
     * @param sub The event to add.
     */
    void addNudgeEvent(const ConfEvent& sub) {
	m_nudge_del.push_back(sub);
    }

    /**
     * Add a listener to listen for events.
     * @param l The listener to add.
     */
    void addListener(ConfListener* l) {
	m_list.push_back(l);
    }

    /**
     * Removes an event so it no longer listens for changes.
     * @param sub the event to remove.
     */
    void deleteChangeEvent(const ConfEvent& sub) {
	m_change_del.remove(sub);
    }

    /**
     * Removes an event from the subject so it no longer listens for nudges.
     * @param sub the event to remove.
     */
    void deleteNudgeEvent(const ConfEvent& sub) {
	m_nudge_del.remove(sub);
    }

    /**
     * Removes a listener from the subject so it no longer listens for
     * events here.
     * @param l The listener to remove.
     */
    void deleteListener(ConfListener* l) {
	m_list.remove(l);
    }

    /**
     * Rises a nudge event.
     * @param source The source of the event.
     */
    void notifyConfNudge(ConfNode& source);

    /**
     * Rises a change event.
     * @param source The source of the event.
     */
    void notifyConfChange(ConfNode& source);

    /**
     * Rises a new child creation event.
     * @param child The created child.
     */
    void notifyConfNewChild(ConfNode& child);

    /**
     * Rises a new child creation event.
     * @param child The created child.
     */
    void notifyConfRemoveChild(ConfNode& child);
};

/**
 * A backend for a config system. It implements the actual phisical storing of
 * the data.
 */
class ConfBackend
{
public:
    /** Virtual destructor */
    virtual ~ConfBackend() {}

    /**
     * This is called when the backend has been attached to a node.
     * This allows the backend to start listening to its events in case it
     * provides live update.
     * @param node The node to which it has been attached.
     */
    virtual void attach(ConfNode& node) = 0;

    /**
     * This is called when the backend has been detached from a node.
     * This allows the backend to stop listening to its events in case it
     * provides live update.
     * @param node The node to which it has been attached.
     */
    virtual void datach(ConfNode& node) = 0;

    /**
     * Permanently store the data of the config.
     * @param node The config to store.
     */
    virtual void save(ConfNode& node) = 0;

    /**
     * Load the data -and the hierarchy- in the phisical medium.
     * @param node Where to load the data.
     */
    virtual void load(ConfNode& node) = 0;

    /**
     * Load the data -and the hierarchy- in the phisical medim, but using
     * @c def functions instead of @set so the old values al preserved
     * if the user has already set them.
     */
    virtual void defLoad(ConfNode& node) = 0;
};

/**
 * A configuration node. It stores a value of any type in @c ConfType. It has
 * some childs identifyed by its name crating a whole hierarchy that allows
 * a better organization of the values and the information.
 */
class ConfNode : public ConfSubject,
		 public TreeNode<std::string, ConfNode>
{
    class ConfElement
    {
	union ConfData {
	    int* intptr;
	    float* floatptr;
	    std::string* stringptr;
	};

	ConfType m_type;
	ConfData m_data;
	bool m_unset;
    
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
	    m_type(CONF_NONE),
	    m_unset(true)
	    {}

	~ConfElement() {
	    destroy();
	}

	bool unset() {
	    return m_unset;
	}
    
	ConfType type() const {
	    return m_type;
	}
    
	void set(const float& val) {
	    toType(CONF_FLOAT);
	    *m_data.floatptr = val;
	    m_unset = false;
	}

	void set(const int& val) {
	    toType(CONF_INT);
	    *m_data.intptr = val;
	    m_unset = false;
	}

	void set(const std::string& val) {
	    toType(CONF_STRING);
	    *m_data.stringptr = val;
	    m_unset = false;
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
    };

    ConfElement m_element;
    ConfBackend* m_backend;

    void onInit() {
	m_backend = 0;
	if (getParent())
	    m_backend = getParent()->m_backend;
    }

    void onNewChild(ConfNode& new_child) {
	notifyConfNewChild(new_child);
    }

    void onRemoveChild(ConfNode& new_child) {
	notifyConfRemoveChild(new_child);
    }
    
public:
    /** Constructor */
    ConfNode() :
	m_backend(0)
	{}
    
    /** Destructor. */
    ~ConfNode() {
	if (!getParent() || getParent()->m_backend != m_backend)
	    delete m_backend;
    }
    
    /**
     * Sets the value of the node. The node type is set to @a T, unless
     * @a T is not a valid type in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void set(const T& val) {
	m_element.set(val);
	notifyConfChange(*this);
    }

    /**
     * Sets the value of the node if no previous value has been give to
     * the node. The node type is set to @a T, unless @a T is not a valid type
     * in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void def(const T& val) {
	if (m_element.unset()) {
	    m_element.set(val);
	    notifyConfChange(*this);
	}
    }

    /**
     * Gets the value of the node. Make sure that @a T matches the node type.
     * @param data Somewhere to store the node value.
     */
    template<class T>
    void get(T& data) const {
	m_element.get(data);
    }

    /**
     * Returns the type of the node.
     */
    ConfType type() const {
	return m_element.type();
    }

    /**
     * Changes the type of the node. After calling this the type is changed but
     * the node contains rubbish.
     */
    void setType(ConfType type) {
	m_element.toType(type);
    }

    /**
     * Changes the type of the node only if the node has not been set yet.
     */
    void defType(ConfType type) {
	if (m_element.unset())
	    m_element.toType(type);
    }

    /**
     * Nudges the node. It just raises a nudge event.
     */
    void nudge() {
	notifyConfNudge(*this);
    }

    /**
     * Permanently stores this node hierarchy using the attached backend.
     */
    void save() {
	if (m_backend)
	    m_backend->save(*this);
    }

    /**
     * Loads this node hierarchy from the permanent media.
     */
    void load() {
	if (m_backend)
	    m_backend->load(*this);
    }

    /**
     * Loads this node hierarchy from the permanent media but leaving already
     * set values with their old value.
     */
    void defLoad() {
	if (m_backend)
	    m_backend->defLoad(*this);
    }

    /**
     * Attaches a backend to the node. If another backend has already been
     * attached it is datached.
     * @param backend The backend to attach.
     */
    void attachBackend(ConfBackend* backend) {
	datachBackend();
	m_backend = backend;
	m_backend->attach(*this);
    }

    /**
     * Dataches the currently attached backend to the node.
     */
    void datachBackend() {
	if (m_backend) {
	    m_backend->datach(*this);
	    m_backend = NULL;
	}
    }
};

/**
 * Singleton @c ConfNode to store the global config.
 */
class Config : public ConfNode,
	       public Singleton<Config>
{
    friend class Singleton<Config>;

    /** Hidden constructor. */
    Config() {};

     /** Hidden destructor. */
    ~Config() {};
public:
};

} /* namespace psynth */

#endif /* PSYNTH_CONFIG_H */
