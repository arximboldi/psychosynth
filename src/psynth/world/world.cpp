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

#include "world/world.hpp"

#include "graph/node_output.hpp"
#include "graph/node_mixer.hpp"
#include "graph/node_oscillator.hpp"
#include "graph/node_filter.hpp"
#include "graph/node_sampler.hpp"
#include "graph/node_step_seq.hpp"
#include "graph/node_noise.hpp"
#include "graph/node_echo.hpp"
#include "graph/node_delay.hpp"
#include "graph/node_double_sampler.hpp"

using namespace std;

namespace psynth
{

void world_subject::notify_add_node (world_node& nod)
{
    m_nod_listeners [nod.get_id()];
    for (listener_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handle_add_node (nod);
}

void world_subject::notify_delete_node (world_node& nod)
{
    m_nod_listeners.erase (nod.get_id());
    for (listener_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handle_delete_node (nod);
}
    
void world_subject::notify_activate_node (world_node& nod)
{
    for (node_listener_iter i = m_all_nod_list.begin(); i != m_all_nod_list.end(); i++)
	(*i)->handle_activate_node (nod);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_nod_listeners.find (nod.get_id ())) != m_nod_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_activate_node (nod);
    }
}
    
void world_subject::notify_deactivate_node (world_node& nod)
{
    for (node_listener_iter i = m_all_nod_list.begin(); i != m_all_nod_list.end(); i++)
	(*i)->handle_deactivate_node (nod);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_nod_listeners.find(nod.get_id ())) != m_nod_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_deactivate_node (nod);
    }
}

void world_subject::notify_set_param_node (world_node& nod, int param_id)
{
    for (node_listener_iter i = m_all_nod_list.begin(); i != m_all_nod_list.end(); i++)
	(*i)->handle_set_param_node (nod, param_id);
    
    std::map<int, std::list<world_node_listener*> >::iterator it;
    if ((it = m_nod_listeners.find (nod.get_id ())) != m_nod_listeners.end()) {
	for (node_listener_iter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handle_set_param_node (nod, param_id);	
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
    m_output = new graph::node_output (m_info);
    m_mixer = new graph::node_audio_mixer (m_info, MIXER_CHANNELS);

    m_mixer->param("amplitude").set(0.5f);

    m_node_mgr.add_node (base::manage (m_output), OUTPUT_ID);
    m_node_mgr.add_node (base::manage (m_mixer), MIXER_ID);

    register_default_node_factory ();
}

void world::register_default_node_factory ()
{
    register_node_factory (graph::get_node_audio_mixer_factory());
    register_node_factory (graph::get_node_control_mixer_factory());
    register_node_factory (graph::get_node_audio_oscillator_factory());
    register_node_factory (graph::get_node_lfo_factory());
    register_node_factory (graph::get_node_output_factory());
    register_node_factory (graph::get_node_filter_factory());
    register_node_factory (graph::get_node_sampler_factory());
    register_node_factory (graph::get_node_step_seq_factory());
    register_node_factory (graph::get_node_audio_noise_factory());
    register_node_factory (graph::get_node_control_noise_factory());
    register_node_factory (graph::get_node_echo_factory());
    register_node_factory (graph::get_node_delay_factory());
    //register_node_factory (graph::get_node_double_sampler_factory());
}

world::~world ()
{
}

void
world::clear ()
{
    for (auto it = m_node_mgr.begin();
	 it != m_node_mgr.end();)
	if ((*it)->get_id () >= MIN_USER_ID) {
	    if (m_patcher)
		m_patcher->delete_node (*it);
	    world_node nod(*it, this);
	    notify_delete_node (nod);
	    //m_node_mgr.remove(it++);
	    m_node_mgr.delete_node (it);
	} else
	    ++it;
}

world_node world::find_node (int id)
{
    graph::node_manager::iterator i = m_node_mgr.find (id);
    if (i == m_node_mgr.end ())
	return world_node(NULL, NULL);
    else
	return world_node(*i, this);
}

#if 0
world_node world::add_node (int type)
{
    graph::node* nod;

    switch (type) {
    case NOD_OSCILLATOR:
	nod = new graph::node_audio_oscillator(m_info);
	break;
    case NOD_LFO:
	nod = new graph::node_lfo(m_info);
	break;
    case NOD_FILTER:
	nod = new graph::node_filter(m_info);
	break;
    case NOD_MIXER:
	nod = new graph::node_audio_mixer(m_info);
	break;
    case NOD_CONTROLMIXER:
	nod = new graph::node_control_mixer (m_info);
	break;
    case NOD_SAMPLER:
	nod = new graph::node_sampler(m_info);
	break;
    default:
	nod = NULL;
	return world_node(NULL, NULL);
    }
    
    if (!m_node_mgr.attachNode(nod, m_last_id++))
	return world_node(NULL, NULL);

    world_node tnod(nod, this);
    notifyAddNode(tnod);
    return tnod;
}
#endif

world_node world::add_node (const std::string& name)
{
    graph::node* nod;
    world_node tnod;
	
    nod = m_nodfact.create (name, m_info);

    if (nod) {
	if (!m_node_mgr.add_node (base::manage (nod), m_last_id++))
	    return world_node (0, 0);
    
	tnod = world_node (nod, this);
	notify_add_node (tnod);
    }
    
    return tnod;
}

void world::delete_node (world_node& nod)
{
    if (m_patcher)
	m_patcher->delete_node (nod.m_nod);

    notify_delete_node (nod);

    m_node_mgr.delete_node (nod.m_nod->get_id ());
}

void world::activate_node (world_node& nod)
{
    /* HACK */
    nod.m_nod->update_params_in ();
     
    if (m_patcher)
	m_patcher->add_node (nod.m_nod);
    notify_activate_node (nod);
}

void world::deactivate_node (world_node& nod)
{
    if (m_patcher)
	m_patcher->delete_node (nod.m_nod);
    notify_deactivate_node (nod);
}

void world::set_patcher (base::mgr_ptr<patcher> pat)
{
    unset_patcher ();
    m_patcher = pat;
    pat->add_listener (this);
    for (auto it = m_node_mgr.begin();
	 it != m_node_mgr.end();
	 ++it)
	m_patcher->add_node (*it);
    m_patcher->update ();
}

void world::unset_patcher ()
{
    if (m_patcher) {
	m_patcher->delete_listener (this);
	m_patcher->clear ();
	m_patcher = 0;
    }
}

} /* namespace psynth */
