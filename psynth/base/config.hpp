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

#include <psynth/base/signal.hpp>
#include <psynth/base/tree.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/base/singleton.hpp>

namespace psynth
{

/**
 * The type of a data in the config.
 */
enum conf_type
{
    CONF_NONE,   /**< No config asigned yet. */
    CONF_FLOAT,  /**< @c float data type. */
    CONF_INT,    /**< @c int data type. */
    CONF_STRING, /**< @c string data type. */
    N_CONF_TYPE  /**< Total number of conf types. */
};

class conf_node;

/**
 * Listener for events in a conf_node.
 */
class conf_listener
{
public:
    /** Virtual destructor. */
    virtual ~conf_listener() {};

    /**
     * The @c conf_node has been nudged, probably because there has been some
     * changes in its hierarchy or any other meaning that you want to give to
     * it -this is a bit generic event.
     * @param node The node that has raised the event.
     */
    virtual bool handle_conf_nudge (conf_node& node) = 0;

    /**
     * The config node has been changed.
     * @param node The node that has raised the event.
     */
    virtual bool handle_conf_change (conf_node& node) = 0;

    /**
     * A child has been added to this node.
     * @param child The child that has been added.
     */
    virtual bool handle_conf_new_child (conf_node& child) = 0;

    /**
     * A child has been added to this node.
     * @param child The child that has been added.
     */
    virtual bool handle_conf_remove_child (conf_node& child) = 0;
};

/* TODO */
#if 0
//typedef fastdelegate::FastDelegate<bool (const conf_node&)> ConfEvent;

/**
 * A config event delegate.
 */
typedef fastdelegate::FastDelegate1<conf_node&, bool> conf_event;

/**
 * A conf_node subject to handle its events.
 */
class conf_subject
{    
    std::list<conf_listener*> m_list;
    std::list<conf_event> m_change_del;
    std::list<conf_event> m_nudge_del;
    
public:

    /**
     * Add an event to listen for changes.
     * @param sub The event to add.
     */
    void add_change_event (const conf_event& sub) {
	m_change_del.push_back(sub);
    }

    /**
     * Add an event to listen for nudges.
     * @param sub The event to add.
     */
    void add_nudge_event (const conf_event& sub) {
	m_nudge_del.push_back(sub);
    }

    /**
     * Add a listener to listen for events.
     * @param l The listener to add.
     */
    void add_listener (conf_listener* l) {
	m_list.push_back(l);
    }

    /**
     * Removes an event so it no longer listens for changes.
     * @param sub the event to remove.
     */
    void delete_change_event (const conf_event& sub) {
	m_change_del.remove(sub);
    }

    /**
     * Removes an event from the subject so it no longer listens for nudges.
     * @param sub the event to remove.
     */
    void delete_nudge_event (const conf_event& sub) {
	m_nudge_del.remove (sub);
    }

    /**
     * Removes a listener from the subject so it no longer listens for
     * events here.
     * @param l The listener to remove.
     */
    void delete_listener (conf_listener* l) {
	m_list.remove (l);
    }

    /**
     * Rises a nudge event.
     * @param source The source of the event.
     */
    void notify_conf_nudge (conf_node& source);

    /**
     * Rises a change event.
     * @param source The source of the event.
     */
    void notify_conf_change (conf_node& source);

    /**
     * Rises a new child creation event.
     * @param child The created child.
     */
    void notify_conf_new_child (conf_node& child);

    /**
     * Rises a new child creation event.
     * @param child The created child.
     */
    void notify_conf_remove_child (conf_node& child);
};
#endif

class conf_subject
{
public:
    typedef sigc::signal <void, conf_node&> signal;

    signal on_change;
    signal on_nudge;
    signal on_new_child;
    signal on_remove_child;
};

/**
 * A backend for a config system. It implements the actual phisical storing of
 * the data.
 */
class conf_backend
{
public:
    /** Virtual destructor */
    virtual ~conf_backend() {}

    /**
     * This is called when the backend has been attached to a node.
     * This allows the backend to start listening to its events in case it
     * provides live update.
     * @param node The node to which it has been attached.
     */
    virtual void attach (conf_node& node) = 0;

    /**
     * This is called when the backend has been detached from a node.
     * This allows the backend to stop listening to its events in case it
     * provides live update.
     * @param node The node to which it has been attached.
     */
    virtual void datach (conf_node& node) = 0;

    /**
     * Permanently store the data of the config.
     * @param node The config to store.
     */
    virtual void save (conf_node& node) = 0;

    /**
     * Load the data -and the hierarchy- in the phisical medium.
     * @param node Where to load the data.
     */
    virtual void load (conf_node& node) = 0;

    /**
     * Load the data -and the hierarchy- in the phisical medim, but using
     * @c def functions instead of @set so the old values al preserved
     * if the user has already set them.
     */
    virtual void def_load (conf_node& node) = 0;
};

/**
 * A configuration node. It stores a value of any type in @c conf_type. It has
 * some childs identifyed by its name crating a whole hierarchy that allows
 * a better organization of the values and the information.
 */
class conf_node : public conf_subject,
		  public tree_node <conf_node>
{
    class conf_element
    {
	union conf_data {
	    int* intptr;
	    float* floatptr;
	    std::string* stringptr;
	};

	conf_type m_type;
	conf_data m_data;
	bool m_unset;
    
	void destroy () {
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
	conf_element () :
	    m_type(CONF_NONE),
	    m_unset(true)
	    {}

	~conf_element () {
	    destroy ();
	}

	bool unset () {
	    return m_unset;
	}
    
	conf_type type () const {
	    return m_type;
	}
    
	void set (const float& val) {
	    to_type (CONF_FLOAT);
	    *m_data.floatptr = val;
	    m_unset = false;
	}

	void set (const int& val) {
	    to_type (CONF_INT);
	    *m_data.intptr = val;
	    m_unset = false;
	}

	void set (const std::string& val) {
	    to_type (CONF_STRING);
	    *m_data.stringptr = val;
	    m_unset = false;
	}

	void get (float& val) const {
	    if (m_type == CONF_FLOAT)
		val = *m_data.floatptr;
	}

	void get (int& val) const {
	    if (m_type == CONF_INT)
		val = *m_data.intptr;
	
	}

	void get (std::string& val) const {
	    if (m_type == CONF_STRING)
		val = *m_data.stringptr; 
	}

	void to_type (conf_type type) {
	    if (m_type != type) {
		destroy ();

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

    conf_element m_element;
    conf_backend* m_backend;

    void on_init () {
	m_backend = 0;
	if (get_parent ())
	    m_backend = get_parent ()->m_backend;
    }
    
public:
    /** Constructor */
    conf_node ()
	: m_backend(0)
	{}
    
    /** Destructor. */
    ~conf_node () {
	if (!get_parent () || get_parent ()->m_backend != m_backend)
	    delete m_backend;
    }
    
    /**
     * Sets the value of the node. The node type is set to @a T, unless
     * @a T is not a valid type in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void set (const T& val) {
	m_element.set (val);
	on_change (*this);
    }

    /**
     * Sets the value of the node if no previous value has been give to
     * the node. The node type is set to @a T, unless @a T is not a valid type
     * in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void def (const T& val) {
	if (m_element.unset ()) {
	    m_element.set (val);
	    on_change (*this);
	}
    }

    /**
     * Gets the value of the node. Make sure that @a T matches the node type.
     * @param data Somewhere to store the node value.
     */
    template<class T>
    void get (T& data) const {
	m_element.get (data);
    }

    /**
     * Returns the type of the node.
     */
    conf_type type () const {
	return m_element.type ();
    }

    /**
     * Changes the type of the node. After calling this the type is changed but
     * the node contains rubbish.
     */
    void set_type (conf_type type) {
	m_element.to_type (type);
    }

    /**
     * Changes the type of the node only if the node has not been set yet.
     */
    void def_type (conf_type type) {
	if (m_element.unset ())
	    m_element.to_type (type);
    }

    /**
     * Nudges the node. It just raises a nudge event.
     */
    void nudge () {
	on_nudge (*this);
    }

    /**
     * Permanently stores this node hierarchy using the attached backend.
     */
    void save () {
	if (m_backend)
	    m_backend->save (*this);
    }

    /**
     * Loads this node hierarchy from the permanent media.
     */
    void load () {
	if (m_backend)
	    m_backend->load (*this);
    }

    /**
     * Loads this node hierarchy from the permanent media but leaving already
     * set values with their old value.
     */
    void def_load () {
	if (m_backend)
	    m_backend->def_load (*this);
    }

    /**
     * Attaches a backend to the node. If another backend has already been
     * attached it is datached.
     * @param backend The backend to attach.
     */
    void attach_backend (conf_backend* backend) {
	datach_backend ();
	m_backend = backend;
	m_backend->attach (*this);
    }

    /**
     * Dataches the currently attached backend to the node.
     */
    void datach_backend () {
	if (m_backend) {
	    m_backend->datach (*this);
	    m_backend = NULL;
	}
    }
};

/**
 * Singleton @c conf_node to store the global config.
 */
class config : public conf_node,
	       public singleton <config>
{
    friend class singleton <config>;

    /** Hidden constructor. */
    config () {};

     /** Hidden destructor. */
    ~config () {};
public:
};

} /* namespace psynth */

#endif /* PSYNTH_CONFIG_H */
