/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef PSYNTH_WORLD_NODE_CREATOR_H
#define PSYNTH_WORLD_NODE_CREATOR_H

#include <list>
#include <string>
#include <iostream>
#include <libpsynth/world/world.h>

namespace psynth
{

class node_factory_manager;
class audio_info;
class node;

class world_node_creator
{
    class param_maker_base
    {
    public:
	virtual ~param_maker_base () {}
	virtual void apply (world_node& obj, const std::string& par) = 0;
	virtual param_maker_base* clone () const = 0;
    };

    template <class T>
    class param_maker : public param_maker_base
    {
	T m_val;
	
    public:
	param_maker (const T& val) :
	    m_val(val) {}
	    
	void apply (world_node & obj, const std::string& par) {
	    obj.set_param (par, m_val);
	}

	void set (const T& val) {
	    m_val = val;
	}
	
	param_maker_base* clone () const {
	    return new param_maker <T> (m_val);
	}
    };

    typedef std::list<std::pair<std::string, param_maker_base*> > param_maker_list;

    param_maker_list m_param;
    std::string m_name;

    void destroy ();
    void copy (const world_node_creator& obj);
    param_maker_base* find (const std::string& name);
    
public:
    world_node_creator () {}

    world_node_creator (const world_node_creator& obj) {
	copy (obj);
    }
    
    ~world_node_creator () {
	destroy ();
    };

    world_node_creator& operator= (const world_node_creator& obj) {
	if (this != &obj) {
	    destroy ();
	    copy (obj);
	}
	return *this;
    }
    
    world_node create (world& table);

    void set_name (const std::string& name) {
	m_name = name;
    }
    
    template<class T>
    void add_param (const std::string& name, const T& value) {
	m_param.push_back (std::make_pair (std::string(name),
					   (param_maker_base*)
					   new param_maker<T>(value)));
    }

    template<class T>
    void set_param (const std::string& name, const T& value) {
	param_maker<T>* param = (param_maker<T>*) find(name);
	if (param) param->set(value);
	else add_param (name, value);
    };

    void clear () {
	destroy ();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTCREATOR_H */
