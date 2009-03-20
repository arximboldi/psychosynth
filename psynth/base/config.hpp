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
#include <typeinfo>

#include <boost/any.hpp>

#include <psynth/base/signal.hpp>
#include <psynth/base/tree.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/base/singleton.hpp>
#include <psynth/base/exception.hpp>

namespace psynth
{

PSYNTH_ERROR_WHERE (base_error, conf_error, "psynth.base.conf");
PSYNTH_ERROR_WHAT  (conf_error, no_conf_backend_error,
		    "No configuration backend set on this config node.");
PSYNTH_ERROR_WHAT  (conf_error, conf_type_error,
		    "Type mismatch in configuration node type.");

class conf_node;

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
 *
 * @todo Rethink the backend system.
 */
class conf_node : public conf_subject,
		  public tree_node <conf_node>
{
public:
    /** Constructor */
    conf_node ()
	: m_backend (0)
    {}
    
    /** Destructor. */
    ~conf_node ()
    {
	if (!get_parent () || get_parent ()->m_backend != m_backend)
	    delete m_backend;
    }

    /**
     * Returns wether the node is empty or not.
     */
    bool empty () const
    {
	return m_element.empty ();
    }
    
    /**
     * Sets the value of the node. The node type is set to @a T, unless
     * @a T is not a valid type in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void set (const T& val)
    {
	m_element = val;
	on_change (*this);
    }

    /**
     * Sets the value of the node unless @a overwrite = false and the
     * node is already set.
     * @param val The new value of the node.
     */
    template<class T>
    void set (const T& val, bool overwrite)
    {
	if (overwrite || m_element.empty ()) {
	    m_element = val;
	    on_change (*this);
	}
    }
    
    /**
     * Sets the value of the node if no previous value has been give to
     * the node. The node type is set to @a T, unless @a T is not a valid type
     * in which case nothing happends.
     * @param val The new value of the node.
     */
    template<class T>
    void def (const T& val)
    {
	if (m_element.empty ()) {
	    m_element = val;
	    on_change (*this);
	}
    }

    /**
     * Gets the value of the node. Make sure that @a T matches the node type.
     * @param data Somewhere to store the node value.
     */
    template<class T>
    void get (T& data) const
    {
	try {
	    data = boost::any_cast<T> (m_element);
	} catch (boost::bad_any_cast&) {
	    throw conf_type_error ();
	}
    }
    
    /**
     * Returns the type of the node.
     */
    const std::type_info& type () const
    {
	return m_element.type ();
    }

    /**
     * Nudges the node. It just raises a nudge event.
     */
    void nudge ()
    {
	on_nudge (*this);
    }

    /**
     * Permanently stores this node hierarchy using the attached backend.
     */
    void save ()
    {
	if (m_backend)
	    m_backend->save (*this);
	else
	    throw no_conf_backend_error ();
    }

    /**
     * Loads this node hierarchy from the permanent media.
     */
    void load ()
    {
	if (m_backend)
	    m_backend->load (*this);
	else
	    throw no_conf_backend_error ();
    }

    /**
     * Loads this node hierarchy from the permanent media but leaving already
     * set values with their old value.
     */
    void def_load ()
    {
	if (m_backend)
	    m_backend->def_load (*this);
	else
	    throw no_conf_backend_error ();
    }

    /**
     * Attaches a backend to the node. If another backend has already been
     * attached it is datached.
     * @param backend The backend to attach.
     */
    void attach_backend (conf_backend* backend)
    {
	if (m_backend)
	    datach_backend ();
	m_backend = backend;
	m_backend->attach (*this);
    }

    /**
     * Dataches the currently attached backend to the node.
     */
    void datach_backend ()
    {
	if (m_backend) {
	    m_backend->datach (*this);
	    m_backend = NULL;
	} else
	    throw no_conf_backend_error ();
    }

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
class config : public conf_node,
	       public singleton <config>
{
    friend class singleton <config>;

    /** Hidden constructor. */
    config () {};

    /** Hidden destructor. */
    ~config () {};
};

} /* namespace psynth */

#endif /* PSYNTH_CONFIG_H */
