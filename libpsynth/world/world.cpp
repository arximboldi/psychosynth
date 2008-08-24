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

#include <algorithm>

#include "world/world.h"

#include "node/node_output.h"
#include "node/node_audio_mixer.h"
#include "node/node_control_mixer.h"
#include "node/node_audio_oscillator.h"
#include "node/node_lfo.h"
#include "node/node_filter.h"
#include "node/node_sampler.h"
#include "node/node_step_seq.h"
#include "node/node_audio_noise.h"
#include "node/node_control_noise.h"
#include "node/node_echo.h"
#include "node/node_delay.h"

using namespace std;

namespace psynth
{

void world_subject::notify_add_node (world_node& obj)
{
    m_obj_listeners [obj.get_id()];
    for (listener_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handle_add_node (obj);
}

void world_subject::notify_delete_node (world_node& obj)
{
    m_obj_listeners.erase (obj.get_id());
    for (listener_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handle_delete_node (obj);
}
    
void world_subject::notify_activate_node (world_node& obj)
{
    for (node_listener_iter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handle_activate_node (obj);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_obj_listeners.find (obj.get_id ())) != m_obj_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_activate_node (obj);
    }
}
    
void world_subject::notify_deactivate_node (world_node& obj)
{
    for (node_listener_iter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handle_deactivate_node (obj);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_obj_listeners.find(obj.get_id ())) != m_obj_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_deactivate_node (obj);
    }
}

void world_subject::notify_set_param_node (world_node& obj, int param_id)
{
    for (node_listener_iter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handle_set_param_node (obj, param_id);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_obj_listeners.find (obj.get_id ())) != m_obj_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_set_param_node (obj, param_id);	
    }
}

void world_subject::notify_link_added (const world_patcher_event& ev)
{
    for (list<world_patcher_listener*>::iterator i = m_patch_list.begin();
	 i != m_patch_list.end();)
	(*i++)->handle_link_added (ev);
}

void world_subject::notify_link_deleted (const world_patcher_event& ev)
{
    for (list<world_patcher_listener*>::iterator i = m_patch_list.begin();
	 i != m_patch_list.end();)
	(*i++)->handle_link_deleted (ev);
}

world::world (const audio_info& info)
    : m_info (info)
    , m_patcher (0)
    , m_last_id (MIN_USER_ID)
{
    m_output = new node_output (m_info);
    m_mixer = new node_audio_mixer (m_info, MIXER_CHANNELS);

    m_mixer->param("amplitude").set(0.5f);

    m_node_mgr.attach_node (m_output, OUTPUT_ID);
    m_node_mgr.attach_node (m_mixer, MIXER_ID);

    register_default_node_factory ();
}

void world::register_default_node_factory ()
{
    register_node_factory (get_node_audio_mixer_factory());
    register_node_factory (get_node_control_mixer_factory());
    register_node_factory (get_node_audio_oscillator_factory());
    register_node_factory (get_node_lfo_factory());
    register_node_factory (get_node_output_factory());
    register_node_factory (get_node_filter_factory());
    register_node_factory (get_node_sampler_factory());
    register_node_factory (get_node_step_seq_factory());
    register_node_factory (get_node_audio_noise_factory());
    register_node_factory (get_node_control_noise_factory());
    register_node_factory (get_node_echo_factory());
    register_node_factory (get_node_delay_factory());
}

world::~world ()
{
    delete m_patcher;
}

void
world::clear ()
{
    for (node_manager::iterator it = m_node_mgr.begin();
	 it != m_node_mgr.end();)
	if ((*it)->get_id () >= MIN_USER_ID) {
	    if (m_patcher)
		m_patcher->delete_node (*it);
	    world_node obj(*it, this);
	    notify_delete_node (obj);
	    //m_node_mgr.remove(it++);
	    m_node_mgr.delete_node (it);
	} else
	    ++it;
}

world_node world::find_node (int id)
{
    node_manager::iterator i = m_node_mgr.find (id);
    if (i == m_node_mgr.end ())
	return world_node(NULL, NULL);
    else
	return world_node(*i, this);
}

#if 0
world_node world::add_node (int type)
{
    node* nod;

    switch (type) {
    case OBJ_OSCILLATOR:
	obj = new node_audio_oscillator(m_info);
	break;
    case OBJ_LFO:
	obj = new node_lfo(m_info);
	break;
    case OBJ_FILTER:
	obj = new node_filter(m_info);
	break;
    case OBJ_MIXER:
	obj = new node_audio_mixer(m_info);
	break;
    case OBJ_CONTROLMIXER:
	obj = new node_control_mixer (m_info);
	break;
    case OBJ_SAMPLER:
	obj = new node_sampler(m_info);
	break;
    default:
	obj = NULL;
	return world_node(NULL, NULL);
    }
    
    if (!m_node_mgr.attachNode(obj, m_last_id++))
	return world_node(NULL, NULL);

    world_node tobj(obj, this);
    notifyAddNode(tobj);
    return tobj;
}
#endif

world_node world::add_node (const std::string& name)
{
    node* obj;
    world_node tobj;
	
    obj = m_objfact.create (name, m_info);

    if (obj) {
	if (!m_node_mgr.attach_node (obj, m_last_id++))
	    return world_node (NULL, NULL);
    
	tobj = world_node (obj, this);
	notify_add_node (tobj);
    }
    
    return tobj;
}

void world::delete_node (world_node& obj)
{
    if (m_patcher)
	m_patcher->delete_node (obj.m_obj);
    notify_delete_node (obj);
/*
  m_node_mgr.detachNode(obj.m_obj->get_id ());
  delete obj.m_obj;
*/
    m_node_mgr.delete_node (obj.m_obj->get_id ());
}

void world::activate_node (world_node& obj)
{
    /* HACK */
    obj.m_obj->update_params_in ();
     
    if (m_patcher)
	m_patcher->add_node (obj.m_obj);
    notify_activate_node (obj);
}

void world::deactivate_node (world_node& obj)
{
    if (m_patcher)
	m_patcher->delete_node (obj.m_obj);
    notify_deactivate_node (obj);
}

void world::attach_patcher (patcher* pat)
{
    detach_patcher ();
    m_patcher = pat;
    pat->add_listener (this);
    for (node_manager::iterator it = m_node_mgr.begin(); it != m_node_mgr.end(); ++it)
	m_patcher->add_node (*it);
    m_patcher->update ();
}

void world::detach_patcher ()
{
    if (m_patcher) {
	m_patcher->delete_listener (this);
	m_patcher->clear ();
	m_patcher = NULL;
    }
}

} /* namespace psynth */
