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

#ifndef PSYNTH_WORLD_H
#define PSYNTH_WORLD_H

#include <list>
#include <map>
#include <iostream>

#include <psynth/common/audio_info.hpp>
#include <psynth/world/patcher.hpp>
#include <psynth/node/node.hpp>
#include <psynth/node/node_output.hpp>
#include <psynth/node/node_audio_mixer.hpp>
#include <psynth/node/node_manager.hpp>
#include <psynth/node/node_factory_manager.hpp>

namespace psynth
{

class world;
class world_node_listener;

class world_node
{
    friend class world;
    
    node* m_nod; /* Use node_manager::Iterator instead? */
    world* m_world;

    world_node (node* nod, world* world) :
	m_nod (nod), m_world (world) {};

public:
    world_node ()
	: m_nod (NULL)
	, m_world (NULL)
	{}
    
    world_node(const world_node& o)
	: m_nod(o.m_nod)
	, m_world(o.m_world)
	{};

    bool is_null () const {
	return m_nod == NULL;
    };

    int get_id () const {
	return m_nod->get_id ();
    };
    
    int get_type () const {
	return m_nod->get_type ();
    };

    const std::string& get_name () {
	return m_nod->get_name ();
    };
    
    world* get_world() {
	return m_world;
    }
    
    inline void add_listener (world_node_listener* cl);

    inline void delete_listener (world_node_listener* cl);

    inline void delete_me ();
    
    inline void activate ();

    inline void deactivate ();

    template <typename T>
    inline void set_param (int id, const T& data);

    template <typename T>
    inline void set_param (const std::string& name, const T& data);

    void attach_watch (int type, int in_sock, watch* watch) {
	m_nod->attach_watch (type, in_sock, watch);
    }

    void detach_watch (int type, int in_sock, watch* watch) {
	m_nod->detach_watch (type, in_sock, watch);
    }
    
    int get_param_id (const std::string& name) const {
	return m_nod->param(name).get_id ();
    }

    const std::string& get_param_name (int id) const {
	return m_nod->param(id).get_name ();
    }
    
    int get_param_type (int id) const {
	return m_nod->param(id).type();
    }

    int get_param_type (const std::string& name) const {
	return m_nod->param(name).type();
    }
    
    template <typename T>
    void get_param (int id, T& data) const {
	m_nod->param(id).get(data);
    }

    template <typename T>
    void get_param (const std::string& name, T& data) const {
	m_nod->param(name).get(data);
    }

    bool operator== (const world_node& o) const {
	return m_nod == o.m_nod;
    }

    bool operator!= (const world_node& o) const {
	return m_nod != o.m_nod;
    }
};

class world_listener
{
public:
    virtual ~world_listener() {};
    virtual void handle_add_node (world_node& nod) = 0;
    virtual void handle_delete_node (world_node& nod) = 0;
};

class world_node_listener
{
public:
    virtual ~world_node_listener () {};
    virtual void handle_activate_node (world_node& nod) = 0;
    virtual void handle_deactivate_node (world_node& nod) = 0;
    virtual void handle_set_param_node (world_node& ob, int id) = 0;
};

struct world_patcher_event
{
    world_node src;
    world_node dest;
    int src_socket;
    int dest_socket;
    int socket_type;
    
    world_patcher_event (const world_node& s, const world_node& d,
			 int ss, int ds, int st) :
	src(s), dest(d), src_socket(ss), dest_socket(ds), socket_type(st) {}
};

class world_patcher_listener
{
public:
    virtual ~world_patcher_listener () {};
    virtual void handle_link_added (const world_patcher_event& ev) = 0;
    virtual void handle_link_deleted (const world_patcher_event& ev) = 0;
};

class world_subject
{
    typedef std::list<world_node_listener*>::iterator node_listener_iter;
    typedef std::list<world_listener*>::iterator listener_iter;
    
    std::list<world_listener*> m_listeners;
    std::list<world_patcher_listener*> m_patch_list;
    std::map<int, std::list<world_node_listener*> > m_nod_listeners; 
    std::list<world_node_listener*> m_all_nod_list;
    
protected:
    void notify_add_node (world_node& nod);
    void notify_delete_node (world_node& nod);
    void notify_activate_node (world_node& nod);
    void notify_deactivate_node (world_node& nod);
    void notify_set_param_node (world_node& nod, int param_id);
    void notify_link_added (const world_patcher_event& ev);
    void notify_link_deleted (const world_patcher_event& ev);
    
public:
    void add_world_listener (world_listener* cl) {
	m_listeners.push_back(cl);
    };

    void add_world_patcher_listener (world_patcher_listener* cl) {
	m_patch_list.push_back(cl);
    };

    void add_world_node_listener (world_node_listener* cl) {
	m_all_nod_list.push_back(cl);
    };
    
    void add_world_node_listener (world_node& nod, world_node_listener* cl) {
	std::map<int, std::list<world_node_listener*> >::iterator it;
	
	if ((it = m_nod_listeners.find(nod.get_id ())) != m_nod_listeners.end())
	    it->second.push_back(cl);
    };
    
    void delete_world_listener (world_listener* cl) {
	m_listeners.remove(cl);
    };

    void delete_world_patcher_listener (world_patcher_listener* cl) {
	m_patch_list.remove(cl);
    };

    void delete_world_node_listener (world_node_listener* cl) {
	m_all_nod_list.remove(cl);
    };
    
    void delete_world_node_listener (world_node& nod, world_node_listener* cl) {
	std::map<int, std::list<world_node_listener*> >::iterator it;
	
	if ((it = m_nod_listeners.find(nod.get_id())) != m_nod_listeners.end())
	    it->second.remove(cl);
    };
};

class world : public world_subject,
	      public patcher_listener
{    
    audio_info m_info;

    mgr_auto_ptr<patcher> m_patcher;
    
    node_manager m_node_mgr;
    node_output* m_output;
    node_audio_mixer* m_mixer;
    int m_last_id;
    node_factory_manager m_nodfact;
    
    static const int MIXER_CHANNELS = 16;

    void register_default_node_factory ();
    
public:
    enum {
	OUTPUT_ID = 0,
	MIXER_ID,
	MIN_USER_ID = 1024
    };
    
    world (const audio_info& info);
    ~world ();

    void clear ();
    
    const audio_info& get_info () const {
	return m_info;
    };

    void set_info (const audio_info& info) {
	if (m_info != info)
	    m_node_mgr.set_info (info);
    }

    void register_node_factory (node_factory& f) {
	m_nodfact.register_factory (f);
    }
    
    world_node find_node (int id);

    world_node add_node (int type);
    world_node add_node (const std::string& type_name);
    
    template <typename T>
    void set_param_node (world_node& nod, int id, const T& data) {
	nod.m_nod->param(id).set(data);
	notify_set_param_node (nod, id);
	if (m_patcher)
	    m_patcher->set_param_node (nod.m_nod, id);
    }

    template <typename T>
    void set_param_node (world_node& nod, const std::string& name, const T& data) {
	node_param& param = nod.m_nod->param(name);
	param.set (data);
	notify_set_param_node (nod, param.get_id());
	if (m_patcher)
	    m_patcher->set_param_node (nod.m_nod, param.get_id());
    }
    
    void delete_node (world_node& nod);

    void activate_node (world_node& nod);

    void deactivate_node (world_node& nod);

    void attach_output (output* out) {
	m_output->attach_output (out);
    };

    void attach_passive_output (output* out) {
	m_output->attach_passive_output (out);
    };
    
    void detach_output (output* out) {
	m_output->detach_output (out);
    };

    void detach_passive_output (output* out) {
	m_output->detach_passive_output (out);
    };

    void set_patcher (mgr_ptr<patcher> pat);

    void unset_patcher ();
    
    void update () {
	if (m_patcher)
	    m_patcher->update();
    }

    void handle_link_added (const patcher_event& ev) {
	notify_link_added (world_patcher_event (world_node(ev.src,this),
						world_node(ev.dest,this),
						ev.src_socket,
						ev.dest_socket,
						ev.socket_type));
    };
    
    void handle_link_deleted (const patcher_event& ev) {
	notify_link_deleted (world_patcher_event (world_node(ev.src,this),
						  world_node(ev.dest,this),
						  ev.src_socket,
						  ev.dest_socket,
						  ev.socket_type));
    }
};

template <typename T>
void world_node::set_param (int id, const T& data) {
    m_world->set_param_node (*this, id, data);
};

template <typename T>
void world_node::set_param (const std::string& name, const T& data) {
    m_world->set_param_node (*this, name, data);
};

void world_node::add_listener (world_node_listener* cl) {
    m_world->add_world_node_listener (*this, cl);
}

void world_node::delete_listener (world_node_listener* cl) {
    m_world->delete_world_node_listener (*this, cl);
}

void world_node::activate () {
    m_world->activate_node (*this);
}

void world_node::delete_me () {
    m_world->delete_node (*this);
}

void world_node::deactivate () {
    m_world->deactivate_node (*this);
}

} /* namespace psynth */

#endif /* PSYNTH_WORLD_H */
