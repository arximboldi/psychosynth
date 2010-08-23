/**
 *  Time-stamp:  <2010-08-23 11:41:54 raskolnikov>
 *
 *  @file        config.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Configuration system interface. A hierarchical, multi-backend and
 *  thread-safe configuration system.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_CONFIG_H
#define PSYNTH_CONFIG_H

#include <list>
#include <map>
#include <iostream>
#include <typeinfo>

#include <boost/any.hpp>

#include <psynth/base/tree.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/base/singleton.hpp>
#include <psynth/base/exception.hpp>
#include <psynth/base/observer.hpp>

namespace psynth
{

PSYNTH_DECLARE_ERROR (base_error, config_error);
PSYNTH_DECLARE_ERROR (config_error, config_type_error);
PSYNTH_DECLARE_ERROR (config_error, config_backend_error);

class conf_node;

/**
 * A configuration node listener to aid you when you are interested
 * in all the signals emitted by a @a conf_subject.
 */
class conf_listener : public listener_base
{
public:
    /**
     * Invoked on a change event.
     * @param c The source config.
     */
    virtual void handle_conf_change (conf_node& c)    = 0;

    /**
     * Invoked on a nudge event.
     * @param c The source config.
     */
    virtual void handle_conf_nudge (conf_node& c)     = 0;

    /**
     * Invoked on a new child event.
     * @param c The source config.
     */
    virtual void handle_conf_add_child (conf_node& c) = 0;

    /**
     * Invoked on a delete child event.
     * @param c The source config.
     */
    virtual void handle_conf_del_child (conf_node& c) = 0;
};

/**
 * The base class of an observable configuration node.
 */
class conf_subject : public subject_base <conf_listener>
{
public:
    typedef sigc::signal <void, conf_node&> signal;

    /** Event emitted whenever the node value is changed. */
    signal on_change;

    /**
     * Generic event to notify changes on the node subchilds.
     * It should be emitted by the user with the nudge () method.
     */
    signal on_nudge;

    /** Generic event to notify the addition of a child to the config node. */
    signal on_add_child;

    /** Generic event to notify the removal of a child to the config node. */
    signal on_del_child;

    /**
     * Register a new listener in the subject connecting all the signals.
     */
    void add_listener (conf_listener& l);

    /**
     * Unregister a new listener on this object disconnectin all the signals.
     */
    void del_listener (conf_listener& l);
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
 *
 * @note This implementation is thread safe.
 * @todo Rethink the backend system.
 * @todo Template threading model.
 */
class conf_node : public conf_subject,
		  public tree_node <conf_node>,
		  public PSYNTH_DEFAULT_THREADING <conf_node>
{
public:
    /** Constructor */
    conf_node ()
	: m_backend (0)
    {}
    
    /** Destructor. */
    ~conf_node ()
    {
	if (!get_parent () ||
	    get_parent ()->m_backend != m_backend)
	    delete m_backend;
    }

    /**
     * Returns the type of the node.
     */
    const std::type_info& type () const
    {
	lock lock (this);
	return m_element.type ();
    }

    /**
     * Returns wether the node is empty or not.
     */
    bool empty () const
    {
	lock lock (this);
	return m_element.empty ();
    }

    /**
     * Nudges the node. It just raises a nudge event.
     */
    void nudge ()
    {
	on_nudge (*this);
    }
    
    /**
     * Sets the value of the node. The node type is set to @a T, unless
     * @a T is not a valid type in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void set (const T& val)
    {
	lock lock (this);
	m_element = val;
	on_change (*this);
    }

    /**
     * Sets the value of the node unless @a overwrite = false and the
     * node is already set.
     * @param val The new value of the node.
     */
    template<class T>
    void set (const T& val, bool overwrite);
    
    /**
     * Sets the value of the node if no previous value has been give to
     * the node. The node type is set to @a T, unless @a T is not a valid type
     * in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void def (const T& val);
    
    /**
     * Gets the value of the node. Make sure that @a T matches the node type.
     * @param data Somewhere to store the node value.
     */
    template<class T>
    void get (T& data) const;

    template<class T>
    T get () const;
    
  
    /**
     * Permanently stores this node hierarchy using the attached backend.
     */
    void save ();

    /**
     * Loads this node hierarchy from the permanent media.
     */
    void load ();

    /**
     * Loads this node hierarchy from the permanent media but leaving already
     * set values with their old value.
     */
    void def_load ();
    
    /**
     * Attaches a backend to the node. If another backend has already been
     * attached it is datached.
     * @param backend The backend to attach.
     */
    void attach_backend (conf_backend* backend);

    /**
     * Dataches the currently attached backend to the node.
     */
    void datach_backend ();
    
private:
    boost::any m_element;
    conf_backend* m_backend;

    void on_init ()
    {
	m_backend = 0;
	if (get_parent ())
	    m_backend = get_parent ()->m_backend;
    }

    void on_uninit ()
    {
	m_backend = 0;
    }
};

/**
 * Singleton @c conf_node to store the global config.
 */
typedef singleton_holder <conf_node> config;

template<class T>
void conf_node::get (T& data) const
{
    lock lock (this);
    
    try {
	data = boost::any_cast<T> (m_element);
    } catch (boost::bad_any_cast&) {
	throw config_type_error ();
    }
}

template<class T>
T conf_node::get () const
{
    lock lock (this);
    
    try {
	return boost::any_cast<T> (m_element);
    } catch (boost::bad_any_cast&) {
	throw config_type_error ();
    }
}

template<class T>
void conf_node::def (const T& val)
{
    lock lock (this);
    
    if (m_element.empty ()) {
	m_element = val;
	on_change (*this);
    }
}

template<class T>
void conf_node::set (const T& val, bool overwrite)
{
    lock lock (this);
    
    if (overwrite || m_element.empty ()) {
	m_element = val;
	on_change (*this);
    }
}

} /* namespace psynth */

#endif /* PSYNTH_CONFIG_H */
